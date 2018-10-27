#include "RenderViewWidget.h"
#include <QResizeEvent>

#include <thirdparty/SDL2/SDL.h>

#include <gplay-engine.h>

//------------------------------------------------------------------------------------------------------
// key utilities to convert Qt key to SDL key
//------------------------------------------------------------------------------------------------------
static QMap<Qt::Key, SDL_Keycode> __keymap;
static void __initKeyMap();
static Uint16 __convertQtKeyModifierToSDL(Qt::KeyboardModifiers qtKeyModifiers);
static SDL_Keycode __convertQtKeyToSDL(Qt::Key qtKey);

static QMap<Qt::Key, SDL_Scancode> __keymapScancode;
static void __initScancodeKeyMap();
static SDL_Scancode __convertQtKeyToScancodeSDL(Qt::Key qtKey);



//------------------------------------------------------------------------------------------------------
// map keys Qt/SDL
//------------------------------------------------------------------------------------------------------
void __initKeyMap()
{
    __keymap[Qt::Key_unknown]     = SDLK_UNKNOWN;
    __keymap[Qt::Key_Escape]      = SDLK_ESCAPE;
    __keymap[Qt::Key_Tab]         = SDLK_TAB;
    __keymap[Qt::Key_Backspace]   = SDLK_BACKSPACE;
    __keymap[Qt::Key_Return]      = SDLK_RETURN;
    __keymap[Qt::Key_Enter]       = SDLK_KP_ENTER;
    __keymap[Qt::Key_Insert]      = SDLK_INSERT;
    __keymap[Qt::Key_Delete]      = SDLK_DELETE;
    __keymap[Qt::Key_Pause]       = SDLK_PAUSE;
    __keymap[Qt::Key_Print]       = SDLK_PRINTSCREEN;
    __keymap[Qt::Key_SysReq]      = SDLK_SYSREQ;
    __keymap[Qt::Key_Home]        = SDLK_HOME;
    __keymap[Qt::Key_End]         = SDLK_END;
    __keymap[Qt::Key_Left]        = SDLK_LEFT;
    __keymap[Qt::Key_Right]       = SDLK_RIGHT;
    __keymap[Qt::Key_Up]          = SDLK_UP;
    __keymap[Qt::Key_Down]        = SDLK_DOWN;
    __keymap[Qt::Key_PageUp]      = SDLK_PAGEUP;
    __keymap[Qt::Key_PageDown]    = SDLK_PAGEDOWN;
    __keymap[Qt::Key_Shift]       = SDLK_LSHIFT;
    __keymap[Qt::Key_Control]     = SDLK_LCTRL;
    __keymap[Qt::Key_Alt]         = SDLK_LALT;
    __keymap[Qt::Key_CapsLock]    = SDLK_CAPSLOCK;
    __keymap[Qt::Key_NumLock]     = SDLK_NUMLOCKCLEAR;
    __keymap[Qt::Key_ScrollLock]  = SDLK_SCROLLLOCK;
    __keymap[Qt::Key_F1]          = SDLK_F1;
    __keymap[Qt::Key_F2]          = SDLK_F2;
    __keymap[Qt::Key_F3]          = SDLK_F3;
    __keymap[Qt::Key_F4]          = SDLK_F4;
    __keymap[Qt::Key_F5]          = SDLK_F5;
    __keymap[Qt::Key_F6]          = SDLK_F6;
    __keymap[Qt::Key_F7]          = SDLK_F7;
    __keymap[Qt::Key_F8]          = SDLK_F8;
    __keymap[Qt::Key_F9]          = SDLK_F9;
    __keymap[Qt::Key_F10]         = SDLK_F10;
    __keymap[Qt::Key_F11]         = SDLK_F11;
    __keymap[Qt::Key_F12]         = SDLK_F12;
    __keymap[Qt::Key_F13]         = SDLK_F13;
    __keymap[Qt::Key_F14]         = SDLK_F14;
    __keymap[Qt::Key_F15]         = SDLK_F15;
    __keymap[Qt::Key_Menu]        = SDLK_MENU;
    __keymap[Qt::Key_Help]        = SDLK_HELP;

    // A-Z
    for(int key='A'; key<='Z'; key++)
        __keymap[Qt::Key(key)] = key + 32;

    // 0-9
    for(int key='0'; key<='9'; key++)
        __keymap[Qt::Key(key)] = key;
}

//------------------------------------------------------------------------------------------------------
// get SDL key from Qt key
//------------------------------------------------------------------------------------------------------
SDL_Keycode __convertQtKeyToSDL(Qt::Key qtKey)
{
    SDL_Keycode sldKey = __keymap.value(Qt::Key(qtKey));

    if(sldKey == 0)
        gplay::print("Warning: Key %d not mapped", qtKey);

    return sldKey;
}

//------------------------------------------------------------------------------------------------------
// get SDL key modifier from Qt key modifier
//------------------------------------------------------------------------------------------------------
Uint16 __convertQtKeyModifierToSDL(Qt::KeyboardModifiers qtKeyModifiers)
{
    Uint16 sdlModifiers = KMOD_NONE;

    if(qtKeyModifiers.testFlag(Qt::ShiftModifier))
        sdlModifiers |= KMOD_LSHIFT | KMOD_RSHIFT;
    if(qtKeyModifiers.testFlag(Qt::ControlModifier))
        sdlModifiers |= KMOD_LCTRL | KMOD_RCTRL;
    if(qtKeyModifiers.testFlag(Qt::AltModifier))
        sdlModifiers |= KMOD_LALT | KMOD_RALT;

    return sdlModifiers;
}

//------------------------------------------------------------------------------------------------------
// map scanecode keys Qt/SDL
//------------------------------------------------------------------------------------------------------
void __initScancodeKeyMap()
{
    __keymapScancode[Qt::Key_unknown]     = SDL_SCANCODE_UNKNOWN;
    __keymapScancode[Qt::Key_Escape]      = SDL_SCANCODE_ESCAPE;
    __keymapScancode[Qt::Key_Tab]         = SDL_SCANCODE_TAB;
    __keymapScancode[Qt::Key_Backspace]   = SDL_SCANCODE_BACKSPACE;
    __keymapScancode[Qt::Key_Return]      = SDL_SCANCODE_RETURN;
    __keymapScancode[Qt::Key_Enter]       = SDL_SCANCODE_KP_ENTER;
    __keymapScancode[Qt::Key_Insert]      = SDL_SCANCODE_INSERT;
    __keymapScancode[Qt::Key_Delete]      = SDL_SCANCODE_DELETE;
    __keymapScancode[Qt::Key_Pause]       = SDL_SCANCODE_PAUSE;
    __keymapScancode[Qt::Key_Print]       = SDL_SCANCODE_PRINTSCREEN;
    __keymapScancode[Qt::Key_SysReq]      = SDL_SCANCODE_SYSREQ;
    __keymapScancode[Qt::Key_Home]        = SDL_SCANCODE_HOME;
    __keymapScancode[Qt::Key_End]         = SDL_SCANCODE_END;
    __keymapScancode[Qt::Key_Left]        = SDL_SCANCODE_LEFT;
    __keymapScancode[Qt::Key_Right]       = SDL_SCANCODE_RIGHT;
    __keymapScancode[Qt::Key_Up]          = SDL_SCANCODE_UP;
    __keymapScancode[Qt::Key_Down]        = SDL_SCANCODE_DOWN;
    __keymapScancode[Qt::Key_PageUp]      = SDL_SCANCODE_PAGEUP;
    __keymapScancode[Qt::Key_PageDown]    = SDL_SCANCODE_PAGEDOWN;
    __keymapScancode[Qt::Key_Shift]       = SDL_SCANCODE_LSHIFT;
    __keymapScancode[Qt::Key_Control]     = SDL_SCANCODE_LCTRL;
    __keymapScancode[Qt::Key_Alt]         = SDL_SCANCODE_LALT;
    __keymapScancode[Qt::Key_CapsLock]    = SDL_SCANCODE_CAPSLOCK;
    __keymapScancode[Qt::Key_NumLock]     = SDL_SCANCODE_NUMLOCKCLEAR;
    __keymapScancode[Qt::Key_ScrollLock]  = SDL_SCANCODE_SCROLLLOCK;
    __keymapScancode[Qt::Key_F1]          = SDL_SCANCODE_F1;
    __keymapScancode[Qt::Key_F2]          = SDL_SCANCODE_F2;
    __keymapScancode[Qt::Key_F3]          = SDL_SCANCODE_F3;
    __keymapScancode[Qt::Key_F4]          = SDL_SCANCODE_F4;
    __keymapScancode[Qt::Key_F5]          = SDL_SCANCODE_F5;
    __keymapScancode[Qt::Key_F6]          = SDL_SCANCODE_F6;
    __keymapScancode[Qt::Key_F7]          = SDL_SCANCODE_F7;
    __keymapScancode[Qt::Key_F8]          = SDL_SCANCODE_F8;
    __keymapScancode[Qt::Key_F9]          = SDL_SCANCODE_F9;
    __keymapScancode[Qt::Key_F10]         = SDL_SCANCODE_F10;
    __keymapScancode[Qt::Key_F11]         = SDL_SCANCODE_F11;
    __keymapScancode[Qt::Key_F12]         = SDL_SCANCODE_F12;
    __keymapScancode[Qt::Key_F13]         = SDL_SCANCODE_F13;
    __keymapScancode[Qt::Key_F14]         = SDL_SCANCODE_F14;
    __keymapScancode[Qt::Key_F15]         = SDL_SCANCODE_F15;
    __keymapScancode[Qt::Key_Menu]        = SDL_SCANCODE_MENU;
    __keymapScancode[Qt::Key_Help]        = SDL_SCANCODE_HELP;


    __keymapScancode[Qt::Key_A] = SDL_SCANCODE_A;
    __keymapScancode[Qt::Key_B] = SDL_SCANCODE_B;
    __keymapScancode[Qt::Key_C] = SDL_SCANCODE_C;
    __keymapScancode[Qt::Key_D] = SDL_SCANCODE_D;
    __keymapScancode[Qt::Key_E] = SDL_SCANCODE_E;
    __keymapScancode[Qt::Key_F] = SDL_SCANCODE_F;
    __keymapScancode[Qt::Key_G] = SDL_SCANCODE_G;
    __keymapScancode[Qt::Key_H] = SDL_SCANCODE_H;
    __keymapScancode[Qt::Key_I] = SDL_SCANCODE_I;
    __keymapScancode[Qt::Key_J] = SDL_SCANCODE_J;
    __keymapScancode[Qt::Key_K] = SDL_SCANCODE_K;
    __keymapScancode[Qt::Key_L] = SDL_SCANCODE_L;
    __keymapScancode[Qt::Key_M] = SDL_SCANCODE_M;
    __keymapScancode[Qt::Key_N] = SDL_SCANCODE_N;
    __keymapScancode[Qt::Key_O] = SDL_SCANCODE_O;
    __keymapScancode[Qt::Key_P] = SDL_SCANCODE_P;
    __keymapScancode[Qt::Key_Q] = SDL_SCANCODE_Q;
    __keymapScancode[Qt::Key_R] = SDL_SCANCODE_R;
    __keymapScancode[Qt::Key_S] = SDL_SCANCODE_S;
    __keymapScancode[Qt::Key_T] = SDL_SCANCODE_T;
    __keymapScancode[Qt::Key_U] = SDL_SCANCODE_U;
    __keymapScancode[Qt::Key_V] = SDL_SCANCODE_V;
    __keymapScancode[Qt::Key_W] = SDL_SCANCODE_W;
    __keymapScancode[Qt::Key_X] = SDL_SCANCODE_X;
    __keymapScancode[Qt::Key_Y] = SDL_SCANCODE_Y;
    __keymapScancode[Qt::Key_Z] = SDL_SCANCODE_Z;


    __keymapScancode[Qt::Key_0] = SDL_SCANCODE_0;
    __keymapScancode[Qt::Key_1] = SDL_SCANCODE_1;
    __keymapScancode[Qt::Key_2] = SDL_SCANCODE_2;
    __keymapScancode[Qt::Key_3] = SDL_SCANCODE_3;
    __keymapScancode[Qt::Key_4] = SDL_SCANCODE_4;
    __keymapScancode[Qt::Key_5] = SDL_SCANCODE_5;
    __keymapScancode[Qt::Key_6] = SDL_SCANCODE_6;
    __keymapScancode[Qt::Key_7] = SDL_SCANCODE_7;
    __keymapScancode[Qt::Key_8] = SDL_SCANCODE_8;
    __keymapScancode[Qt::Key_9] = SDL_SCANCODE_9;
}

//------------------------------------------------------------------------------------------------------
// get SDL scancode key from Qt key
//------------------------------------------------------------------------------------------------------
SDL_Scancode __convertQtKeyToScancodeSDL(Qt::Key qtKey)
{
    SDL_Scancode scanecode = __keymapScancode.value(Qt::Key(qtKey));

    if(scanecode == 0)
        gplay::print("Warning: Scanecode %d not mapped", qtKey);

    return scanecode;
}


RenderViewWidget::RenderViewWidget(QWidget* parent) :
    QWidget(parent)
{
    //setAttribute(Qt::WA_NativeWindow);
    //setUpdatesEnabled(false);
    //setGeometry(0,0,200,200);

    setMouseTracking(true);
    ///setAttribute(Qt::WA_PaintOnScreen);
    ///setAttribute(Qt::WA_NativeWindow);
    ///setUpdatesEnabled(false);
    setFocusPolicy(Qt::StrongFocus);



    // init keymap for Qt > SDL keys conversion
    __initKeyMap();
    __initScancodeKeyMap();
}

void RenderViewWidget::resizeEvent(QResizeEvent* event)
{
    Q_EMIT(windowResized(event->size()));
}

#if 0

void RenderViewWidget::mousePressEvent(QMouseEvent* event)
{
#if 0
    QWidget::mousePressEvent(event);

    m_lastMousePos = event->pos();
    m_mouseDownPos = event->pos();

    if (event->buttons() & Qt::RightButton)
        setContextMenuPolicy(Qt::PreventContextMenu);



    /*SDL_Event sdlEvent;
    sdlEvent.type = SDL_MOUSEBUTTONDOWN;
    sdlEvent.button.state = SDL_PRESSED;
    if(event->buttons() & Qt::LeftButton)
        sdlEvent.button.button = SDL_BUTTON_LMASK;
    if(event->buttons() & Qt::RightButton)
        sdlEvent.button.button = SDL_BUTTON_RMASK;
    if(event->buttons() & Qt::MiddleButton)
        sdlEvent.button.button = SDL_BUTTON_LMASK;
    QPoint position = event->pos();
    sdlEvent.button.x = position.x();
    sdlEvent.button.y = position.y();
    SDL_PushEvent(&sdlEvent);*/

    const bool leftBtn = (event->buttons() & Qt::LeftButton);
    if(leftBtn)
    {
        QPoint glob = mapToGlobal(QPoint(width()/2,height()/2));
        QCursor::setPos(glob);
        SDL_SetRelativeMouseMode(SDL_TRUE);

        /*Graphics* graphics = GetSubsystem<Graphics>();
        SDL_Window * win = (SDL_Window*)graphics->GetWindow();
        SDL_WarpMouseInWindow(win,glob.x(),glob.y());*/


        m_lastMousePos = glob;
        m_mouseMoveEnabled = true;
    }

    // send event through gplay Event Listener ?
#endif
}

void RenderViewWidget::mouseReleaseEvent(QMouseEvent* event)
{
#if 0
    QWidget::mouseReleaseEvent(event);

    if (event->button() == Qt::RightButton)
    {
        QContextMenuEvent ce(QContextMenuEvent::Mouse, event->pos());
        setContextMenuPolicy(Qt::DefaultContextMenu);
        contextMenuEvent(&ce);
    }

    /*SDL_Event sdlEvent;
    sdlEvent.type = SDL_MOUSEBUTTONUP;
    sdlEvent.button.state = SDL_RELEASED;
    if(event->buttons() & Qt::LeftButton)
        sdlEvent.button.button = SDL_BUTTON_LMASK;
    if(event->buttons() & Qt::RightButton)
        sdlEvent.button.button = SDL_BUTTON_RMASK;
    if(event->buttons() & Qt::MiddleButton)
        sdlEvent.button.button = SDL_BUTTON_LMASK;
    QPoint position = event->pos();
    sdlEvent.button.x = position.x();
    sdlEvent.button.y = position.y();
    SDL_PushEvent(&sdlEvent);*/


    if(SDL_GetRelativeMouseMode())
    {
        SDL_SetRelativeMouseMode(SDL_FALSE);
        QPoint glob = mapToGlobal(QPoint(width()/2,height()/2));
        QCursor::setPos(glob);
    }

    m_mouseMoveEnabled = false;


    // send event through gplay Event Listener ?
#endif
}

void RenderViewWidget::mouseMoveEvent(QMouseEvent* event)
{
#if 0
    if(m_mouseMoveEnabled)
    {
        int dx = event->x() - m_lastMousePos.x();
        int dy = event->y() - m_lastMousePos.y();

        // send event through gplay Event Listener ?

        //m_lastMousePos = me->pos();

        QPoint glob = mapToGlobal(QPoint(width()/2,height()/2));
        QCursor::setPos(glob);
        m_lastMousePos = QPoint(width()/2,height()/2);

        QWidget::mouseMoveEvent(event);
    }


    /*Graphics* graphics = GetSubsystem<Graphics>();
    SDL_Window * win = (SDL_Window*)graphics->GetWindow();
    SDL_WarpMouseInWindow(win,glob.x(),glob.y());*/
#endif
}

void RenderViewWidget::wheelEvent(QWheelEvent* event)
{

}

void RenderViewWidget::keyPressEvent(QKeyEvent* event)
{
    QWidget::keyPressEvent(event);

    // add key to held keys list
    if (!m_keysDown.contains(event->key()) && !event->isAutoRepeat())
        m_keysDown.append(event->key());

    // Transmit key press event to SDL
    SDL_Event sdlEvent;
    sdlEvent.type = SDL_KEYDOWN;
    sdlEvent.key.keysym.sym = __convertQtKeyToSDL( Qt::Key(event->key()) );
    sdlEvent.key.keysym.mod = __convertQtKeyModifierToSDL(event->modifiers());
    sdlEvent.key.keysym.scancode = __convertQtKeyToScancodeSDL( Qt::Key(event->key()) );
    SDL_PushEvent(&sdlEvent);
}

void RenderViewWidget::keyReleaseEvent(QKeyEvent* event)
{
    QWidget::keyReleaseEvent(event);

    // remove key from held keys list
    const int index = m_keysDown.indexOf(event->key());
    if (index != -1 && !event->isAutoRepeat())
        m_keysDown.removeAt(index);

    // Transmit key release event to SDL
    SDL_Event sdlEvent;
    sdlEvent.type = SDL_KEYUP;
    sdlEvent.key.keysym.sym = __convertQtKeyToSDL( Qt::Key(event->key()) );
    sdlEvent.key.keysym.mod = __convertQtKeyModifierToSDL(event->modifiers());
    sdlEvent.key.keysym.scancode = __convertQtKeyToScancodeSDL( Qt::Key(event->key()) );
    SDL_PushEvent(&sdlEvent);
}


#endif



