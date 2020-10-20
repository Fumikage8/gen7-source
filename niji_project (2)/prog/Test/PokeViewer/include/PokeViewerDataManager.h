//======================================================================
/**
 * @file PokeViewerDataManager.h
 * @date 2015/11/30 19:29:25
 * @author araki_syo
 * @brief PokeViewerのデータマネージャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#if !defined POKEVIEWER_DATAMANAGER_H_INCLUDED
#define POKEVIEWER_DATAMANAGER_H_INCLUDED
#pragma once

#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <AppLib/include/Util/app_util_heap.h>

GFL_NAMESPACE_BEGIN(test)
  GFL_NAMESPACE_BEGIN(pokeviewer)

/**
 * @class PokeViewerDataManager
 * @brief ポケビューアのデータマネージャ
 */
class PokeViewerDataManager
{
  GFL_FORBID_COPY_AND_ASSIGN(PokeViewerDataManager);
  
public:
  enum DataID
  {
    DATA_ID_POKEVIEWER_RES2D = 0,
    DATA_ID_NUM,
    DATA_ID_END = DATA_ID_NUM
  };

public:
  /**
   * @fn PokeViewerDataManager
   * @brief ctor
   */
  PokeViewerDataManager(app::util::Heap* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileManager);

  /**
   * @fn ~PokeViewerDataManager
   * dtor
   */
  virtual ~PokeViewerDataManager();

  /**
   * @fn IsDisposable
   * @brief オブジェクトの解放が可能なときtrueを返す
   */
  b32 IsDisposable() const;

  /**
   * @fn Update
   * @brief 毎フレーム呼び出されるUpdate関数
   */
  void Update();

  /**
   * @fn Start
   * @brief 開始/再開処理
   */
  void Start();

  /**
   * @fn Stop
   * @brief 停止処理
   */
  void Stop();

  /**
   * @fn GetDataBuf
   * @brief データバッファを取得する
   * @detail 読み込みが完了していないときはNULLを返す
   */
  void* GetDataBuf(DataID dataID) const;

private:

  enum State
  {
    STATE_STOP,               // 停止中
    STATE_WORK,               // 実行中
    STATE_WORK_WITH_STOP_REQ, // 停止リクエストを受けたがキリのいいところまで実行中
    STATE_END                 // 終了
  };

  enum Seq
  {
    SEQ_START_OPEN,
    SEQ_WAIT_OPEN,
    SEQ_START_LOAD,
    SEQ_WAIT_LOAD,
    SEQ_START_CLOSE,
    SEQ_WAIT_CLOSE,
    SEQ_END
  };

private:
  void ChangeSeq(Seq seq);
  void Update_StartOpen();
  void Update_WaitOpen();
  void Update_StartLoad();
  void Update_WaitLoad();
  void Update_StartClose();
  void Update_WaitClose();
  void Update_End();

private:
  app::util::Heap* m_pHeap;
  gfl2::fs::AsyncFileManager* m_pAsyncFileManager;
  State m_State;
  Seq m_Seq;
  u32 m_StepCounter;
  DataID m_DataID;
  bit32 m_DataLoadBitFlag;
  void* m_pDataBuf[DATA_ID_NUM];
};

  GFL_NAMESPACE_END(test);
GFL_NAMESPACE_END(pokeviewer);
#endif // POKEVIEWER_DATAMANAGER_H_INCLUDED

#endif // PM_DEBUG
