//======================================================================
/**
 * @file FieldEffectKamitsurugiSlash.h
 * @date 2017/02/02 16:35:59
 * @author kawazoe_shinichi
 * @brief カミツルギイベントの斬撃エフェクト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EFFECT_KAMITSURUGI_SLASH_H_INCLUDED__
#define __FIELD_EFFECT_KAMITSURUGI_SLASH_H_INCLUDED__
#pragma once

// gfl2
#include <Effect/include/gfl2_EffectHeap.h>
#include <Effect/include/gfl2_EffectSystem.h>
// Effect
#include "Field/FieldRo/include/Effect/FieldEffectBase.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

/**
*  @class  EffectKamitsurugiSlash
*  @brief  カミツルギイベントの斬撃エフェクト
*/
class EffectKamitsurugiSlash : public IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectKamitsurugiSlash);

public:
  /**
  *  @brief  初期化構造体
  */
  struct SetupData
  {
    void                        *pResource;
    gfl2::Effect::GFHeap        *pEffectHeap;
    gfl2::Effect::System        *pEffectSystem;
    gfl2::math::Vector3         position;
    gfl2::math::Vector3         rotation;
    f32                         fScaleCoeff;
    s32                         resorceID;
  };

public:
  /**
  *  @brief  コンストラクタ
  */
  EffectKamitsurugiSlash(void);

  /**
  *  @brief  デストラクタ
  */
  virtual ~EffectKamitsurugiSlash(void);

  /**
  *  @brief  初期化処理
  */
  void Initialize(SetupData &setupData);

  /**
  *  @brief  破棄
  */
  virtual bool Terminate(void);

  /**
  *  @brief  再生
  */
  void Start(void);

  /**
  *  @brief  停止
  */
  void End(void);

  /**
  *  @brief  再生確認
  *  @retval true  再生中
  *  @retval false そのほか
  */
  bool IsAlive(void);
  virtual bool IsAnimationLastFrame(void){ return !IsAlive(); }

protected:
  /**
  *  @brief  更新処理
  */
  virtual void updateNormal(void);

private:
  gfl2::Effect::GFHeap                *m_pEffectHeap;
  gfl2::Effect::System                *m_pEffectSystem;
  gfl2::Effect::Handle                *m_pEffectHandle;

  gfl2::math::Vector3                 m_vPosition;
  gfl2::math::Vector3                 m_vRotation;
  f32                                 m_fScaleCoeff;

  s32                                 m_sResorceID;

};

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_EFFECT_KAMITSURUGI_SLASH_H_INCLUDED__
