#include <auth.h>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>
#include <string.h>
#include <vector>

using namespace std;

// Write encrypted content into a file stored locally
void create_encrypted_file(string filename, char* encrypted_content, RSA* key_pair) {
    // filename += ".bin";
    FILE* encrypted_file = fopen(&filename[0], "w");
    fwrite(encrypted_content, sizeof(*encrypted_content), RSA_size(key_pair), encrypted_file);
    fclose(encrypted_file);
}

int initial_folder_setup(){
    //create "filesystem", "privatekeys","publickeys" folders
        int status1 = mkdir("filesystem", 0777);
        int status2 = mkdir("privatekeys", 0777);
        int status3 = mkdir("publickeys", 0777);

        if (status1 == 0 && status2 == 0 && status3 == 0){
            cout << "Filesystem created successfully" << endl << endl;
            return 0;
        } else {
            cerr << "Failed to create filesystem. Please check permission and try again " << endl;
            return 1;
        }
}

bool check_invalid_username(string username){
    for(int i=0;i<username.length();i++){
        if(!std::isalpha(username[i]) && !std::isdigit(username[i])) {return false;}
    }
    return true;
}

int user_folder_setup(string new_username){
    string root_folder_path = "filesystem/" + new_username;
    string personal_folder_path = root_folder_path + "/personal";
    string shared_folder_path = root_folder_path + "/shared";

    int status1 = mkdir(&root_folder_path[0], 0777);
    int status2 = mkdir(&personal_folder_path[0], 0777);
    int status3 = mkdir(&shared_folder_path[0], 0777);

    if (status1 == 0 && status2 == 0 && status3 == 0){
        cout << "User " << new_username << " folders created successfully" << endl << endl;
        return 0;
    } else {
        cerr << "Failed to create user folders. Please check permission and try again " << endl;
        return 1;
    }
}

void cmd_adduser(string new_username){
    // create user folders
    int result = user_folder_setup(new_username);
    if (result) {return;}

    // create users RSA public key and private keys (2 copies)
    // Providing a seed value
	srand((unsigned) time(NULL));
	// Get a random number
	int random = rand() % 9999999999;

    string key_name = new_username + "_" + to_string(random);
    auth::create_RSA(key_name);
    cout << "User " << new_username << " Public/Private key pair has been created." << endl;
    cout << "The private key_name is " << key_name << endl;
    cout << "Please give this key_name to user and let user know that it must be remained secret to him/herself only." << endl;
    cout << "User " << new_username << " can login by command: " << endl;
    cout << "./fileserver " << key_name << endl << endl;

}


void command_pwd(vector<string>& dir) {
    if (dir.empty()) {
        cout << "/";
    }
    else {
        for (string str:dir) {
            cout << "/" << str;
        }
    }
    cout << endl;
    return;
}


int main(int argc, char** argv) {

    string username, user_command, key_name;

    if (argc != 2) {
        cout << "Wrong command to start the fileserver. You should use command: " << endl;
        cout << "./fileserver key_name" << endl;
        return 1;
    }

    cout << "--------------------------------------------------------" << endl;
    cout << "     You are accessing Encrypted Secure File System     " << endl;
    cout << "--------------------------------------------------------" << endl << endl;

    struct stat st, st1, st2;
    if (stat("filesystem", &st) == -1 && stat("privatekeys", &st1) == -1 && stat("publickeys", &st2) == -1)
    {
        //Initial Setup
        cout << "No file system exists yet. Execute Initial setup..." << endl << endl;

        int folder_result = initial_folder_setup();
        if (folder_result == 1) {return 1;}

        auth::initial_adminkey_setup();

        cout << "Initial setup finshed, Fileserver closed. Admin now can login using the admin keyfile" << endl;
        return 0;

    } else if (stat("filesystem", &st) == -1 || stat("privatekeys", &st1) == -1 || stat("publickeys", &st2) == -1){
            cout << "Partial file system exist. Please remove folder filesystem/privatekeys/publickeys and try again." << endl;
            return 1;
    } else {
        // cout << "Directory already exists" << endl;
        // Time to do user authentication

        key_name = argv[1];
        int login_result = auth::login_authentication(key_name);
        if (login_result == 1){
            cout << "Invalid key_name is provided. Fileserver closed." << endl;
            return 1;
        } else {
            size_t pos = key_name.find("_");
            username = key_name.substr(0,pos);
            cout << "Welcome! Logged in as " << username << endl;
        }
    }

    /* ....Implement fileserver different commands...... */
    vector<string> dir;
    
    while (true){
        cout << endl;
        cout << "> ";
        getline(cin,user_command);
        // cout << "User input: " << user_command << endl;

        if (user_command == "exit") {
            cout << "Fileserver closed. Goodbye " << username << " :)" << endl;
            return 0;
        }

        /* Directory commands */
        // 1. pwd 
        //
        else if (user_command == "pwd") {
            command_pwd(dir);
        }

        // 2. cd  
        //
        // else if (user_command ....) {

        // }

        // 3. ls  
        //
        // else if (user_command ....) {

        // }

        // 4. mkdir  
        //
        // else if (user_command ....) {

        // }


        /* File commands section*/
        // 5. cat 
        //
        // else if (user_command ....) {
            // command_cat(...);
        // }

        // 6. share 
        //
        // else if (user_command ....) {

        // }

        // 7. mkfile 
        //
        // else if (user_command ....) {

        // }

        /* Admin specific feature */
        // 8. adduser <username>
        // check if user_command start with adduser
        else if (user_command.rfind("adduser", 0) == 0) {
            size_t pos = user_command.find(" ");
            if (pos == -1) {
                // to counter malicious input: adduser
                cout << "No new username provided." << endl;
                continue;
            }
            string new_username = user_command.substr(pos+1, -1);
            if (new_username == ""){
                // to counter malicious input: adduser 
                cout << "No new username provided." << endl;
                continue;
            }
            if (new_username.length() > 10){
                cout << "Invalid new username. Maximum 10 characters." << endl;
                continue;
            }
            if (strcasecmp(new_username.c_str(),"admin") == 0){
                cout << "Invalid new username: " << new_username << endl;
                continue;
            }
            if (!check_invalid_username(new_username)){
                cout << "Invalid new username. Only alphabets and numbers are allowed in a username." << endl;
                continue;
            }
            struct stat st;
            string root_folder_path = "filesystem/" + new_username;
            if (stat(&root_folder_path[0], &st) != -1){
                cout << "User " << new_username << " already exists" << endl;
                continue;
            }
            //passed all exception checks, now we create new user
            cmd_adduser(new_username);
        }


        else {
            cout << "Invalid command." << endl;
        }

    }

    
}

