//===================================================================
/**
 * @file    FieldEffectBase.h
 * @brief   フィールドエフェクト基底クラス
 * @author  saita_kazuki
 * @date    2015.03.18
 */
//===================================================================

#if !defined( FIELD_EFFECT_BASE_H_INCLUDED )
#define FIELD_EFFECT_BASE_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_math.h>

#include <Effect/include/gfl2_EffectHandle.h>

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

  namespace gfx {
    class IGLAllocator;
  }
}

namespace fld { namespace mmodel {
class MoveModel;
}}

namespace Field { namespace Effect {
class Manager;
class Model;
}}


// @todo デバッグフラグにしたい
//#define UNDER_RESOURCE_PARTICLE


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )


class FieldEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(FieldEffectBase); //コピーコンストラクタ＋代入禁止

public:

  /**
   * @brief   コンストラクタ
   * @param   pHeap インスタンス生成などに使用するヒープ
   * @param   pGLAllocator リソース確保用アロケータ
   * @param   pEffectManager フィールドエフェクト管理クラス
   */
  FieldEffectBase( gfl2::heap::HeapBase* pHeap, gfl2::gfx::IGLAllocator* pGLAllocator, Field::Effect::Manager* pEffectManager);

  /**
   * @brief   デストラクタ
   */
  virtual ~FieldEffectBase();

  /**
   * @brief   初期化
   */
  virtual void Initialize( fld::mmodel::MoveModel* pMoveModel);

  /**
   * @brief   再初期化
   */
  virtual void Reinitialize( fld::mmodel::MoveModel* pMoveModel);

  /**
   * @brief   破棄
   */
  virtual b8 Terminate();

  /**
   * @brief   更新
   */
  virtual void Update( fld::mmodel::MoveModel* pMoveModel);

  /**
   * @brief   終了したか
   */
  virtual b8 IsFinish();

  /**
   * @brief   エフェクトモデルインスタンスの取得
   */
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* GetEffectModelInstanceNode();

  /**
   * @brief   有効フラグの設定
   */
  void SetEnable( b8 enable) { m_isEnable = enable; }

protected:

  gfl2::heap::HeapBase*     m_pHeap;
  gfl2::gfx::IGLAllocator*  m_pGLAllocator;
  Field::Effect::Manager*   m_pEffectManager;

  gfl2::Effect::Handle      m_eftHandle;
  Field::Effect::Model*     m_pEffectModel;

  u8                        m_seq;
  b8                        m_isEnable;

  fld::mmodel::MoveModel*   m_pMoveModel;



  enum ResourceSwitch
  {
    RESOURCE_SWITCH_NONE,
    RESOURCE_SWITCH_PARTICLE,
    RESOURCE_SWITCH_MODEL,
  };
  s32 m_resourceSwitch;

#if PM_DEBUG
  void UpdateResourceSwitch();
  void SetStopCalcAndDraw_EmitterSet( bool isStopCalcAndDraw);
  void SetVisible_EffectModel( bool isVisible);
#endif // PM_DEBUG
};

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )

#endif // FIELD_EFFECT_BASE_H_INCLUDED
