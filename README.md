# SOAL SHIFT SISTEM OPERASI MODUL 4
Kelompok IT 28:
- Salsabila Rahmah (5027271005)
- Fadlillah Cantika Sari H (5027271042)
- I Dewa Made Satya Raditya (5027271051)
## Soal 1
## Soal 2
## Soal 3
### archeology.c
define library, menggunakan FUSE
```c
#define FUSE_USE_VERSION 30
#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#define CHUNK_SIZE 10240 // 10KB
```

Path relics
```c static const char *relics_path = "relics"; ```

Untuk split file
```c
void split_file(const char *filepath) {
    FILE *file = fopen(filepath, "rb");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    char chunk_name[256];
    unsigned char buffer[CHUNK_SIZE];
    int chunk_number = 0;
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, CHUNK_SIZE, file)) > 0) {
        sprintf(chunk_name, "%s.%03d", filepath, chunk_number++);
        FILE *chunk = fopen(chunk_name, "wb");
        if (!chunk) {
            perror("Failed to open chunk file");
            fclose(file);
            return;
        }
        fwrite(buffer, 1, bytes_read, chunk);
        fclose(chunk);
    }

    fclose(file);
}
```

Fungsi untuk menggabungkan file
```c
int merge_chunks(const char *path, char **merged_content, size_t *size) {
    char chunk_path[1024];
    FILE *fp;
    size_t chunk_size;
    char *buffer;
    int chunk_num = 0;
    *size = 0;


        snprintf(chunk_path, sizeof(chunk_path), "%s.%03d", path, chunk_num);
        fp = fopen(chunk_path, "rb");
        if (!fp) break;
        fseek(fp, 0, SEEK_END);
        chunk_size = ftell(fp);
        *size += chunk_size;
        fclose(fp);
        chunk_num++;
    }

    if (*size == 0) {
        return -ENOENT;  
    }

    *merged_content = (char *)malloc(*size);
    if (*merged_content == NULL) {
        return -ENOMEM;  
    }
    buffer = *merged_content;

    chunk_num = 0;
    while (1) {
        snprintf(chunk_path, sizeof(chunk_path), "%s.%03d", path, chunk_num);
        fp = fopen(chunk_path, "rb");
        if (!fp) break;
        chunk_size = fread(buffer, 1, CHUNK_SIZE, fp);
        buffer += chunk_size;
        fclose(fp);
        chunk_num++;
    }

    return 0;
}
```
Untuk mendapatkan atribut file atau direktori dan memeriksa file yang sesuai dengan path di relics_path.
```c
static int archeology_getattr(const char *path, struct stat *stbuf) {
    int res = 0;
    memset(stbuf, 0, sizeof(struct stat));
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    } else {
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s%s", relics_path, path);
        if (strstr(path, ".") == NULL) {
            res = stat(full_path, stbuf);
            if (res == -1) return -errno;
            stbuf->st_mode = S_IFREG | 0444;
        } else {
            return -ENOENT;
        }
    }
    return res;
}
```

Membaca isi direktori dan mengisi buffer dengan nama file yang ditemukan.
```c
static int archeology_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    (void) offset;
    (void) fi;

    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    // List merged files
    DIR *dp;
    struct dirent *de;
    dp = opendir(relics_path);
    if (dp == NULL)
        return -errno;

    char filename[256];
    while ((de = readdir(dp)) != NULL) {
        if (strstr(de->d_name, ".000") != NULL) {
            snprintf(filename, sizeof(filename), "%.*s", (int)(strlen(de->d_name) - 4), de->d_name);
            filler(buf, filename, NULL, 0);
        }
    }
    closedir(dp);

    return 0;
}
```

```c
static int archeology_open(const char *path, struct fuse_file_info *fi) {
    return 0;
} ```


Membaca konten file
```c
static int archeology_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    char *merged_content;
    size_t total_size;
    char full_path[1024];

    snprintf(full_path, sizeof(full_path), "%s%s", relics_path, path);

    if (merge_chunks(full_path, &merged_content, &total_size) != 0) {
        return -ENOENT;
    }

    if (offset < total_size) {
        if (offset + size > total_size)
            size = total_size - offset;
        memcpy(buf, merged_content + offset, size);
    } else {
        size = 0;
    }

    free(merged_content);
    return size;
}
```

Menulis konten ke file sementara, memecah file menjadi chunk, dan kemudian menghapus file sementara.
```c
static int archeology_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s%s", relics_path, path);

    FILE *temp_file = fopen(full_path, "wb");
    if (!temp_file) {
        return -errno;
    }
    fwrite(buf, 1, size, temp_file);
    fclose(temp_file);

    split_file(full_path);
    unlink(full_path);

    return size;
}
```
Menghapus file chunk yang terkait dengan path tertentu.
```c
static int archeology_unlink(const char *path) {
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s%s", relics_path, path);

    int chunk_num = 0;
    char chunk_path[1024];
    while (1) {
        snprintf(chunk_path, sizeof(chunk_path), "%s.%03d", full_path, chunk_num++);
        if (unlink(chunk_path) != 0) {
            if (errno == ENOENT) break; 
            return -errno;
        }
    }

    return 0;
}
```

Mendaftarkan operasi yang diimplementasikan untuk FUSE dan main function untuk menjalankan filesystem FUSE dengan operasi yang telah didefinisikan.
```c
static struct fuse_operations archeology_oper = {
    .getattr   = archeology_getattr,
    .readdir   = archeology_readdir,
    .open      = archeology_open,
    .read      = archeology_read,
    .write     = archeology_write,
    .unlink    = archeology_unlink,
};

int main(int argc, char *argv[]) {
    return fuse_main(argc, argv, &archeology_oper, NULL);
}

```

Command untuk compile
``` gcc -Wall archeology.c -o archeology $(pkg-config --cflags --libs fuse) ```
*dibutuhkan install pkgconf (sudo apt-get install pkgconf)

Command untuk menjalankan
``` ./archeology [path ke folder] ```
