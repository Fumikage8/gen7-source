/**
 *  GAME FREAK inc.
 *
 *  @file   FieldBerryPointActor.h
 *  @brief  きのみアクター
 *  @author Masanori Kanamaru
 *  @date   2015.10.07
 */

#if !defined(__FIELD_BERRY_POINT_ACTOR_H__)
#define __FIELD_BERRY_POINT_ACTOR_H__
#pragma once

#include "Field/FieldRo/include/PlacementData/actor/FieldActorBase.h"

#include "Field/FieldStatic/include/Berry/FieldBerryManager.h"

#include "Field/FieldRo/include/PlacementData/accessor/FieldBerryPointAccessor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldStaticModelActor.h"

GFL_NAMESPACE_BEGIN( Field )

class FieldBerryPointActor : public FieldStaticModelActorBase
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldBerryPointActor );

public:

  struct SetupDesc
  {
    System::nijiAllocator* pAllocator;
    gfl2::heap::HeapBase* pHeap;  
    BaseCollisionScene* pTalkCollisionScene;
    BaseCollisionScene* pForbiddenCollisionScene;
  };

  /**
   *  @brief  コンストラクタ
   */
  FieldBerryPointActor();

  /**
   *  @brief  初期化
   */
  void Initialize(const FieldBerryPointAccessor::BerryPointData* pData, const FieldBerryManager* pBerryManager, SetupDesc& desc);

  /**
   *  @brief  破棄
   */
  virtual void Terminate();

  /**
   *  @brief  種別取得
   */
  virtual FieldActorType GetType() const;

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
   *  @brief  きのみポイントIDを返す
   */
  u32 GetBerryPointID() const;

  /**
   *  @brief  イベントIDを返す
   */
  u32 GetEventID() const;

  /**
   *  @brief  フォーカスIDを返す
   */
  u32 GetFocusID() const;

  /**
   *  @brief  姿勢を返す
   */
  void GetRotation(gfl2::math::Quaternion* pOut) const;

  void StartSwing();

  void StopSwing();

private:
  struct CollisionData
  {
    u32 amount;
    u32 type;
    FieldCollisionAccessor::Cylinder data;
  };

  FieldBerryLottedData lottedData;

  FieldStaticModelAccessor::StaticModelData staticModelData;

  u32 eventID;
  u32 focusID;

  StaticCollisionPair talkArea;
  StaticCollisionPair forbiddenArea;

  /**
   *  @brief  シーンへの登録
   */
  void registActor();

  /**
   *  @brief  シーンから除去
   */
  void unregistActor();

  /**
   *  @brief  オフセット計算済みのコリジョンデータを生成する
   */
  void GetFixedCollisionData(const void* src, CollisionData* pOut) const;
};

GFL_NAMESPACE_END( Field )

#endif // !defined(__FIELD_BERRY_POINT_ACTOR_H__)