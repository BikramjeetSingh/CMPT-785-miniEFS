#include <auth.h>
#include <iostream>
#include <openssl/rsa.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <string.h>
#include <cstdlib>
#include <vector>

using namespace auth;
using namespace std;

// This function will create public/private key pairs under /publickeys folder and /privatekeys folder
// keyfile's naming convension: username_randomnumber_publickey and username_randomnumber_privatekey
// Example: Admin_2018509453_privatekey
void auth::create_RSA(string key_name) {
    size_t pos = key_name.find("_");
    string username = key_name.substr(0,pos);

    if (username == "Admin") {

        string publickey_path = "./publickeys/" + username + "_publickey";
        string privatekey_path = key_name + "_privatekey";
        
        RSA   *rsa = NULL;
        FILE  *fp  = NULL;
        FILE  *fp1  = NULL;

        BIGNUM *bne = NULL;
        bne = BN_new();
        BN_set_word(bne, 59);

        RSA *keypair = NULL;
        keypair = RSA_new();
        //2048 bit key
        RSA_generate_key_ex(keypair, 2048, bne, NULL);

        //generate public key and store to local
        fp = fopen(&publickey_path[0], "w");
        PEM_write_RSAPublicKey(fp, keypair);
        fclose(fp);
        
        //generate private key and store to local
        fp1 = fopen(&privatekey_path[0], "w");
        PEM_write_RSAPrivateKey(fp1, keypair, NULL, NULL, 0, NULL, NULL);
        fclose(fp1);
    } else {
        // normal user's public key & private key file creation
        string publickey_path = "./publickeys/" + username + "_publickey";
        string privatekey_path = "filesystem/" + username + "/" + key_name + "_privatekey";
        string privatekey_foradmin_path = "./privatekeys/" + username ;
        
        RSA   *rsa = NULL;
        FILE  *fp  = NULL;
        FILE  *fp1  = NULL;
        FILE  *fp2  = NULL;

        BIGNUM *bne = NULL;
        bne = BN_new();
        BN_set_word(bne, 59);

        RSA *keypair = NULL;
        keypair = RSA_new();
        RSA_generate_key_ex(keypair, 2048, bne, NULL);

        //generate public key and store to local
        fp = fopen(&publickey_path[0], "w");
        PEM_write_RSAPublicKey(fp, keypair);
        fclose(fp);
        
        //generate private key and store to local
        fp1 = fopen(&privatekey_path[0], "w");
        PEM_write_RSAPrivateKey(fp1, keypair, NULL, NULL, 0, NULL, NULL);
        fclose(fp1);

        //Store a copy of private key in privatekeys for admin usage only
        fp2 = fopen(&privatekey_foradmin_path[0], "w");
        PEM_write_RSAPrivateKey(fp2, keypair, NULL, NULL, 0, NULL, NULL);
        fclose(fp2);
    }

}

// This function will read RSA (public or private) keys specified by key_path
RSA * auth::read_RSAkey(string key_type, string key_path){
    
    FILE  *fp  = NULL;
    RSA   *rsa = NULL;

    fp = fopen(&key_path[0], "rb");
    if (fp == NULL){
        //invalid key_name provided
        return rsa;
    }

    if (key_type == "public"){
        PEM_read_RSAPublicKey(fp, &rsa, NULL, NULL);
        fclose(fp);        
    } else if (key_type == "private"){
        PEM_read_RSAPrivateKey(fp, &rsa, NULL, NULL);
        fclose(fp);
    }
    return rsa;
}

// This function implement RSA public key encryption
int auth::public_encrypt(int flen, unsigned char* from, unsigned char* to, RSA* key, int padding) {
    int result = RSA_public_encrypt(flen, from, to, key, padding);
    return result;
}

// This function implement RSA private key decryption
int auth::private_decrypt(int flen, unsigned char* from, unsigned char* to, RSA* key, int padding) {
    int result = RSA_private_decrypt(flen, from, to, key, padding);
    return result;
}

int auth::login_authentication(string key_name) {
    RSA *private_key;
    RSA *public_key;
    string public_key_path, private_key_path, username;

    size_t pos = key_name.find("_");
    username = key_name.substr(0,pos);
    
    public_key_path = "./publickeys/" + username + "_publickey";
    public_key = auth::read_RSAkey("public", public_key_path);

    if (username == "Admin"){
        private_key_path = key_name + "_privatekey";
    } else {
        private_key_path = "./filesystem/" + username + "/" + key_name + "_privatekey";
    }
    private_key = auth::read_RSAkey("private", private_key_path);
    
    if (public_key == NULL || private_key == NULL){
        //not such key by searching the provided key_name
        // cout << "Invalid key_name is provided. Fileserver closed." << endl;
        return 1;
    } else {
        // Successfully read public key and private key. Now User authentication
        // We uses private key to decrypt a message that was encrypted with the corresponding public key.
        // If the decryption is successful, the user is authenticated and can proceed with the session.

        char message[] = "My secret";
        char *encrypt = NULL;
        char *decrypt = NULL;

        // Do RSA encryption using public key
        encrypt = (char*)malloc(RSA_size(public_key));
        int encrypt_length = auth::public_encrypt(strlen(message) + 1, (unsigned char*)message, (unsigned char*)encrypt, public_key, RSA_PKCS1_OAEP_PADDING);
        if(encrypt_length == -1) {
            // cout << "An error occurred in public_encrypt() method" << endl;
            return 1;
        }
        
        // Try to do RSA decryption using corresponding private key
        decrypt = (char *)malloc(encrypt_length);
        int decrypt_length = auth::private_decrypt(encrypt_length, (unsigned char*)encrypt, (unsigned char*)decrypt, private_key, RSA_PKCS1_OAEP_PADDING);
        if(decrypt_length == -1) {
            // cout << "An error occurred in private_decrypt() method" << endl;
            return 1;
        }
        if (strcmp(decrypt, message) == 0){
            // cout << "Successfully login" << endl;
            // cout << decrypt << endl;
            return 0;
        } else {
            return 1;
        }
    }
}

void auth::initial_adminkey_setup(){

    // Providing a seed value
	srand((unsigned) time(NULL));
	// Get a random number
	int random = rand() % 9999999999;

    string username = "Admin";
    string key_name = username + "_" + to_string(random);

    auth::create_RSA(key_name);
    cout << "Admin Public/Private key pair has been created." << endl;
    cout << "Your private key_name is " << key_name << endl;
    cout << "Please store your key_name safely. Admin can login by command: " << endl;
    cout << "./fileserver " << key_name << endl << endl;
}