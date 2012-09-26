#ifndef HASH_H
#define HASH_H 1

struct md_context {
  uint32_t h0;
  uint32_t h1;
  uint32_t h2;
  uint32_t h3;
  
  uint8_t digest[16];
  
  uint64_t count;
  
  uint8_t buffer[64];
};

#endif /* ifndef HASH_H */