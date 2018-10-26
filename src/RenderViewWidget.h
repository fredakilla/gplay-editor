#ifndef RENDERVIEWWIDGET_H
#define RENDERVIEWWIDGET_H

#include <QWidget>

class RenderViewWidget : public QWidget
{
    Q_OBJECT
public:
    RenderViewWidget(QWidget* parent = nullptr);

    void resizeEvent(QResizeEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);

Q_SIGNALS:
    void windowResized(const QSize& size);

private:

    // stuff used by GUI
    QList<int> m_keysDown;
    QPoint m_lastMousePos;
    QPoint m_mouseDownPos;
    bool m_mouseMoveEnabled;
};

#endif // RENDERVIEWWIDGET_H
