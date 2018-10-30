#include "GPRenderer.h"
#include "helpers/Events.h"

GPRenderer3D::GPRenderer3D() :
    _scene(nullptr)
{
    // Create a new empty scene.
    _scene = Scene::create();

    // set fps camera
    Vector3 cameraPosition(0, 1, 5);
    _fpCamera.initialize(1.0, 10000.0f);
    _fpCamera.setPosition(cameraPosition);
    _scene->addNode(_fpCamera.getRootNode());
    _scene->setActiveCamera(_fpCamera.getCamera());
    _scene->getActiveCamera()->setAspectRatio(4/3);

    // load 3d grid
    loadGrid(_scene);

    // Add listeners for events
    EventManager::getInstance()->addListener(GP_EVENT_LISTENER(this, GPRenderer3D::onMouseEvent), MouseEvent::ID());
    EventManager::getInstance()->addListener(GP_EVENT_LISTENER(this, GPRenderer3D::onKeyEvent), KeyEvent::ID());
}

void GPRenderer3D::loadGrid(Scene* scene)
{
    assert(scene);
    Model* gridModel = createGridModel();
    assert(gridModel);
    gridModel->setMaterial("res/data/materials/grid.material");
    Node* node = scene->addNode("grid");
    node->setDrawable(gridModel);
    SAFE_RELEASE(gridModel);
}

void GPRenderer3D::resize(int width, int height)
{
    if(_scene)
    {
        Camera* camera = _scene->getActiveCamera();
        if(camera)
        {
            float ratio = (float)width / (float)height;
            camera->setAspectRatio(ratio);
        }
    }
}

void GPRenderer3D::update(float elapsedTime)
{
    if(Game::getInstance()->isMouseCaptured())
        _fpCamera.updateCamera(elapsedTime);
}

void GPRenderer3D::onMouseEvent(EventDataRef eventData)
{
    auto mouseEvent = std::dynamic_pointer_cast<MouseEvent>(eventData);
    if(!mouseEvent)
        return;

    if(Game::getInstance()->isMouseCaptured())
        _fpCamera.mouseEvent(mouseEvent->event, mouseEvent->mousePos.x, mouseEvent->mousePos.y, 0);
    else
    {
        // force to clear camera movement if key press event was not sent before releasing mouse capture
        _fpCamera.clearMoveFlag();
    }
}

void GPRenderer3D::onKeyEvent(EventDataRef eventData)
{
    auto keyEvent = std::dynamic_pointer_cast<KeyEvent>(eventData);
    if(!keyEvent)
        return;

    if(Game::getInstance()->isMouseCaptured())
        _fpCamera.keyEvent(keyEvent->event, keyEvent->key);
}
