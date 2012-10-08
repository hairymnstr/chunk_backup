all:
	gcc -Wall -O3 main.c db.c hash.c -lsqlite3 -o chunk_backup
