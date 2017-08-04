#ifndef __CTYPE_H__
#define __CTYPE_H__

#include <sys/compiler.h>


extern int __LIB__ __FASTCALL__ isalnum(int);
extern int __LIB__ __FASTCALL__ isalpha(int);
extern int __LIB__ __FASTCALL__ isascii(int);
extern int __LIB__ __FASTCALL__ isbdigit(int);
extern int __LIB__ __FASTCALL__ iscntrl(int);
extern int __LIB__ __FASTCALL__ isdigit(int);
extern int __LIB__ __FASTCALL__ isgraph(int);
extern int __LIB__ __FASTCALL__ isupper(int);
extern int __LIB__ __FASTCALL__ islower(int);
extern int __LIB__ __FASTCALL__ isodigit(int);
extern int __LIB__ __FASTCALL__ isoob(int);
extern int __LIB__ __FASTCALL__ isprint(int);
extern int __LIB__ __FASTCALL__ ispunct(int);
extern int __LIB__ __FASTCALL__ isspace(int);
extern int __LIB__ __FASTCALL__ isxdigit(int);
extern int __LIB__ __FASTCALL__ toascii(int);
extern int __LIB__ __FASTCALL__ tolower(int);
extern int __LIB__ __FASTCALL__ toupper(int);

#define isblank(a) isspace(a)

#endif
