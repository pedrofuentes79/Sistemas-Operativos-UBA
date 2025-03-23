#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


void word_inverter(int n, char* word){
    int word_length = 0;
    while (word[word_length] != '\0') {
        word_length++;
    }

    if (word_length > n){
        
        // itero sobre la palabra para hacerla uppercase
        for (int i = 0; i < word_length; i++){
            word[i] = toupper(word[i]);
        }

        // swapeo los caracteres
        for (int i = 0; i < word_length / 2; i++){
            char curr_char = word[i];
            int new_char_index = word_length - i - 1;
            word[i] = word[new_char_index];
            word[new_char_index] = curr_char;
        }
    }
}

int main(int argc, char* argv[]){
    if (argc != 3) {
        printf("Usage: %s <number> <word>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    char* word = argv[2];

    word_inverter(n, word);
    printf("%s\n", word);

    return 0;
}
