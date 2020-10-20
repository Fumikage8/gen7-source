//======================================================================
/**
 * @file  d_ariizumi.cpp
 * @brief 15/02/25
 * @author  ariizumi
 * @data  有泉テストメニュー
 */
//======================================================================
#if PM_DEBUG
#include <Debug/StartMenu/include/StartLauncher.h>
#include <fade/include/gfl2_FadeManager.h>

#include <arc_index/debug_message.gaix>
#include <arc_index/message.gaix>
#include <niji_conv_header/message/debug/msg_d_ariizumi.h>
#include <niji_conv_header/message/debug/msg_d_btlbgviewer.h>
#include <niji_conv_header/message/debug/msg_debug_menu.h>              // For kawaigari debug
#include <niji_conv_header/trainer/trid_def.h>
#include <niji_conv_header/battle/background/bgsys_normal_ground_def.h>
#include <niji_conv_header/battle/background/bgsys_near_ground_def.h>
#include <niji_conv_header/battle/background/bgsys_far_ground_def.h>
//#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include <Debug/DebugWin/include/DebugWinSystem.h>

#include <Debug/StartMenu/include/d_ariizumi.h>

#include <System/include/HeapDefine.h>
#include <System/include/GflUse.h>
#include <GameSys/include/DllProc.h>
#include <GameSys/include/GameProcManager.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include "SaveData/include/MyStatus.h"
#include <Print/include/PrintSystem.h>
#include <Trainer/Trainer/include/tr_tool.h>
#include <Trainer/Trainer/include/TrainerTypeData.h>
#include <Savedata/include/MyStatus.h>

#include <Battle/include/battle_proc.h>
#include <Test/AriizumiTest/include/EffectViewer.h>
#include <pml/include/pokepara/pml_PokemonParamType.h>

#include <App/Kawaigari/include/KawaigariProc.h>


GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(startmenu)

// 項目文字列データテーブル
const u32 AriTestLauncher::ItemList[AriTestLauncher::LIST_MAX] =
{
  AriDebugMenuItem_01,
  AriDebugMenuItem_02,
  AriDebugMenuItem_03,
  AriDebugMenuItem_04,
  AriDebugMenuItem_05,
  AriDebugMenuItem_06,
  AriDebugMenuItem_07,
  AriDebugMenuItem_08,
  AriDebugMenuItem_09,
  AriDebugMenuItem_10,
  AriDebugMenuItem_11,
  AriDebugMenuItem_12,
  AriDebugMenuItem_13,
  AriDebugMenuItem_14,
  AriDebugMenuItem_15,
  AriDebugMenuItem_16,
  AriDebugMenuItem_17,
  AriDebugMenuItem_18,
  AriDebugMenuItem_19,
  AriDebugMenuItem_20,
  AriDebugMenuItem_21,
  AriDebugMenuItem_22,
  AriDebugMenuItem_23,
  AriDebugMenuItem_24,
};


extern void AriTestZenryokuCheckCallBack( void *work );

extern void AriTest_Update_SearchTrainerId( void* userWork  , gfl2::debug::DebugWinItem *item );

//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//-----------------------------------------------------------------------------
AriTestLauncher::AriTestLauncher(void)
{
  // debug_menuの文字列データ
  ::Debug::DebugWin::DebugWinWork* devWork = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  gfl2::str::MsgData* pMsgData = devWork->GetMessageData();


  gfl2::heap::HeapBase *heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
  gfl2::debug::DebugWinGroup *topGroup = gfl2::debug::DebugWin_AddGroup(heap,L"TestAri",NULL);
  gfl2::debug::DebugWinGroup *bgGroup = gfl2::debug::DebugWin_AddGroup(heap,L"BGSetting",topGroup);

  // For kawaigari
  mpKawaigariMenuTop = gfl2::debug::DebugWin_AddGroup(heap, pMsgData, msg_dmenu_kawaigari_menuitem_18, NULL);


  //gfl2::debug::DebugWinItem* item;

  InitializeBattleDebugParam(&mDebugParam);

  mEffViewMonsNo = MONSNO_GARAGARA;
  mTrainerId[0] = trainer::TRID_MINI_01;
  mTrainerId[1] = trainer::TRID_TANPAN_01;
  mTrainerId[2] = trainer::TRID_MINI_02;
  mSkipEncountEffect = false;
  mRideWareType = 0;
#if defined(DEBUG_ONLY_FOR_ariizumi_nobuhiko)
  mSkipEncountEffect = true;
#endif

  mBgGround = btl::bg::PLATE_GRASS;
  mBgNear = btl::bg::NEAR_GRASS;
  mBgFar = btl::bg::FAR_MOUNT;
  mBgSkyBox = true;
  mBgApplyEffView = false;

  mSelfPoke = 25;
  mSelfPokeForm = 0;
  mEnemyPoke = 1;
  mEnemyPokeForm = 0;
  mEnemyPokeNum = 2;

  mZenryokuChech = -1;
  mTrainerNoBall = false;
  
  gfl2::debug::DebugWin_AddItemValueInt(topGroup,heap,L"HioEff",&mDebugParam.isHioEffResource,0,1,1);
  gfl2::debug::DebugWin_AddItemValueInt(topGroup,heap,L"EffViewMonsNo",&mEffViewMonsNo,0,MONSNO_MAX,1);
  gfl2::debug::DebugWin_AddItemValueBool(topGroup,heap,L"SkipEncount",&mSkipEncountEffect);

  gfl2::debug::DebugWin_AddItemValueIntMsg(topGroup,heap,L"TrainerType1",&mDebugParam.mTrainerType[0],print::GetMessageArcId_Debug(),GARC_debug_message_d_ariizumi_DAT,-2,292,AriDebug_CharaID000);
  gfl2::debug::DebugWin_AddItemValueIntMsg(topGroup,heap,L"TrainerType2",&mDebugParam.mTrainerType[1],print::GetMessageArcId_Debug(),GARC_debug_message_d_ariizumi_DAT,-2,292,AriDebug_CharaID000);
  gfl2::debug::DebugWin_AddItemValueIntMsg(topGroup,heap,L"TrainerType3",&mDebugParam.mTrainerType[2],print::GetMessageArcId_Debug(),GARC_debug_message_d_ariizumi_DAT,-2,292,AriDebug_CharaID000);
  gfl2::debug::DebugWin_AddItemValueIntMsg(topGroup,heap,L"TrainerType4",&mDebugParam.mTrainerType[3],print::GetMessageArcId_Debug(),GARC_debug_message_d_ariizumi_DAT,-2,292,AriDebug_CharaID000);

  gfl2::debug::DebugWin_AddItemUpdateFunc( topGroup,heap,L"SearchTrainerID",this, AriTest_Update_SearchTrainerId);

  gfl2::debug::DebugWin_AddItemValueIntMsg(topGroup,heap,L"TrId2",&mTrainerId[0],print::GetMessageArcId(),GARC_message_trname_DAT, 0,trainer::TRID_MAX-1);
  gfl2::debug::DebugWin_AddItemValueIntMsg(topGroup,heap,L"TrId3",&mTrainerId[1],print::GetMessageArcId(),GARC_message_trname_DAT, 0,trainer::TRID_MAX-1);
  gfl2::debug::DebugWin_AddItemValueIntMsg(topGroup,heap,L"TrId4",&mTrainerId[2],print::GetMessageArcId(),GARC_message_trname_DAT, 0,trainer::TRID_MAX-1);

  gfl2::debug::DebugWin_AddItemValueInt(topGroup,heap,L"ThrowType",&mDebugParam.mTrainerThrowType,-1,8,1);
  gfl2::debug::DebugWin_AddItemValueInt(topGroup,heap,L"Ride(None:Land:Sky)",&mRideWareType,0,2,1);
  gfl2::debug::DebugWin_AddItemValueInt(topGroup,heap,L"SeqFakeComm",&mDebugParam.mSeqOptFakeComm,-1,1,1);

  gfl2::debug::DebugWin_AddItemValueIntMsg(topGroup,heap,L"SelfPoke",&mSelfPoke,print::GetMessageArcId(),GARC_message_monsname_DAT, 1, (int)MONSNO_END );
  gfl2::debug::DebugWin_AddItemValueInt(topGroup,heap,L"SelfPokeForm",&mSelfPokeForm,0,30,1);
  gfl2::debug::DebugWin_AddItemValueIntMsg(topGroup,heap,L"EnemyPoke",&mEnemyPoke,print::GetMessageArcId(),GARC_message_monsname_DAT, 1, (int)MONSNO_END );
  gfl2::debug::DebugWin_AddItemValueInt(topGroup,heap,L"EnemyPokeForm",&mEnemyPokeForm,0,30,1);
  gfl2::debug::DebugWin_AddItemValueInt(topGroup,heap,L"EnemyNum",&mEnemyPokeNum,1,6,1);

  gfl2::debug::DebugWin_AddItemValueBool(topGroup,heap,L"IsLastBoss",&mIsLastBoss);
  gfl2::debug::DebugWin_AddItemValueBool(topGroup, heap, L"ForceRareEffect", &mDebugParam.mForceRareEffect);
  gfl2::debug::DebugWin_AddItemValueBool(topGroup, heap, L"TrainerNoBall", &mTrainerNoBall);

  gfl2::debug::DebugWinItem* item;
  item = gfl2::debug::DebugWin_AddItemValueInt(topGroup,heap,L"ZenryokuCheck",&mZenryokuChech,-1,40,1);
  item->SetChangedValueFunc(AriTestZenryokuCheckCallBack,this);
  gfl2::debug::DebugWin_AddItemValueBool(topGroup,heap,L"AICtrl",&mAiCtrl);

  gfl2::debug::DebugWin_AddItemValueInt(bgGroup,heap,L"HioBg",&mDebugParam.isHioBgResource,0,1,1);
  gfl2::debug::DebugWin_AddItemValueIntMsg(bgGroup,heap,L"Ground",&mBgGround,print::GetMessageArcId_Debug(),GARC_debug_message_d_btlbgviewer_DAT,0,btl::bg::BGSYS_NORMAL_GROUND_MAX-1,D_BTLBGVIEWER_G_00);
  gfl2::debug::DebugWin_AddItemValueIntMsg(bgGroup,heap,L"Near",&mBgNear,print::GetMessageArcId_Debug(),GARC_debug_message_d_btlbgviewer_DAT,0,btl::bg::BGSYS_NEAR_GROUND_MAX-1,D_BTLBGVIEWER_N_00);
  gfl2::debug::DebugWin_AddItemValueIntMsg(bgGroup,heap,L"Far",&mBgFar,print::GetMessageArcId_Debug(),GARC_debug_message_d_btlbgviewer_DAT,0,btl::bg::BGSYS_FAR_GROUND_MAX-1,D_BTLBGVIEWER_F_00);
  gfl2::debug::DebugWin_AddItemValueBool(bgGroup, heap, L"SkyBox", &mBgSkyBox);
  gfl2::debug::DebugWin_AddItemValueBool(bgGroup, heap, L"ApplyEffView", &mBgApplyEffView);

  gfl2::debug::DebugWin_AddItemValueInt(bgGroup,heap,L"Party[1:size 2:special]",&mBgPartySet,0,2,1);
  gfl2::debug::DebugWin_AddItemValueBool(bgGroup,heap,L"Muteki",&mMuteki);

  


  // For kawaigari
  InitializeKawaigariParam( heap );



  // Pokemon 
  gfl2::debug::DebugWinGroup *kawaSetupPoke = gfl2::debug::DebugWin_AddGroup(heap,pMsgData,msg_dmenu_kawaigari_menuitem_1, mpKawaigariMenuTop );
  gfl2::debug::DebugWin_AddItemValueIntMsg( kawaSetupPoke ,heap, pMsgData, msg_dmenu_kawaigari_menuitem_2, &mKawaigariDebugParam.Mons_no,print::GetMessageArcId(),GARC_message_monsname_DAT, 1, (int)MONSNO_END );
  gfl2::debug::DebugWin_AddItemValueInt( kawaSetupPoke ,heap, pMsgData,msg_dmenu_kawaigari_menuitem_3, &mKawaigariDebugParam.Form_no, 0, 255 );
  gfl2::debug::DebugWin_AddItemValueInt( kawaSetupPoke ,heap, pMsgData, msg_dmenu_kawaigari_menuitem_4, &mKawaigariDebugParam.Sex, (int)pml::SEX_MALE, (int)pml::SEX_UNKNOWN );
  gfl2::debug::DebugWin_AddItemValueBool( kawaSetupPoke ,heap, pMsgData, msg_dmenu_kawaigari_menuitem_5, &mKawaigariDebugParam.Rare );

  // Status
  gfl2::debug::DebugWinGroup *kawaSetupStatus = gfl2::debug::DebugWin_AddGroup(heap,pMsgData, msg_dmenu_kawaigari_menuitem_10, mpKawaigariMenuTop );
  gfl2::debug::DebugWin_AddItemValueIntMsg( kawaSetupStatus ,heap, pMsgData, msg_dmenu_kawaigari_menuitem_11, &mKawaigariDebugParam.Bad_status, print::GetMessageArcId_Debug(), GARC_debug_message_debug_menu_DAT, 0, 5, msg_dmenu_kawaigari_bad_status_1 );
  gfl2::debug::DebugWin_AddItemValueU8( kawaSetupStatus ,heap, pMsgData, msg_dmenu_kawaigari_menuitem_7, &mKawaigariDebugParam.Nakayoshi, 0, 255  );
  gfl2::debug::DebugWin_AddItemValueU8( kawaSetupStatus ,heap, pMsgData, msg_dmenu_kawaigari_menuitem_8, &mKawaigariDebugParam.Manpuku, 0, 255  );
  gfl2::debug::DebugWin_AddItemValueU8( kawaSetupStatus ,heap, pMsgData, msg_dmenu_kawaigari_menuitem_9, &mKawaigariDebugParam.Nadenade, 0, 255  );

  // Dirt
  gfl2::debug::DebugWinGroup *kawaDirt = gfl2::debug::DebugWin_AddGroup( heap,pMsgData, msg_dmenu_kawaigari_dirty_type_2, mpKawaigariMenuTop );
  gfl2::debug::DebugWin_AddItemValueIntMsg( kawaDirt ,heap, pMsgData, msg_dmenu_kawaigari_dirty_type_2, &mKawaigariDebugParam.DirtType, print::GetMessageArcId_Debug(), GARC_debug_message_debug_menu_DAT, 0, 4, msg_dmenu_kawaigari_dirty_type_1 );
  gfl2::debug::DebugWin_AddItemValueInt( kawaDirt ,heap, L"Number : ", &mKawaigariDebugParam.DirtNum, 1, 5  );
  //  gfl2::debug::DebugWin_AddItemUpdateFunc( kawaDirt, pHeap,L"Set",this,KawaigariMain::SelectMenuFookFunc_SetDirt );
  //  gfl2::debug::DebugWin_AddItemUpdateFunc( kawaDirt, pHeap,L"All Set",this,KawaigariMain::SelectMenuFookFunc_SetDirtAll );


  // Back Ground
  gfl2::debug::DebugWinGroup *kawaSetupBG = gfl2::debug::DebugWin_AddGroup(heap,pMsgData, msg_dmenu_kawaigari_menuitem_12, mpKawaigariMenuTop );
  gfl2::debug::DebugWin_AddItemValueIntMsg(kawaSetupBG,heap,pMsgData, msg_dmenu_kawaigari_menuitem_13, &mKawaigariDebugParam.BgGround,print::GetMessageArcId_Debug(),GARC_debug_message_d_btlbgviewer_DAT,0,btl::bg::BGSYS_NORMAL_GROUND_MAX-1,D_BTLBGVIEWER_G_00);
  gfl2::debug::DebugWin_AddItemValueIntMsg(kawaSetupBG,heap,pMsgData, msg_dmenu_kawaigari_menuitem_14, &mKawaigariDebugParam.BgNear,print::GetMessageArcId_Debug(),GARC_debug_message_d_btlbgviewer_DAT,0,btl::bg::BGSYS_NEAR_GROUND_MAX-1,D_BTLBGVIEWER_N_00);
  gfl2::debug::DebugWin_AddItemValueIntMsg(kawaSetupBG,heap,pMsgData, msg_dmenu_kawaigari_menuitem_15, &mKawaigariDebugParam.BgFar,print::GetMessageArcId_Debug(),GARC_debug_message_d_btlbgviewer_DAT,0,btl::bg::BGSYS_FAR_GROUND_MAX-1,D_BTLBGVIEWER_F_00);
  gfl2::debug::DebugWin_AddItemValueBool(kawaSetupBG,heap,pMsgData, msg_dmenu_kawaigari_menuitem_16,&mKawaigariDebugParam.BgSkyBox);

}

void AriTestZenryokuCheckCallBack( void* work )
{
  AriTestLauncher *proc = static_cast<AriTestLauncher*>(work);
  if( proc->mZenryokuChech == -1 )
  {
    gfl2::debug::ReleasePrintCore("ZenryokuCheck:OFF\n");
  }
  else
  {
    char *name[41] = 
    {
      "ノーマル",
      "かくとう",
      "ひこう",
      "どく",
      "じめん",
      "いわ",
      "むし",
      "ゴースと",
      "はがね",
      "ほのお",
      "みず",
      "くさ",
      "でんき",
      "エスパー",
      "こおり",
      "ドラゴン",
      "あく",
      "フェアリー",
      "ジュナイパー",
      "ガオガエン",
      "アシレーヌ",
      "ピカチュウ",
      "アロライ",
      "ティキ",
      "イーブイ",
      "カビゴン",
      "ミュウ",
      "サトピカ",
      "マーシャドー",
      "ミミッキュ",
      "ミミッキュ(ぬし",
      "ジャラランガ",
      "ジャラランガ(ぬし",
      "ルガルガン(ひる",
      "ルガルガン(よる",
      "ルガルガン(ゆうぐれ",
      "ルナ",
      "Ｎルナ",
      "ソル",
      "Ｎソル",
      "ヒカリドラゴン ",
    };
    gfl2::debug::ReleasePrintCore("ZenryokuCheck:%s\n",name[proc->mZenryokuChech]);
  }
}

void AriTest_Update_SearchTrainerId( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  AriTestLauncher *proc = static_cast<AriTestLauncher*>(userWork);
  gfl2::heap::HeapBase *heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
  
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    trainer::TRAINER_DATA trdata;
    trainer::TrainerTypeData trTypeData;
    bool isHit = false;
    for( int i=0;i<trainer::TRID_MAX;i++ )
    {
      //現在IDから調べるため
      int trid = (i+proc->mTrainerId[0]+1)%(trainer::TRID_MAX-1);
      if( trid != trainer::TRID_NULL )
      {
        trainer::LoadTrainerData(heap,trainer::TrainerID(trid),&trdata);
        trTypeData.LoadData(heap,trainer::TrType(trdata.tr_type));
        //GFL_PRINT("Search[ID:%d][Type:%d] = [%d]\n",trid,trdata.tr_type,proc->mDebugParam.mTrainerType[1]);
        if( trTypeData.GetBattleMainModelID() == proc->mDebugParam.mTrainerType[1] )
        {
          proc->mTrainerId[0] = trid;
          break;
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
AriTestLauncher::~AriTestLauncher()
{
  ClearBattleParam();
  ClearEffviewParam();
  ClearKawaigariParam();

  gfl2::debug::DebugWin_RemoveGroupByName(L"TestAri");
  gfl2::debug::DebugWin_RemoveGroup( mpKawaigariMenuTop );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetArcDataID
 * @brief   アークデータID取得
 * @date    2015.02.18
 *
 * @return  アークデータID
 */
//-----------------------------------------------------------------------------
gfl2::fs::ArcFile::ARCDATID AriTestLauncher::GetArcDataID(void)
{
  return GARC_debug_message_d_ariizumi_DAT;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetListTable
 * @brief   項目文字列データテーブル取得
 * @date    2015.02.18
 *
 * @return  文字列データテーブル
 */
//-----------------------------------------------------------------------------
const u32 * AriTestLauncher::GetListTable(void)
{
  return ItemList;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetListTable
 * @brief   項目文字列データテーブルの要素数取得
 * @date    2015.02.18
 *
 * @return  文字列データテーブルの要素数
 */
//-----------------------------------------------------------------------------
u32 AriTestLauncher::GetListTableSize(void)
{
  return LIST_MAX;
}

//-----------------------------------------------------------------------------
/**
 * @func    SelectListener
 * @brief   決定時のリスナー
 * @date    2015.02.18
 *
 * @param   pos       カーソル位置 ( キャンセル時はCURSOR_POS_CANCEL )
 * @param   launcher  呼び出し元のランチャークラス
 *
 * @return  ランチャーへのリクエストID
 */
//-----------------------------------------------------------------------------
debug::launcher::ListData::RequestID AriTestLauncher::SelectListener( u32 pos, debug::launcher::LauncherProc * launcher )
{
  // キャンセル
  if( pos == debug::launcher::LauncherProc::CURSOR_POS_CANCEL )
  {
    launcher->CreateListData<debug::startmenu::StartLauncher>();
    return debug::launcher::ListData::REQID_RESET;
  }

  switch( ItemList[pos] )
  {
    //単発系
  case AriDebugMenuItem_10:
    GameSys::CallAriTestPokeDrawProc();
    break;
  case AriDebugMenuItem_11:
    GameSys::CallAriTestDrawProc();
    break;
  case AriDebugMenuItem_12:
    {
      gfl2::heap::HeapBase *heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
      Test::Ariizumi::EffectViewerProc *proc = GameSys::CallEffectViewerProc();
#if PM_DEBUG
      ClearEffviewParam();
      for( int i=0;i<4;i++ )
      {
        mDebugParam.pp[i] = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, (MonsNo)mEffViewMonsNo, 15, 0 );
      }
      mDebugParam.mEnebleBgSetting = mBgApplyEffView;
      mDebugParam.mBgComponent.groudnType = static_cast<btl::bg::bgsys_normal_groundID>(mBgGround);
      mDebugParam.mBgComponent.nearType = static_cast<btl::bg::bgsys_near_groundID>(mBgNear);
      mDebugParam.mBgComponent.farType = static_cast<btl::bg::bgsys_far_groundID>(mBgFar);
      mDebugParam.mBgComponent.useSkyBox = mBgSkyBox;

      proc->SetDebugParam(&mDebugParam);
#endif //PM_DEBUG
    }
    break;
  case AriDebugMenuItem_13:
    GameSys::CallBackgroundViewerProc();
    break;
  case AriDebugMenuItem_14:
    GameSys::CallBattleMeasureProc();
    break;
  case AriDebugMenuItem_15:
    GameSys::CallAutoBattleProc();
    break;
  case AriDebugMenuItem_24:
    //かわいがり
    {
      // Setup Proc data
      SetKawaigariDebugParam(); // BG設定もこの中で行っている

      //mKawaigariProcData.bgParam.groundType = static_cast<btl::bg::bgsys_normal_groundID>(mBgGround);
      //mKawaigariProcData.bgParam.nearType = static_cast<btl::bg::bgsys_near_groundID>(mBgNear);
      //mKawaigariProcData.bgParam.farType = static_cast<btl::bg::bgsys_far_groundID>(mBgFar);
      //mKawaigariProcData.bgParam.useSkyBox = mBgSkyBox;

      GameSys::CallKawaigariProc(&mKawaigariProcData);
    }
    break;
    //バトル呼び出し系
  case AriDebugMenuItem_01: //野生シングル
  case AriDebugMenuItem_03: //トレーナーシングル
  case AriDebugMenuItem_04: //トレーナーダブル
  case AriDebugMenuItem_05: //バトルロイヤル
  case AriDebugMenuItem_06: //バトルマルチ
  case AriDebugMenuItem_07: //バトルマルチ(2vs2)
  case AriDebugMenuItem_08: //バトルフェス
  {
      gfl2::heap::HeapBase *heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
      GameSys::GameManager *gameMng = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
      GameSys::GameData *gameData = gameMng->GetGameData();
      pml::PokeParty*               playerParty = gameData->GetPlayerParty();

      heap->Dump();

      //前回のクリア
      playerParty->Clear();
      ClearBattleParam();

      gameData->GetPlayerStatus()->SetZenryokuRing(true);

      gameData->GetMyItem()->Add(ITEM_MONSUTAABOORU , 10 );
      gameData->GetMyItem()->Add(ITEM_MASUTAABOORU , 10 );

      //@todo ランチャーの仕組みが変わったので開放できない！
      mEnemyParty  = GFL_NEW(heap) pml::PokeParty(heap);
      mEnemyParty->Clear();

      // プレイヤー手持ち
      int pokeNum = 6;
      if (ItemList[pos] == AriDebugMenuItem_08)
      {
        pokeNum = 3;
        mPlayerPokePara[0] = GFL_NEW(heap) pml::pokepara::PokemonParam(heap, MONSNO_PIKATYUU, 100, 0);
        mPlayerPokePara[1] = GFL_NEW(heap) pml::pokepara::PokemonParam(heap, MONSNO_RIZAADON, 100, 0);
        mPlayerPokePara[1]->SetItem(ITEM_RIZAADONAITOx);
        mPlayerPokePara[2] = GFL_NEW(heap) pml::pokepara::PokemonParam(heap, MONSNO_HORAA, 100, 0);
        mPlayerPokePara[2]->SetItem(ITEM_MIMIKKYUz_1);
      }
      else
      {
        switch (mBgPartySet)
        {
        case 0:
        {
          for (int i = 0; i < pokeNum; i++)
          {
            if (mZenryokuChech == -1)
            {
              if (ItemList[pos] == AriDebugMenuItem_08)
              {
                mPlayerPokePara[i] = GFL_NEW(heap) pml::pokepara::PokemonParam(heap, (MonsNo)(mSelfPoke + i), 100, 0);
              }
              else
              {
                mPlayerPokePara[i] = GFL_NEW(heap) pml::pokepara::PokemonParam(heap, (MonsNo)mSelfPoke, 100, 0);
              }

              mPlayerPokePara[i]->SetWaza(0, WAZANO_BORUTEKKAA);
              mPlayerPokePara[i]->SetWaza(1, WAZANO_KAENHOUSYA);
              mPlayerPokePara[i]->SetWaza(2, WAZANO_DOKUDOKU);
              mPlayerPokePara[i]->SetWaza(3, WAZANO_KAWARAWARI);
              mPlayerPokePara[i]->SetItem(ITEM_PIKATYUUBIIZU_1);

              mPlayerPokePara[i]->ChangeFormNo(mSelfPokeForm);
            }
            else
            {
              mPlayerPokePara[i] = ZenryokuCheckSetup();
            }
          }
        }
        break;
        case 1:
          pokeNum = 6;
          mPlayerPokePara[0] = GFL_NEW(heap) pml::pokepara::PokemonParam(heap, MONSNO_REKKUUZA, 100, 0);
          mPlayerPokePara[1] = GFL_NEW(heap) pml::pokepara::PokemonParam(heap, MONSNO_RANKURUSU, 100, 0);
          mPlayerPokePara[2] = GFL_NEW(heap) pml::pokepara::PokemonParam(heap, MONSNO_MAGGYO, 100, 0);
          mPlayerPokePara[3] = GFL_NEW(heap) pml::pokepara::PokemonParam(heap, MONSNO_FAIYAA, 100, 0);
          mPlayerPokePara[4] = GFL_NEW(heap) pml::pokepara::PokemonParam(heap, MONSNO_NASSII, 100, 0);
          mPlayerPokePara[5] = GFL_NEW(heap) pml::pokepara::PokemonParam(heap, MONSNO_HAGANEERU, 100, 0);
          mPlayerPokePara[4]->ChangeFormNo(1);
          mPlayerPokePara[0]->SetWaza(0, WAZANO_GURASUFIIRUDO);
          mPlayerPokePara[0]->SetWaza(1, WAZANO_SAIKOFIIRUDO);
          mPlayerPokePara[0]->SetWaza(2, WAZANO_MISUTOFIIRUDO);
          mPlayerPokePara[0]->SetWaza(3, WAZANO_EREKIFIIRUDO);
          mPlayerPokePara[1]->SetWaza(0, WAZANO_NIHONBARE);
          mPlayerPokePara[1]->SetWaza(1, WAZANO_AMAGOI);
          mPlayerPokePara[1]->SetWaza(2, WAZANO_SUNAARASI);
          mPlayerPokePara[1]->SetWaza(3, WAZANO_ARARE);
          mPlayerPokePara[2]->SetWaza(0, WAZANO_HEBIIBONBAA);
          mPlayerPokePara[2]->SetWaza(1, WAZANO_BUREIBUBAADO);
          break;
        case 2:
          pokeNum = 6;
          mPlayerPokePara[0] = GFL_NEW(heap) pml::pokepara::PokemonParam(heap, MONSNO_GOOSU, 100, 0);
          mPlayerPokePara[1] = GFL_NEW(heap) pml::pokepara::PokemonParam(heap, MONSNO_GARAGARA, 100, 0);
          mPlayerPokePara[2] = GFL_NEW(heap) pml::pokepara::PokemonParam(heap, MONSNO_HOERUOO, 100, 0);
          mPlayerPokePara[3] = GFL_NEW(heap) pml::pokepara::PokemonParam(heap, MONSNO_GENGAA, 100, 0);
          mPlayerPokePara[4] = GFL_NEW(heap) pml::pokepara::PokemonParam(heap, MONSNO_HAGANEERU, 100, 0);
          mPlayerPokePara[5] = GFL_NEW(heap) pml::pokepara::PokemonParam(heap, MONSNO_RANKURUSU, 100, 0);
          mPlayerPokePara[3]->ChangeFormNo(1);
          mPlayerPokePara[4]->ChangeFormNo(1);

          mPlayerPokePara[1]->SetWaza(0, WAZANO_TURUGINOMAI);
          mPlayerPokePara[1]->SetWaza(1, WAZANO_TYOUNOMAI);
          break;
        }

      }
      for (int i = 0; i<pokeNum; i++)
      {
        gameData->GetPlayerStatus()->SetPokemonParent( mPlayerPokePara[i], heap );
        mPlayerPokePara[i]->SetFriendship( 255 );
        playerParty->AddMember( *mPlayerPokePara[i] );
       }

      // 敵手持ち
      for( int i=0;i<mEnemyPokeNum;i++ )
      {
        if( mZenryokuChech == -1 )
        {
          mEnemyPokePara[i] = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, (MonsNo)mEnemyPoke, 15, 0 );
          mEnemyPokePara[i]->SetWaza(0,WAZANO_BORUTEKKAA);
          mEnemyPokePara[i]->SetWaza(1,WAZANO_KAENHOUSYA);
          mEnemyPokePara[i]->SetWaza(2,WAZANO_DOKUDOKU);
          mEnemyPokePara[i]->SetWaza(3,WAZANO_KAWARAWARI);
          mEnemyPokePara[i]->SetItem(ITEM_PIKATYUUBIIZU_1);
          mEnemyPokePara[i]->ChangeFormNo(mEnemyPokeForm);
        }
        else
        {
          mEnemyPokePara[i] = ZenryokuCheckSetup();
        }
        if( ItemList[pos] != AriDebugMenuItem_01 || i == 0  )
        {
          mEnemyParty->AddMember( *mEnemyPokePara[i] );
        }
      }

      BATTLE_SETUP_FIELD_SITUATION_Init( &mSit );
      BATTLE_SETUP_Init( &mSetupParam );

      if( ItemList[pos] == AriDebugMenuItem_01 )
      {
        BATTLE_SETUP_Wild( &mSetupParam , gameMng , mEnemyParty ,&mSit , BTL_RULE_SINGLE , BATTLE_EFFECT_WILD_GRASS , NULL, heap );
      }
      else
      if( ItemList[pos] == AriDebugMenuItem_03 ||
          ItemList[pos] == AriDebugMenuItem_08)
      {
        BTL_SETUP_Trainer( &mSetupParam , gameMng , (trainer::TrainerID)mTrainerId[0] ,&mSit , BTL_RULE_SINGLE , heap );

        mSetupParam.party[1]->Clear();
        if( mZenryokuChech == -1 )
        {
          for( int i=0;i<mEnemyPokeNum;i++ )
          {
            mSetupParam.party[1]->AddMember( *mEnemyPokePara[i] );
          }
        }
        else
        {
          mSetupParam.party[1]->AddMember( *mEnemyPokePara[0] );
          mSetupParam.tr_data[1]->SetAIBit(BTL_AISCRIPT_BIT_WAZA_BASIC | BTL_AISCRIPT_BIT_WAZA_STRONG | BTL_AISCRIPT_BIT_WAZA_EXPERT | BTL_AISCRIPT_BIT_POKECHANGE_BASIC);
          if( mDebugParam.mTrainerType[1] == -2 )
          {
            mSetupParam.playerStatus[1] = GFL_NEW(heap)Savedata::MyStatus();
            mSetupParam.playerStatus[1]->CopyFrom(*gameData->GetSaveData()->GetMyStatus());
          }
        }
        if (ItemList[pos] == AriDebugMenuItem_08)
        {
          mSetupParam.competitor = BTL_COMPETITOR_INST;
        }
      }
      else
      if( ItemList[pos] == AriDebugMenuItem_04 )
      {
        BTL_SETUP_Trainer( &mSetupParam , gameMng , (trainer::TrainerID)mTrainerId[0] ,&mSit , BTL_RULE_DOUBLE , heap );

        mSetupParam.party[1]->Clear();
        for( int i=0;i<mEnemyPokeNum;i++ )
        {
          mSetupParam.party[1]->AddMember( *mEnemyPokePara[i] );
        }
      }
      else
      if( ItemList[pos] == AriDebugMenuItem_05 )
      {
        btl::setup::common_base( &mSetupParam, gameMng, &mSit );
        mSetupParam.competitor = BTL_COMPETITOR_TRAINER;
        mSetupParam.rule = BTL_RULE_ROYAL;
        mSetupParam.fieldSituation.bgComponent.groudnType = btl::bg::PLATE_BTLROYAL;
        mSetupParam.fieldSituation.bgComponent.nearType = btl::bg::NEAR_NONE;
        mSetupParam.fieldSituation.bgComponent.farType = btl::bg::FAR_NONE;

        for( int i=0;i<4;i++ )
        {
          mSetupParam.party[ i ] = GFL_NEW(heap) pml::PokeParty(heap);
        }

        mSetupParam.tr_data[ BTL_CLIENT_ROYAL2 ] = GFL_NEW(heap) BSP_TRAINER_DATA(heap);
        trainer::EncountTrainerPersonalDataMake( (trainer::TrainerID)mTrainerId[0], mSetupParam.tr_data[ BTL_CLIENT_ROYAL2 ], heap );
        mSetupParam.tr_data[ BTL_CLIENT_ROYAL3 ] = GFL_NEW(heap) BSP_TRAINER_DATA(heap);
        trainer::EncountTrainerPersonalDataMake( (trainer::TrainerID)mTrainerId[1], mSetupParam.tr_data[ BTL_CLIENT_ROYAL3 ], heap );
        mSetupParam.tr_data[ BTL_CLIENT_ROYAL4 ] = GFL_NEW(heap) BSP_TRAINER_DATA(heap);
        trainer::EncountTrainerPersonalDataMake( (trainer::TrainerID)mTrainerId[2], mSetupParam.tr_data[ BTL_CLIENT_ROYAL4 ], heap );

        pml::PokeParty* playerParty = gameData->GetPlayerParty();
        mSetupParam.party[ BTL_CLIENT_PLAYER ]->CopyFrom( *playerParty );
        for( int i=1;i<4;i++ )
        {
          mSetupParam.party[i]->Clear();
          for( int j=0;j<mEnemyPokeNum;j++ )
          {
            mSetupParam.party[i]->AddMember( *mEnemyPokePara[j] );
          }
          if( mZenryokuChech != -1 )
          {
            mSetupParam.tr_data[i]->SetAIBit(BTL_AISCRIPT_BIT_WAZA_BASIC | BTL_AISCRIPT_BIT_WAZA_STRONG | BTL_AISCRIPT_BIT_WAZA_EXPERT | BTL_AISCRIPT_BIT_POKECHANGE_BASIC);
            if( mDebugParam.mTrainerType[i] == -2 )
            {
              //mSetupParam.playerStatus[i] = GFL_NEW(heap)Savedata::MyStatus();
              //mSetupParam.playerStatus[i]->CopyFrom(*gameData->GetSaveData()->GetMyStatus());
            }
          }
        }
      }
      else
      if( ItemList[pos] == AriDebugMenuItem_06 )
      {
        BTL_SETUP_Trainer_Tag( &mSetupParam , gameMng , (trainer::TrainerID)mTrainerId[0], (trainer::TrainerID)mTrainerId[2] ,&mSit , BTL_RULE_DOUBLE , heap );
        BATTLE_EFFECT_GetParam( BATTLE_EFFECT_TR_MULTI2D , &mSetupParam.btlEffData );

        mSetupParam.party[1]->Clear();
        mSetupParam.party[3]->Clear();
        for( int i=0;i<mEnemyPokeNum;i++ )
        {
          mSetupParam.party[1]->AddMember( *mEnemyPokePara[i] );
          mSetupParam.party[3]->AddMember( *mEnemyPokePara[i] );
        }
      }
      else
      if( ItemList[pos] == AriDebugMenuItem_07 )
      {
        BTL_SETUP_Trainer_Multi( &mSetupParam , gameMng , (trainer::TrainerID)mTrainerId[1] ,(trainer::TrainerID)mTrainerId[0], (trainer::TrainerID)mTrainerId[2] ,&mSit , BTL_RULE_DOUBLE , heap );
        BATTLE_EFFECT_GetParam( BATTLE_EFFECT_TR_MULTI2D , &mSetupParam.btlEffData );

        mSetupParam.party[1]->Clear();
        mSetupParam.party[2]->Clear();
        mSetupParam.party[3]->Clear();
        for( int i=0;i<mEnemyPokeNum;i++ )
        {
          mSetupParam.party[1]->AddMember( *mEnemyPokePara[i] );
          mSetupParam.party[2]->AddMember( *mEnemyPokePara[i] );
          mSetupParam.party[3]->AddMember( *mEnemyPokePara[i] );
        }
      }

      //フェス専用設定
      if (ItemList[pos] == AriDebugMenuItem_08)
      {
        BATTLE_PARAM_SetBtlStatusFlag(&mSetupParam, BTL_STATUS_FLAG_BATTLE_FES);

        mSetupParam.playerStatus[2] = GFL_NEW(heap)Savedata::MyStatus();
        mSetupParam.playerStatus[3] = GFL_NEW(heap)Savedata::MyStatus();

        poke_3d::model::DressUpModelResourceManager* pDressUpResourceManager = GFL_NEW(heap) poke_3d::model::DressUpModelResourceManager();
        pDressUpResourceManager->Initialize(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager(), heap, System::DressUp::GetDressUpArcIdListField());
        poke_3d::model::DressUpParam dressUpParam;
        gfl2::str::StrBuf *tempStr = GFL_NEW(heap) gfl2::str::StrBuf(System::STRLEN_PLAYER_NAME + System::EOM_LEN, heap);

        tempStr->SetStr(L"じろう");
        mSetupParam.playerStatus[2]->SetNameString(tempStr);
        mSetupParam.playerStatus[2]->SetSex(PM_MALE);
        System::DressUp::Debug_GetDressUpParamRandom(PM_MALE, pDressUpResourceManager->GetDressUpModelResourceManagerCore(poke_3d::model::DressUpParam::SEX_MALE), &dressUpParam);
        mSetupParam.playerStatus[2]->SetDressup(dressUpParam);
        mSetupParam.playerStatus[2]->SetMegaRing(true);

        tempStr->SetStr(L"さぶろう");
        mSetupParam.playerStatus[3]->SetNameString(tempStr);
        mSetupParam.playerStatus[3]->SetSex(PM_FEMALE);
        System::DressUp::Debug_GetDressUpParamRandom(PM_FEMALE, pDressUpResourceManager->GetDressUpModelResourceManagerCore(poke_3d::model::DressUpParam::SEX_FEMALE), &dressUpParam);
        mSetupParam.playerStatus[3]->SetDressup(dressUpParam);

        pDressUpResourceManager->Finalize();
        GFL_DELETE pDressUpResourceManager;
        GFL_DELETE tempStr;

        mSetupParam.tr_data[0] = GFL_NEW(heap)BSP_TRAINER_DATA(heap);
        mSetupParam.tr_data[0]->SetMsgID(0, System::GflUse::GetPublicRand() % 1049, 0);
        mSetupParam.tr_data[2] = GFL_NEW(heap)BSP_TRAINER_DATA(heap);
        mSetupParam.tr_data[2]->SetMsgID(0, System::GflUse::GetPublicRand() % 1049, 0);
        mSetupParam.tr_data[3] = GFL_NEW(heap)BSP_TRAINER_DATA(heap);
        mSetupParam.tr_data[3]->SetMsgID(0, System::GflUse::GetPublicRand() % 1049, 0);
      }
      if( mSkipEncountEffect )
      {
        BTL_SETUP_SetDebugFlag(&mSetupParam,BTL_DEBUGFLAG_SKIP_BTLIN);
      }

      switch( mRideWareType )
      {
      case 0: break;
      case 1: BATTLE_PARAM_SetBtlStatusFlag(&mSetupParam,BTL_STATUS_FLAG_RIDEWARE_LAND);break;
      case 2: BATTLE_PARAM_SetBtlStatusFlag(&mSetupParam,BTL_STATUS_FLAG_RIDEWARE_SWIM);break;
      }

      if( mMuteki )
      {
        BTL_SETUP_SetDebugFlag(&mSetupParam,BTL_DEBUGFLAG_HP_CONST);
      }
      if( mIsLastBoss )
      {
        BATTLE_PARAM_SetBtlStatusFlag(&mSetupParam,BTL_STATUS_FLAG_VS_BOSS);
      }
      if( mZenryokuChech != -1 && ItemList[pos] == AriDebugMenuItem_03 )
      {
        BATTLE_PARAM_SetBtlStatusFlag(&mSetupParam,BTL_STATUS_FLAG_AI_TEND_USE_ZPOWER);
        if( mDebugParam.mTrainerType[1] == -2 )
        {
          mSetupParam.recordDataMode = BTL_RECORDDATA_FIGHT;
        }
      }
      if( mAiCtrl ||
          (mZenryokuChech != -1 && ItemList[pos] == AriDebugMenuItem_05))
      {
        BTL_SETUP_SetDebugFlag(&mSetupParam,BTL_DEBUGFLAG_AI_CTRL);
      }
      if (mTrainerNoBall)
      {
        BATTLE_PARAM_SetBtlStatusFlag(&mSetupParam, BTL_STATUS_FLAG_TRAINER_NO_BALL);
      }
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestDefaultOut(gfl2::Fade::DISP_DOUBLE);

      mSetupParam.fieldSituation.bgComponent.groudnType = static_cast<btl::bg::bgsys_normal_groundID>(mBgGround);
      mSetupParam.fieldSituation.bgComponent.nearType = static_cast<btl::bg::bgsys_near_groundID>(mBgNear);
      mSetupParam.fieldSituation.bgComponent.farType = static_cast<btl::bg::bgsys_far_groundID>(mBgFar);
      mSetupParam.fieldSituation.bgComponent.useSkyBox = mBgSkyBox;

      mSetupParam.btlvDebugParam = mDebugParam;

      btl::BATTLE_PROC_PARAM param;
      param.pAppMemory = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
      param.pAppDeviceMemory = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
      param.pGameManager = gameMng;
      param.pSetupParam = &mSetupParam;
      GameSys::CallBattleProc(&param);
    }
    break;
#if 0
  case AriDebugMenuItem_07:
    {
      HeapTest1();
      {
#if GFL_DEBUG_PRINT //開放忘れ止まりのテストコード。warningが出るのでPRINTの有無で抑制
        gfl2::heap::HeapBase *heap = GFL_CREATE_LOCAL_HEAP_NAME( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG) ,0x10000,gfl2::heap::HEAP_TYPE_EXP , false , "TestHeap" );
        void *buf1 = GflHeapAllocMemory( heap , 13 );
        void *buf2 = GflHeapAllocMemory( heap , 13 );
        void *buf3 = GflHeapAllocMemory( heap , 13 );
        GflHeapFreeMemory(buf1);
        buf2 = NULL;
        GflHeapFreeMemory(buf3);
        GFL_PRINT("Buf[%x]\n",buf2);

        GFL_DELETE_HEAP(heap);
#endif //GFL_DEBUG_PRINT
      }

      /*
      gfl2::heap::HeapBase *heap = GFL_CREATE_LOCAL_HEAP_NAME( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG) ,0x10000,gfl2::heap::HEAP_TYPE_EXP , false , "TestHeap" );
      while(true)
      {
        GflHeapAllocMemory( heap , 0x10000 );
      }
      */
    }
    break;
#endif
    case AriDebugMenuItem_09:
    {
      GameSys::GameManager *gameMng = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
      GameSys::GameData *gameData = gameMng->GetGameData();
#if 0 //ボール投げモーション習得状況はSysFlagに移行したので、Miscセーブデータはオミット　by iwasawa 160205
      gameData->GetMisc()->SetBallThrowLearn(0);
      gameData->GetMisc()->SetBallThrowLearn(2);
      gameData->GetMisc()->SetBallThrowLearn(5);
      gameData->GetMisc()->SetBallThrowLearn(6);
      gameData->GetMisc()->SetBallThrowLearn(8);
      for( int i=0;i<Savedata::Misc::BALL_THROW_TYPE_NUM;i++ )
      {
        GFL_PRINT("[%d]",gameData->GetMisc()->CheckBallThrowLearn(i));
      }
      GFL_PRINT("\n");
      gameData->GetMisc()->ResetBallThrowLearn();
#endif

      gameData->GetMisc()->AddKawaigariPastDays(32);
      GFL_PRINT("[%d]",gameData->GetMisc()->GetKawaigariPastDays());
      gameData->GetMisc()->AddKawaigariPastDays(128);
      GFL_PRINT("[%d]",gameData->GetMisc()->GetKawaigariPastDays());
      gameData->GetMisc()->AddKawaigariPastDays(128);
      GFL_PRINT("[%d]",gameData->GetMisc()->GetKawaigariPastDays());
      gameData->GetMisc()->ResetKawaigariPastDays();
      GFL_PRINT("\n");

      gfl2::heap::HeapBase *heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG);
      gfl2::debug::DebugWinGroup *grp = gfl2::debug::DebugWin_AddGroup( heap , L"AriTest" , NULL , 128 );
      for( int i=0;i<100;i++ )
      {
        gfl2::debug::DebugWin_AddItemValueInt(grp,heap,L"Temp",&i,0,100);
      }

      gfl2::debug::DebugWin_RemoveGroup(grp);

      //破棄テスト
      //Debug::DebugWin::FinalizeSystem();
    }
    break;

    //return launcher::ListData::REQID_CALL;
  }

  return debug::launcher::ListData::REQID_NONE;
}

pml::pokepara::PokemonParam* AriTestLauncher::ZenryokuCheckSetup(void)
{
  gfl2::heap::HeapBase *heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
  int monsno = mEnemyPoke;
  int formNo = 0;
  switch(mZenryokuChech)
  {
  case 18: monsno = MONSNO_HUKUROU3; break;
  case 19: monsno = MONSNO_TORA3; break;
  case 20: monsno = MONSNO_ASIKA3; break;
  case 21: monsno = MONSNO_PIKATYUU; break;
  case 22: monsno = MONSNO_RAITYUU; formNo = 1;break;
  case 23: monsno = MONSNO_THIKITORI; break;
  case 24: monsno = MONSNO_IIBUI; break;
  case 25: monsno = MONSNO_KABIGON; break;
  case 26: monsno = MONSNO_MYUU; break;
  case 27: monsno = MONSNO_PIKATYUU; formNo = 1;break;
  case 28: monsno = MONSNO_MABOROSI17; break;
  case 29: monsno = MONSNO_HORAA; break;
  case 30: monsno = MONSNO_HORAA; formNo = 2; break;
  case 31: monsno = MONSNO_UROKO3; break;
  case 32: monsno = MONSNO_UROKO3; formNo = 1; break;
  case 33: monsno = MONSNO_OOKAMI2; break;
  case 34: monsno = MONSNO_OOKAMI2; formNo = 1; break;
  case 35: monsno = MONSNO_OOKAMI2; formNo = 2; break;
  case 36: monsno = MONSNO_GEKKOU; break;
  case 37: monsno = MONSNO_PURIZUMU; formNo = 2; break;
  case 38: monsno = MONSNO_NIKKOU; break;
  case 39: monsno = MONSNO_PURIZUMU; formNo = 1; break;
  case 40: monsno = MONSNO_PURIZUMU; formNo = 3; break;
  }
  u16 wazaNo[41] = 
  {
    WAZANO_HATAKU,//"ノーマル",
    WAZANO_KARATETYOPPU,//"かくとう",
    WAZANO_KAZEOKOSI,//"ひこう",
    WAZANO_DOKUBARI,//"どく",
    WAZANO_ZISIN,//"じめん",
    WAZANO_IWAOTOSI,//"いわ",
    WAZANO_MUSINOSAZAMEKI,//"むし",
    WAZANO_SITADENAMERU,//"ゴースと",
    WAZANO_AIANTEERU,//"はがね",
    WAZANO_HINOKO,//"ほのお",
    WAZANO_MIZUDEPPOU,//"みず",
    WAZANO_TURUNOMUTI,//"くさ",
    WAZANO_10MANBORUTO,//"でんき",
    WAZANO_SAIKOKINESISU,//"エスパー",
    WAZANO_HUBUKI,//"こおり",
    WAZANO_DORAGONTEERU,//"ドラゴン",
    WAZANO_AKUNOHADOU,//"あく",
    WAZANO_ZYARETUKU,//"フェアリー",
    WAZANO_KAGENUI,//"ジュナイパー",
    WAZANO_ddRARIATTO,//"ガオガエン",
    WAZANO_UTAKATANOARIA,//"アシレーヌ",
    WAZANO_BORUTEKKAA,//"ピカチュウ",
    WAZANO_10MANBORUTO,//"アロライ",
    WAZANO_SIZENNOIKARI,//"ティキ",
    WAZANO_TOTTEOKI,//"イーブイ",
    WAZANO_GIGAINPAKUTO,//"カビゴン",
    WAZANO_SAIKOKINESISU,//"ミュウ",
    WAZANO_10MANBORUTO,//"サトピカ",
    WAZANO_SYADOOSUTIIRU,//"マーシャドー",
    WAZANO_ZYARETUKU,//ミミッキュＺ
    WAZANO_ZYARETUKU,//ミミッキュＺ(ぬし
    WAZANO_SUKEIRUNOIZU,//ジャラランガＺ
    WAZANO_SUKEIRUNOIZU,//ジャラランガＺ(ぬし
    WAZANO_SUTOONEZZI,//ルガルガンＺ(ひる
    WAZANO_SUTOONEZZI,//ルガルガンＺ(よる
    WAZANO_SUTOONEZZI,//ルガルガンＺ(ゆうぐれ
    WAZANO_SYADOOREI,//ルナＺ
    WAZANO_SYADOOREI,//ＮルナＺ
    WAZANO_METEODORAIBU,//ソルＺ
    WAZANO_METEODORAIBU,//ＮソルＺ
    WAZANO_HIKARIDORAGON,//ヒカリドラゴンＺ  
  };
  u16 itemNo[41] = 
  {
    ITEM_NOOMARUBIIZU_1,//"ノーマル",
    ITEM_KAKUTOUBIIZU_1,//"かくとう",
    ITEM_HIKOUBIIZU_1,//"ひこう",
    ITEM_DOKUBIIZU_1,//"どく",
    ITEM_ZIMENBIIZU_1,//"じめん",
    ITEM_IWABIIZU_1,//"いわ",
    ITEM_MUSIBIIZU_1,//"むし",
    ITEM_GOOSUTOBIIZU_1,//"ゴースと",
    ITEM_HAGANEBIIZU_1,//"はがね",
    ITEM_HONOOBIIZU_1,//"ほのお",
    ITEM_MIZUBIIZU_1,//"みず",
    ITEM_KUSABIIZU_1,//"くさ",
    ITEM_DENKIBIIZU_1,//"でんき",
    ITEM_ESUPAABIIZU_1,//"エスパー",
    ITEM_KOORIBIIZU_1,//"こおり",
    ITEM_DORAGONBIIZU_1,//"ドラゴン",
    ITEM_AKUBIIZU_1,//"あく",
    ITEM_FEARIIBIIZU_1,//"フェアリー",
    ITEM_HUKUROU3BIIZU_1,//"ジュナイパー",
    ITEM_TORA3BIIZU_1,//"ガオガエン",
    ITEM_ASIKA3BIIZU_1,//"アシレーヌ",
    ITEM_PIKATYUUBIIZU_1,//"ピカチュウ",
    ITEM_ARORAIBIIZU_1,//"アロライ",
    ITEM_THIKIBIIZU_1,//"ティキ",
    ITEM_IIBUIBIIZU_1,//"イーブイ",
    ITEM_KABIGONBIIZU_1,//"カビゴン",
    ITEM_MYUUBIIZU_1,//"ミュウ",
    ITEM_SATOPIKABIIZU_1,//"サトピカ",
    ITEM_HAISINBIIZU_1,//"マーシャドー",
    ITEM_MIMIKKYUz_1,//ミミッキュＺ
    ITEM_MIMIKKYUz_1,//ミミッキュＺ(ぬし
    ITEM_ZYARARANGAz_1,//ジャラランガＺ
    ITEM_ZYARARANGAz_1,//ジャラランガＺ(ぬし
    ITEM_RUGARUGANz_1,//ルガルガンＺ(ひる
    ITEM_RUGARUGANz_1,//ルガルガンＺ(よる
    ITEM_RUGARUGANz_1,//ルガルガンＺ(ゆうぐれ
    ITEM_RUNAAARAz_1,//ルナＺ
    ITEM_RUNAAARAz_1,//ＮルナＺ
    ITEM_SORUGAREOz_1,//ソルＺ
    ITEM_SORUGAREOz_1,//ＮソルＺ
    ITEM_NEKUROZUMAz_1,//ヒカリドラゴンＺ  
  };
  pml::pokepara::PokemonParam *pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, (MonsNo)monsno, 15, 0 );
  pp->SetWaza(0,(WazaNo)wazaNo[mZenryokuChech]);
  pp->SetWaza(1,WAZANO_NOMIKOMU);
  pp->SetWaza(2,WAZANO_NULL);
  pp->SetWaza(3,WAZANO_NULL);
  pp->SetItem(itemNo[mZenryokuChech]);
  pp->ChangeFormNo(formNo);
  return pp;
}

void AriTestLauncher::ClearBattleParam(void)
{
  BATTLE_SETUP_Clear(&mSetupParam);
  if( mEnemyParty )
  {
    mEnemyParty->Clear();
  }
  for( int i=0;i<6;i++ )
  {
    GFL_SAFE_DELETE(mPlayerPokePara[i]);
    GFL_SAFE_DELETE(mEnemyPokePara[i]);
  }
  GFL_SAFE_DELETE(mEnemyParty);
}

void AriTestLauncher::ClearEffviewParam(void)
{
#if PM_DEBUG
  for( int i=0;i<4;i++ )
  {
    GFL_SAFE_DELETE(mDebugParam.pp[i]);
  }
#endif //PM_DEBUG
}

class AriTestHeapTestClass
{
public:
  AriTestHeapTestClass()
  {
    GFL_PRINT("コンストラクタ\n");
    dummy = 1;
  }
  ~AriTestHeapTestClass()
  {
    GFL_PRINT("デストラクタ\n");
    dummy = 0;
  }
private:
  int dummy;
};

void AriTestLauncher::HeapTest1(void)
{
  gfl2::heap::HeapBase *heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG);

  AriTestHeapTestClass *testClass0 = GFL_NEW(heap)AriTestHeapTestClass();
  GFL_DELETE testClass0;

/*
  void *buf = GflHeapAllocMemory(heap,sizeof(AriTestHeapTestClass) );
  AriTestHeapTestClass *testClass = new(buf)AriTestHeapTestClass();
  GFL_DELETE testClass;
  GflHeapFreeMemory( buf );
*/

  void *buf = GflHeapAllocMemory(heap,sizeof(AriTestHeapTestClass)*2 );
  AriTestHeapTestClass *testClass = new(buf)AriTestHeapTestClass();
  AriTestHeapTestClass *testClass2 = new((void*)(((int)buf)+sizeof(AriTestHeapTestClass)))AriTestHeapTestClass();
  //GFL_DELETE testClass2;
  //GFL_DELETE testClass;
  //Deleteはできない
  testClass2->~AriTestHeapTestClass();
  testClass->~AriTestHeapTestClass();
  GflHeapFreeMemory( buf );

}
void AriTestLauncher::InitializeKawaigariParam(gfl2::heap::HeapBase *heap)
{

  // Init debug param
  mKawaigariDebugParam.Mons_no = (int)MONSNO_HUSIGIDANE;
  mKawaigariDebugParam.Form_no = 0;
  mKawaigariDebugParam.Rare = false;
  mKawaigariDebugParam.Sex = (int)pml::SEX_MALE;
    
  mKawaigariDebugParam.Bad_status = 0;

  mKawaigariDebugParam.Nakayoshi = 0;
  mKawaigariDebugParam.Manpuku = 0;
  mKawaigariDebugParam.Nadenade = 0;
  
  mKawaigariDebugParam.BgGround = btl::bg::PLATE_GRASS;
  mKawaigariDebugParam.BgNear = btl::bg::NEAR_GRASS;
  mKawaigariDebugParam.BgFar = btl::bg::FAR_MOUNT;
  mKawaigariDebugParam.BgSkyBox = true;

  mKawaigariDebugParam.DirtType = 0;
  mKawaigariDebugParam.DirtNum = 1;

  // Init PokemonParam
  mKawaigariProcData.pokemonParam = GFL_NEW(heap) pml::pokepara::PokemonParam(heap);


  // Set paramater
  SetKawaigariDebugParam();

}

void AriTestLauncher::ClearKawaigariParam(void)
{
  GFL_SAFE_DELETE (mKawaigariProcData.pokemonParam);
}

int KAWAIGARI_BAD_STATUS_INFO[] = 
  {
    pml::pokepara::SICK_NULL,
    pml::pokepara::SICK_MAHI,
    pml::pokepara::SICK_NEMURI,
    pml::pokepara::SICK_KOORI,
    pml::pokepara::SICK_YAKEDO,
    pml::pokepara::SICK_DOKU,
  };

void AriTestLauncher::SetKawaigariDebugParam(void)
{
  KawaigariDebugParam &param = mKawaigariDebugParam;

  // Basic info
  mKawaigariProcData.pokemonParam->ChangeMonsNo( (MonsNo)param.Mons_no, (FormNo)param.Form_no );
  mKawaigariProcData.pokemonParam->ChangeFormNo( (FormNo)param.Form_no );    // @fix GFNMCat[4050] ChangeMonsNo()は同一ポケモンでフォルムだけ変化させると反映しないようなので、ChangeFormNo()を追加
  mKawaigariProcData.pokemonParam->ChangeSex( (pml::Sex)param.Sex );
  mKawaigariProcData.pokemonParam->SetGetBall( item::BALLID_MASUTAABOORU );
  mKawaigariProcData.pokemonParam->RecoverAll();

  if( param.Rare )
    {
      mKawaigariProcData.pokemonParam->SetRare();
    }
  else
    {
      mKawaigariProcData.pokemonParam->SetNotRare();
    }

  // Bad status
  if( param.Bad_status == 0 )
    {
      mKawaigariProcData.pokemonParam->RecoverSick();
    }
  else
    {
      mKawaigariProcData.pokemonParam->SetSick( (pml::pokepara::Sick)(KAWAIGARI_BAD_STATUS_INFO[ param.Bad_status ]) );
    }

  // Kawaigari status
  mKawaigariProcData.pokemonParam->SetFeed( param.Manpuku );            // まんぷく
  mKawaigariProcData.pokemonParam->SetNadenadeValue( param.Nadenade );  // なでなで
  mKawaigariProcData.pokemonParam->SetFriendship( param.Nakayoshi );    // 仲良し度

  // Dirt
  {
    pml::pokepara::DirtType DIRT_STATUS[] = 
      {
        pml::pokepara::DIRT_TYPE_NONE,    // 汚れなし
        pml::pokepara::DIRT_TYPE_STAIN,   // よごれ
        pml::pokepara::DIRT_TYPE_SAND,    // すなぼこり
        pml::pokepara::DIRT_TYPE_HAIR,    // みだれげ
        pml::pokepara::DIRT_TYPE_WATER,   // みずぬれ
      };
    
    if( mKawaigariDebugParam.DirtType == 0 )
      {
        mKawaigariProcData.pokemonParam->ClearDirt();
      }
    else
      {
        mKawaigariProcData.pokemonParam->ClearDirt();
        mKawaigariProcData.pokemonParam->SetDirtType( DIRT_STATUS[ mKawaigariDebugParam.DirtType ] );

        for( int i=0; i < mKawaigariDebugParam.DirtNum; i++)
          {
            mKawaigariProcData.pokemonParam->SetDirtPos( i );
          }
      }
  }

  // Back Ground
  mKawaigariProcData.bgParam.groundType = static_cast<btl::bg::bgsys_normal_groundID>(param.BgGround);
  mKawaigariProcData.bgParam.nearType = static_cast<btl::bg::bgsys_near_groundID>(param.BgNear);
  mKawaigariProcData.bgParam.farType = static_cast<btl::bg::bgsys_far_groundID>(param.BgFar);
  mKawaigariProcData.bgParam.useSkyBox = param.BgSkyBox;

  // HP Set:
  mKawaigariProcData.pokemonParam->SetHp(1);

  // Mode Set Initialize:
  mKawaigariProcData.mode = App::Kawaigari::KAWAIGARI_MODE_DEBUG;

}

GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)

#endif //PM_DEBUG
