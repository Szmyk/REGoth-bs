#include <BsPrerequisites.h>

#pragma once

namespace REGoth
{
  struct ShadowSample
  {
    float m_brightness;
  };

  class ShadowSampler
  {
  public:
    ShadowSampler() = default;

    ShadowSampler(const bs::HMesh& mesh, const bs::HMeshCollider& collider);

    bool sampleFor(bs::HSceneObject querySO, ShadowSample& sample) const;

  private:
    struct Face
    {
      bs::UINT32 vertexIdx1;
      bs::UINT32 vertexIdx2;
      bs::UINT32 vertexIdx3;
    };

    using FaceAccessorType = std::function<Face(const bs::MeshData&, bs::UINT32)>;

    using VertexColorUnpackerType = std::function<bs::Color(bs::UINT32)>;

    /**
     * Checks if the the given mesh and collider are valid and if the mesh has vertex colors.
     */
    static void doSanityChecks(const bs::HMesh& mesh, const bs::HMeshCollider& collider);

    /**
     * Utility returning a function that can unpack raw vertex color data being in the format of the
     * vertex colors in the given bs::MeshData.
     */
    static VertexColorUnpackerType getVertexColorUnpackFunction(const bs::MeshData& meshData);

    /**
     * Utility returning a function that can access faces in a bs::MeshData object having the same
     * index type as the given one.
     */
    static FaceAccessorType getFaceAccessor(const bs::MeshData& meshData);

    /**
     * Extracts the brightness per vertex from the given meshData as average over the three vertex
     * color channels.
     */
    void extractBrightnessPerVertex(bs::MeshData& meshData);

    /**
     * Construct a (downward pointing) sample ray which is used for shadow sampling.
     * The method assumes that the query object has a CRenderable component attached and returns
     * false if any errors are encountered.
     */
    bool getSampleRay(bs::HSceneObject querySO, bs::Ray& ray) const;

    bs::HMesh m_mesh;
    bs::HMeshCollider m_collider;
    bs::Vector<float> m_brightnessPerVertex;
    FaceAccessorType m_faceAccessor;
  };
}  // namespace REGoth