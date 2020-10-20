/**
 *  GAME FREAK inc.
 *
 *  @file   FieldActorBase.h
 *  @brief  フィールド用アクター規定
 *  @author miyachi_soichi
 *  @date   2015.05.26
 */

#if !defined(__FIELD_ACTOR_BASE_H__)
#define __FIELD_ACTOR_BASE_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <math/include/gfl2_Vector3.h>
#include <heap/include/gfl2_Heap.h>

// Field
#include "Field/FieldRo/include/PlacementData/accessor/FieldCollisionAccessor.h"
#include "Field/FieldRo/include/Collision/StaticActor.h"
#include "Field/FieldRo/include/Collision/BaseCollisionScene.h"

// 配置モデル
#include "Field/FieldRo/include/StaticModel/StaticModelManager.h"
#include "Field/FieldRo/include/StaticModel/StaticModel.h"

namespace Field { namespace MoveModel
{
  class FieldMoveModel;
}}

GFL_NAMESPACE_BEGIN( Field );

/**
 *  @brief  アクターの派生種別
 */
enum FieldActorType
{
  FIELD_ACTOR_TYPE_PLACEMENT,               //!< 配置データ
  FIELD_ACTOR_TYPE_MAPJUMP,                 //!< マップジャンプ
  FIELD_ACTOR_TYPE_MOVEMODEL,               //!< ムーブモデル
  FIELD_ACTOR_TYPE_ACTION,                  //!< アクションイベント
  FIELD_ACTOR_TYPE_STATICMODEL,             //!< 配置モデル
  FIELD_ACTOR_TYPE_BG,                      //!< BGイベント
  FIELD_ACTOR_TYPE_ENCOUNT,                 //!< エンカウント
  FIELD_ACTOR_TYPE_TRAINER,                 //!< トレーナー
  FIELD_ACTOR_TYPE_TRAFFIC,                 //!< 往来
  FIELD_ACTOR_TYPE_ITEM,                    //!< 配置アイテム
  FIELD_ACTOR_TYPE_GIMMICK_ENCOUNT_STANDBY, //!< ギミックエンカウント待機型
  FIELD_ACTOR_TYPE_GIMMICK_ENCOUNT_CHASE,   //!< ギミックエンカウント接近型
  FIELD_ACTOR_TYPE_GIMMICK_ENCOUNT_PATROL,  //!< ギミックエンカウント巡回型
  FIELD_ACTOR_TYPE_GIMMICK_ENCOUNT_ESCAPE,  //!< ギミックエンカウント逃走型
  FIELD_ACTOR_TYPE_POKEFINDER,              //!< ポケファインダー
  FIELD_ACTOR_TYPE_BERRY_POINT,             //!< きのみ
  FIELD_ACTOR_TYPE_FISHING_SPOT,            //!< 釣りスポット
  FIELD_ACTOR_TYPE_INTRPOS,                 //!< インタラクティブPOS
  FIELD_ACTOR_TYPE_ACTION_TALK,             //!< アクションイベント内で話しかけ範囲があるもの用

  FIELD_ACTOR_TYPE_MAX,
}; // enum FieldActorType

class IFieldActorBase
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( IFieldActorBase );

public:
  /**
   *  初期化用構造体
   */
  struct SetupData
  {
    System::nijiAllocator           *pAllocator;
    gfl2::heap::HeapBase            *pHeap;
    BaseCollisionScene              *pScene;
    FieldCollisionAccessor  *pColAccessor;
    gfl2::math::Vector3             vPos;
  };

  /**
   *  コリジョン初期化用構造体
   */
  struct CollisionSetupData
  {
    BaseCollisionScene* pScene;
    const void*         pCollisionData;
  };

  /**
   *  起動リクエスト構造体
   */
  struct KickRequest
  {
    Field::MoveModel::FieldMoveModel *pPlayer;
    BaseActor                 *pActor;  //!< どのコリジョンが接触したか
    StaticModel::StaticModelManager *pStaticModelManager;
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  IFieldActorBase( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~IFieldActorBase( void );

  /**
   *  @brief  初期化
   */
  void Initialize( SetupData &data );

  /**
   *  @brief  破棄
   */
  virtual void Terminate( void );
  
  /**
   *  @brief  追加
   */
  virtual void RegistActor( void );

  /**
   *  @brief  削除
   */
  virtual void UnregistActor( void );

  /**
   *  @brief  追加チェック
   */
  bool IsRegist( void ) const { return m_bIsRegist; }

  /**
   *  @brief  種別取得
   */
  virtual FieldActorType GetType( void ) const = 0;

  /**
   *  @brief  起動
   *  @param  req パラメータ
   *  @param  p_outIsEventCreate  GameEventを生成したかどうか？ true :起動 false :起動していない
   *  @retval 何かしら処理を実行したか
   */
  virtual bool Kick( const KickRequest &req, bool * p_outIsEventCreate ) = 0;

  /**
   *  @brief  判定
   */
  virtual bool IsInvalid( void ) = 0;

  /**
   *  @brief  座標取得
   */
  const gfl2::math::Vector3 &GetPosision( void ) const { return m_vPos; }

  /**
   *  @brief  カメラサポート停止チェック
   */
  virtual bool IsCameraSupportStop( void ) const { return true; }

  /**
   *  @brief  カメラフォーカス停止チェック
   */
  virtual bool IsCameraFocusStop( void ) const { return true; }
  
  /**
   *  @brief  アクションボタンの注視をクリアするフラグ 通常起動はクリアする。
   */
  virtual bool IsActionButtonInterestClear( void ) const { return true; }

  /**
   *  @brief  イベントID取得
   */
  virtual u32 GetDataEventID() const { return 0; }

  /**
   *  @brief  データポジション取得
   */
  virtual gfl2::math::Vector3 GetDataPosition() const {
    return gfl2::math::Vector3(0,0,0);
  }

  /**
   *  @brief  データクォータニオン取得
   */
  virtual gfl2::math::Quaternion GetDataQuaternion() const {
    return gfl2::math::Quaternion(0,0,0,0);
  }

  /**
   *  @brief  フラグワークの値セット
   */
  virtual bool SetEventFlagWork( u16 value );

protected:
  /**
   *  @brief  スタティックコリジョン登録用構造体
   */
  struct StaticCollisionPair
  {
    StaticActor* pActor;          //< コリジョンアクター
    BaseCollisionScene* pScene;   //< 紐付いているシーン
  };

  BaseCollisionScene  *m_pCollisionScene;
  StaticActor         **m_ppActor;
  u32                 m_Size;
  bool                m_bIsRegist;
  gfl2::math::Vector3 m_vPos;

  /**
   *  @brief  スタティックコリジョンアクターの生成
   *          (現在円柱のみに対応しているが、他の形状も修正により対応可能)
   *  @param  pOut              結果を受け取る構造体
   *  @param  desc              初期化用情報
   *  @param  vPos              座標
   *  @param  pAllocator        コリジョン生成に使用するアロケータ
   *  @param  pHeap             コリジョンインスタンスの取得元
   *  @param  pFieldActor       紐付けるイベントのアクター
   */
  static void CreateCollisionInstance(
    StaticCollisionPair* pOut,
    const CollisionSetupData& desc, 
    gfl2::math::Vector3& vPos,
    System::nijiAllocator* pAllocator,
    gfl2::heap::HeapBase* pHeap,
    IFieldActorBase* pFieldActor
    );

  /**
   *  @brief  コリジョンの登録
   */
  static void RegisterCollision(const StaticCollisionPair* pPair);

  /**
   *  @brief  コリジョンの登録解除
   */
  static void UnregisterCollision(const StaticCollisionPair* pPair);

}; // class IActionEventBase

GFL_NAMESPACE_END( Field );

#endif // !defined(__FIELD_ACTOR_BASE_H__)
