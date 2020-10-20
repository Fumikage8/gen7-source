//======================================================================
/**
 * @file DendouDemoFileLoader.h
 * @date 2016/03/11 15:44:34
 * @author nagihashi_toru
 * @brief ファイル読み込み
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __DENDOU_DEMO_FILE_LOADER_H_INCLUDED__
#define __DENDOU_DEMO_FILE_LOADER_H_INCLUDED__
#pragma once
#include <heap/include/gfl2_Heap.h>
namespace gfl2{ namespace fs { class AsyncFileManager; class BinLinkerAccessor; } }

namespace app {
  namespace dendou_demo {

  /**
   * @class FileLoader
   * @brief ファイル読み込みクラス
   */
  class FileLoader
  {
      GFL_FORBID_COPY_AND_ASSIGN(FileLoader);
  public:
    /**
     * @fn
     * コンストラクタ
     * @brief コンストラクタ
     * @param[in] heap クラス確保用ヒープ
     * @param[in] fileManager ファイル読み込みマネージャ
     */
    FileLoader( gfl2::heap::HeapBase * heap, gfl2::fs::AsyncFileManager * fileManager );
    /**
     * @fn
     * デストラクタ
     * @brief  デストラクタ
     */
    virtual ~FileLoader(void);
    /**
     * @fn
     * 非同期読み込み開始
     * @brief  非同期読み込み開始
     * @param[in] playerSex プレイヤーの性別
     * @param[in] heapForBuff ファイルバッファ確保用のヒープ
     * @param[in] heapForReq  読み込みリクエストで一時的に使うヒープ
     * @param[in] heapForUncompresse 解凍用に一時的に使うヒープ
     */
    void StartLoadAsync( u8 playerSex, gfl2::heap::HeapBase * heapForBuff, gfl2::heap::HeapBase * heapForReq, gfl2::heap::HeapBase * heapForUncompresse );
    /**
     * @fn
     * 非同期読み込み処理
     * @brief  非同期読み込み処理
     */
    void Update(void);
    /**
     * @fn
     * 非同期待ち
     * @brief  非同期読待ち
     * @return true で非同期読み込み完了、falseでそれ以外
     */
    bool IsComplete(void) const;
    /**
     * @fn
     * ファイルバッファの取得
     * @brief  ファイルバッファの取得
     * @return ファイルバッファ
     */
    void * GetBGModelFileBuffer(void) const;
    void * GetBGMotionFileBuffer(void) const;
    void * GetCameraMotionFileBuffer(void) const;
    void * GetDrawEnvFileBuffer(void) const;
    void * GetLayoutFileBuffer(void) const;
    void * GetParticleFileBuffer(void) const;
    void * GetSpotModelFileBuffer(int index ) const;

  private:
    gfl2::fs::AsyncFileManager * mFileManager;  //! ファイル読み込みマネージャ
    gfl2::fs::BinLinkerAccessor * mBinLinker;   //! パック解除マネージャ
    gfl2::heap::HeapBase * mHeapForBuff;        //! ファイルバッファ確保用ヒープ
    gfl2::heap::HeapBase * mHeapForReq;         //! 読み込みリクエストで一時的に使うヒープ
    gfl2::heap::HeapBase * mHeapForUncompresse; //! 解凍用に一時的に使うヒープ 
    void *mFileBuffer;                          //! ファイルバッファ
    u32 mSeq;                                   //! 読み込みシーケンス
    u8 mPlayerSex;                              //! プレイヤーの性別
  };


  } //dendou_demo
} //app

#endif // __DENDOU_DEMO_FILE_LOADER_H_INCLUDED__