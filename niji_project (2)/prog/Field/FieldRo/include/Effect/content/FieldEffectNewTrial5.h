//======================================================================
/**
 * @file FieldEffectNewTrial5.h
 * @date 2016/12/13 21:38:34
 * @author kawazoe_shinichi
 * @brief 電気試練用エフェクト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EFFECT_NEW_TRIAL_5_H_INCLUDED__
#define __FIELD_EFFECT_NEW_TRIAL_5_H_INCLUDED__
#pragma once

// gfl2
#include <Effect/include/gfl2_EffectHeap.h>
#include <Effect/include/gfl2_EffectSystem.h>
// Effect
#include "Field/FieldRo/include/Effect/FieldEffectBase.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

/**
*  @class  EffectNewTrial5
*  @brief  電気試練用エフェクト
*/
class EffectNewTrial5 : public IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectNewTrial5);

public:
  /**
  *  @brief  初期化構造体
  */
  struct SetupData
  {
    void                        *pResource;
    gfl2::Effect::GFHeap        *pEffectHeap;
    gfl2::Effect::System        *pEffectSystem;
    Field::IField3DObjectNode   *pParentObject;
    gfl2::math::Vector3         position;
  };

public:
  /**
  *  @brief  コンストラクタ
  */
  EffectNewTrial5(void);

  /**
  *  @brief  デストラクタ
  */
  virtual ~EffectNewTrial5(void);

  /**
  *  @brief  初期化処理
  */
  void Initialize(SetupData &setupData);

  /**
  *  @brief  破棄
  */
  virtual bool Terminate(void);

  /**
  *  @brief  エフェクトを再生する
  */
  void CreateEffect(s32 idx);

  /**
  *  @brief  再生
  */
  void Start(void);

  /**
  *  @brief  停止
  */
  void End(void);

  /**
  *  @brief  停止
  */
  void Fade(void);

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
  virtual void setPosition( const gfl2::math::Vector3& rPos, f32 scale = 1.0f );
private:
  gfl2::Effect::GFHeap                                            *m_pEffectHeap;
  gfl2::Effect::System                                            *m_pEffectSystem;
  gfl2::Effect::Handle                                            *m_pEffectHandle;
  Field::IField3DObjectNode                                       *m_pParentObject;
  gfl2::math::Vector3                                             m_vPosition;


  s32                                                             m_EffectResorceID;
  s32                                                             m_Idx;

};

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_EFFECT_NEW_TRIAL_5_H_INCLUDED__
