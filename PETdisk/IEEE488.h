/*
    IEEE488.h
    IEEE Routines in the PETdisk storage device
    Copyright (C) 2011 Michael Hill

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
    Contact the author at bitfixer@bitfixer.com
    http://bitfixer.com
*/

#ifndef _IEEE488_H
#define _IEEE488_H

#define IEEE_PORT PORTC
#define IEEE_CTL  DDRC
#define ATN 0x01
#define NDAC 0x02
#define NOT_NDAC 0xFD
#define NRFD 0x08
#define NOT_NRFD 0xF7
#define DAV 0x04
#define EOI 0x10

#define TALK 0x40

#define DATA_PORT PORTD
#define DATA_CTL  DDRD
#define DATA_HI_IN PIND
#define DATA_LO_IN PINB

#define DATA0   0x01
#define DATA1   0x02

#define DATALO  0x03

#define TALK    0x40
#define LISTEN  0x20

void sendIEEEBytes(unsigned char *entry, int size, unsigned char isLast);
unsigned char wait_for_device_address(unsigned char my_address);
void wait_for_dav_high();
void wait_for_dav_low();
void wait_for_atn_high();
void wait_for_atn_low();
void wait_for_nrfd_high();
void wait_for_ndac_high();
void unlisten();
void recv_byte_IEEE(unsigned char *byte);
void ListFilesIEEE(unsigned long firstCluster);
void writeFileFromIEEE ();

#endif