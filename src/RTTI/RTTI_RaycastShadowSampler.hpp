#pragma once

#include "RTTIUtil.hpp"
#include <visual/RaycastShadowSampler.hpp>

#include <Mesh/BsMesh.h>

using namespace bs; // FIXME: This is ugly and dangereous but necessary as BS_RTTI_MEMBER_PLAIN_ARRAY uses UINT32 without the bs namespace.

namespace REGoth
{
  class RTTI_RaycastShadowSampler
      : public bs::RTTIType<RaycastShadowSampler, ShadowSampler, RTTI_RaycastShadowSampler>
  {
    BS_BEGIN_RTTI_MEMBERS
    BS_RTTI_MEMBER_REFL(mMesh, 0)
    BS_RTTI_MEMBER_REFL(mCollider, 1)
    BS_RTTI_MEMBER_PLAIN_ARRAY(mBrightnessPerVertex, 2)
    BS_END_RTTI_MEMBERS

  public:
    RTTI_RaycastShadowSampler()
    {
    }

    virtual void onDeserializationEnded(bs::IReflectable* obj,
                                        bs::SerializationContext* context) override
    {
      auto sampler = reinterpret_cast<RaycastShadowSampler*>(obj);

      // Make sure to set the face accessor (which is a lambda and cannot be (de)serialized)
      sampler->mFaceAccessor =
          RaycastShadowSampler::getFaceAccessor(*(sampler->mMesh->getCachedData()));
    }

    REGOTH_IMPLEMENT_RTTI_CLASS_FOR_COMPONENT(RaycastShadowSampler)
  };
}  // namespace REGoth