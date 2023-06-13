//#include "../inc/libmx.h"
//
//#include "stdio.h"
//
//int mx_read_line(char **lineptr, size_t buf_size, char delim, const int fd) {
//    if (fd < 0 || buf_size < 0 || !delim || !lineptr) return -2;
//    (*lineptr) = (char *)mx_realloc(*lineptr, buf_size);
//    mx_memset((*lineptr), '\0', sizeof((*lineptr)));
//    size_t size = 0;
//    char buffer;
//    
//    if (read(fd, &buffer, 1)) {
//        if (buffer == delim) return size;
//        (*lineptr) = (char *)mx_realloc(*lineptr, size + 1);
//        (*lineptr)[size] = buffer;
//        size++;
//    }
//    else return -1;
//
//    while (read(fd, &buffer, 1)) {
//        if (buffer == delim) break;
//        if (size >= buf_size) (*lineptr) = (char*)mx_realloc(*lineptr, size + 1);
//        (*lineptr)[size] = buffer;
//        size++;
//    }
//    size_t free_bytes = mx_strlen((*lineptr)) - size;
//
//    mx_memset(&(*lineptr)[size], '\0', free_bytes);
//    return size + 1;
//}


#include "../inc/libmx.h"
#include "stdio.h"

int mx_read_line(char **lineptr, size_t buf_size, char delim, const int fd) {
    if (fd < 0 || buf_size < 0 || !delim || !lineptr) return -2;

    (*lineptr) = (char *)mx_realloc(*lineptr, buf_size);
    mx_memset((*lineptr), '\0', sizeof((*lineptr)));
    size_t size = 0;
    char buffer;

    if (read(fd, &buffer, 1)) {
        if (buffer == delim) return size;

        (*lineptr) = (char *)mx_realloc(*lineptr, size + 1);
        (*lineptr)[size] = buffer;
        size++;
    }
    else
        return -1;

    while (read(fd, &buffer, 1)) {
        if (buffer == delim) break;
        
        if (size >= buf_size)
            (*lineptr) = (char*)mx_realloc(*lineptr, size + 1);

        (*lineptr)[size] = buffer;
        size++;
    }

    (*lineptr) = (char*)mx_realloc(*lineptr, size + 1);

    size_t free_bytes = sizeof((*lineptr)) - size;
    mx_memset(&(*lineptr)[size], '\0', free_bytes);

    return size + 1;
}
