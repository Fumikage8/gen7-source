#if !defined(GFL2_CAPTUREBUFFER_H_INCLUDED)
#define GFL2_CAPTUREBUFFER_H_INCLUDED
#pragma once
#if defined(GF_PLATFORM_CTR)
//==============================================================================
/**
 * @file   gfl2_CaptureBuffer.h
 * @date   2013/03/12 Tue.
 * @date   2015/07/30 Thu. 11:43:41 過去プロジェクトより移植
 * @author muto_yu
 * @brief  DeviceCamera, Y2Rで使用するバッファの実装
 * @note   CTR専用、それ以外ではシンボルが未定義となる
 * @code
 *  //  使用例:
 *  //  DeviceCamera              CaptureBuffer                               Y2RBuffer               
 *  //  (Capture)     - write ->  [    YUV    ]                +- write ->  [  RGB(etc.)  ]           
 *  //                            [    YUV    ]                |            [  RGB(etc.)  ] -+
 *  //                            [    YUV    ] - read -> Y2R -+                             |
 *  //                                                     ↑                                |
 *  //                                                  request                              |
 *  //                                                     ↑                                |
 *  //                                                MainThread  <---------- read ----------+
 * @endcode
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
#include  <debug/include/gfl2_Assert.h>
#include  <heap/include/gfl2_Heap.h>


namespace  gfl2  {
namespace  devicecamera  {
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @brief   deviceCamera撮影用バッファ（triple）
  *  @detail  DeviceCameraからのキャプチャデータを格納するバッファ
  *           DeviceCameraは通常非同期に連続して撮影し、キャプチャイメージをバッファに格納、
  *           メインスレッドより高いpriorityでハードウェアからの撮影完了通知を受け取って処理する
  *           撮影周期がキャプチャイメージのコンバート処理に依存しないよう、トリプルバッファで取りまわす
  *  @note    Write側はDeviceCamera、Read側は通常メインスレッドからの要求でSwap及びアクセスすることになるので、
  *           各SwapのみThreadSafeとしている
  *  @note    Read側Swapの際、前回のSwapReadより後にSwapWriteが済んで（書き込み側の更新が行われて）いないと
  *           フレームが巻き戻ることになる
  *           通常撮影と画像処理については使用側で同期を取ることになるので、本クラスでは冗長を避けるため
  *           サポートしない
  *  @date    2015/07/30 Thu. 11:46:48
  */
//==============================================================================
class CaptureBuffer {
  GFL_FORBID_COPY_AND_ASSIGN(CaptureBuffer);
public:
  CaptureBuffer(void);
  virtual ~CaptureBuffer();

  //------------------------------------------------------------------
  /**
    *  @brief   使用準備（バッファの確保など）
    *  @param   [in]  pDevHeap  deviceHeap
    *  @param   [in]  size      確保するバッファサイズ[B]
    *  @param   [in]  align     アライメント[B]、通常は64で良い
    */
  //------------------------------------------------------------------
  void              Setup(gfl2::heap::HeapBase* pDevHeap, const u32 size, const u32 align = 64);

  //------------------------------------------------------------------
  /**
    *  @brief   開放
    */
  //------------------------------------------------------------------
  void              Cleanup(void);

  //------------------------------------------------------------------
  /**
    *  @brief   書き込みバッファのSwap
    */
  //------------------------------------------------------------------
  u8*               GetWriteBuffer(void);

  //------------------------------------------------------------------
  /**
    *  @brief   読み込みバッファのSwap
    *  @note Writeへの最初のデータ書き込みが完了し、WriteのSwapを実行しなければ取得できない
    */
  //------------------------------------------------------------------
  const u8*         GetReadBuffer(void) const;

  //------------------------------------------------------------------
  /**
    *  @brief   書き込みバッファのSwap
    *  @note    ThreadSafe
    *  @note    通常はCaptureThreadからコールされる
    */
  //------------------------------------------------------------------
  u8*               SwapWriteBuffer(void);

  //------------------------------------------------------------------
  /**
    *  @brief   読み込みバッファのSwap
    *  @note    ThreadSafe
    *  @note    通常はメインスレッドからコールされる
    */
  //------------------------------------------------------------------
  const u8*         SwapReadBuffer(void);


  //------------------------------------------------------------------
  /**
    *  @brief   バッファサイズの取得
    */
  //------------------------------------------------------------------
  inline u32        GetSize(void) const         {return mBufferBytes;}

protected:
private:
  nn::os::CriticalSection     mCS_Swap;             /**<  swap排他のためのCriticalSection     */
  u8*                         mpBufferArray[3];     /**<  トリプルバッファ                    */
  s8                          mWriteBufferIndex;    /**<  現在参照しているWriteBufferのIndex  */
  s8                          mReadBufferIndex;     /**<  現在参照しているReadBufferのIndex   */
  u32                         mBufferBytes;         /**<  バッファサイズ                      */

  void        Invalidate(void);   /**<  各バッファを無効化する  */
};



//==============================================================================
/**
  *  @brief   Y2R(YUV to RGB)用バッファ（double）
  *  @detail  Y2Rからの変換データを格納するバッファ
  *  @note    通常Y2Rはメインスレッドからの要求によってコンバートを開始する
  *           コンバート要求時に前回のコンバートが完了しているのかを確認することになるため
  *           Swap要求自体が単一のスレッドから行われることになるり、本来はスレッド排他は不要
  *  @date    2015/07/30 Thu. 12:16:20
  */
//==============================================================================
class Y2RBuffer {
  GFL_FORBID_COPY_AND_ASSIGN(Y2RBuffer);
public:
  Y2RBuffer(void);
  virtual ~Y2RBuffer();

  //------------------------------------------------------------------
  /**
    *  @brief   使用準備（バッファの確保など）
    *  @param   [in]  pDevHeap  deviceHeap
    *  @param   [in]  size      確保するバッファサイズ[B]
    *  @param   [in]  align     アライメント[B]、通常は64で良い
    */
  //------------------------------------------------------------------
  void              Setup(gfl2::heap::HeapBase* pDevHeap, const u32 size, const u32 align = 64);

  //------------------------------------------------------------------
  /**
    *  @brief   開放
    */
  //------------------------------------------------------------------
  void              Cleanup(void);


  //------------------------------------------------------------------
  /**
    *  @brief   書き込みバッファのSwap
    */
  //------------------------------------------------------------------
  u8*               GetWriteBuffer(void);

  //------------------------------------------------------------------
  /**
    *  @brief   読み込みバッファのSwap
    */
  //------------------------------------------------------------------
  const u8*         GetReadBuffer(void) const;


  //------------------------------------------------------------------
  /**
    *  @brief   バッファのSwap
    *  @note    ThreadSafe
    *  @note    通常はメインスレッドからコールされる
    */
  //------------------------------------------------------------------
  const u8*         SwapBuffer(void);


  //------------------------------------------------------------------
  /**
    *  @brief   バッファサイズの取得
    */
  //------------------------------------------------------------------
  inline u32        GetSize(void) const         {return mBufferBytes;}

protected:
private:
  nn::os::CriticalSection     mCS_Swap;             /**<  swap排他のためのCriticalSection     */
  u8*                         mpBufferArray[2];     /**<  ダブルバッファ                      */
  s8                          mWriteBufferIndex;    /**<  現在参照しているWriteBufferのIndex  */
  s8                          mReadBufferIndex;     /**<  現在参照しているReadBufferのIndex   */
  u32                         mBufferBytes;         /**<  バッファサイズ                      */

  void        Invalidate(void);   /**<  各バッファを無効化する  */
};


//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/07/30 Thu. 12:50:25
  */
//==============================================================================
class ThreadStack : public nn::os::StackMemory {
public:
  ThreadStack(gfl2::heap::HeapBase* pHeap, const u32 size) {
    mpBuffer  = GflHeapAllocMemoryAlign4096(pHeap, size);
    this->Initialize(mpBuffer, size);
  }

  virtual ~ThreadStack()  {
    this->Finalize();
    GflHeapFreeMemory(mpBuffer);
    mpBuffer  = NULL;
  }

protected:
private:
  void*       mpBuffer;
  ThreadStack(void) {}
};


/*  ********************************************************************
    prototype
  *******************************************************************/


}  /*  namespace  devicecamera  */
}  /*  namespace  gfl2  */
#endif  /*  #if defined(GF_PLATFORM_CTR)                  */
#endif  /*  #if !defined(GFL2_CAPTUREBUFFER_H_INCLUDED)   */


