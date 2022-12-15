#!/bin/bash
chmod +x *.sh
chmod +x r
chmod +x b
echo "Script executed from: ${PWD}"
PATH="${PWD}:$PATH"
export PATH
echo $PATH
