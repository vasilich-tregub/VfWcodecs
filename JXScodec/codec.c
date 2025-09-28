#include "JXScodec.h"

// byte size of image data
// usually just the biSizeImage member
static int compute_size(const BITMAPINFOHEADER* h)
{
    if (h->biSizeImage)
        return h->biSizeImage;
    if (h->biCompression == BI_RGB)
    {
        int pitch = h->biWidth * h->biBitCount / 8;
        // adjust to mod 4
        pitch = (pitch + 3) & ~3;
        // height < 0 means vflip, so strip it here
        if (h->biHeight < 0)
            return pitch * -h->biHeight;
        else
            return pitch * h->biHeight;
    }
    return 1;
}

LRESULT jxsvfw_compress_query(PBITMAPINFO bin, PBITMAPINFO bout)
{
    if (compute_size(&bin->bmiHeader) < 0)
        return 0;
    return 1;
}


LRESULT jxsvfw_compress_get_format(PBITMAPINFO bin, PBITMAPINFO bout)
{
    if (!jxsvfw_compress_query(bin, bout))
        return ICERR_OK;

    memcpy(bout, bin, bin->bmiHeader.biSize);
    bout->bmiHeader.biCompression = mmioFOURCC('j', 'x', 's', ' ');
    bout->bmiHeader.biSizeImage = 2 * bin->bmiHeader.biSizeImage;

    return 1;
}

LRESULT jxsvfw_compress_max_size(PBITMAPINFO bin, PBITMAPINFO bout)
{
    return bin->bmiHeader.biSizeImage * 2;
}

LRESULT jxsvfw_compress_begin(PBITMAPINFO bin, PBITMAPINFO bout)
{
    if (!jxsvfw_compress_query(bin, bout))
        return 0;

    return 1;
}

LRESULT jxsvfw_compress_frames_info(ICCOMPRESSFRAMES* icc, size_t* iccsz)
{
    return ICERR_OK;
}

LRESULT jxsvfw_compress(ICCOMPRESS* icc, size_t iccsz)
{
    size_t sz = compute_size(icc->lpbiInput);
    size_t outsize = 0;
    if (sz < 0)
        return ICERR_OK;


    *icc->lpdwFlags = AVIIF_KEYFRAME;
    jxs_compress((icc->lpOutput), &outsize, icc->lpInput, sz);
    icc->lpbiOutput->biSizeImage = (DWORD)outsize;

    return 1;
}

LRESULT jxsvfw_compress_end()
{
    return 1;
}

/*Decompressor*/

LRESULT jxsvfw_decompress_query(PBITMAPINFO bin, PBITMAPINFO bout)
{
    if (compute_size(&bin->bmiHeader) < 0)
        return ICERR_OK;

    return ICERR_OK;
}

LRESULT jxsvfw_decompress_get_format(PBITMAPINFO bin, PBITMAPINFO bout)
{
    //if (!jxsvfw_decompress_query(bin, bout))
        //return 0;

    if (bout == NULL)
    {
        return bin->bmiHeader.biWidth * bin->bmiHeader.biHeight;
    }

    memcpy(bout, bin, bin->bmiHeader.biSize);
    bout->bmiHeader.biCompression = BI_RGB;
    bout->bmiHeader.biSizeImage = bin->bmiHeader.biWidth * bin->bmiHeader.biHeight * 3;

    return 1;
}

LRESULT jxsvfw_decompress_max_size(PBITMAPINFO bin, PBITMAPINFO bout)
{
    return ((bin->bmiHeader.biWidth + 15) & ~15) * ((bout->bmiHeader.biHeight + 31) & ~31) * 3 + 4096;
}

LRESULT jxsvfw_decompress_begin(PBITMAPINFO bin, PBITMAPINFO bout)
{
    if (!jxsvfw_decompress_query(bin, bout))
        return ICERR_OK;

    return 1;

}

LRESULT jxsvfw_decompress(ICDECOMPRESS* icc, size_t iccsz)
{
    int sz = compute_size(icc->lpbiInput);
    if (sz < 0)
        return ICERR_OK;

    icc->lpbiOutput->biSizeImage = sz;

    size_t outsize = 0;
    icc->dwFlags = AVIIF_KEYFRAME;
    jxs_decompress((icc->lpOutput), &outsize, icc->lpInput, sz);
    icc->lpbiOutput->biSizeImage = (DWORD)outsize;

    return 1;
}

LRESULT jxsvfw_decompress_end()
{
    return 1;
}
