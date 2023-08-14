#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mysql/mysql.h>

#define DB_HOST "data.theochino.us"
#define DB_PORT 3306
#define DB_USER "usracct"
#define DB_PASS "usracct"
#define DB_NAME "RepMyBlockTwo"

// Structure to hold data
typedef struct DataEntry {
    int index;
    char name[256];
    struct DataEntry *next;
} DataEntry;

// Function to insert data into the linked list
DataEntry *insertData(DataEntry *head, int index, const char *name) {
    DataEntry *newEntry = (DataEntry *)malloc(sizeof(DataEntry));
    if (newEntry == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return head;
    }

    newEntry->index = index;
    strncpy(newEntry->name, name, sizeof(newEntry->name));
    newEntry->next = head;

    return newEntry;
}

// Function to find data in the linked list
int findIndex(DataEntry *head, const char *name) {
    while (head != NULL) {
        if (strcmp(head->name, name) == 0) {
            return head->index;
        }
        head = head->next;
    }
    return -1; // Return -1 if not found
}

int main() {
    MYSQL *conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "mysql_init failed\n");
        return 1;
    }

    if (mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, DB_PORT, NULL, 0) == NULL) {
        fprintf(stderr, "mysql_real_connect failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }

    struct timespec start_time, stop_time;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start_time);

    char query[] = "SELECT DataFirstName_ID, DataFirstName_Text FROM DataFirstName";
    if (mysql_query(conn, query) != 0) {
        fprintf(stderr, "mysql_query failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "mysql_store_result failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }

    MYSQL_ROW row;
    DataEntry *dataList = NULL;

    while ((row = mysql_fetch_row(result)) != NULL) {
        int index = atoi(row[0]);
        const char *name = row[1];
        dataList = insertData(dataList, index, name);
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &stop_time);

    printf("C loading the DB Information in %f\n", (double)(stop_time.tv_sec - start_time.tv_sec) + (double)(stop_time.tv_nsec - start_time.tv_nsec) / 1e9);

    char name[256];
    printf("What is the name you are seeking?\n");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0'; // Remove trailing newline

    clock_gettime(CLOCK_MONOTONIC_RAW, &start_time);

    int foundIndex = findIndex(dataList, name);
    if (foundIndex != -1) {
        printf("The name %s is index %d\n", name, foundIndex);
    } else {
        printf("Name not found\n");
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &stop_time);

    printf("Finding the Index Information in %f\n", (double)(stop_time.tv_sec - start_time.tv_sec) + (double)(stop_time.tv_nsec - start_time.tv_nsec) / 1e9);

    // Free memory for the linked list
    while (dataList != NULL) {
        DataEntry *temp = dataList;
        dataList = dataList->next;
        free(temp);
    }

    mysql_free_result(result);
    mysql_close(conn);

    return 0;
}
