#include "ShadowSampler.hpp"
#include <Components/BsCMeshCollider.h>
#include <Components/BsCRenderable.h>
#include <Math/BsRay.h>
#include <Scene/BsSceneObject.h>

namespace REGoth
{
  ShadowSampler::ShadowSampler(bs::HMeshCollider collider)
      : m_collider(collider)
  {
  }

  bool ShadowSampler::sampleFor(bs::HSceneObject so, ShadowSample& sample)
  {
    bs::Ray sampleRay;
    if (!getSampleRay(so, sampleRay))
    {
      // TODO: Error as we tried to sample for a scene object without visual representation?
      return false;
    }

    // Trace a sample ray from the scene object to our associated collider
    bs::PhysicsQueryHit hit;
    if (!m_collider->rayCast(sampleRay, hit))
    {
      return false;
    }

    // Obtain the mesh
    auto mesh = m_collider->getMesh();
    if (!mesh)
    {
      // TODO: Error
    }

    //
  }

  bool ShadowSampler::getSampleRay(bs::HSceneObject so, bs::Ray& ray)
  {
    // Without a renderable component we cannot construct the sample ray
    auto renderable = so->getComponent<bs::CRenderable>();
    if (!renderable)
    {
      return false;
    }

    auto aabb = renderable->getBounds().getBox();

    // TODO: Construct a ray that traces down (look at the original REGoth for reference)
    ray = bs::Ray(aabb.getCenter(), bs::Vector3(0.f, 0.f, -1.f));

    return true;
  }
}  // namespace REGoth