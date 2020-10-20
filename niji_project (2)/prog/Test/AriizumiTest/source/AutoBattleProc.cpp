//======================================================================
/**
 * @file AutoBattleProc.cpp
 * @date 2016/06/08 19:47:05
 * @author ariizumi_nobuhiko
 * @brief 自動バトルテストProc
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <stdio.h>

#include <macro/include/gfl2_Macros.h>
#include <Debug/DebugWin/include/DebugWinSystem.h>
#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include <fade/include/gfl2_FadeManager.h>

#include <pml/include/pokepara/pml_PokemonParamType.h>

#include <System/include/HeapDefine.h>
#include <GameSys/include/DllProc.h>
#include <GameSys/include/GameProcManager.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <Savedata/include/ConfigSave.h>
#include <Sound/include/Sound.h>

#include <Trainer/Trainer/include/tr_tool.h>
#include <Battle/include/battle_proc.h>
#include "Test/AriizumiTest/include/AutoBattleProc.h"

#include "Field/FieldStatic/include/BattleInst/BattleInstTool.h"
#include "Field/FieldStatic/include/BattleInst/BattleInstData.h"

#include "arc_def_index/arc_def.h"
#include "arc_index/battle_tree_poke.gaix"

#if  PM_DEBUG

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Ariizumi)

//コンストラクタ
AutoBattleProc::AutoBattleProc()
{
}
//デストラクタ
AutoBattleProc::~AutoBattleProc(void)
{
}

//============================================================================
//GameProc
//============================================================================
//------------------------------------------------------------------
/**
  * @brief   PROCの初期化を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事でMainFuncに進む
  */
//------------------------------------------------------------------
gfl2::proc::Result AutoBattleProc::InitFunc(gfl2::proc::Manager* pManager)
{
  mMainSeq = 0;

  mRule = 1;
  mPokeNum = 4;
  mEffectSkip = true;
  mNoDraw = false;
  mOutputLogSd = false;
  mLogUniqueNo = 0;
  mSetupRandSeed = -1;
  mBattleRandSeed = -1;

  mPokeCreateSetting = 1;
  mItemSetting = 1;
  mWazaSetting = 1;

  mTowerMode = true;
  mSpecialDebugMode = 1;  //1:NMCat4252

  mBattleNum = 0;

  gfl2::heap::HeapBase *heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
  gfl2::debug::DebugWinGroup *topGroup = gfl2::debug::DebugWin_AddGroup(heap,L"AutoBattle",NULL);

  gfl2::debug::DebugWinGroup *btlvGroup = gfl2::debug::DebugWin_AddGroup(heap,L"BtlvSetting",NULL);

  gfl2::debug::DebugWin_AddItemValueInt( topGroup ,heap, L"Rule[S,D,R] ", &mRule, 0, 2  );
  gfl2::debug::DebugWin_AddItemValueInt( topGroup ,heap, L"PokeNum", &mPokeNum, 1, 6  );
  gfl2::debug::DebugWin_AddItemValueBool( topGroup ,heap, L"EffectSkip", &mEffectSkip);

  gfl2::debug::DebugWin_AddItemValueInt( topGroup ,heap, L"PokeSelectType", &mPokeCreateSetting, 0, 2  );
  gfl2::debug::DebugWin_AddItemValueInt( topGroup ,heap, L"ItemSelectType", &mItemSetting, 0, 2  );
  gfl2::debug::DebugWin_AddItemValueInt( topGroup ,heap, L"WazaSelectType", &mWazaSetting, 0, 2  );
  gfl2::debug::DebugWin_AddItemValueBool( topGroup ,heap, L"TowerMode", &mTowerMode);

  gfl2::debug::DebugWin_AddItemValueBool( topGroup ,heap, L"TowerMode", &mTowerMode);
  gfl2::debug::DebugWin_AddItemValueInt( topGroup ,heap, L"SpecialDebug", &mSpecialDebugMode,0,10);

  gfl2::debug::DebugWin_AddItemValueInt( topGroup ,heap, L"SetupRandSeed", &mSetupRandSeed, -1, 0xFFFFFF  );
  gfl2::debug::DebugWin_AddItemValueInt( topGroup ,heap, L"BattleRandSeed", &mBattleRandSeed, -1, 0xFFFFFF  );

  gfl2::debug::DebugWin_AddItemValueBool( topGroup ,heap, L"NoDraw", &mNoDraw);
  gfl2::debug::DebugWin_AddItemValueBool( topGroup ,heap, L"OutputLogSd", &mOutputLogSd);
  gfl2::debug::DebugWin_AddItemValueInt( topGroup ,heap, L"LogSdUniqueNo", &mLogUniqueNo, 0, 9999  );

  mIsPlayBattleBgm = false;
  Sound::StartBGMReq( STRM_BGM_SILENCE );

  return gfl2::proc::RES_FINISH;
  //return gfl2::proc::RES_CONTINUE;
}

//------------------------------------------------------------------
/**
  * @brief   PROCの終了を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
  */
//------------------------------------------------------------------
gfl2::proc::Result AutoBattleProc::EndFunc(gfl2::proc::Manager* pManager)
{

  gfl2::debug::DebugWin_RemoveGroupByName(L"AutoBattle");

  return gfl2::proc::RES_FINISH;
  //return gfl2::proc::RES_CONTINUE;
}

//------------------------------------------------------------------
/**
  * @brief   PROCのメインを行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
  */
//------------------------------------------------------------------
gfl2::proc::Result AutoBattleProc::UpdateFunc(gfl2::proc::Manager* pManager)
{
  gfl2::ui::DeviceManager *uiMng = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager();
  gfl2::ui::Button *btn = uiMng->GetButton(0);

  gfl2::heap::HeapBase *heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
  GameSys::GameManager *gameMng = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  GameSys::GameData *gameData = gameMng->GetGameData();

  enum
  {
    SEQ_INPUT_WAIT,
    SEQ_START_BATTLE,
    SEQ_WAIT_BATTLE,
    SEQ_GO_NEXT,
  };

  switch( mMainSeq )
  {
  case SEQ_INPUT_WAIT:
    if( gfl2::debug::DebugWin_IsOpen() == false )
    {
      if( btn->IsTrigger( gfl2::ui::BUTTON_START ) )
      {
        return gfl2::proc::RES_FINISH;
      }

      if( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
      {
        mMainSeq = SEQ_START_BATTLE;
      }
    }
    break;

  case SEQ_START_BATTLE:
    {
      mBattleNum++;
      if( mIsPlayBattleBgm == false )
      {
        mIsPlayBattleBgm = true;
        Sound::StartBattleBGMReq( STRM_BGM_VS_BROYAL );
      }
      else
      {
        Sound::ChangeBattleBGMReq( STRM_BGM_VS_BROYAL );
      }
      //基本初期化
      BATTLE_SETUP_FIELD_SITUATION_Init( &mSit );
      BATTLE_SETUP_Init( &mSetupParam );

      int seed;
      if( mSetupRandSeed != -1 )
      {
        seed = mSetupRandSeed;
      }
      else
      {
        seed = System::GflUse::GetPublicRand();
      }
      mRandSys.Initialize(seed);

      if( mBattleRandSeed != -1 )
      {
        mSetupParam.recRandSeed = mBattleRandSeed;
      }
      else
      {
        mSetupParam.recRandSeed = mRandSys.Next();
      }

      //まずい数値の補正
      if( mRule == BTL_RULE_DOUBLE)
      {
        mPokeNum = gfl2::math::Max(mPokeNum,2);
      }
      if( mRule == BTL_RULE_ROYAL)
      {
        mPokeNum = gfl2::math::Min(mPokeNum,3);
      }

      //ポケモンの初期化
      for( int i=0;i<AB_PARTY_NUM;i++ )
      {
        mPokeParty[i] = GFL_NEW(heap) pml::PokeParty(heap);
      }

      {
        for( int i=0;i<AB_PARTY_NUM;i++ )
        {
          for( int j=0;j<mPokeNum;j++ )
          {
            POKE_CREATE_SETTING setting;
            setting.isTower = mTowerMode;
            setting.pokeMode = mPokeCreateSetting;
            setting.itemMode = mItemSetting;
            setting.wazaMode = mWazaSetting;
            mPokePara[i][j] = CreatePokeFunc(heap,heap,setting);
          }
        }
      }
      gameData->GetPlayerParty()->Clear();
      for( int i=0;i<6;i++ )
      {
        if( mPokePara[0][i] )
        {
          gameData->GetPlayerStatus()->SetPokemonParent( mPokePara[0][i], heap );
          gameData->GetPlayerParty()->AddMember( *mPokePara[0][i] );
        }
      }

      if( mOutputLogSd )
      {
        mDebugParam.mSdLogFile = GFL_NEW(heap)SDCardLog();
        mDebugParam.mSdLogFile->OpenLogFile(heap,"BattleLog",true,mLogUniqueNo);
        mDebugParam.mSdLogFile->OutputLog("StartBattle Rule[%d]\n",mRule);

        for( int i=0;i<AB_PARTY_NUM;i++ )
        {
          for( int ii=0;ii<AB_POKE_NUM;ii++ )
          {
            if(mPokePara[i][ii])
            {
              const pml::pokepara::PokemonParam *pp = mPokePara[i][ii];
              mDebugParam.mSdLogFile->OutputLog("Poke[%d][%d] ",i,ii);
              mDebugParam.mSdLogFile->OutputLog("Mons[%3d][%2d] ",pp->GetMonsNo(),pp->GetFormNo());
              mDebugParam.mSdLogFile->OutputLog("Tokusei[%3d] ",pp->GetTokuseiNo());
              mDebugParam.mSdLogFile->OutputLog("Waza[%3d][%3d][%3d][%3d]",pp->GetWazaNo(0),pp->GetWazaNo(1),pp->GetWazaNo(2),pp->GetWazaNo(3));
              mDebugParam.mSdLogFile->OutputLog("Rand[0x%8x][0x%8x] ",seed,mSetupParam.recRandSeed);
              mDebugParam.mSdLogFile->OutputLog("\n");
            }
          }
        }
      }
      GFL_RELEASE_PRINT("▼▽▼▽▼▽▼▽▼▽\n",mRule);
      GFL_RELEASE_PRINT("StartBattle Rule[%d][%d]\n",mRule,mBattleNum);
      GFL_RELEASE_PRINT("△▲△▲△▲△▲△▲\n",mRule);
      GFL_RELEASE_PRINT("Rand[0x%8x][0x%8x]\n",seed,mSetupParam.recRandSeed);
      for( int i=0;i<AB_PARTY_NUM;i++ )
      {
        for( int ii=0;ii<AB_POKE_NUM;ii++ )
        {
          if(mPokePara[i][ii])
          {
            const pml::pokepara::PokemonParam *pp = mPokePara[i][ii];
            GFL_RELEASE_PRINT("Poke[%d][%d] ",i,ii);
            GFL_RELEASE_PRINT("Mons[%3d][%2d] ",pp->GetMonsNo(),pp->GetFormNo());
            GFL_RELEASE_PRINT("Tokusei[%3d] ",pp->GetTokuseiNo());
            GFL_RELEASE_PRINT("Waza[%3d][%3d][%3d][%3d]",pp->GetWazaNo(0),pp->GetWazaNo(1),pp->GetWazaNo(2),pp->GetWazaNo(3));
            GFL_RELEASE_PRINT("Item[%3d]",pp->GetItem());
            GFL_RELEASE_PRINT("\n");
          }
        }
      }

      mSit.bgComponent.groudnType = static_cast<btl::bg::bgsys_normal_groundID>(1);
      mSit.bgComponent.nearType = static_cast<btl::bg::bgsys_near_groundID>(1);
      mSit.bgComponent.farType = static_cast<btl::bg::bgsys_far_groundID>(1);

      mSit.bgComponent.useSkyBox = true;

      //BATTLE_SETUP_Wild( &mSetupParam , gameMng , mPokeParty[1] ,&mSit , BTL_RULE_SINGLE , BATTLE_EFFECT_WILD_GRASS , NULL, heap );
      btl::setup::common_base( &mSetupParam, gameMng, &mSit );
      mSetupParam.competitor = BTL_COMPETITOR_INST;
      mSetupParam.rule = (BtlRule)mRule;
      mSetupParam.fieldSituation.bgComponent.groudnType = btl::bg::PLATE_BTLROYAL;
      mSetupParam.fieldSituation.bgComponent.nearType = btl::bg::NEAR_NONE;
      mSetupParam.fieldSituation.bgComponent.farType = btl::bg::FAR_NONE;

      int memberNum = (mRule==BTL_RULE_ROYAL?4:2);

      for( int i=0;i<memberNum;i++ )
      {
        mSetupParam.party[ i ] = GFL_NEW(heap) pml::PokeParty(heap);
      }

      mSetupParam.tr_data[ BTL_CLIENT_ROYAL2 ] = GFL_NEW(heap) BSP_TRAINER_DATA(heap);
      trainer::EncountTrainerPersonalDataMake( (trainer::TrainerID)1, mSetupParam.tr_data[ BTL_CLIENT_ROYAL2 ], heap );
      if( memberNum == 4 )
      {
        mSetupParam.tr_data[ BTL_CLIENT_ROYAL3 ] = GFL_NEW(heap) BSP_TRAINER_DATA(heap);
        trainer::EncountTrainerPersonalDataMake( (trainer::TrainerID)2, mSetupParam.tr_data[ BTL_CLIENT_ROYAL3 ], heap );
        mSetupParam.tr_data[ BTL_CLIENT_ROYAL4 ] = GFL_NEW(heap) BSP_TRAINER_DATA(heap);
        trainer::EncountTrainerPersonalDataMake( (trainer::TrainerID)6, mSetupParam.tr_data[ BTL_CLIENT_ROYAL4 ], heap );
      }

      {
        pml::PokeParty* playerParty = gameData->GetPlayerParty();
        mSetupParam.party[ BTL_CLIENT_PLAYER ]->CopyFrom( *playerParty );
        for( int i=1;i<memberNum;i++ )
        {
          mSetupParam.party[i]->Clear();
          for( int j=0;j<AB_POKE_NUM;j++ )
          {
            if( mPokePara[i][j] )
            {
              mSetupParam.party[i]->AddMember( *mPokePara[i][j] );
            }
          }
        }
      }

      //各種設定
      BTL_SETUP_SetDebugFlag(&mSetupParam,BTL_DEBUGFLAG_SKIP_BTLIN);
      if( mEffectSkip )
      {
        mSetupParam.configData->SetBattleAnimation(Savedata::ConfigSave::BATTLE_ANIMATION_UNVISIBLE);
      }
      else
      {
        mSetupParam.configData->SetBattleAnimation(Savedata::ConfigSave::BATTLE_ANIMATION_VISIBLE);
      }
      mSetupParam.configData->SetTalkingSpeed(Savedata::ConfigSave::MSG_NO_WAIT);
      mSetupParam.btlEffData.bgm_win = STRM_BGM_WIN1;
      mDebugParam.mIsAutoMsg = true;
      mDebugParam.mIsAutoTestMode = true;
      mDebugParam.mIsNoDraw = mNoDraw;

      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->ForceDefaultOut(gfl2::Fade::DISP_DOUBLE);
      mSetupParam.btlvDebugParam = mDebugParam;
      btl::BATTLE_PROC_PARAM param;
      param.pAppMemory = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
      param.pAppDeviceMemory = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
      param.pGameManager = gameMng;
      param.pSetupParam = &mSetupParam;
      GameSys::CallBattleProc(&param);

      mMainSeq = SEQ_WAIT_BATTLE;
    }
    break;
  case SEQ_WAIT_BATTLE:

    if( mDebugParam.mSdLogFile )
    {
      mDebugParam.mSdLogFile->CloseLogFile();
      GFL_SAFE_DELETE(mDebugParam.mSdLogFile);
    }

    //開放
    BATTLE_SETUP_Clear(&mSetupParam);
    for( int i=0;i<AB_PARTY_NUM;i++ )
    {
      mPokeParty[i]->Clear();
      for( int ii=0;ii<AB_POKE_NUM;ii++ )
      {
        GFL_SAFE_DELETE(mPokePara[i][ii]);
      }
      GFL_SAFE_DELETE(mPokeParty[i]);
    }
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->Reset(gfl2::Fade::DISP_DOUBLE);

    mMainSeq = SEQ_GO_NEXT;
    break;
  case SEQ_GO_NEXT:
    if( gfl2::debug::DebugWin_IsOpen() )
    {
      mMainSeq = SEQ_INPUT_WAIT;
    }
    else
    {
      mMainSeq = SEQ_START_BATTLE;
    }
    break;

  }

  return gfl2::proc::RES_CONTINUE;
}

/**
* @brief	PROCの描画処理(下準備)
* @param pManager   プロセス管理マネージャー
* 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
* 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
*/
void AutoBattleProc::PreDrawFunc( gfl2::proc::Manager* pManager )
{
}

/**
* @brief	PROCの描画処理
* @param pManager   プロセス管理マネージャー
* @param displayNo  描画対象画面
* 登録したPROCによる、フレームバッファへの描画を実行します。
* ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
*/
void AutoBattleProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
}

typedef struct  
{
  MonsNo monsNo;
  FormNo formNo;
}ABT_MONS_DATA;
#include "AutoBattleProcData.cdat"

pml::pokepara::PokemonParam* CreatePokeFunc(gfl2::heap::HeapBase *pokeHeap,gfl2::heap::HeapBase *workHeap,const POKE_CREATE_SETTING &setting)
{
  pml::pokepara::PokemonParam *pp;

  const ABT_MONS_DATA *monsArr;
  const int *itemArr,*wazaArr;
  int monsNum,itemNum,wazaNum;

  if( setting.isTower == true )
  {
    BattleInst::BINST_POKEMON_ROM_DATA  pd;
    pd.mons_no = 0;
    /// 空データ判定
    while( pd.mons_no == 0 )
    {
      int idx = System::GflUse::GetPublicRand(GARC_battle_tree_poke_DATA_NUM);
      BattleInstTool::GetPokemonRomData(workHeap, GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager(), ARCID_BATTLE_TREE_POKE, idx, &pd);
    }
    return BattleInstTool::MakePokemonParam( &pd, 50, (0x1f*7)/8, workHeap );//特性, 性別指定しない
  }

  if( setting.pokeMode == 1 )
  {
    monsArr = checkMonsData;
    monsNum = GFL_NELEMS(checkMonsData);
  }
  else
  {
    monsArr = allMonsData;
    monsNum = GFL_NELEMS(allMonsData);
  }
  if( setting.itemMode == 1 )
  {
    itemArr = checkItemData;
    itemNum = GFL_NELEMS(checkItemData);
  }
  else
  {
    itemArr = allItemData;
    itemNum = GFL_NELEMS(allItemData);
  } 
  if( setting.wazaMode == 1 )
  {
    wazaArr = checkWazaData;
    wazaNum = GFL_NELEMS(checkWazaData);
  }
  else
  {
    wazaArr = allWazaData;
    wazaNum = GFL_NELEMS(allWazaData);
  }

  
  u32 pokeIdx = System::GflUse::GetPublicRand(monsNum);
  u32 itemIdx = System::GflUse::GetPublicRand(itemNum);
  u16 wazaIdx[4];
  int idx=0;
  while( idx<4 )
  {
    bool isSame = false;
    wazaIdx[idx] = System::GflUse::GetPublicRand(wazaNum);
    for( int j=0;j<idx;j++ )
    {
      if( wazaIdx[idx] == wazaIdx[j] )
      {
        isSame = true;
        break;
      }
    }
    if( isSame == false )
    {
      idx++;
    }
  }
  pml::pokepara::InitialSpec spec;
  spec.monsno = monsArr[pokeIdx].monsNo;
  spec.formno = monsArr[pokeIdx].formNo;
  spec.level  = 50;
  spec.sex = System::GflUse::GetPublicRand(2);
  spec.seikaku = System::GflUse::GetPublicRand(24);
  spec.tokusei_index = System::GflUse::GetPublicRand(3);
  for( int i=0;i<pml::pokepara::POWER_NUM;i++ )
  {
    spec.talentPower[i] = 0;
  }

  pp = GFL_NEW( pokeHeap ) pml::pokepara::PokemonParam( pokeHeap, spec );

  for( int i=0;i<4;i++ )
  {
    pp->SetWaza(i,(WazaNo)wazaArr[wazaIdx[i]]);
  }
  pp->SetItem(itemArr[itemIdx]);

  return pp;
}


//STRCORE→sjis変換
typedef struct 
{
  int  strcode;
  char *sjiscode;
}STR_CONV_DATA;

//#include "niji_conv_header/test/ariizumi/strcode_to_sjis_table.cdat"
static const STR_CONV_DATA convDataArr[] =
{
  {L'あ',"あ"},	{L'い',"い"},	{L'う',"う"},	{L'え',"え"},	{L'お',"お"},	
  {L'か',"か"},	{L'き',"き"},	{L'く',"く"},	{L'け',"け"},	{L'こ',"こ"},	
  {L'さ',"さ"},	{L'し',"し"},	{L'す',"す"},	{L'せ',"せ"},	{L'そ',"そ"},	
  {L'た',"た"},	{L'ち',"ち"},	{L'つ',"つ"},	{L'て',"て"},	{L'と',"と"},	
  {L'な',"な"},	{L'に',"に"},	{L'ぬ',"ぬ"},	{L'ね',"ね"},	{L'の',"の"},	
  {L'は',"は"},	{L'ひ',"ひ"},	{L'ふ',"ふ"},	{L'へ',"へ"},	{L'ほ',"ほ"},	
  {L'ま',"ま"},	{L'み',"み"},	{L'む',"む"},	{L'め',"め"},	{L'も',"も"},	
  {L'や',"や"},	{L'ゆ',"ゆ"},	{L'よ',"よ"},	{L'ら',"ら"},	{L'り',"り"},	
  {L'る',"る"},	{L'れ',"れ"},	{L'ろ',"ろ"},	{L'わ',"わ"},	{L'を',"を"},	
  {L'ん',"ん"},	{L'が',"が"},	{L'ぎ',"ぎ"},	{L'ぐ',"ぐ"},	{L'げ',"げ"},	
  {L'ご',"ご"},	{L'ざ',"ざ"},	{L'じ',"じ"},	{L'ず',"ず"},	{L'ぜ',"ぜ"},	
  {L'ぞ',"ぞ"},	{L'だ',"だ"},	{L'ぢ',"ぢ"},	{L'づ',"づ"},	{L'で',"で"},	
  {L'ど',"ど"},	{L'ば',"ば"},	{L'び',"び"},	{L'ぶ',"ぶ"},	{L'べ',"べ"},	
  {L'ぼ',"ぼ"},	{L'ぱ',"ぱ"},	{L'ぴ',"ぴ"},	{L'ぷ',"ぷ"},	{L'ぺ',"ぺ"},	
  {L'ぽ',"ぽ"},	{L'ぁ',"ぁ"},	{L'ぃ',"ぃ"},	{L'ぅ',"ぅ"},	{L'ぇ',"ぇ"},	
  {L'ぉ',"ぉ"},	{L'っ',"っ"},	{L'ゃ',"ゃ"},	{L'ゅ',"ゅ"},	{L'ょ',"ょ"},	
  {L'ア',"ア"},	{L'イ',"イ"},	{L'ウ',"ウ"},	{L'エ',"エ"},	{L'オ',"オ"},	
  {L'カ',"カ"},	{L'キ',"キ"},	{L'ク',"ク"},	{L'ケ',"ケ"},	{L'コ',"コ"},	
  {L'サ',"サ"},	{L'シ',"シ"},	{L'ス',"ス"},	{L'セ',"セ"},	{L'ソ',"ソ"},	
  {L'タ',"タ"},	{L'チ',"チ"},	{L'ツ',"ツ"},	{L'テ',"テ"},	{L'ト',"ト"},	
  {L'ナ',"ナ"},	{L'ニ',"ニ"},	{L'ヌ',"ヌ"},	{L'ネ',"ネ"},	{L'ノ',"ノ"},	
  {L'ハ',"ハ"},	{L'ヒ',"ヒ"},	{L'フ',"フ"},	{L'ヘ',"ヘ"},	{L'ホ',"ホ"},	
  {L'マ',"マ"},	{L'ミ',"ミ"},	{L'ム',"ム"},	{L'メ',"メ"},	{L'モ',"モ"},	
  {L'ヤ',"ヤ"},	{L'ユ',"ユ"},	{L'ヨ',"ヨ"},	{L'ラ',"ラ"},	{L'リ',"リ"},	
  {L'ル',"ル"},	{L'レ',"レ"},	{L'ロ',"ロ"},	{L'ワ',"ワ"},	{L'ヲ',"ヲ"},	
  {L'ン',"ン"},	{L'ガ',"ガ"},	{L'ギ',"ギ"},	{L'グ',"グ"},	{L'ゲ',"ゲ"},	
  {L'ゴ',"ゴ"},	{L'ザ',"ザ"},	{L'ジ',"ジ"},	{L'ズ',"ズ"},	{L'ゼ',"ゼ"},	
  {L'ゾ',"ゾ"},	{L'ダ',"ダ"},	{L'ヂ',"ヂ"},	{L'ヅ',"ヅ"},	{L'デ',"デ"},	
  {L'ド',"ド"},	{L'バ',"バ"},	{L'ビ',"ビ"},	{L'ブ',"ブ"},	{L'ベ',"ベ"},	
  {L'ボ',"ボ"},	{L'パ',"パ"},	{L'ピ',"ピ"},	{L'プ',"プ"},	{L'ペ',"ペ"},	
  {L'ポ',"ポ"},	{L'ァ',"ァ"},	{L'ィ',"ィ"},	{L'ゥ',"ゥ"},	{L'ェ',"ェ"},	
  {L'ォ',"ォ"},	{L'ッ',"ッ"},	{L'ャ',"ャ"},	{L'ュ',"ュ"},	{L'ョ',"ョ"},	
  {L'１',"１"},	{L'２',"２"},	{L'３',"３"},	{L'４',"４"},	{L'５',"５"},	
  {L'６',"６"},	{L'７',"７"},	{L'８',"８"},	{L'９',"９"},	{L'０',"０"},	
  {L'1',"1"},	{L'2',"2"},	{L'3',"3"},	{L'4',"4"},	{L'5',"5"},	
  {L'6',"6"},	{L'7',"7"},	{L'8',"8"},	{L'9',"9"},	{L'0',"0"},	
  {L'、',"、"},	{L'。',"。"},	{L'…',"…"},	{L'・',"・"},	{L'！',"！"},	
  {L'？',"？"},	{L'ー',"ー"},	
  
  {L'　',"　"},	  //必ず最後(不明文字)
};

void DebugStrcodeToUtf8(const gfl2::str::STRCODE *buf,char *ret)
{
  int idx = 0;
  int retIdx = 0;
  while( buf[idx] != '\0' )
  {
    bool isFound = false;
    for(int i=0;i<GFL_NELEMS(convDataArr);i++ )
    {
      if( convDataArr[i].strcode == buf[idx] )
      {
        int codePos = 0;
        while( convDataArr[i].sjiscode[codePos] != '\0' )
        {
          ret[retIdx] = convDataArr[i].sjiscode[codePos];
          retIdx++;
          codePos++;
        }
        isFound = true;
        //GFL_PRINT("%s",convDataArr[i].sjiscode);
        break;
      }
    }
    if( isFound == false )
    {
      int lastIdx = GFL_NELEMS(convDataArr)-1;
      int codePos = 0;
      while( convDataArr[lastIdx].sjiscode[codePos] != '\0' )
      {
        ret[retIdx] = convDataArr[lastIdx].sjiscode[codePos];
        retIdx++;
        codePos++;
      }
      //GFL_PRINT("？");
    }
    idx++;
  }
  ret[retIdx] = '\0';
}

void SDCardLog::OpenLogFile(gfl2::heap::HeapBase *heap, char *name,bool addTime,int uniqueNo)
{
  if( mFile == NULL )
  {
    const size_t bufSize = 256;
    char fileName[bufSize];
    gfl2::system::Date date;
    gfl2::system::Date::GetNow(&date);

#if defined(GF_PLATFORM_WIN32)
    sprintf_s( fileName,bufSize,"%s",name );
#elif defined(GF_PLATFORM_CTR)
    sprintf( fileName,"%s",name );
#endif

    if( uniqueNo != -1 )
    {
#if defined(GF_PLATFORM_WIN32)
      sprintf_s( fileName,bufSize,"%d_",fileName,uniqueNo );
#elif defined(GF_PLATFORM_CTR)
      sprintf( fileName,"%d_",fileName,uniqueNo );
#endif
    }
    if( addTime )
    {

#if defined(GF_PLATFORM_WIN32)
      sprintf_s( fileName,bufSize,"%s_%04d%02d%02d_%02d%02d%02d" ,fileName
        ,date.GetYear() ,date.GetMonth() ,date.GetDay()
        ,date.GetHour() ,date.GetMinute() ,date.GetSecond() );
#elif defined(GF_PLATFORM_CTR)
      sprintf( fileName,"%s_%04d%02d%02d_%02d%02d%02d" ,fileName
        ,date.GetYear() ,date.GetMonth() ,date.GetDay()
        ,date.GetHour() ,date.GetMinute() ,date.GetSecond() );
#endif
    }
#if defined(GF_PLATFORM_WIN32)
    sprintf_s( fileName,bufSize,"%s.log",fileName );
#elif defined(GF_PLATFORM_CTR)
    sprintf( fileName,"%s.log",fileName );
#endif

    mFile = gfl2::fs::System::CreateDirectSdmcFile(heap,fileName,gfl2::fs::Define::OpenMode::WRITE);

    OutputLog("StartLog [%04d%02d%02d_%02d%02d%02d]\n"
      ,date.GetYear() ,date.GetMonth() ,date.GetDay()
      ,date.GetHour() ,date.GetMinute() ,date.GetSecond() );
  }
}

void SDCardLog::CloseLogFile(void)
{
  if( mFile )
  {
    gfl2::system::Date date;
    gfl2::system::Date::GetNow(&date);

    OutputLog("EndLog [%04d%02d%02d_%02d%02d%02d]\n"
      ,date.GetYear() ,date.GetMonth() ,date.GetDay()
      ,date.GetHour() ,date.GetMinute() ,date.GetSecond() );

    gfl2::fs::System::DestroyDirectSdmcFile(mFile);
    mFile = NULL;
  }
}

void SDCardLog::OutputLog(const char* fmt , ... )
{
  if( mFile )
  {
    const size_t bufSize = 512;
    va_list	argp;
    char pszBuf[bufSize];
    va_start(argp, fmt);
#if defined(GF_PLATFORM_WIN32)
    vsprintf_s( pszBuf, bufSize, fmt, argp);
#elif defined(GF_PLATFORM_CTR)
    vsprintf( pszBuf, fmt, argp);
#endif
    va_end(argp);

    int len = strlen(pszBuf);

    mFile->Write(pszBuf,len);

  }
}

GFL_NAMESPACE_END(Ariizumi)
GFL_NAMESPACE_END(Test)

#endif  //PM_DEBUG
