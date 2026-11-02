#pragma once

enum class InputKey {
    NONE,
    QUIT,

    // Letters
    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

    // Number row
    NUM_0, NUM_1, NUM_2, NUM_3, NUM_4,
    NUM_5, NUM_6, NUM_7, NUM_8, NUM_9,

    // Punctuation
    MINUS,
    EQUALS,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    BACKSLASH,
    SEMICOLON,
    APOSTROPHE,
    GRAVE,
    COMMA,
    PERIOD,
    SLASH,

    // Whitespace and editing
    SPACE,
    TAB,
    ENTER,
    ESCAPE,
    BACKSPACE,
    DELETE_KEY,
    INSERT,

    // Navigation
    UP,
    DOWN,
    LEFT,
    RIGHT,
    HOME,
    END,
    PAGE_UP,
    PAGE_DOWN,

    // Function keys
    F1, F2, F3, F4, F5, F6,
    F7, F8, F9, F10, F11, F12,

    // Modifiers
    LEFT_SHIFT,
    RIGHT_SHIFT,
    LEFT_CTRL,
    RIGHT_CTRL,
    LEFT_ALT,
    RIGHT_ALT,
    LEFT_GUI,
    RIGHT_GUI,
    CAPS_LOCK,
    NUM_LOCK,
    SCROLL_LOCK,

    // Numeric keypad
    KP_0, KP_1, KP_2, KP_3, KP_4,
    KP_5, KP_6, KP_7, KP_8, KP_9,

    KP_DECIMAL,
    KP_DIVIDE,
    KP_MULTIPLY,
    KP_MINUS,
    KP_PLUS,
    KP_ENTER,
    KP_EQUALS,

    // Additional keys
    PRINT_SCREEN,
    PAUSE,
    MENU
};
