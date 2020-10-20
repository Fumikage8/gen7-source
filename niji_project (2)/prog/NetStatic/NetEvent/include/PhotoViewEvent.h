#if !defined(PHOTOVIEWEVENT_H_INCLUDED)
#define PHOTOVIEWEVENT_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   PhotoViewEvent.h
 * @date   2017/02/07 Tue. 18:16:34
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>

#include  <GameSys/include/GameManager.h>
#include  <GameSys/include/GameEvent.h>

#include  <NetApp/PhotoView/include/PhotoViewProc.h>


// =============================================================================
/**/
//==============================================================================
namespace NetApp {
namespace PhotoView  {
  class PhotoViewProc;
}
}



namespace NetEvent {
namespace PhotoView {
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2017/02/07 Tue. 18:20:57
  */
//==============================================================================
class PhotoViewEvent
  : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( PhotoViewEvent );
public:

  struct  Param
  {
    NetApp::PhotoView::APP_PARAM  appParam;
    
    Param(void) 
      :  appParam()
    {}
  };

  static void StartEvent(GameSys::GameManager* pGameManager, Param& rParam);

  /*
      GameEvent I/F
  */
  virtual bool  BootChk( GameSys::GameManager* pGameManager );
  virtual void  InitFunc( GameSys::GameManager* pGameManager );
  virtual void  EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );
  /*
      End GameEvent I/F
  */

  PhotoViewEvent( gfl2::heap::HeapBase* pHeap );
  virtual ~PhotoViewEvent();

private:
  Param                               m_EventParam;
  
  NetApp::PhotoView::PhotoViewProc*   m_pProc;

};


}  /*  namespace PhotoView  */
}  /*  namespace NetEvent  */
#endif  /*  #if !defined(PHOTOVIEWEVENT_H_INCLUDED)  */
