//======================================================================
/**
 * @file	btlv_object_chara.h
 * @brief	バトル描画 キャラモデル
 * @author	ariizumi
 * @data	15/09/03
 */
//======================================================================

#include "System/include/GflUse.h"

#include <model/include/gfl2_CharaModelFactory.h>
#include <Trainer/Trainer/include/TrainerTypeData.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include "btlv_core.h"
#include "btlv_object_chara.h"
#include "btlv_trainer_data.h"
#include "btlv_camera.h"

// モデルのインデックス
#include <niji_conv_header/trainer/trtype_def.h>
#include <niji_conv_header/battle/chara/chara_model_id.h>
#include <niji_conv_header/battle/chara/p1_base.h>  //モーション種類
#include <niji_conv_header/battle/chara/chara_model_dynamic_motion.h>  //非常駐モーション種類
#include <System/include/DressUp.h>

GFL_NAMESPACE_BEGIN(btl)


//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------

//読み込み開始
void BtlvObject_Chara::StartLoad(void)
{
  SetIsBattleModel();
  u8 clientId = GetClientID();
  mIsDressUp = false;
  mIsLoad = true;
  mTrTypeData = GFL_NEW(mBtlvCore->GetTrainerHeap(mIdx))trainer::TrainerTypeData();
#if PM_DEBUG
  BTLV_DEBUG_PARAM *debParam = mBtlvCore->GetDebugParam();

  if( debParam && debParam->mTrainerType[mIdx] != -1 )
  {
    if(debParam->mTrainerType[mIdx]==0||debParam->mTrainerType[mIdx]==1)
    {
      if( debParam->mTrainerType[mIdx]==0 )
      {
        System::DressUp::GetDressUpParamDefaultHero(&mTempDressParam);
        mTrainerType = trainer::TRTYPE_HERO;
        mTrTypeData->LoadData(mBtlvCore->GetTrainerHeap(mIdx),trainer::TRTYPE_HERO);
        SetCharaModelId(CHARA_MODEL_ID_P1_BASE);
      }
      else
      {
        System::DressUp::GetDressUpParamDefaultHeroine(&mTempDressParam);
        mTrainerType = trainer::TRTYPE_HEROINE;
        mTrTypeData->LoadData(mBtlvCore->GetTrainerHeap(mIdx),trainer::TRTYPE_HEROINE);
        SetCharaModelId(CHARA_MODEL_ID_P2_BASE);
      }

      mIsDressUp = true;
    }
    else
    if(debParam->mTrainerType[mIdx]==-2)
    {
      const Savedata::MyStatus* myStatus = mBtlvCore->GetClientPlayerData( 0 );
      mTempDressParam = myStatus->GetDressup();

      // @fix MMCat[337]nijiの場合は、バングルをnijiのに上書きする
      {
        if( myStatus->IsDressUpParamRomVersionNiji() )
        {
          System::DressUpParamSaveData::OverrideNijiBngl( &mTempDressParam );
        }
      }

      if( myStatus->GetSex() == PM_MALE )
      {
        mTrTypeData->LoadData(mBtlvCore->GetTrainerHeap(mIdx),trainer::TRTYPE_HERO);
        mTrainerType = trainer::TRTYPE_HERO;
        SetCharaModelId(CHARA_MODEL_ID_P1_BASE);
      }
      else
      {
        mTrTypeData->LoadData(mBtlvCore->GetTrainerHeap(mIdx),trainer::TRTYPE_HEROINE);
        mTrainerType = trainer::TRTYPE_HEROINE;
        SetCharaModelId(CHARA_MODEL_ID_P2_BASE);
      }

      mIsDressUp = true;
    }
    else
    {
      mTrainerType = trainer::TRTYPE_MAX;
      for( int i=0;i<trainer::TRTYPE_MAX;i++ )
      {
        mTrTypeData->LoadData(mBtlvCore->GetTrainerHeap(mIdx),(trainer::TrType)i); 
        if( mTrTypeData->GetBattleMainModelID() == debParam->mTrainerType[mIdx] )
        {
          GFL_PRINT("DebugTrainer Type is mutch! idx[%d] charaId[%d] trType[%d]\n",mIdx,debParam->mTrainerType[mIdx],i);
          mTrainerType = i;
          break;
        }
      }
      if( mTrainerType == trainer::TRTYPE_MAX )
      {
        GFL_PRINT("DebugTrainer Type not found! idx[%d] charaId[%d]\n",mIdx,debParam->mTrainerType[mIdx]);
        //ダミー
        mTrainerType = trainer::TRTYPE_HEROINE;
        mTrTypeData->LoadData(mBtlvCore->GetTrainerHeap(mIdx),trainer::TRTYPE_HEROINE);
      }
      SetCharaModelId(debParam->mTrainerType[mIdx]);

      mIsDressUp = false;
    }
  }
  else
#endif
  if( mBtlvCore->GetBtlvMode() == BTLV_MODE_CAPTURE && mIdx == 0 )
  {
    //捕獲チュートリアル
    //プレイヤーは博士
    if( mBtlvCore->GetIsZenryokuTutorial() == false )
    {
      //リング無し
      SetCharaModelId(CHARA_MODEL_ID_BTTR0004_00);
    }
    else
    {
      //リング有
      SetCharaModelId(CHARA_MODEL_ID_BTTR0004_00_OB0021_00);
    }
  }
  else
    if (mBtlvCore->GetClientPlayerData(clientId, mBtlFesIdx))
  {
    const Savedata::MyStatus* myStatus = mBtlvCore->GetClientPlayerData(clientId, mBtlFesIdx);

    mTempDressParam = myStatus->GetDressup();

    // @fix MMCat[337]nijiの場合は、バングルをnijiのに上書きする
    {
      if( myStatus->IsDressUpParamRomVersionNiji() )
      {
        System::DressUpParamSaveData::OverrideNijiBngl( &mTempDressParam );
      }
    }

    if( mBtlvCore->GetSetupStatusFlag(BTL_STATUS_FLAG_RIDEWARE_LAND) )
    {
      System::DressUp::OverwrideDressUpParamRide(myStatus->GetSex(),true,&mTempDressParam );
    }
    else
    if( mBtlvCore->GetSetupStatusFlag(BTL_STATUS_FLAG_RIDEWARE_SWIM) )
    {
      System::DressUp::OverwrideDressUpParamRide(myStatus->GetSex(),false,&mTempDressParam );
    }

    if( myStatus->GetSex() == PM_MALE )
    {
      mTrTypeData->LoadData(mBtlvCore->GetTrainerHeap(mIdx),trainer::TRTYPE_HERO);
      mTrainerType = trainer::TRTYPE_HERO;
      SetCharaModelId(CHARA_MODEL_ID_P1_BASE);
    }
    else
    {
      mTrTypeData->LoadData(mBtlvCore->GetTrainerHeap(mIdx),trainer::TRTYPE_HEROINE);
      mTrainerType = trainer::TRTYPE_HEROINE;
      SetCharaModelId(CHARA_MODEL_ID_P2_BASE);
    }

    mIsDressUp = true;
  }
  else
  {
    const u16 trType = mBtlvCore->GetTrainerType(clientId);

    mTrainerType = trType;
    mTrTypeData->LoadData(mBtlvCore->GetTrainerHeap(mIdx),static_cast<trainer::TrType>(trType));

    SetCharaModelId(mTrTypeData->GetBattleMainModelID());
  }

  if( GetCharaModelId() == 0xFFFF )
  {
    mTrainerType = trainer::TRTYPE_MINI;
    SetCharaModelId(CHARA_MODEL_ID_BTTR0078_00);
  }

  //ここは共通リソースがあれば共通になるのでトレーナー共通領域から取っておく 
  mBtlvCore->GetCharaModelFactory()->LoadModelAsync(mBtlvCore->GetTrainerCommonHeap(),GetCharaModelId());
  if( mIsDressUp )
  {
    mDynamicAnimeType = 0;
    //非常駐アニメのID取得
#if PM_DEBUG
    BTLV_DEBUG_PARAM *debParam = mBtlvCore->GetDebugParam();
    if( debParam )
    {
      if(debParam->mTrainerType[mIdx]==0||debParam->mTrainerType[mIdx]==1)
      {
        if( debParam->mTrainerType[mIdx]==0 )
        {
          if (GetThrowType() == 8)
          {
            //momiji追加のためIndexが飛んでる
            mDynamicAnimeType = BTPC0001_00_SELECT9;
          }
          else
          { 
            mDynamicAnimeType = BTPC0001_00_SELECT1 + GetThrowType() * 0x00010000;
          }
        }
        else
        {
          if (GetThrowType() == 8)
          {
            //momiji追加のためIndexが飛んでる
            mDynamicAnimeType = BTPC0002_00_SELECT9;
          }
          else
          {
            mDynamicAnimeType = BTPC0002_00_SELECT1 + GetThrowType() * 0x00010000;
          }
        }
      }
      else
      if(debParam->mTrainerType[mIdx]==-2)
      {
        const Savedata::MyStatus* myStatus = mBtlvCore->GetClientPlayerData(0, mBtlFesIdx);
        if( myStatus )
        {
          if( myStatus->GetSex() == PM_MALE )
          {
            if (GetThrowType() == 8)
            {
              //momiji追加のためIndexが飛んでる
              mDynamicAnimeType = BTPC0001_00_SELECT9;
            }
            else
            {
              mDynamicAnimeType = BTPC0001_00_SELECT1 + GetThrowType() * 0x00010000;
            }
          }
          else
          {
            if (GetThrowType() == 8)
            {
              //momiji追加のためIndexが飛んでる
              mDynamicAnimeType = BTPC0002_00_SELECT9;
            }
            else
            {
              mDynamicAnimeType = BTPC0002_00_SELECT1 + GetThrowType() * 0x00010000;
            }
          }
        }
      }
    }
#endif
    const Savedata::MyStatus* myStatus = mBtlvCore->GetClientPlayerData(GetClientID(), mBtlFesIdx);
    if( myStatus )
    {
      if( myStatus->GetSex() == PM_MALE )
      {
        if (GetThrowType() == 8)
        {
          //momiji追加のためIndexが飛んでる
          mDynamicAnimeType = BTPC0001_00_SELECT9;
        }
        else
        {
          mDynamicAnimeType = BTPC0001_00_SELECT1 + GetThrowType() * 0x00010000;
        }
      }
      else
      {
        if (GetThrowType() == 8)
        {
          //momiji追加のためIndexが飛んでる
          mDynamicAnimeType = BTPC0002_00_SELECT9;
        }
        else
        {
          mDynamicAnimeType = BTPC0002_00_SELECT1 + GetThrowType() * 0x00010000;
        }
      }
    }

    //来るはずはないが一応チェック
    GFL_ASSERT_MSG(mDynamicAnimeType!=0,"TrainetDynamicAnimeTypeError!\n"); //@check 開発中に気づくはず

    mDressUpModelResourceManager = GFL_NEW(mBtlvCore->GetTrainerHeap(mIdx))poke_3d::model::DressUpModelResourceManager();
    mDressUpModelResourceManager->InitializeAsync(mBtlvCore->GetAsyncFileManager(),mBtlvCore->GetTrainerHeap(mIdx),System::DressUp::GetDressUpArcIdListBattle());
    
    mIsSetupDressMnager = false;
  }
  else
  {
    mDressUpModelResourceManager = NULL;
    mIsSetupDressMnager = true;
    mDynamicAnimeType = 0;
  }

  mTrainerData = mBtlvCore->getTrainerExtraInfo(GetCharaModelId());

  //サブオブジェの読み込み
  {
    mAttachModelId = -1;
    if( mTrainerData )
    {
      if( mTrainerData->mAccessoryID != -1 )
      {
        mAttachModelId = mTrainerData->mAccessoryID;
        mBtlvCore->GetCharaModelFactory()->LoadModelAsync(mBtlvCore->GetTrainerCommonHeap(),mAttachModelId);
        GFL_PRINT("[%d]\n",mAttachModelId);
      }
    }
  }
}

bool BtlvObject_Chara::IsFinishLoad(void)
{
  if( mIsLoad == false )
  {
    return true;
  }
  if( mBtlvCore->GetCharaModelFactory()->IsModelLoaded(GetCharaModelId()) == false )
  {
    return false;
  }
  if( mIsDressUp == true  )
  {
    if( mIsSetupDressMnager == false )
    {
      if( mDressUpModelResourceManager->WaitInitializeAsync() )
      {
        mDressUpModelResourceManager->LoadDressUpPartsAsync(mTempDressParam);

        //非常駐の投げモーションの読み込み
        mBtlvCore->GetCharaModelFactory()->LoadDynamicAnimationAsync(GetDynamicAnimeType());
        mDressUpModelResourceManager->LoadDressUpDynamicAnimationAsync(mTempDressParam,GetDynamicAnimeType());

        mIsSetupDressMnager = true;
      }
      //ここはどっちにしても抜けておく。
      return false;
    }
    else
    {
      if( mDressUpModelResourceManager->IsDressUpPartsLoaded(mTempDressParam) == false ||
          mBtlvCore->GetCharaModelFactory()->IsDynamicAnimationLoaded(GetDynamicAnimeType()) == false ||
          mDressUpModelResourceManager->IsDressUpDynamicAnimationLoaded(mTempDressParam,GetDynamicAnimeType())== false )
      {
        return false;
      }
    }
  }
  //サブオブジェ
  if( mAttachModelId != -1 )
  {
    if( mBtlvCore->GetCharaModelFactory()->IsModelLoaded(mAttachModelId) == false )
    {
      return false;
    }
  }
  return true;
}

bool BtlvObject_Chara::AsyncSetup(void)
{
  if( mIsLoad == false )
  {
    return true;
  }
  if( mIsDressUp == false )
  {
    return true;
  }

  switch(mAsyncSetupStep)
  {
  case 0:
    mDressUpModelResourceManager->SetupDressUpPartsAsync( mBtlvCore->GetTrainerGLAllocator(mIdx) , mTempDressParam  );
    mAsyncSetupStep++;
    return false;
    GFL_WARNING_PUSH  //この文で#pragma設定を退避
    GFL_WARNING_CTR_IGNORE(111)  //#111-Dを抑止
    break;
    GFL_WARNING_POP  //この文で#pragma設定を回復
  case 1:
    {
      bool ret = mDressUpModelResourceManager->WaitSetupDressUpPartsAsync( mBtlvCore->GetTrainerGLAllocator(mIdx) , mTempDressParam  );
      if( ret == false )
      {
        return false;
      }
      mAsyncSetupStep++;
    }
    //break; //through
  case 2:
    return true;
  }
  return false;
}

void BtlvObject_Chara::CreateModel(void)
{
  if( mIsLoad == false )
  {
    return;
  }

  mBtlvCore->GetCharaModelFactory()->SetupModel(mBtlvCore->GetTrainerGLAllocator(mIdx),GetCharaModelId());
  if( mIsDressUp == true )
  {
    if( mAsyncSetupStep == 0 )
    {
      mDressUpModelResourceManager->SetupDressUpParts( mBtlvCore->GetTrainerGLAllocator(mIdx) , mTempDressParam  );
    }
    mBtlvCore->GetCharaModelFactory()->SetupDynamicAnimation(mBtlvCore->GetTrainerGLAllocator(mIdx) ,mBtlvCore->GetTrainerHeap(mIdx), GetDynamicAnimeType());
    mDressUpModelResourceManager->SetupDressUpDynamicAnimation(mBtlvCore->GetTrainerGLAllocator(mIdx),mBtlvCore->GetTrainerHeap(mIdx) ,mTempDressParam,GetDynamicAnimeType());
  }

  {
    DemoLib::Obj::Object_Chara::LOAD_PARAM param;
    param.charaId = GetCharaModelId();
    if( mIsDressUp == true )
    {
      param.dressParam = &mTempDressParam;
    }
    if( mAttachModelId != -1 )
    {
      param.attachModelId = mAttachModelId;
      gfl2::std::MemCopy(mTrainerData->mJointStr,param.attachJoint,32);
      mBtlvCore->GetCharaModelFactory()->SetupModel(mBtlvCore->GetTrainerGLAllocator(mIdx),mAttachModelId);
    }
    SetDressUpResouceManager(mDressUpModelResourceManager);
    CreateNoLoad( param , mBtlvCore->GetTrainerHeap(mIdx) , mBtlvCore->GetTrainerGLAllocator(mIdx) );
  }

  //影モデル作成
  {
    DemoLib::Obj::Object_Chara::LOAD_PARAM param;
    param.charaId = CHARA_MODEL_ID_BTOB0401_00;
    param.dressParam = NULL;
    mShadowModel.CreateNoLoad( param , mBtlvCore->GetTrainerHeap(mIdx) , mBtlvCore->GetTrainerGLAllocator(mIdx) );
    GetCharaModel()->SetupRandomFuc(System::GflUse::GetPublicRand);
    mShadowModel.SetScale(mTrainerData->mShadowScale);

    int matNum = mShadowModel.GetBaseModel()->GetMaterialCount();
    for( int i=0;i<matNum;i++ )
    {
      //影の描画順更新
#if defined(GF_PLATFORM_CTR)
      mShadowModel.GetBaseModel()->GetModelInstanceNode()->CopyMaterialDepthStencilStateToStateObject();
#endif
      mShadowModel.GetBaseModel()->SetMaterialDepthTestFunction( i, poke_3d::model::BaseModel::MATERIAL_TEST_FUNCTION_ALWAYS );
      mShadowModel.GetBaseModel()->GetModelInstanceNode()->GetMaterialInstanceNode(i)->GetDepthStencilStateObject()->UpdateState();
      mShadowModel.GetBaseModel()->GetModelInstanceNode()->SetUpDrawTag();
    }

  }
  
  //ライト番号の変更
  {
    {
      int matNum = GetCharaModel()->GetMaterialCount();
      for( int i=0;i<matNum;i++ )
      {
        GetCharaModel()->SetMaterialLightSetIndex(i,LIGHTNO_CHARA_TOP+mIdx);
      }
    }
    if( GetDressUpModel() )
    {
      for( int i=0;i<poke_3d::model::DressUpModel::PARTS_MODEL_COUNT;i++ )
      {
        poke_3d::model::BaseModel *partsModel = GetDressUpModel()->GetPartsModel((poke_3d::model::DressUpModel::PartsModel)i);
        if( partsModel )
        {
          int matNum = partsModel->GetMaterialCount();
          for( int mat=0;mat<matNum;mat++ )
          {
            partsModel->SetMaterialLightSetIndex(mat,LIGHTNO_CHARA_TOP+mIdx);
          }
        }
      }
    }
    if( GetAttachModel() )
    {
      int matNum = GetAttachModel()->GetMaterialCount();
      for( int i=0;i<matNum;i++ )
      {
        GetAttachModel()->SetMaterialLightSetIndex(i,LIGHTNO_CHARA_TOP+mIdx);
      }
    }
  }
}

void BtlvObject_Chara::ChangeAnimationRand( u32 animeIdx , bool isRand )
{
  if( IsEnable() == false )
  {
    return;
  }
  //GFMCat[399] マルチ投げモーションは主人公と位置あわせのため25ほど原点からずれ居ている。
  //nijiでは見方側はカメラが高く足元が見えず、敵側はball1を使っていたため問題なかったが、
  //投げ前に止まっているのを解消するためmomijiでマルチ投げに変更した。そのためmomijiで問題が発覚した。
  if (mIsDressUp == false)
  {
    if (animeIdx == TRAINER_ANIME_MULTIBALL00 ||
      animeIdx == TRAINER_ANIME_MULTIBALL01)
    {
      if (mIdx == BTL_VPOS_NEAR_1 || mIdx == BTL_VPOS_NEAR_2)
      {
        mShadowModel.SetOffset(0, 0, 25.0f);
      }
      else
      {
        mShadowModel.SetOffset(0, 0, -25.0f);
      }
    }
    else
    {
      mShadowModel.SetOffset(0, 0, 0);
    }
  }

  mIsPlayResourceMotion = false;
  if( animeIdx == TRAINER_ANIME_WAIT1 ||
    animeIdx == TRAINER_ANIME_SELECT_WAIT1 ||
    animeIdx == TRAINER_ANIME_SELECT_WAIT2 ||
    animeIdx == TRAINER_ANIME_SELECT_WAIT3 ||
    //animeIdx == TRAINER_ANIME_SELECT_WAIT4 || //@attention このモーションは自動目パチしない！
    animeIdx == TRAINER_ANIME_SELECT_WAIT5 ||
    animeIdx == TRAINER_ANIME_SELECT_WAIT6 ||
    animeIdx == TRAINER_ANIME_SELECT_WAIT7 ||
    animeIdx == TRAINER_ANIME_SELECT_WAIT8 ||
    animeIdx == TRAINER_ANIME_SELECT_WAIT9 )
  {
    SetAutoBlink(true);
  }
  else
  {
    SetAutoBlink(false);
  }
  DemoLib::Obj::Object_Chara::ChangeAnimation(animeIdx);
  //モーションの開始位置をずらす
  if( isRand )
  {
    const f32 maxFrame = GetBaseModel()->GetAnimationEndFrame();
    if( maxFrame != 0 )
    {
      const s32 frame = System::GflUse::GetPublicRand((u32)maxFrame);
      SetAnimationFrame((f32)frame);
    }
  }
}

void BtlvObject_Chara::ChangeAnimationResource( gfl2::renderingengine::scenegraph::resource::ResourceNode *anmRes,int aniSlot )
{
  if( anmRes != NULL )
  {
    mIsPlayResourceMotion = true;
  }
  else
  {
    mIsPlayResourceMotion = false;
  }
  DemoLib::Obj::Object_Chara::ChangeAnimationResource(anmRes,aniSlot);
}
void BtlvObject_Chara::ChangeAnimationResourceDressUp( void *buf, gfl2::gfx::IGLAllocator *allocator  )
{
  if( buf != NULL )
  {
    mIsPlayResourceMotion = true;
  }
  else
  {
    mIsPlayResourceMotion = false;
  }
  DemoLib::Obj::Object_Chara::ChangeAnimationResourceDressUp(buf,allocator);
}

bool BtlvObject_Chara::IsFinishWaitB(void)
{
  //@ todo ちゃんとチェック
  return true;
}
//IsEnableはチェック済み。
//毎フレーム呼ばれる
void BtlvObject_Chara::UpdateAlways(int step)
{
  DemoLib::Obj::Object_Chara::UpdateAlways(step);
  if( IsFinishAnimation() && mIsAutoResetAnime )
  {
    if( mEnableWaitB )
    {
      if( System::GflUse::GetPublicRand(100) < 5 )
      {
        ChangeAnimation(TRAINER_ANIME_WAIT2);
      }
      else
      {
        ChangeAnimation(TRAINER_ANIME_WAIT1);
      }
    }
    else
    {
      ChangeAnimation(TRAINER_ANIME_WAIT1);
    }
  }
  GetCharaModel()->UpdateEye();
}

int BtlvObject_Chara::GetThrowType(void)
{
  u8 clientId = GetClientID();
  const Savedata::MyStatus* myStatus = mBtlvCore->GetClientPlayerData(clientId, mBtlFesIdx);
  if( myStatus )
  {
#if PM_DEBUG
    BTLV_DEBUG_PARAM *debParam = mBtlvCore->GetDebugParam();

    if( debParam && debParam->mTrainerThrowType != -1 )
    {
      return debParam->mTrainerThrowType;
    }
#endif  //PM_DEBUG
    return myStatus->GetBallThrowType();
  }

  return 0;
}
//非常駐アニメのID取得
int BtlvObject_Chara::GetDynamicAnimeType(void)
{
  //開放時にMyStatsuが無いことがあったので変数化
  return mDynamicAnimeType;
}


//影のための継承用
void BtlvObject_Chara::SetPosition(const gfl2::math::Vector3 pos )
{
  DemoLib::Obj::Object_Chara::SetPosition(pos);
  gfl2::math::Vector3 tempPos = pos;
  mShadowModel.SetPosition(pos);
}
void BtlvObject_Chara::SetPosition(const f32 x,const f32 y,const f32 z)
{
  DemoLib::Obj::Object_Chara::SetPosition(x,y,z);
  mShadowModel.SetPosition(x,y,z);
}
void BtlvObject_Chara::Delete(void)
{
  DemoLib::Obj::Object_Chara::Delete();
  mShadowModel.Delete();
  GFL_SAFE_DELETE(mTrTypeData);

  if( mDressUpModelResourceManager )
  {
    mBtlvCore->GetCharaModelFactory()->UnloadDynamicAnimation(GetDynamicAnimeType());
    mDressUpModelResourceManager->UnloadDressUpDynamicAnimation(mTempDressParam,GetDynamicAnimeType());
    mDressUpModelResourceManager->UnloadDressUpPartsAll();
    mDressUpModelResourceManager->Finalize();
    GFL_SAFE_DELETE(mDressUpModelResourceManager);
  }
}

void BtlvObject_Chara::SetVisible(const bool flg)
{
  mIsVisibleChara = flg;
  UpdateVisible();
  mShadowModel.SetVisible(flg);
}

void BtlvObject_Chara::SetShadowVisible(const bool flg)
{
  mShadowModel.SetVisible(flg);
}
bool BtlvObject_Chara::GetVisible(void)const
{
  return mIsVisibleChara;
}
//表示状態の更新
void BtlvObject_Chara::UpdateVisible(void)
{
  if( mIsVisibleChara && mCamHitErace == false)
  {
    DemoLib::Obj::Object_Chara::SetVisible(true);
  }
  else
  {
    DemoLib::Obj::Object_Chara::SetVisible(false);
  }
}
  

//ObjectRenderPipeLine経由で追加する
void BtlvObject_Chara::AddRenderPath(int opt )
{
  DemoLib::Obj::Object_Chara::AddRenderPath(opt);
  mShadowModel.AddRenderPath(BattleRenderingPipeLine::RENDER_OPT_SHADOW_MODEL);
}
void BtlvObject_Chara::RemoveRenderPath(void)
{
  DemoLib::Obj::Object_Chara::RemoveRenderPath();
  mShadowModel.RemoveRenderPath();
}

//通信時子だとClientIDが若干変わるので
u8 BtlvObject_Chara::GetClientID(void)
{
  return mBtlvCore->GetBtlvPosToClientId( static_cast<BtlvPos>(mIdx) );
}

void BtlvObject_Chara::SetAutoBlink(bool flg)
{
  if( IsEnable() )
  {
    if( flg == true )
    {
      GetCharaModel()->SetAutoBlinkMode(poke_3d::model::CharaModel::AUTO_BLINK_MODE_BATTLE);
    }
    else
    {
      GetCharaModel()->SetAutoBlinkMode(poke_3d::model::CharaModel::AUTO_BLINK_MODE_NONE);
    }
  }
}

//カメラとの当たり判定用
bool BtlvObject_Chara::CheckHitCamera(const gfl2::math::Vector3 &pos,f32 ofsSize)
{
  const TrainerExtraInfoStruct *data =  GetTrainerData();
  if( data->mMinY == 0 && data->mMaxY == 0 )
  {
    return false;
  }

  gfl2::math::AABB box;
  gfl2::math::Vector4 maxVec(data->mMaxX,data->mMaxY,data->mMaxZ,0.0f);
  gfl2::math::Vector4 minVec(data->mMinX,data->mMinY,data->mMinZ,0.0f);
  box.SetMax(maxVec);
  box.SetMin(minVec);

  return CheckHitCollision(pos,ofsSize,this,box);
}


void BtlvObject_Chara::UpdateCulling(void)
{
  //カメラめり込みチェック
  if( IsEnable() )
  {
    bool isHit = false;
    bool isCulling = false;

    //コリジョンが待機A準拠なので、待機ABでしか判定しない。
    if( mIsPlayResourceMotion == false &&
        (GetAnimationIndex() == P1_BASE_BA005_WAIT01 ||
        GetAnimationIndex() == P1_BASE_BA006_WAIT02 ||
        GetAnimationIndex() == P1_BASE_BA003_ORDER01 )) //MNCat3982 指示だし中も有効にする
    {
      gfl2::math::Vector3 camPos = mBtlvCore->GetCameraSys()->GetPositionAnime();
      f32 nearVal = mBtlvCore->GetCameraSys()->GetNearAnime_NMCat4000fix();    

      isHit = CheckHitCamera(camPos,nearVal);

      //画面外判定
      {
        // カリング用行列の取得
        gfl2::math::Matrix44 *cullingMatrixTable = mBtlvCore->GetRenderSys()->GetFrustumCullingTable();
        const TrainerExtraInfoStruct *data =  GetTrainerData();
        if( data->mMinY != 0 || data->mMaxY != 0 )
        {
          gfl2::math::AABB box;
          gfl2::math::Vector4 maxVec(data->mMaxX,data->mMaxY,data->mMaxZ,0.0f);
          gfl2::math::Vector4 minVec(data->mMinX,data->mMinY,data->mMinZ,0.0f);
          box.SetMax(maxVec);
          box.SetMin(minVec);

          //回転の加味ができないので、最大サイズのコリジョンにしてしまう。
          CalcBoxMaxSize(box);
          box = box.Offset(GetPosition());
          if( gfl2::math::AABB::Util::IsIntersectFrustum( cullingMatrixTable, box ) == false )
          {
            isCulling = true;
          }
        }
      }
    }

    if( (isHit || isCulling) != mCamHitErace )
    {
      if( (isHit || isCulling) )
      {
        ARIIZUMI_PRINT("TrainerCamHit ON![%d][%d:%d]\n",mIdx,isHit,isCulling);
        mCamHitErace = true;
        UpdateVisible();
      }
      else
      {
        ARIIZUMI_PRINT("TrainerCamHit OFF![%d]\n",mIdx);
        mCamHitErace = false;
        UpdateVisible();
      }
    }
  }  
}

void BtlvObject_Chara::UpdatePositionForce(void)
{
  //ノード座標取得の更新のために、座標を強制更新する
  //アタッチモデルの速度が継承されていなかったため、セットは用意された関数を使う。
  f32 spd = GetCharaModel()->GetAnimationStepFrame();
  SetAnimationSpeed(0);
  UpdateCore(0);
  UpdateAlwaysAfterCore(0);
  SetAnimationSpeed(spd);
}

#if PM_DEBUG
void BtlvObject_Chara::ReloadData(void)
{
  mTrainerData = mBtlvCore->getTrainerExtraInfo(GetCharaModelId());
}
#endif

GFL_NAMESPACE_END(btl)


