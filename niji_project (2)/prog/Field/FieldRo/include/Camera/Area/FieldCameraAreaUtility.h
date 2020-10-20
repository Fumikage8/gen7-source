//======================================================================
/**
* @file FieldCameraAreaUtility.h
* @date 2015/07/03 18:52:30
* @author ikuta_junya
* @brief フィールドカメラエリア関連のUtil関数定義
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if !defined __FIELDCAMERAAREAERUTIL_H_INCLUDED__
#define __FIELDCAMERAAREAERUTIL_H_INCLUDED__
#pragma once


#include <math/include/gfl2_Quaternion.h>
#include <math/include/gfl2_Easing.h>
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaTypes.h"

GFL_NAMESPACE_BEGIN(poke_3d)
GFL_NAMESPACE_BEGIN(model)
class BaseCamera;
GFL_NAMESPACE_END(model)
GFL_NAMESPACE_END(poke_3d)

// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Area );

class Utility
{
public:

  /**
  * @brief カメラパラメーターの補間
  * @note  ターゲットオブジェク制御座標からの相対座標で補間する
  *
  * @param  pOut        結果格納先( r1、r2と別のインスタンスを指定 )
  * @param  r1          0.0地点のパラメータ
  * @param  r2          1.0地点のパラメータ
  * @param  t           現在位置( 0.0f ～ 1.0f )
  *
  * @return 無し
  */
  static void Lerp( MAIN_PARAM* pOut, const MAIN_PARAM& r1, const MAIN_PARAM& r2, f32 t );

  // Vector3の全てのメンバーを度→ラジアン変換
  static void ConvertDegToRad( gfl2::math::Vector3* pOut )
  {
    pOut->x = gfl2::math::ConvDegToRad( pOut->x );
    pOut->y = gfl2::math::ConvDegToRad( pOut->y );
    pOut->z = gfl2::math::ConvDegToRad( pOut->z );
  }

  // カメラパラメータの全ての回転設定を度→ラジアン変換
  static void ConvertDegToRad( MAIN_PARAM* pOut )
  {
    pOut->fovY = gfl2::math::ConvDegToRad( pOut->fovY );
    ConvertDegToRad( &pOut->rotation );
  }

  static void ConvertQuatToEuler( const gfl2::math::Quaternion& rSrc, gfl2::math::Vector3* pOut );

  /**
   * @brief イベントスクリプト用のカメラパラメータの取得
   * @note  アニメーション再生中はTargetPositionが最後に指定された座標になっている
   */
  static void GetEventScriptCameraParameter( const poke_3d::model::BaseCamera* cpCamera, gfl2::math::Vector3 * pCameraTargetPos, gfl2::math::Vector3* pCameraPos, gfl2::math::Vector3* pCameraRotate, f32* pCameraDistance );

#if PM_DEBUG
  // EaseFuncの値から表示テキストを取得
  static const c16* GetEasingFuncLable( gfl2::math::Easing::EaseFunc func );
#endif
};

class ValueController
{
public:

  ValueController()
  {
    m_Value = 1.0f;
    m_EndValue = 1.0f;
    m_NowFrame = 0.0f;
    m_MaxFrame = 0.0f;
    m_WaitFrame = 0;
    m_StepFrame = 1.0f;
  }
  ~ValueController()
  {

  }

  void Set( f32 value )
  {
    m_Value = m_StartValue = m_EndValue = value;
    m_NowFrame = m_MaxFrame = m_WaitFrame = 0;
  }

  void Update( void )
  {
    // 無効
    if( IsValid() == false )
    {
      return;
    }

    // 終了
    if( IsEnd() )
    {
      m_Value = m_EndValue;
      return;
    }

    if( m_WaitFrame > 0 )
    {
      m_WaitFrame--;
    }
    else
    {
      m_NowFrame += m_StepFrame;
    }

    if( m_NowFrame > m_MaxFrame )
    {
      m_NowFrame = m_MaxFrame;
    }

    f32 t = m_NowFrame / m_MaxFrame;

    m_Value = gfl2::math::Easing::GetValue( 
      m_Easing,
      m_StartValue,
      m_EndValue,
      t );
  }

  void Move( f32 start, f32 end, u32 frame, gfl2::math::Easing::EaseFunc easing, u32 waitFrame = 0 )
  {
    m_Value = start;
    m_StartValue = start;
    m_EndValue = end;
    m_MaxFrame = frame;
    m_Easing = easing;
    m_NowFrame = 0.0f;
    m_WaitFrame = waitFrame;
  }

  f32 GetValue( void ) const
  { 
    return m_Value;
  }

  b32 IsEnd( void ) const
  {
    return ( m_NowFrame >= m_MaxFrame );
  }

  // 有効かどうか取得
  b32 IsValid( void ) const
  {
    return ( m_MaxFrame != 0 );
  }

  f32 GetEndValue( void ) const
  {
    return m_EndValue;
  }

  u32 GetFrame( void ) const
  {
    return m_NowFrame;
  }

  void SetStepFrame( f32 stepFrame )
  {
    m_StepFrame = stepFrame;
  }

  // 無効化
  void Clear( void )
  {
    m_Value = 1.0f;
    m_EndValue = 1.0f;
    m_NowFrame = 0.0f;
    m_MaxFrame = 0.0f;
    m_WaitFrame = 0;
  }

  f32 m_NowFrame;
  f32 m_MaxFrame;
  u32 m_WaitFrame;
  f32 m_StartValue;
  f32 m_EndValue;
  gfl2::math::Easing::EaseFunc m_Easing;

  f32 m_Value;
  f32 m_StepFrame;

};

// 補間パラメータ制御クラス
class Interpolation
{
public:
  Interpolation( void ) : 
    m_NowFrame( 0 )
    ,m_EndFrame( 0 )
    ,m_EaseFunc( gfl2::math::Easing::LINEAR )
    ,m_OffsetTargetObjectToCameraTarget( 0.0f, 0.0f, 0.0f )
    ,m_OffsetCameraTargetToCameraPosition( 0.0f, 0.0f, 0.0f )
  {

  }

  ~Interpolation( void )
  {

  }

  // 1フレーム分進める処理
  void Update( void )
  {
    if( m_NowFrame < m_EndFrame )
    {
      m_NowFrame++;
    }
  }

  // 保持データを無効化
  void Clear( void )
  {
    m_NowFrame = m_EndFrame = 0;
  }

  /**
  * @brief 補間設定
  * @note  ターゲットオブジェク制御座標からの相対座標で補間する
  *
  * @param  rTargetObjectPositon    開始時のターゲットオブジェクト制御座標
  * @param  rStartPositon           開始時のカメラ座標
  * @param  rStartTargetPositon     開始時のカメラターゲット座標
  * @param  frame                   補間にかけるフレーム
  *
  * @return 無し
  */
  void SetUp( const gfl2::math::Vector3& rTargetObjectPositon, const gfl2::math::Vector3& rStartPositon, const gfl2::math::Vector3& rStartTargetPositon, u32 frame )
  {
    m_OffsetTargetObjectToCameraTarget = rStartTargetPositon - rTargetObjectPositon;
    m_OffsetCameraTargetToCameraPosition = rStartPositon - rStartTargetPositon;

    m_NowFrame = 0;
    m_EndFrame = frame;
  }

  /**
  * @brief 補間後の座標を取得する
  * @note  ターゲットオブジェク制御座標からの相対座標で補間する
  *
  * @param  rTargetObjectPositon    現在のターゲットオブジェクト制御座標
  * @param  rNowPositon             現在のカメラ座標
  * @param  rNowTargetPositon       現在のカメラターゲット座標
  * @param  pNewPositon             カメラ座標格納先
  * @param  pNewTargetPositon       ターゲット座標格納先
  *
  * @return 無し
  */
  void Get( 
    const gfl2::math::Vector3& rTargetObjectPositon,
    const gfl2::math::Vector3& rNowPositon,
    const gfl2::math::Vector3& rNowTargetPositon,
    gfl2::math::Vector3* pNewPositon,
    gfl2::math::Vector3* pNewTargetPositon
    )
    const
  {
    // そのまま返す
    if( this->IsValid() == false )
    {
      *pNewPositon = rNowPositon;
      *pNewTargetPositon = rNowTargetPositon;
      return;
    }

    gfl2::math::Vector3 workPositon( rNowPositon );
    gfl2::math::Vector3 workTargetPositon( rNowTargetPositon );

    // 0地点を設定
    *pNewTargetPositon = rTargetObjectPositon + m_OffsetTargetObjectToCameraTarget;
    *pNewPositon = *pNewTargetPositon + m_OffsetCameraTargetToCameraPosition;

    f32 t;
    t = ( static_cast<f32>( m_NowFrame ) / m_EndFrame );
    t = gfl2::math::Easing::GetValue( m_EaseFunc, t );

    // 補間
    pNewPositon->Lerp( workPositon, t );
    pNewTargetPositon->Lerp( workTargetPositon, t );
  } 

  // 終了フレームを取得
  u32 GetEndFrame( void ) const
  {
    return m_EndFrame;
  }

  // 補間データが有効か取得
  b32 IsValid( void ) const
  {
    if( m_NowFrame >= m_EndFrame || m_EndFrame == 0 )
    {
      return false;
    }

    return true;
  }

  void SetEaseFunc( gfl2::math::Easing::EaseFunc easeFunc )
  {
    m_EaseFunc = easeFunc;
  }

private:

  u32 m_NowFrame;
  u32 m_EndFrame;
  gfl2::math::Easing::EaseFunc  m_EaseFunc;

  gfl2::math::Vector3 m_OffsetTargetObjectToCameraTarget;     // ターゲット対象座標 -> カメラターゲット座標
  gfl2::math::Vector3 m_OffsetCameraTargetToCameraPosition;   // カメラターゲット座標 -> カメラ座標
};

GFL_NAMESPACE_END( Area );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );


#endif // __FIELDCAMERAAREAERUTIL_H_INCLUDED__