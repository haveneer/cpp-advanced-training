#!/bin/sh
docker run --rm -v $(pwd):$(pwd) -w $(pwd) gcc11 gcc-11 $@

