#include "ShadowSampler.hpp"
#include <Components/BsCMeshCollider.h>
#include <Components/BsCRenderable.h>
#include <Math/BsRay.h>
#include <Mesh/BsMesh.h>
#include <Physics/BsPhysicsMesh.h>
#include <RenderAPI/BsVertexDataDesc.h>
#include <Scene/BsSceneObject.h>

namespace REGoth
{
  ShadowSampler::ShadowSampler(const bs::HMesh& targetMesh, const bs::HMeshCollider& targetCollider)
  {
    doSanityChecks(targetMesh, targetCollider);

    m_targetCollider = targetCollider;

    extractBrightnessPerVertex(targetMesh);
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
    if (!m_targetCollider->rayCast(sampleRay, hit))
    {
      return false;
    }

    auto vertexIdx = hit.unmappedTriangleIdx * 3U;
  }

  void ShadowSampler::doSanityChecks(const bs::HMesh& targetMesh,
                                     const bs::HMeshCollider& targetCollider)
  {
    // Perform some sanity checks (for valid mesh data and existance of vertex colors)

    if (targetMesh == nullptr || targetCollider == nullptr)
    {
      // TODO: Error
    }

    auto originalMeshData = targetMesh->getCachedData();
    auto physicsMeshData  = targetCollider->getMesh()->getMeshData();

    if (originalMeshData == nullptr || physicsMeshData == nullptr)
    {
      // TODO: Error
    }

    auto vertexDesc         = originalMeshData->getVertexDesc();
    auto vertexColorElement = vertexDesc->getElement(bs::VertexElementSemantic::VES_COLOR);

    if (!vertexColorElement)
    {
      // TODO: Error
    }
  }

  std::function<bs::Color(bs::UINT32)> ShadowSampler::getVertexColorUnpackFunction(
      const bs::MeshData& meshData)
  {
    // The function to unpack vertex colors is determined by the type of the vertex color element

    auto vertexColorElement =
        meshData.getVertexDesc()->getElement(bs::VertexElementSemantic::VES_COLOR);

    std::function<bs::Color(bs::UINT32)> unpackFunction = bs::Color::fromRGBA;
    if (vertexColorElement->getType() == bs::VertexElementType::VET_COLOR_ARGB)
    {
      unpackFunction = bs::Color::fromARGB;
    }
    else if (vertexColorElement->getType() == bs::VertexElementType::VET_COLOR_ABGR)
    {
      unpackFunction = bs::Color::fromABGR;
    }

    return unpackFunction;
  }

  void ShadowSampler::extractBrightnessPerVertex(const bs::HMesh& targetMesh)
  {
    auto meshData = targetMesh->getCachedData();

    bs::Vector<bs::UINT32> vertexColors(meshData->getNumVertices());
    targetMesh->getCachedData()->getVertexData(bs::VertexElementSemantic::VES_COLOR,
                                               vertexColors.data(),
                                               vertexColors.size() * sizeof(bs::UINT32));

    m_targetBrightnessPerVertex.reserve(vertexColors.size());

    // Extract brightness per vertex as average over the color channels
    auto unpackColor = getVertexColorUnpackFunction(*meshData);
    for (bs::UINT32 packedColor : vertexColors)
    {
      bs::Color color = unpackColor(packedColor);
      m_targetBrightnessPerVertex.push_back((color.r + color.g + color.b) / 3.f);
    }
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