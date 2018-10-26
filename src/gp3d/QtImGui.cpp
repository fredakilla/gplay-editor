#include "QtImGui.h"
#include "ImGuiRenderer.h"
#include <QWidget>

namespace QtImGui {

class QWidgetWindowWrapper : public WindowWrapper
{
public:
    QWidgetWindowWrapper(QWidget *w) : w(w) {}
    void installEventFilter(QObject *object) override
    {
        return w->installEventFilter(object);
    }
    QSize size() const override
    {
        return w->size();
    }
    qreal devicePixelRatio() const override
    {
        return w->devicePixelRatioF();
    }
    bool isActive() const override
    {
        return w->isActiveWindow();
    }
    QPoint mapFromGlobal(const QPoint &p) const override
    {
        return w->mapFromGlobal(p);
    }
private:
    QWidget *w;
};

void initialize(QWidget *window)
{
    ImGuiRenderer::instance()->initialize(new QWidgetWindowWrapper(window));
}

void newFrame()
{
    ImGuiRenderer::instance()->newFrame();
}

}

