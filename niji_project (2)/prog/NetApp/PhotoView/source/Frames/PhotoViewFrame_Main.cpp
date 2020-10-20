
//==============================================================================
/**
 * @file   PhotoViewFrame_Main.cpp
 * @date   2017/02/07 Tue. 13:22:34
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <NetStatic/NetAppLib/include/System/ResourceManager.h>
#include  <Fade/include/gfl2_FadeManager.h>
#include  "../ApplicationSystem/PhotoViewWork.h"
#include  "../PhotoViewResource.h"

#include  "./PhotoViewFrame_Main.h"


namespace NetApp  {
namespace PhotoView {
// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_PhotoViewFrame_Main
namespace FILELOCAL {

  static u32  PreShowFrame    = 5;
  static u32  SlideShowFrame  = 150;
  static u32  FadeFrame       = 60;

  enum  {
    STATE_Entry = 0,
    STATE_Loading,
    STATE_PreShow,
    STATE_Show,
    STATE_Showing,
    /**
      @fix  MMcat[496:   【フェスサークル】特定のタイミングでスライドショー画面から抜けると、次の写真が一瞬表示される
        アプリ終了時に、TYPE_CROSSで表示していた前の画像が透明の黒となり、裏読みしていた次の写真が一瞬表示されてしまう
        FrameWorkに任せていたFadeを抑制し、手動で行う
        アプリ終了時のFadeOutを行うStateを追加
    */
    STATE_FadeOut,
    STATE_WaitForFade,
    STATE_Exit,

    STATE_     /*  terminator  */
  };

  static const gfl2::math::Vector4 fadeColor_Start(0,0,0,0);
  static const gfl2::math::Vector4 fadeColor_End(0,0,0,255.0f);
}


// =============================================================================
/**/
//==============================================================================



//------------------------------------------------------------------
/**
  *  @func     Frame_Main
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_Main::Frame_Main(PhotoViewWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->GetViewContainer())
  , m_State(FILELOCAL::STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_FrameCount(FILELOCAL::SlideShowFrame)
  , m_JFPlayerDataListItr()
  , m_JFPlayerDataListItr_Begin()
  , m_JFPlayerDataListItr_End()
  /*  views  */
  , m_pUpperView_Photo(NULL)
  , m_pLowerView_Info(NULL)

{
  m_pUpperView_Photo  = m_pViewContainer->m_pUpperView_Photo;
  m_pLowerView_Info   = m_pViewContainer->m_pLowerView_Info;
}


//------------------------------------------------------------------
/**
  *  @func     Frame_Main
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_Main::~Frame_Main()
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
bool  Frame_Main::startup()
{
  m_pViewContainer->SetScene(PhotoViewViewContainer::SCENE_View);

  {
    m_JFPlayerDataListItr       = m_pWork->m_pAppParam->m_pPlayList->Begin();
    m_JFPlayerDataListItr_Begin = m_pWork->m_pAppParam->m_pPlayList->Begin();
    m_JFPlayerDataListItr_End   = m_pWork->m_pAppParam->m_pPlayList->End();

    RequestPhoto();
    m_pLowerView_Info->SetData(m_JFPlayerDataListItr->name, m_JFPlayerDataListItr->photoData.photo_param);

    m_FrameCount  = FILELOCAL::SlideShowFrame;
  }

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
void  Frame_Main::setListener()
{
  m_pLowerView_Info->SetEventHandler(this);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_Main::removeListener()
{
  m_pLowerView_Info->SetEventHandler(NULL);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_Main::updateSequence()
{
  switch(m_State)
  {
    case  FILELOCAL::STATE_Entry:
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_CROSS, &FILELOCAL::fadeColor_Start, &FILELOCAL::fadeColor_End, 1);
      /**
        @fix  MMcat[496:   【フェスサークル】特定のタイミングでスライドショー画面から抜けると、次の写真が一瞬表示される
          アプリ終了時に、TYPE_CROSSで表示していた前の画像が透明の黒となり、裏読みしていた次の写真が一瞬表示されてしまう
          FrameWorkに任せていたFadeを抑制し、手動で行う
          ここでは写真のLoad待ちのため、上はクリアカラーのCapture画像でマスクしておき、下画面のみFadeInとなる
      */
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn( gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA);
      m_State = FILELOCAL::STATE_Loading;
      /*  fallthrough  */

    /*  読み込み待ち  */
    case  FILELOCAL::STATE_Loading:
      if(m_pUpperView_Photo->IsLoaded())
      {
        if(GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd(gfl2::Fade::DISP_DOUBLE))
        {
          m_FrameCount  = FILELOCAL::PreShowFrame;
          m_State = FILELOCAL::STATE_PreShow;
        }
      }
      break;

    /*  表示待ち  */
    case FILELOCAL::STATE_PreShow:
      if(m_FrameCount)  --m_FrameCount;

      if(!m_FrameCount)
      {
        /*  表示許可  */
        m_pLowerView_Info->SetData(m_JFPlayerDataListItr->name, m_JFPlayerDataListItr->photoData.photo_param);
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_CROSS, FILELOCAL::FadeFrame);
        m_State = FILELOCAL::STATE_Show;
      }
      break;

    /*  表示待ち  */
    case  FILELOCAL::STATE_Show:
      if(GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd(gfl2::Fade::DISP_DOUBLE))
      {
        /*  現在の写真を閲覧済みにする  */
        m_JFPlayerDataListItr->Showed();

        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_CROSS, &FILELOCAL::fadeColor_Start, &FILELOCAL::fadeColor_End, 1);
        if(1 < m_pWork->m_pAppParam->m_pPlayList->Size())
        {
           /*  次があればrequest  */
          ++m_JFPlayerDataListItr;
          if(m_JFPlayerDataListItr == m_JFPlayerDataListItr_End)  m_JFPlayerDataListItr = m_JFPlayerDataListItr_Begin;
          RequestPhoto();
          m_FrameCount = FILELOCAL::SlideShowFrame;
        }
        m_State = FILELOCAL::STATE_Showing;
      }
      break;
      
    case  FILELOCAL::STATE_Showing:
      if(1 < m_pWork->m_pAppParam->m_pPlayList->Size()) /*  次があればロード待ち  */
      {
        if(m_FrameCount)
        {
          if(!(--m_FrameCount)) m_State = FILELOCAL::STATE_Loading;
        }
      }
      break;

    /**
      @fix  MMcat[496:   【フェスサークル】特定のタイミングでスライドショー画面から抜けると、次の写真が一瞬表示される
        アプリ終了時に、TYPE_CROSSで表示していた前の画像が透明の黒となり、裏読みしていた次の写真が一瞬表示されてしまう
        FrameWorkに任せていたFadeを抑制し、手動で行う
        上画面は補間なし、下画面は通常
    */
    case  FILELOCAL::STATE_FadeOut:
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, &FILELOCAL::fadeColor_Start, &FILELOCAL::fadeColor_End, 1);
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA, &FILELOCAL::fadeColor_Start, &FILELOCAL::fadeColor_End, gfl2::Fade::FADE_DEFAULT_SYNC);
      m_State = FILELOCAL::STATE_WaitForFade;
      /*  fall through  */
    
    case  FILELOCAL::STATE_WaitForFade:
      if(GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd(gfl2::Fade::DISP_DOUBLE))
      {
        m_State = FILELOCAL::STATE_Exit;
      }
      break;


    case  FILELOCAL::STATE_Exit:
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
bool  Frame_Main::cleanup()
{
  return true;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_Main::LowerView_Info_OnEvent(LowerView_Info* pSender, const LowerView_Info::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  LowerView_Info::IEventHandler::EVENT_Exit:    ExitFrame(RESULT_MAIN_Exit);    break;
    case  LowerView_Info::IEventHandler::EVENT_Cancel:  ExitFrame(RESULT_MAIN_Cancel);  break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_Main::ExitFrame(const FrameResult frameResult)
{
  m_Result  = frameResult;
  /**
    @fix  MMcat[496:   【フェスサークル】特定のタイミングでスライドショー画面から抜けると、次の写真が一瞬表示される
      アプリ終了時に、TYPE_CROSSで表示していた前の画像が透明の黒となり、裏読みしていた次の写真が一瞬表示されてしまう
      FrameWorkに任せていたFadeを抑制し、手動で行う
      アプリ終了時のFadeOutを行うStateを追加
  */
  m_State   = FILELOCAL::STATE_FadeOut;
  m_pViewContainer->SetAllInputEnable(false);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_Main::SlideShow_Update(void)
{
  if(m_pWork->m_pAppParam->m_pPlayList->Size() <= 1)  return;

  if(m_FrameCount)
  {
    if(!(--m_FrameCount))
    {
      ++m_JFPlayerDataListItr;
      if(m_JFPlayerDataListItr == m_JFPlayerDataListItr_End)  m_JFPlayerDataListItr = m_JFPlayerDataListItr_Begin;

      RequestPhoto();

      m_FrameCount  = FILELOCAL::SlideShowFrame;
      m_State       = FILELOCAL::STATE_Loading;
    }
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_Main::RequestPhoto(void)
{
  const APP_PARAM::PhotoInfo& rPhotoInfo  = *m_JFPlayerDataListItr;

  m_pUpperView_Photo->Request(rPhotoInfo.photoData);
}



} /*  namespace PhotoView    */
} /*  namespace NetApp  */
