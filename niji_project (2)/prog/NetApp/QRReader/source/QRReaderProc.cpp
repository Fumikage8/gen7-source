﻿//==============================================================================
/**
 * @file   QRReaderProc.cpp
 * @date   2016/01/14 Thu. 11:48:06
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

#include  "./System/QRReaderWork.h"
#include  "./System/QRReaderApplicationSystem.h"
#include  "./System/QRReaderBuilder.h"
#include  "./QRReaderFrameDef.h"
#include  "./QRReaderResource.h"
#include  "../include/QRReaderProc.h"


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


namespace NetApp    {
namespace QRReader {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     QRReaderProc
  *  @brief    ctor
  */
//------------------------------------------------------------------
QRReaderProc::QRReaderProc( APP_PARAM* pAppParam )
  : GameSys::GameProc()
  , m_pAppParam(pAppParam)
  , m_pApplicationSystem(NULL)
  , m_pWork(NULL)
{
}

//------------------------------------------------------------------
/**
  *  @func     ~QRReaderProc
  *  @brief    dtor
  */
//------------------------------------------------------------------
QRReaderProc::~QRReaderProc()
{
}


//------------------------------------------------------------------
/**
  *  @func     InitFunc
  *  @brief    初期化
  */
//------------------------------------------------------------------
gfl2::proc::Result  QRReaderProc::InitFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result      result    = gfl2::proc::RES_CONTINUE;
  gfl2::heap::HeapBase*   pHeapBase = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  switch(GetSubSeq())
  {
    case  SUBSEQ_INIT_Initial:
      /*  applicationSystem  */
      m_pWork               = GFL_NEW(pHeapBase)  QRReaderWork();
      m_pWork->m_pAppParam  = m_pAppParam;

      {
        const FrameID    firstFrame  = GetFirstFrameID();   /*  最初のFrame  */
        QRReaderBuilder  builder( m_pWork, firstFrame );

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
gfl2::proc::Result QRReaderProc::EndFunc( gfl2::proc::Manager* pProcManager )
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
gfl2::proc::Result QRReaderProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
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
void  QRReaderProc::PreDrawFunc( gfl2::proc::Manager* pProcManager )
{
  m_pApplicationSystem->PreDraw();
}


//------------------------------------------------------------------
/**
  *  @func     DrawFunc
  *  @brief    描画
  */
//------------------------------------------------------------------
void  QRReaderProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
{
//  GFL_PRINT("%s\n", __FUNCTION__);
/*
    LEFT,           //!< 3DSの左画面 400x240
    RIGHT,          //!< 3DSの右画面 400x240
    DOWN,           //!< 3DSの下画面 320x240
*/

//  GFL_PRINT(" >display %d\n", static_cast<u32>(displayNo));

  m_pApplicationSystem->Draw( displayNo );
#if PM_DEBUG
  m_pApplicationSystem->DebugDraw( displayNo );
#endif
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
FrameID QRReaderProc::GetFirstFrameID(void) const
{
  FrameID result  = FRAME_QRSimpleRead;

  switch(m_pWork->m_pAppParam->m_BootMode)
  {
    case  APP_PARAM::BOOTMODE_QRScan:
    {
      if(GetQRReaderSaveData()->IsFieldScanActive() || GetQRReaderSaveData()->IsScanPowerEnoughForFieldScan())
      {
        result  = FRAME_FieldScan;
      }
      else
      {
        result  = FRAME_Tutorial;
      }
    }
      break;

    case  APP_PARAM::BOOTMODE_FriendMatch:
    case  APP_PARAM::BOOTMODE_LiveMatch:
    case  APP_PARAM::BOOTMODE_BattleTeam:
      result  = FRAME_QRSimpleRead;
      break;

    default:
      GFL_ASSERT(false);
      break;
  }

  return result;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
Savedata::QRReaderSaveData*   GetQRReaderSaveData(void)
{
  return GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetQRReaderSave();
}


} /*  namespace QRReader  */
} /*  namespace NetApp    */

