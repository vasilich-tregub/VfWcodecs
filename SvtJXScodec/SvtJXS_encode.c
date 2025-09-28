#include "svtjxs/SvtJpegxsEnc.h"

int32_t main(int32_t argc, char* argv[]) {
    FILE* output_file = NULL;
    fopen_s(&output_file, "SVT_JXS.jxs", "wb");

    svt_jpeg_xs_encoder_api_t enc;
    SvtJxsErrorType_t err = svt_jpeg_xs_encoder_load_default_parameters(SVT_JPEGXS_API_VER_MAJOR, SVT_JPEGXS_API_VER_MINOR, &enc);

    if (err != SvtJxsErrorNone) {
        return err;
    }

    enc.source_width = 320;
    enc.source_height = 240;
    enc.input_bit_depth = 8;
    enc.colour_format = COLOUR_FORMAT_PLANAR_YUV444_OR_RGB;
    enc.bpp_numerator = 1;
    enc.verbose = 0;

    err = svt_jpeg_xs_encoder_init(SVT_JPEGXS_API_VER_MAJOR, SVT_JPEGXS_API_VER_MINOR, &enc);
    if (err != SvtJxsErrorNone) {
        return err;
    }

    uint32_t pixel_size = enc.input_bit_depth <= 8 ? 1 : 2;
    svt_jpeg_xs_image_buffer_t in_buf;
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

    svt_jpeg_xs_bitstream_buffer_t out_buf;
    uint32_t bitstream_size = (uint32_t)(
        ((uint64_t)enc.source_width * enc.source_height * enc.bpp_numerator / enc.bpp_denominator + 7) / +8);
    out_buf.allocation_size = bitstream_size;
    out_buf.used_size = 0;
    out_buf.buffer = malloc(out_buf.allocation_size);
    if (!out_buf.buffer) {
        return SvtJxsErrorInsufficientResources;
    }

    { //loop over multiple frames
        //Get YUV 8bit from file or any other location
        for (int iy = 0; iy < enc.source_height; ++iy)
        {
            for (int ix = 0; ix < enc.source_width; ++ix)
            {
                *((unsigned char*)in_buf.data_yuv[0] + iy * enc.source_width + ix) =
                    (16 + iy * 480 / enc.source_height + ix * 480 / enc.source_width) % 256;
                *((unsigned char*)in_buf.data_yuv[1] + (iy * enc.source_width + ix)) = 128;
                *((unsigned char*)in_buf.data_yuv[2] + (iy * enc.source_width + ix)) = 128;
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
        printf("bitstream output: pointer: %p bitstream size: %u\n", enc_output.bitstream.buffer, enc_output.bitstream.used_size);
        fwrite(enc_output.bitstream.buffer, 1, enc_output.bitstream.used_size, output_file);
    }

    svt_jpeg_xs_encoder_close(&enc);

    free(out_buf.buffer);
    free(in_buf.data_yuv[0]);
    return 0;
}