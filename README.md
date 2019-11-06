# TCP-C-Framework

- [TCP-C-Framework](#tcp-c-framework)
  - [Outline](#outline)
  - [Features](#features)
  - [Usage](#usage)
  - [Libraries](#libraries)
    - [Server](#server)
    - [Multiplexer](#multiplexer)
    - [Message](#message)
    - [Queue](#queue)
    - [Handlers](#handlers)
    - [Client](#client)

## Outline

this repo was made with the purpose of setting up foundation for working on coe-768 course project. In coe-768 , we are working with socket programming in C ; namely creating different "protocols" and multiplexing a server behavior associated to the said protocol with a client behaviour. Since I didn't find the given code used in our labs satisfactory and robust and could'nt find a satisfactory simple framework written in c on github, I decided to create this repo. 
This repo has three main directories :

- `pkg` : contains all the libraries used in server/client
- `cmd` : contains root `.c` files with `main()` function which gets compiled to server/client binaries
- `bin` : when using `makefile` commands, all compiled binaries are stored in this directory.

I have used `Debian 10 (Buster)` as my operating system , `visual studio code` as my text editor and `gcc (Debian 8.3.0-6) 8.3.0` as my compiler.

## Features

- Thread-safe server able to handle 4096 concurrent clients per server.
- Simple message marshalling/unmarshalling without worrying about endianness.
- Thread-safe request queue on the server side.
- Simple, yet efficient request multiplexer (router) on the server side to automatically deal with triggering approporiate RPC method to handle user's request. 
- Simple CLI on client side

## Usage

In the `makefile` , there are multiple targets used for simplifying build/execution of server and client binaries .

- `make clean` : cleans the repo by removing all garbage created in case of segmentation fault and removes `/bin` folder
- `make dep` : install `clang-format` with node package manager (npm). `clang-format` is used in combination with clang-format visual studio code [`extension`](https://marketplace.visualstudio.com/items?itemName=xaver.clang-format) to help with code formatting and beautifying.
- `make build` : first cleans the project directory by running `clean` target and then compiles client and server code and stores the result binaries in `bin` folder
- `make server` : first cleans the project directory by running `make clean` target and then compiles the server binary and stores it in `bin` folder.
- `make client` : first cleans the project directory by running `make clean` target and then compiles the client binary and stores it in `bin` folder.
- `make run-server` : first runs `make server` target and then starts the server and binds it to `localhost:8080`
- `make run-client` : first runs `make client` target and then starts the client cli. 

the binaries expect the following argument format to be passed to them when you are starting them : 

- **server** : `./bin/server [port]`
- **client** : `./bin/client [server IP] [Server Port]`

As a demo for the framework , I have implemented `echo` and `broadcast` protocols: 
- `echo` protocol returns to the client what it sent to the server.
- `broadcast` protocol is a essentially a chat room . Once a client sends a message to the server, it would broadcaset that message to all clients connected to it. 

to test the demo , first open a terminal in repositories' root directory and run `make run-server` to start the server on localhost (127.0.0.1) and bind it to port `8080` . then open two other terminals in repositories' root directory and run `make run-client` in those two terminals and use the guide shown on the terminal to choose protocols. after choosing a protocol and pressing enter, type in the message and press enter. You can switch to the terminal that is running the server to see server logs and on the terminal running the client you would see the reply and logs ( server reply protocol, data and other relevant info ).


## Libraries

This section explains what each library under `pkg` directory is responsible for. 

### Server

`Server` library has two methods :

- `Bind` : Handles binding server process to the given port.
- `InitializeRPCHandlers` : Sets up request multiplexer, message queue and initializes threads and mutexes associated with the server request handler.

### Multiplexer

Multiplexer library main job is to bind different requests with different methods that are to be invoked on the server side when those requests are recieved.
The `Connection` and `Multiplexer` structs are defined in this library :
a `Connection` struct has the following fields :
- socketFd : socket descriptor of multiplexer
- numClients : number of clients that are connected to multiplexer.
- clientSockets : an array that keeps track of which each connected client's socket descriptor.
a `Multiplexer` struct has the following fields :
- readFds : a field of type `fd_set` helps with accepting incoming connections.
- conn : a struct of type `Connection`
- clientListMutex : a mutex that makes updating the connected clients list thread safe.
- `Queue` : a FIFO queue that stores messages that the server has recieved.
- `clientSocketFd` : is used to store a local copy of the socket per connection

THe following methods are in this package :
- `Multiplex` : Adds a client's fd to list of client fds stored in Multiplexer struct and spawns a new thread per client in which `ClientHandler` is executed.
- `ClientHandler`: a method that acts as a `subscriber` ; it listens for payloads from client to adds them to multplexer struct's message processing queue
- `Disconnect`: it is invoked when a client is disconnected . It Removes the socket from the list of active client sockets and closes it

### Message

Message library helps with request/reply encoding and decoding in an endian safe manner. I did not want to use `c` basic struct encoding and decoding since I wasn't sure about endianness of structs that are encoded and decoded by `c` compiler which is dependent on compiler implementation, target architecture and other parameters so I wrote this library to help with message encoding and decoding. 
A message has an `8 bit header` before the data and it's general structure is explained in the following table :

| **Fields(in order)** | Magic | Protocol | Size | Body |
|:--------------------:|:-----:|:--------:|:----:|:----:|
|  **Size (in Bytes)** |   2   |     2    |   4  | 4096 |


`Message` struct is defined in this library with the following fields :

- `message_sender` : this is used to keep track of the client socket that sent this message 
- `magic` : Helps with defining the marshalling/unmarshalling protocol . Only data that start with `0xC0DE` ( magic = 0xC0DE) are accepted at this point .
- `protocol` : helps with defining the method that is supposed to be invoked when this message is recieved.
- `size` : total size of the payload . Possibly needs to be deprecated
- `body` : data that is stored in this message frame. 

the various methods that are used for marshalling/unmarshalling have extensive comments so take a look at the comments for explanation.

### Queue

A thread safe `FIFO` queue used that stores data of `Message` type.

### Handlers

This is the library in which Methods that are invoked when server recieves a message and Client recieves a reply and when client is sending the message to the server are defined.

When adding/modifying different protocols , this is the main library that needs to be modified. There are two protocols as example that shows how to define them.

In the following paragraphs, I will explain the workflow for adding a new protocol.

1- Adding the protocol type to `MessageType` enum in `handlers.h` file . assuming you would like to use characters to define a protocol , you can find their corresponding hex value in the following [`page`](https://www.ibm.com/support/knowledgecenter/en/SSVSD8_8.4.1/com.ibm.websphere.dtx.dsgnstud.doc/references/r_design_studio_intro_Hex_Decimal_and_Symbol_Values.htm). You must define a unique hex value for request and a unique hex value for reply . I use uppercase character hex representation for request and its lowercase for for reply
2- define prototype for client and server methods in `handlers.h` file. as a general rule, you must define the prototypes with the following signature :

- `{PROTOCOL NAME}HandleServerReply(int socket)` : this method is invoked on the client side . It is supposed to read the server reply and have the client binary perform the unique action it is supposed to perform on based on what was recieved from server.
- `{PROTOCOL NAME}SendRequestToServer(int socket)` :  this method is invoked on the client side . In here you would define and marshall the message and then send it to the server using that value of `socket` passed as an argument.
- `{PROTOCOL NAME}ServerHandler(char *result, Message message)` : this method is invoked on the server side. Always check for protocol type of the `message` argument and if it it the correct type, perform the needed action on the server side. In case the server is supposed to reply back to the client after  recieving the message, define a new struct of `Message` type and fill its fields with the approporiate reply , then use the `Marshall` method to conver it and store it in `result` argument.

3- create a new `.c` file with the same name as protocol of your choosing and import `handlers.h` , copy the previously defined method signatures in the `.c` file you have created and write their implementation. You can look at examples (`echo` and `Broadcast`) for help. 

4- edit `ServerRequestHandler` method in `handlers.c` file and add the protocol's enum to the switch case. follow the examples since this is quite the repetitive pattern .

5- edit the `client` library and add and invoke the methods that are related to the client there.

### Client

creates the client cli and helps deals with client interactions with the server . whenever a protocol is added, you must modify this library . Look at the examples and the source code as it is extensively commented . 

