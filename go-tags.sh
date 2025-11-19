#!/usr/bin/env bash

set -x
ctags -R --languages=C++ --kinds-all=* --fields=+l --extras=+q .
