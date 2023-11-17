#!/bin/bash

if [ $# -ne 1 ]; then
  echo "Missing tool argument"
  exit 1
fi
makedng="$1"

echo "Should run $makedng"