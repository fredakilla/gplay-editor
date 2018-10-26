#ifndef EVENTS_H
#define EVENTS_H

#include <gplay-engine.h>

using namespace gplay;


GP_EVENT_BEGIN(MouseEvent)
    Mouse::MouseEvent event;    // mouse event type
    Vector2 mousePos;           // mouse position
GP_EVENT_END()

GP_EVENT_BEGIN(KeyEvent)
    Keyboard::KeyEvent event;   // key event type
    int key;                    // key
GP_EVENT_END()

#endif // EVENTS_H
