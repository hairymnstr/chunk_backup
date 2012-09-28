#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include "db.h"

int main(int argc, char *argv[]) {
  struct passwd *pw = getpwuid(getuid());
  const char *homedir = pw->pw_dir;
  
  printf("Home directory = %s\n", homedir);
  
  char *db_path;
  db_path = (char *)malloc(sizeof(char) * (strlen(homedir) + strlen("/.cbcache") + 1));
  strcpy(db_path, homedir);
  strcpy(db_path + strlen(db_path), "/.cbcache");
  
  db_init(db_path);
}
