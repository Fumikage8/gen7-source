#ifndef __RESORT_INIT_LOGIC_H__
#define __RESORT_INIT_LOGIC_H__
//=============================================================================
/**
 * @file	 ResortInitLogic.h
 * @brief	 アイランドの計算部分
 * @author k.ohno
 * @date	 2015.9.05
 */
//=============================================================================
#pragma once

#include "macro/include/gfl2_Macros.h"
#include "heap/include/gfl2_Heap.h"

#include "AppLib/include/Frame/AppFrameCellSuper.h"

#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"

#include "App/Resort/include/ResortAppParam.h"
#include "AppLib/include/Util/AppRenderingManager.h"

#include "niji_conv_header/app/island/wildcreate_islandbin.h"
#include "niji_conv_header\app\island\kinomi_islandbin.h"
#include "niji_conv_header\app\island\walk_islandbin.h"
#include "niji_conv_header\app\island\hotspa_islandbin.h"
#include "niji_conv_header\app\island\wildpokemon_islandbin.h"
#include "niji_conv_header\app\island\gim_pointup_islandbin.h"
#include "niji_conv_header\app\island\gim_lv_islandbin.h"
#include "niji_conv_header\app\island\pokemame_islandbin.h"
#include "niji_conv_header\app\island\beans_islandbin.h"
#include "niji_conv_header\app\island\adventure_islandbin.h"
#include "niji_conv_header\app\island\adventure_itemtbl_islandbin.h"
#include "niji_conv_header\app\island\talktype_islandbin.h"
#include "niji_conv_header\app\island\dropbeans_islandbin.h"



GFL_NAMESPACE_BEGIN(Savedata)
class ResortSave;
GFL_NAMESPACE_END(Savedata)
GFL_NAMESPACE_BEGIN( Field )
class EventWork;
GFL_NAMESPACE_END( Field )
GFL_NAMESPACE_BEGIN(PokeTool)
class KawaigariParamCareCoreManager;
GFL_NAMESPACE_END(PokeTool)



GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)



class ResortInitLogic
{
  GFL_FORBID_COPY_AND_ASSIGN(ResortInitLogic);


  static const int _INTERVAL_FRAME = 30;

  static const int ADVENTURE_MAX_LOTT=11;  //一回で抽選する回数の最大


  static const int _NORMAL_TIMER = 20;
  static const int _RARE_TIMER = 60;
  static const int _NIJI_TIMER = 120;
  
  static const u32 WILD_POKE_LV_RAND = 7;   // 野生ポケモンのレベルに使用するランダム値

  
public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  ResortInitLogic(void);

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~ResortInitLogic();



public:


  //------------------------------------------------------------------
  /**
   * @brief   リゾート最初の処理
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  void FirstFunc(void);

  //------------------------------------------------------------------
  /**
   * @brief   初期化を行う関数
   */
  //------------------------------------------------------------------
  void InitFunc(Savedata::ResortSave* pRresort,
                Field::EventWork* pEventSave,gfl2::heap::HeapBase* pHeap,gfl2::heap::HeapBase* pTempHeap,App::Resort::APP_PARAM* pAppParam,Savedata::BoxPokemon* pBoxPoke,
                PokeTool::KawaigariParamCareCoreManager* pCare);
  //------------------------------------------------------------------
  /**
   * @brief   抜ける時の処理
   */
  //------------------------------------------------------------------
  void EndFunc(void);

  //------------------------------------------------------------------
  /**
   * @brief   一秒毎に行う作業
   * @param   resort_index  現在表示中のリゾート
   */
  //------------------------------------------------------------------
  void SecFunc( int resort_index );
  //------------------------------------------------------------------
  /**
   * @brief   木をタッチしたら豆をまく
   */
  //------------------------------------------------------------------
  void IvyTouchOnTheBeans(u8* pbuff);

  //------------------------------------------------------------------
  /**
   * @brief  ボーナス時間反映
   */
  //------------------------------------------------------------------
  void SetBonusTimer(int normal,int rare, int niji);
  
  bool GetBonusTimerMove(void);

  //------------------------------------------------------------------
  /**
   * @brief  ポケモンの数によるボーナス段階を得る
   */
  //------------------------------------------------------------------
  u8 GetBonusLevel(void);
  //------------------------------------------------------------------
  /**
   * @brief  前回のボーナス段階を得る
   */
  //------------------------------------------------------------------
  u8 GetPreviousBonusLevel(void);

  //------------------------------------------------------------------
  /**
   * @brief  テーブルの値に沿ってまめを乱数で返す
   */
  //------------------------------------------------------------------
  int GetDropBeansRandom(void);
  //------------------------------------------------------------------
  /**
   * @brief   まめびんながす処理
   */
  //------------------------------------------------------------------
  void SendBeansBottle(void);

  //------------------------------------------------------------------
  /**
   * @brief   ボックスのみのポケモン数を得る
   */
  //------------------------------------------------------------------
  int GetBoxPokemonNum( void );
  //------------------------------------------------------------------
  /**
   * @brief   ボックス+温泉+運動のポケモン数を得る
   */
  //------------------------------------------------------------------
  int GetBoxAndHotSpaAndGimPokemonNum( void );
  //------------------------------------------------------------------
  /**
   * @brief   探検開始
   */
  //------------------------------------------------------------------
  void StartAdventure(void);
  //------------------------------------------------------------------
  /**
   * @brief   探検中断
   */
  //------------------------------------------------------------------
  void StopAdventure(int step);
  //------------------------------------------------------------------
  /**
   * @brief   野生ポケモン更新
   */
  //------------------------------------------------------------------
  bool IsWildPokemonUpdate(void){ return mbWildPokemonUpdate; }
  //------------------------------------------------------------------
  /**
   * @brief   野生ポケモン更新フラグリセット
   */
  //------------------------------------------------------------------
  void ResetWildPokemonUpdate(void){ mbWildPokemonUpdate=false; }
  //--------------------------------------------------------------
  /**
   * @brief   うんどう装置のトータルのこり時間
   * @param   kind  0-2の運動の場所
   * @param   時間、分、秒
   */
  //--------------------------------------------------------------
  void GetGimTotalEndTime(int kind, u8* hour, u8* min, u8* sec);
  //--------------------------------------------------------------
  /**
   * @brief   うんどうスタート
   * @param   kind  0-2の運動の場所
   */
  //--------------------------------------------------------------
  void StartGimTimer(int kind);

  void PokemonEventStartFunc(bool bforce);
  
  void CreatePokemon(int index, int monsno, int lv);

  void SetPokemonCreateData(void* pData);
  void SetEggWalkData(void* pData);
  void SetFamiliarityData(void* pData);
  void SetWildPokemonData(void* pData);
  void SetGimPointupData(void* pData);
  void SetGimLvData(void* pData);
  void SetPokeMameData(void* pData);
  void SetBeansData(void* pData);
  void SetAdventureData(void* pData);
  void SetAdventureItemTblData(void* pData);
  void SetAdventureItemTblData( void* pData, u32 dataSize );
  void SetKinomiData(void* pData);
  void SetTalkTypeData(void* pData);
  void SetDropBeansData(void* pData);

  bool IsAdventureCourseSet(void);
  bool IsAdventureReturned(void){ return mbAdventureReturned; }  //探検から戻った場合
  void SetAdventureReturned(bool onoff){mbAdventureReturned=onoff;}

  
  int MakePokemon(void);
	void PlantKinomi(int index,int kinomino);

#if PM_DEBUG
  void DebugFull( void );
  void DebugBoxDiffCheck(void);

  void DebugSetSpeed(int speed){ mDebugSpeed = speed;};
#endif //PM_DEBUG

private:

  void _ivyTouchOnTheHiddenBeans(u8* buff);
  void _ivyTouchOnTheBeans(u8* buff);

  bool _pokemonComp(pml::pokepara::CoreParam* cp1,pml::pokepara::CoreParam* cp2);


  int _getGimLestTimeTable(int index);
  void WildPokemonArrive(void);
  bool BoxDiffCheck(void);

  int GetPokemonApperData(int index);
  int GetTalkTypeRandom(int type);
  bool setPokemonData( int nextindex ,pml::pokepara::CoreParam* cp,int i);
  
  void BeansBottleSecFunc(void);
  void PokemonEventSecFunc(void);
  void FriendShipSecFunc(void); 
  void HotSpaSecFunc(void);
  void GimSecFunc(void);
  void _gimSecFunc(void);
  void _hotSpaSecFunc(void);
  void FarmSecFunc(void);
  void _farmSecFunc(void);
  void _adventureSecFunc( int resort_index );
  void AdventureSecFunc( int resort_index );
  u8 calcBonusLevel(int boxnum);

  void _wildpokeSecFunc(void);
  void _beansIvyHiddenSecFunc(void);
  void _friendShipSecFunc(void);


  

#if PM_DEBUG
  void debugInitFunc(Savedata::ResortSave* pRresort, app::util::Heap* pHeap);
#endif //PM_DEBUG

  void InitTimeHotspaGroup(void);
  
private:

  Savedata::ResortSave* mpResortSave;
  Field::EventWork* mpEventSave;
  gfl2::heap::HeapBase* mpLogicHeap;  //メモリ
  gfl2::heap::HeapBase* mpTempHeap;   // 小物用一時ヒープ。確保したらすぐ解放するようなものにしか使わない。

  gfl2::math::Random mRandom;

  App::Resort::APP_PARAM* mpAppParam;     //初期化パラメータ

  App::Resort::wildcreate_island_binstr* mpStrn;
  App::Resort::walk_island_binstr* mpEggWalkBin;
  App::Resort::hotspa_island_binstr* mpFamiliarityBin;
  App::Resort::wildpokemon_island_binstr* mpWildPokemonBin;
  App::Resort::gim_pointup_island_binstr* mpGimPointupBin;
  App::Resort::gim_lv_island_binstr* mpGimLvBin;
  App::Resort::pokemame_island_binstr* mpPokeMameBin;
  App::Resort::beans_island_binstr* mpBeansBin;
  App::Resort::adventure_island_binstr* mpAdventureBin;
  App::Resort::adventure_itemtbl_island_binstr* mpAdventureItemTblBin;
  App::Resort::kinomi_island_binstr* mpKinomiBin;
  App::Resort::talktype_island_binstr* mpTalkTypeBin;
  App::Resort::dropbeans_island_binstr* mpDropBeansBin;

  //使っていないのでコメントアウトPokeTool::KawaigariParamCareCoreManager* mpCareCoreParamManager;
  
  u16* mpBoxLot;  //< 抽選テーブル
  int mBoxPokeCountAll;  //BOX + 運動＋温泉
  int mBoxPokeCount;   //BOXのぽけもん総数
  int mInterval;
  u8 mPreviousBounusLevel;
  
	int mFri_index;
	int mAdv_index;
	int mFar_index;

  bool mbWildPokemonUpdate;
  bool mbAdventureReturned;  //探検から戻った場合

  u32 mAdventureItemTblMax;
  
#if PM_DEBUG
  int mDebugSpeed;
#endif //PM_DEBUG

  
};

GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)


#endif // __RESORT_INIT_LOGIC_H__



