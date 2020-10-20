//======================================================================
/**
 * @file	SequencePlayer.C++
 * @brief	シーケンスエディタで作成したデモファイルを再生する機構
 * @author	PETE
 * @data	2015.04.24
 */
//======================================================================
#include <debug/DebugWin/include/DebugWinSystem.h>
#include "DemoLib/SequencePlayer/include/Player.h"
#include "DemoLib/SeqEditor/include/SeqSystem.h"
///#include "DemoLib/SequencePlayer/include/CommandDefine.h"

#include <niji_conv_header/demoseq/seq_com_define.h>

// CharaModel:
#include <model/include/gfl2_CharaModelFactory.h>

// garc FileLoading
#include <arc_index/chara_model_field.gaix>
#include <arc_def_index/arc_def.h>

// PokeObject:
#include "DemoLib/Object/include/Object_Poke.h"

// モデルのインデックス
#include <niji_conv_header/battle/chara/chara_model_id.h>

// Sound:
#include "Sound/include/Sound.h"

GFL_NAMESPACE_BEGIN(DemoLib)
	GFL_NAMESPACE_BEGIN(Sequence)

#define SEQPLAYER_CMD_DEF(name) #name ,
static const char *const comName[SEQPLAYER_CMD_MAX]=
{
#include "DemoLib/SequencePlayer/include/seq_com.cdat"
};

// Command Table Setup:
typedef void(*SEQ_CMD_FUNC)( DemoLib::SeqEditor::SeqFile *file ,void *userWork , const DemoLib::SeqEditor::CommandParam &param );

#define SEQPLAYER_CMD_DEF(name) SEQ_CMD_ ## name ,
SEQ_CMD_FUNC comFunc[SEQPLAYER_CMD_MAX]=
{
#include "DemoLib/SequencePlayer/include/seq_com.cdat"
};

#define SEQPLAYER_CMD_DEF(name) SEQCOM_ID_ ## name ,
enum
{
#include "DemoLib/SequencePlayer/include/seq_com.cdat"	
};

Player :: Player(SequenceViewSystem *core)
{
	mSeqViewCore = core;
	mSeqFile = NULL;

	gfl2::heap::HeapBase *heapMem = mSeqViewCore->GetMemHeap();

	DemoLib::SeqEditor::InitParam initParam =
	{
		heapMem,
		seq_com_define_size_table
	};

	mSeqSys = GFL_NEW(heapMem)DemoLib::SeqEditor::SeqSystem(initParam);
	mPause = false;
  mPreprocess = false;
  mPokemonCount = 0;

  mUseInframe1 = mUseInframe2 = mUseWarpModelDraw = false;
  mUsePokeColorShader = false;
  mUseMotionBlur = false;
  mUseBloom = false;
}

Player :: ~Player()
{
	GFL_SAFE_DELETE(mSeqSys);
}

enum 
{
  SEQ_OPTIONTYPE_NONE = 0,
  SEQ_OPTIONTYPE_PLAYERMALE = 1,
  SEQ_OPTIONTYPE_PLAYERFEMALE = 2,
  SEQ_OPTIONTYPE_TIMEZONE_MORNINGNOON = 3,  
  SEQ_OPTIONTYPE_TIMEZONE_EVENING = 4,
  SEQ_OPTIONTYPE_TIMEZONE_NIGHT = 5,
  SEQ_OPTIONTYPE_SYSTEM_CANCEL = 10,
  SEQ_OPTIONTYPE_SYSTEM_NOTCANCEL = 11,
  SEQ_OPTIONTYPE_WIN = 12,
  SEQ_OPTIONTYPE_LOSE = 13,
  SEQ_OPTIONTYPE_DRAW = 14,
  SEQ_OPTIONTYPE_PRESTART = 15,
  SEQ_OPTIONTYPE_DEMOTYPE_A = 100,
  SEQ_OPTIONTYPE_DEMOTYPE_B = 101,
  SEQ_OPTIONTYPE_DEMOTYPE_C = 102,
  SEQ_OPTIONTYPE_DEMOTYPE_D = 103,
  SEQ_OPTIONTYPE_DEMOTYPE_E = 104,
  SEQ_OPTIONTYPE_DEMOTYPE_F = 105,
  SEQ_OPTIONTYPE_DEMOTYPE_G = 106,
  SEQ_OPTIONTYPE_DEMOTYPE_H = 107,
  SEQ_OPTIONTYPE_DEMOTYPE_I = 108,
  SEQ_OPTIONTYPE_DEMOTYPE_J = 109,
  SEQ_OPTIONTYPE_DEMOTYPE_K = 110,
  SEQ_OPTIONTYPE_DEMOTYPE_L = 111,
  SEQ_OPTIONTYPE_DEMOTYPE_A_MALE = 1100,
  SEQ_OPTIONTYPE_DEMOTYPE_A_FEMALE = 2100,
  SEQ_OPTIONTYPE_DEMOTYPE_B_MALE = 1101,
  SEQ_OPTIONTYPE_DEMOTYPE_B_FEMALE = 2101,
  SEQ_OPTIONTYPE_LANG_JP = 200,
  SEQ_OPTIONTYPE_LANG_CHN = 201,
  SEQ_OPTIONTYPE_LANG_DEU = 202,
  SEQ_OPTIONTYPE_LANG_ESP = 203,
  SEQ_OPTIONTYPE_LANG_FRA = 204,
  SEQ_OPTIONTYPE_LANG_ITA = 205,
  SEQ_OPTIONTYPE_LANG_KOR = 206,
  SEQ_OPTIONTYPE_LANG_TWN = 207,
  SEQ_OPTIONTYPE_LANG_USA = 208,  
  SEQ_OPTIONTYPE_SUN = 300,  
  SEQ_OPTIONTYPE_MOON = 301,  
  SEQ_OPTIONTYPE_MAX,
};

bool Player :: CanRunCommand(int optionType,bool isSkipMode)
{
  switch(optionType)  
  {
    case SEQ_OPTIONTYPE_NONE: break;

    case SEQ_OPTIONTYPE_PLAYERMALE: 
      return(mSeqViewCore->IsPlayerSexMale());      
    
    case SEQ_OPTIONTYPE_PLAYERFEMALE: 
      return(mSeqViewCore->IsPlayerSexFemale());
    
    case SEQ_OPTIONTYPE_TIMEZONE_MORNINGNOON:    
      return(mSeqViewCore->IsTimezone_Morning() || mSeqViewCore->IsTimezone_Noon());      
    
    case SEQ_OPTIONTYPE_TIMEZONE_EVENING: 
      return(mSeqViewCore->IsTimezone_Evening());
    
    case SEQ_OPTIONTYPE_TIMEZONE_NIGHT: 
      return(mSeqViewCore->IsTimezone_Night());

    case SEQ_OPTIONTYPE_SYSTEM_CANCEL:
      if(isSkipMode) return(true);
      return(mSeqViewCore->IsSystemCancel());

    case SEQ_OPTIONTYPE_SYSTEM_NOTCANCEL:
      if(isSkipMode) return(true);
      return(!mSeqViewCore->IsSystemCancel());

    case SEQ_OPTIONTYPE_WIN:
      return(mSeqViewCore->GetPlaybackParameter(SEQPARAMTYPE_VSBATTLE)==VSBATTLE_WIN);

    case SEQ_OPTIONTYPE_LOSE:
      return(mSeqViewCore->GetPlaybackParameter(SEQPARAMTYPE_VSBATTLE)==VSBATTLE_LOSE);

    case SEQ_OPTIONTYPE_DRAW:
      return(mSeqViewCore->GetPlaybackParameter(SEQPARAMTYPE_VSBATTLE)==VSBATTLE_DRAW);

    case SEQ_OPTIONTYPE_PRESTART:
      return(mSeqViewCore->GetPlaybackParameter(SEQPARAMTYPE_VSBATTLE)==VSBATTLE_PRESTART);

    // Demo Types:
    case SEQ_OPTIONTYPE_DEMOTYPE_A:
      return(mSeqViewCore->IsDemoType(0));

    case SEQ_OPTIONTYPE_DEMOTYPE_A_MALE:
      return(mSeqViewCore->IsDemoType(0) && mSeqViewCore->IsPlayerSexMale());

    case SEQ_OPTIONTYPE_DEMOTYPE_A_FEMALE:
      return(mSeqViewCore->IsDemoType(0) && mSeqViewCore->IsPlayerSexFemale());
    
    case SEQ_OPTIONTYPE_DEMOTYPE_B:
      return(mSeqViewCore->IsDemoType(1));

    case SEQ_OPTIONTYPE_DEMOTYPE_B_MALE:
      return(mSeqViewCore->IsDemoType(1) && mSeqViewCore->IsPlayerSexMale());

    case SEQ_OPTIONTYPE_DEMOTYPE_B_FEMALE:
      return(mSeqViewCore->IsDemoType(1) && mSeqViewCore->IsPlayerSexFemale());

    case SEQ_OPTIONTYPE_DEMOTYPE_C:
      return(mSeqViewCore->IsDemoType(2));

    case SEQ_OPTIONTYPE_DEMOTYPE_D:
      return(mSeqViewCore->IsDemoType(3));

    case SEQ_OPTIONTYPE_DEMOTYPE_E:
      return(mSeqViewCore->IsDemoType(4));

    case SEQ_OPTIONTYPE_DEMOTYPE_F:
      return(mSeqViewCore->IsDemoType(5));

    case SEQ_OPTIONTYPE_DEMOTYPE_G:
      return(mSeqViewCore->IsDemoType(6));

    case SEQ_OPTIONTYPE_DEMOTYPE_H:
      return(mSeqViewCore->IsDemoType(7));

    case SEQ_OPTIONTYPE_DEMOTYPE_I:
      return(mSeqViewCore->IsDemoType(8));

    case SEQ_OPTIONTYPE_DEMOTYPE_J:
      return(mSeqViewCore->IsDemoType(9));

    case SEQ_OPTIONTYPE_DEMOTYPE_K:
      return(mSeqViewCore->IsDemoType(10));

    case SEQ_OPTIONTYPE_DEMOTYPE_L:
      return(mSeqViewCore->IsDemoType(11));

    case SEQ_OPTIONTYPE_LANG_JP:
      return(mSeqViewCore->GetLanguageType() == DEMO_LANG_JP);

    case SEQ_OPTIONTYPE_LANG_CHN:
      return(mSeqViewCore->GetLanguageType() == DEMO_LANG_CHN);

    case SEQ_OPTIONTYPE_LANG_DEU:
      return(mSeqViewCore->GetLanguageType() == DEMO_LANG_DEU);

    case SEQ_OPTIONTYPE_LANG_ESP:
      return(mSeqViewCore->GetLanguageType() == DEMO_LANG_ESP);

    case SEQ_OPTIONTYPE_LANG_FRA:
      return(mSeqViewCore->GetLanguageType() == DEMO_LANG_FRA);

    case SEQ_OPTIONTYPE_LANG_ITA:
      return(mSeqViewCore->GetLanguageType() == DEMO_LANG_ITA);

    case SEQ_OPTIONTYPE_LANG_KOR:
      return(mSeqViewCore->GetLanguageType() == DEMO_LANG_KOR);

    case SEQ_OPTIONTYPE_LANG_TWN:
      return(mSeqViewCore->GetLanguageType() == DEMO_LANG_TWN);

    case SEQ_OPTIONTYPE_LANG_USA:
      return(mSeqViewCore->GetLanguageType() == DEMO_LANG_USA);

    case SEQ_OPTIONTYPE_SUN:
      return(mSeqViewCore->IsSun());
    case SEQ_OPTIONTYPE_MOON:
      return(mSeqViewCore->IsMoon());
          
  }
  
  return(true);

}

void Player :: ForceCreatePokemon(const DemoLib::SeqEditor::CommandParam &param)
{
  if(param.isSkipMode) return; // Don't create during skip mode..
  if(param.commandNo!=SEQCOM_ID_POKEMON_CREATE && param.commandNo!=SEQCOM_ID_POKEMON_CREATEBYPARAM) return;

  switch(param.groupOpt)  
  {
    case SEQ_OPTIONTYPE_SYSTEM_CANCEL:
    case SEQ_OPTIONTYPE_SYSTEM_NOTCANCEL:
      break;

    default: return;
  }
  
  // Need to discard this pokemon:
  SequenceObject_Poke *poke = mSeqViewCore->GetIndexPokemonAndRemove(0);   
  poke->ForceDelete();
  
  GFL_SAFE_DELETE(poke);
  
}

void Player :: CommandCallbackPreprocess( DemoLib::SeqEditor::SeqFile *seqFile , const DemoLib::SeqEditor::CommandParam &param )
{
  switch(param.commandNo)
  {
    case SEQCOM_ID_POKEMON_SHADER_COLOR:
      mUsePokeColorShader = true;
      break;

    case SEQCOM_ID_MODEL_CREATE:
    case SEQCOM_ID_MODEL_CREATE_MONSTERBALL:
      {
        int arcid,useMulti,drawtype;
        seqFile->LoadDataInt(&arcid);
        seqFile->LoadDataInt(&useMulti);
        seqFile->LoadDataInt(&drawtype);
        if(drawtype==SEQ_DEF_DRAWTYPE_WARP) mUseWarpModelDraw = true;
      }
      break;

    case SEQCOM_ID_MOTIONBLUR_START:
    case SEQCOM_ID_MOTIONBLUR_ADJUST:
    case SEQCOM_ID_MOTIONBLUR_COLOR:
      mUseMotionBlur = true;      
      break;

    case SEQCOM_ID_BLOOM_SETTING:
    case SEQCOM_ID_NEW_BLOOM_START:
      mUseBloom = true;
      break;

    case SEQCOM_ID_POKEMON_CREATE:
    case SEQCOM_ID_POKEMON_CREATEBYPARAM:
      if(!CanRunCommand(param.groupOpt,param.isSkipMode))
      {
        if(param.groupOpt!=SEQ_OPTIONTYPE_SYSTEM_CANCEL && param.groupOpt!=SEQ_OPTIONTYPE_SYSTEM_NOTCANCEL) return;
      }

      // Check DrawType:
      {
        int index,type,drawtype;
        seqFile->LoadDataInt(&index);        
        seqFile->LoadDataInt(&type);
        seqFile->LoadDataInt(&drawtype);

        if(drawtype==1 || drawtype==4) mUseInframe1 = true;
        else if(drawtype==2 || drawtype==5) mUseInframe2 = true;
      }

      mPokemonCount++;
      GFL_PRINT("*Found Pokemon %i*\n",mPokemonCount);
      break;

  }
}

void Player :: CommnadCallback( DemoLib::SeqEditor::SeqFile *seqFile , const DemoLib::SeqEditor::CommandParam &param )
{
  if(mPreprocess) 
  {
    CommandCallbackPreprocess(seqFile,param);
    return;
  }

  if(!CanRunCommand(param.groupOpt,param.isSkipMode))
  {
    // Force Load Pokemon if needed:
    ForceCreatePokemon(param);   
    return;
  }

	if( param.isSkipMode )
	{
		switch( param.commandNo )
		{
      
      case SEQCOM_ID_POKEMON_CREATE:
        PreLoad_PokemonModelAndAnimationData(seqFile,NULL,param);
        break;

      case SEQCOM_ID_POKEMON_CREATEBYPARAM:
        PreLoad_PokemonFromParameterData(seqFile,NULL,param);
        break;

      case SEQCOM_ID_POKEMON_SETANIMATIONEXTERNAL:
        PreLoad_PokemonAnimationData(seqFile,NULL,param);
        break;

      case SEQCOM_ID_POKEMON_SETANIMATIONEXTERNAL_MULTI:
        PreLoad_PokemonAnimationDataMulti(seqFile,NULL,param);
        break;

      case SEQCOM_ID_CHARAMODEL_CREATENPC_FIELD:
        PreLoad_CharaModelFieldData(seqFile,NULL,param);
        break;

      case SEQCOM_ID_CHARAMODEL_CREATENPC_FIELD_ATTACHITEM:
        PreLoad_CharaModelFieldData_AttachItem(seqFile,NULL,param);
        break;

      case SEQCOM_ID_CHARAMODEL_CREATENPC_BATTLE:
        PreLoad_CharaModelBattleData(seqFile,NULL,param);
        break;

      case SEQCOM_ID_CHARAMODEL_CREATENPC_BATTLE_ATTACHITEM:
        PreLoad_CharaModelBattleData_AttachItem(seqFile,NULL,param);
        break;

      case SEQCOM_ID_CHARAMODEL_CREATE_VSBATTLE:
        PreLoad_CharaVSBattleData(seqFile,NULL,param);
        break;

      case SEQCOM_ID_BLOOM_SETTING:
      case SEQCOM_ID_NEW_BLOOM_START:
        PreLoad_BloomMask(seqFile, NULL, param);
        break;

			case SEQCOM_ID_MODEL_CREATE:
				PreLoad_Model(seqFile,NULL,param);
				break;

      case SEQCOM_ID_MODEL_CREATE_MONSTERBALL:
        PreLoad_BallModel(seqFile,NULL,param);
        break;

      case SEQCOM_ID_CHARAMODEL_SETANIMATIONEXTERNAL:      
        PreLoad_CharaModelAnimation(seqFile,NULL,param);
        break;

			case SEQCOM_ID_MODEL_ANIMATION:
      case SEQCOM_ID_CHARAMODEL_SETANIMATIONEXTERNAL_DRESSUP:
				PreLoad_ModelAnimation(seqFile,NULL,param);
				break;

      case SEQCOM_ID_MODEL_ANIMATION_MULTI:
      case SEQCOM_ID_MODEL_SET_TIMEANIMATION:
        PreLoad_ModelAnimationMulti(seqFile,NULL,param);
        break;

      case SEQCOM_ID_LIGHT_CREATE_FIELD:
        PreLoad_FieldLight(seqFile,NULL,param);
        break;

      case SEQCOM_ID_LIGHT_CREATE:
      case SEQCOM_ID_LIGHT_CREATE_NORMAL2:
        PreLoad_Light(seqFile,NULL,param);
        break;

      case SEQCOM_ID_LIGHT_ANIMATION:
        PreLoad_LightAnimation(seqFile,NULL,param);
        break;

			case SEQCOM_ID_CAMERA_ANIMATION:
				PreLoad_CameraAnimation(seqFile,NULL,param);
				break;

			case SEQCOM_ID_CREATE_LAYOUT:
				PreLoad_Layout(seqFile,NULL,param);
				break;

      case SEQCOM_ID_CREATE_LAYOUT_MULTILANGUAGE:
        PreLoad_Layout_MutiLanguage(seqFile,NULL,param);
        break;

      case SEQCOM_ID_SKYBOX_CREATE:
        PreLoad_Skybox(seqFile,NULL,param);
        break;

			case SEQCOM_ID_SOUND_BGM_CHANGE:
        if(param.startFrame==0)
				  comFunc[param.commandNo]( seqFile , mSeqViewCore , param );
				break;

      case SEQCOM_ID_SOUND_BGM_PLAY:
        if(param.startFrame==0)
          comFunc[param.commandNo]( seqFile , mSeqViewCore , param );
        break;

      case SEQCOM_ID_PARTICLE_CREATE:
        PreLoad_ParticleEffect(seqFile,NULL,param);
        break;

      case SEQCOM_ID_SYSTEM_TIMING_SYNC:
      case SEQCOM_ID_SYSTEM_KEYWAIT:
        mSeqViewCore->AddSystemSync(param.startFrame);
        break;

      case SEQCOM_ID_MESSAGE_ARCFILE_SETTING:
        {
          int fileID;
          seqFile->LoadDataInt(&fileID);
          mSeqViewCore->SetTextWindowArcID((u32)fileID);
        }
        break;
				
      case SEQCOM_ID_MESSAGE_ARCFILE_SETTING_FIELD:
        {
          int fileID;
          seqFile->LoadDataInt(&fileID);
          mSeqViewCore->SetTextWindowArcID((u32)fileID,true);
        }
        break;

        // Set the Window Style:
      case SEQCOM_ID_MESSAGE_DISPLAY:
        {
          int messageID,windowType;
          seqFile->LoadDataInt(&messageID); 
          seqFile->LoadDataInt(&windowType); 
          mSeqViewCore->SetTextWindowStyle(windowType);
        }
        break;



		}
			
	}
	else
	{

		switch( param.commandNo )
		{
			// Don't execute BGM commands, they are already done during preload:
    case SEQCOM_ID_SOUND_BGM_CHANGE:
    case SEQCOM_ID_SOUND_BGM_PLAY:
        if(param.startFrame==0) 
        {
          break;
        }
       
			default:
				GFL_PRINT("(%i) -> %s\n",GetSeqFrame(),comName[param.commandNo]);
				comFunc[param.commandNo]( seqFile , mSeqViewCore , param );
				break;
		}

		
	}
}

int Player :: GetSeqFrame(void)
{
	if( mSeqFile != NULL ) return mSeqFile->GetFrame();		
	return 0;	
}

int Player :: GetMaxSeqFrame(void)
{
  if( mSeqFile != NULL ) return mSeqFile->GetMaxFrame();		
  return 0;	
}
void Player :: Update(int step)
{
	if( mPause == false && mSeqSys != NULL )
	{
		mSeqSys->UpdateFrame(step);
    if( mSeqFile && mSeqFile->IsFinish() )
    {
      mSeqSys->UnLoad(mSeqFile);
      mSeqFile = NULL;
    }
	}
}

void Player :: DebugLoadPath( char *path )
{
	mSeqFile = mSeqSys->LoadPcFile( mSeqViewCore->GetAsyncFileManager(), path , this );
}

void Player :: LoadSeqFile( int dataID  )
{
	GFL_PRINT("SeqPlayer :: LoadSeqFile[%d]\n",dataID);
	mSeqFile = mSeqSys->LoadFile(mSeqViewCore->GetAsyncFileManager(),ARCID_DEMOSEQUENCE_SCRIPT,dataID,this);
}

void Player :: PreprocessCount(void)
{
  mPreprocess = true;
  mSeqFile->SkipFrame( -1 );
  mSeqFile->ResetSeq();
  mPreprocess = false;
}


void Player :: PreloadAll(void)
{
	mSeqFile->SkipFrame( -1 );
	mSeqFile->ResetSeq();
}

void Player :: SetSeqFrame(int n)
{
	if(GetSeqFrame() == n) return;
	//	mSeqFile->SkipFrame(n,false);
}

void Player :: PreLoad_PokemonModelAndAnimationData( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int index,type,drawgroup,shaderUse,internalAnimation,loadExternalMotion,externalMotion;
  
  file->LoadDataInt(&index);
///  file->LoadDataInt(&preload);
  file->LoadDataInt(&type);
  file->LoadDataInt(&drawgroup);
  file->LoadDataInt(&shaderUse);
  file->LoadDataInt(&internalAnimation);
  file->LoadDataInt(&loadExternalMotion);
  file->LoadDataInt(&externalMotion);


  GFL_PRINT("SeqPlayer :: Preload PokemonModel[%d]\n",index);
  
  switch(type)
  {
    case 0: type = PokeTool::MODEL_ANIMETYPE_BATTLE; break;
    case 1: type = PokeTool::MODEL_ANIMETYPE_KAWAIGARI; break;
    case 2: type = PokeTool::MODEL_ANIMETYPE_APP; break;
  }

  mSeqViewCore->LoadPokemon(index,type);

  if(loadExternalMotion != 0 && externalMotion>=0)
  {
   
    GFL_PRINT("SeqPlayer :: Preload PokemonModelAnimation[%d]\n",externalMotion);

    bool useGasc = false;
#if PM_DEBUG
    if(mSeqViewCore->UseHIO()) useGasc = true;        
#endif

    mSeqViewCore->GetResourceManager()->LoadRequest(mSeqViewCore->GetDevHeap(),mSeqViewCore->GetDevHeap(),ARCID_DEMOSEQUENCE_RESOURCE,externalMotion,true,DemoLib::Obj::RESOURCE_ANIME,useGasc);

  }
   
}

void Player :: PreLoad_PokemonFromParameterData( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int monsNo,formNo,sexType,type,drawgroup,shaderUse,internalAnimation,loadExternalMotion,externalMotion;

  file->LoadDataInt(&monsNo);
  file->LoadDataInt(&formNo);
  file->LoadDataInt(&sexType);  
  file->LoadDataInt(&type);
  file->LoadDataInt(&drawgroup);
  file->LoadDataInt(&shaderUse);
  file->LoadDataInt(&internalAnimation);
  file->LoadDataInt(&loadExternalMotion);
  file->LoadDataInt(&externalMotion);


  GFL_PRINT("SeqPlayer :: Preload PokemonFromParameterModel[%d,%d,%d]\n",monsNo,formNo,sexType);

  mSeqViewCore->LoadPokemonFromParameters(monsNo,formNo,sexType,type);

  if(loadExternalMotion != 0 && externalMotion>=0)
  {
    GFL_PRINT("SeqPlayer :: Preload PokemonModelAnimation[%d]\n",externalMotion);

    bool useGasc = false;
#if PM_DEBUG
    if(mSeqViewCore->UseHIO()) useGasc = true;        
#endif

    mSeqViewCore->GetResourceManager()->LoadRequest(mSeqViewCore->GetDevHeap(),mSeqViewCore->GetDevHeap(),ARCID_DEMOSEQUENCE_RESOURCE,externalMotion,true,DemoLib::Obj::RESOURCE_ANIME,useGasc);
  }


}


void Player :: PreLoad_PokemonAnimationData( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int externalMotion = 0;

  file->LoadDataInt( &externalMotion );	  

  GFL_PRINT("SeqPlayer :: Preload PokemonAnimation[%d]\n",externalMotion);  

  bool useGasc = false;
#if PM_DEBUG
  if(mSeqViewCore->UseHIO()) useGasc = true;        
#endif

  mSeqViewCore->GetResourceManager()->LoadRequest(mSeqViewCore->GetDevHeap(),mSeqViewCore->GetDevHeap(),ARCID_DEMOSEQUENCE_RESOURCE,externalMotion,true,DemoLib::Obj::RESOURCE_ANIME,useGasc);

}

void Player :: PreLoad_PokemonAnimationDataMulti( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int internalMotion = 0;
  int externalMotion = 0;

  // Only need to load external motion, so skip internal motion:
  file->LoadDataInt( &internalMotion );	  
  file->LoadDataInt( &externalMotion );	  

  GFL_PRINT("SeqPlayer :: Preload PokemonAnimationMulti[%d]\n",externalMotion);  

  bool useGasc = false;
#if PM_DEBUG
  if(mSeqViewCore->UseHIO()) useGasc = true;        
#endif

  mSeqViewCore->GetResourceManager()->LoadRequest(mSeqViewCore->GetDevHeap(),mSeqViewCore->GetDevHeap(),ARCID_DEMOSEQUENCE_RESOURCE,externalMotion,true,DemoLib::Obj::RESOURCE_ANIME,useGasc);

}

void Player :: PreLoad_CharaModelFieldData( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int index,dress;
  file->LoadDataInt(&index);
  file->LoadDataInt(&dress);

  GFL_PRINT("SeqPlayer :: Preload CharaModelField[%d]\n",index);

  // Preload Chara Field Model:
  mSeqViewCore->LoadCharaFieldModel(index);

  // DressUp:
  if(dress!=-1)
  {
    if(index!=0) mSeqViewCore->AddDressUpParamLoadField(dress);
    else mSeqViewCore->AddDressUpParamLoadField(dress+1);
  }

}

void Player :: PreLoad_CharaModelFieldData_AttachItem( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int index,dress,item,drawtype;
  file->LoadDataInt(&index);
  file->LoadDataInt(&dress);
  file->LoadDataInt(&drawtype);
  file->LoadDataInt(&item);

  GFL_PRINT("SeqPlayer :: Preload CharaModelField[%d]\n",index);

  // Preload Chara Field Model:
  mSeqViewCore->LoadCharaFieldModel(index);

  // DressUp:
  if(dress!=-1)
  {
    if(index!=0) mSeqViewCore->AddDressUpParamLoadField(dress);
    else mSeqViewCore->AddDressUpParamLoadField(dress+1);
  }

  mSeqViewCore->LoadCharaFieldAttachItemModel(item);


}

void Player :: PreLoad_CharaModelBattleData( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int index,dress;
  file->LoadDataInt(&index);
  file->LoadDataInt(&dress);

  GFL_PRINT("SeqPlayer :: Preload CharaModelBattle[%d]\n",index);

  // Preload Chara Battle Model:
  mSeqViewCore->LoadCharaBattleModel(index);

  // DressUp:
  if(dress!=-1)
  {
    if(index!=0)
      mSeqViewCore->AddDressUpParamLoadBattle(dress);
    else mSeqViewCore->AddDressUpParamLoadBattle(dress+1);
  }  

}

void Player :: PreLoad_CharaModelBattleData_AttachItem( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int index,dress,item,drawtype;
  file->LoadDataInt(&index);
  file->LoadDataInt(&dress);
  file->LoadDataInt(&drawtype);
  file->LoadDataInt(&item);

  GFL_PRINT("SeqPlayer :: Preload CharaModelBattle[%d]\n",index);

  // Preload Chara Battle Model:
  mSeqViewCore->LoadCharaBattleModel(index);

  // DressUp:
  if(dress!=-1)
  {
    if(index!=0)
      mSeqViewCore->AddDressUpParamLoadBattle(dress);
    else mSeqViewCore->AddDressUpParamLoadBattle(dress+1);
  }

  mSeqViewCore->LoadCharaBattleAttachItemModel(item);

}

void Player :: PreLoad_CharaVSBattleData( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int index;
  file->LoadDataInt(&index);  

  GFL_PRINT("SeqPlayer :: Preload CharaVSBattle[%d]\n",index);

  // Preload Chara Battle Model:
  mSeqViewCore->LoadCharaVSBattleModel(mSeqViewCore->GetVSBattleModelID(index) );

  // DressUp:
  mSeqViewCore->AddDressUpParamLoadBattle(index);
  
}

void Player :: PreLoad_BloomMask( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int arcIdx = 0;

  file->LoadDataInt( &arcIdx );	
  
  GFL_PRINT("SeqPlayer :: Preload BloomMask[%d]\n",arcIdx);

  bool useGasc = false;
#if PM_DEBUG
  if(mSeqViewCore->UseHIO()) useGasc = true;        
#endif

  mSeqViewCore->GetResourceManager()->LoadRequest(mSeqViewCore->GetDevHeap(),mSeqViewCore->GetDevHeap(),ARCID_DEMOSEQUENCE_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_NORMAL,useGasc);

}

void Player :: PreLoad_Model( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
	int arcIdx = 0;
	
	file->LoadDataInt( &arcIdx );	
	arcIdx = mSeqViewCore->CheckDataId_Model(arcIdx,0);
		
	GFL_PRINT("SeqPlayer :: Preload Model[%d]\n",arcIdx);

  bool useGasc = false;
#if PM_DEBUG
  if(mSeqViewCore->UseHIO()) useGasc = true;        
#endif

	mSeqViewCore->GetResourceManager()->LoadRequest(mSeqViewCore->GetDevHeap(),mSeqViewCore->GetDevHeap(),ARCID_DEMOSEQUENCE_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_NORMAL,useGasc);

}

void Player :: PreLoad_ModelAnimation( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
	int arcIdx = 0;

	file->LoadDataInt( &arcIdx );	
	arcIdx = mSeqViewCore->CheckDataId_Model(arcIdx,0);

	GFL_PRINT("SeqPlayer :: Preload ModelAnimation[%d]\n",arcIdx);

  bool useGasc = false;
#if PM_DEBUG
  if(mSeqViewCore->UseHIO()) useGasc = true;        
#endif

  mSeqViewCore->GetResourceManager()->LoadRequest(mSeqViewCore->GetDevHeap(),mSeqViewCore->GetDevHeap(),ARCID_DEMOSEQUENCE_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_NORMAL,useGasc);

}

void Player :: PreLoad_ModelAnimationMulti( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int arcIdx = 0;
  int arcIdx2 = 0;

  file->LoadDataInt( &arcIdx );	
  file->LoadDataInt( &arcIdx2 );	
  arcIdx = mSeqViewCore->CheckDataId_Model(arcIdx,0);
  arcIdx2 = mSeqViewCore->CheckDataId_Model(arcIdx2,0);

  GFL_PRINT("SeqPlayer :: Preload ModelAnimationMulti[%d,%d]\n",arcIdx,arcIdx2);

  bool useGasc = false;
#if PM_DEBUG
  if(mSeqViewCore->UseHIO()) useGasc = true;        
#endif

  mSeqViewCore->GetResourceManager()->LoadRequest(mSeqViewCore->GetDevHeap(),mSeqViewCore->GetDevHeap(),ARCID_DEMOSEQUENCE_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_NORMAL,useGasc);
  mSeqViewCore->GetResourceManager()->LoadRequest(mSeqViewCore->GetDevHeap(),mSeqViewCore->GetDevHeap(),ARCID_DEMOSEQUENCE_RESOURCE,arcIdx2,true,DemoLib::Obj::RESOURCE_NORMAL,useGasc);

}

void Player :: PreLoad_CharaModelAnimation( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int arcIdx = 0;

  file->LoadDataInt( &arcIdx );	
  arcIdx = mSeqViewCore->CheckDataId_Model(arcIdx,0);

  GFL_PRINT("SeqPlayer :: Preload CharaModelAnimation[%d]\n",arcIdx);

  bool useGasc = false;
#if PM_DEBUG
  if(mSeqViewCore->UseHIO()) useGasc = true;        
#endif

  mSeqViewCore->GetResourceManager()->LoadRequest(mSeqViewCore->GetDevHeap(),mSeqViewCore->GetDevHeap(),ARCID_DEMOSEQUENCE_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_ANIME,useGasc);

}


void Player :: PreLoad_CameraAnimation( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
	int arcIdx = 0;

	// Draw Env (GfEnv):
	file->LoadDataInt( &arcIdx );	
	GFL_PRINT("SeqPlayer :: Preload Camera DrawEnv[%d]\n",arcIdx);

  bool useGasc = false;
#if PM_DEBUG
  if(mSeqViewCore->UseHIO()) useGasc = true;        
#endif

  mSeqViewCore->GetResourceManager()->LoadRequest(mSeqViewCore->GetDevHeap(),mSeqViewCore->GetDevHeap(),ARCID_DEMOSEQUENCE_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_NORMAL,useGasc);

	// Animation (GFMot):
	file->LoadDataInt( &arcIdx );	
	GFL_PRINT("SeqPlayer :: Preload Camera Motion[%d]\n",arcIdx);
	mSeqViewCore->GetResourceManager()->LoadRequest(mSeqViewCore->GetDevHeap(),mSeqViewCore->GetDevHeap(),ARCID_DEMOSEQUENCE_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_NORMAL,useGasc);

}

void Player :: PreLoad_Layout( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
	int arcIdx = 0;
	file->LoadDataInt( &arcIdx );		
	GFL_PRINT("SeqPlayer :: Preload Layout[%d]\n",arcIdx);
  
  bool useGasc = false;
#if PM_DEBUG
  if(mSeqViewCore->UseHIO()) useGasc = true;        
#endif
///	mSeqViewCore->GetResourceManager()->LoadRequest(mSeqViewCore->GetDevHeap(),mSeqViewCore->GetDevHeap(),ARCID_LYTARCTEST,arcIdx,false);
  mSeqViewCore->GetResourceManager()->LoadRequest(mSeqViewCore->GetDevHeap(),mSeqViewCore->GetDevHeap(),ARCID_DEMOSEQUENCELAYOUT_RESOURCE,arcIdx,false,DemoLib::Obj::RESOURCE_NORMAL,useGasc);
}

void Player :: PreLoad_Layout_MutiLanguage( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int arcIdx = 0;
  file->LoadDataInt( &arcIdx );		
  GFL_PRINT("SeqPlayer :: Preload Multi-Lang Layout[%d]\n",arcIdx);

  int garcID = mSeqViewCore->GetLanguageLayoutGARC();

  bool useGasc = false;
#if PM_DEBUG
  if(mSeqViewCore->UseHIO()) useGasc = true;        
#endif

  ///	mSeqViewCore->GetResourceManager()->LoadRequest(mSeqViewCore->GetDevHeap(),mSeqViewCore->GetDevHeap(),ARCID_LYTARCTEST,arcIdx,false);
  mSeqViewCore->GetResourceManager()->LoadRequest(mSeqViewCore->GetDevHeap(),mSeqViewCore->GetDevHeap(),(gfl2::fs::ArcFile::ARCID)garcID,arcIdx,false,DemoLib::Obj::RESOURCE_NORMAL,useGasc);
}

void Player :: PreLoad_FieldLight( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int arcIdx = 0;
  int motIdx1 = 0;
  int motIdx2 = 0;
  file->LoadDataInt( &arcIdx );		
  file->LoadDataInt( &motIdx1 );		
  file->LoadDataInt( &motIdx2 );		
  GFL_PRINT("SeqPlayer :: Preload Field Light[%d][%d][%d]\n",arcIdx,motIdx1,motIdx2);

  bool useGasc = false;
#if PM_DEBUG
  if(mSeqViewCore->UseHIO()) useGasc = true;        
#endif

  mSeqViewCore->GetResourceManager()->LoadRequest(mSeqViewCore->GetDevHeap(),mSeqViewCore->GetDevHeap(),ARCID_DEMOSEQUENCE_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_NORMAL,useGasc);
  mSeqViewCore->GetResourceManager()->LoadRequest(mSeqViewCore->GetDevHeap(),mSeqViewCore->GetDevHeap(),ARCID_DEMOSEQUENCE_RESOURCE,motIdx1,true,DemoLib::Obj::RESOURCE_ANIME,useGasc);
  mSeqViewCore->GetResourceManager()->LoadRequest(mSeqViewCore->GetDevHeap(),mSeqViewCore->GetDevHeap(),ARCID_DEMOSEQUENCE_RESOURCE,motIdx2,true,DemoLib::Obj::RESOURCE_ANIME,useGasc);

}


void Player :: PreLoad_Light( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int arcIdx = 0;
  file->LoadDataInt( &arcIdx );		
  GFL_PRINT("SeqPlayer :: Preload Light[%d]\n",arcIdx);

  bool useGasc = false;
#if PM_DEBUG
  if(mSeqViewCore->UseHIO()) useGasc = true;        
#endif

  mSeqViewCore->GetResourceManager()->LoadRequest(mSeqViewCore->GetDevHeap(),mSeqViewCore->GetDevHeap(),ARCID_DEMOSEQUENCE_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_NORMAL,useGasc);

}

void Player :: PreLoad_LightAnimation( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int arcIdx = 0;
  file->LoadDataInt( &arcIdx );		
  GFL_PRINT("SeqPlayer :: Preload LightAnimation[%d]\n",arcIdx);

  bool useGasc = false;
#if PM_DEBUG
  if(mSeqViewCore->UseHIO()) useGasc = true;        
#endif

  mSeqViewCore->GetResourceManager()->LoadRequest(mSeqViewCore->GetDevHeap(),mSeqViewCore->GetDevHeap(),ARCID_DEMOSEQUENCE_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_ANIME,useGasc);

}

void Player :: PreLoad_Skybox( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int timeZone,weatherType;  
  f32 rot,scale;
  gfl2::math::Vector3 pos;

  file->LoadDataInt(&timeZone); 
  file->LoadDataInt(&weatherType);
  file->LoadDataF32( &rot );
  file->LoadDataF32( &scale );
  file->LoadDataVec3( &pos );

  GFL_PRINT("SeqPlayer :: PreloadSkybox[%d]\n",weatherType);
  mSeqViewCore->LoadSkyBox(rot);
}

void Player :: PreLoad_ParticleEffect( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int arcIdx = 0;
  file->LoadDataInt( &arcIdx );		
  GFL_PRINT("SeqPlayer :: Preload ParticleEffect[%d]\n",arcIdx);

  bool useGasc = false;
#if PM_DEBUG
  if(mSeqViewCore->UseHIO()) useGasc = true;        
#endif

  mSeqViewCore->GetResourceManager()->LoadRequest(mSeqViewCore->GetDevHeap(),mSeqViewCore->GetDevHeap(),ARCID_DEMOSEQUENCE_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT,useGasc);

}

void Player::PreLoad_BallModel( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )
{
  int ballIdx;
  file->LoadDataInt( &ballIdx );

  //0はBALLID_NULLなので注意
  int ballType = mSeqViewCore->SeqComFunc_GetTargetBall(ballIdx);
  mSeqViewCore->GetResourceManager()->LoadRequest(
    mSeqViewCore->GetDevHeap(),
    mSeqViewCore->GetDevHeap()->GetLowerHandle(),
    ARCID_CHARA_MODEL_BATTLE_NORMAL,
    CHARA_MODEL_ID_BTOB0201_00+ballType-1,
    false,
    DemoLib::Obj::RESOURCE_BINPACK,
    false);
}

	GFL_NAMESPACE_END(Sequence)
GFL_NAMESPACE_END(DemoLib)

