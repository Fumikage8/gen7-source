
// ============================================================================
/*
 * @file JoinFestaPlayerListShowEvent.cpp
 * @brief 閲覧アプリのイベント
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#include <NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h>

#include "../include/JoinFestaPlayerListShowEvent.h"
#include "NetApp/JoinFestaPlayerListShow/include/JoinFestaPlayerListShowProc.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"

#include <Fade/include/gfl2_FadeManager.h>

GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)
// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_JoinFestaPlayerListShowEvent
namespace FILELOCAL {
  enum  Sequence
  {
    SEQ_RunningProc,        /**<  通常実行            */
    /*  PhotoView  */
    SEQ_FadeBeforePhotoView,  /**<  PhotoView呼出し前  */
    SEQ_WaitForPhotoView,     /**<  PhotoView待ち      */
    SEQ_FadeAfterPhotoView,   /**<  PhotoView呼出し後  */
    
    SEQ_Exit
  };
}

// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void JoinFestaPlayerListShowEvent::StartEvent( GameSys::GameManager* pGameManager, const bool bApplyPhotoFlag )
{
  GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
  JoinFestaPlayerListShowEvent* pEvent  = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, JoinFestaPlayerListShowEvent>( pGameEventManager );

  pEvent->SetApplyPhotoFlag(bApplyPhotoFlag);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
JoinFestaPlayerListShowEvent::JoinFestaPlayerListShowEvent( gfl2::heap::HeapBase* pHeap ) :
 NetEvent::NetEventBase( pHeap ),
 m_AppParam(),
 m_pJoinFestaPlayerListShowProc( NULL ),
 /* photoview  */
 m_PhotoViewEventParam()
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
JoinFestaPlayerListShowEvent::~JoinFestaPlayerListShowEvent()
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool JoinFestaPlayerListShowEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void JoinFestaPlayerListShowEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  gfl2::heap::HeapBase* pHeapBase = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  
  /*  proc  */
  m_AppParam.CreateSlideShowTargetList(pHeapBase);
  m_pJoinFestaPlayerListShowProc = GameSys::CallJoinFestaPlayerListShowProc( &m_AppParam );

  /*  photoviewParam  */
  m_PhotoViewEventParam.appParam.m_pPlayList  = NetApp::PhotoView::APP_PARAM::CreatePhotoInfoList(pHeapBase, m_AppParam.pPhotoOwnerList->MaxSize());
  
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void JoinFestaPlayerListShowEvent::EndFunc( GameSys::GameManager* pGameManager )
{
  GFL_SAFE_DELETE(m_PhotoViewEventParam.appParam.m_pPlayList);
  m_AppParam.ReleaseSlideShowTargetList();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
GameSys::GMEVENT_RESULT JoinFestaPlayerListShowEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
  const gfl2::Fade::DISP    fadeDisp      = gfl2::Fade::DISP_DOUBLE;
  GameSys::GMEVENT_RESULT   result        = GameSys::GMEVENT_RES_CONTINUE;


  switch(GetSeqNo())
  {
    case  FILELOCAL::SEQ_RunningProc:          /**<  通常実行            */
      if( pGameManager->GetProcManager()->IsRegisterProcess( m_pJoinFestaPlayerListShowProc ) )
      {
        /*  起動中  */
        if(!m_AppParam.IsEmptySlideShowTarget())
        {
          /*  fade  */
          pFadeManager->ForceDefaultOut(fadeDisp);
          SetSeqNo(FILELOCAL::SEQ_FadeBeforePhotoView);
        }
      }
      else
      {
        /*  終了  */
        result = GameSys::GMEVENT_RES_FINISH;
        SetSeqNo(FILELOCAL::SEQ_Exit);
      }
      break;

    /*  PhotoView  */
    case  FILELOCAL::SEQ_FadeBeforePhotoView:  /**<  PhotoView呼出し前  */
      if(pFadeManager->IsEnd(fadeDisp))
      {
        /*  start photoview  */
        NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList        itr     = m_AppParam.pPhotoOwnerList->Begin();
        const NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList  itrEnd  = m_AppParam.pPhotoOwnerList->End();
        
        m_PhotoViewEventParam.appParam.ClearPhotoInfos();
        for(; itr != itrEnd; ++itr)
        {
          if(!m_PhotoViewEventParam.appParam.RegisterJoinFestaPersonalData(*itr))
          {
            break;
          }
        }
        
        NetEvent::PhotoView::PhotoViewEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager), m_PhotoViewEventParam);
        SetSeqNo(FILELOCAL::SEQ_WaitForPhotoView);
      }
      break;

    case  FILELOCAL::SEQ_WaitForPhotoView:     /**<  PhotoView待ち      */
      /*  fade  */
      pFadeManager->RequestIn(fadeDisp, gfl2::Fade::FADE_TYPE_ALPHA);
      /*  post process  */
      if(m_AppParam.bPhotoFlagApply)
      {
        ApplyPhotoFlagToOwnerList();
      }
      SetSeqNo(FILELOCAL::SEQ_FadeAfterPhotoView);
      break;

    case  FILELOCAL::SEQ_FadeAfterPhotoView:   /**<  PhotoView呼出し後  */
      if(pFadeManager->IsEnd(fadeDisp))
      {
        m_AppParam.ClearSlideShowTarget();
        SetSeqNo(FILELOCAL::SEQ_RunningProc);
      }
      break;
    
    case  FILELOCAL::SEQ_Exit:
    default:
      break;
  }


  return result;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  JoinFestaPlayerListShowEvent::ApplyPhotoFlagToOwnerList(void)
{
  /*
    写真PlayList内の閲覧状態を、写真のOwnerList側へ反映させる
    並び順は同じである前提
  */
  gfl2::util::List<NetApp::PhotoView::APP_PARAM::PhotoInfo>::Iterator         photoListItr    = m_PhotoViewEventParam.appParam.m_pPlayList->Begin();
  const gfl2::util::List<NetApp::PhotoView::APP_PARAM::PhotoInfo>::Iterator&  photoListItrEnd = m_PhotoViewEventParam.appParam.m_pPlayList->End();
  NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList                            jfpdItr         = m_AppParam.pPhotoOwnerList->Begin();
  const NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList                      jfpdItrEnd      = m_AppParam.pPhotoOwnerList->End();


  for(; photoListItr != photoListItrEnd; ++photoListItr)
  {
    const bool isValid = (jfpdItr != jfpdItrEnd) && NetAppLib::JoinFesta::JoinFestaPacketUtil::IsSameFriendKey(photoListItr->key, jfpdItr->GetJoinFestaFriendKey());
    GFL_ASSERT(isValid);
    if(!isValid)  break;

    if(photoListItr->IsShowed())
    {
      jfpdItr->SetNewPhotoOff();
      jfpdItr->FS_SetPhotoDataStatus_Available();    /*  statusキャッシュも変更する  */
    }

    ++jfpdItr;
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  JoinFestaPlayerListShowEvent::SetApplyPhotoFlag(const bool bApplyPhotoFlag)
{
  m_AppParam.bPhotoFlagApply  = bApplyPhotoFlag;
}

GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetEvent)
