# CS118 Project 1: HTTP Server

This is the repo for spring23 CS118 project 1, which consists of the source code for a HTTP server that is capable of sending the file requested by a client through HTTP.

## Provided Files

- `project` folder stores Makefile and server.c.
- `docker-compose.yaml` and `Dockerfile` are files configuring the Docker containers.

## Bash commands

```bash
# Setup the container(s) (make setup)
docker compose up -d

# Bash into the container (make shell)
docker compose exec node1 bash

# Remove container(s) and the Docker image (make clean)
docker compose down -v --rmi all --remove-orphans
```

## Environment

- OS: ubuntu 22.04
- IP: 192.168.10.225. NOT accessible from the host machine.
- Port forwarding: container 8080 <-> host 8080
  - Use http://localhost:8080 to access the HTTP server in the container.
- Files in this repo are in the `/project` folder. That means, `server.c` is `/project/project/server.c` in the container.

## TODO
To compile the source code and create an executable, run

`make`

To run the server and receive incoming HTTP request through localhost port 8080, run

`./server`