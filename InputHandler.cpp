#include "InputHandler.h"
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#endif

bool InputHandler::terminalConfigured = false;

#ifdef _WIN32
// Windows implementation
void InputHandler::initialize() {
    if (!terminalConfigured) {
        // On Windows, we use _kbhit() which doesn't need special setup
        terminalConfigured = true;
    }
}

void InputHandler::restore() {
    // Nothing to restore on Windows
    terminalConfigured = false;
}

KeyCode InputHandler::getNonBlockingKey() {
    if (!_kbhit()) {
        return KEY_NONE;
    }
    
    int ch = _getch();
    
    // Handle arrow keys on Windows (special key codes)
    if (ch == 224 || ch == 0) {
        ch = _getch();
        switch (ch) {
            case 72: return KEY_UP;
            case 80: return KEY_DOWN;
            case 75: return KEY_LEFT;
            case 77: return KEY_RIGHT;
            default: return KEY_NONE;
        }
    }
    
    // Handle regular keys
    switch (ch) {
        case 27: return KEY_ESCAPE;  // ESC
        case 13: return KEY_ENTER;   // Enter
        case 32: return KEY_SPACE;   // Space
        case 'p': case 'P': return KEY_P;
        case 's': case 'S': return KEY_S;
        case 'm': case 'M': return KEY_M;
        case 'r': case 'R': return KEY_R;
        case 'q': case 'Q': return KEY_Q;
        case '1': return KEY_1;
        case '2': return KEY_2;
        case '3': return KEY_3;
        case '4': return KEY_4;
        case '5': return KEY_5;
        case '6': return KEY_6;
        case '7': return KEY_7;
        case '8': return KEY_8;
        case '9': return KEY_9;
        case '0': return KEY_0;
        default: return KEY_NONE;
    }
}

bool InputHandler::hasKeyPressed() {
    return _kbhit() != 0;
}

void InputHandler::clearInputBuffer() {
    while (_kbhit()) {
        _getch();
    }
}

#else
// Unix/macOS/Linux implementation using termios
static struct termios oldTermios;

void InputHandler::initialize() {
    if (terminalConfigured) return;
    
    struct termios newTermios;
    tcgetattr(STDIN_FILENO, &oldTermios);
    newTermios = oldTermios;
    
    // Disable canonical mode and echo
    newTermios.c_lflag &= ~(ICANON | ECHO);
    newTermios.c_cc[VMIN] = 0;
    newTermios.c_cc[VTIME] = 0;
    
    tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);
    terminalConfigured = true;
}

void InputHandler::restore() {
    if (terminalConfigured) {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);
        terminalConfigured = false;
    }
}

KeyCode InputHandler::getNonBlockingKey() {
    fd_set readfds;
    struct timeval timeout;
    
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    
    if (select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout) <= 0) {
        return KEY_NONE;
    }
    
    if (!FD_ISSET(STDIN_FILENO, &readfds)) {
        return KEY_NONE;
    }
    
    char buffer[3];
    int bytesRead = read(STDIN_FILENO, buffer, sizeof(buffer));
    
    if (bytesRead <= 0) {
        return KEY_NONE;
    }
    
    // Handle escape sequences (arrow keys)
    if (bytesRead == 3 && buffer[0] == 27 && buffer[1] == '[') {
        switch (buffer[2]) {
            case 'A': return KEY_UP;
            case 'B': return KEY_DOWN;
            case 'C': return KEY_RIGHT;
            case 'D': return KEY_LEFT;
            default: return KEY_NONE;
        }
    }
    
    // Handle ESC key (single escape character)
    if (bytesRead == 1 && buffer[0] == 27) {
        return KEY_ESCAPE;
    }
    
    // Handle regular keys
    if (bytesRead == 1) {
        switch (buffer[0]) {
            case '\n': case '\r': return KEY_ENTER;
            case ' ': return KEY_SPACE;
            case 'p': case 'P': return KEY_P;
            case 's': case 'S': return KEY_S;
            case 'm': case 'M': return KEY_M;
            case 'r': case 'R': return KEY_R;
            case 'q': case 'Q': return KEY_Q;
            case '1': return KEY_1;
            case '2': return KEY_2;
            case '3': return KEY_3;
            case '4': return KEY_4;
            case '5': return KEY_5;
            case '6': return KEY_6;
            case '7': return KEY_7;
            case '8': return KEY_8;
            case '9': return KEY_9;
            case '0': return KEY_0;
            default: return KEY_NONE;
        }
    }
    
    return KEY_NONE;
}

bool InputHandler::hasKeyPressed() {
    fd_set readfds;
    struct timeval timeout;
    
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    
    return select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout) > 0;
}

void InputHandler::clearInputBuffer() {
    char buffer[256];
    while (hasKeyPressed()) {
        read(STDIN_FILENO, buffer, sizeof(buffer));
    }
}
#endif

