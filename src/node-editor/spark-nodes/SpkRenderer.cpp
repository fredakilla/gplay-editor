#include "SpkRenderer.h"

#include <spark/SPARK.h>
#include <sparkparticles/SparkParticleEmitter.h>
#include <sparkparticles/SparkQuadRenderer.h>
#include <sparkparticles/SparkParticleEmitter.h>
#include "SpkUtils.h"


void createDebugGeomteriesFromZone(const SPK::Ref<SPK::Zone> zone)
{
    // every zone has a position
    const SPK::Vector3D pos = zone->getPosition();

    if(zone->getClassName() == "Point")
    {
        DebugDraw::getInstance()->drawSphere(Vector3(pos.x, pos.y, pos.z), 0.1f, Vector3(1,1,1));
    }
    else if(zone->getClassName() == "Sphere")
    {
        const SPK::Sphere* sphere = dynamic_cast<SPK::Sphere*>(zone.get());
        GP_ASSERT(sphere);
        float radius = sphere->getRadius();
        DebugDraw::getInstance()->drawSphere(Vector3(pos.x, pos.y, pos.z), radius, Vector3(1,1,1));
    }
    else if(zone->getClassName() == "Plane")
    {
        const SPK::Plane* plane = dynamic_cast<SPK::Plane*>(zone.get());
        GP_ASSERT(plane);
        const SPK::Vector3D normal = plane->getNormal();
        DebugDraw::getInstance()->drawPlane(Vector3(normal.x, normal.y, normal.z), 0.0f, Matrix::identity(), Vector3(1,1,1));
    }
    else if(zone->getClassName() == "Box")
    {
        const SPK::Box* box = dynamic_cast<SPK::Box*>(zone.get());
        GP_ASSERT(box);
        Vector3 scale = ToGplayVector3(box->getDimensions());
        Matrix matrix;
        // todo: fix rotation
        Matrix::createTranslation(ToGplayVector3(pos), &matrix);
        BoundingBox bbox(-scale/2.0f, scale/2.0f);
        bbox *= matrix;
        DebugDraw::getInstance()->drawBox(bbox.min, bbox.max, Vector3(1,1,1));
    }
    else if(zone->getClassName() == "Cylinder")
    {
        const SPK::Cylinder* cylinder = dynamic_cast<SPK::Cylinder*>(zone.get());
        GP_ASSERT(cylinder);

        const SPK::Vector3D axis = cylinder->getAxis();
        float height = cylinder->getHeight();
        float radius = cylinder->getRadius();

        // todo: fix rotation
        Matrix matrix;
        Matrix::createTranslation(ToGplayVector3(pos), &matrix);
        DebugDraw::getInstance()->drawCylinder(radius, height/2.0f, 1, matrix, Vector3(1,1,1));
    }
    else if(zone->getClassName() == "Ring")
    {
        const SPK::Ring* ring = dynamic_cast<SPK::Ring*>(zone.get());
        GP_ASSERT(ring);

        float minRadius = ring->getMinRadius();
        float maxRadius = ring->getMaxRadius();

        // todo: fix rotation

        DebugDraw::getInstance()->drawArc(ToGplayVector3(pos), Vector3(0,1,0), Vector3(1,0,0), minRadius, minRadius, 0.0f, MATH_DEG_TO_RAD(360.0f), Vector3(1,1,1), false);
        DebugDraw::getInstance()->drawArc(ToGplayVector3(pos), Vector3(0,1,0), Vector3(1,0,0), maxRadius, maxRadius, 0.0f, MATH_DEG_TO_RAD(360.0f), Vector3(1,1,1), false);
    }
}

void drawDebugShapes(SparkParticleEmitter* spkEffect, Scene* scene)
{
    DebugDraw::getInstance()->begin(scene->getActiveCamera()->getViewProjectionMatrix());

    SPK::Ref<SPK::System> spkSystem = spkEffect->getSparkSystem();
    for(size_t nGroup = 0; nGroup < spkSystem->getNbGroups(); nGroup++)
    {
        // show emitters zones
        for(size_t nEmitter = 0; nEmitter < spkSystem->getGroup(nGroup)->getNbEmitters(); nEmitter++)
        {
            const SPK::Ref<SPK::Zone> zone = spkSystem->getGroup(nGroup)->getEmitter(nEmitter)->getZone();
            createDebugGeomteriesFromZone(zone);
        }

        // show modifiers zones
        for(size_t nModifiers = 0; nModifiers < spkSystem->getGroup(nGroup)->getNbModifiers(); nModifiers++)
        {
            const SPK::Ref<SPK::Modifier> modifier = spkSystem->getGroup(nGroup)->getModifier(nModifiers);

            if(modifier->getClassName() == "PointMass")
            {
                const SPK::PointMass* pointMass = dynamic_cast<SPK::PointMass*>(modifier.get());
                GP_ASSERT(pointMass);
                const SPK::Vector3D pos = pointMass->getPosition();
                DebugDraw::getInstance()->drawSphere(Vector3(pos.x, pos.y, pos.z), 0.25f, Vector3(0,1,0));
            }
            else if(modifier->getClassName() == "Destroyer")
            {
                const SPK::Destroyer* destroyer = dynamic_cast<SPK::Destroyer*>(modifier.get());
                GP_ASSERT(destroyer);
                createDebugGeomteriesFromZone(destroyer->getZone());
            }
            else if(modifier->getClassName() == "Obstacle")
            {
                const SPK::Obstacle* obstacle = dynamic_cast<SPK::Obstacle*>(modifier.get());
                GP_ASSERT(obstacle);
                createDebugGeomteriesFromZone(obstacle->getZone());
            }
            else if(modifier->getClassName() == "LinearForce")
            {
                const SPK::LinearForce* linearForce = dynamic_cast<SPK::LinearForce*>(modifier.get());
                GP_ASSERT(linearForce);
                createDebugGeomteriesFromZone(linearForce->getZone());
            }
        }
    }

    DebugDraw::getInstance()->end();
}







SpkRenderer::SpkRenderer() :
    GPRenderer3D(),
    _isShowDebug(true)
{

}

void SpkRenderer::setCurentParticleSystem(SPK::Ref<SPK::System> sparkSystem)
{
    Node* node = _scene->findNode("sparkSystem");
    if(node)
        _scene->removeNode(node);

    // Create a node in scene and attach spark foutain effect
    SparkParticleEmitter* foutainEmitter = SparkParticleEmitter::createRef(sparkSystem, true);
    Node* particleNode = Node::create("sparkSystem");
    particleNode->setDrawable(foutainEmitter);
    particleNode->setTranslation(0.0f, 0.0f, 0.0f);

    _scene->addNode(particleNode);
}

void SpkRenderer::update(float elapsedTime)
{
    // call super class method
    GPRenderer3D::update(elapsedTime);

    _scene->visit(this, &SpkRenderer::updateEmitters, elapsedTime);

}

void SpkRenderer::render(float elapsedTime)
{
    View::getView(0)->bind();
    _scene->visit(this, &SpkRenderer::drawScene);
}

bool SpkRenderer::drawScene(Node* node)
{
    Drawable* drawable = node->getDrawable();
    if (drawable)
        drawable->draw();
    return true;
}

bool SpkRenderer::updateEmitters(Node* node, float elapsedTime)
{
    SparkParticleEmitter* spkEffect = dynamic_cast<SparkParticleEmitter*>(node->getDrawable());
    if (spkEffect)
        spkEffect->update(elapsedTime);

    // show debug shapes for all zones in spark system
    if(spkEffect && _isShowDebug)
    {
        drawDebugShapes(spkEffect, _scene);
    }

    return true;
}

