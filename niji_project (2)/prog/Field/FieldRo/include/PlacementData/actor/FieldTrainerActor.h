/**
 *  GAME FREAK inc.
 *
 *  @file   FieldTrainerActor.h
 *  @brief  トレーナー用アクター
 *  @author saita_kazuki
 *  @date   2015.06.04
 */

#if !defined(__FIELD_TRAINER_ACTOR_H__)
#define __FIELD_TRAINER_ACTOR_H__
#pragma once

// Field
#include "Field/FieldRo/include/PlacementData/actor/FieldMoveModelActorBase.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldTrainerAccessor.h"
#include "Field/FieldRo/include/MotionSignal/FieldMotionSignal.h"
#include "Field/FieldRo/include/Traffic/FieldTraffic_define.h"
// conv_header
#include <niji_conv_header/trainer/trid_def.h>

// @debug 視線描画用
// @DrawUtilがあるので未使用にする
#if PM_DEBUG
//#define ENABLE_TEST_EYE_DRAW
#ifdef ENABLE_TEST_EYE_DRAW
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include "System/include/nijiAllocator.h"
#include "System/include/HeapDefine.h"
#include <model/include/gfl2_BaseModel.h>
#endif
#endif

namespace Field
{
  class EventWork;
  class TrafficPath;
}

GFL_NAMESPACE_BEGIN( Field );

class FieldTrainerActor : public FieldMoveModelActorBase
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldTrainerActor );

public:
  /**
   *  初期化用構造体
   */
  struct TrainerSetupData
  {
    MoveModel::FieldMoveModelManager                *pMoveModelManager;
    BaseCollisionScene                              *pCollisionSceneForTerrain;
    BaseCollisionScene                              *pCollisionSceneForStaticModel;
    BaseCollisionScene                              *pCollisionSceneForEventPos;
    BaseCollisionScene                              *pCollisionSceneForEventTalk;
    BaseCollisionScene                              *pCollisionSceneForEventPush;
    BaseCollisionScene                              *pCollisionSceneForTrainerEye;
    FieldTrainerAccessor                            *pAccessor;
    const FieldTrainerAccessor::TrainerData         *pTrainerData;
    ZoneID                                          zoneId;
    ZoneID                                          dataZoneId;
  };

  struct TrainerCreateResourceData
  {
     FieldTrainerAccessor         *pAccessor;
     Terrain::TerrainManager      *pTerrainManager;
     MyRenderingPipeLine          *pRenderingPipeLine;
     const EventWork              *cpEventWork;
     gfl2::heap::HeapBase         *pHeap;
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  FieldTrainerActor( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~FieldTrainerActor( void );

  /**
   *  @brief  MoveModel作成
   */
  void Initialize( TrainerSetupData &setupData );
  
  /**
   *  @brief  リソースとコリジョンの設定
   */
  void CreateResource( TrainerCreateResourceData& createResourceData );

  /**
   *  @brief  種別取得
   */
  virtual FieldActorType GetType( void ) const { return FIELD_ACTOR_TYPE_TRAINER; }
  
  /**
   *  @brief  起動
   */
  virtual bool Kick( const KickRequest &req, bool* p_outIsEventCreate );

  /**
   *  @brief  判定
   */
  virtual bool IsInvalid( void );

  /**
   *  @brief  トレーナーID取得
   */
  trainer::TrainerID GetTrainerID() const ;

  /**
   *  @brief  トレーナー対戦フラグ取得
   */
  u16 GetTrainerFlag() const ;

  /**
   *  @brief  対戦済みかどうか
   *  @retval true 対戦済み
   *  @retval false まだ対戦していない
   */
  bool IsBattleFinished() const ;

  /**
   *  @brief  専用データ取得
   */
  const FieldTrainerAccessor::TrainerData *GetData( void ) const { return m_pData; }

  /**
   *  @brief  動作モデル取得
   */
  MoveModel::FieldMoveModel* GetMoveModel() const { return m_pMoveModel; }

  /**
   *  @brief 動作データの取得
   */
  const FieldTrainerAccessor::MoveData * GetMoveData() const { return m_pMoveData; }

  /**
   *  @brief  ぱすの取得
   */
  Field::TrafficPath *GetTrafficPath( void ){ return m_TrafficPath; }

  /**
   *  @brief  パスの生成
   */
  void CreateTrafficPath( gfl2::heap::HeapBase *pHeap, FieldTrainerAccessor *pAccessor );

  /**
   *  @brief  演出点の数を取得する
   */
  u32 GetPathPointDataNum( void );

  /**
   *  @brief  演出点の取得
   */
  const Field::Traffic::PathPointData *GetPathPointData( u32 idx );

  /**
   *  @brief  所属ゾーン（イベントエディタで配置したゾーン）ＩＤを取得
   *  @return ZoneID
   */
  ZoneID GetDataZoneID( void ){ return m_DataZoneId; }

  /**
  *  @brief  カメラフォーカス停止チェック
  */
  virtual bool IsCameraFocusStop( void ) const { return false; }

  /**
   *  @brief  歩けるかチェック
   */
  bool CheckEnableWalk( void );

protected:

  /**
   *  @brief  追加
   */
  virtual void registActor( void );

  /**
   *  @brief  削除
   */
  virtual void unregistActor( void );

  /**
   *  @brief  破棄共通処理
   *  @retval true 破棄処理を行った
   *  @retval false 破棄処理を行っていない
   */
  virtual bool terminate();

  /**
   *  @brief  MoveCodeWorkに対してこのクラスでの設定を行う
   */
  void setupMoveCodeWork();

protected:

  // 専用データ
  const FieldTrainerAccessor::TrainerData             *m_pData;
  const FieldTrainerAccessor::MoveData                *m_pMoveData;
  MoveModel::FieldMoveModel                           *m_pMoveModel;
  MotionSignal                                        *m_MotionSignal;
  TrafficPath                                         *m_TrafficPath;
  const Traffic::PathPointDataHeader                  *m_pPathPointData;

  // コリジョンシーン
  BaseCollisionScene                                  *m_pCollisionSceneForTerrain;
  BaseCollisionScene                                  *m_pCollisionSceneForStaticModel;
  BaseCollisionScene                                  *m_pCollisionSceneForEventTalk;
  BaseCollisionScene                                  *m_pCollisionSceneForTrainerEye;
  // アクター
  DynamicActor                                        *m_pActorForTerrain;
  StaticActor                                         *m_pActorForStaticModel;
  StaticActor                                         *m_pActorForEventTalk;
  StaticActor                                         *m_pActorForTrainerEye;
  StaticActor                                         *m_pActorForTrainerEyeNew;

  const EventWork                                     *m_cpEventWork;

#ifdef ENABLE_TEST_EYE_DRAW
  System::nijiAllocator*                                      m_pGLAllocator;
  void*                                                       m_pEyeModelBuff;
  poke_3d::model::BaseModel*                                  m_pEyeModel;
  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pEyeModelResourceRootNode;

  void CreateEyeModel();
  void DeleteEyeModel();

public:
  poke_3d::model::BaseModel* GetEyeModel() const { return m_pEyeModel; }
#endif

  ZoneID                                                m_DataZoneId;

}; // class FieldTrainerActor : public IFieldActorBase

GFL_NAMESPACE_END( Field );

#endif // !defined(__FIELD_TRAINER_ACTOR_H__)
