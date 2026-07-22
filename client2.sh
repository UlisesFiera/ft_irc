#!/bin/bash
(
    echo "PASS 1234abcd_"
    sleep 0.2
    
    echo "NICK biden"
    sleep 0.2
    
    echo "USER biden 0 * :Joe Biden"
	sleep 0.2

	echo "JOIN #test"
    
    cat
) | nc -c localhost 6667