#if !defined(NETAPPHIOUTIL_H_INCLUDED)
#define NETAPPHIOUTIL_H_INCLUDED
#pragma once

#if defined(GF_PLATFORM_CTR)
#if defined(NN_SWITCH_ENABLE_HOST_IO)
#if PM_DEBUG
//==============================================================================
/**
 * @file   NetAppHIOUtil.h
 * @date   2016/10/04 Tue. 15:28:07
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================



// =============================================================================
/**/
//==============================================================================
#include  <nn/hio.h>

#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>
#include  <heap/include/gfl2_Heap.h>


#define   NETAPP_HIOUTIL_AVAILABLE  (1)


namespace NetAppLib  {
namespace Debug {
// =============================================================================
/**/
//==============================================================================
class HIOUtil
{
  GFL_FORBID_COPY_AND_ASSIGN(HIOUtil);
public:

  class IEventHandler
  {
  public:
    enum  EventID
    {
//      EVENT_OnConnect,
//      EVENT_FoundReadableData,
      EVENT_ReadEnd,
    };

    virtual void  OnEvent(HIOUtil* pSender, const EventID eventID) = 0;


    virtual ~IEventHandler(){}
  };
  void  SetHandler(IEventHandler* pHandler) {m_pHandler = pHandler;}


  struct DataInfo
  {
    u32     word;          /*  "head"  */
    u32     bytes;
    wchar_t description[60];
    int     userdata[32];
    
    DataInfo(void)
    {
      Reset();
    }
    void  Reset(void)
    {
      word            = constWord;   
      bytes           = 0;
      description[0]  = '\0';
      memset(userdata, 0, sizeof(userdata));
    }
    static const u32  constWord = 0x64616568u; /*  "head"  */
  };


  HIOUtil(void);
  virtual ~HIOUtil();

  void  Initialize(gfl2::heap::HeapBase* pHeapBase, const s32 ch);
  void  Finalize(void);

  int   Update(void);
  int   GetState(void) const  {return m_State;}
  
  s32   GetCh(void)const  {return m_Ch;}

  //------------------------------------------------------------------
  /**
    *  @brief    受信データの受け取り
    *            OnEvent(EVENT_ReadEnd)で受け取る
    *  @note     本関数で受信データを受け取ることで、次のデータを受信できるようになる
    *  @note     データは受け取り側でGflHeapFreeMemory()する
    *  @note     受け取っていないデータについてはdtorでFreeされる
    *  @note     データは 128[B] aligned
    */
  //------------------------------------------------------------------
  void* GetData(u32* pDataBytes=NULL);

  //------------------------------------------------------------------
  /**
    *  @brief    受信データ情報の取得
    *  @note     有効期間はOnEvent(EVENT_ReadEnd)後、GetData()の後のUpdateまで
    */
  //------------------------------------------------------------------
  const DataInfo& GetDataInfo(void) const {return m_DataInfo;}

  static f32  Fx20_12ToF32(const s32 fx20_12Val) {return (static_cast<f32>(fx20_12Val) / static_cast<f32>(0x01 << 12));}

protected:
  void  Open(void);
  void  Close(void);
  void  Event(const IEventHandler::EventID eventID);

  void  CreateBuffer(const u32 bytes);
  void  ReleaseBuffer(void);


  gfl2::heap::HeapBase*   m_pHeapBase;
  int                     m_State;
  IEventHandler*          m_pHandler;

  void*                   m_pWorkMemory;
  nn::hio::SerialChannel  m_HIOSCh;
  s32                     m_Ch;

  /*  data  */
  u8*                     m_pDataBuffer;
  u32                     m_DataBufferBytes;
  u32                     m_DataBytes;
  u32                     m_ReadableBytes;
  DataInfo                m_DataInfo;
};



} /*  namespace Debug */
} /*  namespace NetApp  */
#endif  /*  #if PM_DEBUG                  */
#endif  /*  #if defined(NN_SWITCH_ENABLE_HOST_IO)  */
#endif  /*  #if defined(GF_PLATFORM_CTR)    */

#if !defined(NETAPP_HIOUTIL_AVAILABLE)
  #define   NETAPP_HIOUTIL_AVAILABLE  (0)
#endif
#endif  /*  #if !defined(NETAPPHIOUTIL_H_INCLUDED)  */
