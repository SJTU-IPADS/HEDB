#pragma once

char *toBase64(const unsigned char *pIn, int nbIn, char *pOut);
unsigned char *fromBase64(char *pIn, int ncIn, unsigned char *pOut);
