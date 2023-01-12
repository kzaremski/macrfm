#include <ncurses.h>

#include "main.h"

void handleDisclaimerKeyPress() {
    // Handle user input according to mode
    switch (getch()) {
        // F10 or q will exit the program
        case KEY_F(10):
        case 'q':
            
            break;
        default:
            break;
    }
}