#!/bin/sh

exec docker run -it -v "$(pwd)/..:/build" woff2-amazon-1
