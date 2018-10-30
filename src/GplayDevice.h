#ifndef GPDEVICE_H
#define GPDEVICE_H

#include <gplay-engine.h>
#include <spark/SPARK.h>

using namespace gplay;

class GplayDeviceGame : public Game
{
public:

    GplayDeviceGame();
    ~GplayDeviceGame();

    void createRenderWindow(void* hwnd);
    void runFrame();
    void stop();
    void resizeRenderView(int width, int height);
    void setCurentParticleSystem(SPK::Ref<SPK::System> sparkSystem);

private:
    void initialize() override;
    void finalize() override;
    void update(float elapsedTime) override;
    void render(float elapsedTime) override;

    void keyEvent(Keyboard::KeyEvent evt, int key) override;
    bool mouseEvent(Mouse::MouseEvent evt, int x, int y, int wheelDelta) override;

   // bool drawScene(gplay::Node* node);
   // bool updateEmitters(gplay::Node* node, float elapsedTime);

private:
    Platform* _platform;
    //Scene* _scene;
    //bool _isShowDebug;  // show debug shapes in effects
};

typedef Singleton<GplayDeviceGame> GplayDevice;

#endif // GPDEVICE_H
