#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include "db.h"

sqlite3 *db;

int db_create_tables() {
  char sql[1024];
  sqlite3_stmt *statement;
  
  strncpy(sql, "CREATE TABLE files (fid INTEGER PRIMARY KEY, hash_hi INTEGER, hash_lo INTEGER, ctime INTEGER, atime INTEGER, mtime INTEGER, size INTEGER)", 1024);
  sqlite3_prepare(db, sql, strlen(sql), &statement, NULL);
  
  int s = sqlite3_step(statement);
  sqlite3_finalize(statement);
  if(!(s == SQLITE_DONE)) {
    fprintf(stderr, "Failed to initialise the database tables\n");
    fprintf(stderr, "%s %d\n", sqlite3_errmsg(db), s);
    return -1;
  }
  
  return 0;
}

int db_init(const char *dbfile) {
  char sql[1024];
  sqlite3_stmt *statement;
  
  if(!(sqlite3_open(dbfile, &db) == SQLITE_OK)) {
    fprintf(stderr, "Error opening database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return -1;
  }
  
  // database opened okay, see if the tables we want are there if not make
  strcpy(sql, "SELECT name FROM sqlite_master WHERE type='table' AND name='files'");
  sqlite3_prepare(db, sql, strlen(sql), &statement, NULL);
  int s = sqlite3_step(statement);
  if(s == SQLITE_ROW) {
    // there is a result, means the table exists
    sqlite3_finalize(statement);
    return 0;
  } else if(s == SQLITE_DONE) {
    printf("No table found\n");
    sqlite3_finalize(statement);        // done with that query clean up
    db_create_tables();                 // make all the new tables
    return 0;
  } else {
    fprintf(stderr, "Error checking for tables: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return -1;
  }
  
}
