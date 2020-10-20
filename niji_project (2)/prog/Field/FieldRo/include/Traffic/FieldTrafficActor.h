/**
 *  GAME FREAK inc.
 *
 *  @file   FieldTrafficActor.h
 *  @brief  往来専用NPCアクタークラス
 *  @author miyachi_soichi
 *  @date   2015/06/29
 */

#if !defined(__FIELD_TRAFFIC_ACTOR_H__)
#define __FIELD_TRAFFIC_ACTOR_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>

// Field
#include "Field/FieldRo/include/PlacementData/actor/FieldActorBase.h"
#include "Field/FieldRo/include/Traffic/FieldTraffic_define.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"
#include "Field/FieldRo/include/Collision/BaseCollisionScene.h"
#include "Field/FieldRo/include/Collision/StaticActor.h"

GFL_NAMESPACE_BEGIN(Field)
// 前方宣言
class TrafficPath;
class TrafficManager;
GFL_NAMESPACE_BEGIN(MoveModel)
class FieldMoveModelManager;
GFL_NAMESPACE_END(MoveModel)

class TrafficActor : public IFieldActorBase
{
  //! コピー禁止
  GFL_FORBID_COPY_AND_ASSIGN( TrafficActor );

public:
  /**
   *  @brief  コンストラクタ
   */
  TrafficActor( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~TrafficActor( void );

  /**
   *  @brief  初期化
   *  @param  pMoveModelManager   動作モデル管理
   */
  void Initialize( Field::MoveModel::FieldMoveModelManager *pMoveModelManager, Field::TrafficManager *pTrafficManager );

  /**
   *  @brief  コリジョンシーンの設定
   *  @param  setup      初期化パラメータ
   */
  void SetupCollisionScene( const Traffic::SetupCollision &setup );

  /**
   *  @brief  破棄
   */
  void Terminate( void );

  /**
   *  @brief  演者の作成
   *  @param  path_idx  パス番号
   *  @param  coreData  NPC用データ
   */
  void CreateActorWork( u32 path_idx, const Traffic::NpcCoreData &coreData );

  /**
   *  @brief  演者の作成
   */
  void CreateActorResource();

  /**
   *  @brief  演者の破棄
   */
  void DeleteActor( void );

  /**
   *  @brief  リソースの解放
   */
  void DeleteActorResource( void );

  void UnRegistActor( void );

  /**
   *  @brief  種別取得
   */
  virtual FieldActorType GetType( void ) const { return FIELD_ACTOR_TYPE_TRAFFIC; }

  /**
   *  @brief  起動
   *  @param  req パラメータ
   *  @param  p_outIsEventCreate  GameEventを生成したかどうか？ true :起動 false :起動していない
   *  @retval 何かしら処理を実行したか
   */
  virtual bool Kick( const KickRequest &req, bool * p_outIsEventCreate );

  /**
   *  @brief  判定
   */
  virtual bool IsInvalid( void );

  /**
   *  @brief  終了
   */
  void EndTraffic( void );

  //! エモート予約があるか
  inline bool IsEmote( void ) const { return m_bEmote; }
  //! エモート終了通知
  inline void NotifyEndEmote( void ){ m_bEmote = false; m_pTalker = NULL; }
  //! 往来挙動終了判定
  inline bool IsEndTraffic( void ) const { return m_bEndTraffic; }
  //! 往来挙動終了通知
  inline void NotifyEndTraffic( void ){ m_bEndTraffic = true; }
  //! 話者取得
  inline MoveModel::FieldMoveModel *GetTalker( void ){ return m_pTalker; }
  //! 待機ID取得
  inline u32 GetStayID( void ) const { return m_nStayID; }
  //! 移動ID取得
  inline u32 GetWalkID( void ) const { return m_nWalkID; }
  //! 左旋回ID取得
  inline u32 GetTurnLID( void ) const { return m_nTurnLID; }
  //! 右旋回ID取得
  inline u32 GetTurnRID( void ) const { return m_nTurnRID; }
  //! エモートID取得
  inline u32 GetEmoteID( void ) const { return m_nEmoteID; }
  //! プログレスの取得
  inline f32 GetProgress( void ) const { return m_fProgress; }
  //! プログレスの設定
  inline void SetProgress( f32 progress ) { m_fProgress = progress; }
  //! 使用中
  inline bool IsUsed( void ) const { return m_ModelIndex != Field::MoveModel::FIELD_MOVE_MODEL_MAX; }

  /**
   *  @brief  歩けるかチェック
   */
  bool CheckEnableWalk( f32 size, f32 length );

  /**
   *  @brief  パスを取得する
   */
  TrafficPath *GetPath( void );

  /**
   *  @brief  パス番号を取得する
   */
  inline u32 GetPathIdx( void ) const { return m_PathID; }

  /**
   *  @brief  往来キャラクタを取得する
   */
  MoveModel::FieldMoveModel *GetMoveModel( void );

  /**
   *  @brief  レジュム
   */
  bool IsResume( void ){ return m_bResume; }
  void notifyResume( void ){ m_bResume = false; }

  /**
   *  @brief  退避命令
   */
  void RequestEscape( void );
  bool GetEscapeFlag( void ){ return m_bEscape; }

  /**
   *  @brief  スクリプトからの表示操作
   *  @param  bDisp   表示状態
   */
  void RequestScriptDisp( bool bDisp );

private:
  MoveModel::FieldMoveModelManager  *m_pMoveModelManager;
  TrafficManager                    *m_pTrafficManager;
  BaseCollisionScene                *m_pCollisionSceneForTerrain;
  BaseCollisionScene                *m_pCollisionSceneForStaticModel;
  BaseCollisionScene                *m_pCollisionSceneForTrafficModel;
  BaseCollisionScene                *m_pCollisionSceneForEventPos;
  BaseCollisionScene                *m_pCollisionSceneForEventTalk;
  BaseCollisionScene                *m_pCollisionSceneForEventPush;
  BaseCollisionScene                *m_pCollisionSceneForGround;
  StaticActor                       *m_pActorForTrafficModel;
  StaticActor                       *m_pActorForEventTalk;

  MoveModel::FieldMoveModel              *m_pTalker;
  MoveModel::FieldMoveModelHeaderWork     m_MoveModelHeaderWork;
  MoveModel::FieldMoveModelHeaderResource m_MoveModelHeaderResource;

  u32                               m_ModelIndex;
  u32                               m_ScriptID;
  u32                               m_PathID;

  u32                               m_nStayID;
  u32                               m_nWalkID;
  u32                               m_nTurnLID;
  u32                               m_nTurnRID;
  u32                               m_nEmoteID;
  u32                               m_nCharacterID;
  f32                               m_fProgress;    //!< 復帰用進捗退避

  bool                              m_bEmote;
  bool                              m_bEndTraffic;  //!< 往来挙動が終了した

  bool                              m_bResume;
  bool                              m_bEscape;

}; // class TrafficActor : public IFieldActorBase

GFL_NAMESPACE_END(Field)
#endif // !defined(__FIELD_TRAFFIC_ACTOR_H__)