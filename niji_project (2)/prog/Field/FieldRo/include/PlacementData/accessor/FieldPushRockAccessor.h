//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldPushRockAccessor.h
 *	@brief  動作モデル配置情報のアクセサー
 *	@author	tomoya takahashi
 *	@date		2015.05.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELD_PLACEMENT_DATA_PUSHROCK_ACCESSOR_H__ )
#define __FIELD_PLACEMENT_DATA_PUSHROCK_ACCESSOR_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>

#include "Field/FieldRo/include/PlacementData/accessor/FieldMoveModelAccessor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldCollisionAccessor.h"

#include "Field/FieldStatic/include/Zone/ZoneDataLoaderTypes.h"

namespace Field
{
  class EventWork;
}


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
class FieldPushRockAccessor
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( FieldPushRockAccessor );

public:
//C4200を抑止したい場合
GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止

  // 岩データ
  struct RockData
  {
    u32 x;
    u32 z;
    u32 uniq_id;
  };

  // 穴データ
  struct HoleData
  {
    u32 x;
    u32 z;
    u32 uniq_id;
  };

  // 動作モデル配置情報
  struct PushRockData
  {
    u32 charaID;          // キャラクターID
    f32 position[3];      // 座標
    f32 rot_y;            // Y回転
    u32 xnum;
    u32 znum;
    u32 width;
    u32 rock_num;
    u32 hole_num;

    u32 rock_data_offset;
    u32 hole_data_offset;
    u32 point_data_offset;
  };

  // 動作モデル配置情報ヘッダー (Zone単位）
  struct PushRockDataHeader
  {
    u32 num;
    PushRockData data[];
  };

//C4200を抑止したい場合
GFL_WARNING_WIN32_POP

  static const u32 sc_ROCK_COLLISION_R = 150;
  static const u32 sc_ROCK_COLLISION_H = 150;
  static const u32 sc_ROCK_COLLISION_TALK_ADD_R = 20;

  #include "niji_conv_header/field/PushRockDef.h" // PushRockDef

public:

  /**
   *  @biref  コンストラクタ
   */
  FieldPushRockAccessor();

  /**
   *  @brief  デストラクタ
   */
  ~FieldPushRockAccessor();

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
  const PushRockData * GetData( u32 idx ) const;

  /**
   *  @brief 岩データの取得
   */
  const RockData& GetRockData( const PushRockData & data, u32 idx ) const;

  /**
   *  @brief 穴データの取得
   */
  const HoleData& GetHoleData( const PushRockData & data, u32 idx ) const;

  // 穴のコリジョンデータ生成
  void CreateHoleCollisionData( const PushRockData & data, u32 idx, BaseActor* p_actor ) const;

  
  /**
   *  @brief GRIDタイプ
   */
  PushRockDef GetGridType( const PushRockData & data, u32 x, u32 z ) const;

  // @fix GFMMCat[156]:一部のフィールドでカイリキ岩が動かなくなっている 対処→momijiで追加された領域外チェック処理をフェローチェマップのゾーンだけで動くように修正 ↓この関数への変更はZoneIDを引数に追加
  /**
   *  @brief 押し岩の格子のローカルグリッド座標
   */
  static bool GetPushRockLocalGrid(const PushRockData& data, const gfl2::math::Vector3& world_position, u32 * out_x, u32 * out_z, ZoneID zoneID);

  /**
   *  @brief 押し岩のマトリクスを取得
   */
  static void GetPushRockMatrix( const PushRockData & data, gfl2::math::Matrix34 * p_out );

  /**
   *  @brief 押し岩のローカル座標を取得
   */
  static b32 GetPushRockLocalPosition( const PushRockData & data, u32 x, u32 z, gfl2::math::Vector3 * p_out );

  /**
   *  @brief 押し岩の姿勢情報取得
   */
  static void GetPushRockQuaternion( const PushRockData & data, gfl2::math::Quaternion * p_out );

private:

  const PushRockDataHeader * m_pData;

};


//-----------------------------------------------------------------------------
/**
 *  @brief 岩の動作モデル用アクセサー
*/
//-----------------------------------------------------------------------------
class FieldPushRockMoveModelAccessor : public FieldMoveModelAccessor
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( FieldPushRockMoveModelAccessor );

public:
  /**
   *  @biref  コンストラクタ
   */
  FieldPushRockMoveModelAccessor();

  /**
   *  @brief  デストラクタ
   */
  virtual ~FieldPushRockMoveModelAccessor();

  /**
   *  @brief  初期化
   */
  virtual void Initialize( const void * data ); // RockDataを入れる。
  virtual void Setup( const FieldPushRockAccessor::PushRockData& pushRockHeader );

  /**
   *  @brief  最大数
   */
  virtual u32 GetDataMax() const;

  /**
   *  @brief エイリアス　データ数
   */
  virtual u32 GetAliasDataMax() const;

  /**
   *  @brief エイリアス以外のデータ数
   */
  virtual u32 GetNotAliasDataMax() const;

  /**
   *  @brief 情報取得
   */
  virtual const FieldMoveModelAccessor::MoveModelData * GetData( u32 idx ) const;

  /**
   *  @brief 話しかけコリジョン情報の取得
   */
  const virtual void * GetTalkCollisionData( const FieldMoveModelAccessor::MoveModelData & data ) const;

  /**
   *  @brief 衝突用コリジョン情報の取得
   */
  virtual const void * GetCollisionData( const FieldMoveModelAccessor::MoveModelData & data ) const;

  /**
   *  @brief デフォルトのアイドルモーション取得
   */
  virtual u32 GetDefaultWaitMotionId( const EventWork& eventWork, const DefaultWaitAnimeData& animeData ) const;

  /**
   *  @brief 着せ替え用パラメータの取得
   *
   *  @note  動作モデルに着せ替えを適用させる場合に使用する。現状ジョインフェスタしかないはず。 ichiraku
   */
  virtual poke_3d::model::DressUpParam * GetDressupParam() const
  {
    return NULL;
  }

  /**
   *  @brief 着せ替え用リソースマネージャの取得
   *
   *  @note  動作モデルに着せ替えを適用させる場合に使用する。現状ジョインフェスタしかないはず。 ichiraku
   */
  virtual poke_3d::model::DressUpModelResourceManager * GetDressUpModelResourceManager() const
  {
    return NULL;
  }

private:

  // DummyCollision
  // CollisionDataHeaderのメモリ
  struct DummyCollision
  {
    u32 num;
    u32 type;
    FieldCollisionAccessor::Cylinder cylinder;
  };

  struct DummyCollisionBox
  {
    u32 num;
    u32 type;
    FieldCollisionAccessor::Box box;
  };

  const FieldPushRockAccessor::RockData * m_pRockData;

  const FieldPushRockAccessor::PushRockData * m_pPushRockHeader;

  // 動作モデルデータ
  FieldMoveModelAccessor::MoveModelData  m_MoveModelData;

  // 衝突コリジョンデータ
  DummyCollisionBox      m_CollisionHeader; // 全部共通なのでStatic
  static const DummyCollisionBox      m_DefCollisionHeader; // 全部共通なのでStatic

  // 話しかけコリジョンデータ
  DummyCollisionBox      m_TalkCollisionHeader; // 全部共通なのでStatic
  static const DummyCollisionBox      m_DefTalkCollisionHeader; // 全部共通なのでStatic
};



GFL_NAMESPACE_END(Field);

#endif	// __FIELD_PLACEMENT_DATA_PUSHROCK_ACCESSOR_H__

