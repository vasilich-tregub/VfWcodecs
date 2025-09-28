#pragma once

#include "SvtJXScodec.h"

#pragma comment(lib, "vfw32.lib")

int main()
{
    const CHAR* fileread = "sunset_3s.avi";
    const CHAR* filewrite = "sunset_SvtJXS_VfW.avi";

    HIC                 hIC;
    BOOL                res;
    HRESULT             hr;

    AVIFileInit();

    IAVIFile* pFileRead;
    if (AVIFileOpen(&pFileRead, fileread, OF_SHARE_DENY_WRITE, NULL) != AVIERR_OK)
    {
        //an error occures
        if (pFileRead != NULL)
            AVIFileRelease(pFileRead);

        return FALSE;
    }

    IAVIFile* pFileWrite;
    AVIFileOpen(&pFileWrite, filewrite, OF_DELETE, NULL);
    if (AVIFileOpen(&pFileWrite, filewrite, OF_CREATE | OF_WRITE, NULL) != AVIERR_OK)
    {
        printf("AVIFileOpen failed\n");
        return 1;
    }

    AVIFILEINFO fileread_info;
    AVIFileInfo(pFileRead, &fileread_info, sizeof(AVIFILEINFO));

    printf("Dimension: %dx%d\n"
        "Length: %d frames\n"
        "Max bytes per second: %d\n"
        "Samples per second: %d\n"
        "Streams: %d\n"
        "File Type: %d", fileread_info.dwWidth,
        fileread_info.dwHeight,
        fileread_info.dwLength,
        fileread_info.dwMaxBytesPerSec,
        (DWORD)(fileread_info.dwRate / fileread_info.dwScale),
        fileread_info.dwStreams,
        fileread_info.szFileType);

    RECT rect = { 0, 0, fileread_info.dwWidth, fileread_info.dwHeight };

    PAVISTREAM pStreamRead;
    if (AVIFileGetStream(pFileRead, &pStreamRead, streamtypeVIDEO, 0) != AVIERR_OK)
    {
        if (pStreamRead != NULL)
            AVIStreamRelease(pStreamRead);
        printf("Failed to get read stream\n");
        AVIFileExit();
        return FALSE;
    }

    int iNumFrames;
    int iFirstFrame;
    iFirstFrame = AVIStreamStart(pStreamRead);
    if (iFirstFrame == -1)
    {
        if (pStreamRead != NULL)
            AVIStreamRelease(pStreamRead);
        printf("First frame not found\n");
        AVIFileExit();
        return FALSE;
    }

    iNumFrames = AVIStreamLength(pStreamRead);
    if (iNumFrames == -1)
    {
        if (pStreamRead != NULL)
            AVIStreamRelease(pStreamRead);
        printf("Number of frames not found\n");
        AVIFileExit();
        return FALSE;
    }

    BITMAPINFOHEADER bih;
    ZeroMemory(&bih, sizeof(BITMAPINFOHEADER));

    LONG formatSize = 0;
    if (AVIStreamFormatSize(pStreamRead, 0, &formatSize) != ICERR_OK)
    {
        if (pStreamRead != NULL)
            AVIStreamRelease(pStreamRead);
        printf("Failed to get stream format size\n");
        AVIFileExit();
        return FALSE;
    }
    if (formatSize < sizeof(BITMAPINFOHEADER))
    {
        if (pStreamRead != NULL)
            AVIStreamRelease(pStreamRead);
        printf("Format size less then bmi header size\n");
        AVIFileExit();
        return FALSE;
    }
    if (AVIStreamReadFormat(pStreamRead, 0, &bih, &formatSize) != ICERR_OK)
    {
        if (pStreamRead != NULL)
            AVIStreamRelease(pStreamRead);
        printf("Failed to read stream format\n");
        AVIFileExit();
        return FALSE;
    }
    bih.biHeight = -bih.biHeight;

    HGLOBAL hpBuffer = GlobalAlloc(GHND, bih.biSizeImage);
    LPVOID lpBuffer = (LPVOID)GlobalLock(hpBuffer);

    res = ICInstall(ICTYPE_VIDEO, mmioFOURCC('j', 'x', 's', ' '),
        (LPARAM)(FARPROC)&DriverProc, NULL, ICINSTALL_FUNCTION);

    hIC = ICOpen(ICTYPE_VIDEO, mmioFOURCC('j', 'x', 's', ' '), ICMODE_COMPRESS);

    AVISTREAMINFO si = { 0 };
    si.fccType = streamtypeVIDEO;
    si.fccHandler = mmioFOURCC('j', 'x', 's', ' ');
    si.dwScale = 1;// fileread_info.dwScale;
    si.dwRate = fileread_info.dwRate / fileread_info.dwScale;
    si.dwQuality = (DWORD)-1;
    si.rcFrame = rect;
    IAVIStream* pStreamWrite;
    if (AVIFileCreateStream(pFileWrite, &pStreamWrite, &si) != 0)
    {
        printf("AVIFileCreateStream for write failed\n");
        return 1;
    }

    AVICOMPRESSOPTIONS co = { 0 };
    co.fccType = si.fccType;
    co.fccHandler = si.fccHandler;
    co.dwQuality = si.dwQuality;
    IAVIStream* pCompressedStream;
    if (AVIMakeCompressedStream(&pCompressedStream, pStreamWrite, &co, NULL) != 0)
    {
        printf("AVIMakeCompressedStream failed\n");
        return 1;
    }

    LPBITMAPINFOHEADER lpbiIn, lpbiOut;
    lpbiIn = &bih;
    LONG lFormatSize = ICCompressGetFormatSize(hIC, lpbiIn);
    HGLOBAL hbiOut = GlobalAlloc(GHND, lFormatSize);
    lpbiOut = (LPBITMAPINFOHEADER)GlobalLock(hbiOut);
    ICCompressGetFormat(hIC, &bih, lpbiOut);
    LRESULT lres = ICCompressQuery(hIC, (DWORD_PTR)lpbiIn, NULL);

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
    DWORD dwQuality = ICGetDefaultQuality(hIC);

    if (AVIStreamSetFormat(pCompressedStream, 0, lpbiIn, (DWORD)lpbiIn->biSize) != 0)
    {
        printf("AVIStreamSetFormat failed\n");
        return 1;
    }

    int index = 0;
    for (int i = iFirstFrame; i < iNumFrames; i++)
    {
        index = i - iFirstFrame;
        hr = AVIStreamRead(pStreamRead, index, 1, lpBuffer, bih.biSizeImage, NULL, NULL);
        ICCompress(hIC, 0, lpbiOut, lpOutput, lpbiIn, lpBuffer,
            &dwCkID, &dwCompFlags, index, lpbiIn->biSizeImage, dwQuality, NULL, NULL);
        hr = AVIStreamWrite(pStreamWrite, index, 1, lpOutput, lpbiOut->biSizeImage,
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
    if (AVIStreamRelease(pCompressedStream) != 0 || AVIStreamRelease(pStreamRead) != 0 || AVIStreamRelease(pStreamWrite) != 0)
    {
        printf("AVIStreamRelease failed\n");
        return 1;
    }
    if ((AVIFileRelease(pFileRead) != 0) || (AVIFileRelease(pFileWrite) != 0))
    {
        printf("AVIFileRelease failed\n");
        return 1;
    }

    AVIFileExit();

    printf("Succeeded\n");
    return 0;
}