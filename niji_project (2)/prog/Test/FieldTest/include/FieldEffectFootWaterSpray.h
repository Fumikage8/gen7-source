//===================================================================
/**
 * @file    FieldEffectFootWaterSpray.h
 * @brief   フィールドエフェクト足元水しぶきクラス
 * @author  saita_kazuki
 * @date    2015.03.18
 */
//===================================================================

#if !defined( FIELD_EFFECT_FOOT_WATER_SPRAY_H_INCLUDED )
#define FIELD_EFFECT_FOOT_WATER_SPRAY_H_INCLUDED
#pragma once

#include "Test/FieldTest/include/FieldEffectBase.h"


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )


class FieldEffectFootWaterSpray : public FieldEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(FieldEffectFootWaterSpray); //コピーコンストラクタ＋代入禁止

public:

  /**
   * @brief   コンストラクタ
   * @param   pHeap インスタンス生成などに使用するヒープ
   * @param   pGLAllocator リソース確保用アロケータ
   * @param   pEffectManager フィールドエフェクト管理クラス
   */
  FieldEffectFootWaterSpray( gfl2::heap::HeapBase* pHeap, gfl2::gfx::IGLAllocator* pGLAllocator, Field::Effect::Manager* pEffectManager);

  /**
   * @brief   デストラクタ
   */
  virtual ~FieldEffectFootWaterSpray();

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

protected:

  b8    m_isFinish;

};

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )

#endif // FIELD_EFFECT_FOOT_WATER_SPRAY_H_INCLUDED
