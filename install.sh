#!/usr/bin/env bash

# exit when an error occures
set -e

# first build the project
make build

# install the binary in /usr/local/libexec/bFLT-loader
sudo install -Dm755 build/bFLT-loader /usr/local/libexec/bflt-loader

echo "executable copied successfully to /usr/local/libexec/bflt-loader"

# register the executable as a loader for bflt files
printf '%s\n' ':bflt:M::bFLT::/usr/local/libexec/bflt-loader:' |
    sudo tee /etc/binfmt.d/80-bflt.conf > /dev/null

# restart registration to make systemd see the bflt loader
sudo systemctl restart systemd-binfmt.service

if [[ -e /proc/sys/fs/binfmt_misc/bflt ]]; then
    echo "bFLT loader registered successfully"
else
    echo "error: bFLT loader registration failed" >&2
    exit 1
fi
