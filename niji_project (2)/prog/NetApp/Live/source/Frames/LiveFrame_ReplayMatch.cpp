//==============================================================================
/**
 * @file   LiveFrame_ReplayMatch.cpp
 * @date   2015/07/27 Mon. 19:04:03
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "GameSys/include/GameData.h"

#include  "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include  "../ApplicationSystem/LiveWork.h"
#include  "../LiveResource.h"

#include  "./LiveFrame_ReplayMatch.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_Entry = 0,

    STATE_StartLoadRecordData,
    STATE_LoadRecordData,

    STATE_ConfirmReplay,
    STATE_DataBrokenInformation,
    STATE_DataNotFoundInformation,
    STATE_Exit,

    SUBSEQ_     /*  terminator  */
  };
} /*  namespace  */


namespace NetApp  {
namespace Live {
// =============================================================================
/**/
//==============================================================================



//------------------------------------------------------------------
/**
  *  @func     Frame_ReplayMatch
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_ReplayMatch::Frame_ReplayMatch(LiveWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->GetViewContainer())
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_pUpperView_Main(NULL)
  , m_pLowerView_Selection(NULL)

{
  m_pUpperView_Main       = m_pViewContainer->m_pUpperView_Main;
  m_pLowerView_Selection  = m_pViewContainer->m_pLowerView_Selection;
  
}


//------------------------------------------------------------------
/**
  *  @func     Frame_ReplayMatch
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_ReplayMatch::~Frame_ReplayMatch()
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
bool  Frame_ReplayMatch::startup()
{
  m_pViewContainer->SetScene(LiveViewContainer::SCENE_ReplayMatch);
  m_pUpperView_Main->SetViewMode(UpperView_Main::VIEWMODE_TournamentName);
  m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_ReplayMatch_CheckingSDCard);
  m_pViewContainer->SetAllInputEnable(true);

  return true;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_ReplayMatch::setListener()
{
  m_pLowerView_Selection->SetEventHandler(this);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_ReplayMatch::removeListener()
{
  m_pLowerView_Selection->SetEventHandler(NULL);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_ReplayMatch::RecorderDataLoadResultHandling(const ExtSavedata::BattleRecorderSaveData::Result loadResult)
{
  using namespace ExtSavedata;
  BattleRecorderSaveData* pBattleRecorderData = m_pWork->m_pAppParam->m_pBattleRecorderData;

  switch(loadResult)
  {
    case  BattleRecorderSaveData::RESULT_SUCCESS:
      /*  読み込み成功  */
      if(pBattleRecorderData->CreateBattleSetupParam( m_pWork->m_pAppParam->m_pBattleSetupParam, GFL_SINGLETON_INSTANCE( GameSys::GameManager)->GetGameData()->GetConfig()))
      {
        /*  m_pBattleSetupParamはReplayからの戻り時/振り返りキャンセル時にClearする  */
        /*  復帰成功  */
        m_pUpperView_Main->SetViewMode(UpperView_Main::VIEWMODE_BattleReplay);
        m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_ReplayMatch_ConfirmReplay);
        m_State = STATE_ConfirmReplay;
      }
      else
      {
        /*  復帰失敗：LoadせずにCreateするとFalseが返るようだ  */
        GFL_PRINT("%s: load succeded but BattleSetupParam creation failed\n", __FUNCTION__);
        GFL_ASSERT(false);

        /*  見つからなかったとしておく  */
        RecorderDataLoadResultHandling(ExtSavedata::BattleRecorderSaveData::RESULT_FAILED_NO_EXIST);
      }
      break;

    /*
      fatal
    */
    case  BattleRecorderSaveData::RESULT_FAILED_DATA_BROKEN:
    {
      /*  壊れていたので削除する  */
      const BattleRecorderSaveData::Result  deleteResult  = pBattleRecorderData->LiveCupDeleteData();
      
      if((deleteResult == BattleRecorderSaveData::RESULT_SUCCESS) || (deleteResult == BattleRecorderSaveData::RESULT_SUCCESS_AND_FULL))
      {
        /*  メッセージ表示  */
        m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_ReplayMatch_BrokenData);
        m_State = STATE_DataBrokenInformation;
      }
      else
      {
        /*  削除にも失敗した場合はfatalへ  */
        RecorderDataLoadResultHandling(BattleRecorderSaveData::RESULT_FAILED_FATAL_ERROR);
      }
    }
      break;

    case  BattleRecorderSaveData::RESULT_FAILED_FATAL_ERROR:
      /*  fatalへ  */
      m_pWork->m_pAppParam->m_Result  = APPRESULT_Exit_Fatal;
      ExitFrame(RESULT_REPLAY_Exit_Fatal);
      break;


    /*
      RESULT_FAILED_NO_EXIST            ///<ファイルが存在しない
    
      想定外:
        書き込みではない
          RESULT_FAILED_FULL,           ///<セーブ件数が満杯で失敗
          RESULT_FAILED_SD_FULL,        ///<SDカードが満杯で失敗
          RESULT_FAILED_ALREADY_EXIST,  ///<ファイルがすでに存在する
        非同期でない
          RESULT_FAILED_WAIT_ASYNC,     ///<非同期待ち中
          RESULT_FAILED_NO_ASYNC,       ///<同期処理なのに非同期待ちをした
          RESULT_WAIT_ASYNC,            ///<非同期終了待ち
        その他：
          RESULT_FAILED,                ///<その他の失敗
    */
    case  BattleRecorderSaveData::RESULT_FAILED_NO_EXIST:       ///<ファイルが存在しない
    case  BattleRecorderSaveData::RESULT_FAILED_FULL:           ///<セーブ件数が満杯で失敗
    case  BattleRecorderSaveData::RESULT_FAILED_SD_FULL:        ///<SDカードが満杯で失敗
    case  BattleRecorderSaveData::RESULT_FAILED_ALREADY_EXIST:  ///<ファイルがすでに存在する
    case  BattleRecorderSaveData::RESULT_FAILED_WAIT_ASYNC:     ///<非同期待ち中
    case  BattleRecorderSaveData::RESULT_FAILED_NO_ASYNC:       ///<同期処理なのに非同期待ちをした
    case  BattleRecorderSaveData::RESULT_WAIT_ASYNC:            ///<非同期終了待ち
    case  BattleRecorderSaveData::RESULT_FAILED:                ///<その他の失敗
    default:
      /*  なかった表示  */
      m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_ReplayMatch_NoData);
      m_State = STATE_DataNotFoundInformation;
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_ReplayMatch::updateSequence()
{
  switch(m_State)
  {
    case  STATE_Entry:
      m_State = STATE_StartLoadRecordData;
      /*  fallthrough  */

    case  STATE_StartLoadRecordData:
//      m_pWork->m_pAppParam->m_pBattleRecorderData->LiveCupLoadData();
      m_State = STATE_LoadRecordData;
      /*  fallthrough  */

    case  STATE_LoadRecordData:
    {
      ExtSavedata::BattleRecorderSaveData*          pBattleRecorderData = m_pWork->m_pAppParam->m_pBattleRecorderData;
      ExtSavedata::BattleRecorderSaveData::Result   recorderDataLoadResult;
      bool  isSuccess = false;
      
      recorderDataLoadResult  = pBattleRecorderData->LiveCupLoadData();
      RecorderDataLoadResultHandling(recorderDataLoadResult);
    }
      break;

    case  STATE_ConfirmReplay:
      break;
    case  STATE_DataBrokenInformation:
      break;
    case  STATE_DataNotFoundInformation:
      break;

    case  STATE_Exit:
      /*
        workにFrameResultを設定することでSwitcherが呼ばれる
      */
      m_pWork->SetFrameResult( m_Result );
      break;
  }

}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  Frame_ReplayMatch::cleanup()
{
  return true;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
/*  implement LowerView_Selection::IEventHandler  */
void  Frame_ReplayMatch::LowerView_Selection_OnEvent(const EventCode eventCode)
{
  switch(eventCode)
  {
    case  LowerView_Selection::IEventHandler::EVENT_MessageFinish:
      if(m_State  == STATE_DataBrokenInformation)
      {
        ExitFrame();
      }
      else if(m_State == STATE_DataNotFoundInformation)
      {
        ExitFrame();
      }
      break;


    case  LowerView_Selection::IEventHandler::EVENT_ReplayMatch_Replay_Yes:           /**<  対戦リプレイ：振り返る？→はい    */
      m_pWork->m_pAppParam->m_Result  = APPRESULT_CallBattleReplay;
      ExitFrame(RESULT_REPLAY_Replay);
      break;

    case  LowerView_Selection::IEventHandler::EVENT_ReplayMatch_Replay_No:            /**<  対戦リプレイ：振り返る？→いいえ  */
    {
      APP_PARAM*  pAppParam = m_pWork->m_pAppParam;

      pAppParam->m_pBattleRecorderData->ClearBattleSetupParam( pAppParam->m_pBattleSetupParam );
      ExitFrame();
    }
      break;

    default:
      break;
  }

}
/*  endimplement LowerView_Selection::IEventHandler  */



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_ReplayMatch::ExitFrame(const FrameResult frameResult)
{
  m_Result  = frameResult;
  m_State   = STATE_Exit;
  m_pViewContainer->SetAllInputEnable(false);
}



} /*  namespace Live    */
} /*  namespace NetApp  */
