#include "client.h"
// Main loop to take in input and display output result from server
void Loop(int socket)
{
  int file_count = 1;
  char *upload_name = malloc(MAX_BUFFER);

  fd_set clientFds;
  //   char choice[MAX_BUFFER];
  int show_menu = 1;
  int waiting_for_choice = 1;
  int waiting_for_reply = 0;
  int upload_initiated = 0;
  while (1)
  {
    if (show_menu)
    {
      printf("\n--------------------------------------------------\n");
      puts("Please select your prefer service:\n  1. Echo\n  2. "
           "Download\n  3. Upload\n  4. Change Directory\n  5. List "
           "Directory\n  "
           "6. Quit\nEnter your choice: ");
      show_menu = 0;
    }

    // Reset the socket set each time since select()
    // modifies it
    FD_ZERO(&clientFds);
    FD_SET(socket, &clientFds);
    FD_SET(0, &clientFds);
    // wait for an available socket
    if (select(FD_SETSIZE, &clientFds, NULL, NULL, NULL) != -1)
    {
      for (int connection_file_descriptor_socket = 0;
           connection_file_descriptor_socket < FD_SETSIZE;
           connection_file_descriptor_socket++)
      {
        if (FD_ISSET(connection_file_descriptor_socket, &clientFds))
        {
          if (connection_file_descriptor_socket == socket)
          {
            printf("SERVER SOCKET CONNECTED\n");
            char *header = malloc(PROTOCOL_HEADER_LEN);
            int n = read(socket, header, PROTOCOL_HEADER_LEN);
            printf("size read  [%d] \n", n);
            if (n > 1)
            {
              uint16_t magic = ExtractMessageMagic(header);
              if (magic == 0xC0DE)
              {
                uint16_t protocol = ExtractMessageProtocol(header);
                uint32_t payload_size = ExtractMessageBodySize(header);
                char *recv_buffer = malloc(payload_size);
                read(socket, recv_buffer, payload_size);
                Message reply;
                reply.message_sender = socket;
                reply.magic = magic;
                reply.protocol = protocol;
                reply.size = payload_size;
                reply.body = recv_buffer;

                switch (reply.protocol)
                {
                case ERROR_MESSAGE:
                {
                  fprintf(stderr, "[ ERROR MESSAGE ] : [ %s ]", reply.body);
                  break;
                }
                case ECHO_REPLY:
                {
                  fprintf(stderr,
                          "MAGIC "
                          "[0x%04hX] | PROTOCOL "
                          "[0x%04hX] = [%C] | data : %s\n",
                          reply.magic, reply.protocol, reply.protocol,
                          reply.body);
                  fprintf(stderr, "[ ECHO FROM SERVER ] ");
                  break;
                }
                case LIST_DIR_REPLY:
                {
                  fprintf(stderr, "[ List Dir Result ] : [ %s ]", reply.body);
                  break;
                }
                case FILE_REPLY:
                {
                  // this can be piped to a file
                  fprintf(stderr, "[ File Download Reply ] : [ %s ]", reply.body);
                  // the following would store the file ...
                  FILE *fp;
                  char buf[256];
                  // sscanf(file_count, "./fixture/client/recieved", buf);
                  fp = fopen("./fixture/client/recieved", "w+");
                  fprintf(fp, "%s\n", reply.body);
                  fclose(fp);
                  break;
                }
                case READY_REPLY:
                {

                  if (upload_initiated)
                  {
                    char *payload = malloc(MAX_BUFFER);
                    uint16_t protocol;

                    FILE *fp = fopen(upload_name, "r");
                    if (fp == NULL)
                    {
                      memset(payload, 0, sizeof(payload));

                      strcpy(payload, "file not found");
                      protocol = ERROR_MESSAGE;
                    }
                    else
                    {
                      /* Go to the end of the file. */
                      if (fseek(fp, 0L, SEEK_END) == 0)
                      {
                        /* Get the size of the file. */
                        long bufsize = ftell(fp);
                        if (bufsize == -1)
                        {
                          memset(payload, 0, sizeof(payload));

                          strcpy(payload, "could not get file size");
                          protocol = ERROR_MESSAGE;
                        }
                        else
                        {

                          /* Allocate our buffer to that size. */
                          payload = malloc(sizeof(char) * (bufsize + 1));

                          /* Go back to the start of the file. */
                          if (fseek(fp, 0L, SEEK_SET) != 0)
                          {
                            memset(payload, 0, sizeof(payload));

                            strcpy(payload, "could not go to file start");
                            protocol = ERROR_MESSAGE;
                          }
                          else
                          {
                            /* Read the entire file into memory. */
                            size_t newLen = fread(payload, sizeof(char), bufsize, fp);
                            if (ferror(fp) != 0)
                            {
                              memset(payload, 0, sizeof(payload));

                              strcpy(payload, "Error reading file");
                              protocol = ERROR_MESSAGE;
                            }
                            else
                            {
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
                    if (payload_length <= MAX_BUFFER)
                    {
                      char *reply = malloc(strlen(arr_ptr) + PROTOCOL_HEADER_LEN);
                      int mesg_length = MarshallMessage(reply, 0xC0DE, FILE_REPLY, arr_ptr);
                      if (send(socket, reply, payload_length + PROTOCOL_HEADER_LEN, 0) == -1)
                        perror("write failed: ");
                      fprintf(stderr, "[DEBUG] Download Handler Server : Replying back .... \n");
                    }
                    else
                    {
                      while (payload_length > 0)
                      {
                        char *reply = malloc(strlen(arr_ptr) + PROTOCOL_HEADER_LEN);
                        int mesg_length = MarshallMessage(reply, 0xC0DE, FILE_REPLY, arr_ptr);
                        int sent = send(socket, reply, payload_length + PROTOCOL_HEADER_LEN, 0);
                        if (sent == -1)
                          perror("write failed: ");
                        fprintf(stderr, "[DEBUG] Download Handler Server : Replying back .... \n");
                        payload_length = payload_length - sent;
                      }
                    }
                  }
                  upload_initiated = 0;
                  break;
                }

                default:
                {
                  break;
                }
                }
              }
              else
              {
                break;
              }

              show_menu = 1;
            }
          }

          waiting_for_choice = 1;
          // break;
        }

        // continue;
        if (connection_file_descriptor_socket == 0)
        {
          if (waiting_for_choice)
          {
            char choice[MAX_BUFFER];
            if (fgets(choice, MAX_BUFFER - 1, stdin) == NULL)
            {
              if (errno == EINTR)
              {
                perror("fgets error");
                printf("restart...");
                continue;
              }
              else
              {
                perror("fgets else error");
                break;
              }
            }
            if (bcmp(choice, "1", 1) && bcmp(choice, "2", 1) &&
                bcmp(choice, "3", 1) && bcmp(choice, "4", 1) &&
                bcmp(choice, "5", 1) && bcmp(choice, "6", 1))
            {
              printf("Please enter a valid number from 1 to 6\n");
              continue;
            }
            system("clear");

            waiting_for_choice = 0;
            // Quit-----------------------------------------------------------------------------------------
            if (!bcmp(choice, "6", 1))
            {
              printf("Your choice is to Quit the program\n");
              leave_request(socket);
              exit(0);
            }
            // Echo-----------------------------------------------------------------------------------------
            if (!bcmp(choice, "1", 1))
            {
              printf("Your choice is Echo Protocol\n");
              EchoProtocolSendRequestToServer(socket);
            }
            // Download-----------------------------------------------------------------------------------------
            if (!bcmp(choice, "2", 1))
            {
              printf("Your choice is Download Protocol\n");
              DownloadProtocolSendRequestToServer(socket);
            }
            // Upload-----------------------------------------------------------------------------------------
            if (!bcmp(choice, "3", 1))
            {
              printf("Your choice is Upload Protocol\n");
              printf("Enter File Name for upload\n");
              fgets(upload_name, MAX_BUFFER - 1, stdin);
              char *arr_ptr = &upload_name[0];
              arr_ptr[strlen(arr_ptr) - 1] = '\0';

              char *request = malloc(strlen(arr_ptr) + PROTOCOL_HEADER_LEN);
              int mesg_length = MarshallMessage(request, 0xC0DE, UPLOAD_REQUEST, arr_ptr);
              Message message;
              message.body = (char *)(request + PROTOCOL_HEADER_LEN);
              printf("value cli [%s]\n", arr_ptr);
              if (send(socket, request, strlen(arr_ptr) + PROTOCOL_HEADER_LEN, 0) == -1)
                perror("write failed: ");
              fprintf(
                  stderr,
                  "[DEBUG] client : sending upload request for file %s to server\n",
                  message.body);

              upload_initiated = 1;
              continue;
              // UploadProtocolSendRequestToServer(socket);
            }
            // Change
            // Directory-----------------------------------------------------------------------------------------

            if (!bcmp(choice, "4", 1))
            {
              printf("Your choice is ChangeDirectory Protocol\n");
              ChangeDirectoryProtocolSendRequestToServer(socket);
            }
            // List
            // Directory-----------------------------------------------------------------------------------------
            if (!bcmp(choice, "5", 1))
            {
              printf("Your choice is List Directory Protocol\n");
              ListDirectoryProtocolSendRequestToServer(socket);
            }
          }
        }
        continue;
      }
    }
  }
}
void establish_connection_with_server(struct sockaddr_in *serverAddr,
                                      struct hostent *host,
                                      int connection_socket, long port)
{
  memset(serverAddr, 0, sizeof(serverAddr));
  serverAddr->sin_family = AF_INET;
  serverAddr->sin_addr = *((struct in_addr *)host->h_addr_list[0]);
  serverAddr->sin_port = htons(port);
  if (connect(connection_socket, (struct sockaddr *)serverAddr,
              sizeof(struct sockaddr)) < 0)
  {
    perror("Couldn't connect to server");
    exit(1);
  }
}

void set_non_blocking(int file_descriptor)
{
  int flags = fcntl(file_descriptor, F_GETFL);
  if (flags < 0)
    perror("fcntl failed");

  fcntl(file_descriptor, F_SETFL, flags);
}

void leave_request(int socket)
{
  if (write(socket, "/exit\n", MAX_BUFFER - 1) == -1)
    perror("write failed: ");

  close(socket);
  exit(1);
}
