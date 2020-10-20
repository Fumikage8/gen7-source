//======================================================================
/**
 * @file FieldGimmickEncountStandbyActor.h
 * @date 2015/08/24 16:55:44
 * @author saita_kazuki
 * @brief ギミックエンカウント待機型アクター
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_GIMMICK_ENCOUNT_STANDBY_ACTOR_H_INCLUDED__
#define __FIELD_GIMMICK_ENCOUNT_STANDBY_ACTOR_H_INCLUDED__
#pragma once

#include "Field/FieldRo/include/PlacementData/actor/FieldStaticModelActorBase.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldGimmickEncountActorBase.h"

// 前方参照
namespace Field { namespace GimmickEncount {
  class Manager;
  class LocalWork;
  struct StandbyData;
}}


GFL_NAMESPACE_BEGIN(Field)

/**
 * @class FieldGimmickEncountStandbyActor
 * @brief ギミックエンカウント待機型アクター
 */
class FieldGimmickEncountStandbyActor : public FieldStaticModelActorBase , public IFieldGimmickEncountActorBase
{
  GFL_FORBID_COPY_AND_ASSIGN(FieldGimmickEncountStandbyActor);

public:

  /**
   *  ワーク初期化用構造体
   */
  struct SetupWorkData
  {
    const FieldGimmickEncountAccessor::Data*  pGimmickEncountData;
    GimmickEncount::Manager*                  pGimmickEncountManager;
    u16                                       gimmickFlagNo;
    bool                                      isDrawOnly;
  };

  /**
   *  初期化用構造体
   */
  struct SetupData
  {
    gfl2::heap::HeapBase*                     pHeap;
    StaticModel::StaticModelManager*          pStaticModelManager;
    BaseCollisionScene*                       pCollisionSceneForGimmickEncount;
    BaseCollisionScene*                       pCollisionSceneForGimmickEncountAction;
    Field::Terrain::TerrainManager*           pTerrainManager;
    MyRenderingPipeLine*                      pRenderingPipeLine;
    FieldGimmickEncountAccessor*              pAccessor;
    const FieldGimmickEncountAccessor::Data*  pGimmickEncountData;
    System::nijiAllocator*                    pAllocator;
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  FieldGimmickEncountStandbyActor( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~FieldGimmickEncountStandbyActor( void );

  /**
   *  @brief  ワーク初期化
   */
  void InitializeWork( SetupWorkData &setupData );

  /**
   *  @brief  初期化
   */
  void Initialize( SetupData &setupData );

  /**
   *  @brief  種別取得
   */
  virtual FieldActorType GetType( void ) const { return FIELD_ACTOR_TYPE_GIMMICK_ENCOUNT_STANDBY; }
  
  /**
   *  @brief  起動
   */
  virtual bool Kick( const KickRequest &req, bool* p_outIsEventCreate );

  /**
   *  @brief  判定
   */
  virtual bool IsInvalid( void );

  /**
   *  @brief ギミックエンカウント配置情報の取得
   */
  virtual const  FieldGimmickEncountAccessor::Data* GetGimmickEncountData() const;

  /**
   *  @brief ギミックエンカウントマネージャーの取得
   *  @return ギミックエンカウントマネージャー
   */
  virtual GimmickEncount::Manager* GetGimmickEncountManager() const { return m_pGimmickEncountManager; };

  /**
   *  @brief イベント通知
   */
  virtual void Notify( GimmickEncount::NotifyState notify);

  /**
   *  @brief ギミックエンカウントローカルワーク取得
   */
  virtual GimmickEncount::LocalWork* GetLocalWork() const { return m_pGimmickEncountLocalWork; }

  /**
   *  @brief 更新
   */
  virtual void Update();

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

protected:

  // 専用データ
  const FieldGimmickEncountAccessor::Data   *m_pData;
  BaseCollisionScene                        *m_pCollisionSceneForGimmickEncount;
  BaseCollisionScene                        *m_pCollisionSceneForGimmickEncountAction;
  StaticActor                               *m_pActorForGimmickEncount;
  StaticActor                               *m_pActorForGimmickEncountAction;
  GimmickEncount::Manager                   *m_pGimmickEncountManager;
  GimmickEncount::LocalWork                 *m_pGimmickEncountLocalWork;
  bool                                       m_isDrawOnly;
  bool                                       m_isAnimationPlaying;
};

GFL_NAMESPACE_END(Field)

#endif // __FIELD_GIMMICK_ENCOUNT_STANDBY_ACTOR_H_INCLUDED__
