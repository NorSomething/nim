#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>

// ctrl + o to save = 15
// backsapce =  127 or 8 (terminal dependent?)


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

    fptr = fopen("pp.txt", "w");

    if (fptr == NULL) {
        printf("File Opening Error.");
    }

    char line_buffer[10000];
    int l = 0;
    char c;

    while (read(STDIN_FILENO, &c, 1) == 1) { // reads one byte
        //printf("Key: %d\n", c); 

        write(STDOUT_FILENO, &c, 1);
        
        if (l < sizeof(line_buffer)) {
            line_buffer[l++] = c;
        }
        
        if (c == 27) {

            fclose(fptr);
            break;   // ESC quits
        }

        if (c == 127 || c == 8) { // backspace

            if (l > 0) l--;
            write(STDOUT_FILENO, "\b \b", 3); // ascii backspace
 
        }

        if (c == 15) {
            fwrite(line_buffer, 1, l, fptr); // fputs wants null ternimated string but we need raw bytes?
            fflush(fptr);
        }

        

        





    }
}
