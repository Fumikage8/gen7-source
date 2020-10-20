//======================================================================
/**
 * @file FieldEffectRoba2AccelSmoke.h
 * @date 2015/11/13 12:47:58
 * @author miyachi_soichi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EFFECT_ROBA2_ACCELE_SMOKE_H_INCLUDED__
#define __FIELD_EFFECT_ROBA2_ACCELE_SMOKE_H_INCLUDED__
#pragma once

// gfl2
#include <Effect/include/gfl2_EffectHeap.h>
#include <Effect/include/gfl2_EffectSystem.h>
// Effect
#include "Field/FieldRo/include/Effect/FieldEffectBase.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

class EffectRoba2AccelSmoke : public IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectRoba2AccelSmoke);

public:
  /**
   *  @brief  初期化構造体
   */
  struct SetupData
  {
    void                        *pResource;
    gfl2::Effect::GFHeap        *pEffectHeap;
    gfl2::Effect::System        *pEffectSystem;
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  EffectRoba2AccelSmoke( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectRoba2AccelSmoke( void );

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

  /**
   *  @brief  指定位置で再設定
   */
  void Reset( gfl2::math::Vector3 pos, gfl2::math::Vector3 offsetQua );


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

#endif // __FIELD_EFFECT_ROBA2_ACCELE_SMOKE_H_INCLUDED__