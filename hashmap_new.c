//
// Created by karol on 31.10.24.
//

#include <stdlib.h>
#include "hashmap_new.h"
#include "farmhash-c.h"
#include "error.h"


#include <stdio.h>
#include <string.h>
HashTableValueNew update_value_new(HashTableValueNew current_value, HashTableValueNew incoming_value);

unsigned int hash_new(const char* string, int table_size) {
      size_t length = strlen(string);
      uint64_t h64 = farmhash64(string, length);
      return h64 & (table_size-1);
}

HashTableNew* hash_table_create_new(int size) {
      HashTableNew* hash_table = malloc(sizeof(HashTableNew));
      hash_table->entries_count = 0;
      if(hash_table == NULL) {
            ERR_AND_EXIT("malloc");
      }

      hash_table->size = size;
      hash_table->table = (HashTableEntryNew**)(malloc(sizeof(HashTableEntryNew*)*size));
      if(hash_table->table == NULL) {
            ERR_AND_EXIT("malloc");
      }

      for(int i = 0; i < size; i++) {
            hash_table->table[i] = NULL;
      }

      return hash_table;
}

void hash_table_free_new(HashTableNew* table) {
      for(int i = 0; i < table->size; i++) {
            if(table->table[i] != NULL) {
                  HashTableEntryNew* entry = table->table[i];
                        free(entry->key);
                        free(entry->values);
                        free(entry);
            }
      }

      free(table->table);
      free(table);
}

void hash_table_insert_new(HashTableNew* table, HashTableEntryNew* entry){
      if(entry == NULL) {
            ERR_AND_EXIT("Entry is NULL");
      }

      if (table == NULL || table->table == NULL) {
            ERR_AND_EXIT("Unitialized hashtable");
      }

      if (table->entries_count*2 > (table->size ))
      {
            hash_table_resize_new(table);
      }

      unsigned int hash_value = hash_new(entry->key, table->size);
      if(table->table[hash_value] == NULL) {
            table->table[hash_value] = entry;
      } else {
            unsigned int i = 1;
            HashTableEntryNew* current = table->table[(hash_value + i) & (table->size - 1)];
            while(current != NULL)
            {
                 // i = i << 1;
                  i++;
                  current = table->table[(hash_value + i) & (table->size -1)];
            }

            table->table[(hash_value + i) & (table->size - 1)] = entry;
      }
      table->entries_count++;
}
//
// void hash_table_resize_new(HashTableNew* ht)
// {
//       int new_size = ht->size * 2;
//
//       HashTableEntryNew** new_table = malloc(new_size * sizeof(HashTableEntryNew));
//       if (new_table == NULL) {
//             return;
//       }
//
//       HashTableEntryNew** old_table = ht->table;
//       int old_size  = ht->size;
//       ht->table = new_table;
//       ht->size  = new_size;
//       ht->entries_count = 0;
//
//       for (int i = 0; i < old_size; i++) {
//             if (old_table[i]!= NULL) {
//                   hash_table_insert_new(ht,old_table[i]);
//             }
//       }
//       free(old_table);
// }

void hash_table_resize_new(HashTableNew* ht) {
      int new_size = ht->size * 2;
      HashTableEntryNew** new_table = calloc(new_size, sizeof(HashTableEntryNew*));
      if (new_table == NULL) {
            ERR_AND_EXIT("Memory allocation failed");
      }

      for (int i = 0; i < ht->size; ++i) {
            if (ht->table[i] != NULL) {
                  unsigned int new_hash = hash_new(ht->table[i]->key, new_size);
                  new_table[new_hash] = ht->table[i];
            }
      }

      free(ht->table);
      ht->table = new_table;
      ht->size = new_size;
}

HashTableEntryNew* hash_table_search_new(HashTableNew* table, const char* key) {
      unsigned int hash_value = hash_new(key, table->size);
      if(table->table[hash_value] == NULL) {
            return NULL;
      }

      unsigned int i = 1;
      HashTableEntryNew* entry = table->table[(hash_value + i) & (table->size-1)];
      while(entry!= NULL)
      {
            if(strcmp(entry->key, key) == 0)
            {
                return entry;
            }

            i ++;
            // i = i << 1
            entry = table->table[(hash_value + i) & (table->size-1)];
      }

      return NULL;
}

void hash_table_delete_new(HashTableNew* table, const char* key) {
      unsigned int hash_value = hash_new(key, table->size);
      if(table->table[hash_value] == NULL) {
            return;
      }

      unsigned int i = 1;
      HashTableEntryNew* current = table->table[hash_value];
      while(current != NULL) {
            if(strcmp(current->key, key) == 0)
            {
                  free(current->key);
                  free(current->values);
                  free(current);
                  table->entries_count--;
                  table->table[(hash_value + i) & (table->size - 1)] = NULL;

                  return;
            }
            // i = i << 1;
            i++;
            current = table->table[(hash_value + i) & (table->size-1)];
      }
}

void hash_table_print_new(HashTableNew* ht) {
      if (ht == NULL || ht->table == NULL) {
            printf("HashTable is empty.\n");
            return;
      }

      for (int i = 0; i < ht->size; ++i) {
            HashTableEntryNew* entry = ht->table[i];

            if (entry == NULL)
            {
                  continue;
            }
                  printf("Key: %s\n", entry->key);
                  printf("Values:\n");

                  for (int j = 0; j < entry->n_values; ++j) {
                        HashTableValueNew* value = &entry->values[j];

                        switch(value->aggregate_function) {
                              case MIN_NEW: {
                                    printf("  Value[%d]: %ld (Min)\n", j, value->value);
                                    break;
                              }
                              case MAX_NEW: {
                                    printf("  Value[%d]: %ld (Max)\n", j, value->value);
                                    break;
                              }
                              case AVG_NEW:{
                                    printf("  Value[%d]: Accumulator = %ld, Count = %ld (Average)\n", j, value->value, value->count);
                                    break;
                              }
                              case MEDIAN_NEW:{
                                    printf("  Value[%d]: %ld (Max)\n", j, value->value);
                                    break;
                              }
                              case UNKNOWN_NEW: {
                                    printf("  Value[%d]: (Unknown)\n", j);
                                    break;
                              }
                        }
                  }
                  printf("\n");
      }
}

void hash_table_combine_entries_new(HashTableEntryNew* entry1, const HashTableEntryNew* entry2) {
      if(entry1 == NULL || entry2 == NULL) {
            INTERNAL_ERROR("Entry1 or Entry2 is NULL");
            return;
      }

      if(entry1->n_values > entry2->n_values) {
            INTERNAL_ERROR("Entry1 and Entry2 have different number of values");
            return;
      }
      int size = entry1->n_values;

      for(int i=0; i<size; i++) {
            entry1->values[i] = update_value_new(entry1->values[i], entry2->values[i]);
      }
}

HashTableValueNew update_value_new(HashTableValueNew current_value, HashTableValueNew incoming_value) {
      switch (current_value.aggregate_function) {
            case MIN_NEW: {
                  current_value.value = incoming_value.value < current_value.value ? incoming_value.value : current_value.value;
                  break;
            }
            case MAX_NEW: {
                  current_value.value = incoming_value.value > current_value.value ? incoming_value.value : current_value.value;
                  break;
            }
            case AVG_NEW: {
                  current_value.value += incoming_value.value;
                  current_value.count += incoming_value.count;
                  break;
            }
            case MEDIAN_NEW: {
                  // TODO: implement median calculation
                  current_value.value = -1;
                  break;
            }
            case UNKNOWN_NEW:
                  break;
      }

      return current_value;
}

