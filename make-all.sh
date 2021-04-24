#!/bin/bash

set -e

for i in */Makefile; do
    project="$( dirname "$i" )"
    echo '--------------------------------------------------------'
    ( set -x; cd "$project" ; make "$@" )
    echo ''
done
