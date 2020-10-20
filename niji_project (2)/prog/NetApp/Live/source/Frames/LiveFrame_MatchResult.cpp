
//==============================================================================
/**
 * @file   LiveFrame_MatchResult.cpp
 * @date   2015/07/28 Tue. 14:55:14
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include  "../ApplicationSystem/LiveWork.h"
#include  "../LiveResource.h"

#include  "./LiveFrame_MatchResult.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    /*  init  */
    STATE_Entry = 0,

    STATE_SaveStart,
    STATE_Saving,
    STATE_SaveFailed_SDFull,

    STATE_Exit,


    STATE_     /*  terminator  */
  };
} /*  namespace  */


namespace NetApp  {
namespace Live {
// =============================================================================
/**/
//==============================================================================



//------------------------------------------------------------------
/**
  *  @func     Frame_MatchResult
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_MatchResult::Frame_MatchResult(LiveWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->GetViewContainer())
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_SaveUtil()
  , m_pUpperView_Main(NULL)
  , m_pLowerView_Selection(NULL)

{
  m_pUpperView_Main       = m_pViewContainer->m_pUpperView_Main;
  m_pLowerView_Selection  = m_pViewContainer->m_pLowerView_Selection;
  
}


//------------------------------------------------------------------
/**
  *  @func     Frame_MatchResult
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_MatchResult::~Frame_MatchResult()
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
bool  Frame_MatchResult::startup()
{
  m_pViewContainer->SetScene(LiveViewContainer::SCENE_MatchResult);

  m_pUpperView_Main->SetViewMode(UpperView_Main::VIEWMODE_TournamentName);
  m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_MatchResult_Saving);

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
void  Frame_MatchResult::setListener()
{
  m_pLowerView_Selection->SetEventHandler(this);
  m_pViewContainer->SetAllInputEnable(true);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_MatchResult::removeListener()
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
void  Frame_MatchResult::updateSequence()
{
  switch(m_State)
  {
    case  STATE_Entry:
      m_State = STATE_SaveStart;
      /*  fallthrough  */


    case  STATE_SaveStart:
    {
      m_pWork->m_pAppParam->m_pLiveNet->Disconnect();      /*  切断開始  */
      m_SaveUtil.SaveStart(m_pWork);                       /*  保存開始  */
      m_State = STATE_Saving;
    }
      /*  fallthrough  */


    case  STATE_Saving:
    {
      if(m_SaveUtil.SaveMain())
      {
        m_pWork->m_pAppParam->m_pBattleRecorderData->CheckLiveCupSaveDataValidity(m_pWork->GetDeviceHeap()->GetLowerHandle());
        const ExtSavedata::BattleRecorderSaveData::Result saveResult  = m_pWork->m_pAppParam->m_pBattleRecorderData->LiveCupSaveData();

        BattleRecorderDataSaveResultHandling(saveResult);
      }
    }
      break;

    case  STATE_SaveFailed_SDFull:
      /*  nop  */
      break;

    case  STATE_Exit:
      /*
        workにFrameResultを設定することでSwitcherが呼ばれる
      */
      if(m_pWork->m_pAppParam->m_pLiveNet->IsDisconnected())
      {
        m_pWork->SetFrameResult( m_Result );
      }
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
void  Frame_MatchResult::BattleRecorderDataSaveResultHandling(const ExtSavedata::BattleRecorderSaveData::Result saveResult)
{
  using namespace ExtSavedata;
  switch(saveResult)
  {
    case  BattleRecorderSaveData::RESULT_SUCCESS:               ///<セーブ成功
    case  BattleRecorderSaveData::RESULT_SUCCESS_AND_FULL:      ///<セーブ成功したが、セーブ件数が満杯になった
      /*  成功  */
      ExitFrame();
      break;

    case  BattleRecorderSaveData::RESULT_FAILED_SD_FULL:        ///<SDカードが満杯で失敗
      m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_MatchResult_SaveFailed_SDFull);
      m_State = STATE_SaveFailed_SDFull;
      break;


    case  BattleRecorderSaveData::RESULT_FAILED_FATAL_ERROR:    ///<ゲーム復帰不可能なエラー
      /*  fatalへ  */
      m_pWork->m_pAppParam->m_Result  = APPRESULT_Exit_Fatal;
      ExitFrame(RESULT_MATCHRESULT_Exit_Fatal);
      break;

    case  BattleRecorderSaveData::RESULT_FAILED_FULL:           ///<セーブ件数が満杯で失敗
    case  BattleRecorderSaveData::RESULT_FAILED_NO_EXIST:       ///<ファイルが存在しない
    case  BattleRecorderSaveData::RESULT_FAILED_ALREADY_EXIST:  ///<ファイルがすでに存在する
    case  BattleRecorderSaveData::RESULT_FAILED_DATA_BROKEN:    ///<データ破壊
    case  BattleRecorderSaveData::RESULT_FAILED_WAIT_ASYNC:     ///<非同期待ち中
    case  BattleRecorderSaveData::RESULT_FAILED_NO_ASYNC:       ///<同期処理なのに非同期待ちをした
    case  BattleRecorderSaveData::RESULT_FAILED:                ///<その他の失敗
    case  BattleRecorderSaveData::RESULT_WAIT_ASYNC:            ///<非同期終了待ち
    default:
      /*  nop（前作に準拠）  */
      GFL_PRINT("%s: BattleRecorderData save failed\n", __FUNCTION__);
      ExitFrame();
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
bool  Frame_MatchResult::cleanup()
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
void  Frame_MatchResult::LowerView_Selection_OnEvent(const EventCode eventCode)
{
  switch(eventCode)
  {
    case  LowerView_Selection::IEventHandler::EVENT_MessageFinish:
      if(m_State  == STATE_SaveFailed_SDFull)
      {
        ExitFrame();
      }
      break;

    case  LowerView_Selection::IEventHandler::EVENT_Exit: /*  through  */
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
void  Frame_MatchResult::ExitFrame(const FrameResult frameResult)
{
  m_Result  = frameResult;
  m_State   = STATE_Exit;
  m_pViewContainer->SetAllInputEnable(false);
}



} /*  namespace Live    */
} /*  namespace NetApp  */
