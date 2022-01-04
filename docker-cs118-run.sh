#!/bin/bash
basedir="$(dirname "$(realpath "$0")")"
xhost +
docker run --interactive --rm --privileged -e DISPLAY \
             -p 8080:8080 \
             --tmpfs=/curr/${USER}:exec,mode=755,uid=$(id -u),gid=$(id -g) \
             --tty \
             --volume=${basedir}/setup.sh:/opt/docker-cs118/setup.sh:ro \
             --volume=${basedir}/project:/project:rw \
             -v /tmp/.X11-unix:/tmp/.X11-unix \
             -v /lib/modules:/lib/modules \
             ubuntu:20.04
