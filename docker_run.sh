#!/bin/sh -x

docker build -t local:halidecpp .
docker run -it -v .:/src local:halidecpp
