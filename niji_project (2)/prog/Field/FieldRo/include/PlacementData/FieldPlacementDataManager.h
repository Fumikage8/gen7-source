//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldPlacementDataManager.h
 *	@brief  配置データマネージャー
 *	@author	tomoya takahashi
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELD_PLACEMENT_DATA_MANAGER_H__ )
#define __FIELD_PLACEMENT_DATA_MANAGER_H__ 
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <heap/include/gfl2_heap.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>
#include <ro/include/gfl2_RoManager.h>

// GameSys
#include  "GameSys/include/GameData.h"

// Field
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include "Field/FieldStatic/include/FieldWorld.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldPlacementDataTypes.h"
#include "Field/FieldRo/include/PlacementData/PlacementMapJumpData.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainManager.h"
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountDefine.h"

// 各種アクセサー
#include "Field/FieldRo/include/PlacementData/accessor/FieldActionEventAccessor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldBgEventAccessor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldPosEventAccessor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldMoveModelAccessor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldContactPokemonAccessor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldCollisionAccessor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldMapJumpAccessor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldStaticModelAccessor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldEncountAccessor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldGimmickEncountAccessor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldItemAccessor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldTrainerAccessor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldPokeFinderAccessor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldBerryPointAccessor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldPushRockAccessor.h"


#include "Field/FieldStatic/include/Item/FieldItemManager.h"

namespace Savedata{
class FishingSpot;
}

GFL_NAMESPACE_BEGIN(Field);



//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          前方宣言
*/
//-----------------------------------------------------------------------------
class IFieldActorBase;
class IFieldGimmickEncountActorBase;
class FieldBerryPointActor;
class FieldFishingSpotActor;
class FieldPokeFinderActor;

class FieldBgEventActor;
class FieldMoveModelActor;
class FieldTrainerActor;

class FieldResident;
class FieldBerryManager;


namespace StaticModel{
  class StaticModelManager;
}// namespace StaticModel
namespace Terrain
{
  class TerrainManager;
}

class MyRenderingPipeLine;


class EnvSoundManager;

namespace GimmickEncount {
  class Manager;
}


/**
 *  @brief イベントID型
 */
typedef u32 EventID;

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
class PlacementDataManager
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( PlacementDataManager );

public:

  // Fieldmap生成前に設定するパラメータ
  struct Desc
  {
    BaseCollisionScene* pCollisionSceneForTerrain;
    BaseCollisionScene* pCollisionSceneForStaticModel;          /// 
    BaseCollisionScene* pCollisionSceneForEventPos;             /// 上に乗ったら反応
    BaseCollisionScene* pCollisionSceneForEventIntrPos;         /// 上に乗ったら反応
    BaseCollisionScene* pCollisionSceneForEventTalk;            /// 話しかけで反応
    BaseCollisionScene* pCollisionSceneForEventPush;            /// プッシュで反応
    BaseCollisionScene* pCollisionSceneForEncount;              /// エンカウント
    BaseCollisionScene* pCollisionSceneForTrainerEye;           /// トレーナー視線
    BaseCollisionScene* pCollisionSceneForGimmickEncount;       /// ギミックエンカウントのエンカウント
    BaseCollisionScene* pCollisionSceneForGimmickEncountAction; /// ギミックエンカウントの動作実行
    BaseCollisionScene* pCollisionSceneForEventFinder;             /// ファインダー
    BaseCollisionScene* pCollisionSceneForFishingPos;           /// 釣り警戒範囲用

    gfl2::fs::BinLinkerAccessor* pPlacementData;
    World*                       pWorld;
    Area*                        pArea;
    MoveModel::FieldMoveModelManager* pMoveModelManager;
    StaticModel::StaticModelManager* pStaticModelManager;
    EnvSoundManager*             pEnvSoundManager;
    GimmickEncount::Manager*          pGimmickEncountManager;

    const Field::EventWork* cpEventWork;

    const void *  cpMapJumpParam; /// マップジャンプパラメータ
    u32           mapJumpParamSize; /// マップジャンプパラメータサイズ

    FieldResident* pFieldResident;  /// フィールドレジデント
    FieldItemManager* pItemManager; /// アイテムマネージャー
    FieldBerryManager* pBerryManager; /// ベリーマネージャー
    Savedata::FishingSpot* pFishingSpot;  /// フィッシングセーブデータ

    gfl2::heap::HeapBase * pHeap;
  };

  // フィールドマップ初期化時に設定するパラメータ
  struct DescFieldmap
  {
    Terrain::TerrainManager*        pTerrainManager;
    MyRenderingPipeLine*            pRenderingPipeLine;
    FieldItemManager*               pItemManager;
  };

  enum
  {
    // ゾーンごとに配置しているデータ
    PACK_IDX_POS,             ///< ポジション配置情報
    PACK_IDX_MOVEMODEL,       ///< 動作配置情報
    PACK_IDX_MAPJUMP,         ///< マップジャンプ配置情報
    PACK_IDX_BG,              ///< BG配置情報
    PACK_IDX_STATICMODEL,     ///< 配置モデル配置情報
    PACK_IDX_ACTION,          ///< アクションイベント配置情報
    PACK_IDX_ENCOUNT,         ///< エンカウント配置情報
    PACK_IDX_TRAINER,         ///< トレーナー配置情報
    PACK_IDX_GIMMICK_ENCOUNT, ///< ギミックエンカウント配置情報
    PACK_IDX_TRAFFIC,         ///< 往来配置情報
    PACK_IDX_PLACEMENT_ITEM,  ///< アイテム配置情報
    PACK_IDX_BERRY_POINT,     ///< きのみ配置情報
    PACK_IDX_FISHING_SPOT,    ///< 釣りスポット配置情報
    PACK_IDX_FINDER_SPOT,     ///< ファインダースポット配置情報
    PACK_IDX_CONTACT_POKEMON, ///< ポケモンふれあい表現用配置情報

    PACK_IDX_PUSHROCK,        ///< 岩押し情報

    // エリアごとに配置しているデータ
    PACK_IDX_ENVSOUND,        ///< 環境音配置情報

    PACK_IDX_MAX,
  };

  enum
  {
    POS_EVENT_CONTANER_MAX = 32,            ///< POSイベントContainer　ワークサイズ
    ACTION_EVENT_MAX = 16,                  ///< アクションイベントサイズ
    MAPJUMP_EVENT_ACTOR_MAX = 24,           ///< マップジャンプアクターサイズ
    MOVEMODEL_EVENT_ACTOR_MAX = 32,         ///< 動作モデルアクターサイズ
    BG_EVENT_ACTOR_MAX = 28,                ///< BGイベントアクターサイズ
    STATICMODEL_ACTOR_MAX = 24,             ///< 配置モデルアクターサイズ
    ENCOUNT_ACTOR_MAX = 16,                 ///< エンカウントアクターサイズ
    TRAINER_ACTOR_MAX = 16,                 ///< トレーナーアクターサイズ
    GIMMICK_ENCOUNT_ACTOR_MAX = 16,         ///< ギミックエンカウントアクターサイズ
    ITEM_ACTOR_MAX = 16,
    POKEFINDER_MAX = 3,                     ///< ポケファインダーアクターサイズ
    BERRY_POINT_ACTOR_MAX = 4,              ///< きのみイベントアクターサイズ
    FISHING_SPOT_ACTOR_MAX = 6,             ///< 釣りスポットアクターサイズ
    PUSHROCK_ACTOR_MAX = 10,                 ///< ゾーンに配置できる押し岩の数
    PUSHROCK_HOLE_MAX = 10,                 ///< ゾーンに配置できる押し岩が落ちる穴の数
  };

  enum ReverseTimeType
  {
     RTIME_TYPE_ALL,      ///< 常に表示
     RTIME_TYPE_REVERSE,  ///< 時間反転世界でのみ表示
     //RTIME_TYPE_NORMAL, ///< 通常世界でのみ表示 この値は、使用しない。
     RTIME_TYPE_MAX,      ///< タイプ最大値
  };

  // エイリアス識別用コード
  static const u16 ALIAS_CODE = ZONEID_ERROR - 1;

public:

  // コンストラクタ
  PlacementDataManager();

  // デストラクタ
  ~PlacementDataManager();

  // 初期化
  void Initialize( const Desc&  desc );
  void InitializeFieldmap( const DescFieldmap& desc );

  // 破棄
  void Terminate();
  void TerminateFieldmap();


  // flagworkに関連する　全イベントのRegist
  void RegistAllEvent( u32 flagwork );
  // flagworkに関連する 全イベントを対象にUnregist
  void UnRegistAllEvent( u32 flagwork );

  // POSイベントの生成
  void CreateZonePos( ZoneID zoneID );
  // POSイベントの破棄
  void DeleteZonePos();
  // POSイベントの追加
  void RegistZonePos( u32 flagwork );
  // POSイベントの削除
  void UnRegistZonePos( u32 flagwork );

  // MAPJUMPイベントの生成
  void CreateZoneMapJump( ZoneID zoneID );
  // MAPJUMPイベントの破棄
  void DeleteZoneMapJump();
  // MAPJUMPイベントの追加
  void RegistZoneMapJump( u32 flagwork );
  void RegistZoneEventIDMapJump( EventID eventId );
  // MAPJUMPイベントの削除
  void UnRegistZoneMapJump( u32 flagwork );
  void UnRegistZoneEventIDMapJump( EventID eventId );
  //MAPJUMPイベントのサーチ
  IFieldActorBase* SearchZoneMapJump( u32 eventId );

  // アクションイベントの生成
  void CreateZoneActionEvent( ZoneID zoneID );
  // アクションイベントの破棄
  void DeleteZoneActionEvent();
  // アクションイベントの追加
  void RegistZoneActionEvent( u32 flagwork );
  void RegistZoneEventIDActionEvent( EventID eventId );
  // アクションイベントの削除
  void UnRegistZoneActionEvent( u32 flagwork );
  void UnRegistZoneEventIDActionEvent( EventID eventId );
  void UnRegistZoneActionEventForJoinFesta( u32 flagwork );
  //アクションイベントのサーチ
  IFieldActorBase* SearchZoneActionEvent( u32 flagWork );

  // 動作モデルの生成：ワーク作成
  void CreateZoneModelWork( ZoneID zoneID );
  // 動作モデルの生成：リソースとコリジョンを生成
  void CreateZoneModelResource( ZoneID zoneID );
  // 動作モデルの破棄
  void DeleteZoneModel();
  // 動作モデルの生成：リソースとコリジョンを破棄
  void DeleteZoneModelResource();
  // 動作モデルの追加
  void RegistZoneModel( u32 flagwork );
  s32 RegistZoneEventIDModel( EventID eventId, Field::EventWork* pEventWork, b32 isVanishUpdate = true );
  // 動作モデルの削除
  void UnRegistZoneModel( u32 flagwork );
  s32 UnRegistZoneEventIDModel( EventID eventId, Field::EventWork* pEventWork, b32 isVanishUpdate = true );
  // 動作モデルのサーチ
  FieldMoveModelActor* SearchZoneEventIDModel( u32 eventId );
  // 動作モデルアクターの登録があるか？ Regist/Unregistステータスはチェックしない
  bool IsEntryZoneEventIDModel( EventID eventId );

  // 動作モデル　ゾーンの物を全てUnRegistする。
  void UnRegistZoneModel();

  // 配置モデルの生成
  void CreateZoneStaticModel( ZoneID zoneID );
  // 配置モデルの破棄リクエスト
   void RequestDeleteZoneStaticModel();
  // 配置モデルの破棄
   void DeleteZoneStaticModel();
  // 配置モデルの追加
   void RegistZoneStaticModel( u32 flagwork );
   void RegistZoneEventIDStaticModel( EventID eventId );
  // 配置モデルの削除
   void UnRegistZoneStaticModel( u32 flagwork );
   void UnRegistZoneEventIDStaticModel( EventID eventId );
  // 配置モデルのサーチ
   IFieldActorBase* SearchZoneEventIDStaticModel( EventID eventId );

  // Bgイベントの生成
   void CreateZoneBgEvent( ZoneID zoneID );
  // Bgイベントの破棄
   void DeleteZoneBgEvent();
  // Bgイベントの追加
   void RegistZoneBgEvent( u32 flagwork );
   void RegistZoneEventIDBgEvent( EventID eventId );
  // Bgイベントの削除
   void UnRegistZoneBgEvent( u32 flagwork );
   void UnRegistZoneEventIDBgEvent( EventID eventId );
  // Bgイベントのサーチ
   FieldBgEventActor* SearchZoneBgEvent( EventID eventId );

  // エンカウントの生成
   void CreateZoneEncount( ZoneID zoneID );
  // エンカウントの破棄
   void DeleteZoneEncount();
  // エンカウントの追加
   void RegistZoneEncount();
  // エンカウントの削除
   void UnRegistZoneEncount();

  // トレーナーの生成：ワーク作成
   void CreateZoneTrainerWork( ZoneID zoneID );
  // トレーナーの生成：リソースとコリジョンを生成
   void CreateZoneTrainerResource( ZoneID zoneID );
  // トレーナーの破棄
   void DeleteZoneTrainer();
  // トレーナーの生成：リソースとコリジョンを破棄
   void DeleteZoneTrainerResource();
  // トレーナーの追加
   void RegistZoneTrainer( u32 flagwork );
   s32 RegistZoneEventIDTrainer( EventID eventId );
  // トレーナーの削除
   void UnRegistZoneTrainer( u32 flagwork );
   s32 UnRegistZoneEventIDTrainer( EventID eventId );
  // トレーナー ゾーンの物を全てUnRegistする。
   void UnRegistZoneTrainer();
  //トレーナー のサーチ
  FieldTrainerActor* SearchZoneEventIDTrainer( EventID eventId );
  //トレーナー のサーチ
  FieldTrainerActor* SearchZoneScriptIDTrainer( u32 scriptId );

  // エリアの環境音データ登録
  void CreateAreaEnvSound( gfl2::heap::HeapBase* pHeap );
  void DeleteAreaEnvSound();

  // ギミックエンカウントの生成：ワーク作成
   void CreateZoneGimmickEncount( ZoneID zoneID );
  // ギミックエンカウントの生成：リソースとコリジョンを生成
   void CreateZoneGimmickEncountResource( ZoneID zoneID );
  // ギミックエンカウントの破棄
   void DeleteZoneGimmickEncount();
  // ギミックエンカウントの破棄：リソースとコリジョンを破棄
   void DeleteZoneGimmickEncountResource();
  // ギミックエンカウントの破棄リクエスト
   void RequestDeleteZoneGimmickEncount();
  // ギミックエンカウント ゾーンの物を全てUnRegistする。
   void UnRegistZoneGimmickEncount();
  // ゾーンの特定のイベントIDのギミックエンカウントをRegist
   s32 RegistZoneEventIDGimmickEncount( EventID eventId );
  // ゾーンの特定のイベントIDのギミックエンカウントをUnRegist
   s32 UnRegistZoneEventIDGimmickEncount( EventID eventId );
  // ゾーンの特定のイベントIDのギミックエンカウントを削除
   s32 DeleteZoneEventIDGimmickEncount( EventID eventId );

  // 配置アイテムの生成
   void CreateZoneItem( ZoneID zoneID );
  // 配置アイテムの破棄
   void DeleteZoneItem();
  // 配置アイテムの追加
   void RegistZoneItem( u32 flagwork );
   void RegistZoneEventIDItem( EventID eventID );
  // 配置アイテムの削除
   void UnRegistZoneItem( u32 flagwork );
   void UnRegistZoneEventIDItem( EventID eventID );
  // 配置アイテムのサーチ
  IFieldActorBase* SearchZoneItem( EventID eventId );

  // ドロップアイテムの生成
   void CreateDropItem( void );
  // ドロップアイテムの破棄リクエスト
   void RequestDeleteDropItem();
  // ドロップアイテムの破棄リクエスト
   void RequestDeleteDropItem( EventID eventID );
  // ドロップアイテムの破棄
   void DeleteDropItem();
   void DeleteDropItem( EventID eventID );
  // ドロップアイテムの追加
   bool RegistDropItem( u32 itemID, u32 num, gfl2::math::Vector3 pos, FieldItemManager::Type type );
  // ドロップアイテムの削除
   void UnRegistDropItem( EventID eventID );
  
  // ファインダーの生成
   void CreateZonePokeFinder( ZoneID zoneID );
  // ファインダーの破棄
   void DeleteZonePokeFinder( void );
  // ファインダー追加
   void RegistZonePokeFinder( u32 flagwork );
  // ファインダー削除
   void UnRegistZonePokeFinder( u32 flagwork );

   // ファインダーのアクターを取得する
   FieldPokeFinderActor *SearchZonePokeFinder( u32 spotID );
   FieldPokeFinderActor *SearchZonePokeFinderFromIndex( u32 idx );
   FieldPokeFinderActor *SearchZonePokeFinderFromSubjectID( u32 idx );

  // きのみ : ワーク生成
   void CreateZoneBerryPointWork(ZoneID zoneID);
  // きのみ : リソース生成
   void CreateZoneBerryPointResource(ZoneID zoneID);
  // きのみ : ワーク破棄
   void DeleteZoneBerryPointWork();
  // きのみ : リソース破棄
   void DeleteZoneBerryPointResource();
  // きのみ : 破棄リクエスト
   void RequestDeleteZoneBerryPoint();
  // きのみ : アクターの除去(破棄はしない)
   void UnRegisterZoneBerryPoint(u32 beryyPointID);
  // きのみ : アクターの除去(破棄はしない)
   void UnRegisterZoneBerryPoint();

  // きのみ : サーチ
  FieldBerryPointActor* SearchZoneBerryPoint( u32 beryyPointID );
  FieldBerryPointActor* SearchZoneBerryPointFromEventID( EventID eventID );

  // 釣りスポット : ワーク生成
  void CreateZoneFishingSpotWork(ZoneID zoneID);
  // 釣りスポット : リソース生成
  void CreateZoneFishingSpotResource(ZoneID zoneID);
  // 釣りスポット : ワーク破棄
  void DeleteZoneFishingSpotWork();
  // 釣りスポット : リソース破棄
  void DeleteZoneFishingSpotResource();
  // 釣りスポット : 破棄リクエスト
  void RequestDeleteZoneFishingSpot();

  // 釣りスポット : シーンへの登録
  void RegisterZoneFishingSpot(u32 eventID);
  // 釣りスポット : シーンからの登録解除
  void UnregisterZoneFishingSpot();
  // 釣りスポット : シーンからの登録解除
  void UnregisterZoneFishingSpot(u32 eventID);


  // 押す岩を配置
  void CreateZonePushRockWork( ZoneID zoneID );
  // 押す岩を配置
  void CreateZonePushRockResource( ZoneID zoneID );
  // 押す岩を削除
  void DeleteZonePushRockWork( void );
  // 押す岩を削除
  void DeleteZonePushRockResource( void );
  // 押す岩のシーンからの登録解除
  void UnRegistZonePushRock(void);
  // 押す岩のシーンからの登録解除
  void UnRegistZonePushRockHole(u32 holeID);

  /**
   *  @brief 岩のアクターを取得
   *  @param  index 岩のアクター配列の要素数
   */
  FieldMoveModelActor* GetPushRockActor(u32 index){ return reinterpret_cast<FieldMoveModelActor*>(m_pPushRockActor[index]); }

  /**
   *  @brief 岩が穴に落ちたことにする。
   *  @param  pGameData 
   *  @param  holeID      穴ID GetPushRockHoleIDで取得
   *  @param  rock_event_id     岩ID 運んでいる岩MoveModelのEventID
   */
  void UnRegistZonePushRockHoleAndSetSaveWork( GameSys::GameData * pGameData, u32 holeID, u32 rock_event_id );

  /**
   *  @brief 押す岩の格子座標を返す。
   *  @param  world_position  ワールド座標
   *  @param  out_index       格子のインデックス(ゾーン内に複数の格子がある可能性がある）
   *  @param  out_x           X座標
   *  @param  out_z           Z座標
   *
   *  @retval true  格子の領域内
   *  @retval false 格子の領域外
   */
  bool GetPushRockMatrix( ZoneID zoneID, const gfl2::math::Vector3& world_position, u32 * out_index, u32 * out_x, u32* out_z );

  /**
   *  @brief 押す岩の格子グリッドのワールド座標を取得
   *  @param  index       格子のインデックス(ゾーン内に複数の格子がある可能性がある）
   *  @param  x           X座標
   *  @param  z           Z座標
   *  @param  out_position ワールド座標格納先
   *
   *  @retval 格子の範囲内かどうか
   */
  b32 GetPushRockGridWorldPosition( ZoneID zoneID, u32 index, u32 x, u32 z, gfl2::math::Vector3 * out_position );

  /**
   *  @brief 押す岩の格子のタイプを取得
   *  @param  index       格子のインデックス(ゾーン内に複数の格子がある可能性がある）
   *  @param  x           X座標
   *  @param  z           Z座標
   *
   *  @retval 格子のタイプ
   */
  FieldPushRockAccessor::PushRockDef GetPushRockGridType( ZoneID zoneID, u32 index, u32 x, u32 z );

  /**
   *  @brief 押す岩の格子の穴IDを取得
   *  @param  index       格子のインデックス(ゾーン内に複数の格子がある可能性がある）
   *  @param  x           X座標
   *  @param  z           Z座標
   *
   *  @retval true  穴
   *  @retval false 穴以外
   */
  bool GetPushRockHoleID( ZoneID zoneID, u32 index, u32 x, u32 z, u32 * out_holeid );



  // エイリアス　StaticModelの生成
  void CreateAreaStaticModel(void);
  void RequestDeleteAreaStaticModel();
  void DeleteAreaStaticModel(void);

  // エイリアス　動作モデルの生成
  void CreateAreaMoveModelWork(void);
  void DeleteAreaMoveModelWork(void);
  void CreateAreaMoveModelResource(void);
  void DeleteAreaMoveModelResource(void);

  // エイリアス　トレーナーの生成
  void CreateAreaTrainerWork(void);
  void CreateAreaTrainerResource(void);
  void DeleteAreaTrainerWork(void);
  void DeleteAreaTrainerResource(void);



  /**
   *  @brief  アクターが持っているユーザーデータのイベントタイプを返す
 *  @retval FIELD_PLACEMENT_DATA_TYPE_POSEVENT,
            FIELD_PLACEMENT_DATA_TYPE_MVMODEL,
            FIELD_PLACEMENT_DATA_TYPE_BGEVENT,
   */
   static u32 GetActorUserDataEventType( BaseActor* actor );

  /**
   *  @brief  アクターが持っているイベントデータのスクリプトIDを取得
   *  @retval FieldScript::SCRID_NUL  なし
   *  @retval その他　スクリプトID
   */
   static u32 GetScriptID( BaseActor* actor );

  /** 
   *  @brief  特定ゾーンのイベントポインタを取得する。
   *
   *  @param  ZoneID zoneID ゾーンID
   *  @param  pack_idx     何の情報を取得するか 
      PACK_IDX_POS,             ///< ポジション配置情報
      PACK_IDX_MOVEMODEL,       ///< 動作配置情報
      PACK_IDX_MAPJUMP,         ///< マップジャンプ配置情報
      PACK_IDX_BG,              ///< BG配置情報
      PACK_IDX_STATICMODEL,     ///< 配置モデル配置情報
      PACK_IDX_ACTION,          ///< アクションイベント配置情報
      PACK_IDX_ENCOUNT,         ///< エンカウント配置情報
      PACK_IDX_TRAINER,         ///< トレーナー配置情報
      PACK_IDX_ENVSOUND,        ///< 環境音配置情報
      PACK_IDX_GIMMICK_ENCOUNT, ///< ギミックエンカウント配置情報

      @retval NULL  　このゾーンにpack_idxの配置情報がない
      @retval その他　配置データ
   */
   void * GetZonePlacementData( ZoneID zoneID, u32 pack_idx );

  /**
   *  @brief  マップジャンプParamの取得
   */
  inline const PlacementMapJumpData::MapJumpParam* GetMapJumpParam( u32 mapjumpType ) const
  {
    return m_MapJumpEventData.GetMapJumpParam( mapjumpType );
  }

  /**
   *  @brief  トレーナーアクター取得
   *  @param  idx   配列番号
   */
  IFieldActorBase *GetTrainerActor( u32 idx ){ return m_pTrainerActor[ idx ]; }
  const IFieldActorBase *GetTrainerActor( u32 idx ) const { return m_pTrainerActor[ idx ]; }

  /**
  *  @brief  動作モデルアクター取得
  *  @param  idx   配列番号
  */
  IFieldActorBase *GetMoveModelActor( u32 idx ){ return m_pMoveModelActor[ idx ]; }
  const IFieldActorBase *GetMoveModelActor( u32 idx ) const { return m_pMoveModelActor[ idx ]; }


  // イベント生成チェック
  static bool IsEventCreate( const Field::EventWork* cpEventWork, u32 rom_version, u32 flagwork, u32 num );

  /**
   *  @brief  動作モデルアクターのレジスト状態に、イベントデータとの差異がないかチェックする
   *  @note   開発時のチェック用、スクリプト終了時に実行される
   */
  void CheckMoveModelActorResistState( void );


  //
  // データ情報取得
  // 

  /**
  *  @brief 動作モデルアクターのデータ数、データ最大制限数、エイリアスデータ数
  */
  s32 GetMoveModelActorCount( void ) const;
  s32 GetMoveModelActorMax( void ) const { return MOVEMODEL_EVENT_ACTOR_MAX; }
  s32 GetMoveModelActorAlias( void ) const { return m_MoveModelActorAliasNum; }

  /**
  *  @brief 視線トレーナーアクターのデータ数、データ最大制限数、エイリアスデータ数
  */
  s32 GetTrainerActorCount( void ) const;
  s32 GetTrainerActorMax( void ) const { return TRAINER_ACTOR_MAX; }
  s32 GetTrainerActorAlias( void ) const { return m_TrainerActorAliasNum; }

  /**
  *  @brief ギミックエンカウントアクターのデータ数、データ最大制限数
  */
  s32 GetGimmickEncountActorCount( void ) const;
  s32 GetGimmickEncountActorMax( void ) const { return GIMMICK_ENCOUNT_ACTOR_MAX; }

  /**
  *  @brief 主人公がアクションを行った事をふれあいアクターに通知
  *  @note イベント中にふれあいポケモンが喜ぶ動作を行った場合、コール
  */
  void RequestEventPlayerActionForFureaiActor( s32 type );


private:

  // イベント生成チェック
   bool isEventCreate( u32 rom_version, u32 flagwork, u32 num, ReverseTimeType reversetime_type );

  // アクションタイプからコリジョンシーンを取得する
   BaseCollisionScene *getCollisionSceneFromActionType( u32 actionType );

  // ギミックエンカウントのアクターを生成する
   void createGimmickEncountActor(
    u32 idx,
    GimmickEncount::GimmickType gimmickType,
    u16 gimmickFlagNo,
    FieldGimmickEncountAccessor* pGimmickEncountAccessor,
    const FieldGimmickEncountAccessor::Data* pData,
    bool isDrawOnly,
    ZoneID zoneId
    );
  // ギミックエンカウントのアクターのリソース、コリジョンを生成する
   bool createGimmickEncountActorResource(
    u32 idx,
    FieldGimmickEncountAccessor* pGimmickEncountAccessor,
    const FieldGimmickEncountAccessor::Data* pData
    );
  // ギミックエンカウントのアクターを取得
   IFieldActorBase* GetGimmickEncountActor( u32 idx);


#if PM_DEBUG
  static void debugCheckPlacementSymbol();
#endif

  /** 
   *  @brief  indexのイベントポインタを取得する。
   *
   *  @param  index インデックス
   *  @param  pack_idx     何の情報を取得するか 
      PACK_IDX_POS,             ///< ポジション配置情報
      PACK_IDX_MOVEMODEL,       ///< 動作配置情報
      PACK_IDX_MAPJUMP,         ///< マップジャンプ配置情報
      PACK_IDX_BG,              ///< BG配置情報
      PACK_IDX_STATICMODEL,     ///< 配置モデル配置情報
      PACK_IDX_ACTION,          ///< アクションイベント配置情報
      PACK_IDX_ENCOUNT,         ///< エンカウント配置情報
      PACK_IDX_TRAINER,         ///< トレーナー配置情報
      PACK_IDX_ENVSOUND,        ///< 環境音配置情報
      PACK_IDX_GIMMICK_ENCOUNT, ///< ギミックエンカウント配置情報

      @retval NULL  　このゾーンにpack_idxの配置情報がない
      @retval その他　配置データ
   */
   void * getIndexPlacementData( u32 index, u32 pack_idx );

  /**
   *  @brief StaticModelActorの生成
   */
  FieldStaticModelActor * craeteStaticModelActor( const FieldStaticModelAccessor::StaticModelData* staticModelData, FieldStaticModelAccessor* staticModelAccessor );
  /**
   *  @brief StaticModelActorの破棄リクエスト
   */
  void requestDeleteStaticModels( u32 start_idx, u32 end_idx );
  /**
   *  @brief StaticModelActorの破棄
   */
  void deleteStaticModels( u32 start_idx, u32 end_idx );


  /** 
   *  @brief MoveModelActorの生成
   */
  FieldMoveModelActor* createMoveModelActor(const FieldMoveModelAccessor::MoveModelData* moveModelData, ZoneID zoneId, bool alias, const FieldContactPokemonAccessor::ContactPokemonData* contactPokemonData = NULL);
  /**
   *  @brief MoveModel CreateResource
   */
  void createMoveModelActorResource( FieldMoveModelActor* pActor, FieldMoveModelAccessor* moveModelAccessor );
  /**
   *  @brief MoveModelWorkの破棄
   */
  void deleteMoveModels( u32 start_idx, u32 end_idx );
  /**
   *  @brief MoveModelResourceの破棄
   */
  void deleteMoveModelResources( u32 start_idx, u32 end_idx );


  /** 
   *  @brief TrainerActorの生成
   */
  FieldTrainerActor* createTrainerActor( const FieldTrainerAccessor::TrainerData* trainerData, FieldTrainerAccessor* trainerAccessor, ZoneID zoneId, bool alias );
  /**
   *  @brief Trainer CreateResource
   */
  void createTrainerActorResource( FieldTrainerActor* pActor, FieldTrainerAccessor* trainerAccessor );
  /**
   *  @brief TrainerWorkの破棄
   */
  void deleteTrainers( u32 start_idx, u32 end_idx );
  /**
   *  @brief TrainerResourceの破棄
   */
  void deleteTrainerResources( u32 start_idx, u32 end_idx );

private:
  // ワールド情報
  World* m_pWorld;
  
  // エリア情報
  Area*  m_pArea;

  // コリジョンシーン
  BaseCollisionScene* m_pCollisionSceneForTerrain;
  BaseCollisionScene* m_pCollisionSceneForStaticModel;
  BaseCollisionScene* m_pCollisionSceneForEventPos;
  BaseCollisionScene* m_pCollisionSceneForEventIntrPos;
  BaseCollisionScene* m_pCollisionSceneForEventTalk;
  BaseCollisionScene* m_pCollisionSceneForEventPush;
  BaseCollisionScene* m_pCollisionSceneForEncount;
  BaseCollisionScene* m_pCollisionSceneForTrainerEye;
  BaseCollisionScene* m_pCollisionSceneForGimmickEncount;
  BaseCollisionScene* m_pCollisionSceneForGimmickEncountAction;
  BaseCollisionScene* m_pCollisionSceneForEventFinder;
  BaseCollisionScene* m_pCollisionSceneForFishingPos;

  // 動作モデルマネージャー
  MoveModel::FieldMoveModelManager* m_pMoveModelManager;

  // 配置モデルマネージャー
  StaticModel::StaticModelManager* m_pStaticModelManager;

  // 地形マネージャ
  Terrain::TerrainManager* m_pTerrainManager;

  // レンダリングモジュール
  MyRenderingPipeLine* m_pRenderingPipeLine;

  // 環境音マネージャー
  EnvSoundManager* m_pEnvSoundManager;

  GimmickEncount::Manager* m_pGimmickEncountManager;

  FieldResident* m_pFieldResident;  /// フィールドレジデント
  FieldItemManager* m_pItemManager; /// アイテムマネージャー
  FieldBerryManager* m_pBerryManager; /// ベリーマネージャー
  Savedata::FishingSpot* m_pFishingSpot;  /// フィッシングセーブデータ

  PlacementMapJumpData   m_MapJumpEventData;  ///< マップジャンプイベントのデータ

  // 配置データ
  gfl2::fs::BinLinkerAccessor* m_pPlacementData;
  gfl2::fs::BinLinkerAccessor m_AreaPlacementData[PACK_IDX_MAX];

  // イベントワーク
  const Field::EventWork* m_cpEventWork;

  // アクター用ヒープ
  gfl2::heap::HeapBase  *m_pActorHeap;
  System::nijiAllocator *m_pAllocator;

  // Actor
  IFieldActorBase       *m_pPosActor[ POS_EVENT_CONTANER_MAX ];               ///< Pos
  IFieldActorBase       *m_pMapJumpActor[ MAPJUMP_EVENT_ACTOR_MAX ] ;         ///< マップジャンプ用
  IFieldActorBase       *m_pMoveModelActor[ MOVEMODEL_EVENT_ACTOR_MAX ] ;     ///< ムーブモデル用
  u32                    m_MoveModelActorAliasNum;                            ///< ムーブモデルアクターのエイリアス数
  IFieldActorBase       *m_pActionEventActor[ ACTION_EVENT_MAX ];             ///< アクションイベント用
  IFieldActorBase       *m_pBgEventActor[ BG_EVENT_ACTOR_MAX ];               ///< Bgイベント用
  IFieldActorBase       *m_pStaticModelActor[ STATICMODEL_ACTOR_MAX ];        ///< 配置モデル用
  u32                    m_StaticModelActorAliasNum;                          ///< 配置モデルアクターのエイリアス数
  IFieldActorBase       *m_pEncountActor[ ENCOUNT_ACTOR_MAX ];                ///< エンカウント用
  IFieldActorBase       *m_pTrainerActor[ TRAINER_ACTOR_MAX ];                ///< トレーナー用
  u32                    m_TrainerActorAliasNum;                              ///< トレーナーアクターのエイリアス数
  IFieldActorBase       *m_pGimmickEncountActor[ GIMMICK_ENCOUNT_ACTOR_MAX ]; ///< ギミックエンカウント用
  IFieldGimmickEncountActorBase *m_pGimmickEncountActorBase[ GIMMICK_ENCOUNT_ACTOR_MAX ]; ///< ギミックエンカウント用
  IFieldActorBase       *m_pItemActor[ ITEM_ACTOR_MAX ];                      ///< 配置アイテム用
  IFieldActorBase       *m_pDropItemActor[ FieldItemManager::MAX_DROP_ITEM ]; ///< ドロップアイテム用
  FieldPokeFinderActor  *m_pPokeFinderActor[ POKEFINDER_MAX ];                ///< ポケファインダー用
  FieldBerryPointActor  *m_pBerryPointActor[ BERRY_POINT_ACTOR_MAX ];           ///< きのみ用
  FieldFishingSpotActor *m_pFishingSpotActor[ FISHING_SPOT_ACTOR_MAX ];         ///< 釣りスポット用
  IFieldActorBase       *m_pPushRockActor[ PUSHROCK_ACTOR_MAX ];                ///< 押し岩用
  BaseActor             *m_pPushRockHoleStaticActor[ PUSHROCK_HOLE_MAX ];      ///< 押し岩が落ちる穴のコリジョン
  const FieldPushRockAccessor::HoleData* m_pPushRockHoleData[ PUSHROCK_HOLE_MAX ];    ///< 押し岩のデータ
  FieldPushRockMoveModelAccessor *m_pPushRockMoveModelAccessor[ PUSHROCK_ACTOR_MAX ]; ///< MoveModelDataやコリジョンのデータ保持

#if PM_DEBUG
public:
  //! デバッグ用ファインダー
   void Debug_CreateZonePokeFinder( FieldPokeFinderAccessor::PokeFinderData *pData, FieldCollisionAccessor::CollisionData *pColData );
  //! デバッグ用：StaticModelVisible
  void DebugFunc_ChangeVisibleStaticModel( void );
  //! デバッグ用：MoveModelVisible
  void DebugFunc_ChangeVisibleMoveModel( void );
#endif

};

GFL_NAMESPACE_END(Field);

#endif	// __FIELD_PLACEMENT_DATA_MANAGER_H__ 

