#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <sys/stat.h> //for getting last time file saved data

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

int get_index_from_cursor(char *buf, int len, int cx, int cy) {
    
    int x = 0, y = 0;

    for (int i = 0; i < len; i++) {
        if (y == cy && x == cx)
            return i;

        if (buf[i] == '\n') {
            y++;
            x = 0;
        } else {
            x++;
        }
    }

    return len; 
}

int main_menu(char *filename) {
    printf("Enter 1 to Create a new file. \nEnter 2 to Open an existing file.");

    int choice = 0;

    scanf("%d", &choice); //takes input with new line
    getchar(); //removes new line 

    if (choice == 1) {
        printf("Enter file name : ");
        fgets(filename, 256, stdin);
        filename[strcspn(filename, "\n")] = 0; // removing new line char cuz fget keeps it, strcspn finds index
        return choice;
        
    }
    else if (choice == 2) {
        printf("Press Ctrl+O to load the file once inside the editor. \nEnter existing file name : ");
        fgets(filename, 256, stdin);
        filename[strcspn(filename, "\n")] = 0;
        return choice;
    }
    else {
        printf("error");
        
        return choice;

    }

}


int main(int argc, char *argv[]) {

    struct winsize w;
    int choice;
    struct stat status; //for getting last time written

    int fptr_open = 0;

    //gettign screen size
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1){
        perror("ioctrl");
        return -1;
    }

    int last_row = w.ws_row;
    int last_column = w.ws_col;

    char filename[256];

    choice = 0;

    if (argc >= 2) {
        strcpy(filename, argv[1]);
        choice = 2;
    }

    else {
        choice = main_menu(filename);
    }

    int l; //length of contents of file (used for file buffer)

    char time_buffer[100];

    FILE *fptr;

    l = 0;

    char line_buffer[10000];
    
    char c;

    int cursor_x = 0;
    int cursor_y = 0;

    char sq1;
    char sq2;

    // user inputs => x++, backspace => x--, enter key => y++ x=0

    if (choice == 1) {
        fptr = fopen(filename, "w");
        l = 0;
        strcpy(time_buffer, "Not Saved Yet.");
        
    }

    if (choice == 2) {
        
        fptr = fopen(filename, "r");

        if (fptr == NULL) {
            printf("File not found.\n Exiting.. \n");
            return 0;
        }

        while ((c = fgetc(fptr)) != EOF ) {
            line_buffer[l++] = c;
        }

        line_buffer[l] = '\0';

        int tc = 0; //temp counter

        for (int i = 0; i < l; i++) {
            if (line_buffer[i] == '\n')
                tc++;
        }

        if (l > 0 && line_buffer[l-1] == '\n') { 
            cursor_y = tc;
            cursor_x = 0;
        }
        else {
            cursor_x = 0;
        }
        

        fstat(fileno(fptr), &status);
        strcpy(time_buffer, ctime(&status.st_ctime));

        fclose(fptr);

    }

    if (fptr == NULL) {
        printf("File Opening Error.");
    }

    enableRawMode();

    //pre rendering stuff
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    for (int i = 0; i < l; i++) { 
        write(STDOUT_FILENO, &line_buffer[i], 1);
    }


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

            if (sq1 == 91 && sq2 == 66) {
                cursor_y++; //down arrow
                // line_buffer[l++] = '\n';
                
                // for (int i = 0; i < cursor_x; i++)
                //     line_buffer[l++] = ' ';
            }

            if (sq1 == 91 && sq2 == 68){
                if (cursor_x > 0)
                    cursor_x--; //left arrow
            }

            if (sq1 == 91 && sq2 == 67) {
                cursor_x++; //right arrow
                // line_buffer[l++] = ' ';

            }


            
        }

        if (c == 127 || c == 8) { // backspace

            if (l > 0) {
                int pos = get_index_from_cursor(line_buffer, l, cursor_x, cursor_y);

                if (pos > 0) {
                    memmove(&line_buffer[pos - 1], &line_buffer[pos], l - pos);
                    l--;
                    cursor_x--;
                }
                
                line_buffer[l] = '\0';
            }
        }

        if (c == 17) {

            if (fptr_open)
                fclose(fptr);
            break;   // ctrl+q quits

        }

        if (c == 13 || c == 10) { // enter key
            cursor_y++;
            cursor_x=0;
            line_buffer[l++] = '\n';
            
        }



        if (c == 15) {

            if (fptr_open) 
                fclose(fptr);

            fptr = fopen(filename, "w");

            fptr_open = 1;

            fwrite(line_buffer, 1, l, fptr);// fputs wants null ternimated string but we need raw bytes?
            fflush(fptr);

            //getting time data
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            char temp2[50];
            strftime(temp2, sizeof(temp2), "%d-%m-%Y %H:%M:%S", t);
            strcpy(time_buffer, temp2);

        }


        //buffer insertion at the very end    
        if (l < sizeof(line_buffer)) {
            if (c >= 32 && c < 127) {//writable characters
                int pos = get_index_from_cursor(line_buffer, l, cursor_x, cursor_y);
                memmove(&line_buffer[pos + 1], &line_buffer[pos], l - pos); //move 1 step right
                line_buffer[pos] = c;
                cursor_x++;
                l++;
                line_buffer[l] = '\0';
            }
        }

        int line = 1;
        int at_line_start = 1;

        //looping over buffer
        for (int i = 0; i < l; i++) { 

            
            // cursor_x++; this is wrong because im moving editor cursor by 'l' lengths everytime i enter something
            // this is handled by the moving cursor part 
            // model updates and rendering must be separate.

            // writing  out lines
            if (at_line_start) {
        
                char ln[16];
                int len = snprintf(ln, sizeof(ln), "%4d | ", line++);
                write(STDOUT_FILENO, ln, len);
                at_line_start = 0;

            }

            write(STDOUT_FILENO, &line_buffer[i], 1);

            if (line_buffer[i] == '\n') {
                at_line_start = 1;
            }

        }

        int row = cursor_y+1;
        int column = cursor_x+1 + 6; //terminal is 1 indexed, 6 is length of line number width

        // status bar 
        char temp[100];
        sprintf(temp, "\x1b[%d;1H", last_row);
        write(STDOUT_FILENO, temp, strlen(temp));
        char tt[1000];
        
        sprintf(tt, "Row : %d || Column : %d || %s || Last Saved Time : %s", row, column, filename, time_buffer);
        write(STDOUT_FILENO, tt, strlen(tt));

        //moving cursor
        char buff[100];
        sprintf(buff, "\x1b[%d;%dH", row, column); //only writes to memroy
        write(STDOUT_FILENO, buff, strlen(buff)); //drawing buffer



    }

    disableRawMode();
    //printf("%d\n", cursor_y);
    //printf("%d", cursor_x);
    

    return 0;
}