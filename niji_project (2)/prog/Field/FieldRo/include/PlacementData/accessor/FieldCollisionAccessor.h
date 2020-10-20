//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldCollisionAccessor.h
 *	@brief  コリジョンデータアクセサー
 *	@author	tomoya takahashi
 *	@date		2015.05.22
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELD_PLACEMENT_DATA_COLLISION_ACCESSOR_H__ )
#define __FIELD_PLACEMENT_DATA_COLLISION_ACCESSOR_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>


//niji
#include "Field/FieldRo/include/Collision/BaseActor.h"

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
/**
 * @class コリジョンデータアクセサー
 */
class FieldCollisionAccessor
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( FieldCollisionAccessor );

public:

  // 形状の種類
  enum
  {
    CYLINDER,
    BOX,
    LINE,
    TRIANGLE,
    TYPE_MAX,
  };

  // Cilinder
  struct Cylinder
  {
    f32 center[3];    /// 中心座標
    f32 radius;       /// 半径
    f32 height;       /// 高さ
  };

  // Box
  struct Box
  {
    f32 center[3];        /// 中心座標
    f32 quaternion[4];    /// 姿勢
    f32 scale[3];         /// 大きさ
  };

  // Line
  struct Line
  {
    f32 start[3];     /// 開始座標
    f32 end[3];       /// 終了座標
    f32 height;       /// 高さ
  };

  // TRIANGLE
  struct Triangle
  {
    f32 pos0[3];     /// 頂点0
    f32 pos1[3];     /// 頂点1
    f32 pos2[3];     /// 頂点2
  };

  struct CollisionData
  {
    u32 type;
    union
    {
      Cylinder cylinder;
      Box      box;
      Line     line;
      Triangle triangle;
    };
  };

//C4200を抑止したい場合
GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止

  // 動作モデル配置情報ヘッダー (Zone単位）
  struct CollisionDataHeader
  {
    u32 num;
    u32 type[];
  };

//C4200を抑止したい場合
GFL_WARNING_WIN32_POP


public:

  /**
   *  @biref  コンストラクタ
   */
  FieldCollisionAccessor();

  /**
   *  @brief  デストラクタ
   */
  ~FieldCollisionAccessor();

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
  bool GetData( u32 idx, CollisionData* p_out ) const;


  /**
   *  @brief  BaseActorのCreate処理を実行
   */
  static void CreateActor( BaseActor* actor, const CollisionData& data, const gfl2::math::Vector3& parent_pos );


  /**
   *  @brief コリジョン情報からパラメータ取得
   *  @note サポートしているのは、BOXとLINEのみ
   */
  static void GetParameter( const CollisionData& data, gfl2::math::Vector3* out_center_pos, gfl2::math::Vector3* out_x, f32 * out_width );

private:
  u32 getDataType(u32 idx) const;
  uptr getDataAddr( u32 idx ) const;

private:

  const CollisionDataHeader * m_pData;


};

GFL_NAMESPACE_END(Field);



#endif	// __FIELD_PLACEMENT_DATA_COLLISION_ACCESSOR_H__

