#ifndef __GFL_CTREXPHEAP_H__
#define __GFL_CTREXPHEAP_H__
//======================================================================
/**
 * @file	gfl2_CtrExpHeap.h
 * @brief	3DSの拡張ヒープ
 * @author	ariizumi
 * @data	10/10/18
 */
//======================================================================
#pragma once
#ifdef GF_PLATFORM_CTR

#include <nn.h>
#if GFL_HIO_ENABLE
#include <nn/hio.h>
#endif // GFL_HIO_ENABLE
#include <heap/include/gfl2_CtrHeapBase.h>

#if GFL_HIO_ENABLE
//#define GFL_DEBUG_HIO_LOG_IN_MEMORY
#endif //GFL_HIO_ENABLE

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(heap)

//#pragma mark [> HeapBase
//拡張ヒープ
typedef nn::fnd::ThreadSafeExpHeap CtrExpHeapClass;

class CTRExpHeap:public CtrHeapBase
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(CTRExpHeap);
public:
  CTRExpHeap(void)
#if GFL_HIO_ENABLE
    : m_dumpFlag(false)
#ifdef GFL_DEBUG_HIO_LOG_IN_MEMORY
    , m_dumpLogList(NULL)
    , m_dumpLogLast(NULL)
#endif
#endif // GFL_HIO_ENABLE
  {
#if GFL_HEAP_DEBUG
    m_debugNowReverseAllocFlag = false;
#ifdef GFL_DEBUG_HIO_LOG_IN_MEMORY
    m_debugNowHioDumpInMemory = false;
#endif
#endif
  };
  virtual ~CTRExpHeap(){};
  virtual void Dump(void);
  virtual void DumpId(u8 id);
  virtual void SystemDump(void);
  virtual bool Check(void);
  
  void DumpBlock( void *buf );

  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープのサイズを取得します
   *
   *  @return  int サイズ
   */
  //-----------------------------------------------------------------------------
  virtual int GetTotalSize(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープの合計空きサイズを取得します
   *
   *  @return  int サイズ
   */
  //-----------------------------------------------------------------------------
  virtual int GetTotalFreeSize(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープで確保できる最大のサイズを取得します
   *
   *  @return  int サイズ
   */
  //-----------------------------------------------------------------------------
  virtual int GetTotalAllocatableSize(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープの先頭アドレスを返します(管理領域を含まない
   *
   *  @return  void* 
   */
  //-----------------------------------------------------------------------------
  virtual void* GetStartAddress(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief  確保したメモリのリサイズ
   *
   *  @param  void* buf リサイズするメモリ  
   *  @param  int       変更後のサイズ
   */
  //-----------------------------------------------------------------------------
  virtual void ResizeMemory( void* buf , const int newSize);

  //----------------------------------------------------------------------------
  /**
   *  @brief  確保された領域のサイズを取得(HeapManagerにもあります)
   *          継承していなければ非対応
   *
   *  @param  const void mem 調べるメモリ
   *  @return int メモリサイズ(-1は失敗)
   */
  //-----------------------------------------------------------------------------
  virtual int GetMemorySize( const void *mem );

  //----------------------------------------------------------------------------
  /**
   *  @brief  渡されたメモリが正当かどうかをチェックします。
   *
   *  @param  const void mem 調べるメモリ
   *  @return bool チェック失敗でfalse
   */
  //-----------------------------------------------------------------------------
  virtual bool CheckMemoryBlock( const void *mem );

  //----------------------------------------------------------------------------
  /**
   *  @brief  すべてのメモリを解放します。
   *
   *  @return bool 失敗したらfalse
   */
  //-----------------------------------------------------------------------------
  virtual bool FreeAll(void);


#if GFL_HEAP_DEBUG
  // @brief メモリのダングリングをあぶりだすために使用する。 確保先を反転させる挙動にするかどうか。
  static void SetDebugReverseAllocFlag( b32 flag ){ m_debugReverseAllocFlag = flag; }
  static b32 GetDebugReverseAllocFlag() { return m_debugReverseAllocFlag; }

#ifdef GFL_DEBUG_HIO_LOG_IN_MEMORY
  static void SetDebugHioDumpInMemory( b8 flag, HEAPID heapId ){ m_debugHioDumpInMemory = flag; m_dumpLogHeapId = heapId; }
  static b8 GetDebugHioDumpInMemory() { return m_debugHioDumpInMemory; }
#endif //GFL_DEBUG_HIO_LOG_IN_MEMORY
#endif

private:
  virtual void CreateHeap( void *buf , const int size , void *param );
  virtual void DeleteHeap(void);
  virtual void* AllocAlign( const int size , const int align );
#if GFL_HIO_ENABLE
  virtual void AllocDump( void* buf );
#endif
  virtual void Free( void* buf );

  static void BlockVisitorCallback( void* pBlock , const nn::fnd::ExpHeapBase *expHeapBase , uptr param );
  static void BlockVisitorCallbackID( void* pBlock , const nn::fnd::ExpHeapBase *expHeapBase , uptr param );
  static void BlockVisitorCallback_Free( void* pBlock , const nn::fnd::ExpHeapBase *expHeapBase , uptr param );
#if GFL_HIO_ENABLE
  static void CTRExpHeap::BlockVisitorHioCallback( void* pBlock , const nn::fnd::ExpHeapBase *expHeapBase , uptr param );
  static void CTRExpHeap::BlockVisitorHioCallbackID( void* pBlock , const nn::fnd::ExpHeapBase *expHeapBase , uptr param );
#endif // GFL_HIO_ENABLE

  // ダンプ処理ローカル関数
  void dump( bool chk_id = false, u8 id = 0 );

  CtrExpHeapClass *m_heapHandle;
  u8 handleBuff[sizeof(CtrExpHeapClass)];
#if GFL_HIO_ENABLE
  nn::hio::CTR::HostFile m_hostFile;
  bool m_dumpFlag;
  void hfprintf(nn::hio::CTR::HostFile *file, const char* pszFormat, ...);
#ifdef GFL_DEBUG_HIO_LOG_IN_MEMORY
  struct DumpLog{
    int length;
    DumpLog * next;
    char log[0];
  };
  DumpLog *m_dumpLogList;
  DumpLog *m_dumpLogLast;
  void StoreLog( char * string, int length );
  void WriteLog( nn::hio::CTR::HostFile *file );
#endif //GFL_DEBUG_HIO_LOG_IN_MEMORY
#endif // GFL_HIO_ENABLE

#if GFL_HIO_ENABLE
  virtual void StartHioDump();
  virtual void EndHioDump();
  virtual void AbortHioDump();
  //----------------------------------------------------------------------------
  /**
   *  @brief  HIOを使用したヒープ状態のDumpを開始します
   *
   *  @param  bool chk_id
   *  @param  u8   id
   */
  //-----------------------------------------------------------------------------
  virtual void HioDump( bool chk_id = false, u8 id = 0);
#endif // GFL_HIO_ENABLE

#if GFL_HEAP_DEBUG

  static b8 m_debugReverseAllocFlag;
  b8 m_debugNowReverseAllocFlag;


#ifdef GFL_DEBUG_HIO_LOG_IN_MEMORY
  static b8 m_debugHioDumpInMemory;
  static HEAPID m_dumpLogHeapId;
  b8 m_debugNowHioDumpInMemory;
  HEAPID m_nowDumpLogHeapId;
#endif //GFL_DEBUG_HIO_LOG_IN_MEMORY
#endif

};

GFL_NAMESPACE_END(heap)
GFL_NAMESPACE_END(gfl2)

#endif //GF_PLATFORM_CTR

#endif // __GFL_CTREXPHEAP_H__
