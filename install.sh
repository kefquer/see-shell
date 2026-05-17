#!/bin/bash
set -e

echo "=== Build See Shell ==="
make clean
make

echo "=== Install ==="
sudo make Install

SHELL_PATH="/usr/local/bin/see-shell"
echo "=== Adding to path ==="
if ! grep -q "$SHELL_PATH" /etc/shells; then
    sudo sh -c "echo $SHELL_PATH" >> /etc/shells"
    echo "Added $SHELL_PATH in /etc/shells"
else
    echo "$SHELL_PATH already exist"
fi

echo ""
echo "Succeful installed"
echo "⚠️ Warning: This is an educational project. Please test thoroughly before making it your default shell."
