#ifndef HANDLERS
#define HANDLERS
// #include "../message/message.h"
#include "../multiplexer/multiplexer.h"
// #include "../queue/queue.h"
#include "../shared/consts.h"
#include "../shared/utils.h"

#include <stdio.h>

// for uint type
#include <stdint.h>
// for htons and other methods of the same family
#include <arpa/inet.h>
// For string helper methods
#include <string.h>

#include "../message/message.h"
#include "wire.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static char*  ROOT_DIR = "./"; 

// https://www.ibm.com/support/knowledgecenter/en/SSVSD8_8.4.1/com.ibm.websphere.dtx.dsgnstud.doc/references/r_design_studio_intro_Hex_Decimal_and_Symbol_Values.htm

// RequestHandler - this is the main method
// that reads messages from queue and based on
// their protocol, it would redirect them to the approporiate
// handler
void *ServerRequestHandler(void *arg);

void EchoProtocolSendRequestToServer(int socket);
void EchoProtocolServerHandler(int socket, Message message);
void DownloadProtocolSendRequestToServer(int socket);
void DownloadProtocolServerHandler(int socket, Message message);
void UploadProtocolSendRequestToServer(int socket);
void UploadProtocolServerHandler(int socket, Message message);
void ChangeDirectoryProtocolSendRequestToServer(int socket);
void ChangeDirectoryProtocolServerHandler(int socket, Message message);
void ListDirectoryProtocolSendRequestToServer(int socket);
void ListDirectoryProtocolServerHandler(int socket, Message message);
#endif