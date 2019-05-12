#pragma once

#include <RTTI/RTTIUtil.hpp>

#include <BsPrerequisites.h>
#include <Scene/BsComponent.h>

namespace REGoth
{
  class ShadowSampler;

  class ShadowSampling : public bs::Component
  {
  public:
    ShadowSampling(const bs::HSceneObject& parent);

    void setSampler(bs::SPtr<ShadowSampler> sampler);

    virtual void update() override;

  private:
    bs::SPtr<ShadowSampler> mSampler;

  public:
    REGOTH_DECLARE_RTTI(ShadowSampling);

  private:
    ShadowSampling() = default;  // For RTTI
  };

  using HShadowSampling = bs::GameObjectHandle<ShadowSampling>;
}  // namespace REGoth