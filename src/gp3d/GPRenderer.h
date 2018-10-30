#ifndef GPRENDERER_H
#define GPRENDERER_H

#include <gplay-engine.h>
#include "helpers/FirstPersonCamera.h"
#include "helpers/Grid.h"

/**
 * Interface class for sub-renderers
 * A subrenderer manage it's own scene.
 */
class ISubRenderer
{
public:
    virtual ~ISubRenderer() {}
    virtual void resize(int width, int height) = 0;
    virtual void update(float elapsedTime) = 0;
    virtual void render(float elapsedTime) = 0;
};


/**
 * Base subrenderer for 3D view, with scene, fps camera and 3d grid.
 */
class GPRenderer3D : public ISubRenderer
{

public:
    GPRenderer3D();

    void update(float elapsedTime) override;
    void resize(int width, int height) override;

    void onMouseEvent(EventDataRef eventData);
    void onKeyEvent(EventDataRef eventData);

protected:
    Scene* _scene;
    FirstPersonCamera _fpCamera;

private:
    void loadGrid(Scene* scene);
};





#endif // GPRENDERER_H
