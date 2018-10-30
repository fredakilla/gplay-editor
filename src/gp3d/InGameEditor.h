#ifndef INGAMEEDITOR_H
#define INGAMEEDITOR_H

#include "GPRenderer.h"
#include <thirdparty/imgui/imgui.h>

class InGameEditor : public ISubRenderer
{
public:
    InGameEditor();

    void resize(int width, int height) override;
    void update(float elapsedTime) override;
    void render(float elapsedTime) override;
};

#endif // INGAMEEDITOR_H
