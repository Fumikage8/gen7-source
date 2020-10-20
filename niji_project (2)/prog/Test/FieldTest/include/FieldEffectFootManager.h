//===================================================================
/**
 * @file    FieldEffectFootManager.h
 * @brief   フィールドエフェクト足元エフェクト管理クラス
 * @author  saita_kazuki
 * @date    2015.03.18
 */
//===================================================================

#if !defined( FIELD_EFFECT_FOOT_MANAGER_H_INCLUDED )
#define FIELD_EFFECT_FOOT_MANAGER_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_math.h>

namespace gfl2
{
  namespace gfx {
    class IGLAllocator;
  }
}

namespace fld { namespace mmodel {
class MoveModel;
}}

namespace Field { namespace Effect {
class Manager;
class FieldEffectBase;
}}

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FieldTest )
class MyRenderingPipeLine;
GFL_NAMESPACE_END( FieldTest )
GFL_NAMESPACE_END( Test )


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )


class FootManager
{
  GFL_FORBID_COPY_AND_ASSIGN(FootManager); //コピーコンストラクタ＋代入禁止

public:

  enum EffectType
  {
    EFFECT_TYPE_FOOT_WATER_SPRAY,

    EFFECT_TYPE_MAX,
  };

  static const u32 EFFECT_BUFFER_MAX        = 16;    // 生成可能なエフェクトの最大数

  /**
   * @brief   コンストラクタ
   * @param   pHeap インスタンス生成などに使用するヒープ
   * @param   pGLAllocator リソース確保用アロケータ
   * @param   pEffectManager フィールドエフェクト管理クラス
   */
  FootManager( gfl2::heap::HeapBase* pHeap, gfl2::gfx::IGLAllocator* pGLAllocator, Field::Effect::Manager* pEffectManager, Test::FieldTest::MyRenderingPipeLine* pRenderingPipeLine);

  /**
   * @brief   デストラクタ
   */
  virtual ~FootManager();

  /**
   * @brief   初期化
   */
  void Initialize();

  /**
   * @brief   破棄
   */
  b8 Terminate();

  /**
   * @brief   更新
   */
  void Update();

  /**
   * @brief   足元エフェクトを出す動作モデルを登録する
   */
  b8 RegisterMoveModel( fld::mmodel::MoveModel* pMoveModel);

protected:

  void UpdateEffect();
  void UpdateCreateRequest();



  gfl2::heap::HeapBase*     m_pHeap;
  gfl2::gfx::IGLAllocator*  m_pGLAllocator;
  Field::Effect::Manager*   m_pEffectManager;
  Test::FieldTest::MyRenderingPipeLine*      m_pRenderingPipeLine;

  fld::mmodel::MoveModel*           m_pMoveModel;
  Field::Effect::FieldEffectBase*   m_pEffect[ EFFECT_BUFFER_MAX ];

  gfl2::math::Vector3   m_oldPos;
};

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )

#endif // FIELD_EFFECT_FOOT_MANAGER_H_INCLUDED
