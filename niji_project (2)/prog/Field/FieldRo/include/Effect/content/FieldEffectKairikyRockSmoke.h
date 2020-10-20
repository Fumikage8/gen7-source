//======================================================================
/**
 * @file FieldEffectKairikyRockSmoke.h
 * @date 2015/12/09 13:04:53
 * @author miyachi_soichi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EFFECT_KAIRIKY_ROCKSMOKE_H_INCLUDED__
#define __FIELD_EFFECT_KAIRIKY_ROCKSMOKE_H_INCLUDED__
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
 *  @class  EffectKairikyRockSmoke
 *  @brief  カイリキ岩押し
 */
class EffectKairikyRockSmoke : public IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectKairikyRockSmoke);

public:
  /**
   *  @brief  初期化構造体
   */
  struct SetupData
  {
    void                  *pResource;
    gfl2::Effect::GFHeap  *pEffectHeap;
    gfl2::Effect::System  *pEffectSystem;
    Field::IField3DObjectNode   *pParentObject;
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  EffectKairikyRockSmoke( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectKairikyRockSmoke( void );

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
  void EndKill( void );

  /**
   *  @brief  停止(フェード)
   */
  void EndFade( void );

  /**
   *  @brief  再生確認
   *  @retval true  再生中
   *  @retval false そのほか
   */
  bool IsAlive( void );

  /**
   *  @brief  オフセットクォータニオンを設定
   */
  void SetQuaternion( gfl2::math::Quaternion quaternion ){ m_quaternion = quaternion; }

protected:
  /**
   *  @brief  更新処理
   */
  virtual void updateNormal( void );

private:
  gfl2::Effect::GFHeap                                            *m_pEffectHeap;
  gfl2::Effect::System                                            *m_pEffectSystem;
  gfl2::Effect::Handle                                            *m_pEffectHandle;
  poke_3d::model::BaseModel                                       *m_pParent;
  gfl2::math::Quaternion                                           m_quaternion;
};

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_EFFECT_KAIRIKY_ROCKSMOKE_H_INCLUDED__
