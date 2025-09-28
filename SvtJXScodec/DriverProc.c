#include "SvtJXScodec.h"
#pragma comment (lib, "winmm.lib")

LRESULT WINAPI DriverProc(DWORD_PTR dwDriverID, HDRVR hDriver, UINT uiMessage, LPARAM lParam1, LPARAM lParam2)
{
    switch (uiMessage)
    {
    case DRV_DISABLE:
    case DRV_ENABLE:
    case DRV_INSTALL:
    case DRV_REMOVE:
        return DRVCNF_OK;

    case DRV_OPEN:
    {
        return DRVCNF_OK;
    }

    case DRV_CLOSE:
        return DRVCNF_OK;

    case DRV_QUERYCONFIGURE:
        return DRVCNF_CANCEL;

    case DRV_CONFIGURE:
        return DRVCNF_OK;


    case ICM_GETDEFAULTQUALITY:
    case ICM_GETQUALITY:
        return ICERR_OK;

    case ICM_GETSTATE:
    case ICM_SETSTATE:
        return ICERR_OK;

    case ICM_GETINFO:
        if (lParam1)
        {
            if (lParam2 < sizeof(ICINFO))
                return ICERR_UNSUPPORTED;
            ICINFO* inf = (ICINFO*)lParam1;
            inf->dwSize = sizeof(ICINFO);
            inf->fccType = mmioFOURCC('V', 'I', 'D', 'C');
            inf->fccHandler = mmioFOURCC('j', 'x', 's', ' ');
            inf->dwFlags = VIDCF_FASTTEMPORALC | VIDCF_TEMPORAL;
            inf->dwVersion = 1; // arbitrary version number
            inf->dwVersionICM = ICVERSION;
            MultiByteToWideChar(CP_UTF8, 0, "JXS VfW Codec", -1, inf->szName, sizeof(inf->szName) / sizeof(WCHAR));
            MultiByteToWideChar(CP_UTF8, 0, "JXS VfW Codec", -1, inf->szDescription, sizeof(inf->szDescription) / sizeof(WCHAR));
            return sizeof(ICINFO);
        }
        return sizeof(ICINFO);


    case ICM_CONFIGURE:
        if (lParam1 == -1)
            return ICERR_OK;
        else
        {
            return ICERR_OK;
        }

    case ICM_ABOUT:
        if (lParam1 == -1)
            return ICERR_OK;
        else
        {
            return ICERR_OK;
        }

        /* ENCODER MESSAGES */

    case ICM_COMPRESS_QUERY:
        // can you compress (PBITMAPINFO)lParam1 into (PBITMAPINFO)lParam2?
        if (svtjxsvfw_compress_query((PBITMAPINFO)lParam1, (PBITMAPINFO)lParam2))
            return ICERR_OK;
        else
            return ICERR_BADFORMAT;

    case ICM_COMPRESS_GET_FORMAT:
        // populate (PBITMAPINFO)lParam2 with something you can compress to
        if (!lParam2)
            return sizeof(BITMAPINFO);
        if (svtjxsvfw_compress_get_format((PBITMAPINFO)lParam1, (PBITMAPINFO)lParam2))
            return ICERR_OK;
        else
            return ICERR_BADFORMAT;

    case ICM_COMPRESS_GET_SIZE:
        // return max size of compressed frame (worst case)
        return svtjxsvfw_compress_max_size(/*this, */(PBITMAPINFO)lParam1, (PBITMAPINFO)lParam2);


    case ICM_COMPRESS_FRAMES_INFO:
        // set parameters for upcoming compression. ICERR_OK or error
        if (lParam1)
        {
            return svtjxsvfw_compress_frames_info((ICCOMPRESSFRAMES*)lParam1, (size_t*)lParam2);
        }
        return ICERR_ERROR;

    case ICM_COMPRESS_BEGIN:
        // start compression stream
        if (svtjxsvfw_compress_begin((PBITMAPINFO)lParam1, (PBITMAPINFO)lParam2))
            return ICERR_OK;
        else
            return ICERR_BADFORMAT;


    case ICM_COMPRESS:
        // compress a frame
        if (svtjxsvfw_compress((ICCOMPRESS*)lParam1, lParam2))
            return ICERR_OK;
        else
            return ICERR_ERROR;

    case ICM_COMPRESS_END:
        return svtjxsvfw_compress_end();

        /* DECODER MESSAGES */

    case ICM_DECOMPRESS_BEGIN:
        return svtjxsvfw_decompress_begin((BITMAPINFO*)lParam1, (BITMAPINFO*)lParam2);

    case ICM_DECOMPRESS:
        if (svtjxsvfw_decompress((ICDECOMPRESS*)lParam1, lParam2))
            return ICERR_OK;
        else
            return ICERR_ERROR;

    case ICM_DECOMPRESS_END:
        return svtjxsvfw_decompress_end();

    case ICM_DECOMPRESS_GET_FORMAT:
        return svtjxsvfw_decompress_get_format((BITMAPINFO*)lParam1, (BITMAPINFO*)lParam2);

    case ICM_DECOMPRESS_QUERY:
        return svtjxsvfw_decompress_query((BITMAPINFO*)lParam1, (BITMAPINFO*)lParam2);

    default:
        break;
    }


    if (uiMessage < DRV_USER)
        return DefDriverProc(dwDriverID, hDriver, uiMessage, lParam1, lParam2);
    else
        return ICERR_UNSUPPORTED;
}
