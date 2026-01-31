#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

// ctrl + o to save = 15
// ctrl + q to quit = 17?
// backsapce =  127 or 8 (terminal dependent?)
// up arrow = 27 91 65
// down arrow = 27 91 66
// left arrow = 27 91 68
// right arrow = 27 91 67
// entire screen erase = 27 91 50 74
// cursor home = 27 91 27


/* 
    todo : load already written files, cursor movements, status bar?, filename display, remove writing every keypress => redraw screen every time
*/

/*
    note : this is rn input driven : ie like editor is waiting for first byte to enter loop
*/


struct termios orig;

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig);
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig); // saves original terminal settings 

    struct termios raw = orig; // copy to we dont destroy original terminal settings
    raw.c_lflag &= ~(ICANON | ECHO ); // icanon is no waiting for enter, ixon is for ctrl + stuff
    raw.c_iflag &= ~(IXON);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw); //to put terminal in raw mode, there are three tcs modes we use flush here
    atexit(disableRawMode); // restore when program exits
}

int main(int argc, char *argv[]) {

    char *filename = argv[1];

    enableRawMode();

    FILE *fptr;

    fptr = fopen(filename, "w");

    if (fptr == NULL) {
        printf("File Opening Error.");
    }

    char line_buffer[10000];
    int l = 0;
    char c;

    int cursor_x = 0;
    int cursor_y = 0;

    char sq1;
    char sq2;

    // user inputs => x++, backspace => x--, enter key => y++ x=0


    while (read(STDIN_FILENO, &c, 1) == 1) { // reads one byte, and no prinftscanf cuz we dont want buffering

        ///clearing entire screen
        write(STDOUT_FILENO, "\x1b[2J", 4);

        //cursor at home
        write(STDOUT_FILENO, "\x1b[H", 3);

        if (c == 27) { // esc key

            read(STDIN_FILENO, &sq1, 1);
            read(STDIN_FILENO, &sq2, 1);

            if (sq1 == 91 && sq2 == 65) {
                if (cursor_y > 0)
                    cursor_y--; //up arrow
            }

            if (sq1 == 91 && sq2 == 66)
                cursor_y++; //down arrow

            if (sq1 == 91 && sq2 == 68){
                if (cursor_x > 0)
                    cursor_x--; //left arrow
            }

            if (sq1 == 91 && sq2 == 67)
                cursor_x++; //right arrow
            
        }

                //block at the end so as to not clear the entire screen cuz of continue ka working
        if (c == 127 || c == 8) { // backspace

            if (l > 0) l--;
            cursor_x--;
            
        }

        //write(STDOUT_FILENO, "@", 1);

        // if (c >= 65)

        if (c == 17) {

            fclose(fptr);
            break;   // ctrl+q quits

        }



        if (c == 15) {
            fwrite(line_buffer, 1, l, fptr);// fputs wants null ternimated string but we need raw bytes?
            fflush(fptr);
        }


        //buffer insertion at the very end    
        if (l < sizeof(line_buffer)) {
            if (c >= 32 && c < 127) {//writable characters
                line_buffer[l++] = c;
                cursor_x++;
            }
        }

        //looping over buffer
        for (int i = 0; i < l; i++) {
            write(STDOUT_FILENO, &line_buffer[i], 1);
            // cursor_x++; this is wrong because im moving editor cursor by 'l' lengths everytime i enter something
            // this is handled by the moving cursor part 
            // model updates and rendering must be separate.
        }


        int row = cursor_y+1;
        int column = cursor_x+1; //terminal is 1 indexed

        //moving cursor
        char buff[100];
        sprintf(buff, "\x1b[%d;%dH", row, column); //only writes to memroy
        write(STDOUT_FILENO, buff, strlen(buff)); //drawing buffer


    }

    disableRawMode();
    printf("%d\n", cursor_y);
    printf("%d", cursor_x);
    

    return 0;
}
