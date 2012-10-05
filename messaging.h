#ifndef MESSAGING_H_INCLUDED
#define MESSAGING_H_INCLUDED

#include "common.h"

#define BUF_SIZE 256

extern char inbuf_cnt;
extern char inbuf[BUF_SIZE];
extern char command[BUF_SIZE];
extern char *argument;

extern char outbuf[BUF_SIZE];

extern bool get_message();
extern void send_message(char *mes);


#endif // MESSAGING_H_INCLUDED
