#!/usr/bin/python3

import mysql.connector
import time

DB_HOST = "data.theochino.us"
DB_PORT = 3306
DB_USER = "usracct"
DB_PASS = "usracct"
DB_NAME = "RepMyBlockTwo"

# Function to insert data into a dictionary
def insert_data(data_dict, index, name):
    data_dict[name] = index

# Function to find data in the dictionary
def find_index(data_dict, name):
    return data_dict.get(name, -1)

def main():
    try:
        conn = mysql.connector.connect(
            host=DB_HOST,
            port=DB_PORT,
            user=DB_USER,
            password=DB_PASS,
            database=DB_NAME
        )
    except mysql.connector.Error as err:
        print("Connection error:", err)
        return

    cursor = conn.cursor()

    start_time = time.time()
    data_dict = {}

    query = "SELECT DataFirstName_ID, DataFirstName_Text FROM DataFirstName"
    cursor.execute(query)
    for row in cursor.fetchall():
        index = row[0]
        name = row[1].lower()
        insert_data(data_dict, index, name)

    stop_time = time.time()

    print(f"Phython is loading the DB Information in {stop_time - start_time:.6f} seconds")

    name = input("What is the name you are seeking?\n").strip().lower()

    start_time = time.time()
    found_index = find_index(data_dict, name)
    if found_index != -1:
        print(f"The name '{name}' is index {found_index}")
    else:
        print("Name not found")

    stop_time = time.time()

    print(f"Finding the Index Information in {stop_time - start_time:.6f} seconds")

    cursor.close()
    conn.close()

if __name__ == "__main__":
    main()
