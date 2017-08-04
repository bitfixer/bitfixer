include(__link__.m4)

#ifndef _SYS_IOCTL_H
#define _SYS_IOCTL_H

#include <stdint.h>

// GENERAL FOR ALL DEVICES

#define IOCTL_RESET                    0x0000

// IOCTL FOR CONSOLE_01 INPUT TERMINALS

#define IOCTL_ITERM_ECHO               0xc081
#define IOCTL_ITERM_PASS               0xc041
#define IOCTL_ITERM_LINE               0xc021
#define IOCTL_ITERM_COOK               0xc011
#define IOCTL_ITERM_CAPS               0xc009
#define IOCTL_ITERM_CRLF               0xc101
#define IOCTL_ITERM_CURS               0xc201

#define IOCTL_ITERM_TIE                0x0201
#define IOCTL_ITERM_GET_EDITBUF        0x0381
#define IOCTL_ITERM_SET_EDITBUF        0x0301

#define IOCTL_ITERM_GETF               0x7ff9
#define IOCTL_ITERM_SETF               0xbff9

// IOCTL FOR CONSOLE_01 OUTPUT TERMINALS

#define IOCTL_OTERM_PAGE                  0xc082
#define IOCTL_OTERM_PAUSE                 0xc042
#define IOCTL_OTERM_COOK                  0xc022
#define IOCTL_OTERM_CRLF                  0xc012
#define IOCTL_OTERM_CLEAR                 0xe002
#define IOCTL_OTERM_SIGNAL                0xc202
#define IOCTL_OTERM_BELL                  0xc102

#define IOCTL_OTERM_GETF                  0x7ffa
#define IOCTL_OTERM_SETF                  0xbffa

#define IOCTL_OTERM_CLS                   0x0102
#define IOCTL_OTERM_RESET_SCROLL          0x0202
#define IOCTL_OTERM_GET_WINDOW_COORD      0x0382
#define IOCTL_OTERM_SET_WINDOW_COORD      0x0302
#define IOCTL_OTERM_GET_WINDOW_RECT       0x0482
#define IOCTL_OTERM_SET_WINDOW_RECT       0x0402
#define IOCTL_OTERM_GET_CURSOR_COORD      0x0582
#define IOCTL_OTERM_SET_CURSOR_COORD      0x0502
#define IOCTL_OTERM_GET_OTERM             0x0602
#define IOCTL_OTERM_SCROLL                0x0702
#define IOCTL_OTERM_FONT                  0x0802

#define IOCTL_OTERM_FZX_GET_PAPER_COORD   0x0a82
#define IOCTL_OTERM_FZX_SET_PAPER_COORD   0x0a02
#define IOCTL_OTERM_FZX_GET_PAPER_RECT    0x0b82
#define IOCTL_OTERM_FZX_SET_PAPER_RECT    0x0b02
#define IOCTL_OTERM_FZX_LEFT_MARGIN       0x0c02
#define IOCTL_OTERM_FZX_LINE_SPACING      0x0d02
#define IOCTL_OTERM_FZX_SPACE_EXPAND      0x0e02
#define IOCTL_OTERM_FZX_GET_FZX_STATE     0x0f82
#define IOCTL_OTERM_FZX_SET_FZX_STATE     0x0f02

// IOCTL FOR CHARACTER_00 SERIAL INPUT

#define IOCTL_ICHAR_CRLF                  0xc103

// IOCTL FOR CHARACTER_00 SERIAL OUTPUT

#define IOCTL_OCHAR_CRLF                  0xc014

// PLATFORM IOCTL

#ifdef __SPECTRUM
#include <sys/ioctl_zx.h>
#endif

__VPROTO(int,,ioctl,int fd,uint16_t request,...)
__DPROTO(int,,vioctl,int fd,uint16_t request,void *arg)

#endif
