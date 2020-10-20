//======================================================================
/**
 * @file  btlv_seq_sys.cpp
 * @brief バトル描画シーケンスシステム
 * @author  ariizumi
 * @data	15/4/22
 */
//======================================================================

#include <arc_def_index/arc_def.h>

#include "btlv_seq_com_define.h"

#include "btlv_local_def.h"
#include "btlv_seq_sys.h"
#include "btlv_ui.h"

#include "btlv_core.h"

#include "btlv_seq_com.h"

// モデルのインデックス
#include <niji_conv_header/battle/chara/chara_model_id.h>
#include <arc_index/wazaeff_resource.gaix>

namespace btl
{

typedef void(*BTLV_SEQ_FUNC)( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );


#define BTL_SEQ_DEF(name) BTLV_SEQCOM_ ## name ,

BTLV_SEQ_FUNC comFunc[BTLV_SEQCOM_MAX]=
{
#include "btlv_seq_com.cdat"
};

BtlvSeqSystem::BtlvSeqSystem(BattleViewSystem *core)
:mBtlvCore(core)
{
  gfl2::heap::HeapBase *heapMem = mBtlvCore->GetResidentHeap();
  
  DemoLib::SeqEditor::InitParam initParam =
  {
    heapMem,
    btlv_seq_com_define_size_table,
  };
  
  mSeqSys = GFL_NEW(heapMem)DemoLib::SeqEditor::SeqSystem(initParam);
  
  mIsPause = false;
}

BtlvSeqSystem::~BtlvSeqSystem()
{
  GFL_DELETE mSeqSys;
}

void BtlvSeqSystem::Update(void)
{
  if( mIsPause == false )
  {
    mSeqSys->UpdateFrame();
    if( mSeqFile && mSeqFile->IsFinish() )
    {
      mSeqSys->UnLoad(mSeqFile);
      mSeqFile = NULL;
    }
  }
}

void BtlvSeqSystem::LoadSeqFile( int arcId )
{
  ARIIZUMI_PRINT("BTLV LoadSeqFile[%d]\n",arcId);
  mSeqFile = mSeqSys->LoadFile( mBtlvCore->GetAsyncFileManager(), ARCID_WAZAEFF_SEQUENCE , arcId , this );
}

void BtlvSeqSystem::LoadQuickHit(void)
{

}

void BtlvSeqSystem::DebugLoadPath( char *path )
{
  mSeqFile = mSeqSys->LoadPcFile( mBtlvCore->GetAsyncFileManager(), path , this );
  mIsPause = true;
}

int BtlvSeqSystem::GetSeqFrame(void)
{
  if( mSeqFile )
  {
    return mSeqFile->GetFrame();
  }
  else
  {
    return 0;
  }
}

void BtlvSeqSystem::CommnadCallback( DemoLib::SeqEditor::SeqFile *seqFile , const DemoLib::SeqEditor::CommandParam &param )
{
  //デバッグ専用コマンドとして処理しない
  if( param.isDebug == true &&
      mBtlvCore->GetBtlvMode() != BTLV_MODE_EFFECT_VIEWER )
  {
    return;
  }

  if( mBtlvCore->CheckCanPlayCommand(param.groupOpt,param.commandNo) == false )
  {
    return;
  }

  if( param.isSkipMode )
  {
    switch( param.commandNo )
    {
      //攻撃モーション
    case SEQCOM_PokemonAttackMotion:
      CheckAttackMotionTimming(seqFile,NULL,param);
      break;
      //各種先読み
    case SEQCOM_PokemonMotionResource:
    case SEQCOM_TrainerChangeMotionResource:
      PreLoad_WazaOuterMotion(seqFile,NULL,param);
      break;
    case SEQCOM_PokemonMotionResourceTiki:
      PreLoad_WazaOuterMotionTiki(seqFile,NULL,param);
      break;
    case SEQCOM_PokemonRareEffect:
      PreLoad_RareEffect(seqFile,NULL,param);
      break;
    case SEQCOM_TrainerChangeMotionKisekae:
      PreLoad_WazaKisekaeMotion(seqFile,NULL,param);
      break;
    case SEQCOM_ModelCreate:
      PreLoad_WazaModel(seqFile,NULL,param);
      break;
    case SEQCOM_ModelCreateBall:
      PreLoad_BallModel(seqFile,NULL,param);
      break;
    case SEQCOM_ClusterCreate:
      PreLoad_WazaModel_Cluster(seqFile,NULL,param);
      break;
    case SEQCOM_ParticleCreate:
      PreLoad_WazaEffectBall(seqFile,NULL,param);
      break;
    case SEQCOM_ClusterCreateEffect:
      PreLoad_WazaEffect(seqFile,NULL,param);
      break;
    case SEQCOM_ModelSetAnimation:
      PreLoad_WazaModelAnime(seqFile,NULL,param);
      break;
    case SEQCOM_CameraAnimetionPoke:
    case SEQCOM_CameraAnimetionPosition:
      PreLoad_WazaCamera(seqFile,NULL,param);
      break;
    case SEQCOM_CameraAnimetionTrainer:
      PreLoad_WazaCameraTrainer(seqFile,NULL,param);
      break;
    case SEQCOM_CameraAnimetionDoubleIntro:
      PreLoad_WazaCameraDoubleIntro(seqFile,NULL,param);
      break;
    case SEQCOM_ModelReplaceTexture:
      PreLoad_WazaTexture(seqFile,NULL,param);
      break;
    case SEQCOM_ModelReplaceTextureTrainer:
      PreLoad_WazaTrainerTexture(seqFile,NULL,param);
      break;
    case SEQCOM_EffCharaLightEnv:
      PreLoad_WazaTrainerLight(seqFile,NULL,param);
      break;
    case SEQCOM_EffCharaLightFile:
      PreLoad_WazaTrainerLightFile(seqFile,NULL,param);
      break;
    case SEQCOM_SpecialChangeWeather:
      PreLoad_Weather(seqFile, NULL, param);
      break;
    case SEQCOM_RotomStartEvent:
      mBtlvCore->GetUiSys()->Rotom_StartLoad();
      break;

      //着地モーション(煙を読む
    case SEQCOM_PokemonIntroMotion:
      PreLoad_WazaEffectLnad(seqFile,NULL,param);
      break;
      
      //連続技定義のチェック(攻撃モーション再生用
    case SEQCOM_SpecialChainAttakDefine:
      CheckSpecialChainAttakDefine(seqFile,NULL,param);
      break;

    case SEQCOM_SpecialFesTrainerSetup:
    case SEQCOM_MessageDispSFes:
      //フェス用メッセージ読み込み
      mBtlvCore->GetResourceManager()->LoadRequestStep(mBtlvCore->GetTempHeap(), mBtlvCore->GetTempHeap()->GetLowerHandle(), print::GetMessageArcId(), GARC_message_jf_phrase_DAT, false, DemoLib::Obj::RESOURCE_NORMAL,true );
      break;
    default:
      //no func
      break;
    }
  }
  else
  {
    comFunc[param.commandNo]( seqFile , mBtlvCore , param );
  }
}

void BtlvSeqSystem::CheckWazaState(void)
{
  mBtlvCore->GetWazaParam()->mStartFrame = 0;
  mSeqFile->SkipFrame( -1 );
  mSeqFile->ResetSeq();
}

//CheckWazaStateのチェック用
void BtlvSeqSystem::CheckAttackMotionTimming( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int trg,motion;

  file->LoadDataInt( &trg );
  file->LoadDataInt( &motion );
  
  //攻撃形態指定モーション
  if( motion >= POKE_MOTION_ATK_BUTSURI )
  {
    BTLV_WAZA_EFF_PARAM *wazaParam = mBtlvCore->GetWazaParam();
    int keyFrame = param.startFrame;
    
    BtlvPos vPos = mBtlvCore->SeqComFunc_GetTargetPoke(trg);
    BtlvObject_PokeModel *mdl = mBtlvCore->GetPokeModel(vPos);
   
    int mot = PokeTool::MODEL_ANIME_BUTURI1;
    mot = mBtlvCore->SeqComFunc_GetWazaMotionType( mdl->GetMonsNo() , 
                                        mdl->GetFormNo() , 
                                        wazaParam->mId,
                                        motion-POKE_MOTION_ATK_BUTSURI );
    if( mot != 10 )
    {
      mot += PokeTool::MODEL_ANIME_BUTURI1;
    }
    else
    {
      mot = PokeTool::MODEL_ANIME_BT_WAIT_A;
    }
    
    int motKeyFrame = mdl->GetAttackKeyFrame( static_cast<PokeTool::MODEL_ANIME>(mot) );
    if( motKeyFrame == -1 )
    {
      ARIIZUMI_PRINT("KeyCheckError!!\n");
      motKeyFrame = 20;
    }

    wazaParam->mEnableKeyAdjust = true;
    wazaParam->mIsPlayMotion = false;
    wazaParam->mKeyDiffFrame = keyFrame - motKeyFrame - wazaParam->mStartFrame;
    wazaParam->mAttackMotion = mot;
    ARIIZUMI_PRINT("KeyAdjust[%d-%d] [%d]\n",keyFrame,motKeyFrame,wazaParam->mKeyDiffFrame);
  }
}

//連続技定義のチェック
void BtlvSeqSystem::CheckSpecialChainAttakDefine( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int startFrame[6];
  int endFrame[6];

  for( int i=0;i<6;i++ )
  {
    file->LoadDataInt( &startFrame[i] );
    file->LoadDataInt( &endFrame[i] );
  }

  BTLV_WAZA_EFF_PARAM* wazaParam = mBtlvCore->GetWazaParam();
  if( startFrame[wazaParam->mTurnType] != 0 )
  {
    wazaParam->mStartFrame = startFrame[wazaParam->mTurnType];
  }
}

void BtlvSeqSystem::PreLoad_WazaModel( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int arcIdx;
  file->LoadDataInt( &arcIdx );

  bool isUseGasc = false;
#if PM_DEBUG
  if( mBtlvCore->GetDebugParam() && mBtlvCore->GetDebugParam()->isHioEffResource )
  {
    isUseGasc = true;
  }
#endif

  arcIdx = mBtlvCore->CheckWazaDataId_Model(arcIdx,0);
  mBtlvCore->GetResourceManager()->LoadRequestStep(mBtlvCore->GetTempHeap(),mBtlvCore->GetTempHeap()->GetLowerHandle(),ARCID_WAZAEFF_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_NORMAL,isUseGasc);
}

void BtlvSeqSystem::PreLoad_BallModel( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int ballIdx;
  file->LoadDataInt( &ballIdx );

  //0はBALLID_NULLなので注意
  int ballType = mBtlvCore->SeqComFunc_GetTargetBall(ballIdx);
  mBtlvCore->GetResourceManager()->LoadRequestStep(
                              mBtlvCore->GetTempHeap(),
                              mBtlvCore->GetTempHeap()->GetLowerHandle(),
                              ARCID_CHARA_MODEL_BATTLE_NORMAL,
                              CHARA_MODEL_ID_BTOB0201_00+ballType-1,
                              false,
                              DemoLib::Obj::RESOURCE_BINPACK,
                              false);
}

//こっちはアニメも一緒に読む
void BtlvSeqSystem::PreLoad_WazaModel_Cluster( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int arcIdxMdl,arcIdxAnm;
  file->LoadDataInt( &arcIdxMdl );
  file->LoadDataInt( &arcIdxAnm );

  bool isUseGasc = false;
#if PM_DEBUG
  if( mBtlvCore->GetDebugParam() && mBtlvCore->GetDebugParam()->isHioEffResource )
  {
    isUseGasc = true;
  }
#endif

  arcIdxMdl = mBtlvCore->CheckWazaDataId_Model(arcIdxMdl,0);
  arcIdxAnm = mBtlvCore->CheckWazaDataId_Model(arcIdxAnm,0);
  mBtlvCore->GetResourceManager()->LoadRequestStep(mBtlvCore->GetTempHeap(),mBtlvCore->GetTempHeap()->GetLowerHandle(),ARCID_WAZAEFF_RESOURCE,arcIdxMdl,true,DemoLib::Obj::RESOURCE_NORMAL,isUseGasc);
  mBtlvCore->GetResourceManager()->LoadRequestStep(mBtlvCore->GetTempHeap(),mBtlvCore->GetTempHeap()->GetLowerHandle(),ARCID_WAZAEFF_RESOURCE,arcIdxAnm,true,DemoLib::Obj::RESOURCE_NORMAL,isUseGasc);

}

void BtlvSeqSystem::PreLoad_WazaModelAnime( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int arcIdx;
  file->LoadDataInt( &arcIdx );

  bool isUseGasc = false;
#if PM_DEBUG
  if( mBtlvCore->GetDebugParam() && mBtlvCore->GetDebugParam()->isHioEffResource )
  {
    isUseGasc = true;
  }
#endif

  arcIdx = mBtlvCore->CheckWazaDataId_Model(arcIdx,0);
  mBtlvCore->GetResourceManager()->LoadRequestStep(mBtlvCore->GetTempHeap(),mBtlvCore->GetTempHeap()->GetLowerHandle(),ARCID_WAZAEFF_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_NORMAL,isUseGasc);

}
void BtlvSeqSystem::PreLoad_WazaEffect( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int arcIdx;
  file->LoadDataInt( &arcIdx );

  bool isUseGasc = false;
#if PM_DEBUG
  if( mBtlvCore->GetDebugParam() && mBtlvCore->GetDebugParam()->isHioEffResource )
  {
    isUseGasc = true;
  }
#endif

  arcIdx = mBtlvCore->CheckWazaDataId_Effect(arcIdx,0);
  mBtlvCore->GetResourceManager()->LoadRequestStep(mBtlvCore->GetTempHeap(),mBtlvCore->GetTempHeap()->GetLowerHandle(),ARCID_WAZAEFF_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT,isUseGasc);
}
void BtlvSeqSystem::PreLoad_WazaEffectBall( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int arcIdx;
  int index;
  file->LoadDataInt( &arcIdx );
  file->LoadDataInt( &index );

  bool isUseGasc = false;
#if PM_DEBUG
  if( mBtlvCore->GetDebugParam() && mBtlvCore->GetDebugParam()->isHioEffResource )
  {
    isUseGasc = true;
  }
#endif

  arcIdx = mBtlvCore->CheckWazaDataId_Effect(arcIdx,index);
  mBtlvCore->GetResourceManager()->LoadRequestStep(mBtlvCore->GetTempHeap(),mBtlvCore->GetTempHeap()->GetLowerHandle(),ARCID_WAZAEFF_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT,isUseGasc);
}
void BtlvSeqSystem::PreLoad_WazaEffectLnad( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  bool isUseGasc = false;
#if PM_DEBUG
  if( mBtlvCore->GetDebugParam() && mBtlvCore->GetDebugParam()->isHioEffResource )
  {
    isUseGasc = true;
  }
#endif

  int trgPoke;
  file->LoadDataInt( &trgPoke );

  BtlvPos vPos = mBtlvCore->SeqComFunc_GetTargetPoke(trgPoke);
  BTLV_LAND_TYPE landType = mBtlvCore->CheckLandType(vPos);

  int arcIdx = -1;

  switch(landType)
  {
  case BTLV_LAND_HEAVY:        //200kg以上
    arcIdx = GARC_wazaeff_resource_ee007_land_smoke_m_lz_BPTCL;
    break;
  case BTLV_LAND_LIGHT_HEAVY:  //100kg以上
    arcIdx = GARC_wazaeff_resource_ee007_land_smoke_s_lz_BPTCL;
    break;
  case BTLV_LAND_MIDDLE:       //50kg以上
  case BTLV_LAND_LIGHT:        //50kg未満
  case BTLV_LAND_NONE:         //ふうせん持ちか着地モーションがない
    break;
  }
  if( arcIdx != -1 )
  {
    mBtlvCore->GetResourceManager()->LoadRequestStep(mBtlvCore->GetTempHeap(),mBtlvCore->GetTempHeap()->GetLowerHandle(),ARCID_WAZAEFF_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT,isUseGasc);
  }
}
//レアエフェクトコマンド先読み(レアが居なくても呼んじゃう
void BtlvSeqSystem::PreLoad_RareEffect( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{

  bool isUseGasc = false;
#if PM_DEBUG
  if( mBtlvCore->GetDebugParam() && mBtlvCore->GetDebugParam()->isHioEffResource )
  {
    isUseGasc = true;
  }
#endif
  mBtlvCore->GetResourceManager()->LoadRequestStep(mBtlvCore->GetTempHeap(),mBtlvCore->GetTempHeap()->GetLowerHandle(),ARCID_WAZAEFF_RESOURCE,GARC_wazaeff_resource_ee003_star_lz_BPTCL,true,DemoLib::Obj::RESOURCE_EFFECT,isUseGasc);
}
void BtlvSeqSystem::PreLoad_WazaCamera( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int envIdx,motIdx;
  file->LoadDataInt( &envIdx );
  file->LoadDataInt( &motIdx );

  bool isUseGasc = false;
#if PM_DEBUG
  if( mBtlvCore->GetDebugParam() && mBtlvCore->GetDebugParam()->isHioEffResource )
  {
    isUseGasc = true;
  }
#endif

  envIdx = mBtlvCore->CheckWazaDataId_Env(envIdx,0);
  motIdx = mBtlvCore->CheckWazaDataId_Camera(motIdx,0);

  mBtlvCore->GetResourceManager()->LoadRequestStep(mBtlvCore->GetTempHeap(),mBtlvCore->GetTempHeap()->GetLowerHandle(),ARCID_WAZAEFF_RESOURCE,envIdx,true,DemoLib::Obj::RESOURCE_NORMAL,isUseGasc);
  mBtlvCore->GetResourceManager()->LoadRequestStep(mBtlvCore->GetTempHeap(),mBtlvCore->GetTempHeap()->GetLowerHandle(),ARCID_WAZAEFF_RESOURCE,motIdx,true,DemoLib::Obj::RESOURCE_NORMAL,isUseGasc);
}

void BtlvSeqSystem::PreLoad_WazaCameraTrainer( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int envIdx,motIdx,trgTrainer;

  file->LoadDataInt( &envIdx );
  file->LoadDataInt( &motIdx );
  file->LoadDataInt( &trgTrainer );

  bool isUseGasc = false;
#if PM_DEBUG
  if( mBtlvCore->GetDebugParam() && mBtlvCore->GetDebugParam()->isHioEffResource )
  {
    isUseGasc = true;
  }
#endif

  envIdx = mBtlvCore->CheckWazaDataId_Env(envIdx,trgTrainer);
  motIdx = mBtlvCore->CheckWazaDataId_Camera(motIdx,trgTrainer);

  mBtlvCore->GetResourceManager()->LoadRequestStep(mBtlvCore->GetTempHeap(),mBtlvCore->GetTempHeap()->GetLowerHandle(),ARCID_WAZAEFF_RESOURCE,envIdx,true,DemoLib::Obj::RESOURCE_NORMAL,isUseGasc);
  mBtlvCore->GetResourceManager()->LoadRequestStep(mBtlvCore->GetTempHeap(),mBtlvCore->GetTempHeap()->GetLowerHandle(),ARCID_WAZAEFF_RESOURCE,motIdx,true,DemoLib::Obj::RESOURCE_NORMAL,isUseGasc);
}

void BtlvSeqSystem::PreLoad_WazaCameraDoubleIntro( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int envIdx,motIdx[3],trgTrainer;

  file->LoadDataInt( &envIdx );
  file->LoadDataInt( &motIdx[0] );  //分岐処理をこちらに入れるのは手間なので全部呼んじゃう
  file->LoadDataInt( &motIdx[1] );
  file->LoadDataInt( &motIdx[2] );

  bool isUseGasc = false;
#if PM_DEBUG
  if( mBtlvCore->GetDebugParam() && mBtlvCore->GetDebugParam()->isHioEffResource )
  {
    isUseGasc = true;
  }
#endif

  envIdx = mBtlvCore->CheckWazaDataId_Env(envIdx,0);
  motIdx[0] = mBtlvCore->CheckWazaDataId_Camera(motIdx[0],0);
  motIdx[1] = mBtlvCore->CheckWazaDataId_Camera(motIdx[1],0);
  motIdx[2] = mBtlvCore->CheckWazaDataId_Camera(motIdx[2],0);

  mBtlvCore->GetResourceManager()->LoadRequestStep(mBtlvCore->GetTempHeap(),mBtlvCore->GetTempHeap()->GetLowerHandle(),ARCID_WAZAEFF_RESOURCE,envIdx,true,DemoLib::Obj::RESOURCE_NORMAL,isUseGasc);
  mBtlvCore->GetResourceManager()->LoadRequestStep(mBtlvCore->GetTempHeap(),mBtlvCore->GetTempHeap()->GetLowerHandle(),ARCID_WAZAEFF_RESOURCE,motIdx[0],true,DemoLib::Obj::RESOURCE_NORMAL,isUseGasc);
  mBtlvCore->GetResourceManager()->LoadRequestStep(mBtlvCore->GetTempHeap(),mBtlvCore->GetTempHeap()->GetLowerHandle(),ARCID_WAZAEFF_RESOURCE,motIdx[1],true,DemoLib::Obj::RESOURCE_NORMAL,isUseGasc);
  mBtlvCore->GetResourceManager()->LoadRequestStep(mBtlvCore->GetTempHeap(),mBtlvCore->GetTempHeap()->GetLowerHandle(),ARCID_WAZAEFF_RESOURCE,motIdx[2],true,DemoLib::Obj::RESOURCE_NORMAL,isUseGasc);
}

void BtlvSeqSystem::PreLoad_WazaOuterMotion( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int trg,arcIdx;
  file->LoadDataInt( &trg );    //トレーナーもポケモンもありうる
  file->LoadDataInt( &arcIdx );

  bool isUseGasc = false;
#if PM_DEBUG
  if( mBtlvCore->GetDebugParam() && mBtlvCore->GetDebugParam()->isHioEffResource )
  {
    isUseGasc = true;
  }
#endif

  mBtlvCore->GetResourceManager()->LoadRequestStep(mBtlvCore->GetTempHeap(),mBtlvCore->GetTempHeap()->GetLowerHandle(),ARCID_WAZAEFF_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_ANIME,isUseGasc);
}
//ティキ専用外部モーション読み込み
void BtlvSeqSystem::PreLoad_WazaOuterMotionTiki( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int trg,arcIdx[4];
  file->LoadDataInt( &trg );
  file->LoadDataInt( &arcIdx[0] );
  file->LoadDataInt( &arcIdx[1] );
  file->LoadDataInt( &arcIdx[2] );
  file->LoadDataInt( &arcIdx[3] );

  bool isUseGasc = false;
#if PM_DEBUG
  if( mBtlvCore->GetDebugParam() && mBtlvCore->GetDebugParam()->isHioEffResource )
  {
    isUseGasc = true;
  }
#endif

  BtlvPos vPos = mBtlvCore->SeqComFunc_GetTargetPoke(trg);
  BtlvObject_PokeModel *poke = mBtlvCore->GetPokeModel(vPos);

  mBtlvCore->GetResourceManager()->LoadRequestStep(mBtlvCore->GetTempHeap(),mBtlvCore->GetTempHeap()->GetLowerHandle(),ARCID_WAZAEFF_RESOURCE,arcIdx[poke->GetTikiType()],true,DemoLib::Obj::RESOURCE_ANIME,isUseGasc);
}

void BtlvSeqSystem::PreLoad_WazaKisekaeMotion( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int trg,arcIdx;
  file->LoadDataInt( &trg );
  file->LoadDataInt( &arcIdx );

  bool isUseGasc = false;
#if PM_DEBUG
  if( mBtlvCore->GetDebugParam() && mBtlvCore->GetDebugParam()->isHioEffResource )
  {
    isUseGasc = true;
  }
#endif

  mBtlvCore->GetResourceManager()->LoadRequestStep(mBtlvCore->GetTempHeap(),mBtlvCore->GetTempHeap()->GetLowerHandle(),ARCID_WAZAEFF_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_NORMAL,isUseGasc);
}

void BtlvSeqSystem::PreLoad_WazaTexture( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int arcIdx;
  file->LoadDataInt( &arcIdx );

  arcIdx = mBtlvCore->CheckWazaDataId_Texture(arcIdx,0);

  bool isUseGasc = false;
#if PM_DEBUG
  if( mBtlvCore->GetDebugParam() && mBtlvCore->GetDebugParam()->isHioEffResource )
  {
    isUseGasc = true;
  }
#endif

  mBtlvCore->GetResourceManager()->LoadRequestStep(mBtlvCore->GetTempHeap(),mBtlvCore->GetTempHeap()->GetLowerHandle(),ARCID_WAZAEFF_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_TEXTURE,isUseGasc);
}

void BtlvSeqSystem::PreLoad_WazaTrainerTexture( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int trg;    //先読み処理の関係上先頭で！
  int type;
  file->LoadDataInt( &trg );
  file->LoadDataInt( &type );

  int arcIdx = mBtlvCore->CheckWazaDataId_TrainerTexture(trg,type);

  bool isUseGasc = false;
#if PM_DEBUG
  if( mBtlvCore->GetDebugParam() && mBtlvCore->GetDebugParam()->isHioEffResource )
  {
    isUseGasc = true;
  }
#endif

  mBtlvCore->GetResourceManager()->LoadRequestStep(mBtlvCore->GetTempHeap(),mBtlvCore->GetTempHeap()->GetLowerHandle(),ARCID_WAZAEFF_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_TEXTURE,isUseGasc);
}

void BtlvSeqSystem::PreLoad_WazaTrainerLight( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int trg,lightType;

  file->LoadDataInt( &trg );
  file->LoadDataInt( &lightType );

  bool isUseGasc = false;
#if PM_DEBUG
  if( mBtlvCore->GetDebugParam() && mBtlvCore->GetDebugParam()->isHioEffResource )
  {
    isUseGasc = true;
  }
#endif

  int envIdx = mBtlvCore->GetTrainerLightEnv(lightType,trg);

  mBtlvCore->GetResourceManager()->LoadRequestStep(mBtlvCore->GetTempHeap(),mBtlvCore->GetTempHeap()->GetLowerHandle(),ARCID_WAZAEFF_RESOURCE,envIdx,true,DemoLib::Obj::RESOURCE_NORMAL,isUseGasc);
}

void BtlvSeqSystem::PreLoad_WazaTrainerLightFile( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int trg,envIdx;

  file->LoadDataInt( &trg );  //読み捨て
  file->LoadDataInt( &envIdx );

  bool isUseGasc = false;
#if PM_DEBUG
  if( mBtlvCore->GetDebugParam() && mBtlvCore->GetDebugParam()->isHioEffResource )
  {
    isUseGasc = true;
  }
#endif

  mBtlvCore->GetResourceManager()->LoadRequestStep(mBtlvCore->GetTempHeap(),mBtlvCore->GetTempHeap()->GetLowerHandle(),ARCID_WAZAEFF_RESOURCE,envIdx,true,DemoLib::Obj::RESOURCE_NORMAL,isUseGasc);
}

void BtlvSeqSystem::PreLoad_Weather( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  //天候は常駐するのでResourceManager使えない
  int type;
  file->LoadDataInt( &type );

  mBtlvCore->LoadWeatherResource(type);


}


}

