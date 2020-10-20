//======================================================================
/**
 * @file FieldStaticModelActorBase.h
 * @date 2015/08/19 14:30:07
 * @author saita_kazuki
 * @brief 配置モデルアクター基底クラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_STATIC_MODEL_ACTOR_BASE_H_INCLUDED__
#define __FIELD_STATIC_MODEL_ACTOR_BASE_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>

#include "Field/FieldRo/include/PlacementData/actor/FieldActorBase.h"
#include "Field/FieldRo/include/Terrain/Block/FieldTerrainBlockLodChangeStaticListener.h"



namespace Field
{
  namespace StaticModel
  {
    class StaticModelManager;
  }
  namespace Terrain
  {
    class TerrainManager;
  }
  class MyRenderingPipeLine;
}
namespace System
{
  class nijiAllocator;
}

GFL_NAMESPACE_BEGIN(Field)

/**
 * @class FieldStaticModelActorBase
 * @brief 動作モデルアクター基底
 * @note 初期化、破棄、カリング処理での共通処理を用意
 */
class FieldStaticModelActorBase : public IFieldActorBase
{
  GFL_FORBID_COPY_AND_ASSIGN(FieldStaticModelActorBase);

public:
  /**
   *  @brief  コンストラクタ
   */
  FieldStaticModelActorBase( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~FieldStaticModelActorBase( void );

  /**
   *  @brief  1フレ遅れ解放リクエスト
   */
  virtual void RequestDelayTerminate( void );

  /**
   *  @brief  完全破棄
   */
  virtual void Terminate( void );

  /**
   *  @brief  リソース、コリジョンの破棄(モデルワークは残す）
   */
  void TerminateResource( void );
  
  /**
   *  @brief  追加
   */
  virtual void RegistActor( void );

  /**
   *  @brief  削除
   */
  virtual void UnregistActor( void );

  /**
   *  @brief  種別取得
   */
  virtual FieldActorType GetType( void ) const = 0;
  
  /**
   *  @brief  起動
   */
  virtual bool Kick( const KickRequest &req, bool* p_outIsEventCreate ) = 0;

  /**
   *  @brief  判定
   */
  virtual bool IsInvalid( void ) = 0;

protected:

  /*
  * @class 配置モデル用LOD通知リスナー
  */
  class LodChangeStaticListener : public Field::Terrain::Block::LodChangeStaticListener
  {
    /** @brief  コピー禁止クラスにする */
    GFL_FORBID_COPY_AND_ASSIGN( LodChangeStaticListener );

  public:

    LodChangeStaticListener(){}
    virtual ~LodChangeStaticListener(){}

    /**
    * @brief 地形ブロックに登録された瞬間に実行される関数
    *
    * @param  lodType 登録時の地形のLOD状態
    *
    * @return 無し
    */
    virtual void RegistAfter( Field::Terrain::Block::LodType::Value lodType );

    /**
    * @brief 地形ブロックのLODが切り替わった瞬間に実行される関数
    *
    * @param  lodType 切り替え後のLOD状態
    *
    * @return 無し
    */
    virtual void ChangeLod( Field::Terrain::Block::LodType::Value lodType );

    /**
    * @brief 所属する地形ブロックを決定する座標を取得する
    *
    * @param  pOut　座標格納先  
    *
    * @return 成否
    */
    virtual b32 GetControlPosition( gfl2::math::Vector3* pOut ) const ;

    /**
     *  @brief  監視するアクターを設定
     */
    void Setup( FieldStaticModelActorBase * pActor );

  private:

    FieldStaticModelActorBase * m_pActor;
  };

protected:

  /**
   *  @brief  追加
   */
  virtual void registActor( void ) = 0;

  /**
   *  @brief  削除
   */
  virtual void unregistActor( void ) = 0;

  /**
   *  @brief  コリジョンデータ取得
   *  @param pOutColData コリジョンデータ格納先
   *  @param collisionAccessor コリジョンデータアクセサー
   *  @param index コリジョンデータ番号
   */
  bool getCollisionData(
    FieldCollisionAccessor::CollisionData* pOutColData,
    const FieldCollisionAccessor& collisionAccessor,
    u32 index = 0
    );

  /**
   *  @brief  コリジョンのセットアップ
   *  @param collisionData CollisionAccessorから取得したバイナリデータ
   *  @param parentPos アクターの位置
   *  @param pBaseActor コリジョン生成するActor
   *  @param pUserData Actorに登録するユーザー任意データ(NULLなら登録しない)
   */
  void setupCollision(
    const FieldCollisionAccessor::CollisionData& colData,
    gfl2::math::Vector3& parentPos,
    BaseActor* pCollisionActor,
    void* pUserData
    );

  /**
   *  @brief  LOD通知リスナーの登録
   */
  b32 registLODChangeListener( void );

  /**
   *  @brief  LOD通知リスナーの登録解除
   */
  void unregistLODChangeListener( void );

  /**
   *  @brief  破棄共通処理
   *  @retval true 破棄処理を行った
   *  @retval false 破棄処理を行っていない
   */
  virtual bool terminate( void );

  /**
   *  @brief  カリング状態ON
   */
  void onCulling( void );

  /**
   *  @brief  カリング状態OFF
   */
  void offCulling( void );

  /**
   *  @brief  LOD変更可能かどうか
   *  @retval true 可能
   *  @retval true 不可能
   */
  bool isChangeLOD( void );

  /**
   *  @brief  LOD変更
   */
  void changeLOD( Field::Terrain::Block::LodType::Value lodType );
  bool changeLOD_VisibleOff( StaticModel::ModelType old, StaticModel::ModelType next);
  bool changeLOD_VisibleKeep( StaticModel::ModelType old, StaticModel::ModelType next);

  /**
   *  @brief  アクターを登録する必要があるか
   *  @retval true できる
   *  @retval false できない
   */
  bool isNeedRegistActor( void );

  /**
   *  @brief  アクターを削除する必要があるか
   *  @retval true できる
   *  @retval false できない
   */
  bool isNeedUnRegistActor( void );

  /**
   *  @brief  コリジョンシーンへの登録
   */
  void registCollisionScene( void );

  /**
   *  @brief  コリジョンシーンからの削除
   */
  void unregistCollisionScene( void );

  /**
   *  @brief  配置モデルの表示ON
   */
  void visibleStaticModel( void );

  /**
   *  @brief  配置モデルの表示OFF
   */
  void invisibleStaticModel( void );

  /**
   *  @brief  配置モデルの描画パイプラインへの登録
   */
  void registRenderPipeLine( u32 modelType );

  /**
   *  @brief  配置モデルの描画パイプラインからの削除
   */
  void unregistRenderPipeLine( u32 modelType );

  /**
   *  @brief  地形LODタイプから配置モデルのモデルタイプ取得
   */
  StaticModel::ModelType getModelTypeByTerrainLodType( Terrain::Block::LodType::Value lodType );

protected:

  // 専用データ
  StaticModel::StaticModel*         m_pStaticModel;
  StaticModel::StaticModelManager*  m_pStaticModelManager;
  Field::Terrain::TerrainManager*   m_pTerrainManager;
  MyRenderingPipeLine*              m_pRenderingPipeLine;

  b32                               m_bIsCulling;
  // LOD変更通知リスナー
  LodChangeStaticListener           m_LODChangeListener;
  b32                               m_bIsLODChangeRegist;

#if PM_DEBUG
public:
  void DebugFunc_ChangeVisible( void );
#endif
};

GFL_NAMESPACE_END(Field)

#endif // __FIELD_STATIC_MODEL_ACTOR_BASE_H_INCLUDED__
