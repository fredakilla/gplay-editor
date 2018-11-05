#ifndef SPKRENDERER_H
#define SPKRENDERER_H

#include "../../gp3d/GPRenderer.h"
#include <spark/SPARK.h>

class SparkNodeRender : public GPRenderer3D
{
public:
    SparkNodeRender();

    void setCurentParticleSystem(SPK::Ref<SPK::System> sparkSystem);

    void update(float elapsedTime) override;
    void render(float elapsedTime) override;

    bool updateEmitters(Node* node, float elapsedTime);
    bool drawScene(Node* node);

private:
    bool _isShowDebug;  // show debug shapes in effects
};

#endif // SPKRENDERER_H
