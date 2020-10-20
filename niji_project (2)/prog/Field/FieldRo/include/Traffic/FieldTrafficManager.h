/**
 *  GAME Freak inc.
 *
 *  @file   FieldTrafficManager.h
 *  @brief  往来管理
 *  @author miyachi_soichi
 *  @date   2015/06/25
 */

#if !defined(__FIELD_TRAFFIC_MANAGER_H__)
#define __FIELD_TRAFFIC_MANAGER_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Vector3.h>
#include <util/include/gfl2_List.h>
// field
#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include "Field/FieldRo/include/Traffic/FieldTraffic_define.h"

GFL_NAMESPACE_BEGIN(Field)

// 前方宣言
class PlacementDataManager;
class TrafficActor;
class TrafficPath;
GFL_NAMESPACE_BEGIN(MoveModel)
class FieldMoveModelManager;
GFL_NAMESPACE_END(MoveModel)
GFL_NAMESPACE_BEGIN(weather)
class WeatherControl;
GFL_NAMESPACE_END(weather)

class TrafficManager
{
  //! コピー禁止
  GFL_FORBID_COPY_AND_ASSIGN( TrafficManager );

  enum ReloadType
  {
    RELOAD_TYPE_PRIMARY,
    RELOAD_TYPE_SECONDLY,
    RELOAD_TYPE_NORMAL,
  };
  /**
   *  @brief  リロード用
   */
  struct ReloadData
  {
    u32 path_idx; // パス番号
    f32 cool_time; // 冷却時間
    u32 reload_type; // リロード種別
    bool bNormal;
  };
  typedef gfl2::util::List<ReloadData> ReloadList;

  /**
   *  @brief  抽選用
   */
  struct SelectData
  {
    u32 idx;
    u32 ratio;
    const Traffic::NpcCoreData *data;
  };
  typedef gfl2::util::List<SelectData> SelectList;

public:
  /**
   *  @brief  コンストラクタ
   */
  TrafficManager( void );

  /**
   *  @brief  デストラクタ
   */
  ~TrafficManager( void );

  /**
   *  @brief  初期化
   *  @param  pDeviceHeap         デバイスヒープ
   *  @param  pMoveModelManager   動作モデル管理
   */
  void Initialize( gfl2::heap::HeapBase *pDeviceHeap, Field::MoveModel::FieldMoveModelManager *pMoveModelManager );

  /**
   *  @brief  コリジョンシーンの設定
   *  @param  setup      初期化パラメータ
   */
  void SetupCollisionScene( const Traffic::SetupCollision &setup );

  /**
   *  @brief  天候管理設定
   */
  inline void SetWeatherControl( weather::WeatherControl *pWeather ){ m_pWeatherControl = pWeather; }

  /**
   *  @brief  破棄
   */
  void Terminate( void );

  /**
   *  @brief  更新
   */
  void Update( void );

  /**
   *  @brief  パスを生成する
   *  @param  pFieldHeap  パスの生成メモリはフィールドからもらう
   *  @param  zoneID      生成するゾーン
   */
  void CreateZonePath( gfl2::heap::HeapBase *pFieldHeap, ZoneID zoneID, PlacementDataManager *pPlaceMgr );

  /**
   *  @brief  パスの破棄
   */
  void DeleteZonePath();

  /**
   *  @brief  演者のリソース復帰
   */
  void CreateZoneActorResource();

  /**
   *  @brief  演者のリソース破棄
   */
  void DeleteZoneActorResource();

  /**
   *  @brief  演者の破棄
   */
  void DeleteZoneActor();

  void UnRegistActor();

  /**
   *  @brief  ゾーン変更後のリロード対応
   */
  void Request_ReloadActor( void );

  /**
   *  @brief  パスの取得
   */
  TrafficPath *GetTrafficPath( u32 index ){ return m_pTrafficPath[ index ]; }

  /**
   *  @brief 現状のゾーンIDの取得
   */
  ZoneID GetZoneID( void ) const { return m_ZoneId; }

  /**
   *  @brief  プレイヤーがワープしてくるから解消を予約する
   *  @param  pos   プレイヤーのワープ先の座標
   *  @note   あくまで予約を行うだけなので解消が完了したかどうかは
   *          CheckEscape関数にて行うこと
   */
  void RequestPlayerWarp( const gfl2::math::Vector3 &pos );

  /**
   *  @brief  プレイヤーワープ先の往来が退避完了したか調べる関数
   *  @param  true  退避が完了した＝ワープしていい
   *  @param  false 退避が未完了＝ワープしてはいけない
   */
  bool CheckEscape( void );

  /**
   *  @brief  スクリプトで表示をコントロールする
   *  @param  bDisp   表示状態
   */
  void RequestScriptDisp( bool bDisp );

  /**
  *  @brief  パスの数を取得
  */
  s32 GetPathCount( void ) const;
  s32 GetPathMax( void ) const { return Traffic::FIELD_TRAFFIC_MAX; }

#if PM_DEBUG
  void Debug_CreateActor();
  void Debug_VectorPos( u32 num, gfl2::math::Vector3 *vec_list, bool bCurve );
#endif

private:
  /**
   *  @brief  パスの条件チェック
   */
  bool checkPathCreate( const Traffic::TrafficPathData *data );

  /**
   *  @brief  リロードリストの更新
   */
  void updateReloadList( void );

  /**
   *  @brief  往来NPCの生成
   *  @param  path_idx    パス番号
   *  @return 生成の成否を返却
   */
  bool reloadTrafficNpc( u32 path_idx, bool bRandom );

  /**
   *  @brief  Actorの空きを取得する
   *  @return 未使用のActor配列番号を返却する
   */
  u32 getEmptyActorIndex( void );

  /**
   *  @brief  NPCを選出する
   */
  const Traffic::NpcCoreData *selectNpcCoreData( u32 path_idx );

  /**
   *  @brief  条件チェック
   */
  bool isEvent( const Traffic::NpcCoreData *data );

  /**
   *  @brief  往来NPCが到着したので片付ける
   */
  void endTrafficNpc( u32 actor_idx, u32 path_idx );

  /**
   *  @brief  生成位置をプレイヤーが占拠しているかチェック
   */
  bool isInPlayer( const gfl2::math::Vector3 &vPos );

private:
  gfl2::heap::HeapBase              *m_pHeap;               //!< ローカルヒープ
  MoveModel::FieldMoveModelManager  *m_pMoveModelManager;
  weather::WeatherControl           *m_pWeatherControl;
  void                              *m_pResource;
  TrafficPath                       *m_pTrafficPath[ Traffic::FIELD_TRAFFIC_MAX ];
  TrafficActor                      *m_pTrafficActor[ Traffic::FIELD_TRAFFIC_MAX ];
  ReloadList                        m_ReloadList;
  bool                              m_IsInZone;
  ZoneID                            m_ZoneId; //!<　ゾーンID

}; // class TrafficManager

GFL_NAMESPACE_END(Field)

#endif // !defined(__FIELD_TRAFFIC_MANAGER_H__)
