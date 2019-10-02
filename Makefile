# OS specific part
# -----------------
ifeq ($(OS),Windows_NT)
    CLEAR = cls
    LS = dir
    TOUCH =>>
    RM = del /F /Q
    CPF = copy /y
    RMDIR = -RMDIR /S /Q
    MKDIR = -mkdir
    CMDSEP = &
    ERRIGNORE = 2>NUL || (exit 0)
    GO_PATH = $(subst \,/,${GOPATH})
    SEP=\\
else
    CLEAR = clear
    GO_PATH = ${GOPATH}
    LS = ls
    TOUCH = touch
    CPF = cp -f
    RM = rm -rf
    RMDIR = rm -rf
    CMDSEP = ;
    MKDIR = mkdir -p
    ERRIGNORE = 2>/dev/null
    SEP=/
endif
DIRS = $(notdir $(patsubst %/,%,$(dir $(wildcard ./cmd/*/.))))
PSEP = $(strip $(SEP))
nullstring :=
space := $(nullstring)
TARGET := $(DIRS)
SERVER_IP = 127.0.0.1
SERVER_PORT = 8081
PKG_C_FILES = ./pkg/base62/base62.c ./pkg/queue/queue.c ./pkg/server/server.c ./pkg/multiplexer/multiplexer.c ./pkg/handler/handler.c ./pkg/shared/utils.c

.PHONY:  client run-client server run-server clean dep print
client:
	- $(MKDIR) ./bin
	- $(RM) ./bin/client
	- gcc -o ./bin/client ${PKG_C_FILES} ./cmd/client/main.c -std=c99 -lpthread -Wall  -lnsl
	- $(CLEAR)
run-client: client
	- $(CLEAR)
	- ./bin/client damoon ${SERVER_IP} ${SERVER_PORT}
server:
	- $(CLEAR)
	- $(MKDIR) ./bin
	- $(RM) ./bin/server
	- gcc -o ./bin/server ${PKG_C_FILES} ./cmd/server/main.c -std=c99 -lpthread -Wall -lnsl
run-server: server
	- $(CLEAR)
	- ./bin/server ${SERVER_PORT}

build: clean
	- $(MKDIR) ./bin
	for target in $(TARGET); do \
		gcc -o .$(PSEP)bin$(PSEP)$$target ${PKG_C_FILES} .$(PSEP)cmd$(PSEP)$$target$(PSEP)main.c -std=c99 -lpthread -Wall  -lnsl; \
	done
clean:
	- $(RM) ./bin
	- $(RM) ./core.*
dep:

print:
	- $(CLEAR)
	- @echo $(space) ${PWD}