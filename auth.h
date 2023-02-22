#ifndef MINIIEFS_AUTH // include guard
#define MINIIEFS_AUTH

#include <cstdlib>
#include <openssl/rsa.h>
#include <vector>
#include <string>

using namespace std;

namespace auth
{
    RSA * read_RSAkey(string key_type, string key_path);
    void create_RSA(string key_name);
    int public_encrypt(int flen, unsigned char* from, unsigned char* to, RSA* key, int padding);
    int private_decrypt(int flen, unsigned char* from, unsigned char* to, RSA* key, int padding);
    int login_authentication(string key_name);
    void initial_adminkey_setup();
}

#endif