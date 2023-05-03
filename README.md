# CMPT-785-miniEFS

## Available Commands

*   `adduser <username>` (for admins only)
*   `cd <directory>`
*   `pwd`
*   `ls`
*   `cat <filename>`
*   `share <filename> <username>`
*   `mkdir <directoryname>`
*   `mkfile <filename> <contents>`
*   `exit`

## Installation
In addition to openssl, please also make sure jsoncpp is installed before you compile the code

```bash
sudo apt install libjsoncpp-dev
```

Compile and prepare the binary:

```bash
cd /your-path-to/CMPT-785-miniEFS
g++ main.cpp -std=c++17 -o fileserver -L usr/bin/openssl/lib -I usr/bin/openssl/include -lssl -lcrypto -ljsoncpp -w
chmod +x fileserver
```

Run the fileserver binary:

```bash
./fileserver key_name
```
