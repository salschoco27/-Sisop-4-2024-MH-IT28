#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

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

void UbahPermit(const char *fileName) {
    
    chmod("script.sh", 0755);

}


int main() {
    BikinWM("wm-foto");
    UbahPermit("script.sh");

    return 0;
}
