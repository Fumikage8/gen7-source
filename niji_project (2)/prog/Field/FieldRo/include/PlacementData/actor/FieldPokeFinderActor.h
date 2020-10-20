//======================================================================
/**
 * @file FieldPokeFinderActor.h
 * @date 2015/10/01 16:04:23
 * @author miyachi_soichi
 * @brief 配置：ポケファインダースターター
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_POKEFINDER_ACTOR_H_INCLUDED__
#define __FIELD_POKEFINDER_ACTOR_H_INCLUDED__
#pragma once

#include "Field/FieldRo/include/PlacementData/actor/FieldActorBase.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldPokeFinderAccessor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldCollisionAccessor.h"

GFL_NAMESPACE_BEGIN(Field)

/**
 *  @class  FieldPokeFinderActor
 *  @brief  配置：ポケファインダースターター
 */
class FieldPokeFinderActor : public IFieldActorBase
{
  GFL_FORBID_COPY_AND_ASSIGN(FieldPokeFinderActor);

public:
  /**
   *  初期化用構造体
   */
  struct PokeFinderSetupData
  {
    System::nijiAllocator                         *pAllocator;
    gfl2::heap::HeapBase                          *pHeap;
    BaseCollisionScene                            *pCollisionSceneForEventTalk;
    FieldPokeFinderAccessor                       *pAccessor;
    const FieldPokeFinderAccessor::PokeFinderData *pData;
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  FieldPokeFinderActor( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~FieldPokeFinderActor( void );

  /**
   *  @brief  初期化
   *  @param  setupData   初期化パラメータ
   */
  void Initialize( PokeFinderSetupData &setupData );

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
   *  @brief  種別取得
   */
  virtual FieldActorType GetType( void ) const { return FIELD_ACTOR_TYPE_POKEFINDER; }

  /**
   *  @brief  起動
   *  @param  req   パラメータ
   *  @param  p_outIsEventCreate    GameEventを生成したかどうか？ true :起動 false :起動していない
   *  @return 何かしら処理を実行したか
   */
  virtual bool Kick( const KickRequest &req, bool * p_outIsEventCreate );

  /**
   *  @brief  判定
   */
  virtual bool IsInvalid( void );

  /**
   *  @brief  レア判定
   *  @retval true:レア
   *  @retval false:ノーマル
   */
  bool IsRare( void ){ return m_nSubjectID != -1; }

  /**
   *  @brief  スポットIDを取得
   */
  u32 GetSpotID( void ){ return m_nSpotID; }

  /**
   *  @brief  登場ポケモンテーブルIDを取得する
   */
  s32 GetSubjectID( void ){ return m_nSubjectID; }

  // データ取得
  const FieldPokeFinderAccessor::PokeFinderData *GetData( void ){ return m_pData; }
  const void *GetMoveAData( void ){  return m_pMoveAData; }
  const void *GetMoveBData( void ){  return m_pMoveBData; }
  const void *GetWaitData( void ){   return m_pWaitData; }
  const void *GetOnewayData( void ){ return m_pOnewayData; }

protected:
  gfl2::heap::HeapBase                          *m_pHeap;   //!< ヒープ
  u32                                           m_nSpotID;
  s32                                           m_nSubjectID;
  const FieldPokeFinderAccessor::PokeFinderData *m_pData;
  const void                                    *m_pMoveAData;    // 巡回Ａ
  const void                                    *m_pMoveBData;    // 巡回Ｂ
  const void                                    *m_pWaitData;     // 待機
  const void                                    *m_pOnewayData;   // 一方通行

#if PM_DEBUG
public:
  //! デバッグ用初期化
  struct Debug_PokeFinderSetupData
  {
    System::nijiAllocator                         *pAllocator;
    gfl2::heap::HeapBase                          *pHeap;
    BaseCollisionScene                            *pCollisionSceneForEventTalk;
    FieldPokeFinderAccessor::PokeFinderData       *pData;
    FieldCollisionAccessor::CollisionData         *pColData;
    u32                                           spot_id;
  };
  void Debug_Initialize( Debug_PokeFinderSetupData &setupData );
#endif
};

GFL_NAMESPACE_END(Field)

#endif // __FIELD_POKEFINDER_ACTOR_H_INCLUDED__
