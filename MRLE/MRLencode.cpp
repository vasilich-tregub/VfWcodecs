// VfWcodecs.cpp : Defines the entry point for the application.
//

// based on learn.microsoft.com articles on Using the Video Compression Manager 
// and SO post https://stackoverflow.com/questions/22765194/is-it-possible-to-encode-using-the-mrle-codec-on-video-for-windows-under-windows
// also see 
// https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapinfoheader
// why bmi size should be augmented by the color table size
// `However, some legacy components might assume that a color table is present. Therefore, if you are allocating 
// a BITMAPINFOHEADER structure, it is recommended to allocate space for a color table when the bit depth is 8 bpp or less, 
// even if the color table is not used.`
//

#include <Windows.h>
#include <vfw.h>
#include <stdlib.h>
#include <iostream>

#pragma comment(lib, "vfw32.lib")

int main()
{
    RECT frame = { 0, 0, 64, 8 };
    int nframes = 10;

    const CHAR* filename = "rlenc.avi";

    HIC hIC = ICOpen(ICTYPE_VIDEO, mmioFOURCC('M', 'R', 'L', 'E'), ICMODE_COMPRESS);

    AVIFileInit();

    IAVIFile* pFile;
    AVIFileOpen(&pFile, filename, OF_DELETE, NULL);
    if (AVIFileOpen(&pFile, filename, OF_CREATE | OF_WRITE, NULL) != 0)
    {
        std::cout << "AVIFileOpen failed" << std::endl;
        return 1;
    }

    AVISTREAMINFO si = { 0 };
    si.fccType = streamtypeVIDEO;
    si.fccHandler = mmioFOURCC('M', 'R', 'L', 'E');
    si.dwScale = 1;
    si.dwRate = 15;
    si.dwQuality = (DWORD)-1;
    si.rcFrame = frame;
    IAVIStream* pStream;
    if (AVIFileCreateStream(pFile, &pStream, &si) != 0)
    {
        std::cout << "AVIFileCreateStream failed" << std::endl;
        return 1;
    }

    AVICOMPRESSOPTIONS co = { 0 };
    co.fccType = si.fccType;
    co.fccHandler = si.fccHandler;
    co.dwQuality = si.dwQuality;
    IAVIStream* pCompressedStream;
    if (AVIMakeCompressedStream(&pCompressedStream, pStream, &co, NULL) != 0)
    {
        std::cout << "AVIMakeCompressedStream failed" << std::endl;
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
    lpbiIn->biBitCount = 8;  // 8 bits-per-pixel format 
    lpbiIn->biSizeImage = lpbiIn->biWidth * lpbiIn->biHeight;
    lpbiIn->biClrUsed = 0;
    lpbiIn->biClrImportant = 0;

    LONG lFormatSize = ICCompressGetFormatSize(hIC, lpbiIn);
    HGLOBAL hbiOut = GlobalAlloc(GHND, lFormatSize);
    lpbiOut = (LPBITMAPINFOHEADER)GlobalLock(hbiOut);
    ICCompressGetFormat(hIC, lpbiIn, lpbiOut);

    /*ICINFO ICInfo;
    ICGetInfo(hIC, &ICInfo, sizeof(ICInfo));

    DWORD dwKeyFrameRate;
    dwKeyFrameRate = ICGetDefaultKeyFrameRate(hIC);*/
    DWORD dwQuality = ICGetDefaultQuality(hIC);

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
    LONG  lNumFrames = 15, lFrameNum = 0;
    LONG lSamplesWritten = 0;
    LONG lBytesWritten = 0;

    if (AVIStreamSetFormat(pCompressedStream, 0, lpbiIn, (DWORD)biSize) != 0)
    {
        std::cout << "AVIStreamSetFormat failed" << std::endl;
        return 1;
    }

    unsigned char* bits = (unsigned char*)malloc(lpbiIn->biSizeImage);
    LPVOID lpInput = (LPVOID)bits;
    HRESULT hr;
    for (int frame = 0; frame < nframes; frame++)
    {
        for (int i = 0; i < (int)lpbiIn->biSizeImage; ++i)
            bits[i] = (frame + 1) * ((i + 5) / 5);
        ICCompress(hIC, 0, lpbiOut, lpOutput, lpbiIn, lpInput,
            &dwCkID, &dwCompFlags, frame, lpbiIn->biSizeImage, dwQuality, NULL, NULL);
        hr = AVIStreamWrite(pStream, frame, 1, lpOutput, lpbiOut->biSizeImage,
            AVIIF_KEYFRAME, &lSamplesWritten, &lBytesWritten);
        if (hr != S_OK)
        {
            std::cout << "AVIStreamWrite failed" << std::endl;
            return 1;
        }
    }
    free(bits);

    GlobalUnlock(hbiOut);
    GlobalFree(hbiOut);
    GlobalUnlock(hpOutput);
    GlobalFree(hpOutput);
    if (AVIStreamRelease(pCompressedStream) != 0 || AVIStreamRelease(pStream) != 0)
    {
        std::cout << "AVIStreamRelease failed" << std::endl;
        return 1;
    }
    if (AVIFileRelease(pFile) != 0)
    {
        std::cout << "AVIFileRelease failed" << std::endl;
        return 1;
    }

    std::cout << "Succeeded" << std::endl;
    return 0;
}