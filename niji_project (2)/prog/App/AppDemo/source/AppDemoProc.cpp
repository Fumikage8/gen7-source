//==============================================================================
/**
 * @file   AppDemoProc.cpp
 * @date   2015/08/17 Mon. 17:50:52
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <ro/include/gfl2_RoManager.h>
#include  <System/include/HeapDefine.h>
#include  "GameSys/include/GameManager.h"
#include  "GameSys/include/GameData.h"
#include  "Sound/include/Sound.h"

#include  "App/AppDemo/include/AppDemoProc.h"
#include  <System/include/Timer/gfl2_PerformanceCounter.h>


// =============================================================================
/**/
//==============================================================================
namespace {
  enum
  {
    /*  init  */
    SUBSEQ_INIT_First = 0,

    /*  update  */
    SUBSEQ_UPDATE_First = 0,
    SUBSEQ_UPDATE_Running,

    /*  end  */
    SUBSEQ_END_First = 0,
    SUBSEQ_END_Cleanup,
    
  };
}



namespace App {
namespace AppDemo {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
AppDemoProc::AppDemoProc(const AppDemoProcParam* pParam)
  : m_Param(*pParam)
  , m_pDemoUtil(NULL)
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
AppDemoProc::~AppDemoProc()
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
gfl2::proc::Result AppDemoProc::InitFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result      result    = gfl2::proc::RES_CONTINUE;


  switch(GetSubSeq())
  {
    case  SUBSEQ_INIT_First:
    {
      gfl2::heap::HeapBase* pHeapBase =   gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );  

      m_pDemoUtil  = GFL_NEW(pHeapBase) AppDemoUtility(pHeapBase, GetSequenceDemoHeapModel());
      m_pDemoUtil->SetEventHandler(this);

      result  = gfl2::proc::RES_FINISH;
    }
    break;
  }


  return result;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
gfl2::proc::Result AppDemoProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result      result    = gfl2::proc::RES_CONTINUE;


  switch(GetSubSeq())
  {
    case  SUBSEQ_UPDATE_First:
      SetSubSeq(SUBSEQ_UPDATE_Running);
      /*  fallthrough  */

    case  SUBSEQ_UPDATE_Running:
      AppDemoUtility_CancelQuery();
      m_pDemoUtil->Update();
      if(m_pDemoUtil->IsStable())
      {
        result    = gfl2::proc::RES_FINISH;
      }
      break;
  }


  return result;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
gfl2::proc::Result AppDemoProc::EndFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result      result    = gfl2::proc::RES_CONTINUE;



  switch(GetSubSeq())
  {
    case  SUBSEQ_END_First:
    {
      GFL_SAFE_DELETE(m_pDemoUtil);
    
      result    = gfl2::proc::RES_FINISH;
    }
    break;
  }

  return result;
}


//------------------------------------------------------------------
/**
  *  @func     PreDrawFunc
  *  @brief    描画準備
  */
//------------------------------------------------------------------
void  AppDemoProc::PreDrawFunc( gfl2::proc::Manager* pProcManager )
{
  m_pDemoUtil->PreDraw();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  AppDemoProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pDemoUtil->Draw(displayNo);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
u32  AppDemoProc::AppDemoUtility_StartQuery(DemoLib::Sequence::SequenceViewSystem* pViewSystem)
{
  return m_Param.demoGARCID;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  AppDemoProc::AppDemoUtility_OnSequenceEnd(DemoLib::Sequence::SequenceViewSystem* pViewSystem)
{
  m_pDemoUtil->ExitRequest();
}



//------------------------------------------------------------------
/**
  *  @brief    おくりもの受信デモ
  */
//------------------------------------------------------------------
AppDemoProc_Gift::AppDemoProc_Gift(const AppDemoProcParam_Gift* pParam)
  : AppDemoProc(pParam)
  , m_ParamGift(*pParam)
{
}


//------------------------------------------------------------------
/**
  *  @brief    おくりもの受信デモ
  */
//------------------------------------------------------------------
AppDemoProc_Gift::~AppDemoProc_Gift()
{
}


//------------------------------------------------------------------
/**
  *  @brief    おくりもの受信デモ
  */
//------------------------------------------------------------------
u32   AppDemoProc_Gift::AppDemoUtility_StartQuery(DemoLib::Sequence::SequenceViewSystem* pViewSystem)
{
  for(u32 index = 0; index < GFL_NELEMS(m_ParamGift.pPokemonParams); ++index)
  {
    if(m_ParamGift.pPokemonParams[index])
    {
      GFL_PRINT("set pokemonParam: monsNo: %d\n", static_cast<int>(m_ParamGift.pPokemonParams[index]->GetMonsNo()));
    }

    pViewSystem->SetPokemonParam(index, m_ParamGift.pPokemonParams[index]);
  }

  GFL_PRINT("set demotype:  %d\n", static_cast<int>(m_ParamGift.type));
  pViewSystem->SetDemoType(m_ParamGift.type);

  return m_ParamGift.DEMO_GARCID_GIFT;
}




//------------------------------------------------------------------
/**
  *  @brief    進化デモ
  */
//------------------------------------------------------------------
AppDemoProc_Evolution::AppDemoProc_Evolution(const AppDemoProcParam_Evolution* pParam)
  : AppDemoProc(pParam)
  , m_ParamEvolution(*pParam)
{
}


//------------------------------------------------------------------
/**
  *  @brief    進化デモ
  */
//------------------------------------------------------------------
AppDemoProc_Evolution::~AppDemoProc_Evolution()
{
}


//------------------------------------------------------------------
/**
  *  @brief    進化デモ
  */
//------------------------------------------------------------------
u32   AppDemoProc_Evolution::AppDemoUtility_StartQuery(DemoLib::Sequence::SequenceViewSystem* pViewSystem)
{
  for(u32 index = 0; index < GFL_NELEMS(m_ParamEvolution.pPokemonParams); ++index)
  {
    if(m_ParamEvolution.pPokemonParams[index])
    {
      GFL_PRINT("set pokemonParam: monsNo: %d\n", static_cast<int>(m_ParamEvolution.pPokemonParams[index]->GetMonsNo()));
    }

    pViewSystem->SetPokemonParam(index, m_ParamEvolution.pPokemonParams[index]);
  }
  
  return m_ParamEvolution.DEMO_GARCID_EVOLUTION;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  AppDemoProc_Evolution::SendEvent(const AppDemoProc_Evolution::EventCode eventCode)
{
  switch(eventCode)
  {
    case  EVENT_EvolutionCancelRequest:
      break;

    case  EVENT_SyncSignal:
      break;
  }
}



//------------------------------------------------------------------
/**
  *  @brief    GTSデモ
  */
//------------------------------------------------------------------
AppDemoProc_GTS::AppDemoProc_GTS(const AppDemoProcParam_GTS* pParam)
  : AppDemoProc(pParam)
  , m_ParamGTS(*pParam)
{
}


//------------------------------------------------------------------
/**
  *  @brief    GTSデモ
  */
//------------------------------------------------------------------
AppDemoProc_GTS::~AppDemoProc_GTS()
{
}


//------------------------------------------------------------------
/**
  *  @brief    GTSデモ
  */
//------------------------------------------------------------------
AppDemoUtility::HeapModel   AppDemoProc_GTS::GetSequenceDemoHeapModel(void) const
{
  return AppDemoUtility::HEAPMODEL_75_Percent;
}


//------------------------------------------------------------------
/**
  *  @brief    GTSデモ
  */
//------------------------------------------------------------------
u32   AppDemoProc_GTS::AppDemoUtility_StartQuery(DemoLib::Sequence::SequenceViewSystem* pViewSystem)
{
  pViewSystem->SetPokemonParam( 0, m_ParamGTS.pSendPokemonParam );
  pViewSystem->SetPokemonParam( 1, m_ParamGTS.pRecvPokemonParam );

  pViewSystem->SetPokeNickName( 0, m_ParamGTS.pSendPokemonParam );
  pViewSystem->SetPokeNickName( 2, m_ParamGTS.pRecvPokemonParam );

  GFL_PRINT( "AppDemoProc_GTS Demo SequenceID = %d \n", m_ParamGTS.demoGARCID );
  GFL_PRINT( "AppDemoProc_GTS Send Pokemon No = %d \n", m_ParamGTS.pSendPokemonParam->GetMonsNo() );
  GFL_PRINT( "AppDemoProc_GTS Recv Pokemon No = %d \n", m_ParamGTS.pRecvPokemonParam->GetMonsNo() );

  if( m_ParamGTS.demoGARCID == GARC_demo_seq_script_sd9110_trade_down_BSEQ )
  {
    if( m_ParamGTS.bDownloadTrade )
    {
      pViewSystem->SetDemoType(1);    
    }
    else
    {
      pViewSystem->SetDemoType(0);    
    }
  }
  else
  if( m_ParamGTS.demoGARCID == GARC_demo_seq_script_sd9110_trade_all_BSEQ )
  {
    pViewSystem->SetDemoType(2);    
  }

  return m_ParamGTS.demoGARCID;
}




//------------------------------------------------------------------
/**
  *  @brief    ミラクル交換デモ
  */
//------------------------------------------------------------------
AppDemoProc_MiracleTrade::AppDemoProc_MiracleTrade(const AppDemoProcParam_MiracleTrade* pParam)
  : AppDemoProc(pParam)
  , m_ParamMiracleTrade(*pParam)
  , m_pViewSystem( NULL )
{
}


//------------------------------------------------------------------
/**
  *  @brief    ミラクル交換デモ
  */
//------------------------------------------------------------------
AppDemoProc_MiracleTrade::~AppDemoProc_MiracleTrade()
{
}


//------------------------------------------------------------------
/**
  *  @brief    ミラクル交換デモ
  */
//------------------------------------------------------------------
AppDemoUtility::HeapModel   AppDemoProc_MiracleTrade::GetSequenceDemoHeapModel(void) const
{
  return AppDemoUtility::HEAPMODEL_75_Percent;
}


//------------------------------------------------------------------
/**
  *  @brief    ミラクル交換デモ
  */
//------------------------------------------------------------------
u32   AppDemoProc_MiracleTrade::AppDemoUtility_StartQuery(DemoLib::Sequence::SequenceViewSystem* pViewSystem)
{
  m_pViewSystem = pViewSystem;

  pViewSystem->SetSystemCallBack( this );

  pViewSystem->SetPokemonParam( 0, m_ParamMiracleTrade.pSendPokemonParam );
  pViewSystem->SetPokemonParam( 1, m_ParamMiracleTrade.pRecvPokemonParam );
  
  /*  name  */
  pViewSystem->SetPokeNickName( 0, m_ParamMiracleTrade.pSendPokemonParam );
  pViewSystem->SetTrainerName(  1, m_ParamMiracleTrade.pTradePersonStatus );
  pViewSystem->SetPokeNickName( 2, m_ParamMiracleTrade.pRecvPokemonParam );

  if( compare( m_ParamMiracleTrade.pSendPokemonParam, m_ParamMiracleTrade.pTradePersonStatus ) )
  {    
    pViewSystem->SetDemoType(1);    
  }
  else
  {    
    pViewSystem->SetDemoType(0);    
  }

  return m_ParamMiracleTrade.demoGARCID;
}


void AppDemoProc_MiracleTrade::SequenceCallBackFunc(int command, int paramA, int paramB)
{
  if( command == DemoLib::Sequence::SEQCALLBACK_COMMAND_SYSWINDOW )
  {
    switch( paramA )
    {
      case 1:
      {
        Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();

        if( compare( m_ParamMiracleTrade.pRecvPokemonParam, pMyStatus ) )
        {
          m_pViewSystem->SetDemoType(1);
        }
        else
        {
          m_pViewSystem->SetDemoType(0);
        }
      }
      break;
    }
  }
}


bool AppDemoProc_MiracleTrade::compare( pml::pokepara::PokemonParam* pPokemonParam, Savedata::MyStatus* pMyStatus )
{
  bool bResult = false;

  gfl2::str::StrBuf myStatusName( 128, m_ParamMiracleTrade.pHeap );
  gfl2::str::StrBuf pokeParentName( 128, m_ParamMiracleTrade.pHeap );

  pMyStatus->GetNameString( &myStatusName );
  pPokemonParam->GetParentName( &pokeParentName );

  if( pMyStatus->GetID() == pPokemonParam->GetID()
   && pMyStatus->GetSex() == static_cast<u8>( pPokemonParam->GetParentSex() )
   && myStatusName.Compare( pokeParentName ) )
  {
    bResult = true;
  }

  return bResult;
}








//------------------------------------------------------------------
/**
  *  @brief    おめでとう演出デモ
  */
//------------------------------------------------------------------
AppDemoProc_Congrats::AppDemoProc_Congrats(const AppDemoProcParam_Congrats* pParam)
  : AppDemoProc(pParam)
  , m_ParamCongras(*pParam)
{
}


//------------------------------------------------------------------
/**
  *  @brief    おめでとう演出デモ
  */
//------------------------------------------------------------------
AppDemoProc_Congrats::~AppDemoProc_Congrats()
{
}


//------------------------------------------------------------------
/**
  *  @brief    おめでとう演出デモ
  */
//------------------------------------------------------------------
u32   AppDemoProc_Congrats::AppDemoUtility_StartQuery(DemoLib::Sequence::SequenceViewSystem* pViewSystem)
{
  u32 count = 0;
  for(u32 index = 0; index < m_ParamCongras.pPokeParty->GetMemberCount(); ++index)
  {
		pml::pokepara::PokemonParam * pp = m_ParamCongras.pPokeParty->GetMemberPointer( index );

    //タマゴは殿堂入りに表示させない
		if( pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) == true ){
      GFL_PRINT("unset[EGG] pokemonParam: monsNo: %d\n", static_cast<int>(pp->GetMonsNo()));
			continue;
		}

    GFL_PRINT("set pokemonParam: monsNo: %d\n", static_cast<int>(pp->GetMonsNo()));
    pViewSystem->SetPokemonParam(count++, pp);
  }
 
  if( m_ParamCongras.isFirstClear )
  {
    pViewSystem->SetDemoType( 0 );  //初代チャンピオンタイプA
    Sound::ChangeAppBGMReq( STRM_BGM_DENDOUIRI, Sound::BGM_FADE_VERY_FAST, Sound::BGM_FADE_NONE  );
  }
  else
  {
    pViewSystem->SetDemoType( 1 );  //防衛成功タイプB
    Sound::StartAppBGMReq( STRM_BGM_DENDOUIRI );
  }

  return m_ParamCongras.demoGARCID;
}

//------------------------------------------------------------------
/**
  *  @brief    おめでとう演出デモのキャンセル処理
  */
//------------------------------------------------------------------
void  AppDemoProc_Congrats::AppDemoUtility_CancelQuery(void)
{
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    GFL_PRINT("cancel command [frame:%d]\n", m_pDemoUtil->GetCurrentFrame());
    m_pDemoUtil->SendCommand(App::AppDemo::AppDemoUtility::COMMAND_SetSystemCancel);
  }
}

//------------------------------------------------------------------
/**
  *  @brief    VSデモ
  */
//------------------------------------------------------------------
AppDemoProc_VS::AppDemoProc_VS(const AppDemoProcParam_VS* pParam)
  : AppDemoProc(pParam)
  , m_ParamVS(*pParam)
{
}

//------------------------------------------------------------------
/**
  *  @brief    VSデモ
  */
//------------------------------------------------------------------
AppDemoProc_VS::~AppDemoProc_VS()
{
}

//------------------------------------------------------------------
/**
  *  @brief    VSデモ
  */
//------------------------------------------------------------------
u32 AppDemoProc_VS::AppDemoUtility_StartQuery(DemoLib::Sequence::SequenceViewSystem* pViewSystem)
{
  // どのデモか
  pViewSystem->SetPlaybackParameter( DemoLib::Sequence::SEQPARAMTYPE_VSBATTLE, m_ParamVS.demoType );


  int playerMax = 2;
  if( m_ParamVS.battleRule == Regulation::BATTLE_RULE_ROYAL || m_ParamVS.battleRule == Regulation::BATTLE_RULE_MULTI )
  {
    playerMax = 4;
  }

  for( int i=0; i<playerMax; i++ )
  {
    // プレイヤー
    pViewSystem->SetVSBattleDressUpParam( m_ParamVS.pMyStatus[i], i );

    // モンスター状態
    pViewSystem->SetVSBattleMonsterBallSetting( i, m_ParamVS.pokemonStatus[i] );
  }

  // 背景
  pViewSystem->SetDemoType( m_ParamVS.bgType );

  return m_ParamVS.demoGARCID;
}


//-----------------------------------------------------------------------------
// イントロデモ後半
//-----------------------------------------------------------------------------
AppDemoProc_Intro2nd::AppDemoProc_Intro2nd( const AppDemoProcParam_Intro2nd * param )
  : AppDemoProc( param )
  , m_paramIntro2nd( *param )
{
}

AppDemoProc_Intro2nd::~AppDemoProc_Intro2nd()
{
}

u32 AppDemoProc_Intro2nd::AppDemoUtility_StartQuery( DemoLib::Sequence::SequenceViewSystem * pViewSystem )
{
  pViewSystem->SetDemoType( m_paramIntro2nd.hero_index );
  return m_paramIntro2nd.demoGARCID;
}


} /*  namespace AppDemo */
} /*  namespace App     */
