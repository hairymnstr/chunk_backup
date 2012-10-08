#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include "hash.h"
#include "db.h"

int main(int argc, char *argv[]) {
  struct passwd *pw = getpwuid(getuid());
  const char *homedir = pw->pw_dir;
  FileRecord f;
  struct stat sbuf;
  char buf[2048];
  struct dirent *ent;
  uint8_t hash_buf[16];
    
  char *db_path;
  db_path = (char *)malloc(sizeof(char) * (strlen(homedir) + strlen("/.cbcache") + 1));
  strcpy(db_path, homedir);
  strcpy(db_path + strlen(db_path), "/.cbcache");
  
  db_init(db_path);
  
  DIR *d;
  
  d = opendir("/home/nathan/chunk_backup");
  ent = readdir(d);
  while(ent != NULL) {
    if(!((strcmp(ent->d_name, ".") == 0) || (strcmp(ent->d_name, "..") == 0))) {
      strcpy(f.filename, ent->d_name);
      snprintf(buf, 2048, "/home/nathan/chunk_backup/%s", ent->d_name);
      stat(buf, &sbuf);
    
      f.size = sbuf.st_size;
      f.atime = sbuf.st_atime;
      f.mtime = sbuf.st_mtime;
      f.ctime = sbuf.st_ctime;
    
      md5_file(buf, hash_buf);
      memcpy(&f.hash_hi, hash_buf, 8);
      memcpy(&f.hash_lo, hash_buf+8, 8);
         
      db_insert(&f);
    }
    ent = readdir(d);
  }
  
  exit(0);
}
