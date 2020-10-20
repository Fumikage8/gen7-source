/**
 *  GAME FREAK inc.
 *
 *  @file   FieldStaticModelActor.h
 *  @brief  マップジャンプ用アクター
 *  @author takahashi tomoya 
 *  @date   2015.05.30
 */

#if !defined(__FIELD_STATIC_MODEL_ACTOR_H__)
#define __FIELD_STATIC_MODEL_ACTOR_H__
#pragma once

// Field
#include "Field/FieldRo/include/PlacementData/actor/FieldStaticModelActorBase.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldStaticModelAccessor.h"

GFL_NAMESPACE_BEGIN( Field );

class FieldStaticModelActor : public FieldStaticModelActorBase
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldStaticModelActor );

public:

  /// @brief 初期化構造体
  struct StaticModelSetupData
  {
    gfl2::heap::HeapBase*                             pHeap;
    StaticModel::StaticModelManager*                  pStaticModelManager;
    BaseCollisionScene*                               pCollisionScene;
    FieldStaticModelAccessor*                         pAccessor;
    const FieldStaticModelAccessor::StaticModelData*  pData;
    gfl2::math::Vector3                               vPos;
    gfl2::math::Quaternion                            Quaternion;
    Field::Terrain::TerrainManager*                   pTerrainManager;
    MyRenderingPipeLine*                              pRenderingPipeLine;
    System::nijiAllocator*                            pAllocator;
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  FieldStaticModelActor( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~FieldStaticModelActor( void );

  /**
   *  @brief  種別取得
   */
  virtual FieldActorType GetType( void ) const { return FIELD_ACTOR_TYPE_STATICMODEL; }
  
  /**
   *  @brief  配置モデルの作成
   */
  void Initialize( StaticModelSetupData& setupData );

  /**
   *  @brief  配置モデルの作成 : コリジョンデータを外部から与えるバージョン
   */
  void Initialize(StaticModelSetupData& setupData, const void* pCollisionData);

  /**
   *  @brief  配置モデルの動的作成
   */
  void InitializeForDynamic( StaticModelSetupData& setupData );

  /**
   *  @brief  起動
   */
  virtual bool Kick( const KickRequest &req, bool* p_outIsEventCreate );

  /**
   *  @brief  判定
   */
  virtual bool IsInvalid( void );

  /**
   *  @brief  専用データ取得
   */
  const FieldStaticModelAccessor::StaticModelData *GetData( void ) const { return m_pData; }

  /**
   *  @brief  イベントID取得
   */
  virtual u32 GetDataEventID() const { return m_pData->eventID; }

  /**
   *  @brief  データポジション取得
   */
  virtual gfl2::math::Vector3 GetDataPosition() const {
    return gfl2::math::Vector3(m_pData->position[0],m_pData->position[1],m_pData->position[2]);
  }

  /**
   *  @brief  データクォータニオン取得
   */
  virtual gfl2::math::Quaternion GetDataQuaternion() const {
    return gfl2::math::Quaternion(m_pData->quaternion[0],m_pData->quaternion[1],m_pData->quaternion[2],m_pData->quaternion[3]);
  }

  /**
   *  @brief  フラグワークの値セット
   */
  virtual bool SetEventFlagWork( u16 value );

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
   *  @brief  初期化
   */
  void initialize( StaticModel::StaticModel* pStaticModel, StaticModelSetupData& setupData, const void* pCollisionData );

  /**
   *  @brief  静的配置モデル生成
   */
  StaticModel::StaticModel* createStaticModel( StaticModelSetupData& setupData);

  /**
   *  @brief  動的配置モデル生成
   */
  StaticModel::StaticModel* createStaticModelDynamic( StaticModelSetupData& setupData);

protected:

  // 専用データ
  const FieldStaticModelAccessor::StaticModelData *m_pData;

}; // class FieldStaticModelActor : public IFieldActorBase

GFL_NAMESPACE_END( Field );

#endif // !defined(__FIELD_STATIC_MODEL_ACTOR_H__)
