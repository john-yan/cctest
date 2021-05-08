
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

int main() {

  db_t db = create_db("test_db");
  check(db != NULL, "Can not create db");

  check(true == put(db, "key1", "value_1"), "Can not put to db");

  val_t ret = get(db, "key1");
  check(ret != NULL, "Can not get key1 from db");
  check(strcmp(ret, "value_1") == 0, "Return value doesnt match");

  check(false == put(db, "key1", "value_2"), "Expect duplicated key fail.");
  check(true == put(db, "key2", "value_2"), "Fail to put 2nd pair");

  ret = get(db, "key2");
  check(ret != NULL, "Can not get key2 from db");
  check(strcmp(ret, "value_2") == 0, "Return value doesnt match");

  check(remove_key(db, "key3"), "key3 doesn't exist");
  check(true == remove_key(db, "key1"), "Can not remove key1");
  check(get(db, "key1") == NULL, "key1 has been removed");
  check(false == update(db, "key1", "whatever"), "key1 has been removed");
  check(true == update(db, "key2", "new_value"), "can not update key2");

  ret = get(db, "key2");
  check(ret != NULL, "Can not get key2 from db");
  check(strcmp(ret, "new_value") == 0, "Return value doesnt match");
  check(true == put(db, "key3", "new_value"), "Fail to put key3");
  check(true == put(db, "key4", "new_value"), "Fail to put key4");

  query_result_t qr = query(db, "new_value");
  char* keys[] = {"key2", "key3", "key4"};
  query_result_t current = qr;
  int num_of_keys = 0;
  while(current != NULL) {
    check(one_of_strings(current->key, keys, 3) == true, "Return key doesn't exist");
    current = current->next;
    num_of_keys += 1;
  }
  check(num_of_keys == 3, "should return 3 keys");

  printf("All test passes.");
  return 0;
}
