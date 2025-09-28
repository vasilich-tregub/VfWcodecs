#include "RLEcodec.h"

// credited to https://rosettacode.org/wiki/Run-length_encoding/C

int rle_encode(char* out, const char* in, int l)
{
	int dl, i;
	char cp, c;

	for (cp = c = *in++, i = 0, dl = 0; l > 0; c = *in++, l--) {
		if (c == cp) {
			i++;
			if (i > 255) {
				*out++ = 255;
				*out++ = c; dl += 2;
				i = 1;
			}
		}
		else {
			*out++ = i;
			*out++ = cp; dl += 2;
			i = 1;
		}
		cp = c;
	}
	*out++ = i; *out++ = cp; dl += 2;
	return dl;
}

int rle_decode(char* out, const char* in, int l)
{
	int i, tb;
	char c;

	for (tb = 0; l > 0; l -= 2) {
		i = *in++;
		c = *in++;
		tb += i;
		while (i-- > 0) *out++ = c;
	}
	//if (tb < 0) tb = 201; // what if zero?
	return tb;
}

int rle_compress(LPVOID dst, size_t* outsz, LPVOID src, size_t insz)
{
	*outsz = rle_encode(dst, src, (int)insz);
	return 0;
}

int rle_decompress(LPVOID dst, size_t* outsz, LPVOID src, size_t insz)
{
	*outsz = rle_decode(dst, src, (int)insz);
	return 0;
}
