#ifndef SRV_SERIAL_STDIO_H
#define SRV_SERIAL_STDIO_H

#include <stdio.h>

void srvSerialSetup();
int srvSerialGetChar(FILE *f);
int srvSerialPutChar(char c, FILE *f);

#endif //SRV_SERIAL_STDIO_H