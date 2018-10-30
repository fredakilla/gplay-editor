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




// Test renderer

#include <thirdparty/imgui/imgui.h>

class OpClassNode_Renderer : public GPRenderer3D
{
public:
    OpClassNode_Renderer()
        : GPRenderer3D()
    {

    }

    void update(float elapsedTime)
    {
        // call super class method
        GPRenderer3D::update(elapsedTime);
    }

    void render(float elapsedTime)
    {
        // Create some ImGui controls to manage cube rotation
        static float axis[] = { 0.2f, 0.4f, 0.3f };
        static float speed = { 0.5f };
        ImGui::SetNextWindowSize(ImVec2(200,200), ImGuiCond_FirstUseEver);
        ImGui::Begin("Cube Controls");
        ImGui::SliderFloat3("Axis", axis, 0.0f, 1.0f);
        ImGui::SliderFloat("Speed", &speed, -10.0f, 10.0f);
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
        ImGui::ShowTestWindow();

        _scene->visit(this, &OpClassNode_Renderer::drawScene);
    }

private:
    bool drawScene(gplay::Node* node)
    {
        Drawable* drawable = node->getDrawable();
        if (drawable)
            drawable->draw();
        return true;
    }
};



#endif // GPRENDERER_H
