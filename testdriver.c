
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"

void check(bool ok, char* err_str) {
  if (!ok) {
    printf("%s\n", err_str);
    exit(-1);
  }
}

bool one_of_strings(char* target, char* strings[], int length) {
  for(int i = 0; i < length; i++) {
    if (strcmp(target, strings[i]))
      return true;
  }
  return false;
}

size_t getline(char**, size_t*, FILE*);

int qr_length(query_result_t qr) {
  if (qr == NULL) return 0;
  return qr_length(qr->next) + 1;
}

int main() {

  char* buf = (char*)malloc(1024);
  size_t len, read;
  db_t db;
  while ((read = getline(&buf, &len, stdin) != -1)) {
    char opcode[64], operand[1024], operand2[1024];
    sscanf(buf, "%s %s %s", opcode, operand, operand2);

    if (strcmp(opcode, "create_db") == 0) {
      db = create_db(operand);
      if (db != NULL) {
        printf("create_db Success.\n");
      } else {
        printf("create_db Fail.\n");
      }
    } else if (strcmp(opcode, "open_db") == 0) {
      db = open_db(operand);
      if (db != NULL) {
        printf("open_db Success.\n");
      } else {
        printf("open_db Fail.\n");
      }
    } else if (strcmp(opcode, "close_db") == 0) {
      close_db(db);
      printf("close_db Success.\n");
    } else if (strcmp(opcode, "get") == 0) {
      key_t key = operand;
      val_t val = get(db, key);
      printf("get RET = %s\n", val == NULL ? "NULL": val);
    } else if (strcmp(opcode, "put") == 0) {
      key_t key = operand;
      val_t val = operand2;
      bool ret = put(db, key, val);
      printf("put RET = %s\n", ret ? "true" : "false");
    } else if (strcmp(opcode, "remove_key") == 0) {
      key_t key = operand;
      bool ret = remove_key(db, key);
      printf("remove_key RET = %s\n", ret ? "true" : "false");
    } else if (strcmp(opcode, "update") == 0) {
      key_t key = operand;
      val_t val = operand2;
      bool ret = update(db, key, val);
      printf("update RET = %s\n", ret ? "true" : "false");
    } else if (strcmp(opcode, "query") == 0) {
      val_t val = operand;
      query_result_t qr = query(db, val);
      int length = qr_length(qr);
      if (length == 0) {
        printf("query RET = NULL\n");
      } else {
        char* result[length];
        query_result_t cur = qr;
        for (int i = 0; i < length; i++, cur = cur->next) {
          result[i] = cur->key;
        }
        qsort(result, length, sizeof(char*), strcmp);
        printf("query RET = %s", result[0]);
        for (int i = 1; i < length; i++) {
          printf(",%s", result[i]);
        }
        printf("\n");
      }
      delete_query_result(qr);
    }
  }
  return 0;
}
