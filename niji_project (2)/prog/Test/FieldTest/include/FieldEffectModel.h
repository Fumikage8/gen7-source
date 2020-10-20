//===================================================================
/**
 * @file    FieldEffectModel.h
 * @brief   エフェクトモデルラス
 * @author  saita_kazuki
 * @date    2015.03.17
 */
//===================================================================

#if !defined( FIELD_EFFECT_MODEL_INCLUDED )
#define FIELD_EFFECT_MODEL_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_math.h>

#include <Animation/include/gfl2_AnimationController.h>
#include <Animation/include/gfl2_JointController.h>

namespace gfl2
{
  namespace renderingengine { namespace scenegraph
  {
    namespace resource
    {
      class ResourceNode;
    }
    namespace instance
    {
      class ModelInstanceNode;
    }
  }}

  namespace animation {
    class AnimationController;
  }
}

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )

class Model
{
  GFL_FORBID_COPY_AND_ASSIGN(Model); //コピーコンストラクタ＋代入禁止

public:

  /**
   * @brief       コンストラクタ
   */
  Model();

  /**
   * @brief       デストラクタ
   */
  virtual ~Model();

  /**
   * @brief       初期化
   */
  void Initialize( gfl2::heap::HeapBase* pHeap,
    gfl2::renderingengine::scenegraph::resource::ResourceNode* pModelRootNode,
    gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionRootNode,
    gfl2::animation::AnimationController::InitDescription& desc);

  /**
   * @brief       破棄
   */
  void Terminate();

  /**
   * @brief       更新
   */
  void Update();

  /**
   * @brief       表示フラグ変更
   */
  void SetVisible( b8 isVisible);

  /**
   * @brief       表示フラグ取得
   */
  b8 IsVisible();

  gfl2::animation::AnimationController* GetAnimationController() { return m_pAnimationController; }
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* GetModelInstanceNode() { return m_pModelInstanceNode; }

private:

  gfl2::renderingengine::scenegraph::resource::ResourceNode*        m_pModelRootNode;
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*   m_pModelInstanceNode;

  gfl2::renderingengine::scenegraph::resource::ResourceNode*        m_pMotionRootNode;
  gfl2::animation::AnimationController*                             m_pAnimationController;
};

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )

#endif // FIELD_EFFECT_MODEL_INCLUDED
