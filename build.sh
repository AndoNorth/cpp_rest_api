#!/bin/bash
g++ src/restserver.cpp -lboost_system -lpthread -o build/restserver
printf "build successful!\n"