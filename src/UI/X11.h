#ifndef SELFMAKE_X11_H
#define SELFMAKE_X11_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * netzhaut - Web Browser Engine
 * Copyright (C) 2020 The netzhaut Authors
 * Published under LGPLv3
 */

#include "../Common/Result.h"

#ifdef __unix__
    #include <X11/Xlib.h>
    #include <X11/Xatom.h>
    #include <linux/input.h>
    #include <X11/Xlib-xcb.h>
    #include <xkbcommon/xkbcommon-x11.h>
    #include <sys/mman.h>
#endif

#endif

/** @addtogroup nhmakeStructs
 *  @{
 */

    typedef struct selfmake_X11 {

    #ifdef __unix__
        int screen;
        XFontStruct *Font_p;
        GC Gfx;
        Display *Display_p;
        Window Root;
        Window Window;
        Atom DeleteAtom;
        xcb_connection_t *Connection_p;
        int32_t keyboardDeviceID;
        struct xkb_keymap *Keymap_p;
        struct xkb_state *State_p;
        struct xkb_context *Context_p;
        unsigned long black, white;
        int width, height;
    #endif

    } selfmake_X11;

/** @} */

/** @addtogroup nhmakeFunctions
 *  @{
 */

    SELFMAKE_RESULT selfmake_X11_createWindow(
        char *title, int width, int heigh
    );
    
    SELFMAKE_RESULT selfmake_X11_destroyWindow(
    );
 
    SELFMAKE_RESULT selfmake_X11_getInput(
    );

    void selfmake_X11_drawTextLine(
        int x, int y, char *text_p
    );

    void selfmake_X11_drawLine(
       int x1, int y1, int x2, int y2
    );
 
    void selfmake_X11_fillRectangle(
       int x1, int y1, int x2, int y2
    );

    void selfmake_X11_clearWindow(
    );

/** @} */

#endif 
