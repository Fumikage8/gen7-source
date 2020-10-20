//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldEnvSoundCollisionAccessor.h
 *	@brief  コリジョンデータアクセサー
 *	@author	tomoya takahashi
 *	@date		2015.06.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELD_PLACEMENT_DATA_ENVSOUND_COLLISION_ACCESSOR_H__ )
#define __FIELD_PLACEMENT_DATA_ENVSOUND_COLLISION_ACCESSOR_H__
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
class FieldEnvSoundCollisionAccessor
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( FieldEnvSoundCollisionAccessor );

public:

  // 形状の種類
  enum
  {
    CYLINDER,     /// シリンダー
    SNAKE,        /// スネーク
    SPHERE,       /// スフィア
    PIPE,         /// パイプ
    TYPE_MAX,

    LINE = TYPE_MAX,  /// バイナリには無いが、SNAKEのデータの最小単位のLINE
    PIPE_LINE,        /// バイナリには無いが、PIPEのデータの最小単位のLINE
  };

  // Cilinder
  struct Cylinder
  {
    f32 height;       /// 高さ
    f32 radius;       /// 半径
    f32 center[3];    /// 中心座標
  };


  // Sphere
  struct Sphere
  {
    f32 radius;       /// 半径
    f32 center[3];    /// 中心座標
  };

//C4200を抑止したい場合
GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止

  // SNAKE
  struct Snake
  {
    f32 height;     /// 高さ
    f32 width;      /// 辺の幅
    u32 vertexnum;  /// 頂点数
    f32 vertexList[]; /// 頂点リスト（f32 * 3) * vertexnum
  };

  // PIPE
  struct Pipe
  {
    f32 width;      /// 辺の幅
    u32 vertexnum;  /// 頂点数
    f32 vertexList[]; /// 頂点リスト（f32 * 3) * vertexnum
  };
//C4200を抑止したい場合
GFL_WARNING_WIN32_POP

  // LINE
  struct Line
  {
    f32 height;     /// 高さ
    f32 width;      /// 辺の幅
    f32 startpos[3];   /// 開始座標
    f32 endpos[3];     /// 終了座標
  };

  // PipeLINE
  struct PipeLine
  {
    f32 width;      /// 辺の幅
    f32 startpos[3];   /// 開始座標
    f32 endpos[3];     /// 終了座標
  };

  // コリジョンデータ 1レコード
  struct CollisionData
  {
    u32 type;
    union
    {
      Cylinder cylinder;
      Line     line;
      Sphere   sphere;
      PipeLine pipe_line;
    };
  };

  struct CollisionDataHeader
  {
    u32 type;
    union
    {
      Cylinder cylinder;
      Snake    snake;
      Sphere   sphere;
      Pipe     pipe;
    };
  };


public:

  /**
   *  @biref  コンストラクタ
   */
  FieldEnvSoundCollisionAccessor();

  /**
   *  @brief  デストラクタ
   */
  ~FieldEnvSoundCollisionAccessor();

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


private:

private:

  const CollisionDataHeader * m_pData;


};

GFL_NAMESPACE_END(Field);



#endif	// __FIELD_PLACEMENT_DATA_ENVSOUND_COLLISION_ACCESSOR_H__

