include(__link__.m4)

#ifndef _MATH_H
#define _MATH_H

// THE SELECTED FLOATING POINT PACKAGE MAY NOT SUPPORT ALL LISTED FUNCTIONS

#ifndef _FLOAT_T_DEFINED
#define _FLOAT_T_DEFINED

   #ifdef __SDCC
   
   typedef float float_t;
   
   #else
   
   typedef double float_t;
   
   #endif
   
#endif

#ifndef _DOUBLE_T_DEFINED
#define _DOUBLE_T_DEFINED

   #ifdef __SDCC
   
   typedef float double_t;
   
   #else
   
   typedef double double_t;
   
   #endif
   
#endif

#define FLT_EVAL_METHOD        1
#define MATH_ERRNO             1
#define MATH_ERREXCEPT         2
#define math_errhandling       1

#ifdef __SDCC

#define HUGE_VAL               1.7014117331E+38
#define HUGE_VALF              1.7014117331E+38
#define INFINITY               1.7014117331E+38

#else

#define HUGE_VAL               1.7014118346E+38
#define HUGE_VALF              1.7014118346E+38
#define INFINITY               1.7014118346E+38

#endif

__DPROTO(double_t,,acos,double_t x)
__DPROTO(double_t,,asin,double_t x)
__DPROTO(double_t,,atan,double_t x)
__DPROTO(double_t,,atan2,double_t y,double_t x)

__DPROTO(double_t,,cos,double_t x)
__DPROTO(double_t,,sin,double_t x)
__DPROTO(double_t,,tan,double_t x)

__DPROTO(double_t,,acosh,double_t x)
__DPROTO(double_t,,asinh,double_t x)
__DPROTO(double_t,,atanh,double_t x)

__DPROTO(double_t,,cosh,double_t x)
__DPROTO(double_t,,sinh,double_t x)
__DPROTO(double_t,,tanh,double_t x)

__DPROTO(double_t,,exp,double_t x)
__DPROTO(double_t,,exp2,double_t x)
__DPROTO(double_t,,expm1,double_t x)
__DPROTO(double_t,,frexp,double_t value,int *exp)
__DPROTO(int,,ilogb,double_t x)
__DPROTO(double_t,,ldexp,double_t x,int exp)

__DPROTO(double_t,,log,double_t x)
__DPROTO(double_t,,log10,double_t x)
__DPROTO(double_t,,log1p,double_t x)
__DPROTO(double_t,,log2,double_t x)
__DPROTO(double_t,,logb,double_t x)

__DPROTO(double_t,,scalbn,double_t x,int n)
__DPROTO(double_t,,scalbln,double_t x,int n)

__DPROTO(double_t,,fabs,double_t x)
__DPROTO(double_t,,hypot,double_t x,double_t y)

__DPROTO(double_t,,pow,double_t x,double_t y)
__DPROTO(double_t,,sqrt,double_t x)
__DPROTO(double_t,,cbrt,double_t x)

__DPROTO(double_t,,erf,double_t x)
__DPROTO(double_t,,erfc,double_t x)
__DPROTO(double_t,,lgamma,double_t x)
__DPROTO(double_t,,tgamma,double_t x)

__DPROTO(double_t,,ceil,double_t x)
__DPROTO(double_t,,floor,double_t x)
__DPROTO(double_t,,nearbyint,double_t x)
__DPROTO(double_t,,rint,double_t x)
__DPROTO(long,,lrint,double_t x)
__DPROTO(double_t,,round,double_t x)
__DPROTO(long,,lround,double_t x)
__DPROTO(double_t,,trunc,double_t x)

__DPROTO(double_t,,modf,double_t value,double_t *iptr)
__DPROTO(double_t,,fmod,double_t x,double_t y)
__DPROTO(double_t,,remainder,double_t x,double_t y)
__DPROTO(double_t,,remquo,double_t x,double_t y,int *quo)

__DPROTO(double_t,,copysign,double_t x,double_t y)
__DPROTO(double_t,,nan,const char *tagp)

__DPROTO(double_t,,nextafter,double_t x,double_t y)
__DPROTO(double_t,,nexttoward,double_t x,double_t y)

__DPROTO(double_t,,fdim,double_t x,double_t y)

__DPROTO(double_t,,fmax,double_t x,double_t y)
__DPROTO(double_t,,fmin,double_t x,double_t y)

__DPROTO(double_t,,fma,double_t x,double_t y,double_t z)

__DPROTO(int,,isgreater,double_t x,double_t y)
__DPROTO(int,,isgreaterequal,double_t x,double_t y)
__DPROTO(int,,isless,double_t x,double_t y)
__DPROTO(int,,islessequal,double_t x,double_t y)
__DPROTO(int,,islessgreater,double_t x,double_t y)
__DPROTO(int,,isunordered,double_t x,double_t y)

// NO DISTINCTION BETWEEN FLOAT AND DOUBLE

#define acosf        acos
#define asinf        asin
#define atanf        atan
#define atan2f       atan2

#define cosf         cos
#define sinf         sin
#define tanf         tan

#define acoshf       acosh
#define asinhf       asinh
#define atanhf       atanh

#define coshf        cosh
#define sinhf        sinh
#define tanhf        tanh

#define expf         exp
#define exp2f        exp2
#define expm1f       expm1
#define frexpf       frexp
#define ilogbf       ilogb
#define ldexpf       ldexp

#define logf         log
#define log10f       log10
#define log1pf       log1p
#define log2f        log2
#define logbf        logb

#define scalbnf      scalbn
#define scalblnf     scalbln

#define fabsf        fabs
#define hypotf       hypot

#define powf         pow
#define sqrtf        sqrt
#define cbrtf        cbrt

#define erff         erf
#define erfcf        erfc
#define lgammaf      lgamma
#define tgammaf      tgamma

#define ceilf        ceil
#define floorf       floor
#define nearbyintf   nearbyint
#define rintf        rint
#define lrintf       lrint
#define llrintf      llrint
#define roundf       round
#define lroundf      lround
#define llroundf     llround
#define truncf       trunc

#define modff        modf
#define fmodf        fmod
#define remainderf   remainder
#define remquof      remquo

#define copysignf    copysign
#define nanf         nan

#define nextafterf   nextafter
#define nexttowardf  nexttoward

#define fdimf        fdim

#define fmaxf        fmax
#define fminf        fmin

#define fmaf         fma

#endif
