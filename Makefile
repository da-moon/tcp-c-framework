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
# recursive wildcard
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
PSEP = $(strip $(SEP))
nullstring :=
space := $(nullstring)
SERVER_IP = 127.0.0.1
SERVER_PORT = 8080
LIBRARIES = $(filter-out $(wildcard ./cmd/*/*.c), $(call rwildcard,./,*.c))
TARGET = $(notdir $(patsubst %/,%,$(dir $(wildcard ./cmd/*/.))))

.PHONY:  dep client run-client server run-server clean
client: 
	- $(MKDIR) ./bin
	- $(RM) ./bin/client
	- gcc -o ./bin/client ${LIBRARIES} ./cmd/client/main.c -std=c99 -lpthread -Wall  -lnsl
run-client: client
	- $(CLEAR)
	- ./bin/client ${SERVER_IP} ${SERVER_PORT}

server: clean
	- $(CLEAR)
	- $(MKDIR) ./bin
	- $(RM) ./bin/server
	- gcc -o ./bin/server ${LIBRARIES} ./cmd/server/main.c -std=c99 -lpthread -Wall -lnsl
run-server: server
	- $(CLEAR)
	- ./bin/server ${SERVER_PORT}

build: clean
	- $(MKDIR) ./bin
	for target in $(TARGET); do \
		gcc -o .$(PSEP)bin$(PSEP)$$target ${LIBRARIES} .$(PSEP)cmd$(PSEP)$$target$(PSEP)main.c -std=c99 -lpthread -Wall  -lnsl; \
	done
clean:
	- $(RM) ./bin
	- $(RM) ./core.*
dep:
	- $(info Installing clang-format from npm)
	- npm install -g clang-format

print:
	- $(CLEAR)
	- @echo $(space) $(DIRS)

