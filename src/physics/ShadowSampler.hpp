#include <BsPrerequisites.h>

namespace REGoth
{
  struct ShadowSample
  {
    float m_brightnessFactor;
  };

  class ShadowSampler
  {
  public:
    ShadowSampler(bs::HMeshCollider collider);

    bool sampleFor(bs::HSceneObject so, ShadowSample& sample);

  private:
    bool getSampleRay(bs::HSceneObject so, bs::Ray& ray);

    bs::HMeshCollider m_collider;
  };
}  // namespace REGoth