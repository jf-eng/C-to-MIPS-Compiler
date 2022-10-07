#!/bin/bash
cat $2 | ./bin/compiler 1> $4 2> /dev/null
# cat $2 | ./bin/compiler > $4