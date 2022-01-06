#!/bin/bash

# Replace "Replace with your folder path" to your project folder path, such as C:/Users/zhaoj/Desktop/winter22-project0

winpty docker run --interactive --privileged -e DISPLAY \
             -p 8080:8080 \
             --tmpfs=/curr/${USER}:exec,mode=755,uid=$(id -u),gid=$(id -g) \
             --tty \
             --volume=/"Replace with your folder path"/setup-windows.sh:/opt/docker-cs118/setup.sh:ro \
             --volume=/"Replace with your folder path"/:/project:rw \
             ubuntu:20.04

# ====================== Example ===============================
# winpty docker run --interactive --rm --privileged -e DISPLAY \
#              -p 8080:8080 \
#              --tmpfs=/curr/${USER}:exec,mode=755,uid=$(id -u),gid=$(id -g) \
#              --tty \
#              --volume=/C:/Users/zhaoj/Desktop/winter22-project0/setup-windows.sh:/opt/docker-cs118/setup.sh:ro \
#              --volume=/C:/Users/zhaoj/Desktop/winter22-project0:/project:rw \
#              ubuntu:20.04
