#include "InGameEditor.h"
#include "uuid.h"

// used by the hierarchy window
inline void visitNode(Node* node);
inline void visitScene(Scene* scene);
inline void setNodeToHierarchyTree(Node* node);
Node* _treeHierarchySelectedNode = nullptr;
int _treeHierarchySelectionMask = 0;
int _treeHierarchyItemCount = 0;

enum TreeNodeAction
{
    TreeNodeAction_None,
    TreeNodeAction_Create,
    TreeNodeAction_Delete
};
TreeNodeAction _treeNodeAction = TreeNodeAction_None;


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
    ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
    ImGui::ShowTestWindow();

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
        // reset selection mask and item count
        _treeHierarchySelectionMask = 0;
        _treeHierarchyItemCount = 0;
        _treeNodeAction = TreeNodeAction_None;
        bool isCreateNodeClicked = false;
        bool isDeleteNodeClicked  = false;


        // set default scene name if empty
        const char* sceneName = _scene->getId();
        if(strlen(sceneName) == 0)
            sceneName = "unnamed_scene";

        ImGui::Text("Selected node :"); ImGui::SameLine();
        isCreateNodeClicked = ImGui::Button("Create Node"); ImGui::SameLine();
        isDeleteNodeClicked = ImGui::Button("Delete Node");


        ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Always);
        bool isRootOpen = ImGui::TreeNodeEx(sceneName);
        bool isRootClicked = ImGui::IsItemClicked();

        ImGui::PushID("scene_root");
        if (ImGui::BeginPopupContextItem("item context menu"))
        {
            if (ImGui::Selectable("Create node")) _treeNodeAction = TreeNodeAction_Create;
            ImGui::EndPopup();

            print("Scene is clicked\n");
            _treeHierarchySelectedNode = nullptr;

            // clear selection
            _treeHierarchySelectionMask = 0;

        }
        ImGui::PopID();



        if(isRootOpen)
        {
            visitScene(_scene);
            ImGui::TreePop();
        }

        /*if(isRootClicked)
        {
            print("Scene is clicked\n");
            _treeHierarchySelectedNode = nullptr;

            // clear selection
            _treeHierarchySelectionMask = 0;
        }*/





        switch(_treeNodeAction)
        {
        case TreeNodeAction_Create:
            isCreateNodeClicked = true;
            break;

        case TreeNodeAction_Delete:
            isDeleteNodeClicked = true;
            break;

        case TreeNodeAction_None:
        default:
            break;
        }


        // create node ?
        if(isCreateNodeClicked)
        {
            std::string s = "Node_";
            s.append(UUID::generateUUID());
            Node* node = Node::create(s.c_str());

            if(_treeHierarchySelectedNode)
            {
                // add the new node as child of the selected node
                _treeHierarchySelectedNode->addChild(node);
            }
            else
            {
                // add the new node to scene root
                _scene->addNode(node);
            }
        }

        // delete node ?
        if(isDeleteNodeClicked)
        {
            if(_treeHierarchySelectedNode)
            {
                // remove current selected node

                _treeHierarchySelectedNode->removeAllChildren();
                _scene->removeNode(_treeHierarchySelectedNode);
            }

            // clear selection
            _treeHierarchySelectionMask = 0;
            _treeHierarchySelectedNode = nullptr;
        }


    }
    ImGui::End();

}



inline void setNodeToHierarchyTree(Node* node)
{
    _treeHierarchyItemCount++;

    ImGuiTreeNodeFlags flags = 0
            | ImGuiTreeNodeFlags_OpenOnArrow
            | ImGuiTreeNodeFlags_OpenOnDoubleClick
            | ImGuiTreeNodeFlags_DefaultOpen
            | ImGuiTreeNodeFlags_NoTreePushOnOpen
            | ((_treeHierarchySelectionMask & (1 << _treeHierarchyItemCount)) ? ImGuiTreeNodeFlags_Selected : 0)
            ;

    if(node == _treeHierarchySelectedNode)
        flags |= ImGuiTreeNodeFlags_Selected;

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
        _treeHierarchySelectedNode = node;

        // update selection
        _treeHierarchySelectionMask = (1 << _treeHierarchyItemCount);
    }



    // make unique id for context popup based on node name
    std::string uniqueId = nodeName + std::to_string(_treeHierarchyItemCount);

    ImGui::PushID(uniqueId.c_str());
    if (ImGui::BeginPopupContextItem("item context menu"))
    {
        if (ImGui::Selectable("Create child node")) _treeNodeAction = TreeNodeAction_Create;
        if (ImGui::Selectable("Delete node")) _treeNodeAction = TreeNodeAction_Delete;
        ImGui::EndPopup();

        print("Item %s is clicked [0x%x]\n", nodeName, node);
        _treeHierarchySelectedNode = node;

        // update selection
        _treeHierarchySelectionMask = (1 << _treeHierarchyItemCount);

    }
    ImGui::PopID();


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


