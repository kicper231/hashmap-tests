#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hashmap_old.h"

// Liczba operacji
#define NUM_GROUPS 300000
#define NUM_AGGREGATES_PER_GROUP 5
#define SIZE_OF_HASHTABLE 1000
#define CONFLICT_NUMBER 10000

// Funkcja generująca losowe dane
void generate_random_data(char** keys, HashTableValue** values, int num_groups, int num_aggregates_per_group) {
    for (int i = 0; i < num_groups; i++) {
        keys[i] = malloc(25);
        int j = i % CONFLICT_NUMBER;
        snprintf(keys[i], 25, "group_%d", j);

        // Generowanie wartości (np. agregaty)
        values[i] = malloc(sizeof(HashTableValue) * num_aggregates_per_group);
        for (int j = 0; j < num_aggregates_per_group; j++) {
            values[i][j].value = rand() % 1000;
            values[i][j].count = 1;
            values[i][j].aggregate_function = j % 4;
        }
    }
}

// Funkcja zwalniająca pamięć
void free_random_data(char** keys, HashTableValue** values, int num_groups) {
    for (int i = 0; i < num_groups; i++) {
        free(keys[i]);
        free(values[i]);
    }
    free(keys);
    free(values);
}

void performance_test() {
    HashTable* table = hash_table_create(SIZE_OF_HASHTABLE);  // Tablica o orzmiarze podanym na górze

    // Generowanie danych testowych
    char** keys = malloc(sizeof(char*) * NUM_GROUPS);
    HashTableValue** values = malloc(sizeof(HashTableValue*) * NUM_GROUPS);
    generate_random_data(keys, values, NUM_GROUPS, NUM_AGGREGATES_PER_GROUP);

    clock_t start, end;

    // Test: wstawianie
    start = clock();
    for (int i = 0; i < NUM_GROUPS; i++) {
        HashTableEntry* entry = malloc(sizeof(HashTableEntry));
        entry->key = strdup(keys[i]);
        entry->n_values = NUM_AGGREGATES_PER_GROUP;
        entry->values = malloc(sizeof(HashTableValue) * NUM_AGGREGATES_PER_GROUP);
        memcpy(entry->values, values[i], sizeof(HashTableValue) * NUM_AGGREGATES_PER_GROUP);
        entry->next = NULL;

        hash_table_insert(table, entry);
    }
    end = clock();
    printf("Time taken for %d insert operations: %.2f seconds\n", NUM_GROUPS, (double)(end - start) / CLOCKS_PER_SEC);

    // Test: wyszukiwanie
    start = clock();
    for (int i = 0; i < NUM_GROUPS; i++) {
        HashTableEntry* found = hash_table_search(table, keys[i]);
        if (found == NULL) {
            printf("Key not found: %s\n", keys[i]);
        }
    }
    end = clock();
    printf("Time taken for %d search operations: %.2f seconds\n", NUM_GROUPS, (double)(end - start) / CLOCKS_PER_SEC);

    // Test: aktualizacje
    start = clock();
    for (int i = 0; i < NUM_GROUPS; i++) {
        HashTableEntry* found = hash_table_search(table, keys[i]);
        if (found != NULL) {
            // n_values to 5 zawsze
            for (int j = 0; j < found->n_values; j++) {
                found->values[j] = update_value(found->values[j], values[i][j]);
            }
        }
    }
    end = clock();
    printf("Time taken for %d update operations: %.2f seconds\n", NUM_GROUPS, (double)(end - start) / CLOCKS_PER_SEC);

    // Test: usuwanie
    start = clock();
    for (int i = 0; i < NUM_GROUPS; i++) {
        hash_table_delete(table, keys[i]);
    }
    end = clock();
    printf("Time taken for %d delete operations: %.2f seconds\n", NUM_GROUPS, (double)(end - start) / CLOCKS_PER_SEC);

    // Czyszczenie pamięci
    hash_table_free(table);
    free_random_data(keys, values, NUM_GROUPS);
}

int main() {
    srand(time(NULL));

    printf("Starting performance test for HashTable...\n");
    performance_test();
    printf("Performance test completed.\n");

    return 0;
}
