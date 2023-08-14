package main

import (
	"database/sql"
	"fmt"
	"log"
	"strings"
	"time"

	_ "github.com/go-sql-driver/mysql"
)

const (
	DBHost = "data.theochino.us"
	DBPort = "3306"
	DBUser = "usracct"
	DBPass = "usracct"
	DBName = "RepMyBlockTwo"
)

// DataEntry represents a data entry structure
type DataEntry struct {
	Index int
	Name  string
}

func main() {
	// Set up database connection
	dsn := fmt.Sprintf("%s:%s@tcp(%s:%s)/%s", DBUser, DBPass, DBHost, DBPort, DBName)
	db, err := sql.Open("mysql", dsn)
	if err != nil {
		log.Fatal(err)
	}
	defer db.Close()

	err = db.Ping()
	if err != nil {
		log.Fatal(err)
	}

	// Fetch data from the database
	startTime := time.Now()

	rows, err := db.Query("SELECT DataFirstName_ID, DataFirstName_Text FROM DataFirstName")
	if err != nil {
		log.Fatal(err)
	}
	defer rows.Close()

	dataMap := make(map[string]int)
	for rows.Next() {
		var index int
		var name string
		if err := rows.Scan(&index, &name); err != nil {
			log.Fatal(err)
		}
		dataMap[strings.ToLower(name)] = index
	}

	loadingTime := time.Since(startTime)
	fmt.Printf("GO loading the DB Information in %f seconds\n", loadingTime.Seconds())

	// Search for a name
	fmt.Print("What is the name you are seeking?\n")
	var nameInput string
	fmt.Scanln(&nameInput)
	name := strings.TrimSpace(strings.ToLower(nameInput))

	searchStartTime := time.Now()
	if index, found := dataMap[name]; found {
		fmt.Printf("The name %s is index %d\n", name, index)
	} else {
		fmt.Println("Name not found")
	}
	searchTime := time.Since(searchStartTime)
	fmt.Printf("Finding the Index Information in %f seconds\n", searchTime.Seconds())
}
