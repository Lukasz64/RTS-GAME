#!/bin/bash
chmod +x *.sh
echo "Script executed from: ${PWD}"
PATH="${PWD}:$PATH"
export PATH
echo $PATH
