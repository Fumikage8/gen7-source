/**
 *  GAME FREAK inc.
 *
 *  @file   FieldFishingSpotActor.h
 *  @brief  釣りスポットアクター
 *  @author Masanori Kanamaru
 *  @date   2015.10.23
 */

#if !defined(__FIELD_FISHING_SPOT_ACTOR_H__)
#define __FIELD_FISHING_SPOT_ACTOR_H__
#pragma once

#include "Field/FieldRo/include/PlacementData/actor/FieldActorBase.h"

#include "Field/FieldRo/include/PlacementData/accessor/FieldFishingSpotAccessor.h"

#include "Field/FieldRo/include/PlacementData/actor/FieldStaticModelActor.h"

GFL_NAMESPACE_BEGIN( Field )

// 前方宣言

namespace MoveModel
{
  class FieldMoveModelPlayer;
}

class FieldFishingSpotActor : public FieldStaticModelActorBase
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldFishingSpotActor );

public:
  /**
   *  @brief  コンストラクタ
   */
  FieldFishingSpotActor();

  /**
   *  @brief  種別取得
   */
  virtual FieldActorType GetType() const;

  /**
   *  @brief  初期化
   */
  void Initialize(
    u32 actorIndex,
    const FieldFishingSpotAccessor::Data* pData,
    const CollisionSetupData& watchingColSetupData,
    const CollisionSetupData& actionColSetupData,
    System::nijiAllocator* pAllocator,
    gfl2::heap::HeapBase* pHeap
    );

  /**
   *  @brief  破棄
   */
  virtual void Terminate();
  
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
  virtual bool IsInvalid();

  virtual bool IsActionButtonInterestClear() const { return false; }

  /**
   *  @brief  配置モデル用を作成する
   */
  void SetupStaticModel(FieldStaticModelActor::StaticModelSetupData& setupData);

  /**
   *  @brief  スポットデータを取得する
   */
  const FieldFishingSpotAccessor::Data* GetData() const;

  /**
   *  @brief  抽選用データを取得する
   */
  const FieldFishingSpotAccessor::LotData* GetLotData() const;

  /**
   *  @brief  レアがいるか
   */
  bool ExistRare() const;

  /**
   *  @breif  レア判定を消す
   */
  void EscapeRare();

  class Mist
  {
  public:
    enum State{Stay, StartDiluting, Diluting, Ditutied};

    inline void Initialize(FieldFishingSpotActor* pActor, bool visible);
    State GetState() const;
    void StartDilute();
    void Update();

  private:
    State state;

    FieldFishingSpotActor* pActor;

    static const int FadeFrame = 20;
    static const int MaterialAmount = 4;
    static const int ColorIndex    = 1;
  };

  Mist* GetMist(); 

private:
  Mist mist;

  u32 actorIndex;

  StaticCollisionPair watchingArea;
  StaticCollisionPair actionArea;

  const FieldFishingSpotAccessor::Data* pData;

  FieldStaticModelAccessor::StaticModelData staticModelData;
  
  /**
   *  @brief  シーンへの登録
   */
  void registActor();

  /**
   *  @brief  シーンから除去
   */
  void unregistActor();

  /**
   *  @brief  レア逃走判定と実行
   */
  bool KickEscape(const KickRequest &req);

  /**
   *  @brief  釣りイベント開始判定と実行
   */
  bool KickAction(const KickRequest &req);

  /**
   *  @brief  スポットに近づきすぎか
   */
  bool CheckTooClose(MoveModel::FieldMoveModelPlayer* const pPlayer) const;

  /**
   *  @brief  レアに発見されるか
   */
  bool IsDetected(const MoveModel::FieldMoveModelPlayer* pPlayer) const;
};

GFL_NAMESPACE_END( Field )

#endif // !defined(__FIELD_FISHING_SPOT_ACTOR_H__)