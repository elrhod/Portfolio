#! /usr/bin/sh
FILE=src/Inc/illu_defines.h
FWSHA=$(git rev-parse HEAD | cut -c 1-7)

if ! grep -q "$FWSHA" "$FILE"; then
  sed -i "/#define FW_SHA/ c\#define FW_SHA \"${FWSHA}\"" "$FILE"
fi

