CC	   = clang++
CFLAGS = -g3 -ggdb -Wall -std=c++11 $(PKGFLAGS)
PKGFLAGS = `pkg-config fuse --cflags --libs`

OBJ = hello
SRC = hello.cpp

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OBJ)
