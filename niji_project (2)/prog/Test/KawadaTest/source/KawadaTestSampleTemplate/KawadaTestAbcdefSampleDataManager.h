#if !defined(NIJI_PROJECT_TEST_KAWADATESTABCDEFSAMPLEDATAMANAGER_H_INCLUDED)
#define NIJI_PROJECT_TEST_KAWADATESTABCDEFSAMPLEDATAMANAGER_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    KawadaTestAbcdefSampleDataManager.h
 @author  kawada_koji@gamefreak.co.jp
 @date    0000.00.00
 @brief   KawadaTest
 */
//==============================================================================


#if PM_DEBUG


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)
  class AsyncFileManager;
GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


//==============================================================================
/**
 @class     KawadaTestAbcdefSampleDataManager
 @brief     KawadaTest
 */
//==============================================================================
class KawadaTestAbcdefSampleDataManager
{
  GFL_FORBID_COPY_AND_ASSIGN(KawadaTestAbcdefSampleDataManager);

public:
  //! @brief  データID
  enum DataId
  {
    //DATA_ID_GHIJKL  = 0,
    
    DATA_ID_NUM,                             //!< データ数(ビットフラグを使っているのでbit32で表せる個数におさめなければならない)

    DATA_ID_END = DATA_ID_NUM                //!< データのロードが全て完了していることを示すデータID
  };

public:
  //! @param[in]  appHeap  随時使うヒープ。m_pDevHeap, m_pDevHeapAllocator, m_pDevGLHeapAllocatorしか使わない。
  KawadaTestAbcdefSampleDataManager(app::util::Heap* appHeap, gfl2::fs::AsyncFileManager* romAsyncFileManager);
  virtual ~KawadaTestAbcdefSampleDataManager();

  //! @return  破棄できるときはtrueを返す。数フレームUpdateを呼び続ければそのうちtrueが返ってくるようになる。
  bool CanDestroy(void) const;

  //! @note  毎フレーム呼ぶこと
  void Update(void);

  //! @brief  開始/再開
  void Start(void);
  //! @brief  停止
  //! @note  すぐに停止するわけではない。キリのいいところまでは処理を続ける。
  void Stop(void);

  //! @brief  データバッファを取得する
  //! @return  読み込みが完了していないときはNULLを返す。
  //! @note  データバッファの破棄はこのクラスで行うので、呼び出し元では破棄してはならない。
  void* GetDataBuf(DataId dataId) const;

private:
  //! @brief  状態
  enum State
  {
    STATE_STOP,                // 停止中
    STATE_WORK,                // 実行中
    STATE_WORK_WITH_STOP_REQ,  // 停止リクエストが発行されている実行中
    STATE_END,                 // SEQ_ENDになっている
  };
  //! @brief  シーケンス
  enum Seq
  {
    SEQ_START_OPEN,
    SEQ_WAIT_OPEN,
    SEQ_START_LOAD,
    SEQ_WAIT_LOAD,
    SEQ_START_CLOSE,
    SEQ_WAIT_CLOSE,
    SEQ_END,          // STATE_ENDになっている
  };
 
private:
  void changeSeq(Seq nextSeq);
  void update_StartOpen(void);
  void update_WaitOpen(void);
  void update_StartLoad(void);
  void update_WaitLoad(void);
  void update_StartClose(void);
  void update_WaitClose(void);
  void update_End(void);

private:
  app::util::Heap*             m_appHeap;
  gfl2::fs::AsyncFileManager*  m_romAsyncFileManager;
  State                        m_state;
  Seq                          m_seq;               // 今行うべきシーケンスが入っている
  u32                          m_step;
  DataId                       m_dataIdOfSeqLoad;   // シーケンスがSEQ_WAIT_LOADのときロード中のデータIDが入っている。シーケンスがそれ以外のときこれから読み込むべきデータIDが入っている。
  bit32                        m_dataLoadBitFlag;   // (1 << dataId)が1になっているデータはロードが完了している。
  void*                        m_dataBuf[DATA_ID_NUM];
};


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG


#endif  // NIJI_PROJECT_TEST_KAWADATESTABCDEFSAMPLEDATAMANAGER_H_INCLUDED

