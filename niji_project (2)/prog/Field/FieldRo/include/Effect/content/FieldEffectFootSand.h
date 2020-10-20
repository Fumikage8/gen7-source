//======================================================================
/**
 * @file FieldEffectFootSand.h
 * @date 2015/11/13 12:48:16
 * @author miyachi_soichi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EFFECT_FOOTSAND_H_INCLUDED__
#define __FIELD_EFFECT_FOOTSAND_H_INCLUDED__
#pragma once

// gfl2
#include <Effect/include/gfl2_EffectHeap.h>
#include <Effect/include/gfl2_EffectSystem.h>
// Effect
#include "Field/FieldRo/include/Effect/FieldEffectBase.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

class EffectFootSand : public IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectFootSand);

public:
  /**
   *  @brief  初期化構造体
   */
  struct SetupData
  {
    void                        *pResource;
    gfl2::Effect::GFHeap        *pEffectHeap;
    gfl2::Effect::System        *pEffectSystem;
    gfl2::math::Vector3         vPos;
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  EffectFootSand( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectFootSand( void );

  /**
   *  @brief  初期化処理
   */
  void Initialize( SetupData &setupData );

  /**
   *  @brief  破棄
   */
  virtual bool Terminate( void );

  /**
   *  @brief  オフセットを設定
   */
  void SetOffset( gfl2::math::Vector3 offset,gfl2::math::Vector3 offsetQua );

protected:
  /**
   *  @brief  更新処理
   */
  virtual void updateNormal( void );

private:
  gfl2::Effect::GFHeap                                            *m_pEffectHeap;
  gfl2::Effect::System                                            *m_pEffectSystem;
  gfl2::Effect::Handle                                            *m_pEffectHandle;

};

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_EFFECT_FOOTSAND_H_INCLUDED__
