//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldPosEventAccessor.h
 *	@brief  POSイベント配置情報のアクセサー
 *	@author	tomoya takahashi
 *	@date		2015.05.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELD_POSEVENT_ACCESSOR_H__ )
#define __FIELD_POSEVENT_ACCESSOR_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>

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
class FieldPosEventAccessor
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( FieldPosEventAccessor );

public:
//C4200を抑止したい場合
GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止


  // 動作モデル配置情報
  struct PosData
  {
    u32 type;             // 識別子
    f32 position[3];      // 座標
    f32 quaternion[4];    // 姿勢
    u32 romVersion;       // ROMバージョン
    u32 flagwork;         // フラグワークID
    u32 flagwork_num;     // ワーク値
    u32 scriptID;         // スクリプトID
    u32 reversetime_type; // 反転世界挙動 
    u32 intr_pos_flag;    // IntrPosFlag
    u32 collisionOffset;    // 衝突コリジョンへのオフセット
  };

  // 動作モデル配置情報ヘッダー (Zone単位）
  struct PosDataHeader
  {
    u32 num;
    PosData data[];
  };
//C4200を抑止したい場合
GFL_WARNING_WIN32_POP


public:

  /**
   *  @biref  コンストラクタ
   */
  FieldPosEventAccessor();

  /**
   *  @brief  デストラクタ
   */
  ~FieldPosEventAccessor();

  /**
   *  @brief  初期化
   */
  void Initialize( const void * data );
 

  /**
   *  @brief  最大数
   */
  u32 GetDataMax() const;

  /**
   *  @brief 情報取得
   */
  const PosData * GetData( u32 idx ) const;

  /**
   *  @brief 衝突用コリジョン情報の取得
   */
  const void * GetCollisionData( const PosData & data ) const;

private:

  const PosDataHeader * m_pData;

};


GFL_NAMESPACE_END(Field);


#endif	// __FIELD_POSEVENT_ACCESSOR_H__

