#define WAVPRG_MAJOR 4
#define WAVPRG_MINOR 2
#define WAVPRG_REV 1

#define STR(x) #x
#define MACROSTR(x) STR(x)
#if WAVPRG_REV > 0
#define WAVPRG_VERSION MACROSTR(WAVPRG_MAJOR) "." MACROSTR(WAVPRG_MINOR) "." MACROSTR(WAVPRG_REV)
#else
#define WAVPRG_VERSION MACROSTR(WAVPRG_MAJOR) "." MACROSTR(WAVPRG_MINOR)
#endif
