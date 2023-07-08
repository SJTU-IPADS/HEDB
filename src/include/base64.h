#pragma once

int ToBase64Fast(const unsigned char* pSrc, int nLenSrc, char* pDst, int nLenDst);
int FromBase64Fast(const unsigned char* pSrc, int nLenSrc, char* pDst, int nLenDst);
