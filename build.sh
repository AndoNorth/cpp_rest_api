#!/bin/bash
g++ src/restserver.cpp -lboost_system -lpthread -lmariadbclient -o build/restserver
printf "build successful!\n"