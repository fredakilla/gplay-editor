#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>

#include "node-editor/common/CustomFlowScene.h"
#include "GraphView.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private Q_SLOTS:
    void showNode(Node& node);
    void newFile();
    void open();
    void save();
    void initNode(QtNodes::Node& node);

private:
    void shutdown();
    void createActions();
    void createMenus();
    void closeEvent(QCloseEvent* event) override;
    void timerEvent(QTimerEvent* event) override;

    QMenu *fileMenu;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;

    CustomFlowScene* _nodeScene;
    FlowView* _nodeView;
    QWidget* _viewportContainer;
    QWidget* _renderView;
    GraphView* _pathView;
    QDockWidget* _dockView;
    QDockWidget* _dockNodeGraph;
    QDockWidget* _dockGraph;
    int _gameLoopTimerId;
};

#endif // MAINWINDOW_H
