#!/bin/bash
# Prepend "g++" if the first argument is not an executable
if (! command -v "$1" &>/dev/null ); then
  echo "Command not found: prepend"
  set -- /usr/bin/g++-11 "$@"
fi

echo "CWD=$(pwd)"
ls -l
exec "$@"
