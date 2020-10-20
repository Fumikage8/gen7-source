
#if PM_DEBUG

//==============================================================================
/**
 * @file   QRReaderDebug.cpp
 * @date   2016/02/05 Fri. 16:40:23
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <DeviceCamera/include/gfl2_DeviceCamera.h>

#include  "GameSys/include/GameManager.h"
#include  "GameSys/include/GameData.h"

#include  "../QRReaderResource.h"
#include  "../System/QRReaderWork.h"
#include  "./QRReaderDebug.h"

#include <Debug/DebugWin/include/DebugWinSystem.h>
// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"


// =============================================================================
/**/
//==============================================================================
namespace
{
  bool IsTrigA(void)
  {
    gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

    return (pButton->IsTrigger(gfl2::ui::BUTTON_A));
  }


  NetApp::QRReader::Debug::QRReaderDebug*  ToThis(void* _pThis)
  {
    return static_cast<NetApp::QRReader::Debug::QRReaderDebug*>(_pThis);
  }


  void  CamForceFatal(void)
  {
#if defined(GF_PLATFORM_CTR)
    gfl2::devicecamera::DeviceCamera*   pDeviceCamera = GFL_SINGLETON_INSTANCE(gfl2::devicecamera::DeviceCamera);
    
    if(pDeviceCamera)
    {
      pDeviceCamera->ForceFatal();
    }
#endif
  }
}




namespace NetApp    {
namespace QRReader  {
namespace Debug     {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
QRReaderDebug::QRReaderDebug(QRReaderWork* pWork)
  : m_pWork(pWork)
  , m_pGroup_Root(NULL)
{
  ::Debug::DebugWin::DebugWinWork*  devWork   = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  gfl2::str::MsgData*               pMsgData  = devWork->GetMessageData();
  gfl2::heap::HeapBase*             pHeapBase = m_pWork->GetDeviceHeap();

  /*  QRReader  */
  m_pGroup_Root = gfl2::debug::DebugWin_AddGroup(m_pWork->GetDeviceHeap(), pMsgData, msg_dmenu_qrreader);


  /*  win  */
#define VALDEF(_ItemID, _CaptionID, _min, _max)   do{ \
                                                    m_ViewItemInfos[(_ItemID)].pThis      = this;  \
                                                    m_ViewItemInfos[(_ItemID)].pViewItem  = gfl2::debug::DebugWin_AddItemValueU32(m_pGroup_Root, pHeapBase, pMsgData, _CaptionID, &m_ViewItemInfos[(_ItemID)].value, (_min), (_max)); \
                                                    m_ViewItemInfos[(_ItemID)].value      = 0; \
                                                  }while(false)

#define VALDEFDIRECT(_ItemID, _Caption, _min, _max)   do{ \
                                                    m_ViewItemInfos[(_ItemID)].pThis      = this;  \
                                                    m_ViewItemInfos[(_ItemID)].pViewItem  = gfl2::debug::DebugWin_AddItemValueU32(m_pGroup_Root, pHeapBase, _Caption, &m_ViewItemInfos[(_ItemID)].value, (_min), (_max)); \
                                                    m_ViewItemInfos[(_ItemID)].value      = 0; \
                                                  }while(false)

#define PROCDEF(_CaptionID, _ptrFunc)             do{ \
                                                    gfl2::debug::DebugWin_AddItemUpdateFunc(m_pGroup_Root, pHeapBase, pMsgData, _CaptionID, this, _ptrFunc);  \
                                                  }while(false)

#define PROCDEFDIRECT(_Caption, _ptrFunc)       do{ \
                                                    gfl2::debug::DebugWin_AddItemUpdateFunc(m_pGroup_Root, pHeapBase, _Caption, this, _ptrFunc);  \
                                                  }while(false)


    PROCDEF(msg_dmenu_qrreader_full, FullTank);

    VALDEF( VALITEM_Battery,       msg_dmenu_qrreader_battery,           0, Savedata::QRReaderSaveData::QRREAD_BATTERY_MAX);
    PROCDEF(msg_dmenu_qrreader_battery_set,           SetBattery);

    VALDEF( VALITEM_ScanPower,     msg_dmenu_qrreader_scanpower,         0, Savedata::QRReaderSaveData::FIELDSCAN_SCANPOWER_MAX);
    PROCDEF(msg_dmenu_qrreader_scanpower_set,         SetScanPower);

    VALDEF( VALITEM_ScanRemaining, msg_dmenu_qrreader_fieldscan_remain,  0, Savedata::QRReaderSaveData::FIELDSCAN_DURATION_SECONDS);
    PROCDEF(msg_dmenu_qrreader_fieldscan_remain_set,  SetScanRemaining);

    PROCDEF(msg_dmenu_qrreader_tutorial_reset,        ResetTutorial);     /*  チュートリアルフラグリセット  */
    PROCDEF(msg_dmenu_qrreader_history_reset,         ResetQRHistory);    /*  QRよみこみりれきリセット      */

    VALDEFDIRECT(VALITEM_CameraForceFatal, L"Cam force fatal", 0, 1);

#undef  PROCDEFDIRECT
#undef  VALDEFDIRECT
#undef  PROCDEF
#undef  VALDEF


/*
  {
    gfl2::heap::HeapBase* pHeapBase = m_pWork->GetDeviceHeap();
    
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_pGroup_Root, pHeapBase, pMsgData, msg_dmenu_evolution_do_signal, this, DoSignal);   // "<DoSignal>"
  }
*/
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
QRReaderDebug::~QRReaderDebug()
{
  gfl2::debug::DebugWin_RemoveGroup(m_pGroup_Root);
}


void QRReaderDebug::FullTank(void* _pThis, gfl2::debug::DebugWinItem* pItem)
{
  if(IsTrigA())
  {
    QRReaderDebug*  pThis = ToThis(_pThis);
    
    Savedata::QRReaderSaveData*           pSave = GetQRReaderSaveData();
    Savedata::QRReaderSaveData::CoreData* pCore = static_cast<Savedata::QRReaderSaveData::CoreData*>(pSave->GetData());

    pCore->battery            = Savedata::QRReaderSaveData::QRREAD_BATTERY_MAX;
    pCore->scanPower          = Savedata::QRReaderSaveData::FIELDSCAN_SCANPOWER_MAX;
    pCore->fieldSacnStartDate = GameSys::INijiDate::GetDateToSecond(*(GameSys::DeviceDate().GetDate()));
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  QRReaderDebug::OnUpdate(void)
{
  if(m_ViewItemInfos[VALITEM_CameraForceFatal].value)
  {
    CamForceFatal();
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  QRReaderDebug::OnDraw(const gfl2::gfx::CtrDisplayNo displayNo)
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void QRReaderDebug::SetBattery(void* _pThis, gfl2::debug::DebugWinItem* pItem)
{
  if(IsTrigA())
  {
    QRReaderDebug*                        pThis = ToThis(_pThis);
    Savedata::QRReaderSaveData*           pSave = GetQRReaderSaveData();
    Savedata::QRReaderSaveData::CoreData* pCore = static_cast<Savedata::QRReaderSaveData::CoreData*>(pSave->GetData());

    pCore->battery  = pThis->m_ViewItemInfos[VALITEM_Battery].value;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void QRReaderDebug::SetScanPower(void* _pThis, gfl2::debug::DebugWinItem* pItem)
{
  if(IsTrigA())
  {
    QRReaderDebug*                        pThis = ToThis(_pThis);
    Savedata::QRReaderSaveData*           pSave = GetQRReaderSaveData();
    Savedata::QRReaderSaveData::CoreData* pCore = static_cast<Savedata::QRReaderSaveData::CoreData*>(pSave->GetData());

    pCore->scanPower  = pThis->m_ViewItemInfos[VALITEM_ScanPower].value;
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void QRReaderDebug::SetScanRemaining(void* _pThis, gfl2::debug::DebugWinItem* pItem)
{
  if(IsTrigA())
  {
    QRReaderDebug*                        pThis = ToThis(_pThis);
    Savedata::QRReaderSaveData*           pSave = GetQRReaderSaveData();
    Savedata::QRReaderSaveData::CoreData* pCore = static_cast<Savedata::QRReaderSaveData::CoreData*>(pSave->GetData());

    pCore->fieldSacnStartDate = GameSys::INijiDate::GetDateToSecond(*(GameSys::DeviceDate().GetDate())) - (Savedata::QRReaderSaveData::FIELDSCAN_DURATION_SECONDS - pThis->m_ViewItemInfos[VALITEM_ScanRemaining].value);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void QRReaderDebug::ResetTutorial(    void* _pThis, gfl2::debug::DebugWinItem* pItem)
{
  if(IsTrigA())
  {
    QRReaderDebug*                        pThis = ToThis(_pThis);
    Savedata::QRReaderSaveData*           pSave = GetQRReaderSaveData();
    Savedata::QRReaderSaveData::CoreData* pCore = static_cast<Savedata::QRReaderSaveData::CoreData*>(pSave->GetData());
    
    pCore->bTutorialFinished  = static_cast<b8>(false);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void QRReaderDebug::ResetQRHistory(   void* _pThis, gfl2::debug::DebugWinItem* pItem)
{
  if(IsTrigA())
  {
    QRReaderDebug*                        pThis = ToThis(_pThis);
    Savedata::QRReaderSaveData*           pSave = GetQRReaderSaveData();
    Savedata::QRReaderSaveData::CoreData* pCore = static_cast<Savedata::QRReaderSaveData::CoreData*>(pSave->GetData());
    
    pSave->ResetScannedDataHistory();
  }
}




} /*  namespace Debug     */
} /*  namespace Evolution */
} /*  namespace NetApp    */

#endif  /*  #if PM_DEMUG  */
