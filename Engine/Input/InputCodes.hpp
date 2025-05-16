#pragma once

// based on glfw3.h
// TODO:
//		- separate into separate mouse, key and controller code.h files

// NEEDED?
//		- joystick input
//		- error codes
//		- the rest of  the file (window codes for resizing etc, 
//			monitor stuff,...)

// key unknown
#define HS_KEY_UNKNOWN            -1

// printable keys
#define HS_KEY_SPACE              32
#define HS_KEY_APOSTROPHE         39  /* ' */
#define HS_KEY_COMMA              44  /* , */
#define HS_KEY_MINUS              45  /* - */
#define HS_KEY_PERIOD             46  /* . */
#define HS_KEY_SLASH              47  /* / */
#define HS_KEY_0                  48
#define HS_KEY_1                  49
#define HS_KEY_2                  50
#define HS_KEY_3                  51
#define HS_KEY_4                  52
#define HS_KEY_5                  53
#define HS_KEY_6                  54
#define HS_KEY_7                  55
#define HS_KEY_8                  56
#define HS_KEY_9                  57
#define HS_KEY_SEMICOLON          59  /* ; */
#define HS_KEY_EQUAL              61  /* = */
#define HS_KEY_A                  65
#define HS_KEY_B                  66
#define HS_KEY_C                  67
#define HS_KEY_D                  68
#define HS_KEY_E                  69
#define HS_KEY_F                  70
#define HS_KEY_G                  71
#define HS_KEY_H                  72
#define HS_KEY_I                  73
#define HS_KEY_J                  74
#define HS_KEY_K                  75
#define HS_KEY_L                  76
#define HS_KEY_M                  77
#define HS_KEY_N                  78
#define HS_KEY_O                  79
#define HS_KEY_P                  80
#define HS_KEY_Q                  81
#define HS_KEY_R                  82
#define HS_KEY_S                  83
#define HS_KEY_T                  84
#define HS_KEY_U                  85
#define HS_KEY_V                  86
#define HS_KEY_W                  87
#define HS_KEY_X                  88
#define HS_KEY_Y                  89
#define HS_KEY_Z                  90
#define HS_KEY_LEFT_BRACKET       91  /* [ */
#define HS_KEY_BACKSLASH          92  /* \ */
#define HS_KEY_RIGHT_BRACKET      93  /* ] */
#define HS_KEY_GRAVE_ACCENT       96  /* ` */
#define HS_KEY_WORLD_1            161 /* non-US #1 */
#define HS_KEY_WORLD_2            162 /* non-US #2 */

// function keys
#define HS_KEY_ESCAPE             256
#define HS_KEY_ENTER              257
#define HS_KEY_TAB                258
#define HS_KEY_BACKSPACE          259
#define HS_KEY_INSERT             260
#define HS_KEY_DELETE             261
#define HS_KEY_RIGHT              262
#define HS_KEY_LEFT               263
#define HS_KEY_DOWN               264
#define HS_KEY_UP                 265
#define HS_KEY_PAGE_UP            266
#define HS_KEY_PAGE_DOWN          267
#define HS_KEY_HOME               268
#define HS_KEY_END                269
#define HS_KEY_CAPS_LOCK          280
#define HS_KEY_SCROLL_LOCK        281
#define HS_KEY_NUM_LOCK           282
#define HS_KEY_PRINT_SCREEN       283
#define HS_KEY_PAUSE              284
#define HS_KEY_F1                 290
#define HS_KEY_F2                 291
#define HS_KEY_F3                 292
#define HS_KEY_F4                 293
#define HS_KEY_F5                 294
#define HS_KEY_F6                 295
#define HS_KEY_F7                 296
#define HS_KEY_F8                 297
#define HS_KEY_F9                 298
#define HS_KEY_F10                299
#define HS_KEY_F11                300
#define HS_KEY_F12                301
#define HS_KEY_F13                302
#define HS_KEY_F14                303
#define HS_KEY_F15                304
#define HS_KEY_F16                305
#define HS_KEY_F17                306
#define HS_KEY_F18                307
#define HS_KEY_F19                308
#define HS_KEY_F20                309
#define HS_KEY_F21                310
#define HS_KEY_F22                311
#define HS_KEY_F23                312
#define HS_KEY_F24                313
#define HS_KEY_F25                314
#define HS_KEY_KP_0               320
#define HS_KEY_KP_1               321
#define HS_KEY_KP_2               322
#define HS_KEY_KP_3               323
#define HS_KEY_KP_4               324
#define HS_KEY_KP_5               325
#define HS_KEY_KP_6               326
#define HS_KEY_KP_7               327
#define HS_KEY_KP_8               328
#define HS_KEY_KP_9               329
#define HS_KEY_KP_DECIMAL         330
#define HS_KEY_KP_DIVIDE          331
#define HS_KEY_KP_MULTIPLY        332
#define HS_KEY_KP_SUBTRACT        333
#define HS_KEY_KP_ADD             334
#define HS_KEY_KP_ENTER           335
#define HS_KEY_KP_EQUAL           336
#define HS_KEY_LEFT_SHIFT         340
#define HS_KEY_LEFT_CONTROL       341
#define HS_KEY_LEFT_ALT           342
#define HS_KEY_LEFT_SUPER         343
#define HS_KEY_RIGHT_SHIFT        344
#define HS_KEY_RIGHT_CONTROL      345
#define HS_KEY_RIGHT_ALT          346
#define HS_KEY_RIGHT_SUPER        347
#define HS_KEY_MENU               348

#define HS_KEY_LAST               HS_KEY_MENU

// modifiers
#define HS_MOD_SHIFT           0x0001 // shift key(s)
#define HS_MOD_CONTROL         0x0002 // cntrl key(s)
#define HS_MOD_ALT             0x0004 // alt key(s)
#define HS_MOD_SUPER           0x0008 // super? key(s)
#define HS_MOD_CAPS_LOCK       0x0010 // caps lock
#define HS_MOD_NUM_LOCK        0x0020 // num lock

// mouse buttons
#define HS_MOUSE_BUTTON_1         0
#define HS_MOUSE_BUTTON_2         1
#define HS_MOUSE_BUTTON_3         2
#define HS_MOUSE_BUTTON_4         3
#define HS_MOUSE_BUTTON_5         4
#define HS_MOUSE_BUTTON_6         5
#define HS_MOUSE_BUTTON_7         6
#define HS_MOUSE_BUTTON_8         7
#define HS_MOUSE_BUTTON_LEFT      HS_MOUSE_BUTTON_1
#define HS_MOUSE_BUTTON_RIGHT     HS_MOUSE_BUTTON_2
#define HS_MOUSE_BUTTON_MIDDLE    HS_MOUSE_BUTTON_3
#define HS_MOUSE_BUTTON_LAST      HS_MOUSE_BUTTON_8

// controller input -- original
//#define HS_GAMEPAD_BUTTON_A               0
//#define HS_GAMEPAD_BUTTON_B               1
//#define HS_GAMEPAD_BUTTON_X               2
//#define HS_GAMEPAD_BUTTON_Y               3
//#define HS_GAMEPAD_BUTTON_LEFT_BUMPER     4
//#define HS_GAMEPAD_BUTTON_RIGHT_BUMPER    5
//#define HS_GAMEPAD_BUTTON_BACK            6
//#define HS_GAMEPAD_BUTTON_START           7
//#define HS_GAMEPAD_BUTTON_GUIDE           8
//#define HS_GAMEPAD_BUTTON_LEFT_THUMB      9
//#define HS_GAMEPAD_BUTTON_RIGHT_THUMB     10
//#define HS_GAMEPAD_BUTTON_DPAD_UP         11
//#define HS_GAMEPAD_BUTTON_DPAD_RIGHT      12
//#define HS_GAMEPAD_BUTTON_DPAD_DOWN       13
//#define HS_GAMEPAD_BUTTON_DPAD_LEFT       14
//#define HS_GAMEPAD_BUTTON_LAST            HS_GAMEPAD_BUTTON_DPAD_LEFT

// for my xbox controller
#define HS_GAMEPAD_BUTTON_A               0
#define HS_GAMEPAD_BUTTON_B               1
#define HS_GAMEPAD_BUTTON_X               2
#define HS_GAMEPAD_BUTTON_Y               3
#define HS_GAMEPAD_BUTTON_LEFT_BUMPER     4
#define HS_GAMEPAD_BUTTON_RIGHT_BUMPER    5
#define HS_GAMEPAD_BUTTON_BACK            6
#define HS_GAMEPAD_BUTTON_START           7
#define HS_GAMEPAD_BUTTON_LEFT_THUMB           8
#define HS_GAMEPAD_BUTTON_RIGHT_THUMB      9
#define HS_GAMEPAD_BUTTON_GUIDE    10
#define HS_GAMEPAD_BUTTON_DPAD_UP         11
#define HS_GAMEPAD_BUTTON_DPAD_RIGHT      12
#define HS_GAMEPAD_BUTTON_DPAD_DOWN       13
#define HS_GAMEPAD_BUTTON_DPAD_LEFT       14
#define HS_GAMEPAD_BUTTON_LAST            HS_GAMEPAD_BUTTON_DPAD_LEFT


#define HS_GAMEPAD_BUTTON_CROSS       HS_GAMEPAD_BUTTON_A
#define HS_GAMEPAD_BUTTON_CIRCLE      HS_GAMEPAD_BUTTON_B
#define HS_GAMEPAD_BUTTON_SQUARE      HS_GAMEPAD_BUTTON_X
#define HS_GAMEPAD_BUTTON_TRIANGLE    HS_GAMEPAD_BUTTON_Y

#define HS_GAMEPAD_AXIS_LEFT_X        0
#define HS_GAMEPAD_AXIS_LEFT_Y        1
#define HS_GAMEPAD_AXIS_RIGHT_X       2
#define HS_GAMEPAD_AXIS_RIGHT_Y       3
#define HS_GAMEPAD_AXIS_LEFT_TRIGGER  4
#define HS_GAMEPAD_AXIS_RIGHT_TRIGGER 5
#define HS_GAMEPAD_AXIS_LAST          HS_GAMEPAD_AXIS_RIGHT_TRIGGER