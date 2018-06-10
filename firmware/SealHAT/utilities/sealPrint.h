/*
 * SerialPrint.h
 *
 * Created: 3/7/2018 2:39:48 PM
 *  Author: eslatter
 */


#ifndef SEALPRINT_H_
#define SEALPRINT_H_

#include "driver_init.h"	/* where the IO functions live */
#include <stdio.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include "seal_USB.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 *
 */
size_t printFloat(double number, uint8_t digits);

size_t print(char* string);

size_t println(char* string);

int ftostr(double number, uint8_t digits, char* buff, const int LEN);

int sprintu(char* s, unsigned u);

int sprintf(char* s, const char *fmt, ...);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* SEALPRINT_H_ */
