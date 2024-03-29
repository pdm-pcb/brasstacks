#!/bin/bash

valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=memtest.log \
         $1

grep -ni "definitely" -A 20 memtest.log
