/*
    Copyright (c) 2023 Konstantin Zaremski
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

    3. All advertising materials mentioning features or use of this software must
    display the following acknowledgement:
        This product includes software developed by Konstantin Zaremski.

    4. Neither the name of the copyright holder nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDER "AS IS" AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
    EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <ncurses.h>
#include <time.h>
#include <locale.h>

#include "main.h"

// Initial mode is to show the disclaimer
int globalMode = MODE_DISCLAIMER;

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

    while (globalMode != MODE_EXIT) {
        // Clear the screen
        clear();
        
        // Render according to mode
        switch (globalMode) {
            case MODE_DISCLAIMER:
                drawDisclaimerView();
                break;
            default:
                break;
        }

        // Refresh the screen
        refresh();
        napms(10);

        

        // Correct timer settings minutes (in case changes were made to them)
        correctTimerSettings(&studyMinutes, &breakMinutes);
    }

    // Exit after the main program loop is done
    curs_set(1);             // Enable the cursor
    endwin();                // End curses mode
    return 0;                // Zero exit code meaning that it exited correctly 
}

