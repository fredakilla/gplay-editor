#include "SpkUtils.h"


SPK::Vector3D ToSpkVector3D(const eFXYZ& v)
{
    return SPK::Vector3D(v.x, v.y, v.z);
}

gplay::Vector3 ToGplayVector3(const SPK::Vector3D& v)
{
    return gplay::Vector3(v.x, v.y, v.z);
}

SPK::Color ToSpkColor(const eColor& c)
{
    return SPK::Color(c.r, c.g, c.b, c.a);
}
