/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.

  gcc -Wall `pkg-config fuse --cflags --libs` hello.c -o hello
*/

#define FUSE_USE_VERSION 29

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include <string>
#include <list>
#include <algorithm>

using std::string;
using std::list;

using EntryType = mode_t;
const EntryType DIR_T = S_IFDIR;
const EntryType FILE_T = S_IFREG;

struct Entry
{
    Entry(EntryType ty): type(ty) {}
    Entry(EntryType ty, const string& name)
        : name(name), type(ty)
    {}
    Entry(EntryType ty, const string& name, mode_t perm)
        : name(name), type(ty), permission(perm)
    {}

    virtual ~Entry() {}

    mode_t mode() { return type|permission; }
    virtual size_t size() { return 0; }

    string name;

    EntryType type;
    mode_t permission;
    nlink_t nlink;
};

struct File: public Entry
{
    File(): Entry(FILE_T)
    { init(); }
    File(const string& name)
        : Entry(FILE_T, name)
    { init(); }
    File(const string& name, mode_t perm)
        : Entry(FILE_T, name, perm)
    { init(); }
    File(const string& name, const string& content)
        : Entry(FILE_T, name), content(content)
    { init(); }
    File(const string& name, mode_t perm, const string& content)
        : Entry(FILE_T, name, perm), content(content)
    { init(); }

    virtual size_t size() override { return content.size(); }

    string content;

private:
    void init()
    {
        Entry::nlink = 1;
    }
};

static list<Entry*> allEntries;

struct Directory: public Entry
{
    Directory(): Entry(DIR_T)
    { init(); }
    Directory(const string& name)
        : Entry(DIR_T, name)
    { init(); }
    Directory(const string& name, mode_t perm)
        : Entry(DIR_T, name, perm)
    { init(); }

    virtual ~Directory() override
    { for(Entry* ent : dirent) delete ent; }

    virtual size_t size() override
    {
        size_t sum = 0;
        for(Entry* ent : dirent) sum += ent->size();
        return sum;
    }

    list<Entry*> dirent;

private:
    void init()
    {
        Entry::nlink = 1;
    }
};

static Directory root("/", 0777);

void* hello_init(struct fuse_conn_info *conn)
{
    File* f1 = new File("/hello1", 0666, "hello world1");
    File* f2 = new File("/hello2", 0666, "hello world2");

    Directory* d1 = new Directory("/hellod1", 0777);
    Directory* d2 = new Directory("/hellod2", 0777);

    File* f3 = new File("/hellod1/hello3", 0666, "hello world3");

    root.dirent.push_back(f1);
    root.dirent.push_back(f2);
    root.dirent.push_back(d1);
    root.dirent.push_back(d2);

    d1->dirent.push_back(f3);

    allEntries.push_back(&root);
    allEntries.push_back(f1);
    allEntries.push_back(f2);
    allEntries.push_back(d1);
    allEntries.push_back(d2);
    allEntries.push_back(f3);

    return NULL;
}

void hello_destroy(void* private_data)
{
}

static int hello_getattr(const char *path, struct stat *stbuf)
{
	memset(stbuf, 0, sizeof(struct stat));

    auto it = std::find_if(allEntries.begin(), allEntries.end(),
        [path](Entry* ent)->bool{
            return ent->name == path;
        });

    if(it == allEntries.end()) return -ENOENT;

    stbuf->st_mode = (*it)->mode();
    stbuf->st_nlink = (*it)->nlink;
    stbuf->st_size = (*it)->size();
    return 0;
}

static int hello_access(const char *path, int mask)
{
    auto it = std::find_if(allEntries.begin(), allEntries.end(),
        [path](Entry* ent)->bool{
            return ent->name == path;
        });

    if(it == allEntries.end()) return -ENOENT;

    return 0;
}

static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi)
{
	(void) offset;
	(void) fi;

    auto it = std::find_if(allEntries.begin(), allEntries.end(),
        [path](Entry* ent)->bool{
            return ent->name == path;
        });

    if(it == allEntries.end()) return -ENOENT;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);

    for(Entry* ent : static_cast<Directory*>(*it)->dirent)
    {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_mode = ent->mode();
        st.st_nlink = ent->nlink;
        st.st_size = ent->size();

        int idx = ent->name.size();
        while(idx-- > 0)
            if(ent->name[idx] == '/') break;
        string fname = string(ent->name.begin() + idx + 1,
                              ent->name.end());
        filler(buf, fname.c_str(), &st, 0);
    }

	return 0;
}

// static int hello_open(const char *path, struct fuse_file_info *fi)
// {
//     for(int i=0; i < filenum; i++)
//         if(strcmp(path, files[i].name) == 0)
//             return 0;

// 	return -ENOENT;
// }

// static int hello_read(const char *path, char *buf, size_t size, off_t offset,
//                       struct fuse_file_info *fi)
// {
// 	(void) fi;

//     for(int i=0; i < filenum; i++)
//     {
//         if(strcmp(path, files[i].name) == 0)
//         {
//             size_t len = strlen(files[i].content);
//             if (offset < len)
//             {
//                 if(offset + size > len) size = len - offset;
//                 memcpy(buf, files[i].content + offset, size);
//             }
//             else size = 0;

//             return size;
//         }
//     }

//     return -ENOENT;
// }

// static int hello_truncate(const char *path, off_t size)
// {
//     for(int i=0; i < filenum; i++)
//     {
//         if(strcmp(path, files[i].name) == 0)
//         {
//             files[i].content[size] = 0;
//             return 0;
//         }
//     }

//     return -ENOENT;
// }

// static int hello_write(const char *path, const char *buf, size_t size,
//                        off_t offset, struct fuse_file_info *fi)
// {
//     (void)fi;

//     for(int i=0; i < filenum; i++)
//     {
//         if(strcmp(path, files[i].name) == 0)
//         {
//             size_t len = strlen(files[i].content);

//             char *ptr = files[i].content;
//             if(fi->flags | O_APPEND) ptr += len;
//             strcpy(ptr, buf);

//             return size;
//         }
//     }

//     return -ENOENT;
// }

// int hello_create(const char *path, mode_t mode, struct fuse_file_info* fi)
// {
//     int i = 0;
//     for(; i < filenum; i++)
//         if(strcmp(path, files[i].name) == 0)
//             break;

//     if((fi->flags|O_EXCL) && i < filenum) return -EEXIST;

//     strcpy(files[filenum].name, path);
//     files[i].content[filenum] = 0;

//     filenum++;

//     return 0;
// }

static int hello_release(const char *path, struct fuse_file_info *fi)
{
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */

	(void) path;
	(void) fi;
	return 0;
}

static struct fuse_operations hello_oper = {
    .init     = hello_init,
    .destroy  = hello_destroy,
	.getattr  = hello_getattr,
	.access   = hello_access,
	.readdir  = hello_readdir,
    // .truncate = hello_truncate,
    // .write    = hello_write,
    // .create   = hello_create,
	// .open     = hello_open,
    // .release  = hello_release,
	// .read     = hello_read,
};

int main(int argc, char *argv[])
{
	return fuse_main(argc, argv, &hello_oper, NULL);
}