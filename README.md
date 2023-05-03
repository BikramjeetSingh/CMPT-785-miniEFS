# CMPT-785-miniEFS
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
