//======================================================================
/**
 * @file FieldEffectBaseParticle.h
 * @date 2016/02/08 19:01:13
 * @author miyachi_soichi
 * @brief エフェクト：パーティクル単体呼出
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EFFECT_BASEPARTICLE_H_INCLUDED__
#define __FIELD_EFFECT_BASEPARTICLE_H_INCLUDED__
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
 *  @class  EffectBaseParticle
 *  @brief  パーティクル単体呼出
 */
class EffectBaseParticle : public IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectBaseParticle);

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
    gfl2::math::Vector3   rotation;
    u32                   eResource;
    u32                   nSe;            //!< 再生SEID
    bool                  bSuicide;       //!< 自殺指定
    // ctor
    SetupData( void )
      : pResource( NULL )
      , pEffectHeap( NULL )
      , pEffectSystem( NULL )
      , position( 0.0f, 0.0f, 0.0f )
      , rotation( 0.0f, 0.0f, 0.0f )
      , nSe( 0 )
    {
      bSuicide = false;
    }
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  EffectBaseParticle( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectBaseParticle( void );

  /**
   *  @brief  初期化処理
   */
  void Initialize( SetupData &setupData );

  /**
   *  @brief  破棄
   */
  virtual bool Terminate( void );

  /**
   *  @brief  終了しているか.
   */
  virtual bool IsAnimationLastFrame( void );

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
  bool                                                            m_bSuicide;
};

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_EFFECT_BASEPARTICLE_H_INCLUDED__
