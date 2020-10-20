//======================================================================
/**
 * @file ReportPlayer.h
 * @date 2015/12/07 18:35:45
 * @author takahashi_tomoya
 * @brief　レポートでのアニメーション制御
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __REPORT_PLAYER_H__
#define __REPORT_PLAYER_H__
#pragma once

#include <macro/include/gfl2_Macros.h>

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "GameSys/include/GameEvent.h"


GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(MoveModel)
  class FieldMoveModelPlayer;
GFL_NAMESPACE_END(MoveModel)

GFL_NAMESPACE_BEGIN(Effect)
  class IEffectBase;
  class EffectManager;
GFL_NAMESPACE_END(Effect)

  /**
   * @def
   * マクロのコメント
   */

  /**
   * @enum Enum
   * 列挙体の説明
   */

  /**
   * @struct tStruct1
   * @brief  構造体の説明
   */

 /** 
  * @class レポートでのプレイヤー挙動
  */
class FieldReportPlayer
{
  GFL_FORBID_COPY_AND_ASSIGN( FieldReportPlayer ); // コピーを禁止

public:

  FieldReportPlayer(); 

  ~FieldReportPlayer();

  /**
   *  @brief 初期化
   */
  void Initialize( gfl2::heap::HeapBase * pHeap, MoveModel::FieldMoveModelManager* pManager, const GameSys::GameData& gameData, Effect::EffectManager* pEffectManager );

  /**
   *  @brief 破棄
   */
  void Terminate();

  b32 Update(b32 is_end);

private:

  b32 load();


  /** 
   *  @brief アニメーション開始
   */
  void start(u32 animationID, u32 obAnimationID, u32 layoutAnimationID);

  /**
   *  @brief 更新
   *  @retval true 終了
   *  @retval false 途中
   */
  b32 update();

  /**
   *  @brief プレイヤーは動作させるか？
   */
  b32 isPlayerAction() const;


private:
  static const u32 LOAD_DYNAMIC_ANIMATION[2];
  static const u32 DYNAMIC_START_ANIMATION_ID[2];
  static const u32 DYNAMIC_UPDATE_ANIMATION_ID[2];
  static const u32 DYNAMIC_END_ANIMATION_ID[2];

  static const u32 LOAD_OB0028_CHARA_ID[2];
  static const u32 LOAD_OB0028_DYNAMIC_ANIMATION[2];
  static const u32 DYNAMIC_OB0028_START_ANIMATION_ID[2];
  static const u32 DYNAMIC_OB0028_UPDATE_ANIMATION_ID[2];
  static const u32 DYNAMIC_OB0028_END_ANIMATION_ID[2];

private:

  enum State
  {
    ST_NONE,
    ST_LOAD,
    ST_INITIALIZED,
    ST_ANIMATION,
  };

  u32 m_Sex;

  State m_State;
  u32   m_Seq;


  gfl2::heap::HeapBase* m_pHeap;


  MoveModel::FieldMoveModelPlayer* m_pPlayer;
  MoveModel::FieldMoveModelManager * m_pManager;

  Effect::EffectManager* m_pEffectManager;
  Effect::IEffectBase * m_pEffect;

  const EventWork* m_pEventWork;
};

#if PM_DEBUG
class EventFieldReportPlayerTest : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( EventFieldReportPlayerTest ); // コピーを禁止

public:


  static void StartEvent( GameSys::GameManager* p_gameman );

  
  //--------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   * @param   heap         workをとるためのメモリクラス
   */
  //--------------------------------------------------------------
  EventFieldReportPlayerTest( gfl2::heap::HeapBase* heap );
  virtual ~EventFieldReportPlayerTest();

  //--------------------------------------------------------------
  /**
   * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   * @retval  true  起動してよい
   * @retval  false 起動しない
   */
  //--------------------------------------------------------------
  virtual bool BootChk(GameSys::GameManager* gmgr){return true;}

  //--------------------------------------------------------------
  /**
   * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void InitFunc(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   * @return  GMEVENT_RESULT  イベント制御関数の戻り値
   */
  //--------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void EndFunc(GameSys::GameManager* gmgr);


private:

  FieldReportPlayer* m_pPlayerControl;

  u32 m_Count;

};
#endif // PM_DEBUG

GFL_NAMESPACE_END(Field)

#endif // __REPORT_PLAYER_H__