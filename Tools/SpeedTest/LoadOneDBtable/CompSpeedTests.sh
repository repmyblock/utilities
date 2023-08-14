#!/bin/sh

echo "Compiling the executables"

echo "For Go, make sure you ran: go mod init mysql; go get -u github.com/go-sql-driver/mysql"

g++ -o CPP_SpeedTest SpeedTest.cpp -lmysqlcppconn
gcc SpeedTest.c -o C_SpeedTest -lmysqlclient
go build SpeedTest.go


