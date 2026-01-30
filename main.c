#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>

// ctrl + o to save


struct termios orig;

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig);
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig); // saves original terminal settings 

    struct termios raw = orig; // copy to we dont destroy original terminal settings
    raw.c_lflag &= ~(ICANON | ECHO); // icanon is no waiting for enter 

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw); //to put terminal in raw mode, there are three tcs modes we use flush here
    atexit(disableRawMode); // restore when program exits
}

int main() {
    enableRawMode();

    FILE *fptr;

    fptr = fopen("test.txt", "a");

    if (fptr == NULL) {
        printf("File Opening Error.");
    }


    char c;
    while (read(STDIN_FILENO, &c, 1) == 1) { // reads one byte
        //printf("Key: %d\n", c); 

        write(STDOUT_FILENO, &c, 1);
        
        if (c == 27) {

            fclose(fptr);

            break;   // ESC quits
        }
        //  printf("%c", c);
        fputc(c, fptr);

        





    }
}
