/* $OpenXM: OpenXM/doc/oxlib/test.c,v 1.1 2002/02/25 07:09:01 noro Exp $ */
#include <asir/ox.h>

main() {
	char ibuf[BUFSIZ];
	char *obuf;
	int len,len0;

	asir_ox_init(1);  /* Use the network byte order */

	len0 = BUFSIZ;
	obuf = (char *)malloc(len0);
	while ( 1 ) {
		fgets(ibuf,BUFSIZ,stdin);
		if ( !strncmp(ibuf,"bye",3) )
			exit(0);
		asir_ox_execute_string(ibuf);
		len = asir_ox_peek_cmo_string_length();
		if ( len > len0 ) {
			len0 = len;
			obuf = (char *)realloc(obuf,len0);
		}
		asir_ox_pop_string(obuf,len0);
		printf("%s\n",obuf);
	}
}

