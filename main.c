#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hashmap_new.h"
#include "hashmap_old.h"

#define NUM_GROUPS 500000
#define NUM_AGGREGATES_PER_GROUP 5
#define SIZE_OF_HASHTABLE 1024
#define CONFLICT_NUMBER 10000
#define BUFFER_SIZE 1024

void generate_random_data(char** keys, HashTableValue** values, int num_groups, int num_aggregates_per_group) {
    for (int     i = 0; i < num_groups; i++) {
        keys[i] = malloc(35);
        int j = i % CONFLICT_NUMBER;
        snprintf(keys[i], 35, "group_%d", j);


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

void performance_test_old() {
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

void generate_random_data_new(char** keys, HashTableValueNew** values,
                              int num_groups, int num_aggregates_per_group) {
    for (int i = 0; i < num_groups; i++) {

        keys[i] = malloc(25);
        int j = i % CONFLICT_NUMBER;
        snprintf(keys[i], 25, "group_%d", j);

        values[i] = malloc(sizeof(HashTableValueNew) * num_aggregates_per_group);
        for (int k = 0; k < num_aggregates_per_group; k++) {
            values[i][k].value = rand() % 1000;
            values[i][k].count = 1;
            values[i][k].aggregate_function = k % 4;
        }
    }
}

void free_random_data_new(char** keys, HashTableValueNew** values, int num_groups) {
    for (int i = 0; i < num_groups; i++) {
        free(keys[i]);
        free(values[i]);
    }
    free(keys);
    free(values);
}

void performance_test_new() {

    HashTableNew* tableNew = hash_table_create_new(SIZE_OF_HASHTABLE);

    // Przygotowujemy miejsce na klucze i wartości (z dopiskiem New)
    char** keys = malloc(sizeof(char*) * NUM_GROUPS);
    HashTableValueNew** values = malloc(sizeof(HashTableValueNew*) * NUM_GROUPS);

    // Generujemy dane
    generate_random_data_new(keys, values, NUM_GROUPS, NUM_AGGREGATES_PER_GROUP);

    clock_t start, end;

    // -----------------------------
    // Test: wstawianie (insert)
    // -----------------------------
    start = clock();
    for (int i = 0; i < NUM_GROUPS; i++) {
        // Tworzymy nowy wpis (HashTableEntryNew) – jeśli w Twoim kodzie
        // też jest taka struktura (np. HashTableEntryNew), pamiętaj jej użyć.
        HashTableEntryNew* entryNew = malloc(sizeof(HashTableEntryNew));
        entryNew->key = strdup(keys[i]);
        entryNew->n_values = NUM_AGGREGATES_PER_GROUP;

        // Tutaj alokujemy tablicę wartości typu HashTableValueNew
        entryNew->values = malloc(sizeof(HashTableValueNew) * NUM_AGGREGATES_PER_GROUP);
        memcpy(entryNew->values, values[i], sizeof(HashTableValueNew) * NUM_AGGREGATES_PER_GROUP);


        // Wywołujemy nową funkcję do wstawiania
        hash_table_insert_new(tableNew, entryNew);
    }
    end = clock();
    printf("Time taken for %d insert operations (New): %.2f seconds\n",
           NUM_GROUPS, (double)(end - start) / CLOCKS_PER_SEC);

    start = clock();
    for (int i = 0; i < NUM_GROUPS; i++) {
        HashTableEntryNew* foundNew = hash_table_search_new(tableNew, keys[i]);
        if (foundNew == NULL) {
            printf("Key not found (New): %s\n", keys[i]);
        }
    }
    end = clock();
    printf("Time taken for %d search operations (New): %.2f seconds\n",
           NUM_GROUPS, (double)(end - start) / CLOCKS_PER_SEC);


    start = clock();
    for (int i = 0; i < NUM_GROUPS; i++) {
        HashTableEntryNew* foundNew = hash_table_search_new(tableNew, keys[i]);
        if (foundNew != NULL) {
            // n_values to 5 zawsze
            for (int j = 0; j < foundNew->n_values; j++) {
                foundNew->values[j] = update_value_new(foundNew->values[j], values[i][j]);
            }
        }
    }
    end = clock();
    printf("Time taken for %d update operations (New): %.2f seconds\n",
           NUM_GROUPS, (double)(end - start) / CLOCKS_PER_SEC);

    // -----------------------------
    // Test: usuwanie (delete)
    // -----------------------------
    // start = clock();
    // for (int i = 0; i < NUM_GROUPS; i++) {
    //     hash_table_delete_new(tableNew, keys[i]);
    // }
    // end = clock();
    // printf("Time taken for %d delete operations (New): %.2f seconds\n",
    //        NUM_GROUPS, (double)(end - start) / CLOCKS_PER_SEC);

    // Czyszczenie pamięci
    hash_table_free_new(tableNew);
    free_random_data_new(keys, values, NUM_GROUPS);
}



void load_data_from_tsv(const char* filename, char*** keys, HashTableValue*** values, int* num_rows, int num_aggregates_per_group) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    int row_count = 0;
    char** temp_keys = NULL;
    HashTableValue** temp_values = NULL;

    // Liczymy linie i rezerwujemy miejsce na dane
    while (fgets(buffer, BUFFER_SIZE, file)) {
        row_count++;
    }

    rewind(file);
    temp_keys = malloc(sizeof(char*) * row_count);
    temp_values = malloc(sizeof(HashTableValue*) * row_count);

    int row = 0;
    while (fgets(buffer, BUFFER_SIZE, file)) {
        char* token = strtok(buffer, "\t");
        temp_keys[row] = strdup(token); // Pierwsza kolumna jako klucz

        // Pozostałe kolumny jako wartości
        temp_values[row] = malloc(sizeof(HashTableValue) * num_aggregates_per_group);
        for (int i = 0; i < num_aggregates_per_group; i++) {
            token = strtok(NULL, "\t");
            if (token) {
                temp_values[row][i].value = atoi(token); // Zakładamy, że wartości są liczbami
                temp_values[row][i].count = 1;           // Domyślna wartość count
                temp_values[row][i].aggregate_function = i % 4; // Funkcja agregacji
            } else {
                temp_values[row][i].value = 0;           // Domyślne wartości
                temp_values[row][i].count = 0;
                temp_values[row][i].aggregate_function = 0;
            }
        }
        row++;
    }

    fclose(file);

    *keys = temp_keys;
    *values = temp_values;
    *num_rows = row_count;
}


int main() {
    srand(time(NULL));

    printf("Starting performance test for HashTable...\n");
    performance_test_new();
    performance_test_old();
    printf("Performance test completed.\n");

    return 0;
}
