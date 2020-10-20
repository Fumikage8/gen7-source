//======================================================================
/**
 * @file FieldEffectSharkJet.h
 * @date 2015/09/03 11:21:30
 * @author miyachi_soichi
 * @brief サメハダーのジェット
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EFFECT_SHARKJET_H_INCLUDED__
#define __FIELD_EFFECT_SHARKJET_H_INCLUDED__
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
 *  @class  EffectSharkJet
 *  @brief  サメハダーのジェット
 */
class EffectSharkJet : public IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectSharkJet);

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
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  EffectSharkJet( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectSharkJet( void );

  /**
   *  @brief  初期化処理
   */
  void Initialize( SetupData &setupData );

  /**
   *  @brief  破棄
   */
  virtual bool Terminate( void );

  /**
   *  @brief  各エフェクトで必要な破棄作業
   */
  virtual void Delete( void );

  /**
   *  @brief  エミッター作成
   */
  void CreateEmitter( void );

  /**
   *  @brief  エミッター削除
   */
  void DeleteEmitter( void );

  /**
   *  @brief  座標オフセットの設定
   */
  void setOffsetPosition( gfl2::math::Vector3  offsetPos );

protected:
  /**
   *  @brief  更新処理
   */
  virtual void updateNormal( void );

  /**
   *  @brief  座標取得
   */
  void jointPosition( u32 foots, gfl2::math::Vector3 *pos );

  /**
   *  @brief  座標更新
   */
  void updatePosition( void );

private:
  gfl2::Effect::GFHeap                                            *m_pEffectHeap;
  gfl2::Effect::System                                            *m_pEffectSystem;
  gfl2::Effect::Handle                                            *m_pEffectHandle;
  poke_3d::model::BaseModel                                       *m_pParent;
  gfl2::math::Vector3                                              m_offsetPosition;
};

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_EFFECT_SHARKJET_H_INCLUDED__
