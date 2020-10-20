//==============================================================================
/**
 * @file   BattleSpotProc.cpp
 * @date   2015/10/02 Fri. 12:21:37
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <System/include/HeapDefine.h>
#include  <NetStatic/NetAppLib/include/System/ApplicationSystemBase.h>

#include  "./System/BattleSpotWork.h"
#include  "./System/BattleSpotApplicationSystem.h"
#include  "./System/BattleSpotBuilder.h"
#include  "./BattleSpotFrameDef.h"
#include  "../include/BattleSpotProc.h"


// =============================================================================
/**/
//==============================================================================
namespace 
{
  enum
  {
    SUBSEQ_INIT_Initial = 0,
    SUBSEQ_INIT_Loading,

    SUBSEQ_
  };
}


namespace NetApp      {
namespace BattleSpot  {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     BattleSpotProc
  *  @brief    ctor
  */
//------------------------------------------------------------------
BattleSpotProc::BattleSpotProc( APP_PARAM* pAppParam )
  : GameSys::GameProc()
  , m_pAppParam(pAppParam)
  , m_pApplicationSystem(NULL)
  , m_pWork(NULL)
{
}


//------------------------------------------------------------------
/**
  *  @func     ~BattleSpotProc
  *  @brief    dtor
  */
//------------------------------------------------------------------
BattleSpotProc::~BattleSpotProc()
{
}


//------------------------------------------------------------------
/**
  *  @func     InitFunc
  *  @brief    初期化
  */
//------------------------------------------------------------------
gfl2::proc::Result  BattleSpotProc::InitFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result      result    = gfl2::proc::RES_CONTINUE;
  gfl2::heap::HeapBase*   pHeapBase = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  switch(GetSubSeq())
  {
    case  SUBSEQ_INIT_Initial:
      /*  applicationSystem  */
      m_pWork = GFL_NEW(pHeapBase)  BattleSpotWork();
      
      m_pWork->SetupCountryRegionNameCache();
      m_pWork->m_pAppParam = m_pAppParam;
      {
        BattleSpotBuilder   builder( m_pWork, GetFirstFrameID() );  /*  最初のFrame  */

        m_pApplicationSystem  = GFL_NEW(pHeapBase)  ApplicationSystem(m_pWork);
        m_pApplicationSystem->Build(&builder);
      }
      SetSubSeq(SUBSEQ_INIT_Loading);
      break;

    case  SUBSEQ_INIT_Loading:
      if(!m_pApplicationSystem->Loading())
      {
        result  = gfl2::proc::RES_FINISH;
      }
      break;

    default:
      break;
  }


  return result;
}


//------------------------------------------------------------------
/**
  *  @func     EndFunc
  *  @brief    終了
  */
//------------------------------------------------------------------
gfl2::proc::Result BattleSpotProc::EndFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result  result  = gfl2::proc::RES_CONTINUE;

  // 終了処理が完了するまで待つ
  if( m_pApplicationSystem->End() )
  {
    return gfl2::proc::RES_CONTINUE;
  }

  /*  applicationSystem  */
  GFL_SAFE_DELETE(m_pApplicationSystem);
  GFL_SAFE_DELETE(m_pWork);

  result  = gfl2::proc::RES_FINISH;


  return result;
}


//------------------------------------------------------------------
/**
  *  @func     UpdateFunc
  *  @brief    更新
  */
//------------------------------------------------------------------
gfl2::proc::Result BattleSpotProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result  result  = gfl2::proc::RES_CONTINUE;


  /*  appsystem  */
  if(m_pApplicationSystem->Update())
  {
    /*  稼動Frameがなくなるとここへ  */
    result  = gfl2::proc::RES_FINISH;
  }


  return result;
}


//------------------------------------------------------------------
/**
  *  @func     PreDrawFunc
  *  @brief    描画準備
  */
//------------------------------------------------------------------
void  BattleSpotProc::PreDrawFunc( gfl2::proc::Manager* pProcManager )
{
  m_pApplicationSystem->PreDraw();
}


//------------------------------------------------------------------
/**
  *  @func     DrawFunc
  *  @brief    描画
  */
//------------------------------------------------------------------
void  BattleSpotProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pApplicationSystem->Draw( displayNo );
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
FrameID BattleSpotProc::GetFirstFrameID(void) const
{
#if PM_DEBUG
#if defined( DEBUG_ONLY_FOR_muto_yu )
  { 
    gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

    if(pButton->IsHold(gfl2::ui::BUTTON_L))
    {
      return FRAME_ViewTest;
    }
  }
#endif
#endif

  return  static_cast<FrameID>( m_pAppParam->nextFrame );
}






} /*  namespace BattleSpot  */
} /*  namespace NetApp      */

