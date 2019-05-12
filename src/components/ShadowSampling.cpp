#include "ShadowSampling.hpp"
#include <RTTI/RTTI_ShadowSampling.hpp>
#include <visual/ShadowSampler.hpp>

#include <Components/BsCRenderable.h>
#include <Material/BsMaterial.h>
#include <Scene/BsSceneObject.h>

namespace REGoth
{
  ShadowSampling::ShadowSampling(const bs::HSceneObject& parent)
      : bs::Component(parent)
  {
  }

  void ShadowSampling::setSampler(bs::SPtr<ShadowSampler> sampler)
  {
    // Check if null?

    mSampler = sampler;
  }

  void ShadowSampling::update()
  {
    // Collect renderables of all children
    bs::Queue<bs::HSceneObject> soQueue;
    soQueue.emplace(SO());

    bs::Vector<bs::HSceneObject> sosWithRenderables;

    while (!soQueue.empty())
    {
      auto obj = soQueue.front();
      soQueue.pop();

      if (obj->hasComponent<bs::CRenderable>())
      {
        sosWithRenderables.push_back(obj);
      }

      // Add children
      auto numChildren = obj->getNumChildren();
      for (bs::UINT32 i = 0; i < numChildren; ++i)
      {
        soQueue.emplace(obj->getChild(i));
      }
    }

    for (const auto& obj : sosWithRenderables)
    {
      ShadowSample sample;
      if (mSampler->sampleFor(obj, sample))
      {
        auto renderable = obj->getComponent<bs::CRenderable>();
        auto materials  = renderable->getMaterials();
        for (auto& mat : materials)
        {
          mat->setFloat("gShadowValue", sample.brightness);
        }
      }
    }
  }

  REGOTH_DEFINE_RTTI(ShadowSampling)

}  // namespace REGoth
