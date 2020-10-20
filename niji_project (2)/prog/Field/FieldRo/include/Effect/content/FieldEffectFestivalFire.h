//======================================================================
/**
 * @file FieldEffectFestivalFire.h
 * @date 2015/12/07 14:53:54
 * @author miyachi_soichi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EFFECT_FESTIVALFIRE_H_INCLUDED__
#define __FIELD_EFFECT_FESTIVALFIRE_H_INCLUDED__
#pragma once

// gfl2
#include <Effect/include/gfl2_EffectHeap.h>
#include <Effect/include/gfl2_EffectSystem.h>
// Effect
#include "Field/FieldRo/include/Effect/FieldEffectBase.h"
#include "Field/FieldRo/include/Effect/FieldEffectLayout.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

/**
 *  @class  EffectFestivalFire
 *  @brief  マツリ
 */
class EffectFestivalFire : public IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectFestivalFire);

public:
  /**
   *  @brief  初期化構造体
   */
  struct SetupData
  {
    void                  *pResource;
    gfl2::Effect::GFHeap  *pEffectHeap;
    gfl2::Effect::System  *pEffectSystem;
    gfl2::math::Vector3   position;
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  EffectFestivalFire( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectFestivalFire( void );

  /**
   *  @brief  初期化処理
   */
  void Initialize( SetupData &setupData );

  /**
   *  @brief  破棄
   */
  virtual bool Terminate( void );

  /**
   *  @brief  再生
   */
  void Start( void );

  /**
   *  @brief  停止
   */
  void End( void );

  /**
   *  @brief  再生確認
   *  @retval true  再生中
   *  @retval false そのほか
   */
  bool IsAlive( void );

protected:
  /**
   *  @brief  更新処理
   */
  virtual void updateNormal( void );

private:
  gfl2::Effect::GFHeap                                            *m_pEffectHeap;
  gfl2::Effect::System                                            *m_pEffectSystem;
  gfl2::Effect::Handle                                            *m_pEffectHandle;

  gfl2::math::Vector3                                             m_vPosition;
};

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_EFFECT_FESTIVALFIRE_H_INCLUDED__
