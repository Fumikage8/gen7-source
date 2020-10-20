/**
 *  GAME FREAK inc.
 *
 *  @file   FieldEffectFishingUki.h
 *  @brief  フィールドエフェクト：うき
 *  @author Masanori Kanamaru
 *  @date   2015.10.28
 */


#if !defined(__FIELD_EFFECT_FISHING_UKI_H__)
#define __FIELD_EFFECT_FISHING_UKI_H__
#pragma once

// poke_3d
#include <model/include/gfl2_BaseModel.h>

// Field
#include "Field/FieldRo/include/Effect/FieldEffectBase.h"

#include "niji_conv_header/field/effect/fishing_buoy.h"

namespace Field
{
  class FieldRootNode;
  class MyRenderingPipeLine;
}

namespace gfl2
{
  namespace util
  {
    class GLHeapAllocator;
  }
  namespace renderingengine
  {
    namespace scenegraph
    {
      namespace resource
      {
        class ResourceNode;
      }
      namespace instance
      {
        class ModelInstanceNode;
      }
    }
  }
};

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )

class EffectFishingBuoy : public IEffectBase
{
  //! コピー禁止
  GFL_FORBID_COPY_AND_ASSIGN( EffectFishingBuoy);

public:
  struct SetupData
  {
    void*                       pResource;
    gfl2::heap::HeapBase*       pHeap;
    FieldRootNode*              pRootNode;
    MyRenderingPipeLine* pPipeLine;
    gfl2::math::Vector3         pos;
  };


  EffectFishingBuoy();

  virtual void Initialize(SetupData &setupData);

  virtual bool Terminate();

  virtual void Delete();

  virtual bool IsAnimationLastFrame();

  void ChangeAnimation(int motion);

protected:

  poke_3d::model::BaseModel   model;
  FieldRootNode*              pFieldRootNode;      //!< フィールドルートノード  
  Field::MyRenderingPipeLine* pRenderingPipeLine;  //!< 描画シーンへの追加削除用

  gfl2::util::GLHeapAllocator* pAllocator;

  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstanceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode*      pModelResourceRootNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode*      pMotionResourceRootNodes[BL_IDX_FISHING_BUOY_MAX - 1];

  virtual void updateNormal();
};

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )

#endif // !defined(__FIELD_EFFECT_FISHING_UKI_H__)
