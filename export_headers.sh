#!/bin/bash

docker-compose build naemon

CONTAINER_ID=$(docker create ${PWD##*/}_naemon:latest)
rm -rf devtools/naemon
mkdir -p devtools/include/naemon/
docker cp $CONTAINER_ID:/opt/naemon/include/naemon/ devtools/include/naemon/
docker rm -v $CONTAINER_ID
