//======================================================================
/**
 * @file FieldEffectTrial3.h
 * @date 2016/03/16 16:24:38
 * @author miyachi_soichi
 * @brief 試練3専用
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EFFECT_TRIAL3_H_INCLUDED__
#define __FIELD_EFFECT_TRIAL3_H_INCLUDED__
#pragma once

// gfl2
#include <Effect/include/gfl2_EffectHeap.h>
#include <Effect/include/gfl2_EffectSystem.h>
// Effect
#include "Field/FieldRo/include/Effect/FieldEffectBase.h"
#include "Field/FieldRo/include/Effect/FieldEffectLayout.h"

// 前方宣言
GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Camera )
class CameraManager;
GFL_NAMESPACE_END( Camera )
GFL_NAMESPACE_END( Field )
GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

/**
 *  @class  EffectTrial3
 *  @brief  試練3専用
 */
class EffectTrial3 : public IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectTrial3);

public:
  /**
   *  @brief  初期化構造体
   */
  struct SetupData
  {
    void                  *pResource;
    gfl2::Effect::GFHeap  *pEffectHeap;
    gfl2::Effect::System  *pEffectSystem;
    Camera::CameraManager *pCameraManager;
    gfl2::math::Vector3   position;
    // ctor
    SetupData( void )
      : pResource( NULL )
      , pEffectHeap( NULL )
      , pEffectSystem( NULL )
      , pCameraManager( NULL )
      , position( 0.0f, 0.0f, 0.0f )
    {
    }
  };
  enum
  {
    EFF_HDL_MAX = 3, // エフェクト同時発生数
    COOL_TIME = 5,
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  EffectTrial3( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectTrial3( void );

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

  /**
   *  @brief  エフェクトを再生する
   */
  void CreateEffect( s32 idx );

protected:
  /**
   *  @brief  更新処理
   */
  virtual void updateNormal( void );

  /**
   *  @brief  座標更新
   */
  void updatePosition( void );

private:
  gfl2::Effect::GFHeap                                            *m_pEffectHeap;
  gfl2::Effect::System                                            *m_pEffectSystem;
  gfl2::Effect::Handle                                            *m_pEffectHandle[EFF_HDL_MAX];
  Camera::CameraManager                                           *m_pCameraManager;

  gfl2::math::Vector3                                             m_vPosition;
  u32                                                             m_nIndex;
  u32                                                             m_nCoolTime;
};

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_EFFECT_TRIAL3_H_INCLUDED__
