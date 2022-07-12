#!/bin/bash

cd debug/bin/

valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=memtest.log \
         ./demo

cd ../..

grep -ni "definitely" -A 10 debug/bin/memtest.log
