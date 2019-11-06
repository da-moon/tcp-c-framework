#include "handlers.h"
void UploadProtocolSendRequestToServer(int socket)
{
  printf("Enter File Name for upload\n");
  char input[MAX_BUFFER];
  fgets(input, MAX_BUFFER - 1, stdin);
  char *arr_ptr = &input[0];
  arr_ptr[strlen(arr_ptr) - 1] = '\0';
  char *request = malloc(strlen(arr_ptr) + PROTOCOL_HEADER_LEN);
  int mesg_length = MarshallMessage(request, 0xC0DE, UPLOAD_REQUEST, arr_ptr);
  Message message;
  message.body = (char *)(request + PROTOCOL_HEADER_LEN);
}

void UploadProtocolServerHandler(int socket, Message message)
{
  fprintf(stderr, "[ File Upload ] : [ %s ]", message.body);
  // the following would store the file ...
  FILE *fp;
  // sscanf(file_count, "./fixture/client/recieved", buf);
  fp = fopen("./fixture/server/recieved", "w+");
  fprintf(fp, "%s\n", message.body);
  fclose(fp);
}
