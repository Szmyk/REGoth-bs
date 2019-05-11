#include "RaycastShadowSampler.hpp"
#include <RTTI/RTTI_RaycastShadowSampler.hpp>

#include <Components/BsCMeshCollider.h>
#include <Components/BsCRenderable.h>
#include <Math/BsRay.h>
#include <Mesh/BsMesh.h>
#include <Physics/BsPhysicsMesh.h>
#include <RenderAPI/BsVertexDataDesc.h>
#include <Scene/BsSceneObject.h>

namespace REGoth
{
  RaycastShadowSampler::RaycastShadowSampler(const bs::HMesh& mesh,
                                             const bs::HMeshCollider& collider)
      : mMesh(mesh)
      , mCollider(collider)
  {
    doSanityChecks(mMesh, mCollider);

    extractBrightnessPerVertex(*(mMesh->getCachedData()));

    mFaceAccessor = getFaceAccessor(*(mMesh->getCachedData()));
  }

  bool RaycastShadowSampler::sampleFor(bs::HSceneObject querySO, ShadowSample& sample) const
  {
    bs::Ray sampleRay;
    if (!getSampleRay(querySO, sampleRay))
    {
      // TODO: Error as we tried to sample for a scene object without visual representation?
      return false;
    }

    // Trace a sample ray from the scene object to our associated collider
    bs::PhysicsQueryHit hit;
    if (!mCollider->rayCast(sampleRay, hit))
    {
      return false;
    }

    // Obtain brightness for the hit point through barycentric coordinates
    auto face = mFaceAccessor(*(mMesh->getCachedData()), hit.unmappedTriangleIdx);
    float a   = (hit.uv.x) * mBrightnessPerVertex[face.vertexIdx1];
    float b   = (hit.uv.y) * mBrightnessPerVertex[face.vertexIdx2];
    float c   = (1.f - hit.uv.x - hit.uv.y) * mBrightnessPerVertex[face.vertexIdx3];

    sample.brightness = a + b + c;

    return true;
  }

  void RaycastShadowSampler::doSanityChecks(const bs::HMesh& mesh, const bs::HMeshCollider& collider)
  {
    // Perform some sanity checks (for valid mesh data and existance of vertex colors)

    if (mesh == nullptr || collider == nullptr)
    {
      // TODO: Error
    }

    auto originalMeshData = mesh->getCachedData();

    if (originalMeshData == nullptr)
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

  RaycastShadowSampler::VertexColorUnpackerType RaycastShadowSampler::getVertexColorUnpackFunction(
      const bs::MeshData& meshData)
  {
    auto vertexColorElement =
        meshData.getVertexDesc()->getElement(bs::VertexElementSemantic::VES_COLOR);

    switch (vertexColorElement->getType())
    {
      case bs::VertexElementType::VET_COLOR_ARGB:
        return bs::Color::fromARGB;
      case bs::VertexElementType::VET_COLOR_ABGR:
        return bs::Color::fromABGR;
      default:
        return bs::Color::fromRGBA;
    }
  }

  RaycastShadowSampler::FaceAccessorType RaycastShadowSampler::getFaceAccessor(
      const bs::MeshData& meshData)
  {
    RaycastShadowSampler::FaceAccessorType accessor;

    switch (meshData.getIndexType())
    {
      case bs::IndexType::IT_16BIT:
        accessor = [](const bs::MeshData& meshData, bs::UINT32 faceIndex) {
          auto indices = meshData.getIndices16();
          return RaycastShadowSampler::Face{
              static_cast<bs::UINT32>(indices[faceIndex]),
              static_cast<bs::UINT32>(indices[faceIndex + 1]),
              static_cast<bs::UINT32>(indices[faceIndex + 2]),
          };
        };
        break;
      case bs::IndexType::IT_32BIT:
        accessor = [](const bs::MeshData& meshData, bs::UINT32 faceIndex) {
          auto indices = meshData.getIndices32();
          return RaycastShadowSampler::Face{
              indices[faceIndex],
              indices[faceIndex + 1],
              indices[faceIndex + 2],
          };
        };
        break;
      default:
        // TODO: Error
        break;
    }

    return accessor;
  }

  void RaycastShadowSampler::extractBrightnessPerVertex(bs::MeshData& meshData)
  {
    bs::Vector<bs::UINT32> vertexColors(meshData.getNumVertices());
    meshData.getVertexData(bs::VertexElementSemantic::VES_COLOR, vertexColors.data(),
                           static_cast<unsigned int>(vertexColors.size() * sizeof(bs::UINT32)));

    mBrightnessPerVertex.reserve(vertexColors.size());

    // Extract brightness per vertex as average over the color channels
    auto unpackColor = getVertexColorUnpackFunction(meshData);
    for (bs::UINT32 packedColor : vertexColors)
    {
      bs::Color color = unpackColor(packedColor);
      mBrightnessPerVertex.push_back((color.r + color.g + color.b) / 3.f);
    }
  }

  bool RaycastShadowSampler::getSampleRay(bs::HSceneObject querySO, bs::Ray& ray) const
  {
    // Without a renderable component we cannot construct the sample ray
    auto renderable = querySO->getComponent<bs::CRenderable>();
    if (!renderable)
    {
      return false;
    }

    auto aabb = renderable->getBounds().getBox();

    // TODO: Construct a ray that traces down (look at the original REGoth for reference)
    ray = bs::Ray(aabb.getCenter(), bs::Vector3(0.f, -1.f, 0.f));

    return true;
  }

  REGOTH_DEFINE_RTTI(RaycastShadowSampler)
}  // namespace REGoth