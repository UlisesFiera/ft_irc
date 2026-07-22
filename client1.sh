#!/bin/bash
(
    echo "PASS 1234abcd_"
    sleep 0.2
    
    echo "NICK trump"
    sleep 0.2
    
    echo "USER trump 0 * :Donnald Trump"
    
    cat
) | nc -c localhost 6667