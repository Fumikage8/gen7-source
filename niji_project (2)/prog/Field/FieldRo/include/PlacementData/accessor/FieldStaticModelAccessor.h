//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldStaticModelAccessor.h
 *	@brief  STATIC_MODELイベント配置情報のアクセサー
 *	@author	tomoya takahashi
 *	@date		2015.05.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELD_PLACEMENT_DATA_STATIC_MODEL_ACCESSOR_H__ )
#define __FIELD_PLACEMENT_DATA_STATIC_MODEL_ACCESSOR_H__
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
class FieldStaticModelAccessor
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( FieldStaticModelAccessor );

public:
  // 動作モデル配置情報
  struct StaticModelData
  {
    u32 type;             // 識別子
    f32 position[3];      // 座標
    f32 quaternion[4];    // 姿勢
    u32 romVersion;       // ROMバージョン
    u32 flagwork;         // フラグワークID
    u32 flagwork_num;     // ワーク値
    u32 eventID;          // イベントID
    u16 staticModelID;    // 配置モデルID
    u16 alias;            // エイリアス
    u32 collisionOffset;    // 衝突コリジョンへのオフセット
  };

//C4200を抑止したい場合
GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止

  // 動作モデル配置情報ヘッダー (Zone単位）
  struct StaticModelDataHeader
  {
    u32 num;
    StaticModelData data[];
  };
//C4200を抑止したい場合
GFL_WARNING_WIN32_POP


public:

  /**
   *  @biref  コンストラクタ
   */
  FieldStaticModelAccessor();

  /**
   *  @brief  デストラクタ
   */
  ~FieldStaticModelAccessor();

  /**
   *  @brief  初期化
   */
  void Initialize( const void * data );
 

  /**
   *  @brief  最大数
   */
  u32 GetDataMax() const;

  /**
   *  @brief エイリアスのデータ数
   */
  u32 GetAliasDataMax() const;

  /**
   *  @brief エイリアスでないデータ数
   */
  u32 GetNotAliasDataMax() const;

  /**
   *  @brief 情報取得
   */
  const StaticModelData * GetData( u32 idx ) const;

  /**
   *  @brief 衝突用コリジョン情報の取得
   */
  const void * GetCollisionData( const StaticModelData & data ) const;

private:

  const StaticModelDataHeader * m_pData;

};


GFL_NAMESPACE_END(Field);


#endif	// __FIELD_PLACEMENT_DATA_STATIC_MODEL_ACCESSOR_H__

