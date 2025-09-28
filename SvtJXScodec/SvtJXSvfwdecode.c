#pragma once

#include "SvtJXScodec.h"

#pragma comment(lib, "vfw32.lib")

int main()
{
    const CHAR* infile = "JXS_VfW.avi";
    const CHAR* outfile = "JXS_VfW-decoded.avi";

    HRESULT hr;
    PAVIFILE pfIn, pfOut;

    HIC                 hIC;
    BOOL                res;

    res = ICInstall(ICTYPE_VIDEO, mmioFOURCC('j', 'x', 's', ' '),
        (LPARAM)(FARPROC)&DriverProc, NULL, ICINSTALL_FUNCTION);

    hIC = ICOpen(ICTYPE_VIDEO, mmioFOURCC('j', 'x', 's', ' '), ICMODE_DECOMPRESS);

    AVIFileInit();

    hr = AVIFileOpen(&pfIn, infile, OF_SHARE_DENY_WRITE, NULL);
    if (hr != S_OK)
    {
        printf("Cannot open input file\n");
        if (pfIn != NULL)
            AVIFileRelease(pfIn);
        return 1;
    }

    IAVIStream* pstreamIn;
    if (AVIFileGetStream(pfIn, &pstreamIn, streamtypeVIDEO, 0) != ICERR_OK)
    {
        if (pstreamIn != 0)
            AVIStreamRelease(pstreamIn);
        AVIFileRelease(pfIn);
        AVIFileExit();
        printf("First (and maybe only) stream is not 'streamtypeVIDEO'\n");
        return 1;
    }

    AVIFILEINFO avifile_info;
    AVIFileInfo(pfIn, &avifile_info, sizeof(AVIFILEINFO));

    LPBITMAPINFOHEADER lpbiIn, lpbiOut;

    LONG formatSize = 0;
    hr = AVIStreamReadFormat(pstreamIn, 0, NULL, &formatSize);
    lpbiIn = (BITMAPINFOHEADER*)malloc(sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));
    hr = AVIStreamReadFormat(pstreamIn, 0, (LPVOID)lpbiIn, &formatSize);

    //hIC = ICDecompressOpen(ICTYPE_VIDEO, mmioFOURCC('R', 'L', 'E', 'f'), lpbiIn, NULL);
    LONG lFormatSize = ICDecompressGetFormatSize(hIC, lpbiIn);

    HGLOBAL hpbiOut = GlobalAlloc(GHND, lFormatSize);
    lpbiOut = (LPBITMAPINFOHEADER)GlobalLock(hpbiOut);
    ICDecompressGetFormat(hIC, lpbiIn, lpbiOut);


    if (ICDecompressQuery(hIC, lpbiIn, lpbiOut) != ICERR_OK)
    {
        printf("Decompress query failed\n");
        return 1;
    }

    AVIFileOpen(&pfOut, outfile, OF_DELETE, NULL);
    if (AVIFileOpen(&pfOut, outfile, OF_CREATE | OF_WRITE, NULL) != 0)
    {
        printf("Cannot open output file\n");
        return 1;
    }

    // transcode input stream into output stream
    int iNumFrames;
    int iFirstFrame;

    iFirstFrame = AVIStreamStart(pstreamIn);
    if (iFirstFrame == -1)
    {
        if (pstreamIn != NULL)
            AVIStreamRelease(pstreamIn);

        AVIFileExit();
        return 1;
    }

    iNumFrames = AVIStreamLength(pstreamIn);
    if (iNumFrames == -1)
    {
        if (pstreamIn != NULL)
            AVIStreamRelease(pstreamIn);

        AVIFileExit();
        return 1;
    }

    AVISTREAMINFO si, siOut;
    AVIStreamInfo(pstreamIn, &si, sizeof(si));
    memcpy(&siOut, &si, sizeof(si));
    siOut.fccHandler = BI_RGB;
    siOut.dwSuggestedBufferSize = 0;

    IAVIStream* pstreamOut;
    if (AVIFileCreateStream(pfOut, &pstreamOut, &siOut) != 0)
    {
        printf("Cannot create AVI stream out\n");
        return 1;
    }

    if (AVIStreamSetFormat(pstreamOut, 0, lpbiOut, sizeof(BITMAPINFOHEADER) + 0 * sizeof(RGBQUAD)) != ICERR_OK)
    {
        printf("Cannot set format for output stream\n");
        return 1;
    }

    // transcode frames
    int index = 0;
    HGLOBAL hpOutput = GlobalAlloc(GHND, lpbiOut->biSizeImage);
    LPVOID lpOutput = (LPVOID)GlobalLock(hpOutput);

    HGLOBAL hpInput = GlobalAlloc(GHND, lpbiIn->biSizeImage);
    LPVOID lpInput = (LPVOID)GlobalLock(hpInput);

    LONG lBytesRead, lSamplesRead, lBytesWritten, lSamplesWritten;

    if (ICDecompressBegin(hIC, lpbiIn, lpbiOut) == ICERR_OK)
    {
        for (int i = iFirstFrame; i < iNumFrames; i++)
        {
            index = i - iFirstFrame;
            hr = AVIStreamRead(pstreamIn, i, 1, lpInput, lpbiIn->biSizeImage, &lBytesRead, &lSamplesRead);
            hr = ICDecompress(hIC, 0, lpbiIn, lpInput, lpbiOut, lpOutput);
            hr = AVIStreamWrite(pstreamOut, index, 1, lpOutput, lpbiOut->biSizeImage, AVIIF_KEYFRAME, &lSamplesWritten, &lBytesWritten);
        }
        ICDecompressEnd(hIC);
    }
    else
    {
        printf("Decompress failed\n");
    }

    ICRemove(ICTYPE_VIDEO, mmioFOURCC('j', 'x', 's', ' '), NULL);

    if(GlobalUnlock(hpbiOut))
        GlobalFree(hpbiOut);
    if(GlobalUnlock(hpOutput))
        GlobalFree(hpOutput);
    if(GlobalUnlock(hpInput))
        GlobalFree(hpInput);
    //close the streams after finishing the task
    if (pstreamIn != NULL)
        AVIStreamRelease(pstreamIn);

    if (pstreamOut != NULL)
        AVIStreamRelease(pstreamOut);

    AVIFileRelease(pfIn);
    AVIFileRelease(pfOut);

    AVIFileExit();

    printf("Succeeded\n");
    return 0;
}
