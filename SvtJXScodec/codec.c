#include "SvtJXScodec.h"
#include "svtjxs/SvtJpegxsEnc.h"

SvtJxsErrorType_t err;
svt_jpeg_xs_encoder_api_t enc;
svt_jpeg_xs_image_buffer_t in_buf;
svt_jpeg_xs_bitstream_buffer_t out_buf;


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

LRESULT svtjxsvfw_compress_query(PBITMAPINFO bin, PBITMAPINFO bout)
{
    if (compute_size(&bin->bmiHeader) < 0)
        return 0;
    return 1;
}


LRESULT svtjxsvfw_compress_get_format(PBITMAPINFO bin, PBITMAPINFO bout)
{
    if (!svtjxsvfw_compress_query(bin, bout))
        return ICERR_OK;

    memcpy(bout, bin, bin->bmiHeader.biSize);
    bout->bmiHeader.biCompression = mmioFOURCC('j', 'x', 's', ' ');
    bout->bmiHeader.biSizeImage = 2 * bin->bmiHeader.biSizeImage;

    return 1;
}

LRESULT svtjxsvfw_compress_max_size(PBITMAPINFO bin, PBITMAPINFO bout)
{
    return bin->bmiHeader.biSizeImage * 2;
}

LRESULT svtjxsvfw_compress_begin(PBITMAPINFO bin, PBITMAPINFO bout)
{
    if (!svtjxsvfw_compress_query(bin, bout))
        return 0;

    err = svt_jpeg_xs_encoder_load_default_parameters(SVT_JPEGXS_API_VER_MAJOR, SVT_JPEGXS_API_VER_MINOR, &enc);

    if (err != SvtJxsErrorNone) {
        return err;
    }

    enc.source_width = bin->bmiHeader.biWidth;
    enc.source_height = bin->bmiHeader.biHeight;
    enc.input_bit_depth = bin->bmiHeader.biBitCount / 3; // image color component number
    enc.colour_format = COLOUR_FORMAT_PLANAR_YUV444_OR_RGB;
    enc.bpp_numerator = 1;
    enc.bpp_denominator = 1;
    enc.verbose = 0;

    err = svt_jpeg_xs_encoder_init(SVT_JPEGXS_API_VER_MAJOR, SVT_JPEGXS_API_VER_MINOR, &enc);
    if (err != SvtJxsErrorNone) {
        return err;
    }

    uint32_t pixel_size = enc.input_bit_depth <= 8 ? 1 : 2;
    in_buf.stride[0] = enc.source_width;
    in_buf.stride[1] = enc.source_width;
    in_buf.stride[2] = enc.source_width;
    for (uint8_t i = 0; i < 3; ++i) {
        in_buf.alloc_size[i] = in_buf.stride[i] * enc.source_height * pixel_size;
        in_buf.data_yuv[i] = malloc(in_buf.alloc_size[i]);
        if (!in_buf.data_yuv[i]) {
            return SvtJxsErrorInsufficientResources;
        }
    }

    uint32_t bitstream_size = (uint32_t)(
        ((uint64_t)enc.source_width * enc.source_height * enc.bpp_numerator / enc.bpp_denominator + 7) / +8);
    out_buf.allocation_size = bitstream_size;
    out_buf.used_size = 0;
    out_buf.buffer = malloc(out_buf.allocation_size);
    if (!out_buf.buffer) {
        return SvtJxsErrorInsufficientResources;
    }

    return 1;
}

LRESULT svtjxsvfw_compress_frames_info(ICCOMPRESSFRAMES* icc, size_t* iccsz)
{
    return ICERR_OK;
}

LRESULT svtjxsvfw_compress(ICCOMPRESS* icc, size_t iccsz)
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

LRESULT svtjxsvfw_compress_end()
{
    svt_jpeg_xs_encoder_close(&enc);

    free(out_buf.buffer);
    free(in_buf.data_yuv[0]);
    free(in_buf.data_yuv[1]);
    free(in_buf.data_yuv[2]);

    return 1;
}

/*Decompressor*/

LRESULT svtjxsvfw_decompress_query(PBITMAPINFO bin, PBITMAPINFO bout)
{
    if (compute_size(&bin->bmiHeader) < 0)
        return ICERR_OK;

    return ICERR_OK;
}

LRESULT svtjxsvfw_decompress_get_format(PBITMAPINFO bin, PBITMAPINFO bout)
{
    //if (!svtjxsvfw_decompress_query(bin, bout))
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

LRESULT svtjxsvfw_decompress_max_size(PBITMAPINFO bin, PBITMAPINFO bout)
{
    return ((bin->bmiHeader.biWidth + 15) & ~15) * ((bout->bmiHeader.biHeight + 31) & ~31) * 3 + 4096;
}

LRESULT svtjxsvfw_decompress_begin(PBITMAPINFO bin, PBITMAPINFO bout)
{
    if (!svtjxsvfw_decompress_query(bin, bout))
        return ICERR_OK;

    return 1;

}

LRESULT svtjxsvfw_decompress(ICDECOMPRESS* icc, size_t iccsz)
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

LRESULT svtjxsvfw_decompress_end()
{
    return 1;
}

int jxs_compress(LPVOID dst, size_t* outsz, LPVOID src, size_t insz)
{
    *outsz = jxs_encode(dst, src, (int)insz);
    return 0;
}

int jxs_encode(char* out, const char* in, int insz)
{
    int retsize = 0;

    { //loop over multiple frames
        //Get bgr24 8bit from avi file or any other location and convert to yuv444p
        for (int iy = 0; iy < enc.source_height; ++iy)
        {
            for (int ix = 0; ix < enc.source_width; ++ix)
            {
                *((unsigned char*)in_buf.data_yuv[0] + (iy * enc.source_width + ix)) = *in++;
                *((unsigned char*)in_buf.data_yuv[1] + (iy * enc.source_width + ix)) = *in++;
                *((unsigned char*)in_buf.data_yuv[2] + (iy * enc.source_width + ix)) = *in++;
            }
        }

        svt_jpeg_xs_frame_t enc_input;
        enc_input.bitstream = out_buf;
        enc_input.image = in_buf;
        enc_input.user_prv_ctx_ptr = NULL;

        err = svt_jpeg_xs_encoder_send_picture(&enc, &enc_input, 1 /*blocking*/);
        if (err != SvtJxsErrorNone) {
            return err;
        }

        svt_jpeg_xs_frame_t enc_output;
        err = svt_jpeg_xs_encoder_get_packet(&enc, &enc_output, 1 /*blocking*/);
        if (err != SvtJxsErrorNone) {
            return err;
        }
        //Store bitstream to file
        //printf("bitstream output: pointer: %p bitstream size: %u\n", enc_output.bitstream.buffer, enc_output.bitstream.used_size);
        memcpy(out, enc_output.bitstream.buffer, enc_output.bitstream.used_size);
        retsize = enc_output.bitstream.used_size;
    }

    return retsize;
}
