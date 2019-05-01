#include <BsPrerequisites.h>

namespace REGoth
{
  struct ShadowSample
  {
    float m_brightness;
  };

  class ShadowSampler
  {
  public:
    ShadowSampler(const bs::HMesh& targetMesh, const bs::HMeshCollider& targetCollider);

    bool sampleFor(bs::HSceneObject so, ShadowSample& sample);

  private:
    void doSanityChecks(const bs::HMesh& targetMesh, const bs::HMeshCollider& targetCollider);

    std::function<bs::Color(bs::UINT32)> getVertexColorUnpackFunction(const bs::MeshData& meshData);

    void extractBrightnessPerVertex(const bs::HMesh& targetMesh);

    bool getSampleRay(bs::HSceneObject so, bs::Ray& ray);

    bs::HMeshCollider m_targetCollider;
    bs::Vector<float> m_targetBrightnessPerVertex;
  };
}  // namespace REGoth