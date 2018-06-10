#include "sealPrint.h"

size_t printFloat(double number, uint8_t digits)
{
	static const int OUTPUT_SIZE = 12;
	char output[OUTPUT_SIZE];
	size_t n = 0;

    n = ftostr(number, digits, output, OUTPUT_SIZE);

	usb_write((uint8_t*)output, n);
	return n;
}

size_t print(char* string)
{
    int32_t err;
    int32_t len = strlen(string);

    err = usb_write(string, len);
    
    return (ERR_NONE == err ? len : err);
}

size_t println(char* string)
{
    int32_t err;
    int32_t len = strlen(string);

    err = usb_write(string, len);
    if(!err) {
        delay_ms(5);
        err = usb_write("\n", 1);
        len++;
    }

    return (ERR_NONE == err ? len : err);
}

int ftostr(double number, uint8_t digits, char* buff, const int LEN)
{
    uint8_t i;
    size_t n = 0;

    // Not a number, special floating point value
    if (isnan(number)) {
        n = snprintf(buff, LEN, "nan");
    }
    // infinity, special floating point value
    else if (isinf(number)) {
        n = snprintf(buff, LEN, "inf");
    }
    // constant determined empirically
    else if (number > 4294967040.0) {
        n = snprintf(buff, LEN, "ovf");
    }
    // constant determined empirically
    else if (number <-4294967040.0) {
        n = snprintf(buff, LEN, "ovf");
    }
    // A valid floating point value
    else {
        // Handle negative numbers
        if (number < 0.0) {
            buff[n++] = '-';
            number = -number;
        }

        // Round correctly so that print(1.999, 2) prints as "2.00"
        double rounding = 0.5;
        for (i = 0; i < digits; i++) {
            rounding /= 10.0;
        }
        number += rounding;

        // Extract the integer part of the number and print it
        unsigned long int_part = (unsigned long)number;
        double remainder = number - (double)int_part;
        n += snprintf(&buff[n], LEN-n, "%lu", int_part);

        // Print the decimal point, but only if there are digits beyond
        if (digits > 0) {
            buff[n] = '.';
            buff[++n] = '\0';

            // Extract digits from the remainder one at a time
            while (digits-- > 0) {
                // calculate the current digit
                remainder *= 10.0;
                unsigned int toPrint = (unsigned int)remainder;

                // overwrite the last null terminator with the current digit
                n += snprintf(&buff[n], LEN-n, "%u", toPrint);

                // shift to the next digit
                remainder -= toPrint;
            }
        }
    } // VALID FLOAT BLOCK

    return n;
}

#define _SPRINTF_OVERRIDE 1
#if _SPRINTF_OVERRIDE
/* Override sprintf implement to optimize */

static const unsigned m_val[] = {1000000000u, 100000000u, 10000000u, 1000000u, 100000u, 10000u, 1000u, 100u, 10u, 1u};
int sprintu(char *s, unsigned u)
{
    char tmp_buf[12];
    int  i, n = 0;
    int  m;

    if (u == 0) {
        *s = '0';
        return 1;
    }

    for (i = 0; i < 10; i++) {
        for (m = 0; m < 10; m++) {
            if (u >= m_val[i]) {
                u -= m_val[i];
                } else {
                break;
            }
        }
        tmp_buf[i] = m + '0';
    }
    for (i = 0; i < 10; i++) {
        if (tmp_buf[i] != '0') {
            break;
        }
    }
    for (; i < 10; i++) {
        *s++ = tmp_buf[i];
        n++;
    }
    return n;
}

int sprintf(char *s, const char *fmt, ...)
{
    int     n = 0;
    va_list ap;
    va_start(ap, fmt);
    while (*fmt) {
        if (*fmt != '%') {
            *s = *fmt;
            s++;
            fmt++;
            n++;
            } else {
            fmt++;
            switch (*fmt) {
                case 'c': {
                    char valch = va_arg(ap, int);
                    *s         = valch;
                    s++;
                    fmt++;
                    n++;
                    break;
                }
                case 'd': {
                    int vali = va_arg(ap, int);
                    int nc;

                    if (vali < 0) {
                        *s++ = '-';
                        n++;
                        nc = sprintu(s, -vali);
                        } else {
                        nc = sprintu(s, vali);
                    }

                    s += nc;
                    n += nc;
                    fmt++;
                    break;
                }
                case 'u': {
                    unsigned valu = va_arg(ap, unsigned);
                    int      nc   = sprintu(s, valu);
                    n += nc;
                    s += nc;
                    fmt++;
                    break;
                }
                case 's': {
                    char *vals = va_arg(ap, char *);
                    while (*vals) {
                        *s = *vals;
                        s++;
                        vals++;
                        n++;
                    }
                    fmt++;
                    break;
                }
                default:
                *s = *fmt;
                s++;
                fmt++;
                n++;
            }
        }
    }
    va_end(ap);
    *s = 0;
    return n;
}
#endif /* _SPRINTF_OVERRIDE */
