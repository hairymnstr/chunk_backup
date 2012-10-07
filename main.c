#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include "db.h"

int main(int argc, char *argv[]) {
  struct passwd *pw = getpwuid(getuid());
  const char *homedir = pw->pw_dir;
  FileRecord f;
  
  printf("Home directory = %s\n", homedir);
  
  char *db_path;
  db_path = (char *)malloc(sizeof(char) * (strlen(homedir) + strlen("/.cbcache") + 1));
  strcpy(db_path, homedir);
  strcpy(db_path + strlen(db_path), "/.cbcache");
  
  db_init(db_path);
  
  f.hash_hi = 1;
  f.hash_lo = 2;
  f.ctime = 3;
  f.atime = 4;
  f.mtime = 5;
  f.size = 6;
  
  db_insert(&f);
  
  exit(0);
}
