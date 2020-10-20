//======================================================================
/**
 * @file FieldEffectTrial5.h
 * @date 2016/03/17 15:50:26
 * @author miyachi_soichi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EFFECT_TRIAL5_H_INCLUDED__
#define __FIELD_EFFECT_TRIAL5_H_INCLUDED__
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
 *  @class  EffectTrial5
 *  @brief  試練5専用
 */
class EffectTrial5 : public IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectTrial5);

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

public:
  /**
   *  @brief  コンストラクタ
   */
  EffectTrial5( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectTrial5( void );

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
  gfl2::Effect::Handle                                            *m_pEffectHandle;
  Camera::CameraManager                                           *m_pCameraManager;

  gfl2::math::Vector3                                             m_vPosition;
};

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_EFFECT_TRIAL5_H_INCLUDED__
