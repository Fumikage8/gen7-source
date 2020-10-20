//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldMoveModelAccessor.h
 *	@brief  動作モデル配置情報のアクセサー
 *	@author	tomoya takahashi
 *	@date		2015.05.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELD_PLACEMENT_DATA_MOVEMODEL_ACCESSOR_H__ )
#define __FIELD_PLACEMENT_DATA_MOVEMODEL_ACCESSOR_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>

namespace poke_3d{
  namespace model {
    struct DressUpParam; // 着せ替え用パラメータ
    class DressUpModelResourceManager;  // 着せ替えリソースマネージャ
  }
}

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
class FieldMoveModelAccessor
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( FieldMoveModelAccessor );

public:
//C4200を抑止したい場合
GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止

  enum
  {
    DEFAULT_WAIT_ANIME_IF_NUM = 3,    ///< 待機モーション　条件モーションデータ配列要素数
  };

  // 待機モーションンデータ　の　条件モーションデータ
  struct DefaultWaitAnimeDataIf
  {
    u32 motionId;       // モーションID
    u32 flagwork;       // フラグワークID
    u32 flagwork_num;   // ワーク値
  };

  // 待機モーションデータ
  struct DefaultWaitAnimeData
  {
    u32 defaultMotionID;
    u32 defaultMotionIDNum;
    DefaultWaitAnimeDataIf anime_if[DEFAULT_WAIT_ANIME_IF_NUM];
  };

  // 動作モデル配置情報
  struct MoveModelData
  {
    u32 type;             // 識別子
    f32 position[3];      // 座標
    f32 quaternion[4];    // 姿勢
    u32 romVersion;       // ROMバージョン
    u32 flagwork;         // フラグワークID
    u32 flagwork_num;     // ワーク値
    u32 eventID;          // イベントID
    u32 moveCode;         // 動作コード
    u32 characterId;      // キャラクターID
    u32 scriptID;         // スクリプトID
    DefaultWaitAnimeData defaultAnimeData;  // デフォルトアニメーションデータ
    u32 alias;            // エイリアスフラグ
    u32 reversetime_type; // 反転世界挙動 
    u32 talkCollisionOffset;// 話しかけコリジョンへのオフセット
    u32 collisionOffset;    // 衝突コリジョンへのオフセット
  };

  // 動作モデル配置情報ヘッダー (Zone単位）
  struct MoveModelDataHeader
  {
    u32 num;
    MoveModelData data[];
  };

//C4200を抑止したい場合
GFL_WARNING_WIN32_POP


public:

  /**
   *  @biref  コンストラクタ
   */
  FieldMoveModelAccessor();

  /**
   *  @brief  デストラクタ
   */
  virtual ~FieldMoveModelAccessor();

  /**
   *  @brief  初期化
   */
  virtual void Initialize( const void * data );

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
  virtual const MoveModelData * GetData( u32 idx ) const;

  /**
   *  @brief 話しかけコリジョン情報の取得
   */
  const virtual void * GetTalkCollisionData( const MoveModelData & data ) const;

  /**
   *  @brief 衝突用コリジョン情報の取得
   */
  virtual const void * GetCollisionData( const MoveModelData & data ) const;

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

  const MoveModelDataHeader * m_pData;

};

GFL_NAMESPACE_END(Field);

#endif	// __FIELD_PLACEMENT_DATA_MOVEMODEL_ACCESSOR_H__

