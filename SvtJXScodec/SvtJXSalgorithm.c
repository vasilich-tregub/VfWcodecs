#include "svtjxs/SvtJpegxsEnc.h"
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SvtJXScodec.h"

int jxs_decode(char* out, const char* in, int l)
{
    return -1;
}

int jxs_decompress(LPVOID dst, size_t* outsz, LPVOID src, size_t insz)
{
    *outsz = jxs_decode(dst, src, (int)insz);
    return 0;
}
