#pragma once

#include "SvtJXScodec.h"

#pragma comment(lib, "vfw32.lib")

int main()
{
    RECT frame = { 0, 0, 1024, 768 };
    int nframes = 90;

    const CHAR* filename = "SvtJXS_VfW.avi";

    HIC                 hIC;
    BOOL                res;

    res = ICInstall(ICTYPE_VIDEO, mmioFOURCC('j', 'x', 's', ' '),
        (LPARAM)(FARPROC)&DriverProc, NULL, ICINSTALL_FUNCTION);

    hIC = ICOpen(ICTYPE_VIDEO, mmioFOURCC('j', 'x', 's', ' '), ICMODE_COMPRESS);

    AVIFileInit();

    IAVIFile* pFile;
    AVIFileOpen(&pFile, filename, OF_DELETE, NULL);
    if (AVIFileOpen(&pFile, filename, OF_CREATE | OF_WRITE, NULL) != 0)
    {
        printf("AVIFileOpen failed\n");
        return 1;
    }

    AVISTREAMINFO si = { 0 };
    si.fccType = streamtypeVIDEO;
    si.fccHandler = mmioFOURCC('j', 'x', 's', ' ');
    si.dwScale = 1;
    si.dwRate = 15;
    si.dwQuality = (DWORD)-1;
    si.rcFrame = frame;
    IAVIStream* pStream;
    if (AVIFileCreateStream(pFile, &pStream, &si) != 0)
    {
        printf("AVIFileCreateStream failed\n");
        return 1;
    }

    AVICOMPRESSOPTIONS co = { 0 };
    co.fccType = si.fccType;
    co.fccHandler = si.fccHandler;
    co.dwQuality = si.dwQuality;
    IAVIStream* pCompressedStream;
    if (AVIMakeCompressedStream(&pCompressedStream, pStream, &co, NULL) != 0)
    {
        printf("AVIMakeCompressedStream failed\n");
        return 1;
    }

    LPBITMAPINFOHEADER    lpbiIn, lpbiOut;
    size_t biSize = sizeof(BITMAPINFOHEADER) + 0 * sizeof(RGBQUAD);
    lpbiIn = (BITMAPINFOHEADER*)malloc(biSize);
    lpbiOut = (BITMAPINFOHEADER*)malloc(biSize);
    ZeroMemory(lpbiIn, biSize);
    ZeroMemory(lpbiOut, biSize);
    lpbiIn->biSize = (DWORD)biSize;
    lpbiIn->biWidth = si.rcFrame.right;
    lpbiIn->biHeight = si.rcFrame.bottom;
    lpbiIn->biPlanes = 1;
    lpbiIn->biCompression = BI_RGB;        // standard RGB bitmap for input 
    lpbiIn->biBitCount = 24;  // 8 bits-per-pixel format 
    lpbiIn->biSizeImage = lpbiIn->biWidth * lpbiIn->biHeight * 3;
    lpbiIn->biClrUsed = 1;
    lpbiIn->biClrImportant = 0;

    LONG lFormatSize = ICCompressGetFormatSize(hIC, lpbiIn);
    HGLOBAL hbiOut = GlobalAlloc(GHND, lFormatSize);
    lpbiOut = (LPBITMAPINFOHEADER)GlobalLock(hbiOut);
    ICCompressGetFormat(hIC, lpbiIn, lpbiOut);
    LRESULT lres = ICCompressQuery(hIC, (DWORD_PTR)lpbiIn, NULL);

    ICINFO ICInfo;
    ICGetInfo(hIC, &ICInfo, sizeof(ICInfo));

    DWORD dwKeyFrameRate, dwQuality;
    dwKeyFrameRate = ICGetDefaultKeyFrameRate(hIC);
    dwQuality = ICGetDefaultQuality(hIC);

    LPVOID lpOutput = 0;
    DWORD dwCompressBufferSize = 0;
    HGLOBAL hpOutput = NULL;
    if (ICCompressQuery(hIC, lpbiIn, lpbiOut) == ICERR_OK)
    {
        // Find the worst-case buffer size. 
        dwCompressBufferSize = ICCompressGetSize(hIC, lpbiIn, lpbiOut);

        // Allocate a buffer and get lpOutput to point to it. 
        hpOutput = GlobalAlloc(GHND, dwCompressBufferSize);
        lpOutput = (LPVOID)GlobalLock(hpOutput);
    }

    DWORD dwCkID;
    DWORD dwCompFlags = AVIIF_KEYFRAME;
    LONG  lNumFrames = 30, lFrameNum = 0;
    LONG lSamplesWritten = 0;
    LONG lBytesWritten = 0;

    if (AVIStreamSetFormat(pCompressedStream, 0, lpbiIn, (DWORD)biSize) != 0)
    {
        printf("AVIStreamSetFormat failed\n");
        return 1;
    }

    unsigned char* bits = (unsigned char*)malloc(lpbiIn->biSizeImage);
    LPVOID lpInput = (LPVOID)bits;
    HRESULT hr;
    for (int f = 0; f < nframes; f++)
    {
        for (int iy = 0; iy < lpbiIn->biHeight; ++iy)
        {
            for (int ix = 0; ix < lpbiIn->biWidth; ++ix)
            {
                bits[3 * (iy * lpbiIn->biWidth + ix) + 0] = (f + 16 + iy * 480 / lpbiIn->biHeight + ix * 480 / lpbiIn->biWidth) % 256;
                bits[3 * (iy * lpbiIn->biWidth + ix) + 1] = 128;
                bits[3 * (iy * lpbiIn->biWidth + ix) + 2] = 128;
            }
        }
        //bits[0] = f;
        ICCompress(hIC, 0, lpbiOut, lpOutput, lpbiIn, lpInput,
            &dwCkID, &dwCompFlags, f, lpbiIn->biSizeImage, dwQuality, NULL, NULL);
        hr = AVIStreamWrite(pStream, f, 1, lpOutput, lpbiOut->biSizeImage,
            AVIIF_KEYFRAME, &lSamplesWritten, &lBytesWritten);
        if (hr != S_OK)
        {
            printf("AVIStreamWrite failed\n");
            return 1;
        }
    }

    ICRemove(ICTYPE_VIDEO, mmioFOURCC('j', 'x', 's', ' '), NULL);

    GlobalUnlock(hbiOut);
    GlobalFree(hbiOut);
    GlobalUnlock(hpOutput);
    GlobalFree(hpOutput);
    if (AVIStreamRelease(pCompressedStream) != 0 || AVIStreamRelease(pStream) != 0)
    {
        printf("AVIStreamRelease failed\n");
        return 1;
    }
    if (AVIFileRelease(pFile) != 0)
    {
        printf("AVIFileRelease failed\n");
        return 1;
    }
    AVIFileExit();
    printf("Succeeded\n");
    return 0;
}