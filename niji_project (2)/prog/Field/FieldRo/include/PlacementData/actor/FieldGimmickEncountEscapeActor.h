//======================================================================
/**
 * @file FieldGimmickEncountEscapeActor.h
 * @date 2015/08/19 19:03:22
 * @author saita_kazuki
 * @brief ギミックエンカウント逃走型アクター
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_GIMMICK_ENCOUNT_ESCAPE_ACTOR_H_INCLUDED__
#define __FIELD_GIMMICK_ENCOUNT_ESCAPE_ACTOR_H_INCLUDED__
#pragma once

#include "Field/FieldRo/include/PlacementData/actor/FieldMoveModelActorBase.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldGimmickEncountActorBase.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeGimmickEncountEscape.h"

// 前方参照
namespace Field {

  class TrafficPath;

  namespace GimmickEncount {
    class Manager;
    class LocalWork;
    struct EscapeData;
  }
}

GFL_NAMESPACE_BEGIN(Field)

/**
 * @class FieldGimmickEncountEscapeActor
 * @brief ギミックエンカウント逃走型アクター
 */
class FieldGimmickEncountEscapeActor : public FieldMoveModelActorBase , public IFieldGimmickEncountActorBase
{
  GFL_FORBID_COPY_AND_ASSIGN(FieldGimmickEncountEscapeActor);

public:

  /**
   *  初期化用構造体
   */
  struct SetupData
  {
    MoveModel::FieldMoveModelManager          *pMoveModelManager;
    BaseCollisionScene                        *pCollisionSceneForGimmickEncount;
    BaseCollisionScene                        *pCollisionSceneForGimmickEncountAction;
    FieldGimmickEncountAccessor               *pAccessor;
    const FieldGimmickEncountAccessor::Data   *pGimmickEncountData;
    gfl2::heap::HeapBase                      *pHeap;
    GimmickEncount::Manager                   *pGimmickEncountManager;
    u16                                       gimmickFlagNo;
    ZoneID                        zoneId;
  };

  /**
   *  リソース生成用構造体
   */
  struct CreateResourceData
  {
    FieldGimmickEncountAccessor  *pAccessor;
    Terrain::TerrainManager      *pTerrainManager;
    MyRenderingPipeLine          *pRenderingPipeLine;
    gfl2::heap::HeapBase         *pHeap;
    System::nijiAllocator        *pAllocator;
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  FieldGimmickEncountEscapeActor( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~FieldGimmickEncountEscapeActor( void );

  /**
   *  @brief  初期化
   */
  void Initialize( SetupData &setupData );
  
  /**
   *  @brief  リソースとコリジョンの設定
   */
  void CreateResource( const CreateResourceData& createResourceData );

  /**
   *  @brief  種別取得
   */
  virtual FieldActorType GetType( void ) const { return FIELD_ACTOR_TYPE_GIMMICK_ENCOUNT_ESCAPE; }
  
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
   *  @brief バニッシュ中か
   *  @retval true バニッシュ中
   *  @retval false それ以外
   */
  bool IsVanish();

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
  void SetupMoveCodeWork();

  /**
   *  @brief  MoveCodeWorkの取得
   */
  MoveModel::FieldMoveCodeGimmickEncountEscape::Work* GetMoveCodeWork();

  /**
   *  @brief  パスの生成
   */
  void CreateTrafficPath( gfl2::heap::HeapBase *pHeap, FieldGimmickEncountAccessor *pAccessor );

  /**
   *  @brief  パスの破棄
   */
  void DeleteTrafficPath();

protected:

  // 専用データ
  const FieldGimmickEncountAccessor::Data   *m_pData;
  BaseCollisionScene                        *m_pCollisionSceneForGimmickEncount;
  BaseCollisionScene                        *m_pCollisionSceneForGimmickEncountAction;
  StaticActor                               *m_pActorForGimmickEncount;
  StaticActor                               *m_pActorForGimmickEncountAction1;
  StaticActor                               *m_pActorForGimmickEncountAction2;
  GimmickEncount::Manager                   *m_pGimmickEncountManager;
  GimmickEncount::LocalWork                 *m_pGimmickEncountLocalWork;
  TrafficPath                               *m_pTrafficPath1;
  TrafficPath                               *m_pTrafficPath2;

};

GFL_NAMESPACE_END(Field)

#endif // __FIELD_GIMMICK_ENCOUNT_ESCAPE_ACTOR_H_INCLUDED__
