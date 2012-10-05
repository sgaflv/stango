#include "messaging.h"


char inbuf[BUF_SIZE];
char command[BUF_SIZE];
char *argument;

char outbuf[BUF_SIZE];


static FILE *log_file=NULL;

static void dolog(char *str) {
	if (!log_file) {
		log_file = fopen("sam.log", "w");
	}
	
	if (!log_file) {
		printf("could not open log file\n");
	}
	
	fprintf(log_file, "%s", str);
	fflush(log_file);
} 

static void inlog() {
	dolog("> ");
	dolog(inbuf);
}

static void outlog(char *mes) {
	dolog("< ");
	dolog(mes);
}

bool get_message() {
	char *r = fgets (inbuf, BUF_SIZE , stdin);
	if (!r) return false;
	inlog();
	for (;*r;r++) {
		if (*r=='\n') *r=0;
		if (*r=='\r') *r=0;
	}
	
	sscanf(inbuf, "%s", command);
	int t = strlen(command);
	argument = &inbuf[t];
	
	return true;
}

void send_message(char *mes) {
	outlog(mes);
	printf("%s", mes);
	fflush(stdout);
}
