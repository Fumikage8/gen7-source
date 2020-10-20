//======================================================================
/**
 * @file  btlv_core_cmd.cpp
 * @brief バトル描画コア部分
 * @author  ariizumi
 * @data  10/11/09
 */
//======================================================================

#include <fade/include/gfl2_FadeManager.h>
#include <debug/DebugWin/include/DebugWinSystem.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <debug/include/gfl2_DebugWinItemTypes.h>
#include <Debug/DebugWin/include/DebugWinSystem.h>

#include <pml/include/pmlib.h>
#include <pml/include/pml_Item.h>
#include <pml/include/battle/pml_battle_TypeAffinity.h>

#include <model/include/gfl2_CharaModelFactory.h>

#include "AppLib/include/Fade/DemoFade.h"
#include "DemoLib/SequencePlayer/include/SequencePlayerCore.h"
#include "DemoLib/Object/include/TaskFunc.h"
#include "PokeTool/include/PokeToolMega.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/btlv_resource.gaix>
#include <arc_index/wazaeff_resource.gaix>
#include <arc_index/wazaeff_seqfile.gaix>
#include <arc_index/chara_model_battle.gaix>
#include <niji_conv_header/battle/chara/chara_model_id.h>
#include <niji_conv_header/battle/btlv_cutin_seq.cdat>
#include <niji_conv_header/battle/battle_data_pack.h>

#include "Battle/include/btlv/btlv_debug_param.h"
#include "../btl_str.h"
#include "../btl_mainmodule.h"
#include "btlv_core.h"
#include "btlv_ui.h"
#include "btlv_camera.h"
#include "btlv_task.h"
#include "btlv_debug.h"
#include "btlv_trainer_data.h"
#include "btlv_loader.h"
#include "btlv_seq_com_define.h"

#include "str/include/gfl2_StrSys.h"

#include "./render/btlv_RenderSystem.h"
#include "./render/btlv_RenderPipeLine.h"

GFL_NAMESPACE_BEGIN(btl)

#if !defined( DEBUG_ONLY_FOR_obata_toshihiro )

static void BtlvDebugPrint( const char *file , const int line , const char *state )
{
  GFL_UNUSED(file);
  static int befLine = 0;
  if( befLine != line )
  {
    befLine = line;
    GFL_PRINT("BTLV:Call btlv_core_cmd.cpp[%4d][%s]\n",line,state);
  }
}

#define DEBUG_FUNC_CALL      BtlvDebugPrint(__FILE__,__LINE__,"空")
#define DEBUG_FUNC_CALL_END  BtlvDebugPrint(__FILE__,__LINE__,"済")
#define DEBUG_FUNC_CALL_TEMP BtlvDebugPrint(__FILE__,__LINE__,"仮")

#else
#define DEBUG_FUNC_CALL //Dummy
#define DEBUG_FUNC_CALL_END  //Dummy
#define DEBUG_FUNC_CALL_TEMP //Dummy

#endif

#if PM_DEBUG
#define DEBUG_NODRAW_CHECK if(mDebugParam&&mDebugParam->mIsNoDraw){return;}
#define DEBUG_NODRAW_CHECK_PARAM(ret) if(mDebugParam&&mDebugParam->mIsNoDraw){return ret;}
#else
#define DEBUG_NODRAW_CHECK //dummy
#define DEBUG_NODRAW_CHECK_PARAM(ret) //dummy
#endif 


//初期化
void BattleViewSystem::CMD_StartSetup(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;
  DumpBattleHeap();
  //GFNMCat5784 NMCat4152の対応でmSetupSeqをClenaupの条件に使っている。
  //バトルビデオの時はスキップするとここを通るのでmSetupSeqが初期化されてしまい、Cleanupされなくなってしまう。
  //初期化自体はInitializeでやっているのでここでの初期化はしない
  //mSetupSeq = 0;
  mComScene = 0;
  mPlayingTrainerIntro = false;
}

bool BattleViewSystem::CMD_WaitSetup(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);

  if( IsRecordStartSkip() )
  {
    return CMD_WaitSetup_RecReset();
  }
  enum
  {
    SEQ_LOAD_ARC = 0,
    SEQ_WAIT_ARC,
    SEQ_INIT_RENDER,
    SEQ_INIT_MSG,
    SEQ_INIT_UI,
    SEQ_WAIT_LOAD_BG,
    SEQ_CREATE_TEMP_DATA1,
    SEQ_CREATE_TEMP_DATA_WAIT1,
    SEQ_CREATE_TEMP_DATA2,
    SEQ_CREATE_TEMP_DATA_WAIT2,

    SEQ_FINISH_VIEWER,

    SEQ_START_EFFECT_TRAINER_ENEMY_LOGO,
    SEQ_START_EFFECT_TRAINER_ENEMY_LOGO_WAIT,

    SEQ_START_EFFECT_TRAINER_ENEMY_IN,
    SEQ_START_EFFECT_TRAINER_ENEMY_IN_WAIT_LOAD,
    SEQ_START_EFFECT_TRAINER_ENEMY_IN_WAIT,
    SEQ_START_EFFECT_TRAINER_ENEMY_THROW,
    SEQ_START_EFFECT_TRAINER_ENEMY_THROW_WAIT,
    SEQ_START_EFFECT_TRAINER_SELF,
    SEQ_START_EFFECT_TRAINER_SELF_WAIT,

    SEQ_START_EFFECT_WILD,
    SEQ_START_EFFECT_WILD_WAIT_LOAD,
    SEQ_START_EFFECT_WILD_WAIT,
    SEQ_START_EFFECT_WILD_ROTOM_LOAD,
    SEQ_START_EFFECT_WILD_ROTOM,
    SEQ_START_EFFECT_WILD_SELF,
    SEQ_START_EFFECT_WILD_SELF_WAIT,

    SEQ_START_ROYAL_IN,
    SEQ_START_ROYAL_IN_WAIT,
    SEQ_START_ROYAL_TRAINER,
    SEQ_START_ROYAL_TRAINER_WAIT,
    SEQ_START_ROYAL_END,
    SEQ_START_ROYAL_END_WAIT,

    SEQ_START_COMM_IN,
    SEQ_START_COMM_IN_WAIT_LOAD,
    SEQ_START_COMM_IN_WAIT,
    SEQ_START_EFFECT_COMM_FIRST_THROW,
    SEQ_START_EFFECT_COMM_FIRST_THROW_WAIT,
    SEQ_START_EFFECT_COMM_SECOND_THROW,
    SEQ_START_EFFECT_COMM_SECOND_THROW_WAIT,

    SEQ_SKIP_LOAD,
    SEQ_SKIP_LOAD_WAIT,

    SEQ_GAUGE_IN,
    SEQ_BALL_DISP,
    SEQ_FINISH_NORMAL,
  };
  int setupModelNum = 2;
  if( GetBattleRule() == BTL_RULE_DOUBLE || 
      GetBattleRule() == BTL_RULE_ROYAL )
  {
    setupModelNum = 4;
  }
#if PM_DEBUG
  if( GetBtlvMode() == BTLV_MODE_EFFECT_VIEWER )
  {
    setupModelNum = 4;
  }
#endif

#if PM_DEBUG
  {
    static int befSeq = -1;
    if( befSeq != mSetupSeq )
    {
      GFL_PRINT("BtlvInitSeq[%d]->[%d]\n",befSeq,mSetupSeq);
      befSeq = mSetupSeq;
    }
  }
#endif

  switch( mSetupSeq )
  {
  case SEQ_LOAD_ARC:
    {
      BtlvTickStart(1,"BtlvSetup");
      BtlvTickStart(15,"Setup(LoadArc)");
      {
        gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
        req.arcId = ARCID_WAZAEFF_RESOURCE;
        req.heapForFile = GetResidentHeap();
        req.heapForReq = GetTempHeap()->GetLowerHandle();
#if PM_DEBUG
        if( mDebugParam )
        {
          if( mDebugParam->isHioEffResource )
          {
            req.hioUseSetting       = gfl2::fs::AsyncFileManager::HioUseSetting::ALWAYS_HIO;
            req.arcSrcCreateSetting = gfl2::fs::AsyncFileManager::ArcSrcCreateSetting::ALWAYS_CREATE;
          }
        }
#endif
        GetAsyncFileManager()->AddArcFileOpenReq(req);

        req.arcId = ARCID_BTLV_RESOURCE;
        GetAsyncFileManager()->AddArcFileOpenReq(req);

        req.arcId = ARCID_WAZAEFF_SEQUENCE;
        GetAsyncFileManager()->AddArcFileOpenReq(req);
      }
      {
        gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
        req.heapForFile = GetResidentHeap();
        req.heapForReq = GetTempHeap()->GetLowerHandle();

        req.arcId = ARCID_RENDERPATH;
        GetAsyncFileManager()->AddArcFileOpenReq(req);

        req.arcId = print::GetMessageArcId();
        GetAsyncFileManager()->AddArcFileOpenReq(req);
      }

    }
    mSetupSeq++;
    //break; through
  case SEQ_WAIT_ARC:
    if( GetAsyncFileManager()->IsArcFileOpenFinished(ARCID_WAZAEFF_RESOURCE) == false ||
        GetAsyncFileManager()->IsArcFileOpenFinished(ARCID_WAZAEFF_SEQUENCE) == false ||
        GetAsyncFileManager()->IsArcFileOpenFinished(ARCID_RENDERPATH) == false ||
        GetAsyncFileManager()->IsArcFileOpenFinished(ARCID_BTLV_RESOURCE) == false ||
        GetAsyncFileManager()->IsArcFileOpenFinished(print::GetMessageArcId()) == false )
    {
      return false;
    }
    BtlvTickEnd(15,"Setup(LoadArc)");
    BtlvTickStart(16,"Setup(LoadMsg)");
    mSetupSeq++;
    //break; through

  case SEQ_INIT_RENDER:
    if( mRenderSys->Setup() == false )
    {
      return false;
    }
    if( mMainModule )
    {
      str::StartLoadMessage( mAsyncFileManager , GetResidentHeap() , GetResidentHeap() );
    }
    mSetupSeq++;
    //break; through

  case SEQ_INIT_MSG:
    if( mMainModule )
    {
      if( str::IsFinishLoadMessage(mAsyncFileManager) == false )
      {
        return false;
      }
      mSetupSeq++;
      str::CreateMessage(mAsyncFileManager,GetResidentHeap());
      BtlvTickEnd(16,"Setup(LoadMsg)");
      BtlvTickStart(17,"Setup(SetupUi)");
    }
    else
    {
      mSetupSeq++;
    }
    //break; through
  case SEQ_INIT_UI:
    if( mUiSys->Setup() == false )
    {
      return false;
    }
    BtlvTickEnd(17,"Setup(SetupUi)");
    BtlvTickStart(18,"Setup(BG_Poke)");
    mSetupSeq++;
    //break; through

  case SEQ_WAIT_LOAD_BG:
    //Initializeで作りっぱなしだったやつの待ち
    if( mBgSystem->IsFinishInitialize() == false || 
        mPokeModelSys->IsFinishInitSystemAsync() == false )
    {
      return false;
    }

    mPokeModelSys->CreateSystem(true,GetGLAllocator());

#if PM_DEBUG
    mResidentGLAllocator->DebugVramMemoryInfoDump();
#endif
    BtlvTickEnd(18,"Setup(SetupUi)");
    BtlvTickStart(19,"Setup(Loader)");
    {
      //初期化用
      mPreLoadList[0] = GFL_NEW(GetTempHeap())gfl2::util::List<BtlvLoaderBase*>(GetTempHeap(),32);
      mPreLoadList[1] = GFL_NEW(GetTempHeap())gfl2::util::List<BtlvLoaderBase*>(GetTempHeap(),32);
      mPreLoadList[2] = GFL_NEW(GetTempHeap())gfl2::util::List<BtlvLoaderBase*>(GetTempHeap(),32);

      //登録
      //キャラ影(キャラの初期化に居るので先に
      mPreLoadList[0]->PushBack( GFL_NEW(GetTempHeap())BtlvLoaderCharaShadow(this));
      //ポケモン用追加情報テーブル
      //トレーナー用追加情報テーブル
      //モーションテーブル
      mPreLoadList[0]->PushBack( GFL_NEW(GetTempHeap())BtlvLoaderBattleData(this,&mBattleDataPackBuf,&mBattleDataPack));

      //ポケモン
      mPreLoadList[2]->PushBack( GFL_NEW(GetTempHeap())BtlvLoaderPoke(this,GetPokeModel(BTL_VPOS_NEAR_1),BTL_VPOS_NEAR_1));
      mPreLoadList[1]->PushBack( GFL_NEW(GetTempHeap())BtlvLoaderPoke(this,GetPokeModel(BTL_VPOS_FAR_1) ,BTL_VPOS_FAR_1 ));
      if( setupModelNum > 2 )
      {
        mPreLoadList[2]->PushBack( GFL_NEW(GetTempHeap())BtlvLoaderPoke(this,GetPokeModel(BTL_VPOS_NEAR_2),BTL_VPOS_NEAR_2));
        //@attention 野生ダブルが無いので、手持ちの２体目は後回しでOK
        mPreLoadList[2]->PushBack( GFL_NEW(GetTempHeap())BtlvLoaderPoke(this,GetPokeModel(BTL_VPOS_FAR_2) ,BTL_VPOS_FAR_2 ));
      }

      //トレーナー
      if (IsBattleFesMode())
      {
        mPreLoadList[1]->PushBack(GFL_NEW(GetTempHeap())BtlvLoaderTrainer(this, GetTrainerModelObj(BTL_VPOS_FAR_1), BTL_VPOS_FAR_1));

        mPreLoadList[2]->PushBack(GFL_NEW(GetTempHeap())BtlvLoaderTrainer(this, GetTrainerModelObj(BTL_VPOS_NEAR_1), BTL_VPOS_NEAR_1));
        mPreLoadList[2]->PushBack(GFL_NEW(GetTempHeap())BtlvLoaderTrainer(this, GetTrainerModelObj(BTL_VPOS_NEAR_2), BTL_VPOS_NEAR_2));
        mPreLoadList[2]->PushBack(GFL_NEW(GetTempHeap())BtlvLoaderTrainer(this, GetTrainerModelObj(BTL_VPOS_FAR_2), BTL_VPOS_FAR_2));
      }
      else
      {
        if (GetBattleCompetitor() == BTL_COMPETITOR_WILD)
        {
          //野生のみこの処理の終了タイミングで2万Tickほどかかりカクツキが見えるので対応
          mPreLoadList[2]->PushBack(GFL_NEW(GetTempHeap())BtlvLoaderTrainerAsyncSetup(this, GetTrainerModelObj(BTL_VPOS_NEAR_1), BTL_VPOS_NEAR_1));
          mPreLoadList[2]->PushBack(GFL_NEW(GetTempHeap())BtlvLoaderTrainerAsyncSetup(this, GetTrainerModelObj(BTL_VPOS_NEAR_2), BTL_VPOS_NEAR_2));
        }
        else
        {
          //野生以外はフェードアウトしているんどえ安全性を考慮し保留
          mPreLoadList[2]->PushBack(GFL_NEW(GetTempHeap())BtlvLoaderTrainer(this, GetTrainerModelObj(BTL_VPOS_NEAR_1), BTL_VPOS_NEAR_1));
          mPreLoadList[2]->PushBack(GFL_NEW(GetTempHeap())BtlvLoaderTrainer(this, GetTrainerModelObj(BTL_VPOS_NEAR_2), BTL_VPOS_NEAR_2));
        }

        mPreLoadList[1]->PushBack(GFL_NEW(GetTempHeap())BtlvLoaderTrainer(this, GetTrainerModelObj(BTL_VPOS_FAR_1), BTL_VPOS_FAR_1));
        mPreLoadList[1]->PushBack(GFL_NEW(GetTempHeap())BtlvLoaderTrainer(this, GetTrainerModelObj(BTL_VPOS_FAR_2), BTL_VPOS_FAR_2));

      }

      //背景
      mPreLoadList[1]->PushBack( GFL_NEW(GetTempHeap())BtlvLoaderBg(this));
      //エフェクトモデル
      mPreLoadList[1]->PushBack( GFL_NEW(GetTempHeap())BtlvLoaderEffectModel(this,mBgFadeModel,0));
      mPreLoadList[1]->PushBack( GFL_NEW(GetTempHeap())BtlvLoaderEffectModel(this,mEdgeMaskModel,1));
      mPreLoadList[1]->PushBack( GFL_NEW(GetTempHeap())BtlvLoaderEffectModel(this,mFloorMaskModel,2));

      //ディグダ岩(ポケモンで必要
      mPreLoadList[1]->PushBack( GFL_NEW(GetTempHeap())BtlvLoaderEffectResource(this,&mDigudaStoneResource,GARC_wazaeff_resource_digda_rock_lz_GFBMDLP));

      //DLL
      mPreLoadList[2]->PushBack( GFL_NEW(GetTempHeap())BtlvLoaderDll(this));

      //UI用メッセージ
      mPreLoadList[2]->PushBack( GFL_NEW(GetTempHeap())BtlvLoaderUiMessage(this));

      //初期天候
      if( GetFieldSituation() )
      {
        mNowWeather = GetFieldSituation()->weather;
      }
      else
      {
        mNowWeather = BTL_WEATHER_NONE;
      }
      mPreLoadList[1]->PushBack( GFL_NEW(GetTempHeap())BtlvLoaderWeather(this));

    }
    mSetupSeq++;
    //break; through


  case SEQ_CREATE_TEMP_DATA1:
    DataLoadStart(0);
    mSetupSeq++;
    //break; through

  case SEQ_CREATE_TEMP_DATA_WAIT1:
    DataLoadUpdateLoad(0);
    if( DataLoadWait(0) == false )
    {
      return false;
    }
    DataLoadEnd(0);
    mSetupSeq++;
    //break; through

  case SEQ_CREATE_TEMP_DATA2:
    DataLoadStart(1);
    mSetupSeq++;
    //break; through

  case SEQ_CREATE_TEMP_DATA_WAIT2:
    DataLoadUpdateLoad(1);
    if( DataLoadWait(1) == false )
    {
      return false;
    }
    DataLoadEnd(1);

    if( IsViewerMode() )
    {
      //ビュワーは終了
      mSetupSeq = SEQ_SKIP_LOAD;
    }
    else
    if( IsQuickStart() )
    {
      //スキップモード
      mSetupSeq = SEQ_SKIP_LOAD;
    }
    else
    {
      if( GetBattleRule() == BTL_RULE_ROYAL )
      {
        mSetupSeq = SEQ_START_ROYAL_IN;
      }
      else
      if( GetBattleCompetitor() == BTL_COMPETITOR_WILD )
      {
         mSetupSeq = SEQ_START_EFFECT_WILD;
      }
      else
      if( GetBattleCompetitor() == BTL_COMPETITOR_COMM )
      {
        //通信対戦
        //施設のタッグはBTL_COMPETITOR_INSTになるのでここにこない
         mSetupSeq = SEQ_START_COMM_IN;
      }
      else if( GetClientPlayerData( BTL_CLIENT_ENEMY1 ) )
      {
        //通信対戦の模擬戦
        mSetupSeq = SEQ_START_COMM_IN;
      }
      else
      {
        //トレーナー、施設
         mSetupSeq = SEQ_START_EFFECT_TRAINER_ENEMY_LOGO;
      }
      //それ以外は演出シーケンス
      BtlvTickEnd(1,"BtlvSetup");
      BtlvTickEnd(2,"BattleProcStart");
      BtlvTickEnd(19,"Setup(Loader)");

      BtlvTickStart(3,"LoadSeq");
    }
    DumpBattleHeap();
    break;

  case SEQ_FINISH_VIEWER:
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA);
    return true;
    //break;

  //----------------------------------------------------------
  //通信対戦(プレイヤーとの模擬戦も含む
  case SEQ_START_COMM_IN:
    mIsCommIntro = true;
    SetComTargetReset();
    mComTargetPoke[0] = BTL_VPOS_FAR_1;
    mComTargetPoke[1] = BTL_VPOS_FAR_2;  //ダブル対応
    mTargetTrainer[0] = BTL_VPOS_FAR_1;
    mTargetTrainer[1] = BTL_VPOS_FAR_2;
    mEffectBallId[0] = GetBattlePokeParam(BTL_VPOS_FAR_1)->GetViewSrcData()->GetGetBall();
    if( GetBattlePokeParam(BTL_VPOS_FAR_2) )
    {
      mEffectBallId[1] = GetBattlePokeParam(BTL_VPOS_FAR_2)->GetViewSrcData()->GetGetBall();
    }

    if( IsMultiMode() == true )
    {
      SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee381_BSEQ );
    }
    else
    {
      SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee380_BSEQ );
    }
    mSetupSeq++;
    //break; througth

  case SEQ_START_COMM_IN_WAIT_LOAD:
    {
      //ここではなく下で待つ。ただ更新のために呼ぶ必要はある。
      bool ret = SeqComFunc_IsFinishSequenceFile();
    }
    if( mIsLoadSequenceResource )
    {
      return false;
    }
    DataLoadStartOne(2);
    mSetupSeq++;
    //break; througth
  case SEQ_START_COMM_IN_WAIT:
    DataLoadUpdateLoadOne(2);
    if( SeqComFunc_IsFinishSequenceFile(true) == false)
    {
      return false;
    }
    if( DataLoadWait(2) == false )
    {
      return false;
    }

    DataLoadEnd(2);
    BtlvTickPrint();
    mSetupSeq++;
    //break; through

  case SEQ_START_EFFECT_COMM_FIRST_THROW:
    SetComTargetReset();
    if( GetClientID() == 0 || GetClientID() == 2 )
    {
      //相手先、自分後
      mComTargetPoke[0] = BTL_VPOS_FAR_1;
      mComTargetPoke[1] = BTL_VPOS_FAR_2;
      mTargetTrainer[0] = BTL_VPOS_FAR_1;
      mTargetTrainer[1] = BTL_VPOS_FAR_2;
      mEffectBallId[0] = GetBattlePokeParam(BTL_VPOS_FAR_1)->GetViewSrcData()->GetGetBall();
      if( GetBattlePokeParam(BTL_VPOS_FAR_2) )
      {
        mEffectBallId[1] = GetBattlePokeParam(BTL_VPOS_FAR_2)->GetViewSrcData()->GetGetBall();
      }
      if( IsMultiMode() == true )
      {
        SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee409_BSEQ );
      }
      else
      if( GetBattleRule() == BTL_RULE_DOUBLE )
      {
        SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee408_BSEQ );
      }
      else
      {
        //LAND_A,B,CかLANDかでモーションを分岐する
        if( GetPokeModel(BTL_VPOS_FAR_1)->IsAvailableAnimation(PokeTool::MODEL_ANIME_LAND) )
        {
          SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee411_BSEQ );
        }
        else
        {
          SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee407_BSEQ );
        }
      }
    }
    else
    {
      //自分先、相手後
      mComTargetPoke[0] = BTL_VPOS_NEAR_1;
      mComTargetPoke[1] = BTL_VPOS_NEAR_2;  //ダブル対応
      mTargetTrainer[0] = BTL_VPOS_NEAR_1;
      mTargetTrainer[1] = BTL_VPOS_NEAR_2;
      mEffectBallId[0] = GetBattlePokeParam(BTL_VPOS_NEAR_1)->GetViewSrcData()->GetGetBall();
      if( GetBattlePokeParam(BTL_VPOS_NEAR_2) )
      {
        mEffectBallId[1] = GetBattlePokeParam(BTL_VPOS_NEAR_2)->GetViewSrcData()->GetGetBall();
      }
      //マルチ演出
      if( IsMultiMode() == true )
      {
        SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee305_BSEQ );
      }
      else
      if( GetBattleRule() == BTL_RULE_DOUBLE )
      {
        SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee304_BSEQ );
      }
      else
      {
        //LAND_A,B,CかLANDかでモーションを分岐する
        if( GetPokeModel(BTL_VPOS_NEAR_1)->IsAvailableAnimation(PokeTool::MODEL_ANIME_LAND) )
        {
          SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee306_BSEQ );
        }
        else
        {
          SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee303_BSEQ );
        }
      }
    }
    mSetupSeq++;
    //break; through

  case SEQ_START_EFFECT_COMM_FIRST_THROW_WAIT:
    if( SeqComFunc_IsFinishSequenceFile() == false)
    {
      return false;
    }
    mSetupSeq++;
    //break; through

  case SEQ_START_EFFECT_COMM_SECOND_THROW:
    SetComTargetReset();
    if( GetClientID() == 0 || GetClientID() == 2 )
    {
      mComTargetPoke[0] = BTL_VPOS_NEAR_1;
      mComTargetPoke[1] = BTL_VPOS_NEAR_2;  //ダブル対応
      mTargetTrainer[0] = BTL_VPOS_NEAR_1;
      mTargetTrainer[1] = BTL_VPOS_NEAR_2;
      mEffectBallId[0] = GetBattlePokeParam(BTL_VPOS_NEAR_1)->GetViewSrcData()->GetGetBall();
      if( GetBattlePokeParam(BTL_VPOS_NEAR_2) )
      {
        mEffectBallId[1] = GetBattlePokeParam(BTL_VPOS_NEAR_2)->GetViewSrcData()->GetGetBall();
      }
      //マルチ演出
      if( IsMultiMode() == true )
      {
        SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee305_BSEQ );
      }
      else
      if( GetBattleRule() == BTL_RULE_DOUBLE )
      {
        SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee304_BSEQ );
      }
      else
      {
        //LAND_A,B,CかLANDかでモーションを分岐する
        if( GetPokeModel(BTL_VPOS_NEAR_1)->IsAvailableAnimation(PokeTool::MODEL_ANIME_LAND) )
        {
          SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee306_BSEQ );
        }
        else
        {
          SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee303_BSEQ );
        }
      }
    }
    else
    {
      //自分先、相手後
      //相手先、自分後
      mComTargetPoke[0] = BTL_VPOS_FAR_1;
      mComTargetPoke[1] = BTL_VPOS_FAR_2;
      mTargetTrainer[0] = BTL_VPOS_FAR_1;
      mTargetTrainer[1] = BTL_VPOS_FAR_2;
      mEffectBallId[0] = GetBattlePokeParam(BTL_VPOS_FAR_1)->GetViewSrcData()->GetGetBall();
      if( GetBattlePokeParam(BTL_VPOS_FAR_2) )
      {
        mEffectBallId[1] = GetBattlePokeParam(BTL_VPOS_FAR_2)->GetViewSrcData()->GetGetBall();
      }
      if( IsMultiMode() == true )
      {
        SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee409_BSEQ );
      }
      else
      if( GetBattleRule() == BTL_RULE_DOUBLE )
      {
        SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee408_BSEQ );
      }
      else
      {
        //LAND_A,B,CかLANDかでモーションを分岐する
        if( GetPokeModel(BTL_VPOS_FAR_1)->IsAvailableAnimation(PokeTool::MODEL_ANIME_LAND) )
        {
          SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee411_BSEQ );
        }
        else
        {
          SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee407_BSEQ );
        }
      }
    }
    mSetupSeq++;
    //break; through

  case SEQ_START_EFFECT_COMM_SECOND_THROW_WAIT:
    if( SeqComFunc_IsFinishSequenceFile() == false)
    {
      return false;
    }
    //順番が入れ替わるためシーケンスの締めがフェードになっている。なのでこちらであける。
    ResetDefaultCamera(0);
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA);
    mSetupSeq = SEQ_GAUGE_IN;
    break;
  //----------------------------------------------------------
  //トレーナー
  case SEQ_START_EFFECT_TRAINER_ENEMY_LOGO:
    //導入演出中にキャラを暗くしないため
    mPlayingTrainerIntro = true;
    SetComTargetReset();
    mComTargetPoke[0] = BTL_VPOS_FAR_1;
    mComTargetPoke[1] = BTL_VPOS_FAR_2;  //ダブル対応
    mTargetTrainer[0] = BTL_VPOS_FAR_1;
    mEffectBallId[0] = GetBattlePokeParam(BTL_VPOS_FAR_1)->GetViewSrcData()->GetGetBall();
    {
      u32 seqFile = BTLV_CUTIN_SEQUENCE_LIST[GetEncountEffect()->eff_kind];
      if( seqFile == GARC_wazaeff_seqfile_ee319_BSEQ ||
          seqFile == GARC_wazaeff_seqfile_ee330_BSEQ ||
          seqFile == GARC_wazaeff_seqfile_ee331_BSEQ ||
          seqFile == GARC_wazaeff_seqfile_ee332_BSEQ )
      {
        //スカル団ロゴ
        SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee337_BSEQ );
        mSetupSeq++;
      }
      else
      if( seqFile == GARC_wazaeff_seqfile_ee333_BSEQ ||
          seqFile == GARC_wazaeff_seqfile_ee334_BSEQ ||
          seqFile == GARC_wazaeff_seqfile_ee335_BSEQ )
      {
        //エーテル財団ロゴ
        SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee338_BSEQ );
        mSetupSeq++;
      }
      else
      {
        mSetupSeq = SEQ_START_EFFECT_TRAINER_ENEMY_IN;
      }
    }
    break;
  case SEQ_START_EFFECT_TRAINER_ENEMY_LOGO_WAIT:
    if( SeqComFunc_IsFinishSequenceFile() == false)
    {
      return false;
    }
    mSetupSeq++;
    //break; through

  case SEQ_START_EFFECT_TRAINER_ENEMY_IN:
    
    //マルチ演出
    if( IsClientTrainerExist(BTL_VPOS_TR_FAR_2) )
    {
      mTargetTrainer[1] = BTL_VPOS_FAR_2;
      mEffectBallId[1] = GetBattlePokeParam(BTL_VPOS_FAR_2)->GetViewSrcData()->GetGetBall();

      u16 tyType1 = mTrainerModel[BTL_VPOS_FAR_1]->GetTrainerType();
      u16 tyType2 = mTrainerModel[BTL_VPOS_FAR_2]->GetTrainerType();
      if( (tyType1 == trainer::TRTYPE_SKULLM ||tyType1 == trainer::TRTYPE_SKULLW) &&
          (tyType2 == trainer::TRTYPE_SKULLM ||tyType2 == trainer::TRTYPE_SKULLW) )
      {
        //スカル団
        SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee331_BSEQ );
      }
      else
        if( (tyType1 == trainer::TRTYPE_ETHERESTAFFM ||tyType1 == trainer::TRTYPE_ETHERESTAFFW ||tyType1 == trainer::TRTYPE_ETHERCHIEF) &&
            (tyType2 == trainer::TRTYPE_ETHERESTAFFM ||tyType2 == trainer::TRTYPE_ETHERESTAFFW ||tyType2 == trainer::TRTYPE_ETHERCHIEF) )
      {
        //エーテル財団
        SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee334_BSEQ );
      }
      else
      if ((tyType1 == trainer::TRTYPE_TRED || tyType1 == trainer::TRTYPE_TGREEN) &&
        (tyType2 == trainer::TRTYPE_TRED || tyType2 == trainer::TRTYPE_TGREEN))
      {
        //タワーボス
        SeqComFunc_PlaySequenceFile(GARC_wazaeff_seqfile_ee314_BSEQ);
      }
      else
        if ((tyType1 == trainer::TRTYPE_ROCKETM || tyType1 == trainer::TRTYPE_ROCKETW) &&
            (tyType2 == trainer::TRTYPE_ROCKETM || tyType2 == trainer::TRTYPE_ROCKETW))
      {
        //momiji追加 RR団
        SeqComFunc_PlaySequenceFile(GARC_wazaeff_seqfile_ee323_BSEQ);
      }
      else
      {
        SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee312_BSEQ );
      }
    }
    else
    {
      u32 seqFile = BTLV_CUTIN_SEQUENCE_LIST[GetEncountEffect()->eff_kind];
      SeqComFunc_PlaySequenceFile(seqFile);
    }
    mSetupSeq++;
    //break; through
  case SEQ_START_EFFECT_TRAINER_ENEMY_IN_WAIT_LOAD:
    {
      //ここではなく下で待つ。ただ更新のために呼ぶ必要はある。
      bool ret = SeqComFunc_IsFinishSequenceFile(true);
    }
    if( mIsLoadSequenceResource )
    {
      return false;
    }
    DataLoadStartOne(2);
    mSetupSeq++;
  case SEQ_START_EFFECT_TRAINER_ENEMY_IN_WAIT:
    DataLoadUpdateLoadOne(2);
    if( SeqComFunc_IsFinishSequenceFile(true) == false)
    {
      return false;
    }
    if( DataLoadWait(2) == false )
    {
      return false;
    }
    DataLoadEnd(2);
    BtlvTickPrint();
    mSetupSeq++;
    //break; through

  case SEQ_START_EFFECT_TRAINER_ENEMY_THROW:
    SetComTargetReset();
    //導入演出中にキャラを暗くしないため
    mPlayingTrainerIntro = false;
    mComTargetPoke[0] = BTL_VPOS_FAR_1;
    mComTargetPoke[1] = BTL_VPOS_FAR_2; //ダブル対応
    mTargetTrainer[0] = BTL_VPOS_FAR_1;
    mEffectBallId[0] = GetBattlePokeParam(BTL_VPOS_FAR_1)->GetViewSrcData()->GetGetBall();
    //マルチ演出
    if( IsClientTrainerExist(BTL_VPOS_TR_FAR_2) )
    {
      mTargetTrainer[1] = BTL_VPOS_FAR_2;
      mEffectBallId[1] = GetBattlePokeParam(BTL_VPOS_FAR_2)->GetViewSrcData()->GetGetBall();
      SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee402_BSEQ );
    }
    else
    if( GetBattleRule() == BTL_RULE_DOUBLE )
    {
      mEffectBallId[1] = GetBattlePokeParam(BTL_VPOS_FAR_2)->GetViewSrcData()->GetGetBall();
      SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee401_BSEQ );
    }
    else
    {
      u16 seqFile = GARC_wazaeff_seqfile_ee400_BSEQ;
      if (GetSetupStatusFlag(BTL_STATUS_FLAG_TRAINER_NO_BALL))
      {
        u32 enemyInSeqFile = BTLV_CUTIN_SEQUENCE_LIST[GetEncountEffect()->eff_kind];

        if (enemyInSeqFile == GARC_wazaeff_seqfile_ee400_3_BSEQ)
        {
          //ハウ用
          seqFile = GARC_wazaeff_seqfile_ee400_2_BSEQ;
        }
        else
        {
          //ビースト世界用
          seqFile = GARC_wazaeff_seqfile_ee400_1_BSEQ;
        }
      }
      else
      {
        u16 cutinSeq = BTLV_CUTIN_SEQUENCE_LIST[GetEncountEffect()->eff_kind];
        if( cutinSeq == GARC_wazaeff_seqfile_ee315_BSEQ ||
            cutinSeq == GARC_wazaeff_seqfile_ee316_BSEQ ||
            cutinSeq == GARC_wazaeff_seqfile_ee317_BSEQ ||
            cutinSeq == GARC_wazaeff_seqfile_ee318_BSEQ ||
            cutinSeq == GARC_wazaeff_seqfile_ee319_BSEQ ||
            cutinSeq == GARC_wazaeff_seqfile_ee320_BSEQ ||
            cutinSeq == GARC_wazaeff_seqfile_ee335_BSEQ ||
            cutinSeq == GARC_wazaeff_seqfile_ee336_BSEQ ||
            cutinSeq == GARC_wazaeff_seqfile_ee344_BSEQ ||
            cutinSeq == GARC_wazaeff_seqfile_ee345_BSEQ ||
            cutinSeq == GARC_wazaeff_seqfile_ee346_BSEQ ||
            cutinSeq == GARC_wazaeff_seqfile_ee347_BSEQ ||
            cutinSeq == GARC_wazaeff_seqfile_ee360_BSEQ ||
            cutinSeq == GARC_wazaeff_seqfile_ee361_BSEQ ||
            cutinSeq == GARC_wazaeff_seqfile_ee362_BSEQ ||
            cutinSeq == GARC_wazaeff_seqfile_ee363_BSEQ ||
            cutinSeq == GARC_wazaeff_seqfile_ee339_BSEQ ||  //以下momiji追加
            cutinSeq == GARC_wazaeff_seqfile_ee364_BSEQ ||
            cutinSeq == GARC_wazaeff_seqfile_ee321_BSEQ)
        {
          seqFile = GARC_wazaeff_seqfile_ee410_BSEQ;
        }
        //LAND_A,B,CかLANDかでモーションを分岐する
        if( GetPokeModel(BTL_VPOS_FAR_1)->IsAvailableAnimation(PokeTool::MODEL_ANIME_LAND) )
        {
          if( seqFile == GARC_wazaeff_seqfile_ee400_BSEQ )
          {
            //通常の方
            seqFile = GARC_wazaeff_seqfile_ee404_BSEQ;
          }
          else
          {
            //重要用短縮
            seqFile = GARC_wazaeff_seqfile_ee412_BSEQ;
          }
        }
      }

      SeqComFunc_PlaySequenceFile( seqFile );
    }
    mSetupSeq++;
    //break; through

  case SEQ_START_EFFECT_TRAINER_ENEMY_THROW_WAIT:
    if( SeqComFunc_IsFinishSequenceFile() == false)
    {
      return false;
    }
    mSetupSeq++;
    //break; through

  case SEQ_START_EFFECT_TRAINER_SELF:
    SetComTargetReset();
    mComTargetPoke[0] = BTL_VPOS_NEAR_1;
    mComTargetPoke[1] = BTL_VPOS_NEAR_2;  //ダブル対応
    mTargetTrainer[0] = BTL_VPOS_NEAR_1;
    mEffectBallId[0] = GetBattlePokeParam(BTL_VPOS_NEAR_1)->GetViewSrcData()->GetGetBall();
    //マルチ演出
    if( IsClientTrainerExist(BTL_VPOS_NEAR_2) )
    {
      mTargetTrainer[1] = BTL_VPOS_NEAR_2;
      mEffectBallId[1] = GetBattlePokeParam(BTL_VPOS_NEAR_2)->GetViewSrcData()->GetGetBall();
      SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee305_BSEQ );
    }
    else
    if( GetBattleRule() == BTL_RULE_DOUBLE )
    {
      mEffectBallId[1] = GetBattlePokeParam(BTL_VPOS_NEAR_2)->GetViewSrcData()->GetGetBall();
      SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee304_BSEQ );
    }
    else
    {
      //LAND_A,B,CかLANDかでモーションを分岐する
      if( GetPokeModel(BTL_VPOS_NEAR_1)->IsAvailableAnimation(PokeTool::MODEL_ANIME_LAND) )
      {
        SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee306_BSEQ );
      }
      else
      {
        SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee303_BSEQ );
      }
    }
    mSetupSeq++;
    //break; through

  case SEQ_START_EFFECT_TRAINER_SELF_WAIT:
    if( SeqComFunc_IsFinishSequenceFile() == false)
    {
      return false;
    }
    mSetupSeq = SEQ_GAUGE_IN;
    break;

  //----------------------------------------------------------
  //野生
  case SEQ_START_EFFECT_WILD:
    SetComTargetReset();
    mComTargetPoke[0] = BTL_VPOS_NEAR_1;
    mComTargetPoke[1] = BTL_VPOS_FAR_1;
    {
      u32 seqFile = BTLV_CUTIN_SEQUENCE_LIST[GetEncountEffect()->eff_kind];
      SeqComFunc_PlaySequenceFile( seqFile );
    }
    mSetupSeq++;
    //break; through
  case SEQ_START_EFFECT_WILD_WAIT_LOAD:
    {
      //ここではなく下で待つ。ただ更新のために呼ぶ必要はある。
      bool ret = SeqComFunc_IsFinishSequenceFile(true);
    }
    if( mIsLoadSequenceResource )
    {
      return false;
    }
    DataLoadStartOne(2);
    mSetupSeq++;
  case SEQ_START_EFFECT_WILD_WAIT:
    DataLoadUpdateLoadOne(2);
    if( SeqComFunc_IsFinishSequenceFile() == false)
    {
      return false;
    }
    if( DataLoadWait(2) == false )
    {
      return false;
    }
    DataLoadEnd(2);
    BtlvTickPrint();
    {
      BTL_POKEPARAM *bpp = GetBattlePokeParam(BTL_VPOS_FAR_1);
      if (IsEnableRotomSearch(bpp) == true)
      {
        RegistLookedMonsNo(bpp);
        mSetupSeq++;
      }
      else
      {
        mSetupSeq = SEQ_START_EFFECT_WILD_SELF;
      }
    }
    break;

  case SEQ_START_EFFECT_WILD_ROTOM_LOAD:
    SetComTargetReset();
    mComTargetPoke[0] = BTL_VPOS_FAR_1;

    SeqComFunc_PlaySequenceFile(GARC_wazaeff_seqfile_er001_BSEQ);

    mSetupSeq++;

  case SEQ_START_EFFECT_WILD_ROTOM:
    if (SeqComFunc_IsFinishSequenceFile() == false)
    {
      return false;
    }

    mSetupSeq++;
  
  case SEQ_START_EFFECT_WILD_SELF:
    SetComTargetReset();
    mComTargetPoke[0] = BTL_VPOS_NEAR_1;
    mComTargetPoke[1] = BTL_VPOS_NEAR_2;  //ダブル対応
    mTargetTrainer[0] = BTL_VPOS_NEAR_1;
    mEffectBallId[0] = GetBattlePokeParam(BTL_VPOS_NEAR_1)->GetViewSrcData()->GetGetBall();
    //LAND_A,B,CかLANDかでモーションを分岐する
    if( GetPokeModel(BTL_VPOS_NEAR_1)->IsAvailableAnimation(PokeTool::MODEL_ANIME_LAND) )
    {
      SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee306_BSEQ );
    }
    else
    {
      SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee303_BSEQ );
    }
    mSetupSeq++;
    //break; through

  case SEQ_START_EFFECT_WILD_SELF_WAIT:
    if( SeqComFunc_IsFinishSequenceFile() == false)
    {
      return false;
    }
    mSetupSeq = SEQ_GAUGE_IN;
    break;

  //----------------------------------------------------------
  //ロイヤル
  case SEQ_START_ROYAL_IN:
    //ロイヤル開始演出
    SetComTargetReset();
    SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee403_1_BSEQ );
    DataLoadStartOne(2);
    mSetupSeq++;
    //break; through

  case SEQ_START_ROYAL_IN_WAIT:
    DataLoadUpdateLoadOne(2);
    if( SeqComFunc_IsFinishSequenceFile(true) == false)
    {
      return false;
    }
    if( DataLoadWait(2) == false )
    {
      return false;
    }
    DataLoadEnd(2);
    BtlvTickPrint();
    mComCnt = 0;
    mSetupSeq = SEQ_START_ROYAL_TRAINER;
    //break; through

  case SEQ_START_ROYAL_TRAINER:
    //一人ひとりの投げ
    SetComTargetReset();
    mComTargetPoke[0] = GetClientIdToBtlvPos(mComCnt);
    mTargetTrainer[0] = GetClientIdToBtlvPos(mComCnt);

    mEffectBallId[0] = GetBattlePokeParam(mComTargetPoke[0])->GetViewSrcData()->GetGetBall();

    
    {
      u8 clientId = GetBtlvPosToClientId(mComTargetPoke[0]);
      if( IsClientNpc(clientId) )
      {
        //LAND_A,B,CかLANDかでモーションを分岐する
        if( GetPokeModel(mComTargetPoke[0])->IsAvailableAnimation(PokeTool::MODEL_ANIME_LAND) )
        {
          SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee403_2_2L_BSEQ );
        }
        else
        {
          SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee403_2_2_BSEQ );
        }
      }
      else
      {
        //LAND_A,B,CかLANDかでモーションを分岐する
        if( GetPokeModel(mComTargetPoke[0])->IsAvailableAnimation(PokeTool::MODEL_ANIME_LAND) )
        {
          SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee403_2L_BSEQ );
        }
        else
        {
          SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee403_2_BSEQ );
        }
      }
    }

    mSetupSeq++;

  case SEQ_START_ROYAL_TRAINER_WAIT:
    if( SeqComFunc_IsFinishSequenceFile(true) == false)
    {
      return false;
    }
    mComCnt++;
    if( mComCnt < 4 )
    {
      mSetupSeq = SEQ_START_ROYAL_TRAINER;
    }
    else
    {
      mSetupSeq = SEQ_START_ROYAL_END;
    }
    break;  //ここはブレイクする！
 
  case SEQ_START_ROYAL_END:
    //ロイヤル開始演出
    SetComTargetReset();
    SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee403_3_BSEQ );
    mSetupSeq++;
    //break; through

  case SEQ_START_ROYAL_END_WAIT:
    if( SeqComFunc_IsFinishSequenceFile() == false)
    {
      return false;
    }
    mComCnt = 0;
    mSetupSeq = SEQ_GAUGE_IN;
    break;

  //----------------------------------------------------------
  //スキップ用処理
  case SEQ_SKIP_LOAD:
    DataLoadStartOne(2);
    mSetupSeq++;
    //break; through

  case SEQ_SKIP_LOAD_WAIT:
    DataLoadUpdateLoadOne(2);
    if( DataLoadWait(2) == false )
    {
      return false;
    }
    DataLoadEnd(2);

    for( int i=0;i<setupModelNum;i++ )
    {
      GetPokeModel((BtlvPos)i)->SetVisible(true);
    }
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA);
    mSetupSeq++;
    //break; through

  //----------------------------------------------------------
  //共通
  case SEQ_GAUGE_IN:
    if( GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd(gfl2::Fade::DISP_UPPER) == false ||
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd(gfl2::Fade::DISP_LOWER) == false ||
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd(gfl2::Fade::DISP_CUSTOM_UPPER) == false )
    {
      break;
    }
    {
      //VRAM転送中にやるとまずいのでここでONにする
      EnableFrameSkip(true);

      for (int i = 0; i < setupModelNum; i++)
      {
        GetPokeModel((BtlvPos)i)->StartWaitAnime(true,true);
      }

      ResetDefaultCamera(0);
      GetUiSys()->AllInGauge();
      GetUiSys()->DispBallBarAll(); //表示だけ。導入アニメ未再生ならここで出る
      GetUiSys()->UpdateBallBarAll(); //ロイヤルは導入アニメが出ないので、更新が必要
      //  UIの導入演出開始
      GetUiSys()->StartFirstIntroEvent();

      GFL_SAFE_DELETE( mPreLoadList[0] );
      GFL_SAFE_DELETE( mPreLoadList[1] );
      GFL_SAFE_DELETE( mPreLoadList[2] );
      mSetupSeq++;
    }
    break;
  case SEQ_BALL_DISP:
    //GetUiSys()->WaitAllInGauge() 高速化のためにこれは待たない
    if( GetUiSys()->WaitFirstIntroEvent() == false )
    {
      mSetupSeq++;
    }
    break;
  case SEQ_FINISH_NORMAL:

#if PM_DEBUG
    mResidentGLAllocator->DebugVramMemoryInfoDump();
#endif
    return true;
    //break;

  }
  return false;
}


bool BattleViewSystem::CMD_WaitSetup_RecReset(void)
{
  //バトルレコーダーリセット時のパラメーター
  enum
  {
    SEQ_INIT,
    SEQ_RELOAD_POKE,
    SEQ_RELOAD_POKE_WAIT,
    SEQ_RESET_WEATHER,
    SEQ_RESET_WEATHER_WAIT,
    SEQ_RESET_ZENRYOKU,
    SEQ_RESET_ZENRYOKU_WAIT,
    SEQ_END,
    SEQ_END_WAIT,
  };
  int setupModelNum = 2;
  if( GetBattleRule() == BTL_RULE_DOUBLE || 
      GetBattleRule() == BTL_RULE_ROYAL )
  {
    setupModelNum = 4;
  }
#if PM_DEBUG
  if( GetBtlvMode() == BTLV_MODE_EFFECT_VIEWER )
  {
    setupModelNum = 4;
  }
#endif
  switch( mComScene )
  {
  case SEQ_INIT:
    mComCnt = 0;
    mComScene++;
    break;
  case SEQ_RELOAD_POKE: //1体ずつ読み込む
    if( mComCnt >= setupModelNum )
    {
      mComScene = SEQ_RESET_WEATHER;
    }
    else
    if( mPokeModel[mComCnt] /*&& mPokeModel[mComCnt]->IsEnable()*/ )
    {
      //ここでIsEnableの判定をしているとスキップ開始時に死んでいるポケモンが作られない
      CreateTask_ReloadPokemon( GetTaskSystemAlways() , GetTempHeap() , this , static_cast<BtlvPos>(mComCnt) );
      mComScene++;
    }
    else
    {
      mComCnt++;
    }
    break;
  case SEQ_RELOAD_POKE_WAIT:
    if( IsLoadPokeModel() == false )
    {
      mComScene = SEQ_RELOAD_POKE;
      mComCnt++;
    }
    break;

  case SEQ_RESET_WEATHER:
    LoadWeatherResource(SEQ_DEF_WEATHER_NONE);
    mNowWeather = SEQ_DEF_WEATHER_NONE;
    mComScene++;
    //break;
  case SEQ_RESET_WEATHER_WAIT:
    if( IsFinishLoadWeatherResource() == false )
    {
      return false;
    }
    StartWeather(SEQ_DEF_WEATHER_NONE);
    mComScene++;
    //break;
  case SEQ_RESET_ZENRYOKU:
    if( mZenryokuParam.isZenryoku == true )
    {
      SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ez_cmn_failue_BSEQ );
      mComScene++;
    }
    else
    {
      mComScene = SEQ_END;
      return false;
    }
    //break;

  case SEQ_RESET_ZENRYOKU_WAIT:
    if( SeqComFunc_IsFinishSequenceFile() == false )
    {
      return false;
    }
    mComScene++;
    //break;
  case SEQ_END:
    for( int i=0;i<POKE_MODEL_NUM;i++ )
    {
      if( mPokeModel[i] )
      {
        mPokeModel[i]->SetVisible(true);
        mPokeModel[i]->SetHideMigawariZenryoku(false);
        mPokeModel[i]->SetVisibleMigawari(false);
      }
    }
    for( int i=0;i<TRAINER_MODEL_NUM;i++ )
    {
      if( mTrainerModel[i] )
      {
        mTrainerModel[i]->SetVisible(true);
        ResetTrainer(i);
      }
    }
    /*
    GetBgSystem()->RequestBattleWeather( BTL_WEATHER_NONE );
    GetSkySys()->SetWeather(BTL_WEATHER_NONE,0);
    */
    if( mFieldWazaType != BTL_GROUND_NONE )
    {
      StartLoadFieldWaza(BTL_GROUND_NONE);
    }
    ResetZenryokuParam();
    GetUiSys()->HideTokWin( BTL_VPOS_NEAR_1 );
    GetUiSys()->HideTokWin( BTL_VPOS_FAR_1 );
    mComScene++;
    break;
  case SEQ_END_WAIT:
    if( GetUiSys()->WaitHideTokWin() &&
        IsFinishLoadFieldWaza() )
    {
      return true;
    }
    break;
  }
  return false;
}

//開放
void BattleViewSystem::CMD_StartCleanUp(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;
  //ここはエラー時複数とおるのでmCleanupSeqの初期化はしないで置く
}

bool BattleViewSystem::CMD_WaitCleanUp(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);

  enum
  {
    SEQ_END_CAMERA = 0,
    SEQ_WAIT_END_CAMERA,
    SEQ_FADE,
    SEQ_DELETE_BG,
    SEQ_WAIT_DELETE_BG,
    SEQ_DELETE_POKE,
    SEQ_DELETE_POKE_SYS,
    SEQ_WAIT_DELETE_POKE_SYS,
    SEQ_CLEAN_UI,
    SEQ_CLEAN_RENDER,
    SEQ_CLOSE_ARC,
    SEQ_WAIT_CLOSE_ARC,
    SEQ_FINISH,
  };

#if PM_DEBUG
  {
    static int befSeq = -1;
    if( befSeq != mCleanupSeq )
    {
      GFL_PRINT("BtlvCleanSeq[%d]->[%d]\n",befSeq,mCleanupSeq);
      befSeq = mCleanupSeq;
    }
  }
#endif

  if( mSetupSeq == 0 )
  {
    return true;
  }

  switch( mCleanupSeq )
  {
  case SEQ_END_CAMERA:
    //待機演出で行われている状態異常と待機カメラの停止
    GetCameraSys()->EndWaitCamera();
    Finish_WaitCamSickEffect();
    mCleanupSeq++;
    //break;  //through
                      
  case SEQ_WAIT_END_CAMERA:
    Update_WaitCamSickEffect(); //WaitCamSickEffectのために必要
    if( GetCameraSys()->IsFinishWaitCamera() == false ||
        IsFinish_WaitCamSickEffect() == false )
    {
      return false;
    }
    mCleanupSeq++;
    //break;  //through

  case SEQ_FADE:
    //@attention 可愛がりぬけの時はフェードしてる
    //@attention バトルビデオの終了時の時はフェードしている
    if( mIsFinishFade == false )
    {
      gfl2::math::Vector4 sCol(0,0,0,0);
      gfl2::math::Vector4 eCol(0,0,0,255);
      //AppLib::Fade::FADE_DEFAULT_SYNC デフォルト2フレ
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA,&sCol,&eCol,4);
    }
    else
    if(IsRecordPlayMode()==true )
    {
      //レコーダーの時下画面はフェードしてない
      gfl2::math::Vector4 sCol(0,0,0,0);
      gfl2::math::Vector4 eCol(0,0,0,255);
      //AppLib::Fade::FADE_DEFAULT_SYNC デフォルト2フレ
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA,&sCol,&eCol,4);
      mIsFinishFade = false; //チェックのために落とす
    }
    mCleanupSeq++;
    break;
  case SEQ_DELETE_BG:
    if( mIsFinishFade == false )
    {
      if( GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd(gfl2::Fade::DISP_DOUBLE) == false )
      {
        return false;
      }
      mIsFinishFade = true;
    }

    //描画終了
    mIsEnableDraw = false;
    mBgSystem->Delete();
    mCleanupSeq++;
    //break; through
  case SEQ_WAIT_DELETE_BG:
    if( mBgSystem->IsFinishDelete() == false )
    {
      return false;
    }
    mCleanupSeq++;
    //break; through

  case SEQ_DELETE_POKE:
    for( int i=0;i<POKEMODEL_NUM;i++ )
    {
      mPokeModel[i]->RemoveRenderPath();
    }
    mCleanupSeq++;
    break;//待つ

  case SEQ_DELETE_POKE_SYS:
    if( mRenderSys->GetRenderingPipeLine()->CheckDeleteFPModel() == false )
    {
      return false;
    }
    //ここにあった処理はFinalizeに移動しました。NMCat[3964]対応
    mCleanupSeq++;
    //break; through
  case SEQ_WAIT_DELETE_POKE_SYS:
    //ここにあった処理はFinalizeに移動しました。NMCat[3964]対応
    mCleanupSeq++;
    //break; through

  case SEQ_CLEAN_UI:
    if( mUiSys->CleanUp() == false )
    {
      return false;
    }
    mCleanupSeq++;
    break; //1フレ待つからスルーしない

  case SEQ_CLEAN_RENDER:
    if( mRenderSys->Cleanup() == false )
    {
      return false;
    }
    mCleanupSeq++;
    //break; through

  case SEQ_CLOSE_ARC:
    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
      req.arcId = ARCID_WAZAEFF_RESOURCE;
      req.heapForReq = GetTempHeap()->GetLowerHandle();
      GetAsyncFileManager()->AddArcFileCloseReq(req);

      req.arcId = ARCID_WAZAEFF_SEQUENCE;
      GetAsyncFileManager()->AddArcFileCloseReq(req);

      req.arcId = ARCID_RENDERPATH;
      GetAsyncFileManager()->AddArcFileCloseReq(req);

      req.arcId = ARCID_BTLV_RESOURCE;
      GetAsyncFileManager()->AddArcFileCloseReq(req);

      req.arcId = print::GetMessageArcId();
      GetAsyncFileManager()->AddArcFileCloseReq(req);

    }
    mCleanupSeq++;
    //break; through

  case SEQ_WAIT_CLOSE_ARC:
    if( GetAsyncFileManager()->IsArcFileCloseFinished(ARCID_WAZAEFF_RESOURCE) == false ||
        GetAsyncFileManager()->IsArcFileCloseFinished(ARCID_WAZAEFF_SEQUENCE) == false ||
        GetAsyncFileManager()->IsArcFileCloseFinished(ARCID_RENDERPATH) == false ||
        GetAsyncFileManager()->IsArcFileCloseFinished(ARCID_BTLV_RESOURCE) == false ||
        GetAsyncFileManager()->IsArcFileCloseFinished(print::GetMessageArcId()) == false )
    {
      return false;
    }
    mCleanupSeq++;
    //break; through

  case SEQ_FINISH:
    return true;
    //break;
  }
  return false;
}


bool BattleViewSystem::CMD_InitStartWaitCameraEffect(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);
  if( mIsStartWaitCameraEffect == true )
  {
    return true;
  }

  mComSeq = 0;
  return true;
}
bool BattleViewSystem::CMD_WaitStartWaitCameraEffect(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);

  if( mIsStartWaitCameraEffect == true )
  {
    return true;
  }

  enum
  {
    SEQ_WAIT_MOTION,
    SEQ_SETUP,
    SEQ_WAIT_UI,
    SEQ_DISP_UI,
  };

  switch( mComSeq )
  {
  case SEQ_WAIT_MOTION:
    //モーション終了待ち
    for( int i=0;i<POKE_MODEL_NUM;i++ )
    {
      BtlvObject_PokeModel *poke = GetPokeModel(static_cast<BtlvPos>(i));
      if( poke->IsEnable() && 
          poke->GetVisible() &&
          poke->IsVisibleMigawari() == false &&
          poke->IsVisibleTame() == true )    
      {
        if( mPokeModel[i]->GetAnimationType() != PokeTool::MODEL_ANIME_BT_WAIT_A &&
            mPokeModel[i]->GetAnimationType() != -1 ) //モーションないやつ対応
        {
          if( mPokeModel[i]->GetPokeModel()->GetAnimationIsLoop() )
          {
            //待機以外がループになっている！
            GFL_ASSERT_MSG(0,"Motion is Loop!![%d]\n",mPokeModel[i]->GetAnimationType()); //@cehck 開発中に気づく。フォローあり
            mPokeModel[i]->SetAnimationIsLoop(false);
          }
          return false;
        }
      }
    }
    mComSeq++;
    //break; through
  case SEQ_SETUP:
    DumpBattleHeap();
    //ポケモンのWaitBは待機カメラの方で動かす
    //WaitBを中断させるにはカメラアニメが始まっている必要があるので。

    //Waitカメラ専用ヒープ。リソースサイズ(gfmot+env)の最大は合計3kb以内(0xC00)
    mWaitCameraHeap = GFL_CREATE_LOCAL_HEAP_NAME( GetTempHeap() , 0x5000   , gfl2::heap::HEAP_TYPE_EXP , false , "WaitCamera" );
    mWaitCameraGLAllocator = GFL_NEW( mWaitCameraHeap ) System::nijiAllocator(mWaitCameraHeap);
    //待機中状態異常エフェクト用ヒープ。リソースサイズの最大は166kb(0x29800)
    mWaitSickHeap = GFL_CREATE_LOCAL_HEAP_NAME( GetTempHeap() , 0x60000   , gfl2::heap::HEAP_TYPE_EXP , false , "SickEffect" );

    GetCameraSys()->StartWaitCamera();
    GetUiSys()->StartDispRoyalRanking();

    //ここで初めてピンチ音の許可
    mCanChangePinchBgm = true;
    CheckPinchBgmReq();

    // 大会演出開始
    if( IsConventionDirection() )
    {
      GetUiSys()->StartConventionDirection();
    }

    //状態異常
    Start_WaitCamSickEffect();

    //フェスの控えトレーナー表示
    DispFesTrainer(true);

    mComSeq++;
    //break; through
  case SEQ_WAIT_UI:
    if( GetUiSys()->InitializeActSelect() == false ||
        GetUiSys()->IsFinishDispRoyalRanking() == false )
    {
      return false;
    }
    mComSeq++;
    //break; through
  case SEQ_DISP_UI:
    GetUiSys()->DispGaugeAll();
    GetUiSys()->GaugeUpdateReq();
    GetUiSys()->DispBallBarAll();
    GetUiSys()->UpdateBallBarAll();
    mIsStartWaitCameraEffect = true;
    return true;
  }
  return false;
}

void BattleViewSystem::CMD_InitFinishWaitCameraEffect(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;
  if( mIsStartWaitCameraEffect == false )
  {
    return;
  }
  mComSeq = 0;

  //@caution 通信エラー時はここを通らない場合がある。Finalizeにも開放の処理をしておく事。

  GetCameraSys()->EndWaitCamera();
  GetUiSys()->StartHideRoyalRanking();
  Finish_WaitCamSickEffect();
  //上昇下降エフェクトの連続再生を抑制のリセット
  ResetCheckCanPlayEffect();


  // 大会演出中断
  if( IsConventionDirection() )
    {
      GetUiSys()->EndConventionDirection();
    }

}
bool BattleViewSystem::CMD_WaitFinishWaitCameraEffect(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);

  //@caution 通信エラー時はここを通らない場合がある。Finalizeにも開放の処理をしておく事。

  if( mIsStartWaitCameraEffect == false )
  {
    return true;
  }
  switch( mComSeq )
  {
  case 0:
    if( GetUiSys()->FinalizeActSelect() == false ||
        GetUiSys()->IsFinishHideRoyalRanking() == false )
    {
      return false;
    }
    if( GetCameraSys()->IsFinishWaitCamera() == false )
    {
      return false;
    }

    //状態異常
    if( IsFinish_WaitCamSickEffect() == false )
    {
      return false;
    }

    mComSeq++;
    //break; through
  case 1:
    //待機カメラと状態異常の専用ヒープ削除
    if( mWaitCameraHeap )
    {
      GFL_SAFE_DELETE(mWaitCameraGLAllocator);
      GFL_DELETE_HEAP(mWaitCameraHeap);
      mWaitCameraHeap = NULL;
    }
    if( mWaitSickHeap )
    {
      GFL_DELETE_HEAP(mWaitSickHeap);
      mWaitSickHeap = NULL;
    }
    DumpBattleHeap();
    GetUiSys()->HideBallBarAll();
    //フェスの控えトレーナー非表示
    DispFesTrainer(true);
    mIsStartWaitCameraEffect = false;
    return true;
  }
  return false;

}

//コマンド系
void       BattleViewSystem::CMD_UI_SelectAction_Start( const BTL_POKEPARAM* bpp, u8 pokeIndex, bool fReturnable, BTL_ACTION_PARAM* dest )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;
  mComPokeParam = bpp;
  mComActParam = dest;
  mUiSys->Start_SelectCommand( pokeIndex, fReturnable );

}
BtlAction  BattleViewSystem::CMD_UI_SelectAction_Wait( void )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(BTL_ACTION_FIGHT);

  const BTLV_COMMAND_RESULT ret = mUiSys->WaitAnime_SelectCommand();
  if( ret != BCR_SEL_NONE )
  {
    mComSelResult = ret;
    mComPokeParam = NULL;
    mComActParam = NULL;
    switch( mComSelResult )
    {
    case BCR_SEL_SKILL:
      return BTL_ACTION_FIGHT;
    case BCR_SEL_ITEM:
      return BTL_ACTION_ITEM;
    case BCR_SEL_RUN:
      return BTL_ACTION_ESCAPE;
    case BCR_SEL_POKE:
      return BTL_ACTION_CHANGE;
    }
  }

  return BTL_ACTION_NULL;
}
void BattleViewSystem::CMD_UI_SelectAction_ForceQuit( void )
{
  DEBUG_NODRAW_CHECK;
  mUiSys->ForceQuit_SelectCommand();
}

void BattleViewSystem::CMD_UI_SelectWaza_Start( const BTL_POKEPARAM* bpp, u8 pokeIndex, BTL_ACTION_PARAM* dest )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;

  mComPokeParam = bpp;
  mComActParam = dest;
  mMegaSelected = false;

  mUiSys->Start_SelectSkill( pokeIndex );
}
void BattleViewSystem::CMD_UI_SelectRotationWaza_Start( u8 pokeIndex, BTLV_ROTATION_WAZASEL_PARAM* selParam, BTL_ACTION_PARAM* dest )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;
  
  
}
void BattleViewSystem::CMD_UI_SelectWaza_Restart( u8 pokeIndex )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;
  mUiSys->Start_SelectSkill( pokeIndex );
}

/**
 * @brief 技選択の更新処理
 * @retval true   技選択が完了した
 * @retval false  技選択中
 */
bool BattleViewSystem::CMD_UI_SelectWaza_Wait( void )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);

  bool bUseAuraWaza = false;
  bool bUseUltraBurst = false;
  mUiSys->GetResult_SelectSkill(&mComSelResult, &mMegaSelected, &bUseAuraWaza, &bUseUltraBurst);

  if( mComSelResult == BCR_SEL_CANCEL )
  {
    mComActParam->fight.cmd = BTL_ACTION_NULL;
    return true;
  }

  if( mComSelResult >= BCR_SEL_IDX1 &&
      mComSelResult <= BCR_SEL_IDX4)
  {
    mComActParam->fight.targetPos         = BTL_POS_NULL;
    mComActParam->fight.cmd               = BTL_ACTION_FIGHT;
    mComActParam->fight.waza              = mComPokeParam->WAZA_GetID(mComSelResult-BCR_SEL_IDX1);
    mComActParam->fight.wazaInfoFlag      = false;
    mComActParam->fight.megaEvoFlag       = mMegaSelected;
    mComActParam->fight.zenryokuWazaFlag  = bUseAuraWaza;
    mComActParam->fight.urtlaBurstFlag    = bUseUltraBurst;
    return true;
  }

  return false;
}

/**
 * @brief 技選択の終了処理
 * @retval true   終了処理が完了した
 * @retval false  終了処理が継続中
 */
bool BattleViewSystem::CMD_UI_SelectWaza_End( void )
{
  DEBUG_NODRAW_CHECK_PARAM(true);
  return  mUiSys->EndAnime_SelectSkill();
}

void BattleViewSystem::CMD_UI_SelectWaza_ForceQuit( void )
{
  DEBUG_NODRAW_CHECK;
  mUiSys->ForceQuit_SelectSkill();
}

void       BattleViewSystem::CMD_UI_SelectTarget_Start( u8 poke_index, const BTL_POKEPARAM* bpp, BTL_ACTION_PARAM* dest )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;

  mComActParam = dest;

  WazaNo wazano = BTL_ACTION_GetWazaID( mComActParam, bpp );
  mUiSys->StartAnime_Target( poke_index, bpp, wazano );
}
BtlvResult BattleViewSystem::CMD_UI_SelectTarget_Wait(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(BTLV_RESULT_DONE);

  const BTLV_COMMAND_RESULT ret = mUiSys->WaitAnime_Target();

  if( ret == BCR_SEL_CANCEL )
  {
    return BTLV_RESULT_CANCEL;
  }
  else if( ( BCR_SEL_IDX1 <= ret ) && ( ret <= BCR_SEL_IDX6 ) )
  {
    BtlvPos vpos = static_cast<BtlvPos>( ret - BCR_SEL_IDX1 );
    mComActParam->fight.targetPos = ViewPosToBtlPos( vpos );

    return BTLV_RESULT_DONE;
  }

  return BTLV_RESULT_NONE;
}
void       BattleViewSystem::CMD_UI_SelectTarget_ForceQuit(void)
{
  DEBUG_NODRAW_CHECK;
  mUiSys->ForceQuit_Target();
}

void BattleViewSystem::CMD_UI_RestartIfNotStandBy( void )
{
  DEBUG_FUNC_CALL_TEMP;
  
}
void BattleViewSystem::CMD_UI_Restart( void )
{
  DEBUG_FUNC_CALL_TEMP;
  
}
bool BattleViewSystem::CMD_UI_WaitRestart( void )
{
  DEBUG_FUNC_CALL_TEMP;
  return true;
}

void BattleViewSystem::CMD_StartMemberChangeAct( BtlPokePos pos, u8 clientID, u8 memberIdx )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;

  BtlvPos vPos = BtlPosToViewPos(pos);

  SetComTargetReset();
  mComTargetPoke[0] = vPos;
  mTargetTrainer[0] = GetBtlvPosToTrainerIdx(vPos);
  mEffectBallId[0] = GetBattlePokeParam(vPos)->GetViewSrcData()->GetGetBall();

  mComScene = 0;

  GetUiSys()->ResetGaugeParam( vPos );  //ここでゲージが表示される
  GetUiSys()->HideBallBarAll();
  GetUiSys()->HideGaugeAll();           //ここでゲージを非表示

  // BTS[4374]: ポケモンが場に出た時点で、カーソル位置をリセットする
  if( this->GetClientID() == clientID ) 
  {
    GetUiSys()->ResetCursorPos( memberIdx );
  }
}
bool BattleViewSystem::CMD_WaitMemberChangeAct(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);

  enum
  {
    CREATE_START = 0,
    CREATE_WAIT,
    SEQ_WAIT,
    SEQ_START2,
    SEQ_WAIT2,
    FINISH,

    SKIP_MODE = 20,
  };

  BtlvObject_PokeModel *mdl = GetPokeModel(mComTargetPoke[0]);
  switch( mComScene )
  {
  case CREATE_START:
    mdl->StartLoad_Battle();
    mComScene++;
    //break; //through
  case CREATE_WAIT:
    if( mdl->IsFinishLoad() )
    {
      SuspendFrameSkip();
      mdl->Create(GetPokeHeap(mComTargetPoke[0]),GetPokeGLAllocator(mComTargetPoke[0]),true);
      mdl->AddRenderPath(0);
      mdl->SetVisible(false);
      mdl->ChangeAnimation(PokeTool::MODEL_ANIME_BT_WAIT_A);
      mdl->SetAnimationIsLoop(true);

      ResetPokemon(mComTargetPoke[0],0);

      if (IsBattleFesMode() && mComTargetPoke[0] == BTL_VPOS_NEAR_1 )
      {
        //トレーナーの入れ替え
        //pokeIDが開始時のIdxになっているのでこれを元にトレーナーを探す
        u8 pokeId = GetBattlePokeParam(mComTargetPoke[0])->GetID();

        u8 swapTarget = 0;
        if (GetTrainerModelObj(2)->GetBattleFesIndex() == pokeId)
        {
          swapTarget = 2;
        }
        if (GetTrainerModelObj(3)->GetBattleFesIndex() == pokeId)
        {
          swapTarget = 3;
        }
        if (swapTarget != 0)
        {
          BtlvObject_Chara *tempObj = mTrainerModel[0];
          mTrainerModel[0] = mTrainerModel[swapTarget];
          mTrainerModel[swapTarget] = tempObj;
          mTrainerModel[0]->SetIndex(0);
          mTrainerModel[swapTarget]->SetIndex(swapTarget);

          mTrainerModel[0]->AddRenderPath();
          mTrainerModel[0]->ChangeAnimation(TRAINER_ANIME_WAIT1);
          mTrainerModel[0]->SetAnimationIsLoop(true);
          ResetTrainer(mComTargetPoke[0]);

          mTrainerModel[swapTarget]->SetVisible(false);
          mTrainerModel[swapTarget]->RemoveRenderPath();
          ResetTrainer(swapTarget);
        }

        //ここで改めて表示
        DispFesTrainer(true);
        mTrainerModel[swapTarget]->ChangeAnimation(TRAINER_ANIME_WAIT2);
        mTrainerModel[swapTarget]->SetAnimationIsLoop(false);
        mTrainerModel[swapTarget]->SetAutoResetAniation(true);
      }

      if( IsRecordSkipMode() )
      {
        mdl->SetVisible(true);
        mComScene = FINISH;
      }
      else
      {
        if (IsBattleFesMode() && mComTargetPoke[0] == BTL_VPOS_NEAR_1)
        {
          SeqComFunc_PlaySequenceFile(GARC_wazaeff_seqfile_ee397_BSEQ);
          mComScene++;
        }
        else
        {
          mComScene = SEQ_START2;
        }
      }
    }
    break;
  case SEQ_WAIT:
    if( SeqComFunc_IsFinishSequenceFile(true) )
    {
      //ここで消すと読み込み待ちで消えているところが見えてしまうので、シーケンスで消す
      //DispFesTrainer(false);
      mComScene++;
    }
    break;
  case SEQ_START2:
    {
      //LAND_A,B,CかLANDかでモーションを分岐する
      if (mdl->IsAvailableAnimation(PokeTool::MODEL_ANIME_LAND))
      {
        SeqComFunc_PlaySequenceFile(GARC_wazaeff_seqfile_ee396_BSEQ);
      }
      else
      {
        SeqComFunc_PlaySequenceFile(GARC_wazaeff_seqfile_ee393_BSEQ);
      }
      mComScene++;
    }
    //break;  //through
  case SEQ_WAIT2:
    if (SeqComFunc_IsFinishSequenceFile())
    {
      GetUiSys()->GaugeUpdateReq();
      mComScene++;
    }
    break;

  case FINISH:
    return true;
  }

  return false;
}

void BattleViewSystem::CMD_CopyMsgToBufferExpanded( const STRBUF* src )
{
  DEBUG_FUNC_CALL;
  DEBUG_NODRAW_CHECK;

  str::SetStringExpanded( GetWorkStr(0), src );

}
void BattleViewSystem::CMD_SetMsgToBuffer( const BTLV_STRPARAM* param )
{
  DEBUG_FUNC_CALL_TEMP;
  
  //sangoで未使用
  GFL_ASSERT_MSG(0,"未実装\n");
}
void BattleViewSystem::CMD_StartMsgInBuffer( bool isKeyWait )
{
  DEBUG_FUNC_CALL;
  DEBUG_NODRAW_CHECK;
  
  GetUiSys()->StartDispMessage( GetWorkStr(0), isKeyWait );
}

//--------------------------------------------------------------------------
//
// このコマンドでメッセージ表示を開始した場合、開始直後から、
//
// callbackTarget->PrintCallback( arg ) を、毎フレーム呼び出し続けてください。
// ※ arg については、btl_client.h の PrintCallback() メソッド近辺にコメントがありますので参照してください。
//
// PrintCallback が true を返してきたら、このコマンドを終了してください。
//
//--------------------------------------------------------------------------
void BattleViewSystem::CMD_StartMsgCallback( const BTLV_STRPARAM* param, BTL_CLIENT* callbackTarget )
{
  DEBUG_FUNC_CALL;
  DEBUG_NODRAW_CHECK;

  GetUiSys()->StartDispMessage( param );
  GetUiSys()->SetMessageCallBack( callbackTarget );
}

//=============================================================================================
/**
  * ポケモンリスト処理を開始
  *
  * @param   core            [in]  描画メインモジュールハンドラ
  * @param   param           [in]  リスト処理パラメータポインタ
  * @param   outMemberParam  [in]  退出するメンバーのパラメータ
  * @param   outMemberIndex  [in]  「ポケモン」メニューからのメンバー交替時、退出するメンバーのIndex / それ以外-1
  * @param   fCantEsc        [in]
  * @param   result          [out] 選択結果格納構造体ポインタ
  */
//=============================================================================================
void BattleViewSystem::CMD_StartPokeList( const PokeSelParam* param, const BTL_POKEPARAM* outMemberParam, int outMemberIndex, bool fCantEsc, PokeSelResult* result )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;

  mPokeSelParam  = param;
  mPokeSelResult = result;

  //「さしおさえ」で道具を使用できないポケモンを指定(ここで判定しないで、アイテム使用時の判定は使用処理内で行う)
  /*
  //「さしおさえ」で道具を使用できないポケモンを指定
  bool dummyArr[6];
  {
    const BTL_PARTY* party = GetBattleParty_Self();
    const u32 members = party->GetMemberCount();
    int i;

    ::std::memset( dummyArr, 0, sizeof( dummyArr ) );

    for(i=0; i<members; ++i)
    {
      // 0=使用可 / 1=使用不可
      const BTL_POKEPARAM *bpp = party->GetMemberDataConst(i);
      dummyArr[ i ] = bpp->CheckSick( pml::wazadata::WAZASICK_SASIOSAE );
    }
  }
  */

  mUiSys->StartAnime_PokeList( param, !fCantEsc, outMemberParam, outMemberIndex, NULL /* dummyArr */ );
}

//=============================================================================================
/**
 * ポケモンリスト処理の終了待ち
 *
 * @retval  bool      終了したらtrue
 */
//=============================================================================================
bool BattleViewSystem::CMD_WaitPokeList(void)
{
  DEBUG_NODRAW_CHECK_PARAM(true);
  //DEBUG_FUNC_CALL_TEMP;
  const BTLV_COMMAND_RESULT ret = mUiSys->WaitAnime_PokeList( mPokeSelResult );

  if( ret != BCR_SEL_NONE )
  {
    return true;
  }

  return false;
}

//=============================================================================================
/**
  *  ポケモンリスト処理の強制終了（コマンド制限時間による）
  */
//=============================================================================================
void BattleViewSystem::CMD_ForceQuitPokeList(void)
{
  DEBUG_NODRAW_CHECK;
  mUiSys->ForceQuit_PokeList();
}
  
//=============================================================================================
/**
  *  ポケモンリスト処理の強制終了待ち
  * 
  * @retval true   強制終了完了
  * @retval false  強制終了処理中
  */
//=============================================================================================
bool BattleViewSystem::CMD_WaitForceQuitPokeList(void)
{
  DEBUG_FUNC_CALL_TEMP;
  return true;
}

//=============================================================================================
/**
  * ポケモン選択処理を開始
  *
  * @param   core            [in]  描画メインモジュールハンドラ
  * @param   param           [in]  選択処理パラメータポインタ
  * @param   outMemberIndex  [in]  「ポケモン」メニューからのメンバー交替時、退出するメンバーのIndex / それ以外-1
  * @param   fCantEsc        [in]
  * @param   result          [out] 選択結果格納構造体ポインタ
  */
//=============================================================================================
void BattleViewSystem::CMD_StartPokeSelect( const PokeSelParam* param, int outMemberIndex, bool fCantEsc, PokeSelResult* result )
{
  DEBUG_NODRAW_CHECK;

  //GFMCat[229] 入れ替え字にゲージ類表示
  GetUiSys()->DispBallBarAll();
  GetUiSys()->UpdateBallBarAll();
  GetUiSys()->GaugeUpdateForce();
  GetUiSys()->DispGaugeAll();

  mPokeSelResult = result;
  mUiSys->StartAnime_SelectPoke( param, fCantEsc, outMemberIndex, mPokeSelResult );
}

//=============================================================================================
/**
  * ポケモン選択処理の終了待ち
  *
  * @param   core      [in]  描画メインモジュールハンドラ
  *
  * @retval  bool      終了したらTRUE
  */
//=============================================================================================
bool BattleViewSystem::CMD_WaitPokeSelect(void)
{
  DEBUG_NODRAW_CHECK_PARAM(true);
  const BTLV_COMMAND_RESULT ret = mUiSys->WaitAnime_SelectPoke( mPokeSelResult );
  if( ret == BCR_SEL_NONE )
  {
    return false;
  }
  else 
  {
    return true;
  }
}

//=============================================================================================
/**
  *  ポケモン選択処理の強制終了（コマンド制限時間による）
  */
//=============================================================================================
void BattleViewSystem::CMD_ForceQuitPokeSelect(void)
{
  DEBUG_NODRAW_CHECK;
  mUiSys->ForceQuit_SelectPoke();
}
  
//=============================================================================================
/**
  *  ポケモン選択処理の強制終了（コマンド制限時間による）待ち
  */
//=============================================================================================
bool BattleViewSystem::CMD_WaitForceQuitPokeSelect(void)
{
  DEBUG_FUNC_CALL_TEMP;
  return true;
}



/**
  *  ワザ説明（ショートカット）画面の呼び出し
  */
void BattleViewSystem::CMD_StartWazaInfoView( u8 pokeIdx, u8 wazaIdx )
{
  DEBUG_FUNC_CALL_TEMP;
  
}



//=============================================================================================
/**
  *
  *
  * @param   wk
  * @param   strID
  * @param   clientID
  *
  * @retval  void
  */
//=============================================================================================
void BattleViewSystem::CMD_StartMsg( const BTLV_STRPARAM* param )
{
  DEBUG_NODRAW_CHECK;
  if( IsRecordSkipMode() )
  {
    return;
  }
  mUiSys->StartDispMessage( param );
}
void BattleViewSystem::CMD_StartMsgWaza( u8 pokeID, u16 waza, bool isZenryokuWaza )
{
  DEBUG_NODRAW_CHECK;
  BtlvPos vpos = PokeIDToViewPos(pokeID);
  if( IsRecordSkipMode() )
  {
    return;
  }
  //指示だしモーション
  if( isZenryokuWaza == false )
  {
    BtlvObject_Chara *chara = GetTrainerModelObj(GetBtlvPosToTrainerIdx(vpos));
    //全力技はポーズを取っているので指示しない
    if( chara->IsEnable() )
    {
      chara->ChangeAnimation(2);
      chara->SetAnimationIsLoop(false);
      chara->SetAutoResetAniation(true);
    }
  }

  if( isZenryokuWaza )
  {
    str::MakeStringZenryokuWaza( mWorkStr[0] , pokeID , waza );

    //このタイミングでデフォルトカメラに戻して、表示を戻して、フェイシャルを戻す 
    if( mZenryokuParam.isZenryoku == true && mZenryokuParam.isAttack == false && mZenryokuParam.isUnique == false )
    {
      ResetDefaultCamera(0);
      SeqComFunc_CleanAllWazaResource();
      GetBgSystem()->SetVisible(true);
      SetVisibleWeather(true);
      GetEdgeMaskModelObj()->SetVisible(true);
      GetFloorMaskModelObj()->SetVisible(true);
      GetBgFadeModelObj()->SetVisible(false);

      for( int i=0;i<POKEMODEL_NUM;i++ )
      {
        mPokeModel[i]->SetVisible(true);
        mPokeModel[i]->ResetOverrideEyeType();
        mPokeModel[i]->SetUseEffectLight(false);
        //演出スキップ時はここで身代わりを戻す
        if( IsWazaEffectEnable() == false  )
        {
          if( mPokeModel[i]->IsHideMigawariZenryoku() )
          {
            mPokeModel[i]->SetVisibleMigawari(true);
            mPokeModel[i]->SetHideMigawariZenryoku(false);
          }
        }
      }
      for( int i=0;i<TRAINER_MODEL_NUM;i++ )
      {
        int degY;
        gfl2::math::Vector3 pos(0,0,0);
        GetDefaultTrainerPos(i,&pos,&degY);
        gfl2::math::Vector3 rot(0,degY,0);

        GetTrainerModelObj(i)->SetPosition(pos);
        GetTrainerModelObj(i)->SetRotate(rot);

        GetTrainerModelObj(i)->SetVisible(true);
        GetTrainerModelObj(i)->SetUseEffectLight(false);
      }
    }
  }
  else
  {
    str::MakeStringWaza( mWorkStr[0] , pokeID , waza );
  }
  mUiSys->StartDispMessage( mWorkStr[0] );
}
void BattleViewSystem::CMD_PrintMsgAtOnce( const BTLV_STRPARAM* param )
{
  DEBUG_NODRAW_CHECK;
  if( IsRecordSkipMode() )
  {
    return;
  }
  mUiSys->StartDispMessage( param );
  
}


void BattleViewSystem::CMD_StartMsgStd( u16 strID, const int* args )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;
  if( IsRecordSkipMode() )
  {
    return;
  }
  str::MakeStringStdWithArgArray( mWorkStr[0] , strID , args );

 mUiSys->StartDispMessage( mWorkStr[0] );
}
void BattleViewSystem::CMD_StartMsgSet( u16 strID, const int* args )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;
  if( IsRecordSkipMode() )
  {
    return;
  }
  str::MakeStringSet( mWorkStr[0] , strID , args );

 mUiSys->StartDispMessage( mWorkStr[0] );
}
bool BattleViewSystem::CMD_StartMsgTrainer( u32 trainerID, int param, bool isKeyWait )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);
  if( IsRecordSkipMode() )
  {
    return true;
  }
  str::MakeStringTrainer( mWorkStr[0] , static_cast<trainer::TrainerID>(trainerID) , static_cast<trainer::MessageKind>(param) );
  mUiSys->StartDispMessage( mWorkStr[0] , isKeyWait );
  return true;
}

bool BattleViewSystem::CMD_StartMsgFesTrainer(u16 strId)
{
  gfl2::str::MsgData *msgData;
  gfl2::heap::HeapBase* heap = GetTempHeap();

  if (IsRecordSkipMode())
  {
    return true;
  }

  void *buf = GetResourceManager()->GetDataBuf(print::GetMessageArcId(), GARC_message_jf_phrase_DAT);
  if( buf )
  {
    msgData = GFL_NEW(heap)gfl2::str::MsgData(GetResourceManager()->GetDataBuf(print::GetMessageArcId(), GARC_message_jf_phrase_DAT), heap);
    msgData->GetString(strId, *GetWorkStr(0));
    GFL_DELETE(msgData);
    mUiSys->StartDispMessage(GetWorkStr(0),true);
  }
  return true;
}

bool BattleViewSystem::CMD_WaitMsg(void)
{
  DEBUG_NODRAW_CHECK_PARAM(true);
  if( mUiSys && !mUiSys->IsUpdateMessage() )
  {
    mUiSys->HideMessage();
    return true;
  }
  return false;
}     // Update & Window消去まで

bool BattleViewSystem::CMD_WaitMsg_WithoutHide(void)
{
  DEBUG_NODRAW_CHECK_PARAM(true);
  if( mUiSys && !mUiSys->IsUpdateMessage() )
  {
    return true;
  }
  return false;
}   // Updateのみ

void BattleViewSystem::CMD_HideMsg(void)
{
  DEBUG_NODRAW_CHECK;
  mUiSys->HideMessage();
}               // Window消去のみ

typedef struct
{
  u32 wazaNo;
  int form; //-1で何でもOK
  u32 startSeq;
  u32 wazaSeq;
}ZENRYOKU_DATA;

ZENRYOKU_DATA ZenryokuDataTable[] =
{
  {WAZANO_HISSATUNOPIKATYUUTO   ,-1,GARC_wazaeff_seqfile_ez_2_pika01_BSEQ,GARC_wazaeff_seqfile_ez_2_pika02_BSEQ}, //ピカチュウ
  {WAZANO_ORIZINZUSUUPAANOVHA   ,-1,GARC_wazaeff_seqfile_ez_2_mew01_BSEQ,GARC_wazaeff_seqfile_ez_2_mew02_BSEQ}, //ミュウ
  {WAZANO_HONKIWODASUKOUGEKI    ,-1,GARC_wazaeff_seqfile_ez_2_kabi01_BSEQ,GARC_wazaeff_seqfile_ez_2_kabi02_BSEQ}, //カビゴン

  {WAZANO_SYADOUAROOZUSUTORAIKU ,-1,GARC_wazaeff_seqfile_ez_2_fuku01_BSEQ,GARC_wazaeff_seqfile_ez_2_fuku02_BSEQ}, //ふくろう３
  {WAZANO_HAIPAADAAKUKURASSYAA  ,-1,GARC_wazaeff_seqfile_ez_2_tora01_BSEQ,GARC_wazaeff_seqfile_ez_2_tora02_BSEQ}, //トラ３
  {WAZANO_WADATUMINOSINFONIA    ,-1,GARC_wazaeff_seqfile_ez_2_ashi01_BSEQ,GARC_wazaeff_seqfile_ez_2_ashi02_BSEQ}, //アシカ３
  {WAZANO_GAADHIANDEAROORA      ,-1,GARC_wazaeff_seqfile_ez_2_tiki01_BSEQ,GARC_wazaeff_seqfile_ez_2_tiki02_BSEQ}, //ティキ
  {WAZANO_RAITONINGUSAAHURAIDO  ,-1,GARC_wazaeff_seqfile_ez_2_rai01_BSEQ,GARC_wazaeff_seqfile_ez_2_rai02_BSEQ}, //アローらライチュウ
  {WAZANO_SEBUNEBORUBUUSUTO     ,-1,GARC_wazaeff_seqfile_ez_2_eie01_BSEQ,GARC_wazaeff_seqfile_ez_2_eie02_BSEQ}, //イーブイ
  {WAZANO_HAISINSENYOUZENRYOKU  ,-1,GARC_wazaeff_seqfile_ez_2_mv1701_BSEQ,GARC_wazaeff_seqfile_ez_2_mv1702_BSEQ}, //配信
  {WAZANO_SATOPIKASENYOUZENRYOKU,-1,GARC_wazaeff_seqfile_ez_2_spika01_BSEQ,GARC_wazaeff_seqfile_ez_2_spika02_BSEQ}, //サトピカ

  //momiji追加分
  { WAZANO_SINNEKUROZUMAZETTO   ,-1,GARC_wazaeff_seqfile_ez_2_hikari01_BSEQ, GARC_wazaeff_seqfile_ez_2_hikari02_BSEQ }, // シンネクロズマゼット
  { WAZANO_ZENRYOKUMETEODORAIBU , 0,GARC_wazaeff_seqfile_ez_2_sol01_BSEQ, GARC_wazaeff_seqfile_ez_2_sol02_BSEQ }, // ゼンリョクメテオドライブ
  { WAZANO_ZENRYOKUMETEODORAIBU , 1,GARC_wazaeff_seqfile_ez_2_sol01_1_BSEQ, GARC_wazaeff_seqfile_ez_2_sol02_1_BSEQ }, // ゼンリョクメテオドライブ
  { WAZANO_ZENRYOKUSYADOOREI    , 0,GARC_wazaeff_seqfile_ez_2_runa01_BSEQ, GARC_wazaeff_seqfile_ez_2_runa02_BSEQ }, // ゼンリョクシャドーレイ
  { WAZANO_ZENRYOKUSYADOOREI    , 2,GARC_wazaeff_seqfile_ez_2_runa01_1_BSEQ, GARC_wazaeff_seqfile_ez_2_runa02_1_BSEQ }, // ゼンリョクシャドーレイ
  //ミミッキュの分岐は通常、首折れ、ヌシ、ヌシ首折れとなっているのでこうなる。
  { WAZANO_ZENRYOKUZYARETUKU    , 0,GARC_wazaeff_seqfile_ez_2_mmq01_BSEQ, GARC_wazaeff_seqfile_ez_2_mmq02_BSEQ }, // ぜんりょくじゃれつく
  { WAZANO_ZENRYOKUZYARETUKU    , 1,GARC_wazaeff_seqfile_ez_2_mmq01_BSEQ, GARC_wazaeff_seqfile_ez_2_mmq02_BSEQ }, // ぜんりょくじゃれつく
  { WAZANO_ZENRYOKUZYARETUKU    , 2,GARC_wazaeff_seqfile_ez_2_mmq01_1_BSEQ, GARC_wazaeff_seqfile_ez_2_mmq02_1_BSEQ }, // ぜんりょくじゃれつく
  { WAZANO_ZENRYOKUZYARETUKU    , 3,GARC_wazaeff_seqfile_ez_2_mmq01_1_BSEQ, GARC_wazaeff_seqfile_ez_2_mmq02_1_BSEQ }, // ぜんりょくじゃれつく
  { WAZANO_ZENRYOKUSUTOONEZZI   , 0,GARC_wazaeff_seqfile_ez_2_rgrg01_BSEQ, GARC_wazaeff_seqfile_ez_2_rgrg02_BSEQ }, // ぜんりょくストーンエッジ
  { WAZANO_ZENRYOKUSUTOONEZZI   , 1,GARC_wazaeff_seqfile_ez_2_rgrg01_1_BSEQ, GARC_wazaeff_seqfile_ez_2_rgrg02_1_BSEQ }, // ぜんりょくストーンエッジ
  { WAZANO_ZENRYOKUSUTOONEZZI   , 2,GARC_wazaeff_seqfile_ez_2_rgrg01_2_BSEQ, GARC_wazaeff_seqfile_ez_2_rgrg02_2_BSEQ }, // ぜんりょくストーンエッジ
  { WAZANO_ZENRYOKUSUKEIRUNOIZU , 0,GARC_wazaeff_seqfile_ez_2_jrrg01_BSEQ, GARC_wazaeff_seqfile_ez_2_jrrg02_BSEQ }, // ぜんりょくスケイルノイズ
  { WAZANO_ZENRYOKUSUKEIRUNOIZU , 1,GARC_wazaeff_seqfile_ez_2_jrrg01_1_BSEQ, GARC_wazaeff_seqfile_ez_2_jrrg02_1_BSEQ }, // ぜんりょくスケイルノイズ
};

void BattleViewSystem::CMD_ACT_WazaEffect_Start( BtlPokePos atPokePos, BtlPokePos defPokePos, WazaID waza,
  WazaTarget wazaRange, BtlvWazaEffect_TurnType turnType, u8 continueCount , bool syncDamageEffect/*, BtlObjBreak* objBreakManager*/ )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;
  //GFL_PRINT("技エフェクト 技[%d:%s] [%d]→[%d]\n",waza,dmyWazaNameArr[waza],atPokePos,defPokePos);
  ARIIZUMI_PRINT("WazaEffectStart atkPos[%d] defPos[%d] waza[%d] range[%d] turnType[%d] contCnt[%d] Sync[%d]\n",atPokePos, defPokePos, waza, wazaRange, turnType, continueCount,syncDamageEffect);

  mWazaParam.mAtkPos = atPokePos;
  mWazaParam.mDefPos = defPokePos;
  mWazaParam.mId     = waza;
  mWazaParam.mRange  = wazaRange;
  mWazaParam.mTurnType = turnType;
  mWazaParam.mContinueCount = continueCount;  //今は必ず0
  mWazaParam.mSyncDamageEffect = syncDamageEffect;
  mWazaParam.mDmageAbout = pml::battle::TypeAffinity::TYPEAFF_ABOUT_NONE;

  for( int i=0;i<BTL_EXIST_POS_MAX;i++ )
  {
    mComTargetDamage[i] = BTL_VPOS_ERROR;
  }

  mZenryokuParam.isAttack = false;
  mZenryokuParam.isUnique = false;
  mZenryokuParam.seq = WAZANO_NULL;

  //全力技の設定(補助技の場合seqはWAZANO_NULLになる
  pml::PokeType type = pml::wazadata::GetType(mWazaParam.mId);
  //派生技の場合、開始演出と技名が変わる恐れがある。テクスチャの分岐で使っているので上書き
  //(Zネコの手→各種攻撃技
  mZenryokuParam.wazaType = type;

  if( mWazaParam.mId >= WAZANO_URUTORADASSYUATAKKU && mWazaParam.mId <= WAZANO_FEARIIZENRYOKU )
  {
    mZenryokuParam.isAttack = true;
    mZenryokuParam.seq = GARC_wazaeff_seqfile_ez_0_01_BSEQ+type;
  }
  else
  {
    for( int i=0;i<GFL_NELEMS(ZenryokuDataTable);i++ )
    {
      if( ZenryokuDataTable[i].wazaNo == mWazaParam.mId &&
        (ZenryokuDataTable[i].form == -1 || 
         ZenryokuDataTable[i].form == GetBattlePokeParam(BtlPosToViewPos(atPokePos))->GetFormNo()))
      {
        mZenryokuParam.isAttack = true;
        mZenryokuParam.isUnique = true;
        mZenryokuParam.seq = ZenryokuDataTable[i].wazaSeq;
      }
    }
  }

  mComScene = 0;

  //上昇下降エフェクトの連続再生を抑制のリセット
  ResetCheckCanPlayEffect();
}

bool BattleViewSystem::CMD_ACT_WazaEffect_Wait(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);
  switch( mComScene )
  {
  case 0:
    if( mWazaParam.mSyncDamageEffect == false )
    {
      StartWazaEffect();
      mComScene++;
    }
    else
    {
      //エフェクトはダメージタイミング
      return true;
    }

    break;
  case 1:
    if( UpdateWazaEffect() )
    {
      //GetUiSys()->UpdateBallBar();
      return true;
    }
    break;
  }
  return false;
}
//技エフェクトスキップ時の背景破壊フラグのセット
void BattleViewSystem::CMD_ACT_SetObjBreakFlg(void)
{
  DEBUG_FUNC_CALL_TEMP;
  
}
  
void BattleViewSystem::CMD_ACT_DamageEffectSingle_Start( WazaID wazaID, BtlPokePos defPokePos, BtlTypeAffAbout affAbout )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;

  SetComTargetReset();
  mComTargetPoke[0] = BtlPosToViewPos(defPokePos);
  mComTargetPoke[1] = BtlPosToViewPos(defPokePos);
  mComTargetDamage[0] = BtlPosToViewPos(defPokePos);
  mWazaParam.mDmageAbout = affAbout;

  mComScene = 0;
  if( IsRecordSkipMode() )
  {
    mComScene = 100;
  }
}
bool BattleViewSystem::CMD_ACT_DamageEffectSingle_Wait(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);
  return CMD_ACT_DamageEffect_WaitCore();
}


//=============================================================================================
/**
  * 複数体一斉ダメージ処理 開始
  *
  * @param   wk
  * @param   pokeCnt      ポケモン数
  * @param   affifnity    相性ID
  * @param   pokeID       対象となるポケモンID配列
  * @param   pokeAffinity 対象となるポケモン一体一体の相性配列
  */
//=============================================================================================
void BattleViewSystem::CMD_ACT_DamageEffectPlural_Start( u32 pokeCnt, BtlTypeAffAbout affAbout, const u8* pokeID, const BtlTypeAff* pokeAffinity, WazaID waza )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;

  SetComTargetReset();
  if( mWazaParam.mSyncDamageEffect == false )
  {
    mComTargetPoke[0] = PokeIDToViewPos(pokeID[0]);
    mComTargetPoke[1] = PokeIDToViewPos(pokeID[0]);
    for( int i=0;i<BTL_EXIST_POS_MAX;i++ )
    {
      mComTargetDamage[i] = BTL_VPOS_ERROR;
    }
  }

  u32 cnt = 0;
  for( int i=0;i<BTL_EXIST_POS_MAX;i++ )
  {
    if( mComTargetDamage[i] == BTL_VPOS_ERROR )
    {
      mComTargetDamage[i] = PokeIDToViewPos(pokeID[cnt]);
      cnt++;
      if( cnt >= pokeCnt )
      {
        break;
      }
    }
  }

  mWazaParam.mDmageAbout = affAbout;

  mComScene = 0;

  if( IsRecordSkipMode() )
  {
    mComScene = 100;
  }  
}
bool BattleViewSystem::CMD_ACT_DamageEffectPlural_Wait(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);
  return CMD_ACT_DamageEffect_WaitCore();
}

void BattleViewSystem::CMD_ACT_MigawariDamageEffect_Start( WazaID wazaID, BtlPokePos migawariPos, BtlTypeAffAbout affAbout )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;

  CMD_ACT_DamageEffectSingle_Start( wazaID , migawariPos , affAbout );
}
bool BattleViewSystem::CMD_ACT_MigawariDamageEffect_Wait(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);
  return CMD_ACT_DamageEffect_WaitCore();
}
  
//上3つのダメージ処理(Single・Plural・Migawari)の共通処理
bool BattleViewSystem::CMD_ACT_DamageEffect_WaitCore(void)
{
  DEBUG_NODRAW_CHECK_PARAM(true);
  enum
  {
    SEQ_CHECK,
    SEQ_WAIT_WAZA,
    SEQ_SKIP_ZENRYOKU_START,
    SEQ_SKIP_ZENRYOKU_WAIT,

    SEQ_START_DAMAGE,
    SEQ_WAIT_DAMAGE,

    SEQ_REQ_SKIP = 100,
  };

  //この処理は1フレームに複数回呼ばれる場合がある。
  //ダブルバトルで片方みがわりの時に全体攻撃を打つと、
  //PluralとMigawariが同時に呼ばれる。
  switch( mComScene )
  {
  case SEQ_CHECK:
    if( mWazaParam.mSyncDamageEffect )
    {
      StartWazaEffect();
      mComScene = SEQ_WAIT_WAZA;
    }
    else
    {
      if( mZenryokuParam.isZenryoku == true )
      {
        BtlvObject_PokeModel *poke = GetPokeModel(mZenryokuParam.targetPoke);
        //死亡や退場している時は出さない
        if( poke->IsEnable() )
        {
          mComScene = SEQ_SKIP_ZENRYOKU_START;
        }
        else
        {
          mComScene = SEQ_START_DAMAGE;
        }
      }
      else
      {
        mComScene = SEQ_START_DAMAGE;
      }
    }
    break;
  case SEQ_WAIT_WAZA:
    if( UpdateWazaEffect() )
    {
      GetUiSys()->UpdateBallBarAll();
      return true;
    }
    break;
    //スキップ時に全力技系をリセットする対応
  case SEQ_SKIP_ZENRYOKU_START:
    //ターゲットのバックアップ
    {
      for(int i=0;i<BTL_EXIST_POS_MAX;i++)
      {
        mComTargetDamageBackup[i] = mComTargetDamage[i];
      }

      SetComTargetReset();

      mComTargetPoke[0] = mZenryokuParam.targetPoke;
      mComTargetPoke[1] = mZenryokuParam.targetPoke;
      mComTargetDamage[0] = mZenryokuParam.targetPoke;

      mTargetTrainer[0] = GetBtlvPosToTrainerIdx(mZenryokuParam.targetPoke);
      
      SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ez_cmn_skip_BSEQ );

      mComScene = SEQ_SKIP_ZENRYOKU_WAIT;
    }
    break;
  case SEQ_SKIP_ZENRYOKU_WAIT:
    if( SeqComFunc_IsFinishSequenceFile() )
    {
      SetComTargetReset();
      for(int i=0;i<BTL_EXIST_POS_MAX;i++)
      {
        mComTargetDamage[i] = mComTargetDamageBackup[i];
      }
      mComTargetPoke[0] = mComTargetDamage[0];
      mComTargetPoke[1] = mComTargetDamage[0];

      mComScene = SEQ_START_DAMAGE;
    }
    break;

  case SEQ_START_DAMAGE:
    {
      if( mWazaParam.mDmageAbout != pml::battle::TypeAffinity::TYPEAFF_ABOUT_NONE )
      {
        //効果の音でヒットSE
        SeqComFunc_PlaySequenceFileEffect( BTLEFF_SIMPLE_DAMAGE_NO_SOUND );
      }
      else
      {
        //シーケンスでヒットSE
        SeqComFunc_PlaySequenceFileEffect( BTLEFF_SIMPLE_DAMAGE );
      }
      //mUi->GaugeUpdate_Anime();
      mComScene = SEQ_WAIT_DAMAGE;
    }
  case SEQ_WAIT_DAMAGE:
    if( SeqComFunc_IsFinishSequenceFile() )
    {
      return true;
    }
    break;
  
  case SEQ_REQ_SKIP: //RecSkip時
    return true;
    //break;
  }

  return false;
}
 
/**
 * @param isKillCountEffect  「相手を倒した数の加算」演出を再生するか？
 */
void BattleViewSystem::CMD_StartDeadAct( BtlPokePos pokePos, bool isKillCountEffect )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;

  SetComTargetReset();
  mComTargetPoke[0] = BtlPosToViewPos(pokePos);
  mComTargetPoke[1] = BtlPosToViewPos(pokePos);
  mComTargetDamage[0] = BtlPosToViewPos(pokePos);
  mEffectBallId[0] = GetBattlePokeParam(BtlPosToViewPos(pokePos))->GetViewSrcData()->GetGetBall();
  mTargetTrainer[0] = GetBtlvPosToTrainerIdx(mComTargetPoke[0]);
  
  mComScene = 0;

  GetUiSys()->HideBallBarAll();
  GetUiSys()->HideGaugeAll();

  if( GetBattleCompetitor() != BTL_COMPETITOR_WILD ||
    IsPlayerSide(mComTargetPoke[0]) )
  {
    if( isKillCountEffect == true )
    {
      //トレーナー(撃墜数有
      mComEffectId = GARC_wazaeff_seqfile_ee392_3_BSEQ;
    }
    else
    {
      if (GetSetupStatusFlag(BTL_STATUS_FLAG_TRAINER_NO_RETURN_BALL) &&
        mComTargetPoke[0] == BTL_VPOS_FAR_1)
      {
        //ボールに入っていないポケモンなので、野生ダウンを仕様
        mComEffectId = GARC_wazaeff_seqfile_ee392_2_BSEQ;
      }
      else
      {
        //トレーナー
        mComEffectId = GARC_wazaeff_seqfile_ee392_BSEQ;
      }
    }
  }
  else
  {
    //野生
    mComEffectId = GARC_wazaeff_seqfile_ee392_2_BSEQ;
  }
  

  if( IsRecordSkipMode() )
  {
    mComScene = 255;
  }
}
bool BattleViewSystem::CMD_WaitDeadAct(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);

  enum
  {
    START_SEQ = 0,
    WAIT_SEQ,
    WAIT_DELETE,
    FINISH,

    SKIP_DELETE = 255,
    SKIP_DELETE_WAIT,
  };

  switch( mComScene )
  {
  case START_SEQ:
    SeqComFunc_PlaySequenceFile( mComEffectId );
    mComScene++;
    //break;  //through
  case WAIT_SEQ:
    if( SeqComFunc_IsFinishSequenceFile() )
    {
      BtlvObject_PokeModel *mdl = GetPokeModel(mComTargetPoke[0]);
      mdl->SetVisible(false);
      mdl->RemoveRenderPath();
      mComScene++;
    }
    break;
  case WAIT_DELETE:
    {
      BtlvObject_PokeModel *mdl = GetPokeModel(mComTargetPoke[0]);
      if( mdl->GetPokeModel()->CanDelete() == false )
      {
        break;
      }
      if( mdl->GetMigawariModel() &&
        mdl->GetMigawariModel()->GetBaseModel()->CanDestroy() == false )
      {
        break;
      }
      if( GetRenderSys()->GetRenderingPipeLine()->CheckDeleteFPModel() == false )
      {
        break;
      }
      mdl->Delete();

      ResetDefaultCamera(0);

      mComScene++;
    }
    break;
  case FINISH:
    return true;

  case SKIP_DELETE:
    {
      BtlvObject_PokeModel *mdl = GetPokeModel(mComTargetPoke[0]);
      mdl->SetVisible(false);
      mdl->RemoveRenderPath();
      mComScene++;
    }
    //break;  //through
  case SKIP_DELETE_WAIT:
    {
      BtlvObject_PokeModel *mdl = GetPokeModel(mComTargetPoke[0]);
      if( mdl->GetPokeModel()->CanDelete() == false )
      {
        break;
      }
      if( mdl->GetMigawariModel() &&
          mdl->GetMigawariModel()->GetBaseModel()->CanDestroy() == false )
      {
        break;
      }
      if( GetRenderSys()->GetRenderingPipeLine()->CheckDeleteFPModel() == false )
      {
        break;
      }
      mdl->Delete();
      mComScene = FINISH;
      return true;
    }
    //break;
  }
  return false;
}
void BattleViewSystem::CMD_StartDeadActPlural( u32 pokeCnt, const BtlPokePos *pokePos )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;
  //@caution nijiでは未使用なので動作未確認
  CMD_StartDeadAct(pokePos[0], false);
}
bool BattleViewSystem::CMD_WaitDeadActPlural(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);
  //@caution nijiでは未使用なので動作未確認
  return CMD_WaitDeadAct();
}

void BattleViewSystem::CMD_StartReliveAct( BtlPokePos pos )
{
  DEBUG_FUNC_CALL_TEMP;
  
}
bool BattleViewSystem::CMD_WaitReliveAct(void)
{
  DEBUG_FUNC_CALL_TEMP;
  return true;
}
void BattleViewSystem::CMD_ACT_MemberOut_Start( BtlvPos vpos, u16 effectNo )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;

  GetUiSys()->HideBallBarAll();

  SetComTargetReset();
  mComTargetPoke[0] = vpos;
  mComTargetPoke[1] = vpos;
  mComTargetDamage[0] = vpos;

  mTargetTrainer[0] = GetBtlvPosToTrainerIdx(vpos);
  mEffectBallId[0] = GetBattlePokeParam(vpos)->GetViewSrcData()->GetGetBall();
  mComScene = 0;
  mComEffectId = effectNo;

  if( IsRecordSkipMode() )
  {
    mComScene = 255;
  }
}
bool BattleViewSystem::CMD_ACT_MemberOut_Wait(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);

  enum
  {
    START_SEQ = 0,
    WAIT_SEQ,
    WAIT_DELETE,
    FINISH,

    SKIP_DELETE = 255,
    SKIP_DELETE_WAIT,
  };

  switch( mComScene )
  {
  case START_SEQ:
    if( mComEffectId == 0 || mComEffectId == BTLEFF_POKEMON_MODOSU )
    {
      SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee391_BSEQ );
    }
    else
    {
      SeqComFunc_PlaySequenceFileEffect( mComEffectId );
    }
    mComScene++;
    //break;  //through
  case WAIT_SEQ:
    if( SeqComFunc_IsFinishSequenceFile() )
    {
      BtlvObject_PokeModel *mdl = GetPokeModel(mComTargetPoke[0]);
      mdl->SetVisible(false);
      mdl->RemoveRenderPath();
      mComScene++;
    }
    break;
  case WAIT_DELETE:
    {
      BtlvObject_PokeModel *mdl = GetPokeModel(mComTargetPoke[0]);
      if( mdl->GetPokeModel()->CanDelete() == false )
      {
        break;
      }
      if( mdl->GetMigawariModel() &&
        mdl->GetMigawariModel()->GetBaseModel()->CanDestroy() == false )
      {
        break;
      }
      if( GetRenderSys()->GetRenderingPipeLine()->CheckDeleteFPModel() == false )
      {
        break;
      }
      mdl->Delete();

      ResetDefaultCamera(0);

      mComScene++;
    }
    break;
  case FINISH:
    return true;

  case SKIP_DELETE:
    {
      BtlvObject_PokeModel *mdl = GetPokeModel(mComTargetPoke[0]);
      mdl->SetVisible(false);
      mdl->RemoveRenderPath();
      mComScene++;
    }
    //break;  //through
  case SKIP_DELETE_WAIT:
    {
      BtlvObject_PokeModel *mdl = GetPokeModel(mComTargetPoke[0]);
      if( mdl->GetPokeModel()->CanDelete() == false )
      {
        break;
      }
      if( mdl->GetMigawariModel() &&
          mdl->GetMigawariModel()->GetBaseModel()->CanDestroy() == false )
      {
        break;
      }
      if( GetRenderSys()->GetRenderingPipeLine()->CheckDeleteFPModel() == false )
      {
        break;
      }
      mdl->Delete();
      mComScene = FINISH;
      return true;
    }
    //break;
  }
  return false;
}
void BattleViewSystem::CMD_ACT_TameWazaHide( BtlvPos vpos, BTLV_VANISH_FLAG vanishFlag )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;

  if( vanishFlag == BTLV_VANISH_OFF )
  {
    GetPokeModel(vpos)->SetVisibleTame( true );

    GetPokeModel(vpos)->SetVisible( true );

    ResetPokemon(vpos,0);
  }
  else
    if( vanishFlag == BTLV_VANISH_ON )
    {
      GetPokeModel(vpos)->SetVisibleTame( false );
    }
}
void BattleViewSystem::CMD_ACT_SimpleHPEffect_Start( BtlPokePos pokePos )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;
  
  mWazaParam.mDmageAbout = pml::battle::TypeAffinity::TYPEAFF_ABOUT_NONE;
  mComScene = 0;

}
bool BattleViewSystem::CMD_ACT_SimpleHPEffect_Wait(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);

    switch( mComScene )
  {
    case 0:
      GetUiSys()->GaugeUpdate_Anime();
      mComScene = 10;
      break;
    case 10:
      if( GetUiSys()->IsAnimeGauge() == false )
      {
        return true;
      }
      break;
  }
  return false;
}

void BattleViewSystem::CMD_AddEffect( u16 effectNo )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;

  SetComTargetReset();
  mComTargetPoke[0] = BTL_VPOS_NEAR_1;
  mComTargetPoke[1] = BTL_VPOS_NEAR_1;
  mComTargetDamage[0] = BTL_VPOS_NEAR_1;

  if( effectNo == BTLEFF_ZOOM_IN_RESET &&
    mIsMigawariReserve )
  {
    //今はZOOM_IN_RESETはここでしか使われていない。
    //しかも処理もないので再生はしない
    //連続技の後の身代わり戻し
    mIsMigawariReserve = false;
    if( mComTargetMigawariReserve != BTL_VPOS_ERROR &&
      GetPokeModel(mComTargetMigawariReserve) &&
      GetPokeModel(mComTargetMigawariReserve)->GetVisible() )
    {
      mComTargetPoke[0] = mComTargetMigawariReserve;
      mComTargetPoke[1] = mComTargetMigawariReserve;
      mComTargetDamage[0] = mComTargetMigawariReserve;
      mComTargetMigawariReserve = BTL_VPOS_ERROR;
      SeqComFunc_PlaySequenceFileEffect( BTLEFF_MIGAWARI_WAZA_AFTER );
    }
  }
  else
  {
    SeqComFunc_PlaySequenceFileEffect( effectNo );
  }

}
bool BattleViewSystem::CMD_WaitEffect(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);
  if( SeqComFunc_IsFinishSequenceFile() )
  {
    ResetDefaultCamera(0);
    return true;
  }

  return false;
}
void BattleViewSystem::CMD_AddEffectByPos( BtlvPos vpos, u16 effectNo )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;
  
  if( vpos == BTL_VPOS_ERROR )
  {
    //nijiでBtlPokePosからBtlvPosへの変換関数の挙動が変わり、BTL_POKE_NULLがBTL_VPOS_ERRORになるようになった。
    //なので、ここに来て吹っ飛ぶケースがあった。基本的に非表示の時は出さないはずなので、
    //保険としてASSERTと例外処理を挟んでおく。
    GFL_ASSERT_MSG(0,"AddEffectByPos target is ERROR![%d]\n",effectNo); //@check
    mComScene = 255;
    return;
  }

  SetComTargetReset();
  mComTargetPoke[0] = vpos;
  mComTargetPoke[1] = vpos;
  mComTargetDamage[0] = vpos;
  bool chekcVisible = true;
  mComScene = 0;

  //GFNMCat[5793] 全力技の途中で他のエフェクトが発生すると、全力技のリソースも開放してしまい停止
  //発生条件① 防御側がみがわり、タイプ無効特性
  //発生条件② 防御側がみがわり、半減木の実持ち、攻撃側がすり抜け
  //みがわりがある事で表示の判定が間違った結果になり、能力上昇、回復、木の実使用が発生してしまう
  //表示判定を直すのはコストが高いので例外対処を入れて対応する
  if (mZenryokuParam.isZenryoku == true &&
    (effectNo == BTLEFF_HP_RECOVER || effectNo == BTLEFF_EAT_NUTS_POKE))
  {
    mComScene = 255;
    return;
  }

  //KujiraBTS3677 非表示でも表示するエフェクト。
  //SWANではエフェクトのコマンドの中で対処していた。
  //今回はできないので、プログラムで対応。
  //もちろん該当するエフェクトのコマンドによっては飛ぶので注意
  //niji追記 エフェクト内の分岐は現実的ではないと思う。呼び出し関数自体を分けるなりした方が健全
  if( effectNo == BTLEFF_RAINBOW ||
      effectNo == BTLEFF_BURNING ||
      effectNo == BTLEFF_MOOR )
  {
    chekcVisible = false;
  }
  if( chekcVisible == false ||
    (GetPokeModel(vpos) &&
     GetPokeModel(vpos)->IsEnable() &&
     GetPokeModel(vpos)->GetVisible()) )
  {  
    if( effectNo == BTLEFF_FUNJIN &&
        mZenryokuParam.isZenryoku == true )
    {
      //全力技中に粉塵で失敗する場合、諸々のリセットが必要
      mComScene = 50;
      SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ez_cmn_skip_BSEQ );
    }
    else
    {
      SeqComFunc_PlaySequenceFileEffect( effectNo );
    }
  }
  else
  {
    //いなくても処理をする例外エフェクト
    if( effectNo == BTLEFF_MIGAWARI_DEAD )
    {
      //身代わりを消しておく
      if( GetPokeModel(vpos) )
      {
        GetPokeModel(vpos)->SetVisibleMigawari(false);
      }
    }
    
    mComScene = 255;
  }
}
bool BattleViewSystem::CMD_WaitEffectByPos(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);
  if( mComScene == 255 )
  {
    return true;
  }

  switch( mComScene )
  {
  case 0:
    //通常処理
    if( SeqComFunc_IsFinishSequenceFile() )
    {
      ResetDefaultCamera(0);
      return true;
    }
    break;
  case 50:
    //粉塵での失敗処理
    if( SeqComFunc_IsFinishSequenceFile() )
    {
      SeqComFunc_PlaySequenceFileEffect( BTLEFF_FUNJIN );
      mComScene = 0;  //本流に戻す
    }
    break;
  }
  return false;
}
void BattleViewSystem::CMD_AddEffectByDir( BtlvPos vpos_from, BtlvPos vpos_to, u16 effectNo )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;

  SetComTargetReset();
  mComTargetPoke[0] = vpos_from;
  mComTargetPoke[1] = vpos_to;
  mComTargetDamage[0] = vpos_to;
  mComScene = 0;

  //Dirの方は攻撃側がいない時に非表示
  if( GetPokeModel(vpos_from) &&
      GetPokeModel(vpos_from)->IsEnable() &&
      GetPokeModel(vpos_from)->GetVisible() )
  {
    SeqComFunc_PlaySequenceFileEffect( effectNo );
  }
  else
  {
    mComScene = 255;
  }

}
bool BattleViewSystem::CMD_WaitEffectByDir(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);
  if( mComScene == 255 )
  {
    return true;
  }

  if( SeqComFunc_IsFinishSequenceFile() )
  {
    ResetDefaultCamera(0);
    return true;
  }
  return false;
}


void BattleViewSystem::CMD_TokWin_DispStart( BtlPokePos pos, bool fFlash )
{
  GFL_UNUSED(fFlash);
  DEBUG_NODRAW_CHECK;

  mComScene = 0;
  
  if( GetUiSys()->IsDispTokWin( BtlPosToViewPos(pos) ) == false )
    {
      GetUiSys()->DispTokWin( BtlPosToViewPos(pos) );
      PlaySe(SEQ_SE_BT_030);
    }
  else if( GetUiSys()->GetTokWinDispPos( BtlPosToViewPos(pos) ) != BtlPosToViewPos(pos) )
    {
      //一回非表示にしてから
      mComScene = 100;
      GetUiSys()->HideTokWin( BtlPosToViewPos(pos) );
    }
  else
    {
      mComScene = 255;
    }

  //上昇下降エフェクトの連続再生を抑制するため
  ResetCheckCanPlayEffect();
}

bool BattleViewSystem::CMD_TokWin_DispWait( BtlPokePos pos )
{
  DEBUG_NODRAW_CHECK_PARAM(true);

  if( mComScene == 255 )
  {
    return true;
  }

  if( mComScene == 100 )
  {
    if( GetUiSys()->WaitHideTokWin() )
    {
      mComScene = 0;
      GetUiSys()->DispTokWin( BtlPosToViewPos(pos) );
      PlaySe(SEQ_SE_BT_030);
    }
  }
  else
  //スキルスワップは2連続でこのコマンドが呼ばれているので、最初の方は待てない。
    if( IsFinishSe( SEQ_SE_BT_030 ) )
  {
    return GetUiSys()->WaitDispTokWin();
  }
  return false;
}

void BattleViewSystem::CMD_QuitTokWin( BtlPokePos pos )
{
  DEBUG_NODRAW_CHECK;
  mComScene = 0;

  if( GetUiSys()->IsDispTokWin( BtlPosToViewPos(pos) ) == true )
  {
    GetUiSys()->HideTokWin( BtlPosToViewPos(pos) );
  }
  else
  {
    mComScene = 255;
  }
  
}

bool BattleViewSystem::CMD_QuitTokWinWait( BtlPokePos pos )
{
  DEBUG_NODRAW_CHECK_PARAM(true);
  if( mComScene == 255 )
  {
    return true;
  }
  return GetUiSys()->WaitHideTokWin();
}

void BattleViewSystem::CMD_TokWin_Renew_Start( BtlPokePos pos )
{
  DEBUG_NODRAW_CHECK;
  SetComTargetReset();
  mComTargetPoke[0] = BtlPosToViewPos(pos);
  mComScene = 0;
}


bool BattleViewSystem::CMD_TokWin_Renew_Wait( BtlPokePos pos )
{
  DEBUG_NODRAW_CHECK_PARAM(true);
  switch( mComScene )
  {
  case 0:
    GetUiSys()->HideTokWin( mComTargetPoke[0] );
    mComScene++;
    break;
  case 1:
    if( GetUiSys()->WaitHideTokWin() )
    {
      GetUiSys()->DispTokWin( mComTargetPoke[0] );
      PlaySe(SEQ_SE_BT_030);
      mComScene++;
    }
    break;
  case 2:
    if( IsFinishSe( SEQ_SE_BT_030 ) )
    {
      return GetUiSys()->WaitDispTokWin();
    }
    break;
  }
  
  return false;
}

// ----------------------------------

void BattleViewSystem::CMD_TokWin_SwapStart( BtlPokePos pos1, BtlPokePos pos2 )
{
  DEBUG_NODRAW_CHECK;
  SetComTargetReset();
  mComTargetPoke[0] = BtlPosToViewPos(pos1);
  mComTargetPoke[1] = BtlPosToViewPos(pos2);
  mComScene = 0;
}

bool BattleViewSystem::CMD_TokWin_SwapWait(void)
{
  DEBUG_NODRAW_CHECK_PARAM(true);
  switch( mComScene )
  {
  case 0:
    GetUiSys()->HideTokWin( mComTargetPoke[0] );
    GetUiSys()->HideTokWin( mComTargetPoke[1] );
    mComScene++;
    break;
  case 1:
    if( GetUiSys()->WaitHideTokWin() )
    {
      GetUiSys()->DispTokWin( mComTargetPoke[0] );
      GetUiSys()->DispTokWin( mComTargetPoke[1] );
      PlaySe(SEQ_SE_BT_030);
      mComScene++;
    }
    break;
  case 2:
    if( IsFinishSe( SEQ_SE_BT_030 ) )
    {
      return GetUiSys()->WaitDispTokWin();
    }
    break;
  }
  
  return false;
}

// ----------------------------------

/**
 * @param vpos          能力ランクがダウンするポケモンの見た目位置
 * @param rankUpVolume  能力ランクが何段階ダウンするか( 正数 ) ※UIで使う予定だったが未使用
 */
void BattleViewSystem::CMD_StartRankDownEffect( u8 vpos, u8 rankDownVolume )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;

  SetComTargetReset();
  mComTargetPoke[0] = static_cast<BtlvPos>(vpos);

  //GFNMCat[5793] MMCat[5] 全力技の途中で他のエフェクトが発生すると、全力技のリソースも開放してしまい停止
  //発生条件① 防御側がみがわり、タイプ無効特性
  //発生条件② 防御側がみがわり、半減木の実持ち、攻撃側がすり抜け
  //みがわりがある事で表示の判定が間違った結果になり、能力上昇、回復、木の実使用が発生してしまう
  //表示判定を直すのはコストが高いので例外対処を入れて対応する
  if (mZenryokuParam.isZenryoku == true &&
      GetResourceManager()->IsReleaseAllResouce() == false )
  {
    mComScene = 100;
  }
  else
  if (GetPokeModel(mComTargetPoke[0])->GetVisible() ||
    GetPokeModel(mComTargetPoke[0])->IsVisibleMigawari() )
  {
    SeqComFunc_PlaySequenceFileEffect( BTLEFF_STATUS_DOWN );
    mComScene = 0;
  }
  else
  {
    mComScene = 100;
  }

}
/**
 * @param vpos          能力ランクがアップするポケモンの見た目位置
 * @param rankUpVolume  能力ランクが何段階アップするか ※UIで使う予定だったが未使用
 */
void BattleViewSystem::CMD_StartRankUpEffect( u8 vpos, u8 rankUpVolume )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;

  SetComTargetReset();
  mComTargetPoke[0] = static_cast<BtlvPos>(vpos);

  //GFNMCat[5793] MMCat[5] 全力技の途中で他のエフェクトが発生すると、全力技のリソースも開放してしまい停止
  //発生条件① 防御側がみがわり、タイプ無効特性
  //発生条件② 防御側がみがわり、半減木の実持ち、攻撃側がすり抜け
  //みがわりがある事で表示の判定が間違った結果になり、能力上昇、回復、木の実使用が発生してしまう
  //表示判定を直すのはコストが高いので例外対処を入れて対応する
  if (mZenryokuParam.isZenryoku == true &&
      GetResourceManager()->IsReleaseAllResouce() == false)
  {
    mComScene = 100;
  }
  else
  if (GetPokeModel(mComTargetPoke[0])->GetVisible() ||
      GetPokeModel(mComTargetPoke[0])->IsVisibleMigawari() )
  {
    SeqComFunc_PlaySequenceFileEffect( BTLEFF_STATUS_UP );
    mComScene = 0;
  }
  else
  {
    mComScene = 100;
  }
}
bool BattleViewSystem::CMD_WaitRankEffect( u8 vpos )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);
  switch( mComScene )
  {
  case 0:
    if( SeqComFunc_IsFinishSequenceFile() )
    {
      return true;
    }
    break;
  case 20:
    return true;
    //break;
  case 100:
    return true;
    //break;
  }
  return false;
}


void BattleViewSystem::CMD_StartCommWait(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;
  GetUiSys()->DispWaitCommStr( true );
}
bool BattleViewSystem::CMD_WaitCommWait(void)
{
  DEBUG_FUNC_CALL_TEMP;
  return true;
}
void BattleViewSystem::CMD_ResetCommWaitInfo(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;

  GetUiSys()->DispWaitCommStr( false );
}

void BattleViewSystem::CMD_ItemAct_Start( BtlPokePos pos )
{
  //DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;
  CMD_AddEffectByPos(BtlPosToViewPos(pos),BTLEFF_SOUBI_ITEM);
}
bool BattleViewSystem::CMD_ItemAct_Wait( BtlPokePos pos )
{
  //DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);
  return CMD_WaitEffectByPos();
}
void BattleViewSystem::CMD_KinomiAct_Start( BtlPokePos pos )
{
  //DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;
  CMD_AddEffectByPos(BtlPosToViewPos(pos),BTLEFF_EAT_NUTS_POKE);
}
bool BattleViewSystem::CMD_KinomiAct_Wait( BtlPokePos pos )
{
  //DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);
  return CMD_WaitEffectByPos();
}

void BattleViewSystem::CMD_FakeDisable_Start( BtlPokePos pos, bool fSkipMode )
{
  DEBUG_FUNC_CALL;
  DEBUG_NODRAW_CHECK;

  GFL_UNUSED(fSkipMode);

  SetComTargetReset();
  mComTargetPoke[0] = BtlPosToViewPos(pos);
  mComTargetPoke[1] = BtlPosToViewPos(pos);
  mComTargetDamage[0] = BtlPosToViewPos(pos);;
  CMD_ACT_PokeChangeEffect_StartCore(mComTargetPoke[0],BTLEFF_ILLUSION,false);
}
bool BattleViewSystem::CMD_FakeDisable_Wait(void)
{
  DEBUG_FUNC_CALL;
  DEBUG_NODRAW_CHECK_PARAM(true);
  return CMD_ACT_PokeChangeEffect_WaitCore();
}
void BattleViewSystem::CMD_ChangeForm_Start( BtlvPos vpos )
{
  DEBUG_FUNC_CALL;
  DEBUG_NODRAW_CHECK;

  SetComTargetReset();
  mComTargetPoke[0] = vpos;
  mComTargetPoke[1] = vpos;
  mComTargetDamage[0] = vpos;

  const BTL_POKEPARAM *bpp = GetBattlePokeParam( vpos );
  //例外対応
  if( bpp->GetMonsNo() == MONSNO_GIRUGARUDO )
  {
    if( bpp->GetFormNo() == FORMNO_GIRUGARUDO_TATE )
    {
      if( mZenryokuParam.isZenryoku == true )
      {
        CMD_ACT_PokeChangeEffect_StartCore(vpos,BTLEFF_FORMCHANGE_GIRUGARUDO_HOKO_TO_TATE_Z,true);
      }
      else
      {
        CMD_ACT_PokeChangeEffect_StartCore(vpos,BTLEFF_FORMCHANGE_GIRUGARUDO_HOKO_TO_TATE,true);
      }
    }
    else
    {
      if( mZenryokuParam.isZenryoku == true )
      {
        CMD_ACT_PokeChangeEffect_StartCore(vpos,BTLEFF_FORMCHANGE_GIRUGARUDO_TATE_TO_HOKO_Z,true);
      }
      else
      {
        CMD_ACT_PokeChangeEffect_StartCore(vpos,BTLEFF_FORMCHANGE_GIRUGARUDO_TATE_TO_HOKO,true);
      }
    }
  }
  else
  if( bpp->GetMonsNo() == MONSNO_KAIOOGA )
  {
    CMD_ACT_PokeChangeEffect_StartCore(vpos,BTLEFF_FORMCHANGE_KAIOOGA,false);
  }
  else
  if( bpp->GetMonsNo() == MONSNO_GURAADON )
  {
    CMD_ACT_PokeChangeEffect_StartCore(vpos,BTLEFF_FORMCHANGE_GURAADON,false);
  }
  else
  if( bpp->GetMonsNo() == MONSNO_YOWASI )
  {
    if( bpp->GetFormNo() == FORMNO_YOWASI_GYOGUN_L )
    {
      CMD_ACT_PokeChangeEffect_StartCore(vpos,BTLEFF_FORMCHANGE_YOWASHI_SMALL_TO_BIG,false);
    }
    else
    {
      CMD_ACT_PokeChangeEffect_StartCore(vpos,BTLEFF_FORMCHANGE_YOWASHI_BIG_TO_SMALL,false);
    }
  }
  else
  if( bpp->GetMonsNo() == MONSNO_HORAA )
  {
    CMD_ACT_PokeChangeEffect_StartCore(vpos,BTLEFF_FORMCHANGE_MIMIKKYU,true);
  }
  else
  if( bpp->GetMonsNo() == MONSNO_NAGAREBOSI )
  {
    //この時点のフォルムは変化後の値
    if( bpp->GetFormNo() <= FORMNO_NAGAREBOSI_M_PURPLE )
    {
      //殻無しから殻が付く時は汎用演出
      CMD_ACT_PokeChangeEffect_StartCore(vpos,BTLEFF_FORMCHANGE,false);
    }
    else
    {
      //殻が割れる時は専用演出
      CMD_ACT_PokeChangeEffect_StartCore(vpos,BTLEFF_FORMCHANGE_METENO,true);
    }
  }
  else
  if( bpp->GetMonsNo() == MONSNO_GEKKOUGA )
  {
    CMD_ACT_PokeChangeEffect_StartCore(vpos,BTLEFF_FORMCHANGE_GEKKOUGA_SATOSHI,false);
  }
  else 
  if(bpp->GetMonsNo() == MONSNO_ZIGARUDE)
  {
    CMD_ACT_PokeChangeEffect_StartCore(vpos,BTLEFF_FORMCHANGE_JIGARUDE,false);
  }
  else
  {
    CMD_ACT_PokeChangeEffect_StartCore(vpos,BTLEFF_FORMCHANGE,false);
  }
}
bool BattleViewSystem::CMD_ChangeForm_Wait(void)
{
  DEBUG_FUNC_CALL;
  DEBUG_NODRAW_CHECK_PARAM(true);
  return CMD_ACT_PokeChangeEffect_WaitCore();
}
void BattleViewSystem::CMD_Hensin_Start( BtlvPos atkVpos, BtlvPos tgtVpos )
{
  DEBUG_FUNC_CALL;
  DEBUG_NODRAW_CHECK;
  
  SetComTargetReset();
  mComTargetPoke[0] = atkVpos;
  mComTargetPoke[1] = tgtVpos;  //変身はこれに変身する
  mComTargetDamage[0] = tgtVpos;
  CMD_ACT_PokeChangeEffect_StartCore(atkVpos,BTLEFF_HENSHIN,false);
}
bool BattleViewSystem::CMD_Hensin_Wait(void)
{
  DEBUG_FUNC_CALL;
  DEBUG_NODRAW_CHECK_PARAM(true);
  return CMD_ACT_PokeChangeEffect_WaitCore();
}

//上3つの読み替え処理ダメージ処理(イリュージョン・へんしん・フォルムチェンジ)の共通処理
void BattleViewSystem::CMD_ACT_PokeChangeEffect_StartCore(BtlvPos vpos,u16 effId,bool isNoLoad )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;

  mComEffectId = effId;
  mIsMigawariWaza = GetPokeModel(vpos)->IsVisibleMigawari();

  if( IsRecordSkipMode() ||
      GetPokeModel(vpos)->GetVisible() == false )
  {
    if( isNoLoad== true )
    {
      mComScene = 120;
    }
    else
    {
      mComScene = 100;
    }
  }
  else
  {
    mComScene = 0;
  }  
}
bool BattleViewSystem::CMD_ACT_PokeChangeEffect_WaitCore(void)
{
  DEBUG_NODRAW_CHECK_PARAM(true);
  enum
  {
    HIDE_TOK_WIN = 0,
    HIDE_TOK_WIN_WAIT,
    MIGAWARI_CHECK,

    MIGAWARI_OUT,
    MIGAWARI_OUT_WAIT,
    MIGAWARI_OUT_UPDATE,

    LOAD_WAIT,
    UPDATE_EFFECT,

    MIGAWARI_IN,
    MIGAWARI_IN_WAIT,
    MIGAWARI_IN_UPDATE,

    FINISH_WAZA,
  };
  
  if( mComScene >= 100 )
  {
    //メガシンカと共通化のため。
    return CMD_ACT_PokeChangeEffect_WaitCore_Skip();
  }
  else
  {
    switch( mComScene )
    {
    case HIDE_TOK_WIN:
      mComScene = MIGAWARI_CHECK;
      //どちらかの特性ウィンドウが出ていたら、消し待ちへ行く
      if( GetUiSys()->IsDispTokWin( BTL_VPOS_NEAR_1 ) == true )
      {
        GetUiSys()->HideTokWin(BTL_VPOS_NEAR_1);
        mComScene = HIDE_TOK_WIN_WAIT;
      }
      if( GetUiSys()->IsDispTokWin( BTL_VPOS_FAR_1 ) == true )
      {
        GetUiSys()->HideTokWin(BTL_VPOS_FAR_1);
        mComScene = HIDE_TOK_WIN_WAIT;
      }
      break;

    case HIDE_TOK_WIN_WAIT:
      if( GetUiSys()->WaitHideTokWin() == false )
      {
        return false;
      }
      mComScene = MIGAWARI_CHECK;
      //break; //through

    case MIGAWARI_CHECK:
      if( mIsMigawariWaza )
      {
        mComScene = MIGAWARI_OUT;
      }
      else
      {
        SeqComFunc_PlaySequenceFileEffect( mComEffectId );
        mComScene = UPDATE_EFFECT;
      }
      break;

    case MIGAWARI_OUT:
      SeqComFunc_PlaySequenceFileEffect( BTLEFF_MIGAWARI_WAZA_BEFORE );
      mComScene = MIGAWARI_OUT_UPDATE;
      break;
    case MIGAWARI_OUT_UPDATE:
      if( SeqComFunc_IsFinishSequenceFile() )
      {
        SeqComFunc_PlaySequenceFileEffect( mComEffectId );
        mComScene = UPDATE_EFFECT;
      }
      break;

    case UPDATE_EFFECT:
      {
        const BTL_POKEPARAM *bpp = GetBattlePokeParam( mComTargetPoke[0] );
        if( bpp->GetMonsNo() == MONSNO_GIRUGARUDO &&
            mZenryokuParam.isZenryoku == true )
        {
          //全力技中は開放するとまずい。保険でギルガルドだけの処理にしておく
          if( SeqComFunc_IsFinishSequenceFile(true))
          {
            if( mIsMigawariWaza )
            {
              mComScene = MIGAWARI_IN;
            }
            else
            {
              mComScene = FINISH_WAZA;
            }
          }
        }
        else
        {
          if( SeqComFunc_IsFinishSequenceFile())
          {
            //ジガルデがHPが変わるようになったのでゲージ更新が必要
            GetUiSys()->GaugeUpdateReq();
            if( mIsMigawariWaza )
            {
              mComScene = MIGAWARI_IN;
            }
            else
            {
              mComScene = FINISH_WAZA;
            }
          }
        }
      }
      break;


    case MIGAWARI_IN:
      SeqComFunc_PlaySequenceFileEffect( BTLEFF_MIGAWARI_WAZA_AFTER );
      mComScene = MIGAWARI_IN_UPDATE;
      break;
    case MIGAWARI_IN_UPDATE:
      if( SeqComFunc_IsFinishSequenceFile() )
      {
        mComScene = FINISH_WAZA;
      }
      break;

    case FINISH_WAZA:
      return true;
      //break;
    }
  }
  return false;
}

bool BattleViewSystem::CMD_ACT_PokeChangeEffect_WaitCore_Skip(void)
{
  DEBUG_NODRAW_CHECK_PARAM(true);
  //メガシンカと共通化
  //必ずseqは100から
  enum
  {
    SKIP_START = 100,
    SKIP_WAIT,
    SKIP_FINISH,
    
    SKIP_GIRUGARUDO = 120,
  };

  switch( mComScene )
  {
  case SKIP_START:
    CreateTask_HenshinPokemon( GetTaskSystemAlways(),GetTempHeap(),this,mComTargetPoke[0],mComTargetPoke[1] );
    mComIsVisibleTame = mPokeModel[mComTargetPoke[0]]->IsVisibleTame();
    mComScene++;
    break;
  case SKIP_WAIT:
    if( GetTaskSystemAlways()->GetActiveTaskNum() == 0  )
    {
      GetPokeModel(mComTargetPoke[0])->SetVisibleTame(mComIsVisibleTame);
      GetPokeModel(mComTargetPoke[0])->SetVisibleMigawari(mIsMigawariWaza);
      GetPokeModel(mComTargetPoke[0])->SetVisible(true);
      ResetTrainer(GetBtlvPosToTrainerIdx(mComTargetPoke[0]));
      //NMCat4032 本来は全部でチェックして良いが、修正時期を鑑みてジガルデのみに影響を限定
      const BTL_POKEPARAM *bpp = GetBattlePokeParam( mComTargetPoke[0] );
      if( bpp->GetMonsNo() == MONSNO_ZIGARUDE )
      {
        //ジガルデがHPが変わるようになったのでゲージ更新が必要
        GetUiSys()->GaugeUpdateReq();
      }
      mComScene++;
    }
    break;
  case SKIP_FINISH:
    return true;
    //break;
  
  case SKIP_GIRUGARUDO:
    GetPokeModel(mComTargetPoke[0])->NoLoadFormChange();
    mComScene = SKIP_FINISH;
    break;
  }
  return false;

}

void BattleViewSystem::CMD_ACT_MoveMember_Start( u8 clientID, u8 vpos1, u8 vpos2, u8 posIdx1, u8 posIdx2 )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;

  SetComTargetReset();
  mComTargetPoke[0] = (BtlvPos)vpos1;
  mComTargetPoke[1] = (BtlvPos)vpos2;

  mComScene = 0;
 
}
bool BattleViewSystem::CMD_ACT_MoveMember_Wait(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);

  switch( mComScene )
  {
  case 0:
    {
      BtlvObject_PokeModel* temp = mPokeModel[mComTargetPoke[0]];
      mPokeModel[mComTargetPoke[0]] = mPokeModel[mComTargetPoke[1]];
      mPokeModel[mComTargetPoke[1]] = temp;

      int tempIdx1 = GetPokeModel(mComTargetPoke[0])->GetIndex();
      int tempIdx2 = GetPokeModel(mComTargetPoke[1])->GetIndex();
      GetPokeModel(mComTargetPoke[0])->SetIndex(tempIdx2);
      GetPokeModel(mComTargetPoke[1])->SetIndex(tempIdx1);

      gfl2::heap::HeapBase* tempHeap = mPokeHeap[tempIdx1];
      mPokeHeap[tempIdx1] = mPokeHeap[tempIdx2];
      mPokeHeap[tempIdx2] = tempHeap;

      System::nijiAllocator *tempAllocator = mPokeGLAllocator[tempIdx1];
      mPokeGLAllocator[tempIdx1] = mPokeGLAllocator[tempIdx2];
      mPokeGLAllocator[tempIdx2] = tempAllocator;

      gfl2::math::Vector3 pos;
      int deg;
      for( int i=0;i<2;i++ )
      {
        ResetPokemon(mComTargetPoke[i],0);
        GetPokeModel(mComTargetPoke[i])->SetVisible(true);
        GetUiSys()->ResetGaugeParam( mComTargetPoke[i] );
      }
      GetUiSys()->GaugeUpdateReq();

      mComScene++;
    }
    break;
  case 1:
    return true;
  }

  return false;
}


/**
 * @brief 全力技の開始演出を開始する
 * @param pokePos    全力技を使用するポケモンの位置
 * @param wazaNo     実行する全力技の技No.( ただし、実行する技が「ゆびをふる」等の派生技の場合は、派生技の技No. )
 * @param strId      全力効果発動時に表示するメッセージID( BTL_STRID_SET_xxxx )
 * @param strParam1  全力効果発動時に表示するメッセージの引数1
 * @param strParam2  全力効果発動時に表示するメッセージの引数2
 * @param strParam3  全力効果発動時に表示するメッセージの引数3
 */
void BattleViewSystem::CMD_ACT_ZenryokuWazaBegin_Start( BtlPokePos pokePos, BtlPokePos targetPos, WazaNo wazaNo, u16 strId, u8 strParam1, u8 strParam2, u8 strParam3 )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;

  ARIIZUMI_PRINT("[%d->%d] waza[%d] str[%d(%d:%d:%d)\n",pokePos,targetPos,wazaNo,strId,strParam1,strParam2,strParam3);

  if( IsRecordSkipMode() )
  {
    return;
  }

  BtlvPos vpos = BtlPosToViewPos(pokePos);

  SetComTargetReset();
  mComTargetPoke[0] = vpos;
  //GFNMCat[5315] 前半と後半の演出で攻防反転が入れ替わってしまうので、開始演出にもターゲットの指定。
  //  攻撃技に限定するのは、変化技の場合ターゲット指定が複雑、かつ自分の場合もあるので対処が複雑化する。
  //　また、変化技の場合は一旦待機カメラに戻るので違和感になりにくいため。
  if( pml::wazadata::GetDamageType(wazaNo) == pml::wazadata::DMG_PHYSIC ||
    pml::wazadata::GetDamageType(wazaNo) == pml::wazadata::DMG_SPECIAL )
  {
    //攻防同じだと自分が消える可能性がある。一緒の場合反転に影響は無いので念のためはじいておく
    if( BtlPosToViewPos(targetPos) != mComTargetPoke[0] )
    {
      mComTargetPoke[1] = BtlPosToViewPos(targetPos);
      mComTargetDamage[0] = BtlPosToViewPos(targetPos);
    }
  }

  mTargetTrainer[0] = GetBtlvPosToTrainerIdx(vpos);


  mZenryokuParam.isZenryoku = true;
  mZenryokuParam.aulaMsgId = strId;
  mZenryokuParam.aulaMsgParam[0] = strParam1;
  mZenryokuParam.aulaMsgParam[1] = strParam2;
  mZenryokuParam.aulaMsgParam[2] = strParam3;
  mZenryokuParam.wazaType = pml::wazadata::GetType(wazaNo);
  mZenryokuParam.targetPoke = vpos;

  pml::PokeType type = pml::wazadata::GetType(wazaNo);
  u32 startSeq = GARC_wazaeff_seqfile_ez_cmn_01_BSEQ+type;

  const Savedata::MyStatus* myStatus = GetClientPlayerData( SeqComFunc_GetTargetChara(mTargetTrainer[0])->GetClientID() );
#if PM_DEBUG
  BTLV_DEBUG_PARAM *debParam = GetDebugParam();
  if( debParam && debParam->mTrainerType[mTargetTrainer[0]] == -2 )
  {
    myStatus = GetClientPlayerData( SeqComFunc_GetTargetChara(0)->GetClientID() );
  }
#endif
  if(myStatus)
  {
    //プレイヤー用
    if (IsWazaEffectEnable() == false)
    {
      //エフェクトスキップの場合は短縮版
      startSeq = GARC_wazaeff_seqfile_ez_cmn_short_BSEQ;
    }
    else
    if( wazaNo >= WAZANO_URUTORADASSYUATAKKU && wazaNo <= WAZANO_FEARIIZENRYOKU )
    {
      startSeq = GARC_wazaeff_seqfile_ez_cmn_01_BSEQ+type;
    }
    else
    {
      //固有技
      for (int i = 0; i < GFL_NELEMS(ZenryokuDataTable); i++)
      {
        if (ZenryokuDataTable[i].wazaNo == wazaNo &&
          (ZenryokuDataTable[i].form == -1 ||
          ZenryokuDataTable[i].form == GetBattlePokeParam(vpos)->GetFormNo()))
        {
          {
            mZenryokuParam.isUnique = true;
            startSeq = ZenryokuDataTable[i].startSeq;
          }
        }
      }
    }
  }
  else
  {
    //NPC用
    startSeq = GARC_wazaeff_seqfile_ez_cmn_short_BSEQ;
  }
  SeqComFunc_PlaySequenceFile( startSeq );

  GetUiSys()->HideGaugeAll();
  GetUiSys()->HideBallBarAll();

  GetBattleSound()->BgmVolumeDown_ZWaza();
  mComScene = 0;
}

/**
 * @brief オーラ技の開始演出の完了を待つ
 * @retval true   演出が完了した
 * @retval false  演出中
 */
bool BattleViewSystem::CMD_ACT_ZenryokuWazaBegin_Wait( void )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);
  if( IsRecordSkipMode() )
  {
    return true;
  }

  if( SeqComFunc_IsFinishSequenceFile(true) )
  {
    //演出内で回復するが、ゲージ処理が無いので強制更新をかけておく
    GetUiSys()->GaugeUpdateForce();

    mReserveResetZenryokuTrainerTarget = mTargetTrainer[0];
    mReserveResetZenryokuTrainer = true;
    return true;
  }
  return false;
}

/**
 * @brief オーラ技の終了演出を開始する
 * @param pokePos  オーラ技を使用するポケモンの位置
 */
void BattleViewSystem::CMD_ACT_ZenryokuWazaEnd_Start( BtlPokePos pokePos )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;
  if( IsRecordSkipMode() )
  {
    return;
  }
  //@attention この処理はCMD_ACT_ZenryokuWazaBegin_Startと必ずついになっていない。
  //具体的にはGFNMCat#5566 のように、ひるみ、こんらん、メロメロで行動キャンセルになったときにEndのほうだけが呼ばれる。
  if( mZenryokuParam.isZenryoku == false )
  {
    //何もしないで抜ける
    mComScene = 255;
    return;
  }

  BtlvPos vpos;

  if( pokePos == BTL_POS_NULL )
  {
    //nijiでBtlPokePosからBtlvPosへの変換関数の挙動が変わり、BTL_POKE_NULLがBTL_VPOS_ERRORになるようになった。
    //全力技を打ったポケモンがレッドカードで退場すると発生する

    //開始時に取っておいたvposと同じ位置に出るように対処する
    vpos = mZenryokuParam.targetPoke;
  }
  else
  {
    vpos = BtlPosToViewPos(pokePos);
  }

  SetComTargetReset();
  mComTargetPoke[0] = vpos;
  mComTargetPoke[1] = vpos;
  mComTargetDamage[0] = vpos;

  mTargetTrainer[0] = GetBtlvPosToTrainerIdx(vpos);

  ResetZenryokuParam();

  if( GetPokeModel(vpos)->IsEnable() == false )
  {
    //死んでいる。
    mComScene = 255;
    ResetZenryokuTrainerMotion();
  }
  else
  {
    mReserveResetZenryokuTrainer = false; //シーケンスで対応される
     SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ez_cmn_failue_BSEQ );
    mComScene = 0;
  }
  GetBattleSound()->BgmVolumeReset_ZWaza();
}

/**
 * @brief オーラ技の開始女演出の完了を待つ
 * @retval true   演出が完了した
 * @retval false  演出中
 */
bool BattleViewSystem::CMD_ACT_ZenryokuWazaEnd_Wait( void )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);
  if( IsRecordSkipMode() )
  {
    return true;
  }

  if( mComScene == 255 )
  {
    //死んでいるかスキップ時
    return true;
  }

  enum
  {
    SEQ_WAIT_RESET = 0,

    SEQ_WAIT_MIGAWARI,
  };

  switch(mComScene)
  {
  case SEQ_WAIT_RESET:
    if( SeqComFunc_IsFinishSequenceFile() )
    {
      //本当は身代わりリセットを先にした方がきれいだが、カメラリセットは全力リセットに入っているので、こっちが後の方が良い
      if( mPokeModel[mComTargetPoke[0]]->IsHideMigawariZenryoku() )
      {
        mPokeModel[mComTargetPoke[0]]->SetHideMigawariZenryoku(false);
        SeqComFunc_PlaySequenceFileEffect( BTLEFF_MIGAWARI_WAZA_AFTER );
        mComScene++;
      }
      else
      {
        return true;
      }
    }
    break;
  case SEQ_WAIT_MIGAWARI:
    if( SeqComFunc_IsFinishSequenceFile() )
    {
      return true;
    }
    break;
  }

  return false;
}


void BattleViewSystem::CMD_ITEMSELECT_Start( u8 bagMode, u8 energy, u8 reserved_energy, u8 fFirstPokemon, u8 fBallTargetHide )
{
  DEBUG_NODRAW_CHECK;
  GFL_UNUSED( bagMode );
  GFL_UNUSED( energy );
  GFL_UNUSED( reserved_energy );
  
  ItemBallUseStatus ballUseMode = BBAG_BALLUSE_NORMAL;

  // 先頭のポケモン行動時しか投げられない
  if( !fFirstPokemon )
  {
    ballUseMode = BBAG_BALLUSE_NOT_FIRST;
  }
  // 手持ち・ボックスが満杯で投げられない
  else
  if( GetSetupStatusFlag( BTL_STATUS_FLAG_BOXFULL ) )
  {
    ballUseMode = BBAG_BALLUSE_POKEMAX;
  }
  // 野生戦の場合…
  else
  if( GetBattleCompetitor() == BTL_COMPETITOR_WILD )
  {
    // ぬし相手には投げられない
    if( GetSetupStatusFlag( BTL_STATUS_FLAG_VS_NUSI ) )
    {
      ballUseMode = BBAG_BALLUSE_VS_NUSI;
    }
    // 試練中は投げられない
    else if( GetSetupStatusFlag( BTL_STATUS_FLAG_SIREN_BATTLE ) )
    {
      ballUseMode = BBAG_BALLUSE_SIREN;
    }
    // 保護区なので投げられない
    else if( GetSetupStatusFlag( BTL_STATUS_FLAG_IN_POKE_SANCTUARY ) )
    {
      ballUseMode = BBAG_BALLUSE_SANCTUARY;
    }
    // 捕獲不可戦闘なので投げられない
    else if (GetSetupStatusFlag(BTL_STATUS_FLAG_NO_CAPTURE))
    {
      ballUseMode = BBAG_BALLUSE_NO_CAPTURE;
    }
    else
    {
      // 場に２体以上いるなら投げられない
      POKECON*  pPokeCtrl  = GetBattleContainer();
      if( pPokeCtrl != NULL )
      {
        if( 2 <= pPokeCtrl->GetPartyData( BTL_CLIENT_ENEMY1 )->GetAliveMemberCount() )
        {
          ballUseMode = BBAG_BALLUSE_DOUBLE;
        }
        // 場のポケモンが（そらをとぶなどで）消えてるなら投げられない
        else
        if( fBallTargetHide )
        {
          ballUseMode = BBAG_BALLUSE_POKE_NONE;
        }
      }
    }
  }

  //「さしおさえ」で道具を使用できないポケモンを指定(ここで判定しないで、アイテム使用時の判定は使用処理内で行う)
/*
  bool dummyArr[6];
  {
    ::std::memset( dummyArr, 0, sizeof( dummyArr ) );
    const BTL_PARTY* party = GetBattleParty_Self();
    const u32 members = party->GetMemberCount();
    int i;

    for(i=0; i<members; ++i)
    {
      const BTL_POKEPARAM *bpp = party->GetMemberDataConst(i);
      dummyArr[ i ] = bpp->CheckSick( pml::wazadata::WAZASICK_SASIOSAE );
    }

    for( ; i<GFL_NELEMS(dummyArr); ++i)
    {
     dummyArr[ i ] = 0;  // 0=使用可 / 1=使用不可
    }
  }
*/
  mUiSys->StartAnime_Bag( ballUseMode, NULL /* dummyArr */ );
}
bool BattleViewSystem::CMD_ITEMSELECT_Wait(void)
{
  DEBUG_NODRAW_CHECK_PARAM(true);
  const BTLV_COMMAND_RESULT ret = mUiSys->WaitAnime_Bag();
  if( ret != BCR_SEL_NONE )
  {
    mUiSys->EndAnime_Bag();
  }
  else
  {
    return false;
  }

  return true;
}
void BattleViewSystem::CMD_ITEMSELECT_ForceQuit(void)
{
  DEBUG_NODRAW_CHECK;
  mUiSys->ForceQuit_Bag();
}

u16 BattleViewSystem::CMD_ITEMSELECT_GetItemID(void)
{
  DEBUG_NODRAW_CHECK_PARAM(0);
  GFL_ASSERT_STOP( mUiSys );
  return  mUiSys->GetResultDataBag().useItemId;
}
u8  BattleViewSystem::CMD_ITEMSELECT_GetCost(void)
{
  DEBUG_FUNC_CALL_TEMP;
  return 0;
}
u8  BattleViewSystem::CMD_ITEMSELECT_GetTargetIdx(void)
{
  DEBUG_NODRAW_CHECK_PARAM(0);
  GFL_ASSERT_STOP( mUiSys );
  return  mUiSys->GetResultDataBag().targetPokeIndex;
}
u8  BattleViewSystem::CMD_ITEMSELECT_GetWazaIdx(void)
{
  DEBUG_NODRAW_CHECK_PARAM(0);
  GFL_ASSERT_STOP( mUiSys );
  return  mUiSys->GetResultDataBag().targetWazaIndex;
}
void BattleViewSystem::CMD_ITEMSELECT_ReflectUsedItem(void)
{
  DEBUG_FUNC_CALL_TEMP;
  
}



void BattleViewSystem::CMD_YESNO_Start( bool b_cancel, YesNoMode yesno_mode )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;
  mUiSys->StartAnime_SelectYesNo( b_cancel, yesno_mode );

  GetUiSys()->DispBallBarAll();
  GetUiSys()->UpdateBallBarAll();  
  GetUiSys()->GaugeUpdateForce();
  GetUiSys()->DispGaugeAll();
}
bool BattleViewSystem::CMD_YESNO_Wait( BtlYesNo* result )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);
  const BTLV_COMMAND_RESULT ret = mUiSys->WaitAnime_SelectYesNo();

  if( ret != BCR_SEL_NONE )
  {
    switch( ret )
    {
    case BCR_SEL_YES:
      *result = BTL_YESNO_YES;
      break;

    case BCR_SEL_NO:
      *result = BTL_YESNO_NO;
      break;
    }
    return true;
  }
  return false;

}
void BattleViewSystem::CMD_YESNO_ForceQuit( void )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;
  mUiSys->ForceQuit_SelectYestNo();
}
void BattleViewSystem::CMD_YESNO_Delete(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;
  mUiSys->EndAnime_SelectYesNo();
}

void BattleViewSystem::CMD_WAZAWASURE_Start( u8 pos, WazaID waza )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;
  
  mUiSys->Start_WazaWasure( pos , waza );
}
/**
 * @param[out] selectedWazaIndex  選択した技のインデックス
 * @return 選択結果
 */
BattleViewSystem::WazaWasureResult BattleViewSystem::CMD_WAZAWASURE_Wait( u8* selectedWazaIndex )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(WAZAWASURE_RESULT_CANCEL);
  *selectedWazaIndex = 0;

  BTLV_COMMAND_RESULT ret = mUiSys->WaitAnime_WazaWasure();
  switch( ret )
  {
  case BCR_SEL_CANCEL:
    *selectedWazaIndex = pml::MAX_WAZA_NUM;
    return WAZAWASURE_RESULT_CANCEL;
    
  case BCR_SEL_IDX1:
  case BCR_SEL_IDX2:
  case BCR_SEL_IDX3:
  case BCR_SEL_IDX4:
    *selectedWazaIndex = ret - BCR_SEL_IDX1;
    return WAZAWASURE_RESULT_SELECT_OLD;

  case BCR_SEL_IDX5:
    *selectedWazaIndex = pml::MAX_WAZA_NUM;
    return WAZAWASURE_RESULT_SELECT_NEW;
  }

  return WAZAWASURE_RESULT_NONE;
}

void BattleViewSystem::CMD_StartSelectChangeOrEscape(void)
{
  DEBUG_FUNC_CALL_TEMP;
  
}
bool BattleViewSystem::CMD_WaitSelectChangeOrEscape( u8* fSelect )
{
  DEBUG_FUNC_CALL_TEMP;
  return true;
}

// レベルアップ情報処理
void BattleViewSystem::CMD_LvupWin_StartDisp( const BTL_POKEPARAM* bpp, const BTL_LEVELUP_INFO* lvupInfo )
{
  DEBUG_NODRAW_CHECK;
  GetUiSys()->Start_LevelUp( bpp, lvupInfo );
  PlaySe(SEQ_SE_SYS_NJ_002);
}
bool BattleViewSystem::CMD_LvupWin_WaitDisp(void)
{
  DEBUG_NODRAW_CHECK_PARAM(true);
  return (GetUiSys()->IsDispLevelUp());
}
void BattleViewSystem::CMD_LvupWin_StepFwd(void)
{
  DEBUG_NODRAW_CHECK;
  GetUiSys()->Step_LevelUp();
  PlaySe(SEQ_SE_SYS_NJ_003);
}
bool BattleViewSystem::CMD_LvupWin_WaitFwd(void)
{
  DEBUG_NODRAW_CHECK_PARAM(true);
  return (GetUiSys()->IsDispLevelUp());
}
void BattleViewSystem::CMD_LvupWin_StartHide(void)
{
  DEBUG_NODRAW_CHECK;
  GetUiSys()->End_LevelUp();
  GetUiSys()->GaugeUpdateReq(); // @fix NMCat[1905]
}
bool BattleViewSystem::CMD_LvupWin_WaitHide(void)
{
  /* CMD_LvupWin_StartHide()でパッと消す仕様なのでここでは常にtrue */
  DEBUG_FUNC_CALL_TEMP;
  return true;
}

// 録画再生時フェードアウト
void BattleViewSystem::CMD_RecPlayFadeOut_Start(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;
  mRec_IsStartSkip = true;
  gfl2::math::Vector4 sCol(0,0,0,0);
  gfl2::math::Vector4 eCol(0,0,0,255);
  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA ,&sCol,&eCol);
 
  //終了時にフェードアウトが2重でかかるのを防止するため
  mIsFinishFade = true;
}
bool BattleViewSystem::CMD_RecPlayFadeOut_Wait(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);
  return GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd(gfl2::Fade::DISP_UPPER);
}
void BattleViewSystem::CMD_RecPlayFadeIn_Start(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;
  
  mComCnt = 0;
  mComScene = 0;
  mComSceneSub = 0;
  
  mRec_IsStartSkip = false;
  mRec_IsSkip = false;

  //終了しなかったので終了時のフェードフラグのリセット
  mIsFinishFade = false;
}
bool BattleViewSystem::CMD_RecPlayFadeIn_Wait(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);
  enum
  {
    START_FADE,
    WAIT_FADE,
  };

  switch(mComScene)
  {
  case START_FADE:
    for( int i=0;i<POKE_MODEL_NUM;i++ )
    {
      GetUiSys()->ResetGaugeParam(static_cast<BtlvPos>(i));
    }
    GetUiSys()->UpdateBallBarAll();
    GetUiSys()->GaugeUpdateForce();
    GetUiSys()->DispGaugeAll();
    
    ResetDefaultCamera(0);

    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA );
    mComScene = WAIT_FADE;
    break;
  case WAIT_FADE:
    return GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd(gfl2::Fade::DISP_UPPER);
    //break;
  }
  return false;
}

//録画再生のターンの時にゲージを表示する仕組み
void BattleViewSystem::CMD_RecDispTurnGauge_Start(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;
  mComScene = 0;
  mComCnt = 0;

  GetUiSys()->DispBallBarAll();
  GetUiSys()->UpdateBallBarAll();  
  GetUiSys()->DispGaugeAll();

  if( IsRecordSkipMode() )
  {
    //録画スキップ時は即終了
    mComCnt = 45;
  }
}
bool BattleViewSystem::CMD_RecDispTurnGauge_Wait(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);
  mComCnt++;
  if( mComCnt > 45 )
  {
    GetUiSys()->HideBallBarAll();
    GetUiSys()->HideGaugeAll();
    return true;
  }
  return false;
}


// かわいがり（なでる）処理
void BattleViewSystem::CMD_Naderu_Start( BtlvPos vpos )
{
  DEBUG_FUNC_CALL_TEMP;
  
}
bool BattleViewSystem::CMD_Naderu_Wait( BtlvPos vpos )
{
  DEBUG_FUNC_CALL_TEMP;
  return true;
}

/**
 * @brief 「お手入れボタン」の表示を開始する
 *
 * @note 相手の最後のポケモンを倒した後、
 *       「○○は××の経験値を得た！」メッセージ表示の直前にコールされます。
 */
void BattleViewSystem::CMD_OteireButton_StartDisp( void )
{
  DEBUG_FUNC_CALL;
  DEBUG_NODRAW_CHECK;
  mUiSys->DispOteire();
}

/**
 * @brief お手入れ移行演出( 開始 )
 * @param vpos  お手入れ対象ポケモンの位置
 */
void BattleViewSystem::CMD_OteireExecuteEffect_Start( BtlvPos vpos )
{
  DEBUG_FUNC_CALL;
  DEBUG_NODRAW_CHECK;

  if( mIsPlayNushiWin == true )
  {
    //ヌシ勝利演出ですでに振り向いてしまっているので演出をしない
    return;
  }

  SetComTargetReset();
  mComTargetPoke[0] = vpos;
  mComScene = 0;

  //@ attention ここの演出で白フェードで終わる 
  SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee394_BSEQ );
  mIsFinishFade = true;
}

/**
 * @brief お手入れ移行演出( 終了待ち )
 * @retval true   演出の途中
 * @retval false  演出が終了した
 */
bool BattleViewSystem::CMD_OteireExecuteEffect_Wait( void )
{
  DEBUG_FUNC_CALL;
  DEBUG_NODRAW_CHECK_PARAM(true);

  if( mIsPlayNushiWin == true )
  {
    return true;
  }

  if( SeqComFunc_IsFinishSequenceFile(true) ) //次につなげるからリソース残す
  {
    return true;
  }
  return false;
}

/**
 * @brief ぬし戦勝利演出( 開始 )
 */
void BattleViewSystem::CMD_VsNusiWinEffect_Start( void )
{
  DEBUG_FUNC_CALL;
  DEBUG_NODRAW_CHECK;

  SetComTargetReset();
  mComTargetPoke[0] = BTL_VPOS_NEAR_1;
  mComScene = 0;

  SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee395_BSEQ );

  mIsPlayNushiWin = true; //お手入れ演出のブロックのため
}

/**
 * @brief ぬし戦勝利演出( 終了待ち )
 * @retval true   演出の途中
 * @retval false  演出が終了した
 */
bool BattleViewSystem::CMD_VsNusiWinEffect_Wait( void )
{
  DEBUG_FUNC_CALL;
  DEBUG_NODRAW_CHECK_PARAM(true);

  if( SeqComFunc_IsFinishSequenceFile(true) ) //次につなげるからリソース残す
  {
    return true;
  }
  return false;
}

/**
 * @brief 乱入( 助けを呼ぶ演出　開始 )
 * @param callPokePos  助けを呼ぶポケモンの位置
 */
void BattleViewSystem::CMD_Intrude_Call_Start( BtlPokePos callPokePos )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;

  SetComTargetReset();
  mComTargetPoke[0] = BtlPosToViewPos(callPokePos);
  mComScene = 0;

  // ぬし戦・ウルトラビースト戦なら、通常の乱入とは異なるシーケンスを再生する
  u32 seqIndex = ( GetSetupStatusFlag( BTL_STATUS_FLAG_VS_NUSI ) || GetSetupStatusFlag( BTL_STATUS_FLAG_VS_ULTRA_BEAST ) ) ? ( GARC_wazaeff_seqfile_ee310_BSEQ ) : ( GARC_wazaeff_seqfile_ee307_BSEQ );
  SeqComFunc_PlaySequenceFile( seqIndex );
}

/**
 * @brief 乱入( 助けを呼ぶ演出　終了待ち )
 * @retval true   演出が終了した
 * @retval false  演出中
 */
bool BattleViewSystem::CMD_Intrude_Call_Wait( void )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);
  if( SeqComFunc_IsFinishSequenceFile(true) ) //次につなげるからリソース残す
  {
    return true;
  }
  return false;
}

/**
 * @brief 乱入( 助けが来なかった場合の演出　開始 )
 * @param callPokePos  助けを呼んだポケモンの位置
 */
void BattleViewSystem::CMD_Intrude_NotAppeared_Start( BtlPokePos callPokePos )
{
  DEBUG_NODRAW_CHECK;
  SetComTargetReset();
  mComTargetPoke[0] = BtlPosToViewPos(callPokePos);
  mComScene = 0;

  SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee309_BSEQ );
}

/**
 * @brief 乱入( 助けが来なかった場合の演出　終了待ち )
 * @retval true   演出が終了した
 * @retval false  演出中
 */
bool BattleViewSystem::CMD_Intrude_NotAppeared_Wait( void )
{
  DEBUG_NODRAW_CHECK_PARAM(true);
  if( SeqComFunc_IsFinishSequenceFile() )
  {
    return true;
  }
  return false;}

/**
 * @brief 乱入( 乱入ポケモンの登場演出　開始 )
 * @param callPokePos     助けを呼んだポケモンの位置
 * @param intrudePokePos  乱入してきたポケモンの位置
 */
void BattleViewSystem::CMD_Intrude_Appear_Start( BtlPokePos callPokePos, BtlPokePos intrudePokePos )
{
  DEBUG_NODRAW_CHECK;
  SetComTargetReset();
  mComTargetPoke[0] = BtlPosToViewPos(intrudePokePos);
  //シーケンスデモ使っていないのでコメントアウト。
  //mComTargetPokeにレアエフェクトが出てしまうので有効かする時は注意
  //mComTargetPoke[1] = BtlPosToViewPos(callPokePos);
  mComScene = 0;
}

/**
 * @brief 乱入( 乱入ポケモンの登場演出　終了待ち )
 * @retval true   演出が終了した
 * @retval false  演出中
 */
bool BattleViewSystem::CMD_Intrude_Appear_Wait( void )
{
 DEBUG_NODRAW_CHECK_PARAM(true);
  enum
  {
    CREATE_START = 0,
    CREATE_WAIT,
    START_SEQ,
    WAIT_SEQ,
    START_ROTOM_SEQ,
    WAIT_ROTOM_SEQ,
    FINISH_DISP,
    FINISH,
  };

  BtlvObject_PokeModel *mdl = GetPokeModel(mComTargetPoke[0]);
  switch( mComScene )
  {
  case CREATE_START:
    mdl->StartLoad_Battle();
    mComScene++;
    //break; //through
  case CREATE_WAIT:
    if( mdl->IsFinishLoad() == false )
    {
      return false;
    }
    {
      SuspendFrameSkip();
      mdl->Create(GetPokeHeap(mComTargetPoke[0]),GetPokeGLAllocator(mComTargetPoke[0]),true);
      mdl->AddRenderPath(0);
      mdl->SetVisible(true);
      mdl->ChangeAnimation(PokeTool::MODEL_ANIME_BT_WAIT_A);
      mdl->SetAnimationIsLoop(true);

      gfl2::math::Vector3 pos;
      int deg;

      //位置の初期化
      {
        ResetPokemon(BTL_VPOS_FAR_1,0);
        ResetPokemon(BTL_VPOS_FAR_2,0);
      }


      mComScene++;
    }
    //break; //through
  case START_SEQ:
    SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ee308_BSEQ );
    mComScene++;
    //break; //through
  case WAIT_SEQ:
    if( SeqComFunc_IsFinishSequenceFile() == false )
    {
      return false;
    }
    {
      BTL_POKEPARAM *bpp = GetBattlePokeParam(mComTargetPoke[0]);
      if (IsEnableRotomSearch(bpp) == true)
      {
        RegistLookedMonsNo(bpp);
        mComScene++;
      }
      else
      {
        mComScene = FINISH_DISP;
      }
    }
    break;
  case START_ROTOM_SEQ:
    //そのままTargetを使う
    SeqComFunc_PlaySequenceFile(GARC_wazaeff_seqfile_er001_BSEQ);
    mComScene++;
    //break; //through
  case WAIT_ROTOM_SEQ:
    if (SeqComFunc_IsFinishSequenceFile() == false)
    {
      return false;
    }
    mComScene++;
    //break; //through
  case FINISH_DISP:
    //最後の表示処理
    GetUiSys()->AllInGauge();
    GetUiSys()->ResetGaugeParam( mComTargetPoke[0] );
    GetUiSys()->DispGauge( mComTargetPoke[0] );
    ResetDefaultCamera(0);
    mComScene++;
    //break; //through

  case FINISH:
    return true;
  }

  return false;
}


//=============================================================================================
//  時間制限によるアクション選択の強制終了対応
//=============================================================================================
void BattleViewSystem::CMD_MsgWinHide_Start(void)
{
  //DEBUG_FUNC_CALL_TEMP;
  
}
bool BattleViewSystem::CMD_MsgWinHide_Wait(void)
{
  //DEBUG_FUNC_CALL_TEMP;
  return true;
}

//=============================================================================================
//  時間制限によるアクション選択の強制終了対応
//=============================================================================================
void BattleViewSystem::CMD_ForceQuitInput_Notify(void)
{
  DEBUG_FUNC_CALL_TEMP;
  
}
bool BattleViewSystem::CMD_ForceQuitInput_Wait(void)
{
  DEBUG_FUNC_CALL_TEMP;
  return true;
}

//=============================================================================================
//  録画再生モード
//=============================================================================================
void BattleViewSystem::CMD_RecPlayer_Init(u16 max_chapter)
{
  DEBUG_NODRAW_CHECK;
  app::BattleUIRecPlayLowerView*  pRecPlayLowerView = mUiSys->GetUIRecPlayLowerView();
  //  最大チャプター数は設定
  pRecPlayLowerView->SetChapterMax( max_chapter );
}

int  BattleViewSystem::CMD_CheckRecPlayerInput(void)
{
  DEBUG_NODRAW_CHECK_PARAM(-1);
  app::BattleUIRecPlayLowerView*  pRecPlayLowerView = mUiSys->GetUIRecPlayLowerView();
  pRecPlayLowerView->SetEnableSkipMode( false );

  //  再生Viewの入力を取得
  app::BattleUIRecPlayLowerView::ButtonId pushButtonId  = pRecPlayLowerView->GetPushButtonId();
  if( pushButtonId == app::BattleUIRecPlayLowerView::BUTTON_ID_NONE )
  {
    return  -1;
  }

  if( pushButtonId == app::BattleUIRecPlayLowerView::BUTTON_ID_CANCEL )
  {
    //  再生終了
    pRecPlayLowerView->SetEnableQuitMode();

    return  BTLV_INPUT_BR_SEL_STOP;
  }
  else
  if( pushButtonId == app::BattleUIRecPlayLowerView::BUTTON_ID_ADVANCE )
  {
    //  進める
    pRecPlayLowerView->ClearPushButtonId();
    // @fix NMCat[1057] チャプターが変わったので現在チャプターの色を変更
    pRecPlayLowerView->SetChapterNumSkipColor( true );
    return  BTLV_INPUT_BR_SEL_FF;
  }
  else
  if( pushButtonId == app::BattleUIRecPlayLowerView::BUTTON_ID_RETURN )
  {
    //  戻す
    pRecPlayLowerView->ClearPushButtonId();
    // @fix NMCat[1057] チャプターが変わったので現在チャプターの色を変更
    pRecPlayLowerView->SetChapterNumSkipColor( true );
    return  BTLV_INPUT_BR_SEL_REW;
  }

  return  -1;

}
void BattleViewSystem::CMD_UpdateRecPlayerInput( u16 currentChapter, u16 ctrlChapter )
{
  DEBUG_NODRAW_CHECK;
  app::BattleUIRecPlayLowerView*  pRecPlayLowerView = mUiSys->GetUIRecPlayLowerView();
  if( pRecPlayLowerView->IsOpen() == false )
  {
    //  開いていないならviewを開く
    mUiSys->SetEnableBGFilter( false );
    pRecPlayLowerView->Open();
  }

  //  現在のチャプター状態を設定して更新
  pRecPlayLowerView->SetChapterNum( ctrlChapter );
}
void BattleViewSystem::CMD_RecPlayer_StartSkip( void )
{
  DEBUG_NODRAW_CHECK;
  mRec_IsSkip = true;
}
void BattleViewSystem::CMD_RecPlayer_StartSkipDisplay( u16 nextChapter )
{
  DEBUG_NODRAW_CHECK;
  //  スキップ開始
  app::BattleUIRecPlayLowerView*  pRecPlayLowerView = mUiSys->GetUIRecPlayLowerView();
  pRecPlayLowerView->SetEnableSkipMode( true );
  pRecPlayLowerView->SetChapterNumSkipColor( false );
}
void BattleViewSystem::CMD_RecPlayer_StartQuit( u16 chapter, BTLV_INPUT_BR_STOP_FLAG stop_flag )
{
  DEBUG_FUNC_CALL_TEMP;
  
}

//=============================================================================================
//  XY追加コマンド
//=============================================================================================
void BattleViewSystem::CMD_TrainerIn_Win( BtlvTrainerPos position )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;
  SetComTargetReset();
  mTargetTrainer[0] = position;

  u16 seqFile = GARC_wazaeff_seqfile_ee352_BSEQ;

  SeqComFunc_PlaySequenceFile( seqFile );
  GetUiSys()->HideGaugeAll();
  GetUiSys()->HideBallBarAll();
  
}
void BattleViewSystem::CMD_TrainerIn_Win_AZ( BtlvTrainerPos position )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;
  
} //AZ特殊勝利
void BattleViewSystem::CMD_TrainerIn_Lose( BtlvTrainerPos position )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;
  
  SetComTargetReset();
  mTargetTrainer[0] = position;

  u16 seqFile = GARC_wazaeff_seqfile_ee351_BSEQ;
  TrainerExtraInfoStruct *data = GetTrainerModelObj(position)->GetTrainerData();
  if( data )
  {
    seqFile = data->mDefeatSeq;
  }

  //例外分岐(データには見た目ごとにしか敗北シーケンスが設定できないので、プログラムで分岐
  u16 type = GetTrainerModelObj(position)->GetTrainerType();
  switch(type)
  {
  case trainer::TRTYPE_FRIEND3:  //防衛線ハウ
    seqFile = GARC_wazaeff_seqfile_ee516_BSEQ;
    break;
  case trainer::TRTYPE_DOCTOR2:  //防衛線博士
    seqFile = GARC_wazaeff_seqfile_ee519_BSEQ;
    break;
  case trainer::TRTYPE_ROCKBIG4:  //四天王ハラ
  case trainer::TRTYPE_COMBATBIG4:  //四天王マイレ
    seqFile = GARC_wazaeff_seqfile_ee351_BSEQ;
    break;

    //momiji追加
  case trainer::TRTYPE_FRIEND4: //チャンピオンハウ
    seqFile = GARC_wazaeff_seqfile_ee548_BSEQ;
    break;
  case trainer::TRTYPE_SAKAKIRR: //RRサカキ
    seqFile = GARC_wazaeff_seqfile_ee549_BSEQ;
    break;
  case trainer::TRTYPE_EVIL: //クチナシ(しまキング　※フェアリー試練で汎用が増えたので重要を逃がす
    seqFile = GARC_wazaeff_seqfile_ee546_BSEQ;
    break;
    // @fix GFMMcat[174]バトルツリーのククイがバトル中Zリングを持ってない
  case trainer::TRTYPE_DOCTOR:  //ククイ(タワー用)
    seqFile = GARC_wazaeff_seqfile_ee351_BSEQ;  //タワーは汎用
    break;
  }

  SeqComFunc_PlaySequenceFile( seqFile );
  GetUiSys()->HideGaugeAll();
  GetUiSys()->HideBallBarAll();
}

//フェス用勝利演出
void BattleViewSystem::CMD_TrainerIn_WinFes(BtlvTrainerPos position)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;
  SetComTargetReset();
  mTargetTrainer[0] = position;

  u16 seqFile = GARC_wazaeff_seqfile_ee398_BSEQ;

  SeqComFunc_PlaySequenceFile(seqFile);
  GetUiSys()->HideGaugeAll();
  GetUiSys()->HideBallBarAll();
}

//重要キャラの対戦中台詞
void BattleViewSystem::CMD_TrainerIn_Event( BtlvTrainerPos position )
{
  DEBUG_FUNC_CALL_TEMP;
  
}
bool BattleViewSystem::CMD_WaitTrainerIn()
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);
  if( SeqComFunc_IsFinishSequenceFile(true) )
  {
    return true;
  }
  return false;
}

//BTLV_EFFECTから移植
void  BattleViewSystem::CMD_EFFECT_SetGaugeStatus( PokeSick sick,  BtlvPos pos )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;

  GetUiSys()->GaugeUpdateReq();
  GetUiSys()->UpdateBallBarAll();  

  BtlvObject_PokeModel *poke = GetPokeModel(pos);
  poke->SetSick( sick );

}
bool  BattleViewSystem::CMD_EFFECT_CheckExecuteGauge( void )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);

  return GetUiSys()->IsAnimeGauge(); 
}
void  BattleViewSystem::CMD_EFFECT_CalcGaugeEXP( int position, int value )
{
  GFL_UNUSED(position);
  GFL_UNUSED(value);

  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;

  //SE停止タイミングの兼ね合いでgauge_statusで制御
  //PlaySe( SEQ_SE_BT_EXP );
  GetUiSys()->DispGauge( (BtlvPos)position );
  GetUiSys()->GaugeExpAnime( (BtlvPos)position , value);
  
}
void  BattleViewSystem::CMD_EFFECT_CalcGaugeEXPLevelUp( int position, const BTL_POKEPARAM* bpp )
{
  GFL_UNUSED(position);
  GFL_UNUSED(bpp);

  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;

  //SE停止タイミングの兼ね合いでgauge_statusで制御
  //PlaySe( SEQ_SE_BT_EXP );
  GetUiSys()->DispGauge( (BtlvPos)position );
  GetUiSys()->GaugeExpAnime_Lvup( (BtlvPos)position );
  
}
bool  BattleViewSystem::CMD_EFFECT_CheckExistGauge( BtlvPos pos )
{
  DEBUG_FUNC_CALL_TEMP;
  return true;
}

//=============================================================================================
/**
 * @brief 捕獲時ボール投げエフェクト起動
 *
 * @param   vpos        対象ポケモンの描画位置
 * @param   itemNum     投げたボールのアイテムナンバー
 * @param   yure_cnt    ボール揺れ回数（0〜3）
 * @param   f_success   捕獲成功フラグ
 * @param   f_critical  クリティカルフラグ
 */
//=============================================================================================
void  BattleViewSystem::CMD_EFFECT_BallThrow( int position, u16 item_no, u8 yure_cnt, bool f_success, bool f_critical )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;

  SetComTargetReset();
  GetUiSys()->HideGaugeAll();

  //positionはVPOS！
  //エディタでは防御側に捕獲キャラが入っている！
  mComTargetPoke[0] = BTL_VPOS_NEAR_1;
  mComTargetPoke[1] = static_cast<BtlvPos>( position );
  mComTargetDamage[0] = static_cast<BtlvPos>( position );
  mTargetTrainer[0] = BTL_VPOS_NEAR_1;
  mEffectBallId[0] = item::ITEM_GetBallID(item_no);
  if( mEffectBallId[0] == item::BALLID_NULL )
  {
    mEffectBallId[0] = item::BALLID_MONSUTAABOORU;
    GFL_ASSERT_MSG(0,"ItemId is error![%d]\n",item_no);
  }


  mComScene = 0;
  mComSceneSub = 0;

  mComItemId = item_no;
  mComRepeatNum = yure_cnt;
  mComIsSuccsess = f_success;
  mComIsCritical = f_critical;
  
  mComIsCritical = f_critical;
}
bool  BattleViewSystem::CMD_EFFECT_WaitBallThrow( void )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);

  enum
  {
    SEQ_START,
    SEQ_1,
    SEQ_2,
    SEQ_3,
    SEQ_SUCCESS,
    SEQ_FAIL,

    SEQ_SUCCESS_CRI,
    SEQ_FAIL_CRI,

    SEQ_WAIT_SUCCESS_END,
    SEQ_WAIT_FAIL_END
  };

  switch( mComScene )
  {
    //捕獲開始
  case SEQ_START:

    if( mComIsCritical )
    {
      SeqComFunc_PlaySequenceFileEffect( BTLEFF_ETC_CAPTURE_START_CRI );
      if( mComRepeatNum > 0 )
      {
        mComScene = SEQ_SUCCESS_CRI;
      }
      else
      {
        mComScene = SEQ_FAIL_CRI;
      }
    }
    else
    {
      SeqComFunc_PlaySequenceFileEffect( BTLEFF_ETC_CAPTURE_START );
      if( mComRepeatNum > 0 )
      {
        mComScene = SEQ_1;
      }
      else
      {
        mComScene = SEQ_FAIL;
      }
    }

    break;

  case SEQ_1:
    if( SeqComFunc_IsFinishSequenceFile(true) )
    {
      SeqComFunc_PlaySequenceFileEffect( BTLEFF_ETC_CAPTURE_SEQ1 );
      if( mComRepeatNum > 1 )
      {
        mComScene = SEQ_2;
      }
      else
      {
        mComScene = SEQ_FAIL;
      }
    }
    break;
  case SEQ_2:
    if( SeqComFunc_IsFinishSequenceFile(true) )
    {
      SeqComFunc_PlaySequenceFileEffect( BTLEFF_ETC_CAPTURE_SEQ2 );

      if( mComRepeatNum > 2 )
      {
        mComScene = SEQ_3;
      }
      else
      {
        mComScene = SEQ_FAIL;
      }
    }
    break;
  case SEQ_3:
    if( SeqComFunc_IsFinishSequenceFile(true) )
    {
      SeqComFunc_PlaySequenceFileEffect( BTLEFF_ETC_CAPTURE_SEQ3 );

      if( mComIsSuccsess )
      {
        mComScene = SEQ_SUCCESS;
      }
      else
      {
        mComScene = SEQ_FAIL;
      }
    }
    break;
  case SEQ_SUCCESS:
    if( SeqComFunc_IsFinishSequenceFile(true) )
    {
      SeqComFunc_PlaySequenceFileEffect( BTLEFF_ETC_CAPTURE_SUCCESS );

      //BTS6816 捕獲後に敵側のゲージを出さないためのフラグ
      //mIsAfterCapture = true;

      mComScene = SEQ_WAIT_SUCCESS_END;
    }
    break;

  case SEQ_FAIL:
    if( SeqComFunc_IsFinishSequenceFile(true) )
    {
      SeqComFunc_PlaySequenceFileEffect( BTLEFF_ETC_CAPTURE_FAIL_1 + mComRepeatNum );

      mComScene = SEQ_WAIT_FAIL_END;
    }
    break;

  case SEQ_SUCCESS_CRI:
    if( SeqComFunc_IsFinishSequenceFile(true) )
    {
      SeqComFunc_PlaySequenceFileEffect( BTLEFF_ETC_CAPTURE_SUCCESS_CRI );

      //BTS6816 捕獲後に敵側のゲージを出さないためのフラグ
      //mIsAfterCapture = true;

      mComScene = SEQ_WAIT_SUCCESS_END;
    }
    break;

  case SEQ_FAIL_CRI:
    if( SeqComFunc_IsFinishSequenceFile(true) )
    {
      SeqComFunc_PlaySequenceFileEffect( BTLEFF_ETC_CAPTURE_FAIL_CRI );

      mComScene = SEQ_WAIT_FAIL_END;
    }
    break;

  case SEQ_WAIT_SUCCESS_END:
    //成功は開放しない
    if( SeqComFunc_IsFinishSequenceFile(true) )
    {
      mReserveClearResource = true;
      return true;
    }
    break;
  case SEQ_WAIT_FAIL_END:
    //失敗は開放する
    if( SeqComFunc_IsFinishSequenceFile() )
    {
      gfl2::math::Vector3 pos;
      int deg;

      ResetPokemon(mComTargetPoke[1],0);
      GetPokeModel(mComTargetPoke[1])->SetVisible(true);

      ResetDefaultCamera(0);
      return true;
    }
    break;
  }
  return false;
}


void  BattleViewSystem::CMD_EFFECT_BallThrowTrainer( int vpos, u16 item_no )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;
  
  GetUiSys()->HideGaugeAll();
  SetComTargetReset();
  //エディタでは防御側に捕獲キャラが入っている！
  mComTargetPoke[0] = BTL_VPOS_NEAR_1;
  mComTargetPoke[1] = static_cast<BtlvPos>( vpos );
  mComTargetDamage[0] = static_cast<BtlvPos>( vpos );
  mTargetTrainer[0] = BTL_VPOS_NEAR_1;
  mTargetTrainer[1] = GetBtlvPosToTrainerIdx(static_cast<BtlvPos>(vpos));
  mEffectBallId[0] = item::ITEM_GetBallID(item_no);
  if( mEffectBallId[0] == item::BALLID_NULL )
  {
    mEffectBallId[0] = item::BALLID_MONSUTAABOORU;
    GFL_ASSERT_MSG(0,"ItemId is error![%d]\n",item_no);
  }
  mComItemId = item_no;
  SeqComFunc_PlaySequenceFileEffect( BTLEFF_ETC_CAPTURE_FAIL_TRAINER );
}
bool  BattleViewSystem::CMD_EFFECT_WaitBallThrowTrainer( void )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);
  if( SeqComFunc_IsFinishSequenceFile() )
  {
    return true;
  }
  return false;
}

// カメラリセット処理(捕獲後
void BattleViewSystem::CMD_ResetCamera(void)
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;
  mComScene = 0;
}
bool BattleViewSystem::CMD_ResetCamera_Wait( void )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);
  if( mReserveClearResource )
  {
    //捕獲時にカメラリセットした時にボールを消す処理
    switch( mComScene )
    {
    case 0:
      {
        ResetDefaultCamera(0);
        for( int i=0;i<BTLV_EFF_MODEL_NUM;i++ )
        {
          int objNum;
          BtlvObject_Gfmdl **objArr = GetModelObjManager()->GetObjectArrIdx(i,&objNum);
          if( objArr )
          {
            for( int ii=0;ii<objNum;ii++)
            {
              BtlvObject_Gfmdl *obj = objArr[ii];
              obj->SetVisible(false);
              DemoLib::Obj::CreateTask_DeleteObject_GfMdl(GetTaskSystem(),GetTempHeap(),obj,0);
            }
          }
        }
      }
      mComScene++;
      break;
    case 1:
      if( GetTaskSystem()->GetActiveTaskNum() == 0 )
      {
        mReserveClearResource = false;
        SeqComFunc_CrealWazaResource();
        return true;
      }
    }
  }
  else
  {
    ResetDefaultCamera(0);
    return true;
  }
  return false;
}

//BTLV_EFFECTから移植(タイマー
//============================================================================================
/**
 * @brief  時間制限タイマークリエイト
 *
 * @param[in] game_time     ゲーム時間（秒）
 * @param[in] client_time   クライアント持ち時間（秒）
 * @param[in] command_time  コマンド時間（秒）
 */
//============================================================================================
void  BattleViewSystem::CMD_EFFECT_CreateTimer( int game_time, int client_time, int command_time )
{
  DEBUG_NODRAW_CHECK;
  // DEBUG_FUNC_CALL_TEMP;
  GetUiSys()->SetTimerTime( game_time, client_time, command_time );
}

//============================================================================================
/**
 *  @brief  タイマー描画許可
 *
 *  @param[in]  type              タイマータイプ
 *  @param[in]  enable            true:描画　false:非描画
 *  @param[in]  init              true:カウンタを初期化して描画　false:初期化せず描画（enableがtrueのときにしか意味がありません）
 *  @param[in]  isCountDownStart  true:カウントダウンを開始する
 */
//============================================================================================
void BattleViewSystem::CMD_EFFECT_DrawEnableTimer( BTLV_TIMER_TYPE type, bool enable, bool init, bool isCountDownStart )
{
  DEBUG_NODRAW_CHECK;
  //  DEBUG_FUNC_CALL_TEMP;

  if( enable )
  {
    GetUiSys()->DispTimer( type , init, isCountDownStart );
  }
  else
  {
    GetUiSys()->HideTimer( type );
  }
 
}

//============================================================================================
/**
 *  @brief  タイマーゼロチェック
 *
 *  @param[in]  type    タイマータイプ
 *
 */
//============================================================================================
bool  BattleViewSystem::CMD_EFFECT_IsZero( BTLV_TIMER_TYPE type )
{
  DEBUG_NODRAW_CHECK_PARAM(true);
  //DEBUG_FUNC_CALL_TEMP; //これは表示するとうるさいのでOFF
  return GetUiSys()->IsFinishTimer( type );
}

/**
 * @brief 指定クライアントの残り手持ち時間を取得する
 * @param clientID  設定対象クライアントのID
 * @return 残り手持ち時間[s]
 */
u32 BattleViewSystem::CMD_EFFECT_GetClientTime( u8 clientID )
{
  DEBUG_NODRAW_CHECK_PARAM(0);
  return GetUiSys()->GetClientTime( clientID );
}

/**
 * @brief 指定クライアントの残り手持ち時間を設定する
 * @param clientID  設定対象クライアントのID
 * @param time      残り手持ち時間[s]
 */
void BattleViewSystem::CMD_EFFECT_SetClientTime( u8 clientID, u32 time )
{
  DEBUG_NODRAW_CHECK;
  GetUiSys()->SetClientTime( clientID, time );
}

// メガ進化
static u32 MEGA_EVO_SEQ_ARR[] =
{
  GARC_wazaeff_seqfile_em094_0_BSEQ,  //MONSNO_GENGAA,    
  GARC_wazaeff_seqfile_em282_0_BSEQ,  //MONSNO_SAANAITO,  
  GARC_wazaeff_seqfile_em181_0_BSEQ,  //MONSNO_DENRYUU,   
  GARC_wazaeff_seqfile_em003_0_BSEQ,  //MONSNO_HUSIGIBANA,
  GARC_wazaeff_seqfile_em006_0_BSEQ,  //MONSNO_RIZAADON,  FORMNO_RIZAADON_MEGAX
  GARC_wazaeff_seqfile_em006_1_BSEQ,  //MONSNO_RIZAADON,  FORMNO_RIZAADON_MEGAY
  GARC_wazaeff_seqfile_em009_0_BSEQ,  //MONSNO_KAMEKKUSU, 
  GARC_wazaeff_seqfile_em150_0_BSEQ,  //MONSNO_MYUUTUU,   FORMNO_MYUUTUU_MEGAX
  GARC_wazaeff_seqfile_em150_1_BSEQ,  //MONSNO_MYUUTUU,   FORMNO_MYUUTUU_MEGAY
  GARC_wazaeff_seqfile_em257_0_BSEQ,  //MONSNO_BASYAAMO,  
  GARC_wazaeff_seqfile_em308_0_BSEQ,  //MONSNO_TYAAREMU,  
  GARC_wazaeff_seqfile_em229_0_BSEQ,  //MONSNO_HERUGAA,   
  GARC_wazaeff_seqfile_em306_0_BSEQ,  //MONSNO_BOSUGODORA,
  GARC_wazaeff_seqfile_em354_0_BSEQ,  //MONSNO_ZYUPETTA,  
  GARC_wazaeff_seqfile_em248_0_BSEQ,  //MONSNO_BANGIRASU, 
  GARC_wazaeff_seqfile_em212_0_BSEQ,  //MONSNO_HASSAMU,   
  GARC_wazaeff_seqfile_em127_0_BSEQ,  //MONSNO_KAIROSU,   
  GARC_wazaeff_seqfile_em142_0_BSEQ,  //MONSNO_PUTERA,    
  GARC_wazaeff_seqfile_em448_0_BSEQ,  //MONSNO_RUKARIO,   
  GARC_wazaeff_seqfile_em460_0_BSEQ,  //MONSNO_YUKINOOO,  
  GARC_wazaeff_seqfile_em115_0_BSEQ,  //MONSNO_GARUURA,   
  GARC_wazaeff_seqfile_em130_0_BSEQ,  //MONSNO_GYARADOSU, 
  GARC_wazaeff_seqfile_em359_0_BSEQ,  //MONSNO_ABUSORU,   
  GARC_wazaeff_seqfile_em065_0_BSEQ,  //MONSNO_HUUDHIN,   
  GARC_wazaeff_seqfile_em214_0_BSEQ,  //MONSNO_HERAKUROSU,
  GARC_wazaeff_seqfile_em303_0_BSEQ,  //MONSNO_KUTIITO,   
  GARC_wazaeff_seqfile_em310_0_BSEQ,  //MONSNO_RAIBORUTO, 
  GARC_wazaeff_seqfile_em445_0_BSEQ,  //MONSNO_GABURIASU, 
  GARC_wazaeff_seqfile_em380_0_BSEQ,  //MONSNO_RATHIASU,  
  GARC_wazaeff_seqfile_em381_0_BSEQ,  //MONSNO_RATHIOSU,  
  //sango追加
  GARC_wazaeff_seqfile_em260_0_BSEQ,  //MONSNO_RAGURAAZI 
  GARC_wazaeff_seqfile_em254_0_BSEQ,  //MONSNO_ZYUKAIN   
  GARC_wazaeff_seqfile_em302_0_BSEQ,  //MONSNO_YAMIRAMI  
  GARC_wazaeff_seqfile_em334_0_BSEQ,  //MONSNO_TIRUTARISU
  GARC_wazaeff_seqfile_em475_0_BSEQ,  //MONSNO_ERUREIDO  
  GARC_wazaeff_seqfile_em531_0_BSEQ,  //MONSNO_TABUNNE   
  GARC_wazaeff_seqfile_em376_0_BSEQ,  //MONSNO_METAGUROSU
  GARC_wazaeff_seqfile_em319_0_BSEQ,  //MONSNO_SAMEHADAA 
  GARC_wazaeff_seqfile_em080_0_BSEQ,  //MONSNO_YADORAN   
  GARC_wazaeff_seqfile_em208_0_BSEQ,  //MONSNO_HAGANEERU 
  GARC_wazaeff_seqfile_em018_0_BSEQ,  //MONSNO_PIZYOTTO  
  GARC_wazaeff_seqfile_em362_0_BSEQ,  //MONSNO_ONIGOORI  
  GARC_wazaeff_seqfile_em719_0_BSEQ,  //MONSNO_DHIANSII  
  GARC_wazaeff_seqfile_em323_0_BSEQ,  //MONSNO_BAKUUDA   
  GARC_wazaeff_seqfile_em428_0_BSEQ,  //MONSNO_MIMIROPPU 
  GARC_wazaeff_seqfile_em373_0_BSEQ,  //MONSNO_BOOMANDA  
  GARC_wazaeff_seqfile_em015_0_BSEQ,  //MONSNO_SUPIAA    
  GARC_wazaeff_seqfile_em384_0_BSEQ,  //MONSNO_REKKUUZA
};
void  BattleViewSystem::CMD_StartMegaEvo( BtlvPos vpos , bool isLong )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;

  SetComTargetReset();

  mComTargetPoke[0] = vpos;
  mComTargetPoke[1] = vpos;
  mComTargetDamage[0] = vpos;
  mTargetTrainer[0] = GetBtlvPosToTrainerIdx(vpos);
  mIsMigawariWaza = GetPokeModel(vpos)->IsVisibleMigawari();
  mComScene = 0;
  if( IsRecordSkipMode() )
  {
    mComScene = 100;
  }
  
}
bool  BattleViewSystem::CMD_WaitMegaEvo( BtlvPos vpos )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);

  enum
  {
    MIGAWARI_OUT_START,
    MIGAWARI_OUT_WAIT,
    
    START_MEGA1,
    WAIT_MEGA1,

    MIGAWARI_IN_START,
    MIGAWARI_IN_WAIT,

    FINISH_WAIT,
  };
  
  if( mComScene >= 100 )
  {
    //変身系とと共通化のため。
    return CMD_ACT_PokeChangeEffect_WaitCore_Skip();
  }
  else
  {
    switch( mComScene )
    {
    case MIGAWARI_OUT_START:
      if( mIsMigawariWaza )
      {
        SeqComFunc_PlaySequenceFileEffect( BTLEFF_MIGAWARI_WAZA_BEFORE );
        mComScene = MIGAWARI_OUT_WAIT;
      }
      else
      {
        GetBattleSound()->BgmVolumeDown_MegaEvo();
        mComScene = START_MEGA1;
      }
      break;
    case MIGAWARI_OUT_WAIT:
      if( SeqComFunc_IsFinishSequenceFile() )
      {
        GetBattleSound()->BgmVolumeDown_MegaEvo();
        mComScene = START_MEGA1;
      }
      break;

    case START_MEGA1:
      {
        const BTL_POKEPARAM *bpp = GetBattlePokeParam( mComTargetPoke[0] );
        if (bpp->GetMonsNo() == MONSNO_PURIZUMU)
        {
          //ヒカリドラゴン例外処理
          SeqComFunc_PlaySequenceFile(GARC_wazaeff_seqfile_ee034_BSEQ);
        }
        else
        {
          int idx = PokeTool::Mega::GetMegaIndex((MonsNo)bpp->GetMonsNo(), bpp->GetFormNo());
          if (idx < 0 ||
            idx >= GFL_NELEMS(MEGA_EVO_SEQ_ARR))
          {
            idx = 0;  //こないはずだけど一応サポート
          }
          SeqComFunc_PlaySequenceFile(MEGA_EVO_SEQ_ARR[idx]);
        }
        mComScene = WAIT_MEGA1;
      }
      break;

    case WAIT_MEGA1:
      if( SeqComFunc_IsFinishSequenceFile() )
      {
        GetPokeModel(mComTargetPoke[0])->SetVisible(true);
        GetBattleSound()->BgmVolumeReset_MegaEvo();
        GetUiSys()->GaugeUpdateReq(); // @fix NMCat[1971]
        if( mIsMigawariWaza )
        {
          mComScene = MIGAWARI_IN_START;
        }
        else
        {
          mComScene = FINISH_WAIT;
        }
      }
      break;
    case MIGAWARI_IN_START:
      SeqComFunc_PlaySequenceFileEffect( BTLEFF_MIGAWARI_WAZA_AFTER );
      mComScene = MIGAWARI_IN_WAIT;
      break;
    case MIGAWARI_IN_WAIT:
      if( SeqComFunc_IsFinishSequenceFile() )
      {
        mComScene = FINISH_WAIT;
      }
      break;

    case FINISH_WAIT:
      return true;
      //break;
    }
  }
  return false;
}

// かわいがり系振り向き処理
void BattleViewSystem::CMD_TurnPoke( BtlPokePos pos )
{
  DEBUG_FUNC_CALL_TEMP;
  
}
bool BattleViewSystem::CMD_TurnPoke_Wait( BtlPokePos pos )
{
  DEBUG_FUNC_CALL_TEMP;
  return true;
}

// 天候変更処理
void BattleViewSystem::CMD_ChangeWheather( BtlWeather weather )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;

  if( mNowWeather != weather )
  {
    if( IsRecordSkipMode() )
    {
      //バトルビデオスキップ時
      LoadWeatherResource(weather);
      mComScene = 200;
    }
    else
    {
      static const int effArr[BTL_WEATHER_MAX] =
      {
        BTLEFF_RESET_WEATHER,     //BTL_WEATHER_NONE = 0,   ///< 天候なし
        BTLEFF_WEATHER_HARE,      //BTL_WEATHER_SHINE,    ///< はれ
        BTLEFF_WEATHER_AME,       //BTL_WEATHER_RAIN,     ///< あめ
        BTLEFF_WEATHER_ARARE,     //BTL_WEATHER_SNOW,     ///< あられ
        BTLEFF_WEATHER_SUNAARASHI, //BTL_WEATHER_SAND,     ///< すなあらし
        //sango追加個所対応
        BTLEFF_WEATHER_STORM,       //BTL_WEATHER_RAIN,     ///< あめ
        BTLEFF_WEATHER_DAY,      //BTL_WEATHER_SHINE,    ///< はれ
        BTLEFF_WEATHER_TURBULENCE, //BTL_WEATHER_SAND,     ///< すなあらし
      };

      mComScene = 0;
      CMD_AddEffect( effArr[weather] );
    }
    mNowWeather = weather;
  }
  else
  {
    mComScene = 100;
  }
}
bool BattleViewSystem::CMD_ChangeWheather_Wait( void )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);

  switch(mComScene)
  {
  case 0:
    if( CMD_WaitEffect() )
    {
      return true;
    }
    break;
  case 100:
    //変化なしの場合
    return true;
  case 200:
    //スキップ時
    if( IsFinishLoadWeatherResource() )
    {
      StartWeather(mNowWeather);
      return true;
    }
  }
  return false;
}

// フィールド変更処理
void BattleViewSystem::CMD_ChangeGround( BtlGround type )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK;

  if( IsRecordSkipMode() )
  {
    StartLoadFieldWaza(type);
    mComScene = 100;
  }
  else
  {
    static const int effArr[BTL_GROUND_MAX] =
    {
      BTLEFF_RESET_FIELD_EFFECT, //BTL_GROUND_NONE = 0,   ///< なし
      BTLEFF_GRASS_FIELD,        //BTL_GROUND_GRASS,    ///< グラスフィールド
      BTLEFF_MIST_FIELD,         //BTL_GROUND_MIST,     ///< ミストフィールド
      BTLEFF_EREKI_FIELD,        //BTL_GROUND_ELEKI,    ///< エレキフィールド
      BTLEFF_SAIKO_FIELD,        //BTL_GROUND_PYSCO,    ///< サイコフィールド
      BTLEFF_RESET_FIELD_EFFECT, //BTL_GROUND_AQUA, //存在しない！
    };

    CMD_AddEffect(effArr[type]);
    mComScene = 0;

  }
  
}
bool BattleViewSystem::CMD_ChangeGround_Wait( void )
{
  DEBUG_FUNC_CALL_TEMP;
  DEBUG_NODRAW_CHECK_PARAM(true);
  switch(mComScene)
  {
  case 0:
    if( CMD_WaitEffect() )
    {
      return true;
    }
    break;
  case 100:
    //レコーダースキップ時(作成時
    if( IsFinishLoadFieldWaza() )
    {
      return true;
    }
    break;
  }
  return false;
}

//3DSさかさま判定関連
void BattleViewSystem::CMD_Start3DSReverseWatcher(bool is_short)
{
  DEBUG_NODRAW_CHECK;
  GFL_ASSERT( mTurnWatcher == NULL );
  mTurnWatcher = GFL_NEW( GetTempHeap() ) System::DeviceTurnWatcher( GetTempHeap(), mDeviceManager );

  if ( is_short ) {
    mTurnWatcher->Initialize( System::DeviceTurnWatcher::COUNT_RATE_30F, System::DeviceTurnWatcher::COUNT_MODE_SHORT );
  } else {
    mTurnWatcher->Initialize( System::DeviceTurnWatcher::COUNT_RATE_30F, System::DeviceTurnWatcher::COUNT_MODE_NORMAL );
  }
}
void BattleViewSystem::CMD_Stop3DSReverseWatcher(void)
{
  DEBUG_NODRAW_CHECK;
  GFL_DELETE mTurnWatcher;
  mTurnWatcher = NULL;
}
bool BattleViewSystem::CMD_Get3DSReverseWatcherResult(void)
{
  DEBUG_NODRAW_CHECK_PARAM(true);
  return mTurnWatcher->GetResult();
}

GFL_NAMESPACE_END(btl)


