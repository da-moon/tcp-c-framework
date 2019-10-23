/* time_client.c - main */

#include <sys/types.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <netdb.h>

#define	BUFSIZE 64


/*------------------------------------------------------------------------
 * main - UDP client for TIME service that prints the resulting time
 *------------------------------------------------------------------------
 */

struct pdu
{
 char type;
 char data[100];
} rpdu, tpdu;

int
main(int argc, char *argv[])
{
	char	*host = "localhost";	/* host to use if none supplied	*/
	char	*service = "3000";
	char	now[100];		/* 32-bit integer to hold time	*/
	struct hostent	*phe;	/* pointer to host information entry	*/
	struct sockaddr_in sin;	/* an Internet endpoint address		*/
	int	s, n, type;	/* socket descriptor and socket type	*/
        char buf[100];


	switch (argc) {
	case 1:
		host = "localhost";
		break;
	case 3:
		service = argv[2];
		/* FALL THROUGH */
	case 2:
		host = argv[1];
		break;
	default:
		fprintf(stderr, "usage: UDPtime [host [port]]\n");
		exit(1);
	}

	memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;

    /* Map service name to port number */
        sin.sin_port = htons((u_short)atoi(service));

        if ( phe = gethostbyname(host) ){
                memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
                }
        else if ( (sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE ){
		fprintf(stderr, "Can't get host entry \n");
		exit(1);
	}


    /* Allocate a socket */
        s = socket(PF_INET, SOCK_DGRAM, 0);
        if (s < 0){
		fprintf(stderr, "Can't create socket \n");
		exit(1);
	}


    /* Connect the socket */
        if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0){
		fprintf(stderr, "Can't connect to %s %s \n", host, service);
		exit(1);
	}

    fprintf(stdout, "Enter the name of the file you'd like to receive:\n");
    fgets(buf,100, stdin);// Read from the standard input for a file name to be downloaded
    n = strlen(buf);
    buf[n-1] = '\0';  // Null terminated the character string that represents the filename

    strcpy(tpdu.data,buf);
    fprintf(stdout, "Requesting file: %s\n", tpdu.data);

    tpdu.type= 'C';
    write(s, &tpdu,n+1);           		  // Send the filename PDU to the server
    n=read(s, &rpdu, 101);      		  // read the response from the server
    if(rpdu.type == 'E') // receive an Error PDU
    {
        fprintf(stdout, "Error from server: %s", rpdu.data);
        //write(1, rpdu.data, n-1);
	return;
    }
    else if(rpdu.type == 'D')//receive a Data PDU
    {
        int f = open("receive.txt", O_CREAT | O_RDWR);
        write(f, rpdu.data, n-1); //store the file data or send it to the standard output (terminal)
	while((n=read(s, &rpdu , 101))>0)
	{
	   write(f, buf, n);
	   if(rpdu.type == 'F') return;
	}
        close(f);
    }
    else if(rpdu.type == 'F')	// receive a Final PDU
    {
        fprintf(stdout, "File received\n");
        int f = open("receive.txt", O_CREAT | O_RDWR);
        write(f, rpdu.data, n-1);
        close(f);
        return;
    }
   exit(0);
}
