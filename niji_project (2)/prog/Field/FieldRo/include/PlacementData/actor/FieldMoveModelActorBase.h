//======================================================================
/**
 * @file FieldMoveModelActorBase.h
 * @date 2015/08/19 14:30:07
 * @author saita_kazuki
 * @brief 動作モデルアクター基底クラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_MOVE_MODEL_ACTOR_BASE_H_INCLUDED__
#define __FIELD_MOVE_MODEL_ACTOR_BASE_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>

#include "Field/FieldRo/include/PlacementData/actor/FieldActorBase.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"

namespace poke_3d{
  namespace model {
    class DressUpModelResourceManager;  // 着せ替えリソースマネージャ
  }
}



namespace Field
{
  namespace MoveModel
  {
    class FieldMoveModelManager;
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
 * @class FieldMoveModelActorBase
 * @brief 動作モデルアクター基底
 * @note 初期化、破棄、カリング処理での共通処理を用意
 */
class FieldMoveModelActorBase : public IFieldActorBase
{
  GFL_FORBID_COPY_AND_ASSIGN(FieldMoveModelActorBase);

public:
  /**
   *  @brief  コンストラクタ
   */
  FieldMoveModelActorBase( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~FieldMoveModelActorBase( void );

  /**
   *  @brief  完全破棄
   */
  virtual void Terminate( void );

  /**
   *  @brief  リソース、コリジョンの破棄(モデルワークは残す）
   */
	virtual void TerminateResource(void);
  
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

  /**
   *  @brief  動作モデル取得
   */
  MoveModel::FieldMoveModel *GetMoveModel( void ) const;


  /**
   *  @brief 配置情報内のMoveCodeを、Runtime上のMoveCodeに変換
   */
  static MoveModel::FIELD_MOVE_CODE_ID GetPlacementMoveCodeToMoveCode( u32 placement_movecode );

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
   *  @brief  動作モデルワークの初期化
   */
  void initializeMoveModelWork();

  /**
   *  @brief  動作モデル番号の取得
   *  @param eventID このインスタンスを識別するためのユニークなID
   *  @return 動作モデル番号。MoveModel::FIELD_MOVE_MODEL_MAX以外が返ってきたら復帰処理
   */
  u32 getMoveModelIndex( u32 eventID)  const ;

  /**
   *  @brief  動作モデルリソースの初期化
   *  @param  pDressUpModelResourceManager    着せ替えリソースマネージャ（通常はNULLでOK。ジョインフェスタのみ渡す。)
   */
  void initializeMoveModelResource( poke_3d::model::DressUpModelResourceManager* pDressUpModelResourceManager = NULL );

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
   *  @brief  MoveModelWorkが既に作られているか
   *  @param modelIndex モデルインデックス
   *  @retval true 作られている
   *  @retval true 作られていない
   */
  bool isMoveModelWorkCreated( u32 modelIndex);

  /**
   *  @brief  破棄共通処理
   *  @retval true 破棄処理を行った
   *  @retval false 破棄処理を行っていない
   */
  virtual bool terminate();

  /**
   *  @brief  カリング状態ON
   */
  void onCulling();

  /**
   *  @brief  カリング状態OFF
   */
  void offCulling();

  /**
   *  @brief  アクターを登録できるか
   *  @retval true できる
   *  @retval false できない
   */
  bool isRegistActor( void );

  /**
   *  @brief  アクターを削除できるか
   *  @retval true できる
   *  @retval false できない
   */
  bool isUnRegistActor( void );

  /**
   *  @brief  動作モデルの登録を行う
   */
  void registMoveModel( void );

  /**
   *  @brief  動作モデルの登録解除を行う
   */
  void unregistMoveModel( void );

protected:

  // 専用データ
  MoveModel::FieldMoveModelManager                    *m_pMoveModelManager;
  Terrain::TerrainManager                             *m_pTerrainManager;
  MyRenderingPipeLine                                 *m_pRenderingPipeLine;
  u32                                                 m_ModelIndex;
  MoveModel::FieldMoveModelHeaderWork                 m_MoveModelHeaderWork;
  MoveModel::FieldMoveModelHeaderResource             m_MoveModelHeaderResource;

  // カリングリスナー
  // 未使用
  b32                                                 m_bIsCulling;
  b32                                                 m_bIsCullingRegist;

#if PM_DEBUG
public:
  //! デバッグ用：表示切替
  void DebugFunc_ChangeVisible( void );
#endif
};

GFL_NAMESPACE_END(Field)

#endif // __FIELD_MOVE_MODEL_ACTOR_BASE_H_INCLUDED__