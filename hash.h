#ifndef HASH_H
#define HASH_H 1

#include <stdint.h>

#define BUFSIZE (1024 * 1024)

struct md_context {
//   union {
  uint32_t h[4];
//     uint32_t h1;
//     uint32_t h2;
//     uint32_t h3;
  
  uint8_t digest[16];
//   };
  
//   uint32_t h0;
//   uint32_t h1;
//   uint32_t h2;
//   uint32_t h3;
  
  uint64_t count;
  
  uint8_t buffer[64];
};

int md5_file(const char *path, uint8_t hash[16]);

#endif /* ifndef HASH_H */