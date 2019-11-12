#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* barrier before snprintf */
#define snzprintf(dst, fmt, data...) do { \
        size_t sz = sizeof(dst); \
        memset(dst, 0, sz); \
        snprintf(dst, (sz-1), fmt, data); \
    } while(0)

/* advance strstr string */
#define strstr_adv(dst, str, substr) \
        dst = strstr((str), (substr)); \
        if (dst) \
            dst += strlen(substr);

/* remove '\r', '\n', '\0', '\t' from start of the line */
#define remove_prefix_quotes(s) \
       while((*s) && ((*s) == ',' || \
            (*s) == ')' || (*s) == 0x0d || \
            (*s) == 0x0a || (*s) == 0x00)) { \
            s++; \
       }

char *str_to_lower(char *str)
{
    char *p;

    for(p = str; *p; ++p) {
        *p = (*p >='A' && *p <= 'Z') ? *p | 0x60 : *p;
    }

    return p;
}

char *str_to_upper(char *str)
{
    char *p;

    for(p = str; *p; ++p) {
        *p = (*p >='a' && *p <= 'z') ? *p & 0xDF: *p;
    }

    return p;
}

int validate_prefix(const char *str, const char *prefix)
{
    for(; *str != '\0' && *prefix != '\0'; str++, prefix++) {
        if(*str != *prefix) {
            return 0;
        }
    }

    return *prefix == '\0';
}

size_t grab_string(const unsigned char *s, unsigned char **output)
{
    size_t size = 4, pos = 0;
    bool is_quote = false;

    if(NULL == s) {
        return 0;
    }

    *output = (unsigned char *) ln_zalloc(size);
    if(NULL == *output) {
        return 0;
    }

    while (is_quote ||              /* delimiter*/
            (  *s != ','
               && *s != ')'
               && *s != 0x0d        /* '\r' */
               && *s != 0x0a        /* '\n' */
               && *s != 0x09        /* '\t' */
               && *s != 0x00)) {    /* '\0' */

        /*Check for Quotes*/
        if(*s == '"') {
            is_quote = !is_quote;
        }

        /*Allocate space for trail zero*/
        if(pos + 2 > 0) {
            size += 10;
            *output = (unsigned char *) realloc(*output, size);
            if(NULL == *output) {
                return 0;
            }
        }
        /*Copy to output*/
        (*output)[pos] = *s;
        pos++;
        s++;
    }

    (*output)[pos] = 0;

    /* Strip Quotes '"' */
    if((*output)[0] == '"' && (*output)[pos - 1]) {
        memmove(*output, (*output) + 1, pos - 2);
        (*output)[pos - 2] = 0;
    }

    return pos;
}

