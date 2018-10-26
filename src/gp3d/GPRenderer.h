#ifndef GPRENDERER_H
#define GPRENDERER_H

#include <gplay-engine.h>
#include "helpers/FirstPersonCamera.h"
#include "helpers/Grid.h"

class IRenderer
{

public:
    virtual ~IRenderer() {}
    virtual void resize(int width, int height) {}
    virtual void update(float timestep) {}
    virtual void render() = 0;
};



class GPRenderer3D : public IRenderer
{

public:
    GPRenderer3D();
    void update(float timestep);
    void onMouseEvent(EventDataRef eventData);
    void onKeyEvent(EventDataRef eventData);

protected:
    Scene* _scene;
    FirstPersonCamera _fpCamera;

private:
    void loadGrid(Scene* scene);
};





#include <thirdparty/imgui/imgui.h>

class OpClassNode_Renderer : public GPRenderer3D
{
public:
    OpClassNode_Renderer()
        : GPRenderer3D()
    {

    }

    void update(float timestep)
    {
        GPRenderer3D::update(timestep);

    }

    void render()
    {
        // Create some ImGui controls to manage cube rotation
        /*static float axis[] = { 0.2f, 0.4f, 0.3f };
        static float speed = { 0.5f };
        ImGui::SetNextWindowSize(ImVec2(200,200), ImGuiCond_FirstUseEver);
        ImGui::Begin("Cube Controls");
        ImGui::SliderFloat3("Axis", axis, 0.0f, 1.0f);
        ImGui::SliderFloat("Speed", &speed, -10.0f, 10.0f);
        ImGui::End();*/

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
