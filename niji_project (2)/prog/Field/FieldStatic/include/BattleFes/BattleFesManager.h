//======================================================================
/**
* @file BattleFesManager.h
* @date 2016/11/23
* @author munakata_kai
* @brief バトルフェス常駐マネージャー
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if !defined __BATTLEFESMANAGER_H_INCLUDED__
#define __BATTLEFESMANAGER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "Field/FieldStatic/include/BattleFes/BattleFes.h"
#include "Field/FieldStatic/include/BattleFes/BattleFesDefine.h"
#include "Battle/include/battle_SetupParam.h" //for BATTLE_SETUP_PARAM
#include "Battle/Regulation/include/Regulation.h" //for REGULATION for REG_XXX レギュレーションID
#include <Savedata/include/BattleFesSave.h>

GFL_NAMESPACE_BEGIN( BattleFes )

class BattleFesMsgResourceManager;
class BattleFesPokeLotterySystem;

/**
 * @class 
 */
class BattleFesManager
{
  GFL_FORBID_COPY_AND_ASSIGN( BattleFesManager );

public:
  BattleFesManager(gfl2::heap::HeapBase* pHeap);
  ~BattleFesManager();

  //----------------------------------------------------------------------------
  /**
  *  @brief バトルフェスクラスを作成
  */
  //-----------------------------------------------------------------------------
  void Create( GameSys::GameManager *p_gman );
  //----------------------------------------------------------------------------
  /**
  *  @brief バトルフェスクラスを破棄
  */
  //-----------------------------------------------------------------------------
  void Delete();
  //----------------------------------------------------------------------------
  /**
  *  @brief バトルフェスクラスを取得
  */
  //-----------------------------------------------------------------------------
  BattleFes* GetBattleFes() const { return m_pBattleFes; }
  //----------------------------------------------------------------------------
  /**
  *  @brief バトルフェスクラスの作成完了？
  */
  //-----------------------------------------------------------------------------
  const bool IsCreated() const { return m_pBattleFes != NULL; }

  // 定型文あいさつメッセージを出すため　※バトルフィールド内のみ使用
  //----------------------------------------------------------------------------
  /**
  *  @brief BattleFesMsgResourceManagerのメッセージデータ取得
  */
  //-----------------------------------------------------------------------------
  gfl2::str::MsgData* GetMsgData();
  //----------------------------------------------------------------------------
  /**
  *  @brief BattleFesMsgResourceManagerの設定
  */
  //-----------------------------------------------------------------------------
  inline void SetMsgResourceManager(BattleFesMsgResourceManager* pMsgResourceManager) { m_pMsgResourceManager = pMsgResourceManager; }

  //  バトル中、バトルビデオ、バトル結果に必要なため　※バトルフィールド内のみ使用　フィールドが死んでいても持っていなきゃいけない
  //----------------------------------------------------------------------------
  /**
  *  @brief BATTLE_SETUP_PARAMの取得
  */
  //-----------------------------------------------------------------------------
  BATTLE_SETUP_PARAM* GetSetupParam() const;
  //----------------------------------------------------------------------------
  /**
  *  @brief BATTLE_SETUP_PARAMの設定
  */
  //-----------------------------------------------------------------------------
  inline void SetSetupParam(BATTLE_SETUP_PARAM* pSetupParam) { m_pSetupParam = pSetupParam; }
  
  //! バトルフェスポケモン抽選管理システム取得（ポケモンステータスアプリ起動時に必要なため）※フィールドが死んでいても持っていなきゃいけない、城内のみ使用
  //----------------------------------------------------------------------------
  /**
  *  @brief BattleFesPokeLotterySystemの取得
  */
  //-----------------------------------------------------------------------------
  BattleFesPokeLotterySystem* GetBattleFesPokeLotterySystem() const { return m_pBattleFesPokeLotterySystem; }
  //----------------------------------------------------------------------------
  /**
  *  @brief BattleFesPokeLotterySystemの作成
  */
  //-----------------------------------------------------------------------------
  void CreateBattleFesPokeLotterySystem(gfl2::heap::HeapBase* pHeap);
  //----------------------------------------------------------------------------
  /**
  *  @brief BattleFesPokeLotterySystemの破棄
  */
  //-----------------------------------------------------------------------------
  void DeleteBattleFesPokeLotterySystem();

  // バトル後のバトルビデオを呼ぶ出す時にのみ呼びます　※バトルフィールド内のみ使用　フィールドが死んでいても持っていなきゃいけない
  //----------------------------------------------------------------------------
  /**
  *  @brief レギュレーションを取得
  */
  //-----------------------------------------------------------------------------
  Regulation* GetRegulation() const;
  //----------------------------------------------------------------------------
  /**
  *  @brief レギュレーションを設定
  */
  //-----------------------------------------------------------------------------
  inline void SetRegulation(Regulation* pRegulation) { m_pBtlRegulation = pRegulation; }

  //! レインボーロケット団専用バトルフェス戦闘データ取得 ※フィールドが死んでいても持っていなきゃいけない、レインボーロケット団イベント中のみ使用
  //----------------------------------------------------------------------------
  /**
  *  @brief BattleFesSaveの取得
  */
  //-----------------------------------------------------------------------------
  Savedata::BattleFesSave* GetBattleFesSave() const { return m_pBattleFesSave; }

  //----------------------------------------------------------------------------
  /**
  *  @brief モニター乱数シード値を取得
  */
  //-----------------------------------------------------------------------------
  inline u32 GetMonitorRandSeed() const { return m_monitorRandSeed; };
  //----------------------------------------------------------------------------
  /**
  *  @brief モニター乱数シード値を設定
  */
  //-----------------------------------------------------------------------------
  inline void SetMonitorRandSeed(const u32 seed) { m_monitorRandSeed = seed; }

private:
  BattleFes*                   m_pBattleFes;                                      ///< バトルフェス
  BattleFesMsgResourceManager* m_pMsgResourceManager;                             ///< メッセージデータ管理クラス
  BATTLE_SETUP_PARAM*          m_pSetupParam;                                     ///< バトルパラム 初期化終了タイミングはスクリプト
  BattleFesPokeLotterySystem*  m_pBattleFesPokeLotterySystem;                     ///< ポケモン抽選＆抽選ポケモン保持クラス
  Regulation*                  m_pBtlRegulation;                                  ///< レギュレーション描画情報(レギュレーション実体)

  // レインボーロケット団専用バトルフェス戦闘データ
  Savedata::BattleFesSave*     m_pBattleFesSave;
  // @fix MMCat[663] 日付を跨いだ直後にバトルエージェントのレンタルポケモンの「つよさをみる」と、ラインナップポケモンの更新が発生してつよさをみる前後でのラインナップに相違が発生する
  u32                          m_monitorRandSeed;
};

GFL_NAMESPACE_END( BattleFes )

#endif // __BATTLEFESMANAGER_H_INCLUDED__