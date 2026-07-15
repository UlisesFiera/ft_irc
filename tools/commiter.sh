#!/bin/bash
# This script is used to push the project to the remote repository. It clears the build and pushes the changes. Just run it and pass the commit message after running it.

echo "Enter the commit message (no quotes needed):"
read message

make fclean
git add -A
git commit -m "$message"