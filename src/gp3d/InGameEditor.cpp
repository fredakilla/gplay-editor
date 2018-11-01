#include "InGameEditor.h"
#include "uuid.h"

namespace inGameEditor {

class SceneHierarchy
{
    enum TreeNodeAction
    {
        TreeNodeAction_None,
        TreeNodeAction_Create,
        TreeNodeAction_Delete
    };

    Node* _treeViewSelectedNode;        // current select node in tree
    int _treeViewSelectionMask;         // used to display selection overlay in tree
    int _treeViewItemCount;             // internally used for selection
    TreeNodeAction _treeNodeAction;     // result of context menu on node

public:

    SceneHierarchy()
    {
        _treeViewSelectedNode = nullptr;
        _treeViewSelectionMask = 0;
        _treeViewItemCount = 0;
        _treeNodeAction = TreeNodeAction_None;
    }

    void run(Scene* scene)
    {
        if(!scene)
        {
            print("inGameEditor::SceneHierarchy - scene is null.");
            return;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 10.0f);
        ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_FirstUseEver);

        ImGui::Begin("Scene Hierarchy");

        // reset selection mask and item count
        _treeViewSelectionMask = 0;
        _treeViewItemCount = 0;
        _treeNodeAction = TreeNodeAction_None;
        bool isCreateNodeClicked = false;
        bool isDeleteNodeClicked  = false;

        // set default scene name if empty
        const char* sceneName = scene->getId();
        if(strlen(sceneName) == 0)
            sceneName = "unnamed_scene";

        // buttons
        ImGui::Text("Selected node :"); ImGui::SameLine();
        isCreateNodeClicked = ImGui::Button("Create Node"); ImGui::SameLine();
        isDeleteNodeClicked = ImGui::Button("Delete Node");

        // scene tree node header
        ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Always);
        bool isRootOpen = ImGui::TreeNodeEx(sceneName);
        bool isRootClicked = ImGui::IsItemClicked();

        // context menu on tree node header
        ImGui::PushID(sceneName);
        if (ImGui::BeginPopupContextItem("item context menu"))
        {
            print("Scene is clicked\n");

            if (ImGui::Selectable("Create node")) _treeNodeAction = TreeNodeAction_Create;
            ImGui::EndPopup();

            _treeViewClearSelection();

        }
        ImGui::PopID();

        // visit scene child nodes
        if(isRootOpen)
        {
            _visitScene(scene);
            ImGui::TreePop();
        }

        if(isRootClicked)
        {
            print("Scene is clicked\n");
            _treeViewClearSelection();
        }


        // tree node action result is set by node context menu or buttons at top of tree view
        if(isCreateNodeClicked)
            _treeNodeAction = TreeNodeAction_Create;
        if(isDeleteNodeClicked)
            _treeNodeAction = TreeNodeAction_Delete;

        // process tree node actions
        switch(_treeNodeAction)
        {
        case TreeNodeAction_Create:
            _createChildNode(_treeViewSelectedNode, scene);
            break;

        case TreeNodeAction_Delete:
            _removeNode(_treeViewSelectedNode, scene);
            break;

        case TreeNodeAction_None:
        default:
            break;
        }


        ImGui::End();
        ImGui::PopStyleVar();
    }


private:

    //! add a scene node to tree view and recurse his childs
    void _addNodeToTreeView(Node* node)
    {
        _treeViewItemCount++;

        ImGuiTreeNodeFlags flags = 0
                | ImGuiTreeNodeFlags_OpenOnArrow
                | ImGuiTreeNodeFlags_OpenOnDoubleClick
                | ImGuiTreeNodeFlags_DefaultOpen
                | ImGuiTreeNodeFlags_NoTreePushOnOpen
                | ((_treeViewSelectionMask & (1 << _treeViewItemCount)) ? ImGuiTreeNodeFlags_Selected : 0)
                ;

        if(node == _treeViewSelectedNode)
            flags |= ImGuiTreeNodeFlags_Selected;

        bool hasChilds = node->getChildCount() > 0;
        if(!hasChilds)
            flags |= ImGuiTreeNodeFlags_Leaf;

        // set a default name when node name is empty
        const char* nodeName = node->getId();
        if(strlen(nodeName) == 0)
            nodeName = "unnamed_node";


        ImGui::TreePush();
        bool open = ImGui::TreeNodeEx(nodeName, flags);
        //bool open = ImGui::TreeNodeEx((void*)(intptr_t)&node, flags, nodeName);

        if (ImGui::IsItemClicked())
        {
            print("Item %s is clicked [0x%x]\n", nodeName, node);
            _treeViewSelectedNode = node;

            // update selection
            _treeViewSelectionMask = (1 << _treeViewItemCount);
        }


        // make unique id for context popup based on node name
        std::string uniqueId = nodeName + std::to_string(_treeViewItemCount);

        // context menu
        ImGui::PushID(uniqueId.c_str());
        if (ImGui::BeginPopupContextItem("item context menu"))
        {
            if (ImGui::Selectable("Create child node")) _treeNodeAction = TreeNodeAction_Create;
            if (ImGui::Selectable("Delete node")) _treeNodeAction = TreeNodeAction_Delete;
            ImGui::EndPopup();

            print("Item %s is clicked [0x%x]\n", nodeName, node);

            // update selection
            _treeViewSelectedNode = node;
            _treeViewSelectionMask = (1 << _treeViewItemCount);

        }
        ImGui::PopID();

        // recurse childs
        if(hasChilds && open)
        {
            _visitNode(node);
        }
        ImGui::TreePop();
    }

    //! Recurse for all children.
    void _visitNode(Node* node)
    {
        for (Node* child = node->getFirstChild(); child != NULL; child = child->getNextSibling())
        {
            _addNodeToTreeView(child);
        }
    }

    //! Recurse for all sibling.
    void _visitScene(Scene* scene)
    {
        for (Node* node = scene->getFirstNode(); node != NULL; node = node->getNextSibling())
        {
            _addNodeToTreeView(node);
        }
    }

    //! create child node on parent or if empty on scene
    void _createChildNode(Node* parent, Scene* scene)
    {
        std::string s = "Node_";
        s.append(UUID::generateUUID());
        Node* node = Node::create(s.c_str());

        if(parent)
        {
            // add the new node as child of the selected node
            parent->addChild(node);
        }
        else
        {
            // add the new node to scene root
            scene->addNode(node);
        }
    }

    //! remove node from scene
    void _removeNode(Node* node, Scene* scene)
    {
        if(node)
        {
            node->removeAllChildren();
            scene->removeNode(node);
        }

        _treeViewClearSelection();
    }

    //! clear tree view selection
    void _treeViewClearSelection()
    {
        _treeViewSelectionMask = 0;
        _treeViewSelectedNode = nullptr;
    }

};


} // end namespace inGameEditor






static inGameEditor::SceneHierarchy _sceneHierarchy;


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

    // show scene hierarchy window
    _sceneHierarchy.run(_scene);
}






