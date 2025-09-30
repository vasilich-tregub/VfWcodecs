#include "libjxs/libjxs.h"
//#include "image_open.h"
//#include "cmdline_options.h"
#include "file_io.h" // definitions in jxs_utils.lib
//#include "file_sequence.h"
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool xs_parse_u8array_(uint8_t* values, int max_items, char* cfg_str, int* num)
{
	for (int i = 0; max_items > 0; ++i, --max_items)
	{
		if (*cfg_str == 0)
		{
			return true;
		}
		if (num != 0)
		{
			*num = i;
		}
		values[i] = (uint8_t)atoi(cfg_str);
		while (*cfg_str != ',' && *cfg_str != 0) ++cfg_str;
		if (*cfg_str == ',') ++cfg_str;
	}
	return *cfg_str == 0;
}

int main(int argc, char** argv)
{
	xs_config_t xs_config;
	xs_image_t image = { 0 };
	const char* output_fn = "fraunhofer-color.jxs";
	uint8_t* bitstream_buf = NULL;
	size_t bitstream_buf_size, bitstream_buf_max_size;
	xs_enc_context_t* ctx = NULL;
	int ret = 0;
	int file_idx = 0;
	//cmdline_options_t options;
	//int optind;

	xs_dec_context_t* decctx = NULL;
	fprintf(stderr, "Learn JPEG XS lossless packing\n");

	do
	{
		const int32_t width = 64;
		const int32_t height = 16;
		const uint32_t depth = 8;
		const uint32_t bpp = 12;
		xs_config.verbose = 0; // options.verbose;

		image.width = width;
		image.height = height;
		image.depth = depth;
		image.ncomps = 3;

		// YUV444
		image.sx[2] = image.sx[1] = image.sx[0] = 1;
		image.sy[2] = image.sy[1] = image.sy[0] = 1;
		//word_size = (bytesRead == nsamples444 * 2) ? 16 : 8;
		if (!xs_allocate_image(&image, false))
			return -1;

		uint32_t* ptr0 = image.comps_array[0];
		uint32_t* ptr1 = image.comps_array[1];
		uint32_t* ptr2 = image.comps_array[2];
		for (int iy = 0; iy < height; ++iy) {
			for (int ix = 0; ix < width; ++ix) {
				ptr0[iy * width + ix] = (16 + iy * 480 / height + ix * 480 / width) % 256;
				//(ix - (int)width / 2) * 256 / (int)width + 129;   // Y
				ptr1[iy * width + ix] = 128;   // Cb
				ptr2[iy * width + ix] = 128;   // Cr
			}
		}

		/*printf("Source image:\n");
		for (int iy = 0; iy < height; ++iy) {
			for (int ix = 0; ix < width; ++ix) {
				printf("%d ", (unsigned char*)ptr0[iy * width + ix]);
			}
			printf("\n");
		}
		printf("\n");*/

		xs_config.bitstream_size_in_bytes = -1;// (size_t)(bpp * image.width * image.height / 8);
		xs_config.ra_budget_lines = 20.0f;
		xs_config.verbose = 3;// 255;
		xs_config.gains_mode = XS_GAINS_OPT_PSNR;
		xs_config.profile = XS_PROFILE_UNRESTRICTED;
		xs_config.level = XS_LEVEL_UNRESTRICTED;
		xs_config.sublevel = XS_SUBLEVEL_UNRESTRICTED;
		xs_config.cap_bits = XS_CAP_AUTO;
		xs_config.fbblevel = XS_FBBLEVEL_UNRESTRICTED;  // added in v3
		xs_config.use_tdc_mode = 0;                     // added in v3
		xs_config.use_long_precinct_headers = 0;        // added in v3 (questionable, value provided only to reach consistence with ed2 mono_mls script)
		xs_config.p.tpc.enabled = 0;					// added in v3
		xs_config.p.color_transform = XS_CPIH_NONE;
		xs_config.p.Cw = 0;
		xs_config.p.slice_height = 0x0010;
		xs_config.p.N_g = 4;
		xs_config.p.S_s = 8;
		xs_config.p.Bw = 255;
		xs_config.p.Fq = 8; // fractional bits; 8 in xs_enc_generated/monochrome; must be 0 in mls
		xs_config.p.B_r = 4; // Number of bits to encode a bit-plane count in raw
		xs_config.p.Fslc = 0; // Slice coding mode
		xs_config.p.Ppoc = 0; // Progression order of bands within precincts
		xs_config.p.NLx = 5;
		xs_config.p.NLy = 2;
		xs_config.p.Lh = 0; // long precinct header enforcement flag (0 or 1)
		xs_config.p.Rl = 0; // 1 is raw mode selection per packet; 0 is no RAW_PER_PKT
		xs_config.p.Qpih = 0; // 0 deadzone; 1 uniform
		xs_config.p.Fs = 0;
		xs_config.p.Rm = 0; // 0 (RSF) or 1 (CSF); (==RUN_SIGFLAGS_ZRCSF)?
		xs_config.p.Sd = 0;
		memset(xs_config.p.lvl_gain[0], 255, (MAX_NBANDS + 1) * sizeof(uint8_t));
		if (xs_config.p.NLx == 1)
			xs_parse_u8array_(xs_config.p.lvl_gain[0], MAX_NBANDS, "1,1,1,1,1,1", 0); //  "1,0,0,1,0,0" "1,1,0,0,0,0"; "2,2,2,1,1,1"
		else if (xs_config.p.NLx == 2)
			xs_parse_u8array_(xs_config.p.lvl_gain[0], MAX_NBANDS, "1,1,1", 0); //  "1,0,0,1,0,0" "1,1,0,0,0,0"; "2,2,2,1,1,1"
		else if (xs_config.p.NLx == 3)
			xs_parse_u8array_(xs_config.p.lvl_gain[0], MAX_NBANDS, "1,1,1,1", 0); //  "1,0,0,1,0,0" "1,1,0,0,0,0"; "2,2,2,1,1,1"
		else if (xs_config.p.NLx == 4)
			xs_parse_u8array_(xs_config.p.lvl_gain[0], MAX_NBANDS, "1,1,1,1,1", 0); //  "1,0,0,1,0,0" "1,1,0,0,0,0"; "2,2,2,1,1,1"
		else if (xs_config.p.NLx == 5)
			xs_parse_u8array_(xs_config.p.lvl_gain[0], MAX_NBANDS, "1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1", 0); //  "1,0,0,1,0,0" "1,1,0,0,0,0"; "2,2,2,1,1,1"
		memset(xs_config.p.lvl_prio[0], 255, (MAX_NBANDS + 1) * sizeof(uint8_t));
		if (xs_config.p.NLx == 1)
			xs_parse_u8array_(xs_config.p.lvl_prio[0], MAX_NBANDS, "0,1,0,1,0,1", 0); // "0,2,3,1,4,5" "0,1,2,4,3,5"; "8,7,6,5,3,4"
		else if (xs_config.p.NLx == 2)
			xs_parse_u8array_(xs_config.p.lvl_prio[0], MAX_NBANDS, "0,1,2", 0); // "0,2,3,1,4,5" "0,1,2,4,3,5"; "8,7,6,5,3,4"
		else if (xs_config.p.NLx == 3)
			xs_parse_u8array_(xs_config.p.lvl_prio[0], MAX_NBANDS, "0,1,2,3", 0); // "0,2,3,1,4,5" "0,1,2,4,3,5"; "8,7,6,5,3,4"
		else if (xs_config.p.NLx == 4)
			xs_parse_u8array_(xs_config.p.lvl_prio[0], MAX_NBANDS, "0,1,2,3,4", 0); // "0,2,3,1,4,5" "0,1,2,4,3,5"; "8,7,6,5,3,4"
		else if (xs_config.p.NLx == 5)
			xs_parse_u8array_(xs_config.p.lvl_prio[0], MAX_NBANDS, "0,1,2,3,4,5,1,2,3,4,5,6,1,2,3,4,5,6,1,2,3,4,5,6,1,2,3,4,5,6", 0); // "0,2,3,1,4,5" "0,1,2,4,3,5"; "8,7,6,5,3,4"
		xs_config.p.Tnlt = XS_NLT_NONE;
		xs_config.p.Tnlt_params.quadratic.sigma = 0; xs_config.p.Tnlt_params.quadratic.alpha = 0;
		xs_config.p.Tnlt_params.extended.T1 = 0; xs_config.p.Tnlt_params.extended.T2 = 0; xs_config.p.Tnlt_params.extended.E = 0;
		xs_config.p.tetrix_params.Cf = XS_TETRIX_FULL; xs_config.p.tetrix_params.e1 = 0; xs_config.p.tetrix_params.e2 = 0;
		xs_config.p.cfa_pattern = XS_CFA_RGGB;

		if (!xs_enc_preprocess_image(&xs_config, &image))
		{
			fprintf(stderr, "Error preprocessing the image\n");
			ret = -1;
			break;
		}

		ctx = xs_enc_init(&xs_config, &image);
		if (!ctx)
		{
			fprintf(stderr, "Unable to allocate encoding context\n");
			ret = -1;
			break;
		}

		if (xs_config.bitstream_size_in_bytes == (size_t)-1)
		{
			// Take the RAW image size and add some extra for margin.
			bitstream_buf_max_size = image.width * image.height * image.ncomps * ((image.depth + 7) >> 3) + 1024 * 1024;
		}
		else
		{
			bitstream_buf_max_size = (xs_config.bitstream_size_in_bytes + 7) & (~0x7);
		}
		bitstream_buf = (uint8_t*)malloc(bitstream_buf_max_size);
		if (!bitstream_buf)
		{
			fprintf(stderr, "Unable to allocate codestream mem\n");
			ret = -1;
			break;
		}

		do {
			if (!fileio_writable(output_fn))
			{
				fprintf(stderr, "Output file is not writable %s\n", output_fn);
				ret = -1;
				break;
			}
			if (!xs_enc_image(ctx, 0, &image, (uint8_t*)bitstream_buf, bitstream_buf_max_size, &bitstream_buf_size))
			{
				fprintf(stderr, "Unable to encode image\n");
				ret = -1;
				break;
			}
			/*printf("image.comps_array after encoding:\n");
			int32_t* ptr = image.comps_array[0];
			for (int iy = 0; iy < height; ++iy) {
				for (int ix = 0; ix < width; ++ix) {
					printf("%d ", (unsigned char*)ptr[iy * width + ix]);
				}
				printf("\n");
			}
			printf("\n");*/
			/*/ Only to make sure that the data does not 'leak' via the variable image. Can be discarded through up to 'end of image leakage'
			xs_free_image(&image);
			image.width = image.height = 0;
			if (!xs_dec_probe(bitstream_buf, bitstream_buf_size, &xs_config, &image))
			{
				fprintf(stderr, "Unable to parse input codestream (local variable bitstream_buf)\n");
				ret = -1;
				break;
			}
			if (!xs_allocate_image(&image, false))
			{
				fprintf(stderr, "Image memory allocation error\n");
				ret = -1;
				break;
			}
			// end of 'image leakage'
			decctx = xs_dec_init(&xs_config, &image);
			if (!decctx)
			{
				fprintf(stderr, "Unable to allocate decoding context to decode encoded image\n");
				ret = -1;
				break;
			}
			/*printf("Re-allocated image::\n");
			ptr = image.comps_array[0];
			for (int iy = 0; iy < height; ++iy) {
				for (int ix = 0; ix < width / 2; ++ix) {
					printf("%d ", ptr[iy * width + 2 * ix] >> 12);
				}
				printf("\n");
				for (int ix = 0; ix < width / 2; ++ix) {
					printf("%d ", ptr[iy * width + 2 * ix + 1] >> 12);
				}
				printf("\n");
			}
			if (!xs_dec_bitstream(decctx, 0, bitstream_buf, bitstream_buf_size, &image))
			{
				fprintf(stderr, "Unable to decode encoded image\n");
				ret = -1;
				break;
			}
			/*printf("image.comps_array after decoding:\n");
			for (int iy = 0; iy < height; ++iy) {
				for (int ix = 0; ix < width; ++ix) {
					printf("%d ", (unsigned char*)ptr[iy * width + ix]);
				}
				printf("\n");
			}
			printf("\n");
			xs_free_image(&image);*/
			if (fileio_write(output_fn, bitstream_buf, bitstream_buf_size) < 0)
			{
				fprintf(stderr, "Unable to write output encoded codestream to %s\n", output_fn);
				ret = -1;
				break;
			}
		} while (false);
	} while (false);

	// Cleanup.
	if (output_fn)
	{
		//free(output_fn);
	}
	xs_free_image(&image);
	if (ctx)
	{
		xs_enc_close(ctx);
	}
	if (bitstream_buf)
	{
		free(bitstream_buf);
	}
	return ret;
}
