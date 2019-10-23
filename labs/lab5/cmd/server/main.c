/* time_server.c - main */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>

#define Err1 "PDU type not recognized"
#define Err2 "File not found"
struct stat filestat;

struct pdu
{
	char type;
	char data[100];
} 	rpdu, tpdu;

/*------------------------------------------------------------------------
 * main - Iterative UDP server for TIME service
 *------------------------------------------------------------------------
 */
int
main(int argc, char *argv[])
{
	struct sockaddr_in fsin;	/* the from address of a client	*/
	char	*service = "3000";	/* service name or port number	*/
	char	buf[100];		/* "input" buffer; any size > 0	*/
	char    *pts;
	int	sock,n;			/* server socket		*/
	time_t	now;			/* current time			*/
	int	alen;			/* from-address length		*/
        struct sockaddr_in sin; /* an Internet endpoint address         */
        int     s, type,size;        /* socket descriptor and socket type    */
	u_short	portbase = 0;	/* port base, for non-root servers	*/



	switch (argc) {
	case	1:
		break;
	case	2:
		service = argv[1];
		break;
	default:
		fprintf(stderr, "usage: time_server [host [port]]\n");

	}


        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = INADDR_ANY;

   /* Map service name to port number */
        sin.sin_port = htons((u_short)atoi(service));

    /* Allocate a socket */
        s = socket(AF_INET, SOCK_DGRAM, 0);
        if (s < 0){
		fprintf(stderr, "can't creat socket\n");
		exit(1);
	}

    /* Bind the socket */
        if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		fprintf(stderr, "can't bind to %s port\n", service);

	while(1)
	{
	    alen = sizeof(fsin);
	    recvfrom(s, &rpdu, 101, 0, (struct sockaddr *)&fsin, &alen);// read a pdu from the client

            if(rpdu.type != 'C')
	    {
                fprintf(stdout,"PDU type not recognized\n");
		tpdu.type='E';
		strcpy(tpdu.data,Err1);
		sendto(s, &tpdu, 101, 0, (struct sockaddr *)&fsin, sizeof(fsin));
		continue;
	    }
	    else
	    {
	        fprintf(stdout, "Client requested file: %s\n", rpdu.data);
                int fd = open(rpdu.data, O_RDONLY);
		if(fd <0) //Cannot open the file
		{
                    fprintf(stdout, "File not found. Sending error message to client\n");
	            tpdu.type = 'E';
		    strcpy(tpdu.data, Err2);
		    sendto(s, &tpdu, 101, 0, (struct sockaddr *)&fsin, sizeof(fsin));
                    memset(&tpdu, 0, sizeof(tpdu));
                    memset(&rpdu, 0, sizeof(rpdu));
                    continue;
		}
	       fstat(fd, &filestat);
	       size = filestat.st_size;
	       tpdu.type = 'D';
	       while(1)
		{
		    n=read(fd,tpdu.data,100);
		    size = size-n;
                    if(size == 0)//final pdu
			{
			tpdu.type = 'F';
			sendto(s, &tpdu, 101, 0, (struct sockaddr *)&fsin, sizeof(fsin));
			break;
			}
		    else
                    {
			sendto(s, &tpdu, 101, 0, (struct sockaddr *)&fsin, sizeof(fsin));
                        memset(&tpdu, 0, sizeof(tpdu));
                        memset(&rpdu, 0, sizeof(rpdu));
                    }
                }
	    }
        }
    }
