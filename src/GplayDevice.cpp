#include "GplayDevice.h"
#include "node-editor/common/BaseNode.h"
#include "gp3d/QtImGui.h"
#include "gp3d/helpers/Events.h"
#include "node-editor/spark-nodes/SpkRenderer.h"
#include "gp3d/GPRenderer.h"

ISubRenderer* _curentSubRenderer;


GplayDeviceGame::GplayDeviceGame() :
    _platform(nullptr)
{

}

GplayDeviceGame::~GplayDeviceGame()
{

}

void GplayDeviceGame::createRenderWindow(void* hwnd)
{
    _platform = gplay::Platform::create(this, hwnd);
    GP_ASSERT(_platform);
    _platform->start();

    // create default view
    View::create(0, Rectangle(200, 200), View::ClearFlags::COLOR_DEPTH, 0x556677ff, 1.0f, 0);


    _curentSubRenderer = new SpkRenderer();
}

void GplayDeviceGame::runFrame()
{
    // begin frame
    Renderer::getInstance().beginFrame();
    QtImGui::newFrame();

    // call gplay frame that will invoke update and render methods.
    frame();

    // end frame
    ImGui::Render();
    Renderer::getInstance().endFrame();
}

void GplayDeviceGame::stop()
{
    _platform->stop();
}

void GplayDeviceGame::keyEvent(Keyboard::KeyEvent evt, int key)
{
    // send key event
    std::shared_ptr<KeyEvent> keyEvent = KeyEvent::create();
    keyEvent.get()->event = evt;
    keyEvent.get()->key = key;
    EventManager::getInstance()->queueEvent(keyEvent);
}

bool GplayDeviceGame::mouseEvent(Mouse::MouseEvent evt, int x, int y, int wheelDelta)
{
    // when right button is pressed set on mouse captured to interact with fps camera
    if(evt == Mouse::MOUSE_PRESS_RIGHT_BUTTON)
        setMouseCaptured(true);
    else if(evt == Mouse::MOUSE_RELEASE_RIGHT_BUTTON)
        setMouseCaptured(false);

    // send mouse event
    std::shared_ptr<MouseEvent> mouseEvent = MouseEvent::create();
    mouseEvent.get()->event = evt;
    mouseEvent.get()->mousePos = Vector2(x, y);
    EventManager::getInstance()->queueEvent(mouseEvent);

    return true;
}

void GplayDeviceGame::resizeRenderView(int width, int height)
{
    // resize game engine window
    _platform->setWindowSize(width, height);

    // resize default view
    View::getView(0)->setViewRect(Rectangle(width, height));

    // resize current renderer
    _curentSubRenderer->resize(width, height);
}

void GplayDeviceGame::initialize()
{

}

void GplayDeviceGame::finalize()
{

}

void GplayDeviceGame::update(float elapsedTime)
{
    _curentSubRenderer->update(elapsedTime);
}

void GplayDeviceGame::render(float elapsedTime)
{
    bgfx::touch(0);

    _curentSubRenderer->render(elapsedTime);
}


void GplayDeviceGame::setCurentParticleSystem(SPK::Ref<SPK::System> sparkSystem)
{
    SpkRenderer* _spkRenderer = dynamic_cast<SpkRenderer*>(_curentSubRenderer);
    if(_spkRenderer)
        _spkRenderer->setCurentParticleSystem(sparkSystem);
}
