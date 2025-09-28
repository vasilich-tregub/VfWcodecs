#pragma once
#include <Windows.h>
#include <vfw.h>
#include <stdlib.h>
#include <stdio.h>

LRESULT WINAPI DriverProc(DWORD_PTR dwDriverID, HDRVR hDriver, UINT uiMessage, LPARAM lParam1, LPARAM lParam2);

LRESULT svtjxsvfw_compress_query(PBITMAPINFO bin, PBITMAPINFO bout);
LRESULT svtjxsvfw_compress_get_format(PBITMAPINFO bin, PBITMAPINFO bout);
LRESULT svtjxsvfw_compress_max_size(PBITMAPINFO bin, PBITMAPINFO bout);
LRESULT svtjxsvfw_compress_begin(PBITMAPINFO bin, PBITMAPINFO bout);
LRESULT svtjxsvfw_compress_frames_info(ICCOMPRESSFRAMES* icc, size_t* iccsz);
LRESULT svtjxsvfw_compress(ICCOMPRESS* icc, size_t iccsz);
LRESULT svtjxsvfw_compress_end();
LRESULT svtjxsvfw_decompress_query(PBITMAPINFO bin, PBITMAPINFO bout);
LRESULT svtjxsvfw_decompress_get_format(PBITMAPINFO bin, PBITMAPINFO bout);
LRESULT svtjxsvfw_decompress_max_size(PBITMAPINFO bin, PBITMAPINFO bout);
LRESULT svtjxsvfw_decompress_begin(PBITMAPINFO bin, PBITMAPINFO bout);
LRESULT svtjxsvfw_decompress(ICDECOMPRESS* icc, size_t iccsz);
LRESULT svtjxsvfw_decompress_end();

int jxs_compress(LPVOID, size_t*, LPVOID, size_t);
int jxs_decompress(LPVOID, size_t*, LPVOID, size_t);
