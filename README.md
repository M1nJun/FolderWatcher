Here's some stuff I learned in the process:

# Directories

getcwd()

The preferred method for obtaining the current working directory is the getcwd() system call, which POSIX standardized:

```C
    #include <unistd.h>
    char * getcwd (char *buf, size_t size);
```

A successful call to getcwd() copies the current working directory as an absolute path‐ name into the buffer pointed at by buf, which is of length size bytes and returns a pointer to buf. On failure, the call returns NULL

opendir()

To begin reading a directory’s contents you need to create a directory stream, which is represented by a DIR object:

```C
    #include <sys/types.h>
    #include <dirent.h>
    DIR * opendir (const char *name);
```

A successful call to opendir() creates a directory stream representing the directory
given by name.
A directory stream is little more than a file descriptor representing the open directory, some metadata, and a buffer to hold the directory’s contents.

readdir()

Once you have created a directory stream with opendir(), your program can begin reading entries from the directory. To do this, use readdir(), which returns entries one by one from a given DIR object:

```C
    #include <sys/types.h>
    #include <dirent.h>
    struct dirent * readdir (DIR *dir);
```

A successful call to readdir() returns the next entry in the directory represented by dir.
Applications successively invoke readdir(), obtaining each file in the directory, until they find the file they are searching for or until the entire directory is read, at which time readdir() returns NULL.

On failure, readdir() also returns NULL. To differentiate between an error and having read all of the files, applications must set errno to 0 before each readdir() invocation and then check both the return value and errno. The only errno value set by read dir() is EBADF, signifying that dir is invalid. Thus, many applications do not bother to check for errors and assume that NULL means that no more files remain.

closedir()

To close a directory stream opened with opendir(), use closedir():

```C
    #include <sys/types.h>
    #include <dirent.h>
    int closedir (DIR *dir);
```

A successful call to closedir() closes the directory stream represented by dir, includ‐ ing the backing file descriptor, and returns 0. On failure, the function returns −1 and sets errno to EBADF, the only possible error code, signifying that dir is not an open directory stream.

# Running a New Process

In Unix, the act of loading into memory and executing a program image is separate from the act of creating a new process. One system call loads a binary program into memory, replacing the previous contents of the address space, and begins execution of the new program. This is called executing a new program, and the functionality is pro‐ vided by the exec family of calls.
A different system call is used to create a new process, which initially is a near-duplicate of its parent process. Often, the new process immediately executes a new program. The act of creating a new process is called forking, and this functionality is provided by the fork() system call. Two acts—first a fork to create a new process, and then an exec to load a new binary into that process—are thus required to execute a new program in a new process. We will cover the exec calls first, then fork().

execve()

```C

int execve (const char *filename,
            char *const argv[],
            char *const envp[]);
```

On success, the exec system calls do not return. On failure, the calls return −1
