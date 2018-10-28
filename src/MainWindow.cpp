#include "MainWindow.h"
#include "GPDevice.h"

#include <QMessageBox>
#include <QCloseEvent>
#include <QDebug>
#include <QMenuBar>
#include <QApplication>
#include <QVBoxLayout>

#include "node-editor/common/Nodestyle.h"
#include "node-editor/spark-nodes/spark-nodes.h"

#include <nodes/DataModelRegistry>
using QtNodes::DataModelRegistry;


#include "gp3d/GPRenderer.h"
GPRenderer3D* _gpRenderer;



static std::shared_ptr<DataModelRegistry> registerDataModels()
{
    auto ret = std::make_shared<DataModelRegistry>();

    ret->registerModel<NodeSparkTest>("test");

    ret->registerModel<NodeSparkZonePoint>("zones");
    ret->registerModel<NodeSparkZonePlane>("zones");
    ret->registerModel<NodeSparkZoneSphere>("zones");
    ret->registerModel<NodeSparkZoneBox>("zones");
    ret->registerModel<NodeSparkZoneCylinder>("zones");
    ret->registerModel<NodeSparkZoneRing>("zones");

    ret->registerModel<NodeSparkGroup>("system");
    ret->registerModel<NodeSparkSystem>("system");
    ret->registerModel<NodeSparkGroupList>("system");
    ret->registerModel<NodeSparkQuadRenderer>("renderer");

    ret->registerModel<NodeSparkEmitterList>("emitters");
    ret->registerModel<NodeSparkEmitterStatic>("emitters");
    ret->registerModel<NodeSparkEmitterSpheric>("emitters");
    ret->registerModel<NodeSparkEmitterRandom>("emitters");
    ret->registerModel<NodeSparkEmitterStraight>("emitters");
    ret->registerModel<NodeSparkEmitterNormal>("emitters");

    ret->registerModel<NodeSparkModifierList>("modifiers");
    ret->registerModel<NodeSparkModifierGravity>("modifiers");
    ret->registerModel<NodeSparkModifierFriction>("modifiers");
    ret->registerModel<NodeSparkModifierCollider>("modifiers");
    ret->registerModel<NodeSparkModifierDestroyer>("modifiers");
    ret->registerModel<NodeSparkModifierObstacle>("modifiers");
    ret->registerModel<NodeSparkModifierPointMass>("modifiers");
    ret->registerModel<NodeSparkModifierRandomForce>("modifiers");
    ret->registerModel<NodeSparkModifierRotator>("modifiers");
    ret->registerModel<NodeSparkModifierVortex>("modifiers");
    ret->registerModel<NodeSparkModifierEmitterAttacher>("modifiers");
    ret->registerModel<NodeSparkModifierLinearForce>("modifiers");

    ret->registerModel<NodePath>("interpolators");
    ret->registerModel<NodeSparkInterpolator_ColorInitializerDefault>("interpolators");
    ret->registerModel<NodeSparkInterpolator_ColorInitializerRandom>("interpolators");
    ret->registerModel<NodeSparkInterpolator_ColorInterpolatorSimple>("interpolators");
    ret->registerModel<NodeSparkInterpolator_ColorInterpolatorRandom>("interpolators");
    ret->registerModel<NodeSparkInterpolator_ColorInterpolatorGraph>("interpolators");

    ret->registerModel<NodeSparkInterpolatorParamList>("interpolators");
    ret->registerModel<NodeSparkInterpolator_ParamInitializer>("interpolators");
    ret->registerModel<NodeSparkInterpolator_ParamInitializerRandom>("interpolators");
    ret->registerModel<NodeSparkInterpolator_ParamInterpolatorSimple>("interpolators");
    ret->registerModel<NodeSparkInterpolator_ParamInterpolatorRandom>("interpolators");

    return ret;
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    //setNodeStyle();
    createActions();
    createMenus();


    // create widget for gplay engine view
    _renderView = new QWidget(this);
    _renderView->setMouseTracking(true);
    _renderView->setFocusPolicy(Qt::StrongFocus);

    _viewportContainer = new QWidget(this);
    _viewportContainer->setLayout(new QVBoxLayout());
    _viewportContainer->layout()->addWidget(_renderView);


    _dockView = new QDockWidget("Viewport", this);
    _dockView->setWidget(_viewportContainer);
    _dockView->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::TopDockWidgetArea, _dockView);

    GplayDevice::get().createRenderWindow((void*)_renderView);
    _gpRenderer = new OpClassNode_Renderer();


    _nodeScene = new CustomFlowScene(registerDataModels());
    _nodeView = new FlowView(_nodeScene);
    _nodeView->setWindowTitle("Node-based flow editor");
    _nodeView->resize(800, 600);
    _nodeView->show();
    _nodeView->scale(0.9, 0.9);

    _dockNodeGraph = new QDockWidget("NodeGraph", this);
    _dockNodeGraph->setWidget(_nodeView);
    _dockNodeGraph->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, _dockNodeGraph);

    _pathView = new GraphView(this);

    _dockGraph = new QDockWidget("Graph", this);
    _dockGraph->setWidget(_pathView);
    _dockGraph->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::TopDockWidgetArea, _dockGraph);


    connect(_nodeScene, &FlowScene::nodeDoubleClicked, this, &MainWindow::showNode);
    connect(_nodeScene, &FlowScene::nodeCreated, this, &MainWindow::initNode);

    // connect to FlowView deleteSelectionAction a method to delete comments graphics items.
    QAction* deleteAction = _nodeView->deleteSelectionAction();
    connect(deleteAction, &QAction::triggered, _nodeScene, &CustomFlowScene::deleteSelectedComments);

    _gameLoopTimerId = startTimer(0);
}

MainWindow::~MainWindow()
{
    delete _nodeScene;
    delete _nodeView;
    delete _renderView;
    delete _viewportContainer;
    delete _dockView;
    delete _dockNodeGraph;
}

void MainWindow::createActions()
{
    newAct = new QAction(tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("New"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);

    openAct = new QAction(tr("&Open"), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);

    saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
}

void MainWindow::newFile()
{
    _nodeScene->clearScene();
    _nodeScene->clearComments();
}

void MainWindow::open()
{
    _nodeScene->load();
}

void MainWindow::save()
{
    _nodeScene->save();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, qApp->applicationName(),
                                                                    tr("Are you sure?\n"),
                                                                    QMessageBox::No | QMessageBox::Yes,
                                                                    QMessageBox::Yes);
    if(resBtn != QMessageBox::Yes)
    {
        event->ignore();
    }
    else
    {
        event->accept();
        shutdown();
    }
}

void MainWindow::shutdown()
{
    killTimer(_gameLoopTimerId);
    GplayDevice::get().stop();
}


float m_time = 0;

void MainWindow::timerEvent(QTimerEvent* event)
{
    QWidget::timerEvent(event);

    // compute timestep for manually updating gplay
    static float lasttime = 0.0f;
    float step = m_time - lasttime;
    lasttime = m_time;
    if(step < 0)
        step = 0.0f;    

    GplayDevice::get().beginFrame();
    _gpRenderer->update(step);
    _gpRenderer->render();
    GplayDevice::get().endFrame();
}

void MainWindow::showNode(QtNodes::Node& node)
{
    // is a system node ?
    NodeSparkSystem* systemNode = dynamic_cast<NodeSparkSystem*>(node.nodeDataModel());
    if(systemNode)
    {
        if(systemNode->getResult().get() != nullptr)
        {
           GplayDevice::get().setCurentParticleSystem(systemNode->getResult());
        }

        return;
    }

    // is a Path node ?
    NodePath* pathNode = dynamic_cast<NodePath*>(node.nodeDataModel());
    if(pathNode)
    {
        if(pathNode->getResult() != nullptr)
        {
            _pathView->setPathNode(pathNode);
        }
        return;
    }
}

void MainWindow::initNode(QtNodes::Node& node)
{
    BaseNode* baseNode = dynamic_cast<BaseNode*>(node.nodeDataModel());
    baseNode->init();
}
