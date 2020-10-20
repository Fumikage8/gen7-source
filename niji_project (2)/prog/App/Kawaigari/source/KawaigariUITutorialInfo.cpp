//======================================================================
/**
 * @file	KawaigariUITutorialInfo.C++
 * @brief	Kawaigari Tutorial Info UI Interface 
 * @author	PETE
 * @data	2016.03.17
 */
//======================================================================
#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>
#include <GameSys/include/GameProcManager.h>
// UI Base:
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"

#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookType.h>
#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookTypeListener.h>

#include "KawaigariUIManager.h"

#include "KawaigariMain.h"
#include "KawaigariRenderSystem.h"

#include "niji_conv_header/message/msg_pokeparure.h"

#include <AppLib/include/Tool/app_tool_LytTexData.h>

#include "niji_conv_header/app/kawaigari/res2d/kw_layout.h"
#include "niji_conv_header/app/kawaigari/res2d/kw_layout_pane.h"
#include "niji_conv_header/app/kawaigari/res2d/kw_layout_anim_list.h"


GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)


  //==============================================================================
  //
  //  KawadaTestInfoWindowSampleInfoWindowBookTypeListener
  //
  //==============================================================================
class KawaigariInfoWindowBookTypeListener : public app::tool::InfoWindowBookTypeListener
{
  GFL_FORBID_COPY_AND_ASSIGN(KawaigariInfoWindowBookTypeListener);

public:
  KawaigariInfoWindowBookTypeListener(KawaigariUITutorialInfo *coreInfo)
    : app::tool::InfoWindowBookTypeListener()    ,
    mCoreInfo(coreInfo)
  {}
  virtual ~KawaigariInfoWindowBookTypeListener()
  {}

  virtual void OnClose(void)
  {
    GFL_PRINT("InfoWindowBookTypeListener::OnClose\n");
    mCoreInfo->SetIsTutorialOver(true);
  }

  virtual void OnChangePage(u32 pageNo)
  {   
  }

  virtual void OnCommand(u32 pageNo)
  {   
    
  }

private:
  //KawadaTestInfoWindowSampleFrameCell* m_frameCell;
  KawaigariUITutorialInfo *mCoreInfo;
  
};




KawaigariUITutorialInfo::KawaigariUITutorialInfo(void)
{    
  SetState(GUI_STATE_INIT);
  mInit = false;
  mIsFinishedTutorial = false;
}

KawaigariUITutorialInfo::~KawaigariUITutorialInfo(void)
{  
}

void KawaigariUITutorialInfo::Setup(void)
{
  mInit = true;

  // MsgData
  {
    for(u32 i=0; i<STR_BUF_NUM; ++i)
    {
      m_strBuf[i] = GFL_NEW(coreUISys->GetAppHeap()->GetDeviceHeap()) gfl2::str::StrBuf( STR_BUF_CHAR_NUM_MAX, coreUISys->GetAppHeap()->GetDeviceHeap() );
    }    
  }

  mInfoWindowBookType = GFL_NEW(coreUISys->GetAppHeap()->GetDeviceHeap()) app::tool::InfoWindowBookType(coreUISys->GetAppHeap());
  mBook = mInfoWindowBookType;

  mInfoWindowBookTypeListener = GFL_NEW(coreUISys->GetAppHeap()->GetDeviceHeap()) KawaigariInfoWindowBookTypeListener(this);
  mBookListener = mInfoWindowBookTypeListener;

  mInfoWindowBookType->SyncCreate(
    coreUISys->GetAppHeap(),    
    coreUISys->GetCoreSys()->GetRenderSystem()->GetAppRenderingManager(),
    4,
    NULL
    );

  coreUISys->AddSubView(mInfoWindowBookType);

  mInfoWindowBookType->ResetContents(4, true);

  gfl2::str::StrBuf* strBuf0 = coreUISys->GetG2DUtil()->GetTempStrBuf(0);

  for(u32 pageNo=0; pageNo<4; ++pageNo)
  {
    u32 strBufIndex0 = pageNo*2;
    u32 strBufIndex1 = strBufIndex0+1;
  
    coreUISys->GetMessage()->GetString(msg_pkparure_info_01+pageNo, *strBuf0);
    coreUISys->GetWordSet()->Expand(m_strBuf[strBufIndex0], strBuf0);
    coreUISys->GetMessage()->GetString(msg_pkparure_info_01+pageNo, *strBuf0);
    coreUISys->GetWordSet()->Expand(m_strBuf[strBufIndex1], strBuf0);

    mInfoWindowBookType->SetTextureAndTextContent(
      pageNo,
      coreUISys->GetTexInfoRes()->GetTexture(LYTRES_KW_LAYOUT_KAWAGARI_INFO1_BFLIM+pageNo,0),
      m_strBuf[strBufIndex0],
      NULL);
  }

  
}

void KawaigariUITutorialInfo::SetState(int s) 
{ 
  mStateReg=s; 
  mStateRegSub=0; 
  for(int r=0;r<MAX_UI_STATE_REGS;r++) mStateRegs[r] = 0;

}
void KawaigariUITutorialInfo::SetSubState(int s) 
{ 
  mStateRegSub=s; 
  for(int r=0;r<MAX_UI_STATE_REGS;r++) mStateRegs[r] = 0;
}

void KawaigariUITutorialInfo::PrintState(void)
{
  GFL_PRINT("TutorialInfoState: %i  %i -> ",mStateReg,mStateRegSub);

  for(int r=0;r<MAX_UI_STATE_REGS;r++)
  {
    if(r && (r&3)==0) GFL_PRINT("  ");
    GFL_PRINT("%03i ",mStateRegs[r]);    
  }

  GFL_PRINT("\n");
}

void KawaigariUITutorialInfo::State_Init(void)
{     
  if(!mInit) return;
  //SetState(GUI_STATE_MAIN);  
  //UpdateAgain();
}

void KawaigariUITutorialInfo::State_Main(void)
{    
  switch(mStateRegSub)
  {
    case 0:
      {      
        gfl2::math::Vector3 pos(160, 120, 0);
        mBook->SetBgVisible(true);
        mBook->StartOpen(
          mBookListener,
          pos,
          app::tool::InfoWindowBookType::OPEN_TYPE_NO_ANIM
          );
      }

      mStateRegSub++;
      break;
  }
}

void KawaigariUITutorialInfo::State_End(void)
{  
  switch(mStateRegSub)
  {
    case 0:
      mBook->RemoveFromSuperView();   // InfoWindowBookType‚Ìe‚Å‚ ‚éapp::ui::UIView‚Ìƒƒ“ƒoŠÖ”
      mStateRegSub++;
      break;

    case 1:      
      if(mBook->EndFunc()) mStateRegSub++;
      break;

    case 2:
      for(u32 i=0; i<STR_BUF_NUM; ++i)
      {
        GFL_SAFE_DELETE(m_strBuf[i]);
      }

      GFL_SAFE_DELETE(mBook);
      GFL_SAFE_DELETE(mBookListener);
      
      SetState(GUI_STATE_FINISH);
      break;
  }
}

void KawaigariUITutorialInfo::State_Cleanup(void)
{  
}

void KawaigariUITutorialInfo::State_Finish(void)
{  
}

void KawaigariUITutorialInfo::Finish(void)
{
  SetState(GUI_STATE_END);
}

bool KawaigariUITutorialInfo::IsFinish(void)
{
  return(mStateReg==GUI_STATE_FINISH); 
}

void KawaigariUITutorialInfo::Update(void)
{      

  do 
  {
    mRequireUpdate = false;

    switch(mStateReg)
    {
    case GUI_STATE_INIT: State_Init(); break;
    case GUI_STATE_MAIN: State_Main(); break;    
    case GUI_STATE_END: State_End(); break;
    case GUI_STATE_CLEANUP: State_Cleanup(); break;
    case GUI_STATE_FINISH: State_Finish(); break;
    }

  } while (mRequireUpdate);


  // Print State Debug:
  /// PrintState();
}


  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)