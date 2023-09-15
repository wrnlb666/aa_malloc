CC = gcc
CFLAG = -Wall -Wextra -pedantic -std=c2x -Os
LIB = 
POST_FIX = 
ELF_FILES = 

ifeq ($(OS),Windows_NT)
	POST_FIX = dll
	LIB += -L. -ldict
else
	UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        CFLAG += -Wno-unused-result
		POST_FIX = so
		CFLAG += -Wl,-rpath=./
		LIB += -L. -ldict
		ELF_FILES := $(shell find . -type f -executable -exec sh -c 'file -b {} | grep -q ELF' \; -print)
    endif
endif

all: aa

.PHONY: aa

aa: src/aa.c src/aa.h 
	$(CC) $(CFLAG) -fPIC -shared $< -o lib$@.$(POST_FIX)

clean: 
	rm *.dll *.exe *.so $(ELF_FILES)
