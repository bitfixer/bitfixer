//
//  main.cpp
//  dgrom
//
//  Created by Michael Hill on 1/16/17.
//  Copyright © 2017 Michael Hill. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

unsigned char program[] =
{
    0xc3, // 0 (0)
    0x33, // 1 (1)
    0x0, // 2 (2)
    0xd2, // 3 (3)
    0xe5, // 4 (4)
    0xe1, // 5 (5)
    0xe4, // 6 (6)
    0xa0, // 7 (7)
    0xc3, // 8 (10)
    0x2, // 9 (11)
    0x1, // a (12)
    0xda, // b (13)
    0xad, // c (14)
    0xb8, // d (15)
    0xb0, // e (16)
    0xa0, // f (17)
    0xc3, // 10 (20)
    0x5, // 11 (21)
    0x1, // 12 (22)
    0xc9, // 13 (23)
    0xce, // 14 (24)
    0xc9, // 15 (25)
    0xd4, // 16 (26)
    0xc9, // 17 (27)
    0xc3, // 18 (30)
    0x8, // 19 (31)
    0x1, // 1a (32)
    0xc1, // 1b (33)
    0xcc, // 1c (34)
    0xc9, // 1d (35)
    0xda, // 1e (36)
    0xc5, // 1f (37)
    0xc3, // 20 (40)
    0xb, // 21 (41)
    0x1, // 22 (42)
    0xa0, // 23 (43)
    0xc3, // 24 (44)
    0xe1, // 25 (45)
    0xf3, // 26 (46)
    0xf3, // 27 (47)
    0xc3, // 28 (50)
    0xe, // 29 (51)
    0x1, // 2a (52)
    0xe5, // 2b (53)
    0xf4, // 2c (54)
    0xf4, // 2d (55)
    0xe5, // 2e (56)
    0xa0, // 2f (57)
    0xc3, // 30 (60)
    0x11, // 31 (61)
    0x1, // 32 (62)
    0x21, // 33 (63)
    0x0, // 34 (64)
    0x1, // 35 (65)
    0x18, // 36 (66)
    0x3, // 37 (67)
    0xc3, // 38 (70)
    0x14, // 39 (71)
    0x1, // 3a (72)
    0x7e, // 3b (73)
    0xfe, // 3c (74)
    0x53, // 3d (75)
    0x20, // 3e (76)
    0x7, // 3f (77)
    0x2c, // 40 (100)
    0x7e, // 41 (101)
    0xfe, // 42 (102)
    0x53, // 43 (103)
    0xca, // 44 (104)
    0x0, // 45 (105)
    0x5, // 46 (106)
    0x31, // 47 (107)
    0x0, // 48 (110)
    0x2, // 49 (111)
    0xcd, // 4a (112)
    0xe6, // 4b (113)
    0x0, // 4c (114)
    0x21, // 4d (115)
    0x3, // 4e (116)
    0x0, // 4f (117)
    0x6, // 50 (120)
    0x6, // 51 (121)
    0xe, // 52 (122)
    0x5, // 53 (123)
    0x7e, // 54 (124)
    0xcd, // 55 (125)
    0xfa, // 56 (126)
    0x0, // 57 (127)
    0x2c, // 58 (130)
    0xd, // 59 (131)
    0x20, // 5a (132)
    0xf8, // 5b (133)
    0x2c, // 5c (134)
    0x2c, // 5d (135)
    0x2c, // 5e (136)
    0x10, // 5f (137)
    0xf1, // 60 (140)
    0x21, // 61 (141)
    0x17, // 62 (142)
    0x1, // 63 (143)
    0x18, // 64 (144)
    0x3, // 65 (145)
    0xc3, // 66 (146)
    0x1d, // 67 (147)
    0x1, // 68 (150)
    0x36, // 69 (151)
    0x1f, // 6a (152)
    0x2c, // 6b (153)
    0x36, // 6c (154)
    0x0, // 6d (155)
    0x2c, // 6e (156)
    0x36, // 6f (157)
    0x1, // 70 (160)
    0x2c, // 71 (161)
    0x36, // 72 (162)
    0xff, // 73 (163)
    0x2c, // 74 (164)
    0x36, // 75 (165)
    0x7, // 76 (166)
    0xed, // 77 (167)
    0x5b, // 78 (170)
    0x18, // 79 (171)
    0x1, // 7a (172)
    
    0xcd, // 7b (173)
    0x9c, // 7c (174)
    0x0, // 7d (175)
    0x6, // 7e (176)
    0x80, // 7f (177)
    0x10, // 80 (200)
    0xfe, // 81 (201)
    0x12, // 82 (202)
    0x1a, // 83 (203)
    0xbc, // 84 (204)
    0x7a, // 85 (205)
    0x28, // 86 (206)
    0x2, // 87 (207)
    0x3e, // 88 (210)
    0xae, // 89 (211)
    0xe6, // 8a (212)
    0x7, // 8b (213)
    0xf6, // 8c (214)
    0xb0, // 8d (215)
    0xcd, // 8e (216)
    0xfa, // 8f (217)
    0x0, // 90 (220)
    0x2a, // 91 (221)
    0x1a, // 92 (222)
    0x1, // 93 (223)
    0x23, // 94 (224)
    0x13, // 95 (225)
    0xed, // 96 (226)
    0x52, // 97 (227)
    0x20, // 98 (230)
    0xe1, // 99 (231)
    0x18, // 9a (232)
    0xa8, // 9b (233)
    
    // start of read byte subroutine
    0x00,	//0x2e, // 9c (234)
    0x00,	//0x8, // 9d (235)
    0x00,	//0x6, // 9e (236)
    0x00,	//0x3, // 9f (237)
    0x00,	//0xdb, // a0 (240)
    0x00,	//0x1, // a1 (241)
    0x00,	//0xcb, // a2 (242)
    0x00,	//0x47, // a3 (243)
    0x00,	//0x20, // a4 (244)
    0x00,	//0xf8, // a5 (245)
    0x00,    //0x10, // a6 (246)
    0x00,    //0xf8, // a7 (247)
    0x00,    //0xcd, // a8 (250)
    0x00,    //0xb4, // a9 (251)
    0x00,    //0x0, // aa (252)
    0x00,    //0x26, // ab (253)
    0x00,    //0x0, // ac (254)
    0x00,    //0xcd, // ad (255)
    0x00,    //0xbb, // ae (256)
    0x00,    //0x0, // af (257)
    0x00,    //0x2d, // b0 (260)
    0x00,    //0x20, // b1 (261)
    0x00,    //0xfa, // b2 (262)
    0x00,    //0xc9, // b3 (263)
    0x00,    //0x3a, // b4 (264)
    0x00,    //0x17, // b5 (265)
    0x00,    //0x1, // b6 (266)
    0x00,    //0xd6, // b7 (267)
    0x00,    //0x6, // b8 (270)
    0x00,    //0x18, // b9 (271)
    0x00,    //0x3, // ba (272)
    0x00,    //0x3a, // bb (273)
    0x00,    //0x17, // bc (274)
    0x00,    //0x1, // bd (275)
    0x00,    //0x47, // be (276)
    0x00,    //0xe, // bf (277)
    0x00,    //0x80, // c0 (300)
    0x00,    //0xdb, // c1 (301)
    0x00,    //0x1, // c2 (302)
    0x00,    //0xcb, // c3 (303)
    0x00,    //0x47, // c4 (304)
    0x00,    //0xc2, // c5 (305)
    0x00,    //0xcc, // c6 (306)
    0x00,    //0x0, // c7 (307)
    0x00,    //0xc, // c8 (310)
    0x00,    //0xc3, // c9 (311)
    0x00,    //0xd0, // ca (312)
    0x00,    //0x0, // cb (313)
    0x00,    //0xd, // cc (314)
    0x00,    //0xc3, // cd (315)
    0x00,    //0xd0, // ce (316)
    0x00,    //0x0, // cf (317)
    0x00,    //0x10, // d0 (320)
    0x00,    //0xef, // d1 (321)
    0x00,    //0xcb, // d2 (322)
    0x00,    //0x79, // d3 (323)
    0x00,    //0xc2, // d4 (324)
    0x00,    //0xdc, // d5 (325)
    0x00,    //0x0, // d6 (326)
    0x00,    //0x3e, // d7 (327)
    0x00,    //0x1, // d8 (330)
    0x00,    //0xc3, // d9 (331)
    0x00,    //0xe1, // da (332)
    0x00,    //0x0, // db (333)
    0x00,    //0x3e, // dc (334)
    0x00,    //0x0, // dd (335)
    0x00,    //0xc3, // de (336)
    0x00,    //0xe1, // df (337)
    0x00,    //0x0, // e0 (340)
    0xdb,    //0x84, // e1 (341) // IN 001
    0x01,    //0xf, // e2 (342)
    0x67, // e3 (343) LD H,A
    0xc9, // e4 (344) RET
    0x0, // e5 (345)
    0x3e, // e6 (346)
    0x7f, // e7 (347)
    0xcd, // e8 (350)
    0xfa, // e9 (351)
    0x0, // ea (352)
    0x6, // eb (353)
    0x0, // ec (354)
    0xe, // ed (355)
    0x4, // ee (356)
    0xcd, // ef (357)
    0xf8, // f0 (360)
    0x0, // f1 (361)
    0xd, // f2 (362)
    0x20, // f3 (363)
    0xfa, // f4 (364)
    0x10, // f5 (365)
    0xf6, // f6 (366)
    0xc9, // f7 (367)
    0x3e, // f8 (370)
    0xa0, // f9 (371)
    0xd3, // fa (372)
    0x0, // fb (373)
    0xaf, // fc (374)
    0xd3, // fd (375)
    0x0, // fe (376)
    0xc9, // ff (377)
};
int program_bytes = 256;

int main(int argc, const char * argv[]) {
    // insert code here...
    for (int i = 0; i < program_bytes; i++)
    {
        unsigned char b = program[i];
        printf("%02X ", b);
    }
    
    return 0;
}
