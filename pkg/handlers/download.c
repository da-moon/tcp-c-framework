
#include "handlers.h"

void DownloadProtocolSendRequestToServer(int socket) {
  printf("Enter File Name for download\n");
  char input[MAX_BUFFER];
  fgets(input, MAX_BUFFER - 1, stdin);
  char *arr_ptr = &input[0];
  arr_ptr[strlen(arr_ptr) - 1] = '\0';
  char *request = malloc(strlen(arr_ptr) + PROTOCOL_HEADER_LEN);
  int mesg_length = MarshallMessage(request, 0xC0DE, DOWNLOAD_REQUEST, arr_ptr);
  Message message;
  message.body = (char *)(request + PROTOCOL_HEADER_LEN);

  if (send(socket, request, strlen(arr_ptr) + PROTOCOL_HEADER_LEN, 0) == -1)
    perror("write failed: ");
  fprintf(stderr,
          "[DEBUG] client : sending download request for file %s to server\n",
          message.body);
}
void DownloadProtocolServerHandler(int socket, Message message) {
  char *payload = malloc(MAX_BUFFER);
  uint16_t protocol;

  FILE *fp = fopen(message.body, "r");
  if (fp == NULL) {
    memset(payload, 0, sizeof(payload));

    strcpy(payload, "file not found");
    protocol = ERROR_MESSAGE;
  } else {
    /* Go to the end of the file. */
    if (fseek(fp, 0L, SEEK_END) == 0) {
      /* Get the size of the file. */
      long bufsize = ftell(fp);
      if (bufsize == -1) {
        memset(payload, 0, sizeof(payload));

        strcpy(payload, "could not get file size");
        protocol = ERROR_MESSAGE;
      } else {

        /* Allocate our buffer to that size. */
        payload = malloc(sizeof(char) * (bufsize + 1));

        /* Go back to the start of the file. */
        if (fseek(fp, 0L, SEEK_SET) != 0) {
          memset(payload, 0, sizeof(payload));

          strcpy(payload, "could not go to file start");
          protocol = ERROR_MESSAGE;
        } else {
          /* Read the entire file into memory. */
          size_t newLen = fread(payload, sizeof(char), bufsize, fp);
          if (ferror(fp) != 0) {
            memset(payload, 0, sizeof(payload));

            strcpy(payload, "Error reading file");
            protocol = ERROR_MESSAGE;
          } else {
            payload[newLen++] = '\0'; /* Just to be safe. */
          }
        }
      }
    }
    fclose(fp);
  }
  char *arr_ptr = &payload[0];
  int payload_length = strlen(arr_ptr);
  // fprintf(stderr, "file Content  %s\n", arr_ptr);
  if (payload_length <= MAX_BUFFER) {
    char *reply = malloc(strlen(arr_ptr) + PROTOCOL_HEADER_LEN);
    int mesg_length = MarshallMessage(reply, 0xC0DE, FILE_REPLY, arr_ptr);
    if (send(socket, reply, payload_length + PROTOCOL_HEADER_LEN, 0) == -1)
      perror("write failed: ");
    fprintf(stderr, "[DEBUG] Download Handler Server : Replying back .... \n");
  } else {
    while (payload_length > 0) {
      char *reply = malloc(strlen(arr_ptr) + PROTOCOL_HEADER_LEN);
      int mesg_length = MarshallMessage(reply, 0xC0DE, FILE_REPLY, arr_ptr);
      int sent = send(socket, reply, payload_length + PROTOCOL_HEADER_LEN, 0);
      if (sent == -1)
        perror("write failed: ");
      fprintf(stderr,
              "[DEBUG] Download Handler Server : Replying back .... \n");
      payload_length = payload_length - sent;
    }
  }
}
