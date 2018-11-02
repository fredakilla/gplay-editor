#include "InGameEditor.h"
#include "uuid.h"

namespace inGameEditor {

class NodeInspector
{
private:

    struct TransformValues
    {
        float translation[3];
        float rotation[3];
        float scale[3];

        TransformValues()
        {
            reset();
        }

        void reset()
        {
            translation[0] = translation[1] = translation[2] = 0.0f;
            rotation[0] = rotation[1] = rotation[2] = 0.0f;
            scale[0] = scale[1] = scale[2] = 1.0f;
        }
    };

    std::map<Node*, TransformValues> _transformMap;

public:
    NodeInspector()
    {

    }

    void run(Node* node)
    {

        ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(300, 0), ImGuiCond_FirstUseEver);

        ImGui::Begin("Node Inspector");

        if(node)
        {
            // if this is the first time we see this node in map
            // get original node transformation and map with editor TransformValues.
            if(_transformMap.count(node) == 0)
            {
                const Matrix& matrix = node->getMatrix();

                Vector3 s,t;
                Quaternion r;
                matrix.decompose(&s,&r,&t);

               _transformMap[node] = TransformValues();

               TransformValues& transformValues = _transformMap[node];

               transformValues.translation[0] = t.x;
               transformValues.translation[1] = t.y;
               transformValues.translation[2] = t.z;
               transformValues.scale[0] = s.x;
               transformValues.scale[1] = s.y;
               transformValues.scale[2] = s.z;

               float yaw, pitch, roll = 0.0f;
               r.computeEuler(&yaw, &pitch, &roll);
               transformValues.rotation[0] = yaw;
               transformValues.rotation[1] = pitch;
               transformValues.rotation[2] = roll;

               /*Vector3 euler = matrix.getEulerAngles();
               transformValues.rotation[0] = MATH_DEG_TO_RAD(euler.x);
               transformValues.rotation[1] = MATH_DEG_TO_RAD(euler.y);
               transformValues.rotation[2] = MATH_DEG_TO_RAD(euler.z);*/

            }

            // enabled
            {
                static bool enabled = true;
                enabled = node->isEnabled();
                if(ImGui::Checkbox("Enabled", &enabled))
                    node->setEnabled(enabled);
            }

            // name
            {
                static char str0[128] = "coucou";
                ImGui::InputText("Name", str0, IM_ARRAYSIZE(str0));
            }

            ImGui::Separator();
            ImGui::Text("Transformation");

            // reset button
            {
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.6f, 0.6f));
                if(ImGui::SmallButton("Reset"))
                {
                    TransformValues& transformValues = _transformMap[node];
                    transformValues.reset();

                    node->setIdentity();
                }
                ImGui::PopStyleColor();
            }

            // transform
            {
                /*static float translation[] = { 0.0f, 0.0f, 0.0f };
                static float rotation[] = { 0.0f, 0.0f, 0.0f };
                static float scale[] = { 1.0f, 1.0f, 1.0f };*/
                TransformValues& transformValues = _transformMap[node];

                bool isTranslationUpdated = false;
                bool isRotationUpdated = false;
                bool isScaleUpdated = false;

                if(ImGui::DragFloat3("Translation", transformValues.translation, 0.025f)) isTranslationUpdated = true;
                if(ImGui::DragFloat3("Rotation", transformValues.rotation, 0.025f)) isRotationUpdated = true;
                if(ImGui::DragFloat3("Scale", transformValues.scale, 0.025f)) isScaleUpdated = true;

                if(isScaleUpdated || isRotationUpdated || isTranslationUpdated)
                {
                    Vector3 s(transformValues.scale);
                    Vector3 t(transformValues.translation);
                    Quaternion r;

                    //Quaternion::createFromEuler(rotation[0], rotation[1], rotation[2], &r);

                    Matrix m;
                    Matrix::createFromEuler(transformValues.rotation[1], transformValues.rotation[0], transformValues.rotation[2], &m);
                    Quaternion::createFromRotationMatrix(m, &r);

                    Transform transform(s,r,t);
                    node->set(transform);
                }
            }

#if 0

            // node translation
            {
                static float pos[] = { 0.0f, 0.0f, 0.0f };
                pos[0] = node->getTranslationX();
                pos[1] = node->getTranslationY();
                pos[2] = node->getTranslationZ();
                if(ImGui::DragFloat3("Position", pos, 0.025f))
                    node->setTranslation(pos[0], pos[1], pos[2]);
            }

            // node rotation
            {

                static float rot[] = { 0.0f, 0.0f, 0.0f };

                // method 1 - from axis and angle
                {
                    Vector3 rotation;
                    float angle = node->getRotation(&rotation);
                    rotation *= angle;

                    rot[0] = rotation.x;
                    rot[1] = rotation.y;
                    rot[2] = rotation.z;

                    if(ImGui::DragFloat3("Rotation", rot, 0.025f))
                    {
                        rotation = Vector3(&rot[0]);
                        angle = rotation.length();
                        rotation.normalize();
                        node->setRotation(rotation, angle);
                    }
                }


                // method 2 - from euler
               /* {
                    Quaternion rotation;
                    node->getRotation(&rotation);
                    rotation.computeEuler(&rot[0], &rot[1], &rot[2]);

                    if(ImGui::DragFloat3("Rotation", rot, 0.025f))
                    {
                        Quaternion q;
                        Quaternion::createFromEuler(rot[0], rot[1], rot[2], &q);
                        node->setRotation(q);
                    }
                }*/

            }

            // node scale
            {
                static float scale[] = { 0.0f, 0.0f, 0.0f };
                scale[0] = node->getScaleX();
                scale[1] = node->getScaleY();
                scale[2] = node->getScaleZ();
                if(ImGui::DragFloat3("Scale", scale, 0.025f))
                    node->setScale(scale[0], scale[1], scale[2]);
            }

#endif


            // tag
            {

            }



        }

        ImGui::End();
    }


};





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

    Node* getSelectedNode()
    {
        return _treeViewSelectedNode;
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
static inGameEditor::NodeInspector _nodeInspector;


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



    // test model

    Bundle* bundle = Bundle::create("res/data/scenes/box.gpb");
    Model* modelBox = Model::create(bundle->loadMesh("box_Mesh"));
    modelBox->setMaterial("res/data/materials/box.material");
    Node* nodeBox = Node::create("nodeBox");
    nodeBox->setDrawable(modelBox);
    SAFE_RELEASE(modelBox);
    _scene->addNode(nodeBox);

    nodeBox->setScale(2,1,3);
    nodeBox->setTranslation(1,2,-3);
    //nodeBox->setRotation(Vector3(-0.2,4,1.456), MATH_DEG_TO_RAD(25));

    Quaternion q;
    Quaternion::createFromEuler(2,3,1,&q);
    nodeBox->setRotation(q);

    SAFE_RELEASE(nodeBox);
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

    // show inspector window
   _nodeInspector.run(_sceneHierarchy.getSelectedNode());

}






