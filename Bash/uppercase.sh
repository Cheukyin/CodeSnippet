#!/bin/bash

# uppercase.sh : Changes input to uppercase.
# usage example: ls -l | ./uppercase.sh

# Letter ranges must be quoted
#+ to prevent filename generation from single-letter filenames.
tr 'a-z' 'A-Z'

exit 0