//======================================================================
/**
 * @file  AriTestBtlv.h
 * @brief 15/03/13
 * @author  ariizumi
 * @data  デバッグ用BattleViewSystem
 */
//======================================================================

#ifndef __ARI_TEST_BTLV_H__
#define __ARI_TEST_BTLV_H__
#pragma once
#if PM_DEBUG

#include <heap/include/gfl2_heap.h>
#include <macro/include/gfl2_Macros.h>

#include <Battle/source/btlv/btlv_core.h>
#include <Battle/source/btl_sound.h>

GFL_NAMESPACE_BEGIN( GameSys )
class GameProc;
GFL_NAMESPACE_END( GameSys )

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Ariizumi)

class DebugBattleViewSystem:public btl::BattleViewSystem
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugBattleViewSystem);
 public:
  DebugBattleViewSystem();
  virtual ~DebugBattleViewSystem();

  //システム初期化
  virtual bool Initialize(gfl2::heap::HeapBase* debugHeap,gfl2::heap::HeapBase* btlvHeap);

  virtual btl::BtlvMode GetBtlvMode(void)const{return btl::BTLV_MODE_EFFECT_VIEWER;}

  virtual btl::BTL_POKEPARAM* GetBattlePokeParam( btl::BtlvPos pos ) const{return mBpp[pos];}
  pml::pokepara::PokemonParam *GetPokemonParam( int pos ) const{return mPp[pos];}

  //対戦種類(野生・トレーナー・サブウェイ
  virtual BtlCompetitor GetBattleCompetitor(void)const{return BTL_COMPETITOR_TRAINER;}
  //自分側？
  virtual bool IsPlayerSide(btl::BtlvPos pos );
  virtual bool IsViewerMode()  const { return true; }
  virtual bool IsPokeExist( btl::BtlvPos pos ) const;
  virtual bool IsClientTrainerExist( u8 clientID ) const { return true; }//4体初期化したいのでtrue
  virtual const Savedata::MyStatus* GetClientPlayerData(u8 clientID, int btlFesIdx = -1) const;

  virtual u8 GetBtlvPosToClientId( btl::BtlvPos vpos ) const{return vpos;}

  virtual void GetStringUi(gfl2::str::StrBuf* dst, u16 strID );
  virtual GameSys::GameProc* GetBattleProc(void)const{return mProc;}
  virtual btl::BtlSound* GetBattleSound(void){return &mBtlSound;}
  virtual const BTL_FIELD_SITUATION* GetFieldSituation(void) { return &mFiledSituation; }
  void SetBattleProc(GameSys::GameProc *proc){mProc = proc;}

  void InitEffectViewerDebugMenu(gfl2::heap::HeapBase* debugHeap);
  void TermEffectViewerDebugMenu(void);

  static void TestBtlvDebug_TargetCallBack( void *work );

  // Effect Viewer 下画面用
  void SetDefaultPokePos(void);
  void SetDefaultTrainerPos(void);
  void ChangeRule( BtlRule rule );

  virtual BtlRule GetBattleRule(void)const{return mBtlRule;}

  void SetTargetPoke( int idx , int pokeNo ){mComTargetPoke[idx] = (btl::BtlvPos)pokeNo;}
  btl::BtlvPos GetTargetPoke( int idx ){return mComTargetPoke[idx];}
  void SetDamagePoke( int idx , int pokeNo ){mComTargetDamage[idx] = (btl::BtlvPos)pokeNo;}
  btl::BtlvPos GetDamagePoke( int idx ){return mComTargetDamage[idx];}
  void SetTargetTrainer( int idx , int no ){mTargetTrainer[idx] = no;}
  
  void SetDebugBallID (int slot, int ball_id)
  {
    mEffectBallId[slot] = ball_id;
  };

  void ReloadBattleData(void);

 private:

  bool mStartInitialize;
  int mTempComTargetPos[2];
  pml::pokepara::PokemonParam *mPp[4];
  btl::BTL_POKEPARAM *mBpp[4];

  BtlRule mBtlRule;     // Main Module 側のが使えないので

  GameSys::GameProc *mProc;

  btl::BtlSound mBtlSound;  //BgmFadeの処理が必要なだけなので初期化とかはしない
  BTL_FIELD_SITUATION mFiledSituation;
};

GFL_NAMESPACE_END(Ariizumi)
GFL_NAMESPACE_END(Test)



#endif  //PM_DEBUG
#endif  //__ARI_TEST_BTLV__
