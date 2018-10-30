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

        ImGui::TreeNode(sceneName);

        int id = 0;
        for (Node* node = _scene->getFirstNode(); node != nullptr; node = node->getNextSibling(), id++)
        {
            ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow
                    | ImGuiTreeNodeFlags_OpenOnDoubleClick
                    | ImGuiTreeNodeFlags_Leaf
                    | ImGuiTreeNodeFlags_NoTreePushOnOpen;

            ImGui::TreeNodeEx((void*)(intptr_t)id, node_flags, node->getId(), id);

            for (Node* child = node->getFirstChild(); child != nullptr; child = child->getNextSibling(), id++)
            {
                ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
                        ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
                ImGui::TreeNodeEx((void*)(intptr_t)id, node_flags, child->getId(), id);
            }
        }
    }
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
    ImGui::ShowTestWindow();
}

