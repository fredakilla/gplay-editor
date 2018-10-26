#include "../core/Base.h"
#include "../core/Platform.h"
#include "../core/Game.h"

#include <bgfx/platform.h>

#include "../renderer/Renderer.h"
#include "../renderer/BGFXImGui.h"

#include <QX11Info>
#include <QDebug>
#include <QThread>
#include <QWidget>

#include "QtImGui.h"


namespace gplay {

int __app_argc = 0;
char** __app_argv = nullptr;

static QWidget* __widget;

// timer
static double __timeAbsolute;
static std::chrono::time_point<std::chrono::high_resolution_clock> __timeStart;

// mouse input
static bool __mouseCaptured = false;
static float __mouseCapturePointX = 0;
static float __mouseCapturePointY = 0;
static bool __cursorVisible = true;


void updateWindowSize()
{
    Q_ASSERT(__widget);

    int width = __widget->width();
    int height = __widget->height();

    Renderer::getInstance().resize(width, height);
}


//-------------------------------------------------------------------------------------------------------------
// Platform Qt impl
//-------------------------------------------------------------------------------------------------------------

Platform::Platform(Game* game) :
    _game(game)
{
    Q_ASSERT(__app_argc);
    Q_ASSERT(__app_argv);
}

Platform::~Platform()
{
}

Platform* Platform::create(Game* game, void* externalWindow)
{
    Q_ASSERT(game);
    Q_ASSERT(externalWindow);

    FileSystem::setResourcePath("./");
    Platform* platform = new Platform(game);

    QWidget* renderWidget = (QWidget*)externalWindow;
    __widget = renderWidget;

    bgfx::PlatformData pd;
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    pd.ndt          = QX11Info::display();
    pd.nwh          = (void*)renderWidget->winId();
#elif BX_PLATFORM_WINDOWS
#endif // BX_PLATFORM_
    pd.context      = NULL;
    pd.backBuffer   = NULL;
    pd.backBufferDS = NULL;
    bgfx::setPlatformData(pd);


    bgfx::Init init;
    init.type = bgfx::RendererType::OpenGL;
    bgfx::init(init);

    uint32_t debug = BGFX_DEBUG_TEXT;
    uint32_t reset = BGFX_RESET_VSYNC;

    bgfx::reset(renderWidget->width(), renderWidget->height(), reset);
    bgfx::setDebug(debug);

    BGFXRenderer::initInstance();
    Renderer::getInstance().queryCaps();

    game->setVsync(true);
    game->setViewport(Rectangle(0,0,renderWidget->width(), renderWidget->height()));

    updateWindowSize();

    // Create ImGui context and init
    QtImGui::initialize(renderWidget);


    return platform;
}

void Platform::start()
{
    GP_ASSERT(_game);

    // Get the initial time.
    __timeStart = std::chrono::high_resolution_clock::now();
    __timeAbsolute = 0L;

    // Run the game.
    _game->run();
}

void Platform::stop()
{
    // shutdow bgfx
    bgfx::shutdown();
}

void Platform::frame()
{
    if (_game)
    {
        Renderer::getInstance().beginFrame();        
        QtImGui::newFrame();
        _game->frame();
        ImGui::Render();
        Renderer::getInstance().endFrame();
    }
}

int Platform::processEvents()
{/*
    SDL_Event evt;

    while (SDL_PollEvent(&evt))
    {
        // Process ImGui events
        ImGui_ImplSdlGL3_ProcessEvent(&evt);

        // Process SDL2 events
        switch (evt.type)
        {
            case SDL_QUIT:
            {
                _game->exit();
                return 0;
            }
            break;

            case SDL_MOUSEWHEEL:
            {
                if(ImGui::GetIO().WantCaptureMouse)
                    continue;

                const SDL_MouseWheelEvent& wheelEvent = evt.wheel;
                int wheelDelta = wheelEvent.y;
                gplay::Platform::mouseEventInternal(gplay::Mouse::MOUSE_WHEEL, 0, 0, wheelDelta);
            }
            break;

            case SDL_MOUSEBUTTONDOWN:
            {
                if(ImGui::GetIO().WantCaptureMouse)
                    continue;

                gplay::Mouse::MouseEvent mouseEvt;
                const SDL_MouseButtonEvent& sdlMouseEvent = evt.button;

                switch (sdlMouseEvent.button)
                {
                case SDL_BUTTON_LEFT:
                    mouseEvt = gplay::Mouse::MOUSE_PRESS_LEFT_BUTTON;
                    break;
                case SDL_BUTTON_RIGHT:
                    mouseEvt = gplay::Mouse::MOUSE_PRESS_RIGHT_BUTTON;
                    break;
                case SDL_BUTTON_MIDDLE:
                    mouseEvt = gplay::Mouse::MOUSE_PRESS_MIDDLE_BUTTON;
                    break;
                }

                if (!gplay::Platform::mouseEventInternal(mouseEvt, sdlMouseEvent.x, sdlMouseEvent.y, 0))
                {
                    gplay::Platform::touchEventInternal(gplay::Touch::TOUCH_PRESS, sdlMouseEvent.x, sdlMouseEvent.y, 0, true);
                }
            }
            break;

            case SDL_MOUSEBUTTONUP:
            {
                if(ImGui::GetIO().WantCaptureMouse)
                    continue;

                gplay::Mouse::MouseEvent mouseEvt;
                const SDL_MouseButtonEvent& sdlMouseEvent = evt.button;

                switch (sdlMouseEvent.button)
                {
                case SDL_BUTTON_LEFT:
                    mouseEvt = gplay::Mouse::MOUSE_RELEASE_LEFT_BUTTON;
                    break;
                case SDL_BUTTON_RIGHT:
                    mouseEvt = gplay::Mouse::MOUSE_RELEASE_RIGHT_BUTTON;
                    break;
                case SDL_BUTTON_MIDDLE:
                    mouseEvt = gplay::Mouse::MOUSE_RELEASE_MIDDLE_BUTTON;
                    break;
                }

                if (!gplay::Platform::mouseEventInternal(mouseEvt, sdlMouseEvent.x, sdlMouseEvent.y, 0))
                {
                    gplay::Platform::touchEventInternal(gplay::Touch::TOUCH_RELEASE, sdlMouseEvent.x, sdlMouseEvent.y, 0, true);
                }
            }
            break;

            case SDL_MOUSEMOTION:
            {
                if(ImGui::GetIO().WantCaptureMouse)
                    continue;

                const SDL_MouseMotionEvent& motionEvt = evt.motion;

                int x = motionEvt.x;
                int y = motionEvt.y;
                if (__mouseCaptured)
                {
                    if (x == __mouseCapturePointX && y == __mouseCapturePointY)
                    {
                        // Discard the first MotionNotify following capture since it contains bogus x,y data.
                        break;
                    }

                    // Convert to deltas
                    x -= __mouseCapturePointX;
                    y -= __mouseCapturePointY;

                    // Warp mouse back to center of screen.
                    SDL_WarpMouseInWindow(__window, __mouseCapturePointX, __mouseCapturePointY);
                }

                if (!gplay::Platform::mouseEventInternal(gplay::Mouse::MOUSE_MOVE, x, y, 0))
                {
                    //if (evt.xmotion.state & Button1Mask)
                    if (evt.button.button == SDL_BUTTON_LEFT)
                    {
                        gplay::Platform::touchEventInternal(gplay::Touch::TOUCH_MOVE, x, y, 0, true);
                    }
                }
            }
            break;


            case SDL_KEYDOWN:
            {
                if(!ImGui::GetIO().WantTextInput)
                {
                    const SDL_KeyboardEvent& keyEvent = evt.key;
                    Keyboard::Key key = translateKey(keyEvent.keysym.scancode);
                    gplay::Platform::keyEventInternal(gplay::Keyboard::KEY_PRESS, key);

                    switch (key)
                    {
                    case Keyboard::KEY_F1:
                        Renderer::getInstance().toggleDebugStats();
                        break;
                    case Keyboard::KEY_F3:
                        Renderer::getInstance().toggleWireFrame();
                        break;
                    case Keyboard::KEY_F7:
                        Renderer::getInstance().toggleVSync();
                        break;
                    }
                }
            }
            break;

            case SDL_KEYUP:
            {
                if(!ImGui::GetIO().WantTextInput)
                {
                    const SDL_KeyboardEvent& keyEvent = evt.key;
                    Keyboard::Key key = translateKey(keyEvent.keysym.scancode);
                    gplay::Platform::keyEventInternal(gplay::Keyboard::KEY_RELEASE, key);
                }
            }
            break;
        }
    }*/

    return 1;
}

void Platform::swapBuffers()
{
    Renderer::getInstance().endFrame();
}

void Platform::signalShutdown()
{
    GP_ERROR("Fix me !");
}

bool Platform::canExit()
{
    return true;
}

unsigned int Platform::getDisplayWidth()
{
    return __widget->width();
}

unsigned int Platform::getDisplayHeight()
{
    return __widget->height();
}

double Platform::getAbsoluteTime()
{
    auto now = std::chrono::high_resolution_clock::now();
    typedef std::chrono::duration<double, std::milli> duration;
    duration elapsed = now - __timeStart;
    __timeAbsolute = elapsed.count();
    return __timeAbsolute;
}

void Platform::setAbsoluteTime(double time)
{
    __timeAbsolute = time;
}

bool Platform::isVsync()
{
    return Renderer::getInstance().isVSync();
}

void Platform::setVsync(bool enable)
{
     Renderer::getInstance().setVSync(enable);
}

void Platform::sleep(long ms)
{
    QThread::sleep(ms);
}

void Platform::setMultiSampling(bool enabled)
{
    GP_ERROR("Fix me !");
}

bool Platform::isMultiSampling()
{
    GP_ERROR("Fix me !");
    return false;
}

void Platform::setMultiTouch(bool enabled)
{
}

bool Platform::isMultiTouch()
{
    return false;
}

bool Platform::hasMouse()
{
    return true;
}

void Platform::setMouseCaptured(bool captured)
{/*
    if (captured != __mouseCaptured)
    {
        if (captured)
        {
            // Hide the cursor and warp it to the center of the screen
            __mouseCapturePointX = getDisplayWidth() / 2;
            __mouseCapturePointY = getDisplayHeight() / 2;

            setCursorVisible(false);
            SDL_CaptureMouse(SDL_TRUE);
        }
        else
        {
            // Restore cursor
            setCursorVisible(true);
            SDL_CaptureMouse(SDL_FALSE);
        }

        __mouseCaptured = captured;
    }*/
}

bool Platform::isMouseCaptured()
{
    return __mouseCaptured;
}

void Platform::setCursorVisible(bool visible)
{/*
    if (visible != __cursorVisible)
    {
        if (visible==false)
        {
            SDL_ShowCursor(false);
        }
        else
        {
            SDL_ShowCursor(true);
        }

        __cursorVisible = visible;
    }*/
}

bool Platform::isCursorVisible()
{
    return __cursorVisible;
}

bool Platform::hasAccelerometer()
{
    return false;
}

void Platform::getAccelerometerValues(float* pitch, float* roll)
{
    *pitch = 0;
    *roll = 0;
}

void Platform::getSensorValues(float* accelX, float* accelY, float* accelZ, float* gyroX, float* gyroY, float* gyroZ)
{
    GP_ERROR("Fix me !");
}

void Platform::setArguments(int* argc, char*** argv)
{
    __app_argc = *argc;
    __app_argv = *argv;
}

void Platform::getArguments(int* argc, char*** argv)
{
    if (argc)
        *argc = __app_argc;
    if (argv)
        *argv = __app_argv;
}

void Platform::displayKeyboard(bool display)
{

}

bool Platform::isGestureSupported(Gesture::GestureEvent evt)
{
    GP_ERROR("Fix me !");
}

void Platform::registerGesture(Gesture::GestureEvent evt)
{
    GP_ERROR("Fix me !");
}

void Platform::unregisterGesture(Gesture::GestureEvent evt)
{
    GP_ERROR("Fix me !");
}

bool Platform::isGestureRegistered(Gesture::GestureEvent evt)
{
    GP_ERROR("Fix me !");
    return false;
}

bool Platform::launchURL(const char* url)
{
    GP_ERROR("Fix me !");
    return false;
}

std::string Platform::displayFileDialog(size_t mode, const char* title, const char* filterDescription, const char* filterExtensions, const char* initialDirectory)
{
    GP_ERROR("Fix me !");
    return "";
}

void Platform::pollGamepadState(Gamepad* gamepad)
{
    GP_ERROR("Fix me !");
}

void Platform::setWindowSize(int width, int height)
{
    updateWindowSize();
    resizeEventInternal(width, height);
}

void* Platform::getWindowHandle()
{
    return __widget;
}



} //end namespace gplay
