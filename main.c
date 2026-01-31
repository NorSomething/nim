#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>

// ctrl + o to save = 15
// ctrl + q to quit = 17?
// backsapce =  127 or 8 (terminal dependent?)
// up arrow = 27 91 65
// down arrow = 27 91 66
// left arrow = 27 91 68
// right arrow = 27 91 67


/* 
    todo : load already written files, cursor movements, status bar?, filename display, remove writing every keypress => redraw screen every time
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


    while (read(STDIN_FILENO, &c, 1) == 1) { // reads one byte

        // read(STDIN_FILENO, &sq1, 1);
        // read(STDIN_FILENO, &sq2, 1);

        // printf("Key: %d\n", c); 

        //write("hi");
        //write(STDOUT_FILENO, &c, 1);

        if (c == 27) {

            read(STDIN_FILENO, &sq1, 1);
            read(STDIN_FILENO, &sq2, 1);

            if (sq1 == 91 && sq2 == 65)
                cursor_y--; //up arrow
            if (sq1 == 91 && sq2 == 66)
                //arrow down
            if (sq1 == 91 && sq2 == 68)
                //arow left
            if (sq1 == 91 && sq2 == 67)
                //arow right
            
        }

        if (c == 17) {

            fclose(fptr);
            break;   // ctrl+q quits

        }
        


        if (c == 127 || c == 8) { // backspace

            if (l > 0) l--;
            write(STDOUT_FILENO, "\b \b", 3); // ascii backspace
            cursor_x--;
 
        }

        if (c == 15) {
            fwrite(line_buffer, 1, l, fptr); // fputs wants null ternimated string but we need raw bytes?
            fflush(fptr);
        }


        //buffer insertion at the very end    
        if (l < sizeof(line_buffer)) {
            line_buffer[l++] = c;
        }


    }
    disableRawMode();
    printf("%d", cursor_y);
    

    return 0;
}
