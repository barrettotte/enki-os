#ifndef IO_H
#define IO_H

unsigned char inpb(unsigned short port);
unsigned short inpw(unsigned short port);

void outb(unsigned short port, unsigned char val);
void outw(unsigned short port, unsigned short val);

#endif