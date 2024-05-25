# SOAL SHIFT SISTEM OPERASI MODUL 4
Kelompok IT 28:
- Salsabila Rahmah (5027271005)
- Fadlillah Cantika Sari H (5027271042)
- I Dewa Made Satya Raditya (5027271051)
## Soal 1
**Define library**
```c
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
```

**Function untuk membuat watermark**
```c
void BikinWM(const char *folderName) {
   DIR *dir;
    struct dirent *ent;
    char command[1024];
    mkdir("wm", 0755);  // bikin folder wm

    // buka direktori
    dir = opendir(".");

    // masukin file ke direktori/folder
    while ((ent = readdir(dir)) != NULL) {
        // cek tipe file
        if (strstr(ent->d_name, ".jpg") || strstr(ent->d_name, ".png") || strstr(ent->d_name, ".jpeg")) {
            // tambahi watermark
            snprintf(command, sizeof(command), "convert '%s' -draw 'gravity south fill white text 0,48 \"inikaryakita.id\"' 'wm-foto/wm-%s'", ent->d_name, ent->d_name);

            // eksekusi perintah
            system(command);
        }
    }

    // tutup direktori
    closedir(dir);
}
```

**Function untuk mengubah permit**
```c
void UbahPermit(const char *fileName) {
    
    chmod("script.sh", 0755);
}
```

**Main Function**
```c
int main() {
    BikinWM("wm-foto");
    UbahPermit("script.sh");

    return 0;
}
```

## Soal 2

### pesan.c
Definisi header & library

```c
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
```


Definisi Variabel Global
```c
static const char *dirpath = "/home/aca/pesan";
```

Variabel ini mendefinisikan path ke direktori yang akan dipantau oleh FUSE.

***Fungsi untuk mencatat history***
```c
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
```
Fungsi `log_action` mencatat tindakan tertentu ke dalam file log. Ini termasuk status, tag, dan informasi tambahan, serta timestamp saat pencatatan dilakukan.


```c
void base64_decode(const char *input, char **output) {
    // Implementasi decoding Base64
}

void rot13_decode(const char *input, char *output) {
    // Implementasi decoding ROT13
}

void hex_decode(const char *input, char *output) {
    // Implementasi decoding Hex
}

void reverse_text(const char *input, char *output) {
    // Implementasi membalik teks
}
```
Fungsi-fungsi ini direncanakan untuk mengimplementasikan berbagai metode decoding teks, seperti Base64, ROT13, Hex, dan membalik teks.

**Fungsi `getattr` untuk mendapatkan atribut dari sebuah file atau direktori yang ditunjuk oleh `path`. Atribut tersebut disimpan dalam `stbuf`.** 
```c
static int pesan_getattr(const char *path, struct stat *stbuf) {
    int res;
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);
    res = lstat(fpath, stbuf);
    if (res == -1) return -errno;
    return 0;
}
```

**Struktur Operasi FUSE**

```c
static struct fuse_operations pesan_oper = {
    .getattr = pesan_getattr,
    // Anda bisa menambahkan fungsi-fungsi lain yang digunakan di sini
};
```

Struktur ini mendefinisikan operasi-operasi FUSE yang digunakan dalam sistem file ini, salah satunya adalah `getattr`.

**Fungsi `main`**
```c
int main(int argc, char *argv[]) {
    return fuse_main(argc, argv, &pesan_oper, NULL);
}
```

Fungsi `main` adalah titik masuk program yang memanggil `fuse_main` dengan argumen yang diberikan dan struktur operasi FUSE.

## rahasia.c

Define library & header
```c
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
```

Kode ini mendefinisikan versi FUSE yang digunakan dan mengimpor berbagai header yang diperlukan untuk operasi file dan direktori.

**Fungsi untuk Memeriksa Password**

```c
int check_password() {
    char input[100];
    printf("Masukkan password untuk mengakses folder ini: ");
    scanf("%99s", input);
    return strcmp(input, "acacicu") == 0; // Ganti dengan password yang Anda inginkan
}
```

Fungsi `check_password` meminta pengguna untuk memasukkan password dan memeriksa apakah password tersebut cocok dengan yang diharapkan.

**Fungsi `getattr`**

```c
static int rahasia_getattr(const char *path, struct stat *stbuf) {
    int res;
    res = lstat(path, stbuf);
    if (res == -1)
        return -errno;
    return 0;
}
```

Fungsi ini mendapatkan atribut dari sebuah file atau direktori yang ditunjuk oleh `path`. Atribut tersebut disimpan dalam `stbuf`.

**Fungsi `readdir`**

```c
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
```

Fungsi ini membaca isi direktori yang ditunjuk oleh `path`. Sebelum membaca direktori, fungsi ini memeriksa password pengguna. Jika password salah, akses akan ditolak.

**Fungsi `open`**

```c
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
```

Fungsi ini membuka file yang ditunjuk oleh `path`. Sebelum membuka file, fungsi ini memeriksa password pengguna.

**Fungsi `read`**

```c
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
```

Fungsi ini membaca isi file yang ditunjuk oleh `path`. Sebelum membaca file, fungsi ini memeriksa password pengguna.

**Fungsi `write`**

```c
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
```

Fungsi ini menulis data ke file yang ditunjuk oleh `path`. Sebelum menulis ke file, fungsi ini memeriksa password pengguna.

**Struktur Operasi FUSE**

```c
static struct fuse_operations rahasia_oper = {
    .getattr = rahasia_getattr,
    .readdir = rahasia_readdir,
    .open = rahasia_open,
    .read = rahasia_read,
    .write = rahasia_write,
};
```

Struktur ini mendefinisikan operasi-operasi FUSE yang digunakan dalam sistem file ini, termasuk `getattr`, `readdir`, `open`, `read`, dan `write`.

**Fungsi `main`**

```c
int main(int argc, char *argv[]) {
    return fuse_main(argc, argv, &rahasia_oper, NULL);
}
```

Fungsi `main` adalah titik masuk program yang memanggil `fuse_main` dengan argumen yang diberikan dan struktur operasi FUSE.

## Soal 3
### archeology.c
**define library, menggunakan FUSE**
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


**Path relics**
```c static const char *relics_path = "relics"; ```



**Untuk split file**
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



**Fungsi untuk menggabungkan file**
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


**Untuk mendapatkan atribut file atau direktori dan memeriksa file yang sesuai dengan path di relics_path.**
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

**Membaca isi direktori dan mengisi buffer dengan nama file yang ditemukan.**
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
}
```


**Membaca konten file**
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


**Menulis konten ke file sementara, memecah file menjadi chunk, dan kemudian menghapus file sementara.**
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


**Menghapus file chunk yang terkait dengan path tertentu.**
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


**Mendaftarkan operasi yang diimplementasikan untuk FUSE dan main function untuk menjalankan filesystem FUSE dengan operasi yang telah didefinisikan.**
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

- **Command untuk compile**
``` gcc -Wall archeology.c -o archeology $(pkg-config --cflags --libs fuse) ```
*dibutuhkan install pkgconf (sudo apt-get install pkgconf)

- **Command untuk menjalankan**
``` ./archeology [path ke folder] ```

- **Langkah untuk mount folder**
```
mkdir sisop4
cd sisop4
mount
```

- **Langkah untuk unmount folder**
```sudo fusermount -u /home/salsa/sisop4/mount_point```
