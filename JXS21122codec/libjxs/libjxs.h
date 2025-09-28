/***************************************************************************
** intoPIX SA, Fraunhofer IIS and Canon Inc. (hereinafter the "Software   **
** Copyright Holder") hold or have the right to license copyright with    **
** respect to the accompanying software (hereinafter the "Software").     **
**                                                                        **
** Copyright License for Evaluation and Testing                           **
** --------------------------------------------                           **
**                                                                        **
** The Software Copyright Holder hereby grants, to any implementer of     **
** this ISO Standard, an irrevocable, non-exclusive, worldwide,           **
** royalty-free, sub-licensable copyright licence to prepare derivative   **
** works of (including translations, adaptations, alterations), the       **
** Software and reproduce, display, distribute and execute the Software   **
** and derivative works thereof, for the following limited purposes: (i)  **
** to evaluate the Software and any derivative works thereof for          **
** inclusion in its implementation of this ISO Standard, and (ii)         **
** to determine whether its implementation conforms with this ISO         **
** Standard.                                                              **
**                                                                        **
** The Software Copyright Holder represents and warrants that, to the     **
** best of its knowledge, it has the necessary copyright rights to        **
** license the Software pursuant to the terms and conditions set forth in **
** this option.                                                           **
**                                                                        **
** No patent licence is granted, nor is a patent licensing commitment     **
** made, by implication, estoppel or otherwise.                           **
**                                                                        **
** Disclaimer: Other than as expressly provided herein, (1) the Software  **
** is provided “AS IS” WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING  **
** BUT NOT LIMITED TO, THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A   **
** PARTICULAR PURPOSE AND NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS**
** and (2) neither the Software Copyright Holder (or its affiliates) nor  **
** the ISO shall be held liable in any event for any damages whatsoever   **
** (including, without limitation, damages for loss of profits, business  **
** interruption, loss of information, or any other pecuniary loss)        **
** arising out of or related to the use of or inability to use the        **
** Software.                                                              **
**                                                                        **
** RAND Copyright Licensing Commitment                                    **
** -----------------------------------                                    **
**                                                                        **
** IN THE EVENT YOU WISH TO INCLUDE THE SOFTWARE IN A CONFORMING          **
** IMPLEMENTATION OF THIS ISO STANDARD, PLEASE BE FURTHER ADVISED THAT:   **
**                                                                        **
** The Software Copyright Holder agrees to grant a copyright              **
** license on reasonable and non- discriminatory terms and conditions for **
** the purpose of including the Software in a conforming implementation   **
** of the ISO Standard. Negotiations with regard to the license are       **
** left to the parties concerned and are performed outside the ISO.       **
**                                                                        **
** No patent licence is granted, nor is a patent licensing commitment     **
** made, by implication, estoppel or otherwise.                           **
***************************************************************************/

#ifndef LIBJXS_H
#define LIBJXS_H

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef JXS_SHARED_LIB_API
#define JXS_SHARED_LIB_API
#endif

#define CONFORMANCE_ENABLE_GENERATE_CSV_CODE

typedef int32_t xs_data_in_t;

#define MAX_NDECOMP_H 5
#define MAX_NDECOMP_V 2
#define MAX_NCOMPS 4

#define MAX_NFILTER_TYPES (2 * (MAX_NDECOMP_V) + (MAX_NDECOMP_H) + 1)
#define MAX_NBANDS (MAX_NCOMPS * MAX_NFILTER_TYPES)

enum xs_gains_mode_e
{
  XS_GAINS_OPT_PSNR,
  XS_GAINS_OPT_VISUAL,
  XS_GAINS_OPT_EXPLICIT,
};
typedef enum xs_gains_mode_e xs_gains_mode_t;

enum xs_profile_e
{
  XS_PROFILE_AUTO = 0xffff,
  XS_PROFILE_UNRESTRICTED = 0x0000,
  XS_PROFILE_LIGHT_422_10 = 0x1500,
  XS_PROFILE_LIGHT_444_12 = 0x1a00,
  XS_PROFILE_LIGHT_SUBLINE_422_10 = 0x2500,
  XS_PROFILE_MAIN_420_12 = 0x3240,
  XS_PROFILE_MAIN_422_10 = 0x3540,
  XS_PROFILE_MAIN_444_12 = 0x3a40,
  XS_PROFILE_MAIN_4444_12 = 0x3e40,
  XS_PROFILE_HIGH_420_12 = 0x4240,
  XS_PROFILE_HIGH_444_12 = 0x4a40,
  XS_PROFILE_HIGH_4444_12 = 0x4e40,
  XS_PROFILE_MLS_12 = 0x6ec0,
  XS_PROFILE_MLS_16 = 0x6ed0,
  XS_PROFILE_LIGHT_BAYER = 0x9300,
  XS_PROFILE_MAIN_BAYER = 0xb340,
  XS_PROFILE_HIGH_BAYER = 0xc340,
  XS_PROFILE_CHIGH_444_12 = 0x4a44,
  XS_PROFILE_TDC_444_12 = 0x4a45,
  XS_PROFILE_TDC_MLS_444_12 = 0x6a45,
};
typedef enum xs_profile_e xs_profile_t;

enum xs_level_e
{
  XS_LEVEL_AUTO = 0xffff,
  XS_LEVEL_UNRESTRICTED = 0x0000,
  XS_LEVEL_1K_1 = 0x0400,
  XS_LEVEL_2K_1 = 0x1000,
  XS_LEVEL_4K_1 = 0x2000,
  XS_LEVEL_4K_2 = 0x2400,
  XS_LEVEL_4K_3 = 0x2800,
  XS_LEVEL_8K_1 = 0x3000,
  XS_LEVEL_8K_2 = 0x3400,
  XS_LEVEL_8K_3 = 0x3800,
  XS_LEVEL_10K_1 = 0x4000,
  XS_LEVEL_MASK = 0x7c00,
};
typedef enum xs_level_e xs_level_t;

enum xs_sublevel_e
{
  XS_SUBLEVEL_AUTO = 0xffff,
  XS_SUBLEVEL_UNRESTRICTED = 0x0000,
  XS_SUBLEVEL_FULL = 0x0080,
  XS_SUBLEVEL_12_BPP = 0x0010,
  XS_SUBLEVEL_9_BPP = 0x000c,
  XS_SUBLEVEL_6_BPP = 0x0008,
  XS_SUBLEVEL_4_BPP = 0x0006,
  XS_SUBLEVEL_3_BPP = 0x0004,
  XS_SUBLEVEL_2_BPP = 0x0003,
  XS_SUBLEVEL_MASK = 0x009f
};
typedef enum xs_sublevel_e xs_sublevel_t;

enum xs_fbblevel_e
{
  XS_FBBLEVEL_AUTO = 0xffff,
  XS_FBBLEVEL_UNRESTRICTED = 0x0000,
  XS_FBBLEVEL_3_BPP = 0x0060,
  XS_FBBLEVEL_4_5_BPP = 0x0100,
  XS_FBBLEVEL_12_BPP = 0x0300,
  XS_FBBLEVEL_FULL = 0x0360,
  XS_FBBLEVEL_MASK = 0x0360,
};
typedef enum xs_fbblevel_e xs_fbblevel_t;

enum xs_cpih_e
{
  XS_CPIH_AUTO = 0xff,
  XS_CPIH_NONE = 0x00,
  XS_CPIH_RCT = 0x01,
  XS_CPIH_TETRIX = 0x03,
};
typedef enum xs_cpih_e xs_cpih_t;

enum xs_cap_e
{
  XS_CAP_AUTO = 0xffff,
  XS_CAP_STAR_TETRIX = 0x4000,
  XS_CAP_NLT_Q = 0x2000,
  XS_CAP_NLT_E = 0x1000,
  XS_CAP_SY = 0x0800,
  XS_CAP_SD = 0x0400,
  XS_CAP_MLS = 0x0200,
  XS_CAP_RAW_PER_PKT = 0x0080,
  XS_CAP_FRAMEBUFFER = 0x0040,
};
typedef enum xs_cap_e xs_cap_t;

enum xs_nlt_e
{
  XS_NLT_NONE = 0,
  XS_NLT_QUADRATIC = 1,
  XS_NLT_EXTENDED = 2,
};
typedef enum xs_nlt_e xs_nlt_t;

union xs_nlt_parameters_t
{
  struct
  {
    uint16_t sigma : 1;
    uint16_t alpha : 15;
  } quadratic;
  struct
  {
    uint32_t T1;
    uint32_t T2;
    uint8_t E;
  } extended;
};
typedef union xs_nlt_parameters_t xs_nlt_parameters_t;

enum xs_tetrix_e
{
  XS_TETRIX_FULL = 0,
  XS_TETRIX_INLINE = 3,
};
typedef enum xs_tetrix_e xs_tetrix_t;

struct xs_cts_parameters_t
{
  xs_tetrix_t Cf;
  uint8_t e1;
  uint8_t e2;
};
typedef struct xs_cts_parameters_t xs_cts_parameters_t;

enum xs_crg_pattern_e
{
  XS_CFA_RGGB,
  XS_CFA_BGGR,
  XS_CFA_GRBG,
  XS_CFA_GBRG,
};
typedef enum xs_crg_pattern_e xs_cfa_pattern_t;
// This define is only added for readability of code (like profile definitions) where CFA is not allowed/relevant.
#define XS_CFA_NONE XS_CFA_RGGB

struct xs_tpc_parameters_t
{
  uint8_t S_i;  // always 8 -- TDC selection size in number of code groups
  int8_t Qbi;  // [-8;7] -- signaled in sigma-alpha notation
  int8_t Qbr;  // [-8;7] -- signaled in sigma-alpha notation
  uint8_t Yh[MAX_NBANDS + 1];  // positional hash value (same length as Sh, 0xff is a valid value)
  uint8_t Sh[MAX_NBANDS + 1];  // hash mask exponent, 0xff-terminated (0 to indicate -inf of the standard, 1 to 7 is valid)
  bool enabled;  // indicates if TPC marker is (to be) present (in codestream)
};
typedef struct xs_tpc_parameters_t xs_tpc_parameters_t;

struct xs_config_parameters_t
{
  // Refer to xs_config.c when making any changes to this struct!
  xs_cpih_t color_transform;
  uint16_t Cw; // column width
  uint16_t slice_height; // slice height in lines
  uint8_t N_g; // = 4
  uint8_t S_s; // = 8
  uint8_t Bw;
  uint8_t Fq; // 8, 6, or 0
  uint8_t B_r; // = 4 or 5
  uint8_t Fslc; // = 0
  uint8_t Ppoc; // = 0
  int8_t NLx; // = 1 to 8
  int8_t NLy; // = maxi(log2(sy[i])) to min(NLx, 6)
  uint8_t Lh; // long precinct header enforcement flag (0 or 1)
  uint8_t Rl; // raw-mode selection per packet flag (0 or 1)
  uint8_t Qpih; // = 0 (dead-zone), or 1 (uniform)
  uint8_t Fs; // sign handling strategy, = 0 (jointly), or 1 (separate)
  uint8_t Rm; // run mode, = 0 (zero prediction residuals), or 1 (zero coefficients)
  // CWD (optional)
  int8_t Sd; // wavelet decomposition supression
  // WGT
  uint8_t lvl_gain[2][MAX_NBANDS + 1]; // 0xff-terminated
  uint8_t lvl_prio[2][MAX_NBANDS + 1]; // 0xff-terminated
  // NLT (optional)
  xs_nlt_t Tnlt;
  xs_nlt_parameters_t Tnlt_params;
  // CTS (when Cpih is TETRIX)
  xs_cts_parameters_t tetrix_params;
  // CRG (when Cpih is TETRIX)
  xs_cfa_pattern_t cfa_pattern;
  // TPC (when using TDC profile)
  xs_tpc_parameters_t tpc;
};
typedef struct xs_config_parameters_t xs_config_parameters_t;

struct xs_config_t
{
  size_t bitstream_size_in_bytes; // target size of complete codestream in bytes, (size_t)-1 is used for MLS (infinite budget)
  float ra_budget_lines; // used for rate allocation
  uint8_t tdc_enc_refresh_cycle; // a max number of frames without a forced refresh

  int verbose; // higher is more
  xs_gains_mode_t gains_mode; // not really an XS option
  bool refresh_gains_explicit; // not really an XS option, if not explicit, it will be derived from normal gains

  xs_profile_t profile;
  xs_level_t level;
  xs_sublevel_t sublevel;
  xs_fbblevel_t fbblevel;
  xs_cap_t cap_bits;

  bool use_long_precinct_headers;  // implicit calculated option
  bool use_tdc_mode;  // implicit calculated option

  xs_config_parameters_t p;
};
typedef struct xs_config_t xs_config_t;

struct xs_buffer_model_parameters_t
{
  int Nbpp;
  int Nsbu;
  int Ssbo;
  int Wcmax;
  int Ssbu;
  int N_g;
};
typedef struct xs_buffer_model_parameters_t xs_buffer_model_parameters_t;

struct xs_image_t
{
  int width;
  int height;
  int8_t ncomps;
  int8_t sx[MAX_NCOMPS];
  int8_t sy[MAX_NCOMPS];
  int8_t depth;
  xs_data_in_t* comps_array[MAX_NCOMPS];
};
typedef struct xs_image_t xs_image_t;

#ifdef __cplusplus
extern "C"
{
#endif

  JXS_SHARED_LIB_API bool xs_allocate_image(xs_image_t* ptr, const bool set_zero);
  JXS_SHARED_LIB_API void xs_free_image(xs_image_t* ptr);

  typedef struct xs_enc_context_t xs_enc_context_t;

  JXS_SHARED_LIB_API xs_enc_context_t* xs_enc_init(xs_config_t* xs_config, xs_image_t* image);
  JXS_SHARED_LIB_API void xs_enc_close(xs_enc_context_t* ctx);
  JXS_SHARED_LIB_API bool xs_enc_image(xs_enc_context_t* ctx, const uint32_t frame_num, xs_image_t* image, uint8_t* bitstream_buf, size_t bitstream_buf_byte_size, size_t* bitstream_byte_size);
  JXS_SHARED_LIB_API bool xs_enc_preprocess_image(const xs_config_t* xs_config, xs_image_t* image);

  typedef struct xs_dec_context_t xs_dec_context_t;
#ifdef CONFORMANCE_ENABLE_GENERATE_CSV_CODE
  typedef void (*xs_fragment_info_cb_t)(void* context, const int f_id, const int f_Sbits, const int f_Ncg, const int f_padding_bits);
  typedef void (*xs_fbc_bpc_info_cb_t)(void* context, const int bp_count);

  JXS_SHARED_LIB_API bool xs_dec_set_fragment_info_cb(xs_dec_context_t* ctx, xs_fragment_info_cb_t ficb, void* fictx);
  JXS_SHARED_LIB_API bool xs_dec_set_fbc_bpc_info_cb(xs_dec_context_t* ctx, xs_fbc_bpc_info_cb_t fbcb, void* fbctx);
#endif

  JXS_SHARED_LIB_API bool xs_dec_probe(uint8_t* bitstream_buf, size_t codestream_size, xs_config_t* xs_config, xs_image_t* image);
  JXS_SHARED_LIB_API xs_dec_context_t* xs_dec_init(xs_config_t* xs_config, xs_image_t* image);
  JXS_SHARED_LIB_API void xs_dec_close(xs_dec_context_t* ctx);
  JXS_SHARED_LIB_API bool xs_dec_bitstream(xs_dec_context_t* ctx, uint32_t frame_num, uint8_t* bitstream_buf, size_t bitstream_buf_byte_size, xs_image_t* image_out);
  JXS_SHARED_LIB_API bool xs_dec_postprocess_image(const xs_config_t* xs_config, xs_image_t* image_out);

  JXS_SHARED_LIB_API bool xs_config_parse_and_init(xs_config_t* cfg, const xs_image_t* im, const char* config_str, const size_t config_str_max_len);
  JXS_SHARED_LIB_API bool xs_config_dump(const xs_config_t* cfg, const xs_image_t* im, char* config_str, const size_t config_str_max_len, const int details);
  JXS_SHARED_LIB_API bool xs_config_validate(const xs_config_t* cfg, const xs_image_t* im);
  JXS_SHARED_LIB_API bool xs_config_retrieve_buffer_model_parameters(const xs_config_t* cfg, xs_buffer_model_parameters_t* bmp);
  JXS_SHARED_LIB_API void xs_config_nlt_extended_auto_thresholds(xs_config_t* cfg, const uint8_t bpp, const xs_data_in_t th1, const xs_data_in_t th2);
  JXS_SHARED_LIB_API char* xs_get_version_str();
  JXS_SHARED_LIB_API int* xs_get_version();

#ifdef __cplusplus
}
#endif

#endif
