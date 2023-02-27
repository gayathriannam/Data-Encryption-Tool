#include <iostream>
#include <sqlite3.h>

using namespace std;

// Function to encrypt the user input
string encrypt(string input) {
    // Encryption logic here
    // For simplicity, we will use a basic Caesar Cipher
    int shift = 3;
    string output = "";
    for (char& c : input) {
        if (isalpha(c)) {
            c = toupper(c);
            c = ((c - 65 + shift) % 26) + 65;
        }
        output += c;
    }
    return output;
}

int main() {
    sqlite3* db;
    char* error = 0;

    // Open the database
    int result = sqlite3_open("data.db", &db);
    if (result != SQLITE_OK) {
        cout << "Failed to open database: " << sqlite3_errmsg(db) << endl;
        return 1;
    }

    // Create the table
    result = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS data (id INTEGER PRIMARY KEY AUTOINCREMENT, encrypted TEXT);", NULL, NULL, &error);
    if (result != SQLITE_OK) {
        cout << "Failed to create table: " << error << endl;
        sqlite3_free(error);
        return 1;
    }

    // Get the user input
    cout << "Enter a message to encrypt: ";
    string input;
    getline(cin, input);

    // Encrypt the input
    string encrypted = encrypt(input);

    // Insert the encrypted message into the database
    sqlite3_stmt* stmt;
    result = sqlite3_prepare_v2(db, "INSERT INTO data (encrypted) VALUES (?);", -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return 1;
    }
    sqlite3_bind_text(stmt, 1, encrypted.c_str(), -1, SQLITE_STATIC);
    result = sqlite3_step(stmt);
    if (result != SQLITE_DONE) {
        cout << "Failed to insert data: " << sqlite3_errmsg(db) << endl;
        return 1;
    }
    sqlite3_finalize(stmt);

    // Retrieve the encrypted message from the database and display it
    result = sqlite3_prepare_v2(db, "SELECT encrypted FROM data WHERE id = ?;", -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return 1;
    }
    sqlite3_bind_int(stmt, 1, 1);
    result = sqlite3_step(stmt);
    if (result == SQLITE_ROW) {
        string retrieved = (char*)sqlite3_column_text(stmt, 0);
        cout << "Encrypted message: " << retrieved << endl;
    }
    sqlite3_finalize(stmt);

    // Close the database
    sqlite3_close(db);

    return 0;
}
