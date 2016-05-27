# Hello FileSystem

# SimpleFS
A simple in-memory filesystem implemented with fuse,
supporting the creation and remove of directory and regular file, and the read/write operation of regular files

# limitations
not thread-safe, not supporting file descriptor, not supporting rename

# Build
```sh
sudo apt-get install libfuse-dev
sudo apt-get install clang
make
mkdir tmp
./simplefs -s tmp
```
then you can do regular operations on tmp directory

to umount simplefs, just typing `fusermount -u tmp`
