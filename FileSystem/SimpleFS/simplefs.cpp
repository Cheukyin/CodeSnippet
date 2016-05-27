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

struct Entry;
struct Directory;

static list<Entry*> allEntries;

struct Entry
{
    Entry(EntryType ty): type(ty) {}
    Entry(EntryType ty, const string& name)
        : name(name), type(ty)
    {}
    Entry(EntryType ty, const string& name, mode_t perm)
        : name(name), type(ty), permission(perm)
    {}

    virtual ~Entry()
    {
        auto it = std::find(allEntries.begin(), allEntries.end(), this);
        if(it == allEntries.end()) return;
        allEntries.erase(it);
    }

    mode_t mode() { return type|permission; }
    virtual size_t size() { return 0; }

    string name;

    EntryType type;
    mode_t permission;
    nlink_t nlink;

    Directory* parent;
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

void* simplefs_init(struct fuse_conn_info *conn)
{
    root.parent = NULL;
    return NULL;
}

void simplefs_destroy(void* private_data)
{
}

static int simplefs_getattr(const char *path, struct stat *stbuf)
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

static int simplefs_access(const char *path, int mask)
{
    auto it = std::find_if(allEntries.begin(), allEntries.end(),
        [path](Entry* ent)->bool{
            return ent->name == path;
        });

    if(it == allEntries.end()) return -ENOENT;

    return 0;
}

static int simplefs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
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

static int simplefs_open(const char *path, struct fuse_file_info *fi)
{
    (void) fi;

    auto it = std::find_if(allEntries.begin(), allEntries.end(),
        [path](Entry* ent)->bool{
            return ent->name == path;
        });

    return it == allEntries.end() ? -ENOENT : 0;
}

static int simplefs_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi)
{
    (void)fi;

    auto it = std::find_if(allEntries.begin(), allEntries.end(),
        [path](Entry* ent)->bool{
            return ent->name == path;
        });

    if(it == allEntries.end()) return -ENOENT;

    if((*it)->type == DIR_T) return -EINVAL;

    if((*it)->type == FILE_T)
    {
        File* f = static_cast<File*>(*it);
        size_t len = f->content.size();
        if (offset < len)
        {
            if(offset + size > len) size = len - offset;
            memcpy(buf, f->content.c_str() + offset, size);
        }
        else size = 0;
    }

    return size;
}

static int simplefs_truncate(const char *path, off_t size)
{
    auto it = std::find_if(allEntries.begin(), allEntries.end(),
        [path](Entry* ent)->bool{
            return ent->name == path;
        });

    if(it == allEntries.end()) return -ENOENT;

    if((*it)->type == DIR_T) return -EINVAL;

    if((*it)->type == FILE_T)
    {
        File* f = static_cast<File*>(*it);
        if(size < f->content.size())
            f->content = f->content.substr(0, size);
    }

    return 0;
}

static int simplefs_write(const char *path, const char *buf, size_t size,
                       off_t offset, struct fuse_file_info *fi)
{
    (void)fi;

    auto it = std::find_if(allEntries.begin(), allEntries.end(),
        [path](Entry* ent)->bool{
            return ent->name == path;
        });

    if(it == allEntries.end()) return -ENOENT;

    if((*it)->type == DIR_T) return -EINVAL;

    if((*it)->type == FILE_T)
    {
        File* f = static_cast<File*>(*it);
        if(fi->flags | O_APPEND) f->content += string(buf, buf+size);
        else f->content = string(buf, buf+size);

        if(f->content[f->content.size()-1] == '\r')
            f->content[f->content.size()-1] = '\n';
    }

    return size;
}

static int insert_entry(Entry* ent)
{
    int idx = ent->name.size();
    while(idx-- > 0)
        if(ent->name[idx] == '/') break;
    string dirname(ent->name.substr(0, idx));
    if(idx == 0) dirname = "/";

    auto it = std::find_if(allEntries.begin(), allEntries.end(),
        [&dirname](Entry* ent)->bool{
            return ent->name == dirname;
        });

    if(it == allEntries.end() || (*it)->type != DIR_T)
        return -ENOENT;

    static_cast<Directory*>(*it)->dirent.push_back(ent);
    ent->parent = static_cast<Directory*>(*it);
    allEntries.push_back(ent);

    return 0;
}

static int simplefs_create(const char *path, mode_t mode, struct fuse_file_info* fi)
{
    (void)fi;

    auto it = std::find_if(allEntries.begin(), allEntries.end(),
        [path](Entry* ent)->bool{
            return ent->name == path;
        });

    if(it != allEntries.end() && fi->flags|O_EXCL)
        return -EEXIST;

    if(it != allEntries.end() && (*it)->type == DIR_T)
        return -EINVAL;

    File* f;
    if(it != allEntries.end())
        f = static_cast<File*>(*it);
    else f = new File(path, 0666);

    f->content = string();

    if(it == allEntries.end())
        return insert_entry(f);

    return 0;
}

static int simplefs_mkdir(const char *path, mode_t mode)
{
    (void)mode;

    auto it = std::find_if(allEntries.begin(), allEntries.end(),
        [path](Entry* ent)->bool{
            return ent->name == path;
        });

    if(it != allEntries.end()) return -EEXIST;

    Directory* d = new Directory(path, 0666);

    return insert_entry(d);
}

static int rmhelper(const char *path)
{
    auto it = std::find_if(allEntries.begin(), allEntries.end(),
        [path](Entry* ent)->bool{
            return ent->name == path;
        });

    if(it == allEntries.end()) return -ENOENT;

    Entry* ent = *it;

    Directory* parent = ent->parent;
    if(!parent) return 0;

    it = std::find(parent->dirent.begin(), parent->dirent.end(), ent);
    if(it != parent->dirent.end())
        parent->dirent.erase(it);

    delete ent;

    return 0;
}

static int simplefs_rmdir(const char *path)
{
    return rmhelper(path);
}

static int simplefs_unlink(const char *path)
{
    return rmhelper(path);
}

static int simplefs_release(const char *path, struct fuse_file_info *fi)
{
    /* Just a stub.	 This method is optional and can safely be left
    unimplemented */

    (void) path;
    (void) fi;
    return 0;
}

static int simplefs_fsync(const char *path, int isdatasync,
                       struct fuse_file_info *fi)
{
    /* Just a stub.	 This method is optional and can safely be left
       unimplemented */

    (void) path;
    (void) isdatasync;
    (void) fi;
    return 0;
}

static struct fuse_operations simplefs_oper = {
    .init     = simplefs_init,
    .destroy  = simplefs_destroy,
    .getattr  = simplefs_getattr,
    .access   = simplefs_access,
    .readdir  = simplefs_readdir,
    .truncate = simplefs_truncate,
    .write    = simplefs_write,
    .create   = simplefs_create,
    .open     = simplefs_open,
    .release  = simplefs_release,
    .read     = simplefs_read,
    .mkdir    = simplefs_mkdir,
    .rmdir    = simplefs_rmdir,
    .unlink   = simplefs_unlink,
    .fsync    = simplefs_fsync
};

int main(int argc, char *argv[])
{
    File* f1 = new File("/hello1", 0666, "hello world1");
    File* f2 = new File("/hello2", 0666, "hello world2");

    Directory* d1 = new Directory("/hellod1", 0777);
    Directory* d2 = new Directory("/hellod2", 0777);

    File* f3 = new File("/hellod1/hello3", 0666, "hello world3");

    f1->parent = f2->parent = d1->parent = d2->parent = &root;
    f3->parent = d1;

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

    return fuse_main(argc, argv, &simplefs_oper, NULL);
}