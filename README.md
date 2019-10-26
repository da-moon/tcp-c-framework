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

- Thread-safe server able to handle 1024 concurrent clients per server.
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

TO BE WRITTEN ... 

### Multiplexer

TO BE WRITTEN ... 

### Message

TO BE WRITTEN ... 

### Queue

TO BE WRITTEN ... 

### Handlers

TO BE WRITTEN ... 

### Client

TO BE WRITTEN ... 
