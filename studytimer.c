/*
    Konstantin Zaremski <konstantin.zaremski@gmail.com>
        November 29, 2022
    
    studytimer
        Remake of my study timer script that I had originally made in python.
        Made in part of my efforts to learn C.
        Good old ncurses.

        Keys:
            q           Quit the application
            space       Pause/play the timer
            s           Enter study mode
            b           Enter break mode
            +           Increase time by one minute
            -           Decrease time by one minute

        How to Use:

    Copyright (c) 2022 Konstantin Zaremski

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include <ncurses.h>
#include <time.h>
#include <locale.h>

#define STUDY    2
#define BREAK    3

void printASCIINumber(int, int, char);
char intDigitToChar(int);
void correctTimerSettings(int*, int*);

// Main
int main() {
    // Set locale
    setlocale(LC_ALL, "");

    // Curses setup
    initscr();              // Initialize curses mode in the terminal window/screen
    raw();                  // Start ride 
    keypad(stdscr, TRUE);   // Get the keys from all keyboard keys (including arrow keys and other escape sequences)
    noecho();               // Do not automatically echo the keys that were pressed back to the 
    timeout(1);             // Do not wait for user input
    curs_set(0);            // Disable the cursor

    // Set up the main program loop and the getting of user input
    bool mainLoopRunning = true;
    // Modes
    int mode = STUDY;
    bool run = false;
    // Times
    int studyMinutes = 0;
    int breakMinutes = 0;
    int currentModeMinutesSetting = 0;
    int start = 0;
    // Loop
    while (mainLoopRunning) {
        // Clear the screen
        clear();
        
        // Get the size of the window
        int row, col;
        getmaxyx(stdscr, row, col);
        // Get the current time
        int now = time(NULL);
        if (!run) start = now;      // If timer is not started, no time has elapsed
        currentModeMinutesSetting = mode == STUDY ? studyMinutes : breakMinutes;

        // ** Draw the timer
        // Figure out the position of the timer
        int timerLeftPos = (col / 2) - 20;
        int timerTopPos = (row / 2) - 2;
        // Calculate the time that is remaining
        int elapsedTime = now - start;
        int remainingTime;
        if (mode == STUDY) remainingTime = (currentModeMinutesSetting * 60) - elapsedTime;
        else remainingTime = (currentModeMinutesSetting * 60) - elapsedTime;
        /*

             █████╗   █████╗       █████╗   █████╗ 
            ██╔══██╗ ██╔══██╗ ██╗ ██╔══██╗ ██╔══██╗
            ██║  ██║ ██║  ██║ ╚═╝ ██║  ██║ ██║  ██║
            ██║  ██║ ██║  ██║     ██║  ██║ ██║  ██║
            ╚█████╔╝ ╚█████╔╝ ██╗ ╚█████╔╝ ╚█████╔╝
             ╚════╝   ╚════╝  ╚═╝  ╚════╝   ╚════╝ 
               0        1      2     3        4

        */
        // Dertermine what numbers should be displayed
        int minutes = remainingTime / 60;
        int seconds = remainingTime % 60;
        char timerDigits[5] = {
            '0',
            '0',
            now % 2 == 0 || !run ? ':' : ' ',
            '0',
            '0'
        };
        timerDigits[1] = intDigitToChar(minutes % 10);
        minutes /= 10;
        timerDigits[0] = intDigitToChar(minutes % 10);
        timerDigits[4] = intDigitToChar(seconds % 10);
        seconds /= 10;
        timerDigits[3] = intDigitToChar(seconds % 10);
        // Output
        printASCIINumber(timerTopPos, timerLeftPos,                     timerDigits[0]);
        printASCIINumber(timerTopPos, timerLeftPos + 8 + 1,             timerDigits[1]);
        printASCIINumber(timerTopPos, timerLeftPos + 8 + 8 + 2,         timerDigits[2]);
        printASCIINumber(timerTopPos, timerLeftPos + 8 + 8 + 3 + 3,     timerDigits[3]);
        printASCIINumber(timerTopPos, timerLeftPos + 8 + 8 + 3 + 8 + 4, timerDigits[4]);  // right end will be at leftPos + 35

        // Render the statuses
        mvprintw(timerTopPos - 2, timerLeftPos, "%s (%s)", mode == STUDY ? "STUDY" : "BREAK", run ? "RUN" : "SET");

        // ** Handle timer finish
        if (remainingTime < 0) {
            // Set
            run = false;
            // Blink and ring 5 times
            for (int i = 0; i < 5; i++) {
                beep();
                flash();
                napms(50);
            }
            mode = mode == STUDY ? BREAK : STUDY;
            start = now;
            run = true;
        }

        // Refresh the screen
        move(0, 0);
        refresh();
        napms(10);

        // Handle user input and take action based on the key that they may have pressed
        switch(getch()) {
            // Space to toggle run
            case ' ':
                run = !run;
                break;
            // TAB to toggle the study/break mode
            case '\t':
                if (mode == STUDY) mode = BREAK;
                else mode = STUDY;
                break;
            // Adjust times
            case '=':
                // Do not adjust times if running
                if (run) break;
                // Adjust times according to the current mode
                if (mode == STUDY) studyMinutes += 1;
                else breakMinutes += 1;
                break;
            case '-':
                // Do not adjust times if running
                if (run) break;
                // Adjust times according to the current mode
                if (mode == STUDY) studyMinutes -= 1;
                else breakMinutes -= 1;
                break;
            // F10 or q will exit the program
            case KEY_F(10):
            case 'q':
                mainLoopRunning = false;
                break;
            default:
                break;
        }

        // Correct timer settings minutes (in case changes were made to them)
        correctTimerSettings(&studyMinutes, &breakMinutes);
    }

    // Exit after the main program loop is done
    curs_set(1);             // Enable the cursor
    endwin();                // End curses mode
    return 0;                // Zero exit code meaning that it exited correctly 
}

// Print an ASCII art number to the curses screen
void printASCIINumber(int row, int col, char ch) {
    // Print a different number at the current position
    // Based on the int of the ch that was provided to the function
    switch(ch) {
        case '1':
            mvaddstr(row + 0, col, "  ███╗  ");
            mvaddstr(row + 1, col, " ████║  ");
            mvaddstr(row + 2, col, "██╔██║  ");
            mvaddstr(row + 3, col, "╚═╝██║  ");
            mvaddstr(row + 4, col, "███████╗");
            mvaddstr(row + 5, col, "╚══════╝");
            break;
        case '2':
            mvaddstr(row + 0, col, "██████╗ ");
            mvaddstr(row + 1, col, "╚════██╗");
            mvaddstr(row + 2, col, "  ███╔═╝");
            mvaddstr(row + 3, col, "██╔══╝  ");
            mvaddstr(row + 4, col, "███████╗");
            mvaddstr(row + 5, col, "╚══════╝");
            break;
        case '3':
            mvaddstr(row + 0, col, "██████╗ ");
            mvaddstr(row + 1, col, "╚════██╗");
            mvaddstr(row + 2, col, " █████╔╝");
            mvaddstr(row + 3, col, " ╚═══██╗");
            mvaddstr(row + 4, col, "██████╔╝");
            mvaddstr(row + 5, col, "╚═════╝ ");
            break;
        case '4':
            mvaddstr(row + 0, col, "  ██╗██╗");
            mvaddstr(row + 1, col, " ██╔╝██║");
            mvaddstr(row + 2, col, "██╔╝ ██║");
            mvaddstr(row + 3, col, "███████║");
            mvaddstr(row + 4, col, "╚════██║");
            mvaddstr(row + 5, col, "     ╚═╝");
            break;
        case '5': 
            mvaddstr(row + 0, col, "███████╗");
            mvaddstr(row + 1, col, "██╔════╝");
            mvaddstr(row + 2, col, "██████╗ ");
            mvaddstr(row + 3, col, "╚════██╗");
            mvaddstr(row + 4, col, "██████╔╝");
            mvaddstr(row + 5, col, "╚═════╝ ");
            break;
        case '6':
            mvaddstr(row + 0, col, " █████╗ ");
            mvaddstr(row + 1, col, "██╔═══╝ ");
            mvaddstr(row + 2, col, "██████╗ ");
            mvaddstr(row + 3, col, "██╔══██╗");
            mvaddstr(row + 4, col, "╚█████╔╝");
            mvaddstr(row + 5, col, " ╚════╝ ");
            break;
        case '7':
            mvaddstr(row + 0, col, "███████╗");
            mvaddstr(row + 1, col, "╚════██║");
            mvaddstr(row + 2, col, "    ██╔╝");
            mvaddstr(row + 3, col, "   ██╔╝ ");
            mvaddstr(row + 4, col, "  ██╔╝  ");
            mvaddstr(row + 5, col, "  ╚═╝   ");
            break;
        case '8':
            mvaddstr(row + 0, col, " █████╗ ");
            mvaddstr(row + 1, col, "██╔══██╗");
            mvaddstr(row + 2, col, "╚█████╔╝");
            mvaddstr(row + 3, col, "██╔══██╗");
            mvaddstr(row + 4, col, "╚█████╔╝");
            mvaddstr(row + 5, col, " ╚════╝ ");
            break;
        case '9':
            mvaddstr(row + 0, col, " █████╗ ");
            mvaddstr(row + 1, col, "██╔══██╗");
            mvaddstr(row + 2, col, "╚██████║");
            mvaddstr(row + 3, col, " ╚═══██║");
            mvaddstr(row + 4, col, " █████╔╝");
            mvaddstr(row + 5, col, " ╚════╝ ");
            break;
        case '0':
            mvaddstr(row + 0, col, " █████╗ ");
            mvaddstr(row + 1, col, "██╔══██╗");
            mvaddstr(row + 2, col, "██║  ██║");
            mvaddstr(row + 3, col, "██║  ██║");
            mvaddstr(row + 4, col, "╚█████╔╝");
            mvaddstr(row + 5, col, " ╚════╝ ");
            break;
        case ':':
            mvaddstr(row + 0, col, "   ");
            mvaddstr(row + 1, col, "██╗");
            mvaddstr(row + 2, col, "╚═╝");
            mvaddstr(row + 3, col, "   ");
            mvaddstr(row + 4, col, "██╗");
            mvaddstr(row + 5, col, "╚═╝");
            break;
        default:
            break;
    }
}

// Convert the int of a digit in to a char of the digit from the ASCII table
char intDigitToChar(int digit) {
    return digit + '0';
}

// Correct the timer time settings
void correctTimerSettings(int *studyMinutes, int *breakMinutes) {
    // Correct study minutes
    if (*studyMinutes > 90) *studyMinutes = 90;
    else if (*studyMinutes < 0) *studyMinutes = 0;
    // Correct break minutes
    if (*breakMinutes > 90) *breakMinutes = 90;
    else if (*breakMinutes < 0) *breakMinutes = 0;
}


