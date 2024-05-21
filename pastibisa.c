//pesan.c

#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

static const char *dirpath = "/home/aca/pesan";

// Fungsi untuk mencatat tindakan
void log_action(const char *status, const char *tag, const char *info) {
    FILE *log_file = fopen("/home/aca/logs-fuse.log", "a");
    if (log_file != NULL) {
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        fprintf(log_file, "[%s]::%02d/%02d/%04d-%02d:%02d:%02d::[%s]::[%s]\n",
                status, tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
                tm.tm_hour, tm.tm_min, tm.tm_sec, tag, info);
        fclose(log_file);
    }
}

// Fungsi decoding Base64
void base64_decode(const char *input, char **output) {
    // Implementasi decoding Base64
}

// Fungsi decoding ROT13
void rot13_decode(const char *input, char *output) {
    // Implementasi decoding ROT13
}

// Fungsi decoding Hex
void hex_decode(const char *input, char *output) {
    // Implementasi decoding Hex
}

// Fungsi membalik teks
void reverse_text(const char *input, char *output) {
    // Implementasi membalik teks
}

// Deklarasi fungsi pesan_getattr
static int pesan_getattr(const char *path, struct stat *stbuf);

// Operasi Fuse untuk folder "pesan"
static struct fuse_operations pesan_oper = {
    .getattr = pesan_getattr,
    // Anda bisa menambahkan fungsi-fungsi lain yang digunakan di sini
};

static int pesan_getattr(const char *path, struct stat *stbuf) {
    int res;
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);
    res = lstat(fpath, stbuf);
    if (res == -1) return -errno;
    return 0;
}

int main(int argc, char *argv[]) {
    return fuse_main(argc, argv, &pesan_oper, NULL);
}

//rahasia.c
#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

// Fungsi untuk memeriksa password
int check_password() {
    char input[100];
    printf("Masukkan password untuk mengakses folder ini: ");
    scanf("%99s", input);
    return strcmp(input, "acacicu") == 0; // Ganti dengan password yang Anda inginkan
}

// Fungsi untuk mendapatkan atribut file
static int rahasia_getattr(const char *path, struct stat *stbuf) {
    int res;
    res = lstat(path, stbuf);
    if (res == -1)
        return -errno;
    return 0;
}

// Fungsi untuk membaca isi direktori
static int rahasia_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    // Memeriksa password
    if (!check_password())
        return -EACCES;

    DIR *dp;
    struct dirent *de;
    (void) offset;
    (void) fi;
    
    dp = opendir(path);
    if (dp == NULL)
        return -errno;

    // Membaca isi direktori
    while ((de = readdir(dp)) != NULL) {
        if (filler(buf, de->d_name, NULL, 0))
            break;
    }

    closedir(dp);
    return 0;
}

// Fungsi untuk membuka file
static int rahasia_open(const char *path, struct fuse_file_info *fi) {
    // Memeriksa password
    if (!check_password())
        return -EACCES;

    int fd;
    fd = open(path, fi->flags);
    if (fd == -1)
        return -errno;

    close(fd);
    return 0;
}

// Fungsi untuk membaca file
static int rahasia_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    // Memeriksa password
    if (!check_password())
        return -EACCES;

    int fd;
    int res;

    (void) fi;
    fd = open(path, O_RDONLY);
    if (fd == -1)
        return -errno;

    res = pread(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);
    return res;
}

// Fungsi untuk menulis file
static int rahasia_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    // Memeriksa password
    if (!check_password())
        return -EACCES;

    int fd;
    int res;

    (void) fi;
    fd = open(path, O_WRONLY);
    if (fd == -1)
        return -errno;

    res = pwrite(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);
    return res;
}

// Struktur operasi FUSE
static struct fuse_operations rahasia_oper = {
    .getattr = rahasia_getattr,
    .readdir = rahasia_readdir,
    .open = rahasia_open,
    .read = rahasia_read,
    .write = rahasia_write,
};

// Fungsi utama
int main(int argc, char *argv[]) {
    return fuse_main(argc, argv, &rahasia_oper, NULL);
}
