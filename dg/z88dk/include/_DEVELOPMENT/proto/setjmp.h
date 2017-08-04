include(__link__.m4)

#ifndef _SETJMP_H
#define _SETJMP_H

#define setjmp(env)         l_setjmp(&(env))
#define longjmp(env, val)   l_longjmp(&(env), val)

#ifdef __SDCC

typedef struct
{

   void *ix;
   void *sp;
   void *pc;

} jmp_buf;

#else

typedef struct
{

   void *sp;
   void *pc;

} jmp_buf;

#endif

// must not use callee or fastcall linkage

__OPROTO(int,,l_setjmp,jmp_buf *env)
__OPROTO(void,,l_longjmp,jmp_buf *env,int val)

#endif
