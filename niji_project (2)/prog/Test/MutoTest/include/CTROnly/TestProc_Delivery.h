
#if !defined(TESTPROC_DELIVERY_H_INCLUDED)
#define TESTPROC_DELIVERY_H_INCLUDED
#pragma once
#if PM_DEBUG
//==============================================================================
/**
  *  @file   TestProc_Delivery.h
  *  @brief  
  *  @date   2015.06.11 Thu.
  *  @author Yu Muto
  */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_Macros.h>
#include  <GameSys/include/GameProcManager.h>
#include  <AppLib/include/Frame/AppFrameManager.h>
#include  <AppLib/include/Util/app_util_heap.h>

#include  <NetStatic/NetAppLib/include/debug/NetAppDebugTextDrawer.h>

/*  deliv  */
#include  <NetStatic/NetLib/include/Delivery/DeliveryManager.h>
#include  <NetStatic/NetLib/include/Delivery/DeliverySendListener.h>
#include  <NetStatic/NetLib/include/Delivery/DeliveryRecvListener.h>

#include  "../MutoTest_TestProc.h"




namespace Test  {
namespace MutoTest  {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015.06.11 Thu.
  */
//==============================================================================
#if defined(GF_PLATFORM_CTR)


class TestProc_Delivery
  : public GameSys::GameProc
  , public NetLib::Delivery::DeliverySendListener
  , public NetLib::Delivery::DeliveryRecvListener
{
  GFL_FORBID_COPY_AND_ASSIGN(TestProc_Delivery);

public:
  /** ctor  */
  TestProc_Delivery(void);

  /** dtor  */
  virtual ~TestProc_Delivery();


  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager);
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pManager);
  virtual void PreDrawFunc( gfl2::proc::Manager* pManager );
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);


  /*
    NetLib::Delivery::DeliverySendListener
  */
  virtual void OnNetworkError( const nn::Result &result );


  /*
    NetLib::Delivery::DeliveryRecvListener
  */
  virtual bool OnRecvComplete( const void* pData , size_t dataSize );
  virtual void OnUpdateProgress( const u32 progress );
  virtual void OnNintendoZoneConnected();
  virtual void OnNetworkError( const nn::Result &result , u32 errorCode );
  virtual void OnWifiError( const u32 errorCode );









private:
  void  CreateHeap(void);
  void  DeleteHeap(void);

  void  UpdateFunc_Select(void);
  void  RefreshList(void);


  app::util::Heap                         m_Heap;
  NetAppLib::Debug::NetAppDebugTextDrawer m_TextDrawer;

  /*  archive  */
  u32                       m_SelectIndex_TargetArchive;
  u32                       m_SelectIndexMax_TargetArchive;

  /*  connectionMode  */
  u32                       m_SelectIndex_ConnectionMode;
  u32                       m_SelectIndexMax_ConnectionMode;
  const wchar_t*            m_pConnectionMode;


  u32                       m_SelectIndex;
  u32                       m_SelectIndexMax;

  u32                       m_UpdateSeq;
  u32                       m_EndSeq;

  u32                       m_ReceivingProgress;
  bool                      m_bCorruptCRC;
  bool                      m_bDoUDSSigning;

  const wchar_t*            m_pInformation;
  const wchar_t*            m_pSendDataDescription;

  class DirEntryInfo;
  class FileReader;
  class HIOMounter;
  DirEntryInfo*             m_pDirInfo;
  FileReader*               m_pFileReader;
  HIOMounter*               m_pHIOMounter;

  Savedata::MysteryGiftSave::MYSTERY_GIFT_RECV_DATA  m_GiftReceiveData[21];
};


#else   /*  #if defined(GF_PLATFORM_CTR)    */


class TestProc_Delivery : public TestProc
{
  GFL_FORBID_COPY_AND_ASSIGN(TestProc_Delivery);
public:
  TestProc_Delivery(void){}
  /** dtor  */
  virtual ~TestProc_Delivery(){}
};


#endif  /*  #if defined(GF_PLATFORM_CTR)    */





} /*  namespace MutoTest  */
} /*  namespace Test  */
#endif  /*  #if PM_DEBUG  */
#endif  /*  #if !defined(TESTPROC_DELIVERY_H_INCLUDED)  */
