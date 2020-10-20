//======================================================================
/**
 * @file  AriTestBtlv.cpp
 * @brief 15/03/13
 * @author  ariizumi
 * @data  デバッグ用BattleViewSystem
 */
//======================================================================
#if PM_DEBUG
#include <debug/include/gfl2_DebugWinSystem.h>
#include <debug/include/gfl2_DebugWinItemTypes.h>
#include <Battle/include/battle_DefaultPowerUp.h>
#include <Battle/source/btl_common.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include "Test/AriizumiTest/include/AriTestBtlv.h"

#include <arc_index/btlv_resource.gaix>
#include <niji_conv_header/battle/battle_data_pack.h>

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Ariizumi)

  DebugBattleViewSystem::DebugBattleViewSystem()
  :btl::BattleViewSystem()
{
  mStartInitialize = false;

  mBtlRule = BTL_RULE_SINGLE;
}

DebugBattleViewSystem::~DebugBattleViewSystem()
{
  for( int i=0;i<4;i++ )
    {
      GFL_SAFE_DELETE(mBpp[i]);
    }
}

//システム初期化
bool DebugBattleViewSystem::Initialize(gfl2::heap::HeapBase* debugHeap,gfl2::heap::HeapBase* btlvHeap )
{
  if( mStartInitialize == false )
  {
    //ここは１回で良い。
    DefaultPowerUpDesc defaultPowerUpDesc;
    DEFAULT_POWERUP_DESC_Clear( &defaultPowerUpDesc );

    for( int i=0;i<4;i++ )
    {
      mPp[i] = GetDebugParam()->pp[i];
      mBpp[i] = GFL_NEW(debugHeap)btl::BTL_POKEPARAM();

      btl::BTL_POKEPARAM::SetupParam setupParam;
      setupParam.srcParam    = mPp[i];
      setupParam.pokeID      = i;
      setupParam.friendship  = 0;
      setupParam.fieldStatus = NULL;
      DEFAULT_POWERUP_DESC_Clear( &setupParam.defaultPowerUpDesc );

      mBpp[i]->Setup( setupParam );
    }
    if (GetDebugParam()->mEnebleBgSetting)
    {
      mFiledSituation.bgComponent.groudnType = GetDebugParam()->mBgComponent.groudnType;
      mFiledSituation.bgComponent.nearType = GetDebugParam()->mBgComponent.nearType;
      mFiledSituation.bgComponent.farType = GetDebugParam()->mBgComponent.farType;
      mFiledSituation.bgComponent.useSkyBox = GetDebugParam()->mBgComponent.useSkyBox;
    }
    else
    {
      mFiledSituation.bgComponent.groudnType = btl::bg::PLATE_BUSH1;
      mFiledSituation.bgComponent.nearType = btl::bg::NEAR_GRASS;
      mFiledSituation.bgComponent.farType = btl::bg::FAR_MOUNT;
      mFiledSituation.bgComponent.useSkyBox = true;
    }
    mFiledSituation.sky_type = System::Skybox::Skybox::SKY_TYPE_SUNNY;
    mFiledSituation.weather = BTL_WEATHER_NONE;

    mStartInitialize = true;
  }

  return btl::BattleViewSystem::Initialize(btlvHeap);
}

bool DebugBattleViewSystem::IsPokeExist( btl::BtlvPos pos ) const
{
  switch(GetBattleRule())
  {
  case BTL_RULE_SINGLE:
    if( pos == btl::BTL_VPOS_NEAR_1 ||
        pos == btl::BTL_VPOS_FAR_1 )
    {
      return true;
    }
    break;
  case BTL_RULE_INTRUDE:
    if( pos != btl::BTL_VPOS_NEAR_2 )
    {
      return true;
    }
    break;
  case BTL_RULE_DOUBLE:
  case BTL_RULE_ROYAL:
    return true;
    //break;
  }
  return false;
}

//自分側？
bool DebugBattleViewSystem::IsPlayerSide(btl::BtlvPos pos )
{
  if( GetBattleRule() != BTL_RULE_ROYAL )
  {
    if( pos % 2 == 0 )
    {
      return true;
    }
  }
  else
  {
    if( pos == btl::BTL_VPOS_NEAR_1 )
    {
      return true;
    }
  }
  return false;
}

const Savedata::MyStatus* DebugBattleViewSystem::GetClientPlayerData(u8 clientID, int btlFesIdx) const
{
  if( clientID == 0 )
  {
    const Savedata::MyStatus* myStatus = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatusConst();
    return myStatus;
  }
  return NULL;
}

void DebugBattleViewSystem::GetStringUi(gfl2::str::StrBuf* dst, u16 strID )
{
  dst->SetStr(L"仮\n");
}


void DebugBattleViewSystem::InitEffectViewerDebugMenu(gfl2::heap::HeapBase* debugHeap)
{
  gfl2::debug::DebugWinGroup *topGroup = gfl2::debug::DebugWin_AddGroup(debugHeap,L"EffectViewer",NULL);

  gfl2::debug::DebugWinItem* item;
  item = gfl2::debug::DebugWin_AddItemValueInt(topGroup,debugHeap,L"Target1",&mTempComTargetPos[0],0,3);
  item->SetChangedValueFunc(DebugBattleViewSystem::TestBtlvDebug_TargetCallBack,this);
  item = gfl2::debug::DebugWin_AddItemValueInt(topGroup,debugHeap,L"Target2",&mTempComTargetPos[1],0,3);
  item->SetChangedValueFunc(DebugBattleViewSystem::TestBtlvDebug_TargetCallBack,this);

  item = gfl2::debug::DebugWin_AddItemValueInt(topGroup,debugHeap,L"TargetTr1",&mTargetTrainer[0],0,3);
  item = gfl2::debug::DebugWin_AddItemValueInt(topGroup,debugHeap,L"TargetTr2",&mTargetTrainer[1],0,3);

  item = gfl2::debug::DebugWin_AddItemValueInt(topGroup,debugHeap,L"EffBall1",&mEffectBallId[0],0,25);
  item = gfl2::debug::DebugWin_AddItemValueInt(topGroup,debugHeap,L"EffBall2",&mEffectBallId[1],0,25);

  SetComTargetReset();
  mComTargetPoke[0] = btl::BTL_VPOS_NEAR_1;
  mComTargetPoke[1] = btl::BTL_VPOS_FAR_1;
  mComTargetDamage[1] = btl::BTL_VPOS_FAR_1;
  mTargetTrainer[0] = btl::BTL_VPOS_NEAR_1;
  mTargetTrainer[1] = btl::BTL_VPOS_FAR_1;
  mEffectBallId[0] = 4; //モンスターボール
  mEffectBallId[1] = 4; //モンスターボール
}

void DebugBattleViewSystem::TermEffectViewerDebugMenu(void)
{
  gfl2::debug::DebugWin_RemoveGroupByName(L"EffectViewer");
}

void DebugBattleViewSystem::TestBtlvDebug_TargetCallBack( void *work )
{
  DebugBattleViewSystem *core =  static_cast<DebugBattleViewSystem*>(work);

  core->mComTargetPoke[0] = btl::BtlvPos(core->mTempComTargetPos[0]);
  core->mComTargetPoke[1] = btl::BtlvPos(core->mTempComTargetPos[1]);
  core->mComTargetDamage[0] = btl::BtlvPos(core->mTempComTargetPos[0]);

}

void DebugBattleViewSystem::ChangeRule( BtlRule rule )
{

  // Visible Off
  for( int i=0;i<POKEMODEL_NUM;i++ )
    {
      GetPokeModel((btl::BtlvPos)i)->SetVisible( false );
      //    GetPokeModel((btl::BtlvPos)i)->RemoveScene();
    }

  for( int i=0;i<TRAINERMODEL_NUM;i++ )
    {
      GetTrainerModelObj(i)->SetVisible( false );
    }


  // Set battle rule
  mBtlRule = rule;

  // Visible On
  switch( mBtlRule )
    {
    case BTL_RULE_SINGLE:
      {
	GetPokeModel((btl::BtlvPos)0)->SetVisible( true );
	GetPokeModel((btl::BtlvPos)1)->SetVisible( true );

	GetTrainerModelObj(0)->SetVisible( true );
	GetTrainerModelObj(1)->SetVisible( true );
      }
      break;

    case BTL_RULE_INTRUDE:
      {
	GetPokeModel((btl::BtlvPos)0)->SetVisible( true );
	GetPokeModel((btl::BtlvPos)1)->SetVisible( true );
	GetPokeModel((btl::BtlvPos)3)->SetVisible( true );

	GetTrainerModelObj(0)->SetVisible( true );

      }
      break;

    case BTL_RULE_DOUBLE:
    case BTL_RULE_ROYAL:
      {
        for( int i=0;i<POKEMODEL_NUM;i++ )
          {
            GetPokeModel((btl::BtlvPos)i)->SetVisible( true );
            //    GetPokeModel((btl::BtlvPos)i)->AddScene();
          }

	for( int i=0;i<TRAINERMODEL_NUM;i++ )
	  {
	    GetTrainerModelObj(i)->SetVisible( true );
	  }
	
      }
      break;
    }


  SetDefaultPokePos();
  SetDefaultTrainerPos();
  //  SetupObonParam();
  ResetDefaultCamera(0);

}

void DebugBattleViewSystem::SetDefaultPokePos(void)
{

  gfl2::math::Vector3 pos;
  int deg;

  for( int i=0;i<POKEMODEL_NUM;i++ )
    {
      GetDefaultPokePos( (btl::BtlvPos)i, &pos , &deg );

      GetPokeModel((btl::BtlvPos)i)->SetPosition ( pos );
      GetPokeModel((btl::BtlvPos)i)->SetRotateY ( (f32)deg );
    }

}

void DebugBattleViewSystem::SetDefaultTrainerPos(void)
{

  gfl2::math::Vector3 pos;
  int deg;

  for( int i=0 ;i<TRAINERMODEL_NUM; i++ )
    {
      GetDefaultTrainerPos( i, &pos , &deg );

      GetTrainerModelObj(i)->SetPosition ( pos );
      GetTrainerModelObj(i)->SetRotateY ( (f32)deg );
    }

}

void DebugBattleViewSystem::ReloadBattleData(void)
{
  GflHeapSafeFreeMemory(mBattleDataPackBuf);

  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
  req.arcId = ARCID_BTLV_RESOURCE;
  req.datId = GARC_btlv_resource_battle_data_pack_BIN;
  req.align = 128;
  req.heapForBuf = GetResidentHeap();
  req.heapForReq = GetTempHeap()->GetLowerHandle();
  req.ppBuf = &mBattleDataPackBuf;

#if PM_DEBUG
  if( GetDebugParam() )
  {
    if( GetDebugParam()->isHioEffResource )
    {
      req.arcSrcUseSetting = gfl2::fs::AsyncFileManager::ArcSrcUseSetting::ALWAYS_USE;
    }
  }
#endif

  GetAsyncFileManager()->SyncArcFileLoadData(req);
  mBattleDataPack.Initialize(mBattleDataPackBuf);

  for( int i=0;i<4;i++ )
  {
    GetPokeModel(btl::BtlvPos(i))->ReloadData();
    GetTrainerModelObj(btl::BtlvPos(i))->ReloadData();
  }
  mWazaMotionTableBuf = mBattleDataPack.GetData(BL_IDX_BATTLE_DATA_PACK_WAZA_MOTION_TABLE_BIN);
  mWazaMotionTableIdx = (u16*)mWazaMotionTableBuf;

  void* dataTop = (void*)(((int)mWazaMotionTableBuf) + ((MONSNO_END+1)*2));

  mWazaMotionTable = (WAZA_MOTION_TABLE*)(dataTop);

}

GFL_NAMESPACE_END(Ariizumi)
GFL_NAMESPACE_END(Test)

#endif //PM_DEBUG
