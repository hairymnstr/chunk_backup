#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "hash.h"

// 32 bit arbitrary offset rotate rather than shift
#define leftrotate(x, c) ((x << c) | (x >> (32-c)))

//r specifies the per-round shift amounts
const uint32_t r[64] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                        5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
                        4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                        6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

//(Or just use the following table):
const uint32_t k[64] = {0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
                        0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
                        0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
                        0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
                        0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
                        0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
                        0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
                        0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
                        0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
                        0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
                        0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
                        0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
                        0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
                        0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
                        0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
                        0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};

void md5_transform(struct md_context *context) {
  uint32_t a, b, c, d, f, g, j, x, temp;
  a = context->h0;
  b = context->h1;
  c = context->h2;
  d = context->h3;
  
//   for(j=0;j<64;j++) {
//     if(j < 16) {
//       f = (b & c) | ((~b) & d);
//       g = j;
//     } else if(j < 32) {
//       f = (d & b) | ((~d) & c);
//       g = (5*j + 1) % 16;
//     } else if(j < 48) {
//       f = b ^ c ^ d;
//       g = (3 * j + 5) % 16;
//     } else {
//       f = c ^ (b | (~d));
//       g = (7*j) % 16;
//     }
//     temp = d;
//     d = c;
//     c = b;
//     x = (a + f + k[j] + (*(uint32_t *)(&context->buffer[g * 4])));
//     b = b + leftrotate(x, r[j]);
//     a = temp;
//   }
  
  for(j=0;j<16;j++) {
    f = (b & c) | ((~b) & d);
    g = j;
    temp = d;
    d = c;
    c = b;
    x = (a + f + k[j] + (*(uint32_t *)(&context->buffer[g * 4])));
    b = b + leftrotate(x, r[j]);
    a = temp;
  }
  for(j=16;j<32;j++) {
    f = (d & b) | ((~d) & c);
    g = (5*j + 1) % 16;
    temp = d;
    d = c;
    c = b;
    x = (a + f + k[j] + (*(uint32_t *)(&context->buffer[g * 4])));
    b = b + leftrotate(x, r[j]);
    a = temp;
  }
  for(j=32;j<48;j++) {
    f = b ^ c ^ d;
    g = (3 * j + 5) % 16;
    temp = d;
    d = c;
    c = b;
    x = (a + f + k[j] + (*(uint32_t *)(&context->buffer[g * 4])));
    b = b + leftrotate(x, r[j]);
    a = temp;
  }
  for(j=48;j<64;j++) {
    f = c ^ (b | (~d));
    g = (7*j) % 16;
    temp = d;
    d = c;
    c = b;
    x = (a + f + k[j] + (*(uint32_t *)(&context->buffer[g * 4])));
    b = b + leftrotate(x, r[j]);
    a = temp;
  }
  context->h0 += a;
  context->h1 += b;
  context->h2 += c;
  context->h3 += d;
}

void md5_start(struct md_context *context) {
  context->h0 = 0x67452301;
  context->h1 = 0xefcdab89;
  context->h2 = 0x98badcfe;
  context->h3 = 0x10325476;
  
  context->count = 0;
  
  memset(context->buffer, 0, 64);
  
  memset(context->digest, 0, 16);
  
  return;
}

void md5_update(struct md_context *context, uint8_t *chunk, uint64_t chunk_size) {
  uint8_t *temp;
  int buflen = context->count & 63;
  context->count += chunk_size;
  
  if((buflen + chunk_size) < 64) {
    memcpy(&context->buffer[buflen], chunk, chunk_size);
    return;
  }
  
  memcpy(&context->buffer[buflen], chunk, 64 - buflen);
  md5_transform(context);
  chunk_size -= (64 - buflen);
  chunk += (64 - buflen);
  while(chunk_size >= 64) {
    memcpy(context->buffer, chunk, 64);
    md5_transform(context);
    chunk_size -= 64;
    chunk += 64;
  }
  memcpy(context->buffer, chunk, chunk_size);
  return;
}

void md5_finish(struct md_context *context) {
  int buflen = context->count & 63;
  
  context->buffer[buflen++] = 0x80;
  memset(&context->buffer[buflen], 0, 64-buflen);
  if(buflen > 56) {
    md5_transform(context);
    memset(context->buffer, 0, 64);
  }
  *(uint64_t *)(&context->buffer[56]) = context->count * 8;
  md5_transform(context);
  *(uint32_t *)(&context->digest[0]) = context->h0;
  *(uint32_t *)(&context->digest[4]) = context->h1;
  *(uint32_t *)(&context->digest[8]) = context->h2;
  *(uint32_t *)(&context->digest[12]) = context->h3;
  return;
}

int main(int argc, char *argv[]) {
  // magic constants needed at the start
//   uint32_t h0 = 0x67452301;
//   uint32_t h1 = 0xefcdab89;
//   uint32_t h2 = 0x98badcfe;
//   uint32_t h3 = 0x10325476;
//   uint32_t a, b, c, d, f, g, i, j, temp;
  uint32_t i;
  uint32_t l;
  FILE *fp;
  uint8_t buffer[1024 * 1024];
  
  if(argc < 2) {
    fprintf(stderr, "Please enter a string to be hashed as the first argument\n");
    exit(-2);
  }
  
//   uint64_t msglen = strlen(argv[1]);
  
  struct md_context context;

  fp = fopen(argv[1], "rb");
  md5_start(&context);
  
  l = fread(&buffer, 1, 1024 * 1024, fp);
  
  while(l == (1024 * 1024)) {
    md5_update(&context, buffer, 1024 * 1024);
    l = fread(&buffer, 1, 1024 * 1024, fp);
  }
  if(l < (1024 * 1024))
    md5_update(&context, buffer, l);

//   for(i=0;i<16;i++) {
//     printf("%08x\n", *(uint32_t *)(&context.buffer[i*4]));
//   }
//   printf("%lu\n", context.count);
  md5_finish(&context);
  
  
//   uint8_t *message;
//   
//   printf("msglen = %d (%016x)\n", msglen, msglen*8);
//   
//   uint64_t msglen_chunks = msglen / sizeof(hash_chunk);
//   
//   if((msglen + 1) % 64 > 56) {
//     msglen_chunks += 2;
//   } else {
//     msglen_chunks += 1;
//   }
//   
//   message = (uint8_t *)malloc(64 * msglen_chunks);
//   memset(message, 0, 64 * msglen_chunks);
//   strncpy(message, argv[1], msglen);
//   message[msglen] = 0x80;
//   *(uint64_t *)(&message[64 * msglen_chunks-8]) = (uint64_t)(msglen * 8l);
//   
//   for(i=0;i<msglen_chunks * 16;i++) {
//     printf("%08x\n", *(uint32_t *)(&message[i*4]));
//   }
//   for(i=0;i<msglen_chunks;i++) {
//     a = h0;
//     b = h1;
//     c = h2;
//     d = h3;
//   
//     for(j=0;j<64;j++) {
//       if(j < 16) {
//         f = (b & c) | ((~b) & d);
//         g = j;
//       } else if(j < 32) {
//         f = (d & b) | ((~d) & c);
//         g = (5*j + 1) % 16;
//       } else if(j < 48) {
//         f = b ^ c ^ d;
//         g = (3 * j + 5) % 16;
//       } else {
//         f = c ^ (b | (~d));
//         g = (7*j) % 16;
//       }
//       temp = d;
//       d = c;
//       c = b;
//       b = b + leftrotate((a + f + k[j] + (*(uint32_t *)(&message[i*64 + g * 4]))), r[j]);
//       a = temp;
//     }
//     h0 += a;
//     h1 += b;
//     h2 += c;
//     h3 += d;
//   }
//   uint8_t digest[16];
//   *(uint32_t *)(&digest[0]) = h0;
//   *(uint32_t *)(&digest[4]) = h1;
//   *(uint32_t *)(&digest[8]) = h2;
//   *(uint32_t *)(&digest[12]) = h3;
  
  for(i=0;i<16;i++) {
    printf("%02x", context.digest[i]);
  }
  printf("\n");
  
//   printf("%08x%08x%08x%08x\n", h0, h1, h2, h3);
  
  exit(0);
}
  
//Pre-processing: adding a single 1 bit
// append "1" bit to message    
/* Notice: the input bytes are considered as bits strings,
  where the first bit is the most significant bit of the byte.[37]
  

//Pre-processing: padding with zeros
append "0" bit until message length in bit ≡ 448 (mod 512)
append length mod (2 pow 64) to message


//Process the message in successive 512-bit chunks:
for each 512-bit chunk of message
    break chunk into sixteen 32-bit words w[j], 0 ≤ j ≤ 15
//Initialize hash value for this chunk:
    var int a := h0
    var int b := h1
    var int c := h2
    var int d := h3
//Main loop:
    for i from 0 to 63
        if 0 ≤ i ≤ 15 then
            f := (b and c) or ((not b) and d)
            g := i
        else if 16 ≤ i ≤ 31
            f := (d and b) or ((not d) and c)
            g := (5×i + 1) mod 16
        else if 32 ≤ i ≤ 47
            f := b xor c xor d
            g := (3×i + 5) mod 16
        else if 48 ≤ i ≤ 63
            f := c xor (b or (not d))
            g := (7×i) mod 16
        temp := d
        d := c
        c := b
        b := b + leftrotate((a + f + k[i] + w[g]) , r[i])
        a := temp
    end for
//Add this chunk's hash to result so far:
    h0 := h0 + a
    h1 := h1 + b
    h2 := h2 + c
    h3 := h3 + d
end for

var char digest[16] := h0 append h1 append h2 append h3 //(Output is in little-endian)

//leftrotate function definition
leftrotate (x, c)
    return (x << c) binary or (x >> (32-c));*/