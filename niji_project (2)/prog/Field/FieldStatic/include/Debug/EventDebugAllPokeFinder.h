//======================================================================
/**
 * @file EventDebugAllPokeFinder.h
 * @date 2016/02/04 15:57:58
 * @author miyachi_soichi
 * @brief デバッグ：全ファインダー起動
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __EVENT_DEBUG_ALLPOKEFINDER_H_INCLUDED__
#define __EVENT_DEBUG_ALLPOKEFINDER_H_INCLUDED__
#pragma once
#if PM_DEBUG
// gfl2
#include <macro/include/gfl2_Macros.h>
// event
#include "GameSys/include/GameEvent.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

/**
 *  @class  EventDebugAllPokeFinder
 *  @brief  デバッグ：全ファインダー起動
 */
class EventDebugAllPokeFinder : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(EventDebugAllPokeFinder);

public:
  /**
   *  @brief  コンストラクタ
   *  @param  pHeap   workようヒープ
   */
  EventDebugAllPokeFinder( gfl2::heap::HeapBase *pHeap );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EventDebugAllPokeFinder( void ){}

public:
  /**
   *  @brief  イベントを動かしてよいかどうかの判定関数
   *  @param  pGameManager  ゲームマネージャ
   *  @retval true  起動して良い
   *  @retval false 起動してはいけない
   */
  virtual bool BootChk( GameSys::GameManager *pGameManager );

  /**
   *  @brief  初期化関数
   *  @param  pGameManager  ゲームマネージャ
   */
  virtual void InitFunc( GameSys::GameManager *pGameManager );

  /**
   *  @brief  メイン関数
   *  @param  pGameManager    ゲームマネージャ
   *  @return GMEVENT_RESUTLT イベント制御関数の戻り値
   */
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager *pGameManager );

  /**
   *  @brief  終了関数
   *  @param  pGameManager    ゲームマネージャ
   */
  virtual void EndFunc( GameSys::GameManager *pGameManager );

  /**
   *  @brief  抽選分岐
   */
  void SetLotteryFlag( bool flag ){ m_bLottery = flag; }

private:
  /**
   *  @brief  ゾーンのチェック
   */
  bool checkZone( GameSys::GameManager *pGameManager, u32 zone );

  /**
   *  @brief  分岐チェック
   */
  bool checkAddIdx( GameSys::GameManager *pGameManager );

private:
  enum SeqNo
  {
    SEQ_CheckZone,
    SEQ_MapJump,
    SEQ_CallFinder,
    SEQ_AddIdx,
    SEQ_AddActor,
    SEQ_AddZone,
    SEQ_End,
  };
  u32   m_zoneID;
  u32   m_actorIdx;
  u32   m_spotID;
  bool  m_bLottery;
  bool  m_bAdvent[20];
};

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // PM_DEBUG
#endif // __EVENT_DEBUG_ALLPOKEFINDER_H_INCLUDED__
