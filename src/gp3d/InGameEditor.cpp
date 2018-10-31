#include "InGameEditor.h"

// used by the hierarchy window
inline void visitNode(Node* node);
inline void visitScene(Scene* scene);
inline void setNodeToHierarchyTree(Node* node);



InGameEditor::InGameEditor()
{

}

void InGameEditor::setScene(Scene* scene)
{
    _scene = scene;

    // create a node hierarchy for testing

    Node* n1 = Node::create("n1");
    Node* n2 = Node::create("n2");
    Node* n3 = Node::create("n3");
    Node* n4 = Node::create("n4");
    Node* n5 = Node::create("n5");
    Node* n6 = Node::create("n6");

    n2->addChild(n3);
    n2->addChild(n4);
    n4->addChild(n5);

    _scene->addNode(n1);
    _scene->addNode(n2);
    _scene->addNode(n6);
    _scene->addNode();
}

void InGameEditor::resize(int width, int height)
{

}

void InGameEditor::update(float elapsedTime)
{

}

void InGameEditor::render(float elapsedTime)
{
    //ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
    //ImGui::ShowTestWindow();

   /* // Create some ImGui controls to manage cube rotation
    static float axis[] = { 0.2f, 0.4f, 0.3f };
    static float speed = { 0.5f };
    ImGui::SetNextWindowSize(ImVec2(200,200), ImGuiCond_FirstUseEver);
    ImGui::Begin("Cube Controls");
    ImGui::SliderFloat3("Axis", axis, 0.0f, 1.0f);
    ImGui::SliderFloat("Speed", &speed, -10.0f, 10.0f);
    ImGui::End();*/


    // scene hierarchy window

    ImGui::SetNextWindowSize(ImVec2(200, 500), ImGuiCond_FirstUseEver);
    ImGui::Begin("Scene Hierarchy");
    if(_scene)
    {
        const char* sceneName = _scene->getId();
        if(strlen(sceneName) == 0)
            sceneName = "unnamed_scene";

        if(ImGui::TreeNode(sceneName))
        {
            visitScene(_scene);
            ImGui::TreePop();
        }
    }
    ImGui::End();

}



inline void setNodeToHierarchyTree(Node* node)
{
    ImGuiTreeNodeFlags flags = 0
            | ImGuiTreeNodeFlags_OpenOnArrow
            | ImGuiTreeNodeFlags_OpenOnDoubleClick
            | ImGuiTreeNodeFlags_DefaultOpen
            | ImGuiTreeNodeFlags_NoTreePushOnOpen
            ;

    bool hasChilds = node->getChildCount() > 0;
    if(!hasChilds)
        flags |= ImGuiTreeNodeFlags_Leaf;

    // set a default name when node has no name id
    const char* nodeName = node->getId();
    if(strlen(nodeName) == 0)
        nodeName = "unnamed_node";

    ImGui::TreePush();
    bool open = ImGui::TreeNodeEx(nodeName, flags);
    //bool open = ImGui::TreeNodeEx((void*)(intptr_t)&node, flags, nodeName);

    if (ImGui::IsItemClicked())
    {
        print("Item %s is clicked [0x%x]\n", nodeName, node);
    }

    if(hasChilds && open)
    {
        visitNode(node);
    }
    ImGui::TreePop();
}

inline void visitNode(Node* node)
{
    // Recurse for all children.
    for (Node* child = node->getFirstChild(); child != NULL; child = child->getNextSibling())
    {
        setNodeToHierarchyTree(child);
    }
}

inline void visitScene(Scene* scene)
{
    // Recurse for all sibling.
    for (Node* node = scene->getFirstNode(); node != NULL; node = node->getNextSibling())
    {
        setNodeToHierarchyTree(node);
    }
}


