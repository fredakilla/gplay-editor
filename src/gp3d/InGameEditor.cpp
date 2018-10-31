#include "InGameEditor.h"

InGameEditor::InGameEditor()
{

}

void InGameEditor::resize(int width, int height)
{

}

void InGameEditor::update(float elapsedTime)
{

}

void InGameEditor::setScene(Scene* scene)
{
    _scene = scene;
}

void InGameEditor::render(float elapsedTime)
{
    // Create some ImGui controls to manage cube rotation
    static float axis[] = { 0.2f, 0.4f, 0.3f };
    static float speed = { 0.5f };
    ImGui::SetNextWindowSize(ImVec2(200,200), ImGuiCond_FirstUseEver);
    ImGui::Begin("Cube Controls");
    ImGui::SliderFloat3("Axis", axis, 0.0f, 1.0f);
    ImGui::SliderFloat("Speed", &speed, -10.0f, 10.0f);
    ImGui::End();





    ImGui::Begin("Scene Hierarchy");
    if(_scene)
    {
        const char* sceneName = _scene->getId();
        if(strlen(sceneName) == 0)
            sceneName = "unnamed_scene";

        if(ImGui::TreeNode(sceneName))
        {
            _scene->visit(this, &InGameEditor::fillTreeScene);
            ImGui::TreePop();
        }
    }
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
    ImGui::ShowTestWindow();
}

bool InGameEditor::fillTreeScene(Node* node)
{
    bool hasChilds = node->getChildCount() > 0;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
    if(!hasChilds)
        flags |= ImGuiTreeNodeFlags_Leaf;

    if(ImGui::TreeNodeEx(node->getId(), flags))
    {
        ImGui::TreePop();
    }

    return true;
}
