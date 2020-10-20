//==============================================================================
/**
 * @file   GiftProc.cpp
 * @date   2015/07/31 Fri. 15:03:43
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

#include  "./ApplicationSystem/GiftWork.h"
#include  "./ApplicationSystem/GiftApplicationSystem.h"
#include  "./ApplicationSystem/GiftBuilder.h"
#include  "./Common/GiftDeliveryUtil.h"
#include  "./GiftFrameDef.h"
#include  "./GiftResource.h"
#include  "../include/GiftProc.h"



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
namespace Gift    {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     GiftProc
  *  @brief    ctor
  */
//------------------------------------------------------------------
GiftProc::GiftProc( APP_PARAM* pAppParam )
  : GameSys::GameProc()
  , m_pAppParam(pAppParam)
  , m_pApplicationSystem(NULL)
  , m_pWork(NULL)
{
  m_pAppParam->ClearResult();
}

//------------------------------------------------------------------
/**
  *  @func     ~GiftProc
  *  @brief    dtor
  */
//------------------------------------------------------------------
GiftProc::~GiftProc()
{
}


//------------------------------------------------------------------
/**
  *  @func     InitFunc
  *  @brief    初期化
  */
//------------------------------------------------------------------
gfl2::proc::Result  GiftProc::InitFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result      result    = gfl2::proc::RES_CONTINUE;
  gfl2::heap::HeapBase*   pHeapBase = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  switch(GetSubSeq())
  {
    case  SUBSEQ_INIT_Initial:
      /*  applicationSystem  */
      m_pWork  = GFL_NEW(pHeapBase)  GiftWork();
      m_pWork->m_pAppParam  = m_pAppParam;

      {
        const FrameID startFrame  = (m_pAppParam->bootMode == APP_PARAM::BOOTMODE_FromReceiveDemo ? FRAME_AFTER_DEMO : FRAME_STARTUP);

        GiftBuilder    builder( m_pWork, startFrame );

        m_pApplicationSystem  = GFL_NEW(pHeapBase)  ApplicationSystem(m_pWork);
        m_pApplicationSystem->Build(&builder);
      }
      DeliveryUtil::Create(m_pWork);
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
gfl2::proc::Result GiftProc::EndFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result  result  = gfl2::proc::RES_CONTINUE;

  // 終了処理が完了するまで待つ
  if( m_pApplicationSystem->End() )
  {
    return gfl2::proc::RES_CONTINUE;
  }

  /*  applicationSystem  */
  DeliveryUtil::Destroy();
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
gfl2::proc::Result GiftProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
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
void  GiftProc::PreDrawFunc( gfl2::proc::Manager* pProcManager )
{
  m_pApplicationSystem->PreDraw();
}


//------------------------------------------------------------------
/**
  *  @func     DrawFunc
  *  @brief    描画
  */
//------------------------------------------------------------------
void  GiftProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
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
  m_pApplicationSystem->DebugDraw(displayNo);
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


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
Savedata::MysteryGiftSave*   GetMysteryGiftSave(void)
{
  return GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetMysteryGiftSave();
}




} /*  namespace Live */
} /*  namespace NetApp      */

