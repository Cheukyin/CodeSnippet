CC	   = clang++
CFLAGS = -g3 -ggdb -Wall -std=c++11 $(PKGFLAGS)
PKGFLAGS = `pkg-config fuse --cflags --libs`

OBJ = simplefs
SRC = simplefs.cpp

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OBJ)
