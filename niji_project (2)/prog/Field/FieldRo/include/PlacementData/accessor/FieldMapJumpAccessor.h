//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldMapJumpAccessor.h
 *	@brief  マップジャンプイベントのデータアクセサー
 *	@author	tomoya takahashi
 *	@date		2015.05.25
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELD_PLACEMENT_DATA_MAPJUMP_ACCESSOR_H__ )
#define __FIELD_PLACEMENT_DATA_MAPJUMP_ACCESSOR_H__
#pragma once


// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <ro/include/gfl2_RoManager.h>

#include <math/include/gfl2_math.h>

GFL_NAMESPACE_BEGIN(Field);


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
class FieldMapJumpAccessor
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( FieldMapJumpAccessor );

public:
//C4200を抑止したい場合
GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止


  // 動作モデル配置情報
  struct MapJumpData
  {
    u32 type;             // 識別子
    f32 position[3];      // 座標
    f32 quaternion[4];    // 姿勢
    u32 romVersion;       // ROMバージョン
    u32 flagwork;         // フラグワークID
    u32 flagwork_num;     // ワーク値
    u32 eventID;          // イベントID
    u32 nextEventID;      // 次のイベントID
    u16 zoneID;           // ゾーンID
    u16 nextZoneID;       // 次のゾーンID
    u32 mapJumpType;      // マップ接続タイプ
    f32 jumpPosition[3];  // ジャンプ後出現位置
    u32 inCamera;         // 入るときのカメラ
    u32 outCamera;        // 出るときのカメラ
    u32 focus;            // 注目点
    u32 collisionOffset;  // コリジョンオフセット
  };

  // 動作モデル配置情報ヘッダー (Zone単位）
  struct MapJumpDataHeader
  {
    u32 num;
    MapJumpData data[];
  };

//C4200を抑止したい場合
GFL_WARNING_WIN32_POP


public:

  /**
   *  @biref  コンストラクタ
   */
  GFL_DLL_EXPORT FieldMapJumpAccessor();


  /**
   *  @brief  デストラクタ
   */
  GFL_DLL_EXPORT ~FieldMapJumpAccessor();

  /**
   *  @brief  初期化
   */
  GFL_DLL_EXPORT void Initialize( const void * data );

  /**
   *  @brief  最大数
   */
  GFL_DLL_EXPORT u32 GetDataMax() const;

  /**
   *  @brief 情報取得
   */
  GFL_DLL_EXPORT const MapJumpData * GetData( u32 idx ) const;

  /**
   *  @brief コリジョン情報の取得
   */
  GFL_DLL_EXPORT const void * GetCollisionData( const MapJumpData & data ) const;


  /**
   *  @brief コリジョン情報　ポジションの幅Percentage取得
   */
  f32 GetCollisionCenterWidthPercentage( const MapJumpData & data, const gfl2::math::Vector3& pos ) const;

  /**
   *  @brief コリジョン情報　幅Percentの座標を取得
   */
  void GetCollisionCenterWidthPercentagePosition( const MapJumpData & data, const f32 parcent, const gfl2::math::Vector3& jump_pos, gfl2::math::Vector3* out_pos ) const;


private:

  const MapJumpDataHeader * m_pData;


};

GFL_NAMESPACE_END(Field);

#endif	// __FIELD_PLACEMENT_DATA_MAPJUMP_ACCESSOR_H__

