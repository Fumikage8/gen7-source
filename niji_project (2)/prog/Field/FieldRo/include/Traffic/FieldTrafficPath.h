/**
 *  GAME Freak inc.
 *
 *  @file   FieldTrafficPath.h
 *  @brief  往来パス
 *  @author miyachi_soichi
 *  @date   2015/06/25
 */

#if !defined (__FIELD_TRAFFIC_PATH_H__)
#define __FIELD_TRAFFIC_PATH_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_PathController.h>
#include <heap/include/gfl2_Heap.h>

// Field
#include "Field/FieldRo/include/Traffic/FieldTraffic_define.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(MoveModel)
// 前方宣言
class FieldMoveModelManager;
GFL_NAMESPACE_END(MoveModel)
class TrafficActor;

class TrafficPath
{
  //! コピー禁止
  GFL_FORBID_COPY_AND_ASSIGN( TrafficPath );

  enum
  {
    TRAFFIC_VALUE_MAX = 15 + 1,
  };
public:
  enum
  {
    FLAG_CLEAR = 0,

    FLAG_INITIALIZED  = 0x01 <<  0,   //!< 初期化済
    FLAG_USED         = 0x01 <<  1,   //!< 使用中
    FLAG_REVERSE      = 0x01 <<  2,   //!< 逆流
    FLAG_CURVE        = 0x01 <<  3,   //!< 曲線
    FLAG_LOOP         = 0x01 <<  4,   //!< 循環
    FLAG_CORNER       = 0x01 <<  5,   //!< 直線専用：曲がり角フラグ
    FLAG_LANDING      = 0x01 <<  6,   //!< 接地フラグ
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  TrafficPath( void );

  /**
   *  @brief  デストラクタ
   */
  ~TrafficPath( void );

  /**
   *  @brief  初期化
   *  @param  heap    ヒープ
   */
  void Initialize( gfl2::heap::HeapBase *pHeap );

  /**
   *  @brief  破棄
   */
  void Terminate( void );

  /**
   *  @brief  パスの生成
   *  @param  pCoreData   パス生成に必要なデータ
   */
  void CreatePath( const Traffic::PathCoreData *pCoreData );

  /**
   *  @brief  パスの破棄
   */
  void DeletePath( void );

  /**
   *  @brief  使用開始
   */
  void StartUse( void );

  /**
   *  @brief  使用終了
   */
  void EndUse( void );
  
  /**
   *  @brief  進捗させる
   *  @param  spd   移動速度
   */
  f32 Progress( f32 spd );

  /**
   *  @brief  状態の確認
   */
  inline bool CheckFlags( u32 flag ){ return ( m_Flags & flag ) != 0; }

  /**
   *  @brief  フラグ設定
   */
  void SetFlags( u32 flag, bool state );

  /**
   *  @brief  初期位置の取得
   */
  gfl2::math::Vector3 GetStartPosition( void );

  /**
   *  @brief  初期方向の取得
   */
  gfl2::math::Vector3 GetStartRotation( void );
  
  /**
   *  @brief  現在位置の取得
   */
  gfl2::math::Vector3 GetCurrentPosition( void );

  /**
   *  @brief  現在方向の取得
   */
  gfl2::math::Vector3 GetCurrentRotation( void );

  /**
   *  @brief  進捗を取得
   */
  f32 GetProgress( void ){ return m_fProgress; }

  /**
   *  @brief  進捗を設定
   */
  void SetProgress( f32 progress ){ m_fProgress = progress; }

private:
  /**
   *  @brief  進捗：直線
   */
  f32 progressForStraight( f32 spd );
  /**
   *  @brief  長さの取得
   */
  f32 getLengthForStraight( void );
  /**
   *  @brief  指定位置の取得
   */
  gfl2::math::Vector3 getPositionForStraight( f32 ratio );
  /**
   *  @brief  指定方向の取得
   */
  gfl2::math::Vector3 getRotationForStraight( f32 ratio );

  /**
   *  @brief  進捗：曲線
   */
  f32 progressForCurve( f32 spd );
  /**
   *  @brief  長さの取得
   */
  f32 getLengthForCurve( void );
  /**
   *  @brief  指定位置の取得
   */
  gfl2::math::Vector3 getPositionForCurve( f32 ratio );
  /**
   *  @brief  指定方向の取得
   */
  gfl2::math::Vector3 getRotationForCurve( f32 ratio );

#if PM_DEBUG
public:
  u32 debug_getPosNum( void ){ return m_nValueNum; }
  gfl2::math::Vector3 debug_getPos( u32 idx ){ return m_vValueList[idx]; }

#endif
private:
  gfl2::math::PathController        m_PathControl;
  gfl2::heap::HeapBase              *m_pHeap;

  gfl2::math::Vector3               m_vValueList[ TRAFFIC_VALUE_MAX ];
  u32                               m_nValueNum;
  f32                               m_fLength;
  f32                               m_fProgress;
  u32                               m_Flags;

}; // class TrafficPath

GFL_NAMESPACE_END(Field)

#endif // !defined (__FIELD_TRAFFIC_PATH_H__)
