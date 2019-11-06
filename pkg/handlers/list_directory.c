#include "handlers.h"

#include <dirent.h>
#include <stdint.h>
#include <string.h>

void ListDirectoryProtocolSendRequestToServer(int socket) {
  printf("Enter Directory name For server list\n");
  char input[MAX_BUFFER];
  fgets(input, MAX_BUFFER - 1, stdin);
  char *arr_ptr = &input[0];
  char *request = malloc(strlen(arr_ptr) + PROTOCOL_HEADER_LEN);
  int mesg_length = MarshallMessage(request, 0xC0DE, LIST_DIR_REQUEST, arr_ptr);
  Message message;
  message.body = (char *)(request + PROTOCOL_HEADER_LEN);

  if (send(socket, request, strlen(arr_ptr) + PROTOCOL_HEADER_LEN, 0) == -1)
    perror("write failed: ");
  fprintf(
      stderr,
      "[DEBUG] client : sending list directory request for file %s to server\n",
      message.body);
}
void ListDirectoryProtocolServerHandler(int socket, Message message) {

  //  dir (pointer) -  used for keeping track of the current directory name.
  DIR *dir;
  char payload[MAX_BUFFER];
  uint16_t protocol;
  char buf[256];
  sscanf(message.body, "%s", buf); // Trimming on both sides occurs here
        
  dir = opendir(buf);
  // If the directory exists.
  if (dir != NULL) {
    struct dirent *ent;
    protocol = LIST_DIR_REPLY;
    // While we are in a directory and there are other directories
    // present.
    while ((ent = readdir(dir)) != NULL) {
      char temp[256];

      // Prints all of the data to the console.
      sscanf(ent->d_name, "%s\n",
             temp); // Trimming on both sides occurs here
      strcat(payload, temp);
      strcat(payload, " | ");
    }
    closedir(dir);
  }
  // If the directory does not exist.
  else if (dir == NULL) {
    memset(payload, 0, sizeof(payload));

    strcpy(payload, "You either typed the path incorrectly or the directory "
                    "does not existn");
    protocol = ERROR_MESSAGE;
    closedir(dir);
  }

  char *arr_ptr = &payload[0];
  int payload_length = strlen(arr_ptr);

  char *reply = malloc(strlen(arr_ptr) + PROTOCOL_HEADER_LEN);
  int mesg_length = MarshallMessage(reply, 0xC0DE, protocol, arr_ptr);
  if (send(socket, reply, strlen(arr_ptr) + PROTOCOL_HEADER_LEN, 0) == -1)
    perror("write failed: ");
  fprintf(stderr,
          "[DEBUG] List Directory Handler Server : Replying kbac .... \n");
}
