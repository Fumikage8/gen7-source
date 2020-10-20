/**
 *  GAME FREAK inc.
 *
 *  @file   FieldMoveModelActor.h
 *  @brief  ムーブモデル用アクター
 *  @author miyachi_soichi
 *  @date   2015.05.28
 */

#if !defined(__FIELD_MOVE_MODEL_ACTOR_H__)
#define __FIELD_MOVE_MODEL_ACTOR_H__
#pragma once

// Field
#include "Field/FieldRo/include/PlacementData/actor/FieldMoveModelActorBase.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldMoveModelAccessor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldContactPokemonAccessor.h"
#include "Field/FieldRo/include/PokemonFureai/FieldPokemonFureaiTypes.h"

GFL_NAMESPACE_BEGIN( Field );

class FieldMoveModelActor : public FieldMoveModelActorBase
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldMoveModelActor );


public:
  /**
   *  初期化用構造体
   */
  struct MoveModelSetupData
  {
    MoveModel::FieldMoveModelManager                      *pMoveModelManager;
    BaseCollisionScene                                    *pCollisionSceneForTerrain;
    BaseCollisionScene                                    *pCollisionSceneForStaticModel;
    BaseCollisionScene                                    *pCollisionSceneForEventPos;
    BaseCollisionScene                                    *pCollisionSceneForEventTalk;
    BaseCollisionScene                                    *pCollisionSceneForEventPush;
    const FieldMoveModelAccessor::MoveModelData           *pMoveModelData;
    const FieldContactPokemonAccessor::ContactPokemonData *pContactPokemonData;
    ZoneID                                                zoneId;
    ZoneID                                                dataZoneId;
  };

  struct MoveModelCreateResourceData
  {
     FieldMoveModelAccessor *pAccessor;
     Terrain::TerrainManager * pTerrainManager;
     MyRenderingPipeLine* pRenderingPipeLine;
    const EventWork*      cpEventWork; 
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  FieldMoveModelActor( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~FieldMoveModelActor( void );

  /**
   *  @brief  MoveModel作成
   */
  void Initialize( MoveModelSetupData &setupData );
  
  /**
   *  @brief  リソースとコリジョンの設定
   */
  void CreateResource( const MoveModelCreateResourceData& createResourceData );

	/**
	*  @brief  リソース、コリジョンの破棄(モデルワークは残す）
	*/
	virtual void TerminateResource(void);

  /**
   *  @brief  種別取得
   */
  virtual FieldActorType GetType( void ) const { return FIELD_ACTOR_TYPE_MOVEMODEL; }
  
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
  const FieldMoveModelAccessor::MoveModelData *GetData( void ) const { return m_pData; }

  /**
   *  @brief  動作モデル取得
   */
  MoveModel::FieldMoveModel *GetMoveModel( void ) const;

  /**
   *  @brief  所属ゾーン（イベントエディタで配置したゾーン）ＩＤを取得
   *  @return ZoneID
   */
  ZoneID GetDataZoneID( void ){ return m_DataZoneId; }

  /**
  *  @brief  動作モデル初期化判定
  */
  bool IsSetupMoveModel( void ) const;

  /**
   *  @brief  NPC判定
   */
  bool IsNpc( void ) const;

  /**
   *  @brief  NPCの注目を停止する
   */
  void StopNpcInterest( void );

  /**
  *  @brief  アクターの状態をバニッシュフラグに伝播する
  */
  void ApplyActorToVanishFlag( EventWork* pEventWork );

  /**
  *  @brief  ふれあいポケモンかどうか判定
  */
  bool IsContactPokemon(void) const;

  /**
 *  @brief  ふれあいポケモンの注目を停止する
 */
  void StopContactInterest(void);

	/**
	*  @brief  ふれあいポケモンのねむり状態を開始する
	*/
	void StartFrureaiSleep(void);

  /**
   *  @brief  ふれあいポケモンのねむり状態を停止する
   */
  void StopFrureaiSleep(void);

  /**
  *  @brief  ふれあいポケモンの移動領域データを取得
  */
  void GetFreaiMoveAreaData(FieldCollisionAccessor::CollisionData* pData);

  /**
*  @brief  ふれあいポケモンのベース位置を取得
*/
  void GetFureaiBasePosition( gfl2::math::Vector3* pOut );

	/**
	*  @brief  ふれあいポケモンのベース向きを取得
	*/
	void GetFureaiBaseQuaternion(gfl2::math::Quaternion* pOut);

  /**
*  @brief  ふれあいポケモンの設定を取得
*/
  const Field::PokemonFureai::POKEMON_SETTING_DATA* FieldMoveModelActor::GetFureaiPokemonSettingData(void);

  /**
*  @brief 主人公がアクションを行った事をふれあいアクターに通知
*  @note イベント中にふれあいポケモンが喜ぶ動作を行った場合、コール
*/
  void EventPlayerActionReq( s32 type );


  const FieldContactPokemonAccessor::ContactPokemonData* GetContactPokemonData( void ) { return m_pDataForContact; }

#if PM_DEBUG
	void SetDebugFureaiMoveAreaCollision( b32 flag );
#endif // #if PM_DEBUG


  virtual bool IsActionButtonInterestClear() const;

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
  *  @brief  処理負荷対策:アンレジスト時にアニメーションを停止する対象か？
  */
  b32 IsAnimationStopTarget(void) const;


protected:
  // 専用データ
  const FieldMoveModelAccessor::MoveModelData               *m_pData;
  const FieldContactPokemonAccessor::ContactPokemonData     *m_pDataForContact;

  // コリジョンシーン
  BaseCollisionScene                                  *m_pCollisionSceneForTerrain;
  BaseCollisionScene                                  *m_pCollisionSceneForStaticModel;
  BaseCollisionScene                                  *m_pCollisionSceneForEventTalk;
  // アクター
  DynamicActor                                        *m_pActorForTerrain;
  StaticActor                                         *m_pActorForStaticModel;
  StaticActor                                         *m_pActorForEventTalk;
  StaticActor                                         *m_pActorForEventFureaiTalk;
  ZoneID                                              m_DataZoneId;


#if PM_DEBUG
  StaticActor                                         *m_pDebugFureaiMoveAreaActor;
#endif // PM_DEBUG


  // fureai
  FieldCollisionAccessor::CollisionData               m_FreaiMoveArea;

}; // class FieldMoveModelActor : public IFieldActorBase

GFL_NAMESPACE_END( Field );

#endif // !defined(__FIELD_MOVE_MODEL_ACTOR_H__)
