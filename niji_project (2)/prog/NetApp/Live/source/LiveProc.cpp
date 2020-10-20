//==============================================================================
/**
 * @file   LiveProc.cpp
 * @date   2015/07/09 Thu. 13:08:19
 * @author muto_yu
 * @brief  ライブ大会proc
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================

// =============================================================================
/**/
//==============================================================================
#include  <System/include/HeapDefine.h>
#include  <NetStatic/NetAppLib/include/System/ApplicationSystemBase.h>

#include  "./ApplicationSystem/LiveWork.h"
#include  "./ApplicationSystem/LiveApplicationSystem.h"
#include  "./ApplicationSystem/LiveBuilder.h"
#include  "./LiveFrameDef.h"
#include  "../include/LiveProc.h"

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





namespace NetApp  {
namespace Live {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     LiveProc
  *  @brief    ctor
  */
//------------------------------------------------------------------
LiveProc::LiveProc( APP_PARAM* pAppParam )
  : GameSys::GameProc()
  , m_pAppParam(pAppParam)
  , m_pApplicationSystem(NULL)
  , m_pWork(NULL)
{
  m_pAppParam->m_Result = APPRESULT_Initial;
}

//------------------------------------------------------------------
/**
  *  @func     ~LiveProc
  *  @brief    dtor
  */
//------------------------------------------------------------------
LiveProc::~LiveProc()
{
}


//------------------------------------------------------------------
/**
  *  @func     InitFunc
  *  @brief    初期化
  */
//------------------------------------------------------------------
gfl2::proc::Result  LiveProc::InitFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result      result    = gfl2::proc::RES_CONTINUE;
  gfl2::heap::HeapBase*   pHeapBase = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  switch(GetSubSeq())
  {
    case  SUBSEQ_INIT_Initial:
      /*  applicationSystem  */
      m_pWork                 = GFL_NEW(pHeapBase)  LiveWork();

      m_pWork->SetupCountryRegionNameCache();
      m_pWork->m_pAppParam    = m_pAppParam;
      {
        LiveBuilder    builder( m_pWork, GetFirstFrameID() );

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
gfl2::proc::Result LiveProc::EndFunc( gfl2::proc::Manager* pProcManager )
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
gfl2::proc::Result LiveProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result  result  = gfl2::proc::RES_CONTINUE;

//  GFL_PRINT("%s\n", __FUNCTION__);


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
void  LiveProc::PreDrawFunc( gfl2::proc::Manager* pProcManager )
{
  m_pApplicationSystem->PreDraw();
}


//------------------------------------------------------------------
/**
  *  @func     DrawFunc
  *  @brief    描画
  */
//------------------------------------------------------------------
void  LiveProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
{
//  GFL_PRINT("%s\n", __FUNCTION__);
/*
    LEFT,           //!< 3DSの左画面 400x240
    RIGHT,          //!< 3DSの右画面 400x240
    DOWN,           //!< 3DSの下画面 320x240
*/

//  GFL_PRINT(" >display %d\n", static_cast<u32>(displayNo));


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
FrameID LiveProc::GetFirstFrameID(void)
{
  switch(m_pAppParam->m_BootMode)
  {
    case  BOOTMODE_Top:                       return  FRAME_Main;
    case  BOOTMODE_CeckRegulation_FromQRRead: return  FRAME_ReceivePlayerPass;
    case  BOOTMODE_TeamSelect_Selected:       return  FRAME_Registration;
    case  BOOTMODE_TeamSelect_Canceled:       return  FRAME_Registration;
    case  BOOTMODE_BattleResult:              return  FRAME_MatchResult;
    case  BOOTMODE_MatchMenu:                 return  FRAME_MatchMenu;

    case  BOOTMODE_None:
    default:
      GFL_ASSERT(0);
      return FRAME_INVALID;
  }

}



} /*  namespace Live */
} /*  namespace NetApp      */

