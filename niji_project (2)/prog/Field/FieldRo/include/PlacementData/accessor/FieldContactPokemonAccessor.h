//======================================================================
/**
* @file FieldContactPokemonAccessor.h
* @date 2017/02/09 17:54:39
* @author kawazoe_shinichi
* @brief ふれあいポケモン用のアクセサー
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================
#if !defined( __FIELD_PLACEMENT_DATA_CONTACTPOKEMON_ACCESSOR_H__ )
#define __FIELD_PLACEMENT_DATA_CONTACTPOKEMON_ACCESSOR_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>

#include "Field/FieldRo/include/PlacementData/accessor/FieldMoveModelAccessor.h"

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
class FieldContactPokemonAccessor : public FieldMoveModelAccessor
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN(FieldContactPokemonAccessor);

public:
//C4200を抑止したい場合
GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止

  // ▼ --------- ふれあいポケモン用のデータ --------- ▼
  // ふれあいポケモン配置情報
  struct ContactPokemonData
  {
    u32 npcEventID;             // トレーナーNPCのイベントID
    u32 fureaiNo;               // セーブワーク
    u32 moveCode;               // ふれあい専用動作コード
    u32 moveCollisionOffset;    // 移動領域コリジョンへのオフセット
    u32 reserve1;               // 予備データ1
    u32 reserve2;               // 予備データ2
    u32 is_forceLanding;        // 接地するか？
    u32 reserveCollisionOffset; // 予備領域コリジョンへのオフセット
  };

  // 動作モデルとふれあいポケモンデータが両方存在するデータ
  struct MixedData
  {
    FieldMoveModelAccessor::MoveModelData moveModel;
    ContactPokemonData contactPokemon;

  };

  // ふれあいポケモン情報を含む動作モデル配置情報ヘッダー (Zone単位）
  struct ContactPokemonDataHeader
  {
    u32 num;
    MixedData data[];
  };
  // ▲ --------- ふれあいポケモン用のデータ --------- ▲

//C4200を抑止したい場合
GFL_WARNING_WIN32_POP


public:

  /**
   *  @biref  コンストラクタ
   */
  FieldContactPokemonAccessor();

  /**
   *  @brief  デストラクタ
   */
  virtual ~FieldContactPokemonAccessor();

  /**
   *  @brief  初期化
   */
  virtual void Initialize(const void * data);

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
  *  @brief 動作モデルのみの情報を取得
  */
  virtual const FieldMoveModelAccessor::MoveModelData * GetData(u32 idx) const;

  /**
  *  @brief ふれあい情報のみを取得
  */
  virtual const ContactPokemonData * GetDataForContact(u32 idx) const;

  /**
   *  @brief 話しかけコリジョン情報の取得
   */
  const virtual void * GetTalkCollisionData(const FieldMoveModelAccessor::MoveModelData & data) const;

  /**
   *  @brief 衝突用コリジョン情報の取得
   */
  virtual const void * GetCollisionData(const FieldMoveModelAccessor::MoveModelData & data) const;

  /**
  *  @brief 移動領域用コリジョン情報の取得
  */
  virtual const void * GetMoveAreaCollisionData(const ContactPokemonData & data) const;

  /**
  *  @brief 予備領域用コリジョン情報の取得
  */
  virtual const void * GetReserveAreaCollisionData(const ContactPokemonData & data) const;

  /**
   *  @brief デフォルトのアイドルモーション取得
   */
  virtual u32 GetDefaultWaitMotionId(const EventWork& eventWork, const FieldMoveModelAccessor::DefaultWaitAnimeData& animeData) const;

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

  const ContactPokemonDataHeader *  m_pData;
};

GFL_NAMESPACE_END(Field);

#endif	// __FIELD_PLACEMENT_DATA_CONTACTPOKEMON_ACCESSOR_H__

