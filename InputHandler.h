#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <string>

// Key codes
enum KeyCode {
    KEY_NONE = 0,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_ESCAPE,
    KEY_ENTER,
    KEY_SPACE,
    KEY_P,
    KEY_S,
    KEY_M,
    KEY_R,
    KEY_Q,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_0
};

class InputHandler {
private:
    static bool terminalConfigured;
    
public:
    static void initialize();
    static void restore();
    static KeyCode getNonBlockingKey();
    static bool hasKeyPressed();
    static void clearInputBuffer();
};

#endif // INPUTHANDLER_H

