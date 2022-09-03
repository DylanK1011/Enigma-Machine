/* Dylan Krejci
 * dkrejci
 */

#include "enigma.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

const char *ROTOR_CONSTANTS[] = {
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ", // Identity Rotor (index 0 - and useful for testing):
        "EKMFLGDQVZNTOWYHXUSPAIBRCJ",
        "AJDKSIRUXBLHWTMCQGZNPYFVOE",
        "BDFHJLCPRTXVZNYEIWGAKMUSQO",
        "ESOVPZJAYQUIRHXLNFTGKDCMWB",
        "VZBRGITYUPSDNHLXAWMJQOFECK",
        "JPGVOUMFYQBENHZRDKASXLICTW",
        "NZJHGRCXMYSWBOUFAIVLPEKQDT",
        "FKQHTLXOCBJSPDZRAMEWNIUYGV",
};

// This method reads a character string from the keyboard and
// stores the string in the parameter msg.
// Keyboard input will be entirely uppercase and spaces followed by
// the enter key.
// The string msg should contain only uppercase letters spaces and
// terminated by the '\0' character
// Do not include the \n entered from the keyboard
void Get_Message(char msg[]){
    int i = 0;                                   // Initial index pointer
    char c = getchar();
    while (c != '\n') {                          // While there is a character to read
        if ((c >= 'A' && c <= 'Z') || c == ' ')  // Handle upper case letters - VALID / EXPECTED INPUT
            msg[i] = c;
        else if (c >= 'a' && c <= 'z')           // Handle lower case letters - CHANGE TO UPPER CASE
            msg[i] = c - 32;
        else                                     // Handle non letters - OMIT
            --i;
        ++i;
        c = getchar();
    }

    msg[i] = '\0';
    return;
}

// This function reads up to 4 characters from the keyboard
// These characters will be only digits 1 through 8. The user
// will press enter when finished entering the digits.
// The rotor string filled in by the function should only contain
// the digits terminated by the '\0' character. Do not include
// the \n entered by the user.
// The function returns the number of active rotors.
int Get_Which_Rotors(char which_rotors[]) {
    int num = 0;                     // Number of rotors
    char c = getchar();
    while (num <= 4 && c != '\n') {  // Sets maximum number of rotors allowed
        if (c >= '1' && c <= '8') {  // Ensures input char is in allowed range
            which_rotors[num] = c;
            ++num;
        }
        c = getchar();               // Get next character
    }
    which_rotors[num] = '\0';
    return num;
}

// This function reads an integer from the keyboard and returns it
// This number represents the number of rotations to apply to the
// encryption rotors.  The input will be between 0 and 25 inclusive
int Get_Rotations(){
    int num;
    scanf("%d", &num);         // Get user input and store it in num
    if(num >= 0 && num <= 25)  // Ensures input is in allowed range
        return num;
    return -1;                 // Error
}


// This function copies the rotors indicated in the which_rotors string
// into the encryption_rotors.  For example if which rotors contains the string
// {'3', '1', '\0'} Then this function copies the third and first rotors into the
// encryption rotors array in positions 0 and 1.
// encryptions_rotors[0] = "BDFHJLCPRTXVZNYEIWGAKMUSQO"
// encryptions_rotors[1] = "EKMFLGDQVZNTOWYHXUSPAIBRCJ"
void Set_Up_Rotors(char encryption_rotors[4][27], char which_rotors[5]){
    int i = 0;
    while (which_rotors[i]) {
        int index = which_rotors[i]-'0';                          // Accounts for difference between digits and their ASCII representation
        for (int j = 0; j < 26; ++j)
            encryption_rotors[i][j] = ROTOR_CONSTANTS[index][j];  // Copy jth value of rotor into array
        encryption_rotors[i][26] = '\0';
        ++i;                                                      // Increment to next rotor
    }
    return;
}


// This function rotates the characters in each of the active encryption rotors
// to the right by rotations.  For example if rotations is 3 encryption_rotors[0]
// contains "BDFHJLCPRTXVZNYEIWGAKMUSQO" then after rotation this row will contain
// SQOBDFHJLCPRTXVZNYEIWGAKMU.  Apply the same rotation to all for strings in
// encryption_rotors
void Apply_Rotation(int rotations, char encryption_rotors[4][27]) {
    char temp[4][rotations];                                      // Temporary array to store values rotated from back to front
    char arr[4][27];                                              // Temporary array to store rotated values
    for (int i = 0; i < 4; ++i) {
        for (int j = 26-rotations; j < 26; ++j)
            temp[i][j-(26-rotations)] = encryption_rotors[i][j];  // Store values rotated from back to front
        for (int k = 0; k < rotations; ++k)
            arr[i][k] = temp[i][k];                               // Copy temp into array
        for (int k = rotations; k < 26; ++k)
            arr[i][k] = encryption_rotors[i][k-rotations];        // Copy remaining characters into array
        for (int m = 0; m < 26; ++m)
            encryption_rotors[i][m] = arr[i][m];                  // Copy array back into encryption_rotors
        encryption_rotors[i][26] = '\0';
    }
    return;
}

// This function resursively encrypts a single character
// by applying the enigma machine by the number of rotors used
char ReEncrypt(char c, char rotors[4][27], int count, int ptr) {
    if (count == 0) return c;                                    // Base case
    int index = c - 'A';                                         // Uses ASCII offset of letters to calculate index of c in identity rotor
    ptr++;
    return ReEncrypt(rotors[ptr][index], rotors, count-1, ptr);  // Recurseively call ReEncrypt with next rotor and new index
}

// This function takes a string msg and applys the enigma machine to encrypt the message
// The encrypted message is stored in the string encryped_msg
// Do not change spaces, make sure your encryped_msg is a \0 terminated string
void Encrypt(char encryption_rotors[4][27], int num_active_rotors, char msg[], char encrypted_msg[]){
    int i = 0;
    while(msg[i]) {
        if (isalpha(msg[i]))
            encrypted_msg[i] = ReEncrypt(msg[i], encryption_rotors, num_active_rotors, -1);  // Calls encryption helper method
        else
            encrypted_msg[i] = msg[i];                                                       // Copy non-letters into msg
        ++i;
    }
    encrypted_msg[i] = '\0';
    return;
}

// This function recursively decrypts a single character
// by repeatedly locating a character and searching for
// a character at the same index in the identity rotor
char ReDecrypt(char c, char rotors[4][27], int ptr) {
    if (ptr < 0) return c;                       // Base Case
    int i = 0;
    while (rotors[ptr][i]) {                     // Search current rotor for c
        if (c == rotors[ptr][i]) {
            c = i + 'A';                         // Set c to character in ith index of identity rotor
            return ReDecrypt(c, rotors, --ptr);  // Recursively call ReDecryopt with decremented pointer and new character
        }
        ++i;
    }
    return 0;
}

// This function takes a string msg and applys the enigma machine to decrypt the message
// The encrypted message is stored in the string encryped_msg and the decrypted_message
// is returned as a call by reference variable
// remember the encryption rotors must be used in the reverse order to decrypt a message
// Do not change spaces, make sure your decrytped_msg is a \0 terminated string
void Decrypt(char encryption_rotors[4][27], int num_active_rotors, char encrypted_msg[], char decrypted_msg[]) {
    int i = 0;
    while (encrypted_msg[i]) {
        if (isalpha(encrypted_msg[i]))
            decrypted_msg[i] = ReDecrypt(encrypted_msg[i], encryption_rotors, num_active_rotors-1);
        else
            decrypted_msg[i] = encrypted_msg[i];
        ++i;
    }
    decrypted_msg[i] = '\0';
    return;
}
