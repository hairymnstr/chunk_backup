#ifndef DB_H
#define DB_H

#include <stdint.h>

typedef struct {
  uint64_t fid;
  uint64_t hash_hi;
  uint64_t hash_lo;
  uint64_t ctime;
  uint64_t atime;
  uint64_t mtime;
  uint64_t size;
} FileRecord;

int db_init(const char *);
int db_insert(FileRecord *);
int db_update(FileRecord *);

#endif /* ifndef DB_H */
