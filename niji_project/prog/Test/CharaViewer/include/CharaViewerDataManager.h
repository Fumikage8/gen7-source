//======================================================================
/**
 * @file CharaViewerDataManager.h
 * @date 2015/07/21 15:21:43
 * @author araki_syo
 * @brief CharaViewerのデータマネージャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#if !defined CHARA_VIEWER_DATA_MANAGER_HEADER_H_INCLUDED
#define CHARA_VIEWER_DATA_MANAGER_HEADER_H_INCLUDED
#pragma once

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

GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(chara_viewer)

/**
 * @class     CharaViewerDataManager
 * @brief     CharaViewerのデータマネージャ
 */
class CharaViewerDataManager
{
  GFL_FORBID_COPY_AND_ASSIGN(CharaViewerDataManager);

public:
  //! @brief  データID
  enum DataId
  {
    DATA_ID_CHARA_VIEWER_RES2D          = 0,
    
    DATA_ID_NUM,                         //!< データ数(ビットフラグを使っているのでbit32で表せる個数におさめなければならない)

    DATA_ID_END = DATA_ID_NUM            //!< データのロードが全て完了していることを示すデータID
  };

public:
  //! @param[in]  appHeap  随時使うヒープ。m_pDevHeap, m_pDevHeapAllocator, m_pDevGLHeapAllocatorしか使わない。
  CharaViewerDataManager(app::util::Heap* appHeap, gfl2::fs::AsyncFileManager* romAsyncFileManager);
  virtual ~CharaViewerDataManager();

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


GFL_NAMESPACE_END(chara_viewer)
GFL_NAMESPACE_END(test)

#endif // CHARA_VIEWER_DATA_MANAGER_HEADER_H_INCLUDED

#endif // PM_DEBUG
