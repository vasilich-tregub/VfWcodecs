#pragma once
#include <Windows.h>
#include <vfw.h>
#include <stdlib.h>
#include <stdio.h>

LRESULT WINAPI DriverProc(DWORD_PTR dwDriverID, HDRVR hDriver, UINT uiMessage, LPARAM lParam1, LPARAM lParam2);

LRESULT jxsvfw_compress_query(PBITMAPINFO bin, PBITMAPINFO bout);
LRESULT jxsvfw_compress_get_format(PBITMAPINFO bin, PBITMAPINFO bout);
LRESULT jxsvfw_compress_max_size(PBITMAPINFO bin, PBITMAPINFO bout);
LRESULT jxsvfw_compress_begin(PBITMAPINFO bin, PBITMAPINFO bout);
LRESULT jxsvfw_compress_frames_info(ICCOMPRESSFRAMES* icc, size_t* iccsz);
LRESULT jxsvfw_compress(ICCOMPRESS* icc, size_t iccsz);
LRESULT jxsvfw_compress_end();
LRESULT jxsvfw_decompress_query(PBITMAPINFO bin, PBITMAPINFO bout);
LRESULT jxsvfw_decompress_get_format(PBITMAPINFO bin, PBITMAPINFO bout);
LRESULT jxsvfw_decompress_max_size(PBITMAPINFO bin, PBITMAPINFO bout);
LRESULT jxsvfw_decompress_begin(PBITMAPINFO bin, PBITMAPINFO bout);
LRESULT jxsvfw_decompress(ICDECOMPRESS* icc, size_t iccsz);
LRESULT jxsvfw_decompress_end();

int jxs_compress(LPVOID, size_t*, LPVOID, size_t);
int jxs_decompress(LPVOID, size_t*, LPVOID, size_t);
