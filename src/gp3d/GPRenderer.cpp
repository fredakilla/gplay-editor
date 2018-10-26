#include "GPRenderer.h"
#include "helpers/Events.h"

GPRenderer3D::GPRenderer3D()
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

    loadGrid(_scene);

    // Add listeners for events
    EventManager::get()->addListener(GP_EVENT_LISTENER(this, GPRenderer3D::onMouseEvent), MouseEvent::ID());
    EventManager::get()->addListener(GP_EVENT_LISTENER(this, GPRenderer3D::onKeyEvent), KeyEvent::ID());

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


void GPRenderer3D::update(float timestep)
{
    // get real game frame time, because timestep is relative to editor timeline
    float frameTime = Game::getInstance()->getFrameTime();
    _fpCamera.updateCamera(frameTime);
}

void GPRenderer3D::onMouseEvent(EventDataRef eventData)
{
    auto mouseEvent = std::dynamic_pointer_cast<MouseEvent>(eventData);
    if(!mouseEvent)
        return;
    _fpCamera.touchEvent(Touch::TOUCH_MOVE, mouseEvent->mousePos.x, mouseEvent->mousePos.y, 0);
}

void GPRenderer3D::onKeyEvent(EventDataRef eventData)
{
    auto keyEvent = std::dynamic_pointer_cast<KeyEvent>(eventData);
    if(!keyEvent)
        return;
    _fpCamera.keyEvent(keyEvent->event, keyEvent->key);
}
