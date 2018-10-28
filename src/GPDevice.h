#ifndef GPDEVICE_H
#define GPDEVICE_H

#include <gplay-engine.h>
#include <spark/SPARK.h>
#include <QObject>


using namespace gplay;



class GplayDevice : public Game
{
public:
    static GplayDevice& get();
    void createRenderWindow(void* hwnd);
    void beginFrame();
    void endFrame();
    void stop();
    void resizeRenderView(int width, int height);

    void setCurentParticleSystem(SPK::Ref<SPK::System> sparkSystem);

private:
    GplayDevice();
    ~GplayDevice();

    void initialize() override;
    void finalize() override;
    void update(float elapsedTime) override;
    void render(float elapsedTime) override;

    void keyEvent(Keyboard::KeyEvent evt, int key) override;
    bool mouseEvent(Mouse::MouseEvent evt, int x, int y, int wheelDelta) override;


    bool drawScene(gplay::Node* node);
    bool updateEmitters(gplay::Node* node, float elapsedTime);




    static GplayDevice* _instance;
    Platform* _platform;
    Scene* _scene;
    bool _isShowDebug;  // show debug shapes in effects
};

#endif // GPDEVICE_H
