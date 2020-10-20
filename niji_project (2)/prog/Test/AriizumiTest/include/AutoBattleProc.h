//======================================================================
/**
 * @file AutoBattleProc.h
 * @date 2016/06/08 19:47:19
 * @author ariizumi_nobuhiko
 * @brief 自動バトルテストProc
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __AUTO_BATTLE_PROC_H_INCLUDED__
#define __AUTO_BATTLE_PROC_H_INCLUDED__
#pragma once

#if  PM_DEBUG

#include <macro/include/gfl2_Macros.h>

#include "GameSys/include/GameProc.h"
#include "GameSys/include/GameManager.h"

#include <Battle/include/battle_SetupParam.h>

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Ariizumi)

//ポケモン生成用(Unitテスト用に外に出すため
  /**
  * @brief テスト用ポケモン生成
  * @param gfl2::heap::HeapBase *pokeHeap PokePara用Heap
  * @param gfl2::heap::HeapBase *workHeap 作業用一時Heap
  * @param const POKE_CREATE_SETTING &setting 生成オプション
  */
typedef struct  
{
  int pokeMode; //ポケモン抽選方法 0:全部 1:要注意
  int itemMode; //アイテム抽選方法 0:全部 1:要注意
  int wazaMode; //技抽選方法 0:全部 1:要注意
  bool isTower; //タワーデータからデータを作るか？
}POKE_CREATE_SETTING;
pml::pokepara::PokemonParam* CreatePokeFunc(gfl2::heap::HeapBase *pokeHeap,gfl2::heap::HeapBase *workHeap,const POKE_CREATE_SETTING &setting);


class AutoBattleProc : public GameSys::GameProc 
{
  GFL_FORBID_COPY_AND_ASSIGN(AutoBattleProc); //コピーコンストラクタ＋代入禁止

public:
  AutoBattleProc(); //コンストラクタ
  virtual ~AutoBattleProc(void);  //デストラクタ

  //============================================================================
  //GameProc
  //============================================================================
  //------------------------------------------------------------------
  /**
    * @brief   PROCの初期化を行う関数
    * @return  Result 動作が終了すると RES_FINISHを返してもらう事でMainFuncに進む
    */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager);

  //------------------------------------------------------------------
  /**
    * @brief   PROCの終了を行う関数
    * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
    */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);

  //------------------------------------------------------------------
  /**
    * @brief   PROCのメインを行う関数
    * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
    */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pManager);

  /**
  * @brief	PROCの描画処理(下準備)
  * @param pManager   プロセス管理マネージャー
  * 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
  * 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
  */
  virtual void PreDrawFunc( gfl2::proc::Manager* pManager );

  /**
  * @brief	PROCの描画処理
  * @param pManager   プロセス管理マネージャー
  * @param displayNo  描画対象画面
  * 登録したPROCによる、フレームバッファへの描画を実行します。
  * ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
  */
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );

private:

  int mMainSeq;

  btl::BTLV_DEBUG_PARAM mDebugParam;
  BATTLE_SETUP_PARAM  mSetupParam;
  BTL_FIELD_SITUATION mSit;

  gfl2::math::SFMTRandom mRandSys;

  static const int AB_PARTY_NUM = 4;
  static const int AB_POKE_NUM = 6;

  pml::PokeParty* mPokeParty[AB_PARTY_NUM];
  pml::pokepara::PokemonParam* mPokePara[AB_PARTY_NUM][AB_POKE_NUM];

  bool mIsPlayBattleBgm;
  int mRule;
  int mPokeNum;
  int mSetupRandSeed;
  int mBattleRandSeed;
  bool mEffectSkip;
  bool mNoDraw;
  bool mOutputLogSd;
  int  mLogUniqueNo;

  int mPokeCreateSetting;
  int mItemSetting;
  int mWazaSetting;

  bool mTowerMode;

  int mSpecialDebugMode;
  int mBattleNum;

};  //class AutoBattleProc


//STRCORE→sjis変換
void DebugStrcodeToUtf8(const gfl2::str::STRCODE *buf,char *ret);

//SDカードろぐかきこみ 
class SDCardLog
{
public:
  void OpenLogFile(gfl2::heap::HeapBase *heap, char *name,bool addTime = true ,int uniqueNo = -1 );
  void CloseLogFile(void);

  void OutputLog(const char* fmt , ... );
private:
  gfl2::fs::DirectSdmcFile *mFile;
};

GFL_NAMESPACE_END(Ariizumi)
GFL_NAMESPACE_END(Test)

#endif  //PM_DEBUG
#endif // __AUTO_BATTLE_PROC_H_INCLUDED__