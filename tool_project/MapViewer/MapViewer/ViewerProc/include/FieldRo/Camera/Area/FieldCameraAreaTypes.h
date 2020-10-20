//======================================================================
/**
* @file FieldCameraAreaTypes.h
* @date 2015/07/03 18:52:30
* @author ikuta_junya
* @brief フィールドカメラエリア関連の定義
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if !defined __FIELDCAMERAAREAERTYPES_H_INCLUDED__
#define __FIELDCAMERAAREAERTYPES_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Math.h>
#include <math/include/gfl2_Easing.h>

// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Area );

// @brief フィールドカメラエリアのタイプ定義
struct Type
{
  enum Value
  {
    FOLLOW = 0,             // 通常追従型
    FOLLOW_IPO,             // 2つのパラメータ間を遷移する追従型
    
    // 以下、未実装
    HOLD,
    INHERIT,
    PATH,
    MAX,
    INVALID = 255,
  };
};


// 
struct MAIN_PARAM
{
  gfl2::math::Vector3 offset;
  gfl2::math::Vector3 rotation;
  f32                 fovY;
  f32                 distance;
};

struct AreaDataNo
{
  enum Value
  {
    DEFAULT = 0xFFFF,       // デフォルト設定データ 
    INVALID = 0xFFFE,       // 無効設定データ 
  };
};

// @todo 適切な位置に移動【9月ROMまで】
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

  void Update( void )
  {
    if( m_NowFrame < m_EndFrame )
    {
      m_NowFrame++;
    }
  }

  void Clear( void )
  {
    m_NowFrame = m_EndFrame = 0;
  }

  void SetUp( const gfl2::math::Vector3& rTargetObjectPositon, const gfl2::math::Vector3& rStartPositon, const gfl2::math::Vector3& rStartTargetPositon, u32 frame )
  {
    m_OffsetTargetObjectToCameraTarget = rStartTargetPositon - rTargetObjectPositon;
    m_OffsetCameraTargetToCameraPosition = rStartPositon - rStartTargetPositon;

    m_NowFrame = 0;
    m_EndFrame = frame;
  }

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

  u32 GetEndFrame( void ) const
  {
    return m_EndFrame;
  }

  b32 IsValid( void ) const
  {
    if( m_NowFrame >= m_EndFrame || m_EndFrame == 0 )
    {
      return false;
    }

    return true;
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


#endif // __FIELDCAMERAAREAERTYPES_H_INCLUDED__