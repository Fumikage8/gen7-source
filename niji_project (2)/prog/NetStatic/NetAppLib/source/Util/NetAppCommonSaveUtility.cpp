// ============================================================================
/*
 * @file NetAppCommonSaveUtility.cpp
 * @brief セーブユーティリティ
 * @date 2015.10.29
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/Util/NetAppCommonSaveUtility.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "System/include/HeapDefine.h"
#include "GameSys/include/NijiTime.h"
#include "NetStatic/NetAppLib/include/QR/QRDefine.h"
#include  <thread/include/gfl2_ThreadStatic.h>

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(Util)


NetAppCommonSaveUtility::NetAppCommonSaveUtility() :
  m_eState( STATE_SAVE_START ),
  m_Timer(),
  m_bIsExecute( false )
{
}


NetAppCommonSaveUtility::~NetAppCommonSaveUtility()
{
  // @fix GFNMCat[3789] 【フェスサークル】セーブシーケンス中に通信エラーアプレットが表示されるととまる
  while( m_bIsExecute )
  {
    Save();
    gfl2::thread::ThreadStatic::CurrentSleep(1);
  }
}


bool NetAppCommonSaveUtility::Save( bool bUpdatePlayTime )
{
  bool bSaveComplete = false;

  switch( m_eState )
  {
    // ----------------------------------------------------------------------
    case STATE_SAVE_START:
    {
      saveStart( bUpdatePlayTime );

      setState( STATE_SAVE_MAIN );
    }
    break;

    // ----------------------------------------------------------------------
    case STATE_SAVE_MAIN:
    {
      bSaveComplete = saveMain( bUpdatePlayTime );
      
      if( bSaveComplete )
      {
        setState( STATE_SAVE_START );
      }
    }
    break;
  }

  return bSaveComplete;
}

bool NetAppCommonSaveUtility::SaveWait( bool bUpdatePlayTime )
{
  bool bSaveComplete = false;

  switch( m_eState )
  {
    // ----------------------------------------------------------------------
    case STATE_SAVE_START:
    {
      m_Timer.TimeOutStart();

      saveStart( bUpdatePlayTime );

      setState( STATE_SAVE_MAIN );
    }
    break;

    // ----------------------------------------------------------------------
    case STATE_SAVE_MAIN:
    {
      if( saveMain( bUpdatePlayTime ) )
      {
        setState( STATE_SAVE_WAIT );
      }
    }
    break;

    // ----------------------------------------------------------------------
    case STATE_SAVE_WAIT:
    {
      if( m_Timer.IsTimeOut() )
      {
        bSaveComplete = true;

        setState( STATE_SAVE_START );
      }
    }
    break;
  }

  return bSaveComplete;
}

void NetAppCommonSaveUtility::SetZukanSavePokeSee(Savedata::ZukanData* pZukanSave, const NetApp::QR::QR_ZUKAN& rZukanQR)
{
  pml::Sex SEX_VAL[NetApp::QR::QR_ZUKAN::SEE_FLAG_MAX] =
  {
    pml::SEX_MALE,
    pml::SEX_FEMALE,
    pml::SEX_MALE,
    pml::SEX_FEMALE,
  };
  bool RARE_VAL[NetApp::QR::QR_ZUKAN::SEE_FLAG_MAX] =
  {
    false,
    false,
    true,
    true,
  };

  // 最初に登録するのはdispの姿(図鑑上にdispの姿が表示されるようにするため)  // @fix GFNMCat[1204] 【ロトム図鑑】ＱＲスキャンで撮影したときの見た目が図鑑に登録されないことがある
  PokeTool::SimpleParam dispSp;
  {
    dispSp.isEgg   = false;
    dispSp.monsNo  = static_cast<MonsNo>(rZukanQR.monsNo);
    dispSp.formNo  = static_cast<pml::FormNo>(rZukanQR.dispForm);
    dispSp.sex     = static_cast<pml::Sex>(rZukanQR.dispSex);
    dispSp.isRare  = (rZukanQR.dispRare != 0);
   
    if(dispSp.sex == pml::SEX_MALE || dispSp.sex == pml::SEX_UNKNOWN)
    {
      if(dispSp.isRare)
      {
        dispSp.perRand = rZukanQR.rand[NetApp::QR::QR_ZUKAN::SEE_FLAG_M_RARE];
      }
      else
      {
        dispSp.perRand = rZukanQR.rand[NetApp::QR::QR_ZUKAN::SEE_FLAG_MALE];
      }
    }
    else  // dispSp.sex == pml::SEX_FEMALE
    {
      if(dispSp.isRare)
      {
        dispSp.perRand = rZukanQR.rand[NetApp::QR::QR_ZUKAN::SEE_FLAG_F_RARE];
      }
      else
      {
        dispSp.perRand = rZukanQR.rand[NetApp::QR::QR_ZUKAN::SEE_FLAG_FEMALE];
      }
    }
    
    pZukanSave->SetPokeSee(dispSp);
  }

  // dispの姿以外も登録する
  for(u32 i=0; i<32; ++i)
  {
    bool seeFlag[NetApp::QR::QR_ZUKAN::SEE_FLAG_MAX] = { 0 };
    bool seeOne = false;  // SEE_FLAG_MAX中のどれか1個でも見ていたらtrue
    for(u32 j=0; j<NetApp::QR::QR_ZUKAN::SEE_FLAG_MAX; ++j)
    {
      if( ( rZukanQR.formBit[j] & (1<<i) ) != 0 )
      {
        seeFlag[j] = true;
        seeOne = true;
      }
    }

    if(seeOne)
    {
      PokeTool::SimpleParam sp;
      sp.isEgg = false;
      
      MonsNo      monsNo = static_cast<MonsNo>(rZukanQR.monsNo);
      pml::FormNo formNo = i;

      sp.monsNo = monsNo;
      sp.formNo = formNo;

      // ♂or性別なしの判定
      bool sexUnknown = false;
      pml::personal::LoadPersonalData( monsNo, formNo );
      pml::personal::SexVector sexVec = static_cast<pml::personal::SexVector>( pml::personal::GetPersonalParam(pml::personal::PARAM_ID_sex) );
      if(sexVec == pml::personal::SEX_VECTOR_UNKNOWN) sexUnknown = true;

      for(u32 j=0; j<NetApp::QR::QR_ZUKAN::SEE_FLAG_MAX; ++j)
      {
        if(seeFlag[j])
        {
          sp.sex     = ((SEX_VAL[j]==pml::SEX_MALE)&&(sexUnknown!=false))?(pml::SEX_UNKNOWN):(SEX_VAL[j]);
          sp.isRare  = RARE_VAL[j];
          sp.perRand = rZukanQR.rand[j];

          if( 
                 dispSp.formNo != sp.formNo
              || dispSp.sex    != sp.sex
              || dispSp.isRare != sp.isRare
          )  // dispの姿は登録済みなので飛ばす。
          {
            pZukanSave->SetPokeSee(sp);
          }
        }
      }
    }
  }
}


void NetAppCommonSaveUtility::SetDisconnectDetectedFlag( bool bFlag )
{
  Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

  if( bFlag )
  {
    pJoinFestaDataSave->SetGradePenaltyCount( 30 );
  }
  else
  {
    pJoinFestaDataSave->SetGradePenaltyCount( 0 );
  }
}


void NetAppCommonSaveUtility::setState( State eState )
{
  m_eState = eState;
}


void NetAppCommonSaveUtility::saveStart( bool bUpdatePlayTime )
{
  GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();

  gfl2::heap::HeapBase *pDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
      
  if( bUpdatePlayTime )
  {
    Savedata::PlayTime* pPlayTime = pGameData->GetPlayTime();

    pPlayTime->PushSaveTime();

    Savedata::GameTime gameTime;
    GameSys::DeviceDate date;
    gameTime.SetDate( date.GetDateConst() );

    pPlayTime->SetSaveTime( &gameTime );
  }

  pGameData->SaveStart( pDeviceHeap );

  m_bIsExecute = true;
}


bool NetAppCommonSaveUtility::saveMain( bool bUpdatePlayTime )
{
  bool bSaveComplete = false;

  GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();

  gfl2::fs::FsCtrBackupSystem::Result result = pGameData->SaveMain();

  if( result == gfl2::fs::FsCtrBackupSystem::RESULT_OK )
  {
    bSaveComplete = true;
  }
  else
  if( result == gfl2::fs::FsCtrBackupSystem::RESULT_NG_ERROR )
  {
    if( bUpdatePlayTime )
    {
      Savedata::PlayTime* pPlayTime = pGameData->GetPlayTime();

      pPlayTime->PopSaveTime();
    }

    bSaveComplete = true;
  }

  if( bSaveComplete )
  {
    m_bIsExecute = false;
  }

  return bSaveComplete;
}


GFL_NAMESPACE_END(Util)
GFL_NAMESPACE_END(NetAppLib)
