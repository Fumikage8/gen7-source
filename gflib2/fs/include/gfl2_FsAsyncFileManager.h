#if !defined(GFL2_FSASYNCFILEMANAGER_H_INCLUDED)
#define GFL2_FSASYNCFILEMANAGER_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_FsAsyncFileManager.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   非同期ファイル読み込み書き込みマネージャ
 */
//==============================================================================


// リクエストの実行優先度が同じ場合は、先に発行されたリクエストから実行されます。
// 同期版の優先度は非同期版より高くなるようにしているので、同期版から実行されます。


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <thread/include/gfl2_ScopedLock.h>


// fsのインクルード
#include <fs/include/gfl2_FsToolDefine.h>
#include <fs/include/gfl2_FsDefine.h>
#include <fs/include/gfl2_FsResult.h>
#include <fs/include/internal/gfl2_FsFile.h>
#include <fs/include/gfl2_FsArcFile.h>
#include <fs/include/internal/gfl2_FsElemBase.h>
#include <fs/include/gfl2_FsStr.h>
#include <fs/source/internal/gfl2_FsCalc.h>



// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)
  class DeviceBase;
  class List;
  class FileThread;
  class OtherThread;
GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(thread)
  class Event;
GFL_NAMESPACE_END(thread)
GFL_NAMESPACE_END(gfl2)




// マクロ
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
  #define  AddArcFileOpenReq(req)   __AddArcFileOpenReq(req, __FILE__, __LINE__)     // (req03)
  #define  AddArcFileCloseReq(req)  __AddArcFileCloseReq(req, __FILE__, __LINE__)    // (req04)
  #define  SyncArcFileOpen(req)     __SyncArcFileOpen(req, __FILE__, __LINE__)       // (req03)
  #define  SyncArcFileClose(req)    __SyncArcFileClose(req, __FILE__, __LINE__)      // (req04)
#endif




GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


//==============================================================================
/**
 @class     AsyncFileManager
 @brief     非同期ファイル読み込み書き込みマネージャ
 */
//==============================================================================
class AsyncFileManager
{
  GFL_FORBID_COPY_AND_ASSIGN(AsyncFileManager);

  friend class FileThread;
  friend class OtherThread;
  friend class Win32Util;
  friend class CtrHioUtil;

#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  friend class MatchCheckReqElem;
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST

#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  // 前方宣言
  class MatchCheckManager;
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST



  ////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////
  //
  // 公開
  //
  ////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////

public:
  //! @brief  garcファイルの名前テーブルの種類
  class ArcFileNameTableType
  {
  public:
    enum Tag
    {
      MAIN,         //!< garcファイルの名前テーブル
      HIO,          //!< CTRのHIOのgarcファイルの名前テーブル
      ARCSRC,       //!< ArcSrcファイルの名前テーブル
      DIRECT_SDMC,  //!< DirectSdmcにあるgarcファイルの名前テーブル
      NUM
    };
  };


  //! @brief  どこにあるファイルを扱うかの設定
  //! @brief  HIOを使って、ロム内にあるファイルではなくPC上のファイルを扱うか否か
  class HioUseSetting
  {
  public:
    enum Tag
    {
      SAME_AS_MANAGER,  //!< マネージャと同じ設定
      ALWAYS_MAIN,      //!< ロム内のファイルを扱う設定
      ALWAYS_HIO,       //!< HIOを使ってPC上のファイルを扱う設定。Win32版で設定していてもHIOを使わないので問題はない。
    };
  };
  //! @brief  gascを読み込んで元ファイルテーブルをつくるか否かの設定
  class ArcSrcCreateSetting
  {
  public:
    enum Tag
    {
      SAME_AS_MANAGER,  //!< マネージャと同じ設定
      ALWAYS_CREATE,    //!< 読み込んでつくる設定
      NEVER_CREATE,     //!< 読み込まずつくらない設定
    };
  };
  //! @brief  gascを使って元ファイルを読み込むか否かの設定
  class ArcSrcUseSetting
  {
  public:
    enum Tag
    {
      SAME_AS_MANAGER,  //!< マネージャと同じ設定
      ALWAYS_USE,       //!< 使って読み込む設定
      NEVER_USE,        //!< 使わず読み込まない設定
    };
  };


  //! @brief リクエストの実行優先度
  //! @note 値が小さい方が優先度が高い
  static const u8 REQ_PRIO_HIGHEST =  0;  //!< リクエストの実行優先度  最も高い
  static const u8 REQ_PRIO_NORMAL  = 16;  //!< リクエストの実行優先度  普通
  static const u8 REQ_PRIO_LOWEST  = 31;  //!< リクエストの実行優先度  最も低い


  //! @brief  リクエストのリスト要素のタイプ
  class ReqElemType
  {
  public:
    enum Tag
    {
      ELEM_BASE,                                             // =0

      FILE_EASY_READ_REQ,                  // (req01)        // =1
      FILE_EASY_WRITE_REQ,                 // (req02)        // =2
      ARC_FILE_OPEN_REQ,                   // (req03)        // =3
      ARC_FILE_CLOSE_REQ,                  // (req04)        // =4
      ARC_FILE_LOAD_DATA_REQ,              // (req05)        // =5
      ARC_FILE_LOAD_DATA_BUF_REQ,          // (req06)        // =6
      ARC_FILE_LOAD_DATA_PIECE_BUF_REQ,    // (req07)        // =7
      ARC_FILE_SEEK_DATA_PTR_REQ,          // (req08)        // =8
      ARC_FILE_LOAD_DATA_PTR_BUF_REQ,      // (req09)        // =9
    };
  };


  //! @brief  ArcFileロード間隔チェックの警告レベル
  class IntervalCheckLevel
  {
  public:
    enum Tag
    {
      LEVEL_NONE     = 0,  //!< 警告表示なし
      LEVEL_WARNING  = 1,  //!< GFL_RELEASE_PRINTで表示する
      LEVEL_ASSERT   = 2,  //!< ASSERTで止める
      LEVEL_NUM      = 3
    };
  };


private:
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
  //! @brief  リクエスト発行者の情報
  class CallerInfo
  {
  public:
    enum
    {
      CALLER_FILE_NAME_BUF_SIZE = 20  // 終端の'\0'も含めた長さ。callerFileNameのバイト数。
    };
  public:
    CallerInfo(void);
  public:
    //! @brief  発行者の情報を設定する
    //! @param[in] a_callerFileName  発行者のファイル名(__FILE__)。文字数はいくつでもいいが、終端は'\0'になっていること。NULLでもよい。
    //! @param[in] a_callerLineNo    発行者の行数(__LINE__)。
    void SetCaller(const char* a_callerFileName, u32 a_callerLineNo);
  public:
    char callerFileName[CALLER_FILE_NAME_BUF_SIZE];  // 終端は'\0'になるようにしておく。
    u32  callerLineNo;
  };
#endif


public:
  //! @brief  読み込みリクエスト(オープンとクローズも行う)
  class FileEasyReadReq  // (req01)
  {
  public:
    const char*            fileName;           //!< [in]  ファイル名。終端は'\0'になっていること。
    u8                     prio;               //!< [in]  リクエストの実行優先度(値が小さい方が優先度が高い)
    void**                 ppBuf;              //!< [out] heapForBufから確保したメモリが渡されるポインタ。呼び出し元でGflHeapFreeMemory(*ppBuf)で解放して下さい。  // ****キー****
    size_t*                pBufSize;           //!< [out] heapForBufから確保したメモリのサイズ。heapの管理領域のサイズは含まれていない。NULLでもよい。
    size_t*                pFileSize;          //!< [out] ファイルサイズ。(*pBufSize >= *pFileSize)となる。heapForCompressedがNULLでないときは解凍後のサイズ。NULLでもよい。
    gfl2::heap::HeapBase*  heapForBuf;         //!< [in]  ファイルから読み込んだ内容を入れておくバッファを確保するためのヒープ。
    u32                    align;              //!< [in]  heapForBufから確保する際のアラインメント。2のべき乗(1, 2, 4, 8, 16, 32, 64, 128, ...)にして下さい。圧縮ファイルも同じアラインメントで読みます。
    gfl2::heap::HeapBase*  heapForReq;         //!< [in]  リクエスト終了まで一時的に使用するヒープ。ファイルオープンのためにも使用する。
    gfl2::heap::HeapBase*  heapForCompressed;  //!< [in]  解凍する場合に一時的に使用するヒープ。解凍するときはヒープのポインタを渡し、解凍しないときはNULLを渡して下さい。
    HioUseSetting::Tag     hioUseSetting;      //!< [in]  どこにあるファイルを扱うかの設定
    Result*                result;             //!< [out] 結果。NULLでもよい。リクエスト終了まで使われ続けるので一時変数を渡さないように注意して下さい。
  private:
    friend class FileThread;
    friend class OtherThread;
    friend class AsyncFileManager;
    size_t                 m_fileSize;
    gfl2::thread::Event*   m_syncReqFinishEvent;
  public:
    FileEasyReadReq(void)
      : fileName(NULL),
        prio(REQ_PRIO_NORMAL),
        ppBuf(NULL),
        pBufSize(NULL),
        pFileSize(NULL),
        heapForBuf(NULL),
        align(4),
        heapForReq(NULL),
        heapForCompressed(NULL),
        hioUseSetting(HioUseSetting::SAME_AS_MANAGER),
        result(NULL),
        m_fileSize(0),
        m_syncReqFinishEvent(NULL)
    {}
  };
  //! @brief  書き込みリクエスト(オープンとクローズも行う)
  class FileEasyWriteReq  // (req02)
  {
  public:
    const char*            fileName;       //!< [in]  ファイル名。終端は'\0'になっていること。  // ****キー****
    u8                     prio;           //!< [in]  リクエストの実行優先度(値が小さい方が優先度が高い)
    void*                  pBuf;           //!< [in]  ファイルに書き込む内容。
    size_t                 bufSize;        //!< [in]  pBufのサイズ。
    gfl2::heap::HeapBase*  heapForReq;     //!< [in]  リクエスト終了まで一時的に使用するヒープ。ファイルオープンのためにも使用する。
    HioUseSetting::Tag     hioUseSetting;  //!< [in]  どこにあるファイルを扱うかの設定
    Result*                result;         //!< [out] 結果。NULLでもよい。リクエスト終了まで使われ続けるので一時変数を渡さないように注意して下さい。
  private:
    friend class FileThread;
    friend class OtherThread;
    friend class AsyncFileManager;
    gfl2::thread::Event*   m_syncReqFinishEvent;
  public:
    FileEasyWriteReq(void)
      : fileName(NULL),
        prio(REQ_PRIO_NORMAL),
        pBuf(NULL),
        bufSize(0),
        heapForReq(NULL),
        hioUseSetting(HioUseSetting::SAME_AS_MANAGER),
        result(NULL),
        m_syncReqFinishEvent(NULL)
    {}
  };
  //! @brief  garcファイルオープンリクエスト
  class ArcFileOpenReq  // (req03)
  {
  public:
    const char*                   fileName;             //!< [in]  ファイル名。終端は'\0'になっていること。arcIdがARCID_NULLのときのみ使われる。  // ****キー****
    ArcFile::ARCID                arcId;                //!< [in]  ARCID。ARCID_NULLのときはfileNameが使われる。  // ****キー****
    u8                            prio;                 //!< [in]  リクエストの実行優先度(値が小さい方が優先度が高い)
    gfl2::heap::HeapBase*         heapForFile;          //!< [in]  ヒープ。ファイルインスタンスの作成と索引テーブルの作成に使用。Closeするまで左記のものが保持される。
    ToolDefine::ArcLangType::Tag  lang;                 //!< [in]  対象言語。
    gfl2::heap::HeapBase*         heapForReq;           //!< [in]  リクエスト終了まで一時的に使用するヒープ。索引テーブルの作成のためにも一時的に使用する。ArcSrcの作成の際にも一時的に使用する。
    HioUseSetting::Tag            hioUseSetting;        //!< [in]  どこにあるファイルを扱うかの設定
    ArcSrcCreateSetting::Tag      arcSrcCreateSetting;  //!< [in]  gascを読み込んで元ファイルテーブルをつくるか否かの設定
    bool                          memorizeCaller;       //!< [in]  リクエスト発行者を記録する場合true。専用のデバッグメモリを使用して記録するので、呼び出し元はメモリは気にしなくていい。
    Result*                       result;               //!< [out] 結果。NULLでもよい。リクエスト終了まで使われ続けるので一時変数を渡さないように注意して下さい。
    // ↓arcSrcCreateFlagがtrueのときのみ使われる
    const char*                   arcSrcFileName;       //!< [in]  ArcSrcのファイル名。終端は'\0'になっていること。「arcIdがARCID_NULLのとき」のみこれは使われる。
                                                        //         「arcIdがARCID_NULLのとき」にこれにNULLを渡すと、ArcSrcはつくられない。
                                                        //         「heapForArcSrcがNULLのとき」かつ「マネージャが専用のデバッグメモリを保持していないとき」はArcSrcがつくられないので、
                                                        //         これも使われない。
    gfl2::heap::HeapBase*         heapForArcSrc;        //!< [in]  ヒープ。ArcSrcのインスタンスの作成とソーステーブルの作成に使用。Closeするまで左記のものが保持される。
                                                        //         NULLのときは「マネージャが保持している専用のデバッグメモリ」を使う。
    // ↑arcSrcCreateFlagがtrueのときのみ使われる
private:
    friend class FileThread;
    friend class OtherThread;
    friend class AsyncFileManager;
    gfl2::thread::Event*          m_syncReqFinishEvent;
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
  private:
    CallerInfo*                   m_callerInfo;
#endif
  public:
    ArcFileOpenReq(void)
      : fileName(NULL),
        arcId(ArcFile::ARCID_NULL),
        prio(REQ_PRIO_NORMAL),
        heapForFile(NULL),
        lang(ToolDefine::ArcLangType::DEFAULT_LANG),
        heapForReq(NULL),
        hioUseSetting(HioUseSetting::SAME_AS_MANAGER),
        arcSrcCreateSetting(ArcSrcCreateSetting::SAME_AS_MANAGER),
        memorizeCaller(true),
        result(NULL),
        arcSrcFileName(NULL),
        heapForArcSrc(NULL),
        m_syncReqFinishEvent(NULL)
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
        , m_callerInfo(NULL)
#endif
    {}
  };
  //! @brief  garcファイルクローズリクエスト
  class ArcFileCloseReq  // (req04)
  {
  public:
    const char*            fileName;        //!< [in]  ファイル名。終端は'\0'になっていること。arcIdがARCID_NULLのときのみ使われる。  // ****キー****
    ArcFile::ARCID         arcId;           //!< [in]  ARCID。ARCID_NULLのときはfileNameが使われる。  // ****キー****
    u8                     prio;            //!< [in]  リクエストの実行優先度(値が小さい方が優先度が高い)
    gfl2::heap::HeapBase*  heapForReq;      //!< [in]  リクエスト終了まで一時的に使用するヒープ。同期版ではNULLでよい。
    bool                   memorizeCaller;  //!< [in]  リクエスト発行者を記録する場合true。専用のデバッグメモリを使用して記録するので、呼び出し元はメモリは気にしなくていい。
    Result*                result;          //!< [out] 結果。NULLでもよい。リクエスト終了まで使われ続けるので一時変数を渡さないように注意して下さい。
private:
    friend class FileThread;
    friend class OtherThread;
    friend class AsyncFileManager;
    gfl2::thread::Event*   m_syncReqFinishEvent;
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
  private:
    CallerInfo*            m_callerInfo;
#endif
  public:
    ArcFileCloseReq(void)
      : fileName(NULL),
        arcId(ArcFile::ARCID_NULL),
        prio(REQ_PRIO_NORMAL),
        heapForReq(NULL),
        memorizeCaller(true),
        result(NULL),
        m_syncReqFinishEvent(NULL)
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
        , m_callerInfo(NULL)
#endif
    {}
  };
  //! @brief  garcファイル内のデータ1件の読み込みリクエスト
  class ArcFileLoadDataReq  // (req05)
  {
  public:
    const char*                   fileName;           //!< [in]  ファイル名。終端は'\0'になっていること。arcIdがARCID_NULLのときのみ使われる。
    ArcFile::ARCID                arcId;              //!< [in]  ARCID。ARCID_NULLのときはfileNameが使われる。
    ArcFile::ARCDATID             datId;              //!< [in]  アーカイブファイル内データID
    ToolDefine::ArcLangType::Tag  lang;               //!< [in]  対象言語。
    u8                            prio;               //!< [in]  リクエストの実行優先度(値が小さい方が優先度が高い)
    void**                        ppBuf;              //!< [out] heapForBufから確保したメモリが渡されるポインタ。呼び出し元でGflHeapFreeMemory(*ppBuf)で解放して下さい。  // ****キー****
    size_t*                       pBufSize;           //!< [out] heapForBufから確保したメモリのサイズ。heapの管理領域のサイズは含まれていない。NULLでもよい。
    size_t*                       pRealReadSize;      //!< [out] 実際に読み込んだサイズ。heapForCompressedがNULLでないときは解凍後のサイズ。NULLでもよい。
    gfl2::heap::HeapBase*         heapForBuf;         //!< [in]  ファイルから読み込んだ内容を入れておくバッファを確保するためのヒープ。
    u32                           align;              //!< [in]  heapForBufから確保する際のアラインメント。2のべき乗(1, 2, 4, 8, 16, 32, 64, 128, ...)にして下さい。圧縮ファイルも同じアラインメントで読みます。
    gfl2::heap::HeapBase*         heapForReq;         //!< [in]  リクエスト終了まで一時的に使用するヒープ。『「同期版」かつ「ArcSrcを使わないとき」』はNULLでよい。ArcSrcを使うときはファイルオープンにも使用する。
    gfl2::heap::HeapBase*         heapForCompressed;  //!< [in]  解凍する場合に一時的に使用するヒープ。解凍するときはヒープのポインタを渡し、解凍しないときはNULLを渡して下さい。
    ArcSrcUseSetting::Tag         arcSrcUseSetting;   //!< [in]  gascを使って元ファイルを読み込むか否かの設定
    Result*                       result;             //!< [out] 結果。NULLでもよい。リクエスト終了まで使われ続けるので一時変数を渡さないように注意して下さい。
  private:
    friend class FileThread;
    friend class OtherThread;
    friend class AsyncFileManager;
    size_t                        m_realReadSize;
    gfl2::thread::Event*          m_syncReqFinishEvent;
  public:
    ArcFileLoadDataReq(void)
      : fileName(NULL),
        arcId(ArcFile::ARCID_NULL),
        datId(ArcFile::ARCDATID_NULL),
        lang(ToolDefine::ArcLangType::DEFAULT_LANG),
        prio(REQ_PRIO_NORMAL),
        ppBuf(NULL),
        pBufSize(NULL),
        pRealReadSize(NULL),
        heapForBuf(NULL),
        align(4),
        heapForReq(NULL),
        heapForCompressed(NULL),
        arcSrcUseSetting(ArcSrcUseSetting::SAME_AS_MANAGER),
        result(NULL),
        m_realReadSize(0),
        m_syncReqFinishEvent(NULL)
    {}
  };
  //! @brief  garcファイル内のデータ1件の読み込みリクエスト(あらかじめ確保してあるメモリに読み込む版)
  class ArcFileLoadDataBufReq  // (req06)
  {
  public:
    const char*                   fileName;           //!< [in]  ファイル名。終端は'\0'になっていること。arcIdがARCID_NULLのときのみ使われる。
    ArcFile::ARCID                arcId;              //!< [in]  ARCID。ARCID_NULLのときはfileNameが使われる。
    ArcFile::ARCDATID             datId;              //!< [in]  アーカイブファイル内データID
    ToolDefine::ArcLangType::Tag  lang;               //!< [in]  対象言語。
    u8                            prio;               //!< [in]  リクエストの実行優先度(値が小さい方が優先度が高い)
    void*                         pBuf;               //!< [out] 呼び出し元であらかじめメモリを確保しておいたバッファ。ここにファイルから読み込んだ内容を入れる。  // ****キー****
    size_t                        bufSize;            //!< [in]  pBufのサイズ。データサイズ(パディングを含む)以上にして下さい。
    size_t*                       pRealReadSize;      //!< [out] 実際に読み込んだサイズ。heapForCompressedがNULLでないときは解凍後のサイズ。NULLでもよい。
    u32                           alignForCompressed; //!< [in]  圧縮ファイルを読み込む際にheapForCompressedから確保する際のアラインメント。2のべき乗(1, 2, 4, 8, 16, 32, 64, 128, ...)にして下さい。解凍しない場合は0でよい。
    gfl2::heap::HeapBase*         heapForReq;         //!< [in]  リクエスト終了まで一時的に使用するヒープ。『「同期版」かつ「ArcSrcを使わないとき」』はNULLでよい。ArcSrcを使うときはファイルオープンにも使用する。
    gfl2::heap::HeapBase*         heapForCompressed;  //!< [in]  解凍する場合に一時的に使用するヒープ。解凍するときはヒープのポインタを渡し、解凍しないときはNULLを渡して下さい。
    ArcSrcUseSetting::Tag         arcSrcUseSetting;   //!< [in]  gascを使って元ファイルを読み込むか否かの設定
    Result*                       result;             //!< [out] 結果。NULLでもよい。リクエスト終了まで使われ続けるので一時変数を渡さないように注意して下さい。
  private:
    friend class FileThread;
    friend class OtherThread;
    friend class AsyncFileManager;
    void*                         m_pBuf;
    size_t                        m_realReadSize;
    gfl2::thread::Event*          m_syncReqFinishEvent;
  public:
    ArcFileLoadDataBufReq(void)
      : fileName(NULL),
        arcId(ArcFile::ARCID_NULL),
        datId(ArcFile::ARCDATID_NULL),
        lang(ToolDefine::ArcLangType::DEFAULT_LANG),
        prio(REQ_PRIO_NORMAL),
        pBuf(NULL),
        bufSize(0),
        pRealReadSize(NULL),
        alignForCompressed(4),
        heapForReq(NULL),
        heapForCompressed(NULL),
        arcSrcUseSetting(ArcSrcUseSetting::SAME_AS_MANAGER),
        result(NULL),
        m_pBuf(NULL),
        m_realReadSize(0),
        m_syncReqFinishEvent(NULL)
    {}
  };

  //! @brief garcファイル内のデータ1件の一部を読み込むリクエスト(あらかじめ確保してあるメモリに読み込む版)
  class ArcFileLoadDataPieceBufReq  // (req07)
  {
  public:
    const char*                   fileName;           //!< [in]  ファイル名。終端は'\0'になっていること。arcIdがARCID_NULLのときのみ使われる。
    ArcFile::ARCID                arcId;              //!< [in]  ARCID。ARCID_NULLのときはfileNameが使われる。
    ArcFile::ARCDATID             datId;              //!< [in]  アーカイブファイル内データID
    ToolDefine::ArcLangType::Tag  lang;               //!< [in]  対象言語。
    size_t                        offsetFromData;     //!< [in]  読み込み開始オフセット(byte)(datIdデータ先頭からのオフセット)
    size_t                        readSize;           //!< [in]  読み込みサイズ。offsetFromDataでシークしたポインタからreadSizeだけ読み込む。データ1件のサイズより大きなサイズを指定することも可。
    u8                            prio;               //!< [in]  リクエストの実行優先度(値が小さい方が優先度が高い)
    void*                         pBuf;               //!< [out] 呼び出し元であらかじめメモリを確保しておいたバッファ。ここにファイルから読み込んだ内容を入れる。  // ****キー****
    size_t                        bufSize;            //!< [in]  bufのサイズ。データサイズ(パディングを含む)以上にして下さい。
    size_t*                       pRealReadSize;      //!< [out] 実際に読み込んだサイズ。NULLでもよい。
    gfl2::heap::HeapBase*         heapForReq;         //!< [in]  リクエスト終了まで一時的に使用するヒープ。同期版ではNULLでよい。
    Result*                       result;             //!< [out] 結果。NULLでもよい。リクエスト終了まで使われ続けるので一時変数を渡さないように注意して下さい。
  private:
    friend class FileThread;
    friend class OtherThread;
    friend class AsyncFileManager;
    gfl2::thread::Event*          m_syncReqFinishEvent;
  public:
    ArcFileLoadDataPieceBufReq(void)
      : fileName(NULL),
        arcId(ArcFile::ARCID_NULL),
        datId(ArcFile::ARCDATID_NULL),
        lang(ToolDefine::ArcLangType::DEFAULT_LANG),
        offsetFromData(0),
        readSize(0),
        prio(REQ_PRIO_NORMAL),
        pBuf(NULL),
        bufSize(0),
        pRealReadSize(NULL),
        heapForReq(NULL),
        result(NULL),
        m_syncReqFinishEvent(NULL)
    {}
  };

  //! @brief garcファイル内のデータ1件の読み込みポインタをシークするリクエスト( 取り扱い注意！)  // 【使用禁止】代わりにArcFileLoadDataPieceBufReqを使って下さい！  // @fix CONNMCat[555] ArcFileSeekDataPtrReqとArcFileLoadDataPtrBufReqの間に他のリクエストが入ってしまうことがあるのでこれは使用禁止。
  class ArcFileSeekDataPtrReq  // (req08)
  {
  public:
    const char*                   fileName;           //!< [in]  ファイル名。終端は'\0'になっていること。arcIdがARCID_NULLのときのみ使われる。  // ****キー****
    ArcFile::ARCID                arcId;              //!< [in]  ARCID。ARCID_NULLのときはfileNameが使われる。  // ****キー****
    ArcFile::ARCDATID             datId;              //!< [in]  アーカイブファイル内データID
    ToolDefine::ArcLangType::Tag  lang;               //!< [in]  対象言語。
    size_t                        offsetFromData;     //!< [in]  シークサイズ(byte)(datIdデータ先頭からのオフセット)
    u8                            prio;               //!< [in]  リクエストの実行優先度(値が小さい方が優先度が高い)
    gfl2::heap::HeapBase*         heapForReq;         //!< [in]  リクエスト終了まで一時的に使用するヒープ。同期版ではNULLでよい。
    Result*                       result;             //!< [out] 結果。NULLでもよい。リクエスト終了まで使われ続けるので一時変数を渡さないように注意して下さい。
  private:
    friend class FileThread;
    friend class OtherThread;
    friend class AsyncFileManager;
    gfl2::thread::Event*          m_syncReqFinishEvent;
  public:
    ArcFileSeekDataPtrReq(void)
      : fileName(NULL),
        arcId(ArcFile::ARCID_NULL),
        datId(ArcFile::ARCDATID_NULL),
        lang(ToolDefine::ArcLangType::DEFAULT_LANG),
        offsetFromData(0),
        prio(REQ_PRIO_NORMAL),
        heapForReq(NULL),
        result(NULL),
        m_syncReqFinishEvent(NULL)
    {}
  };

  //! @brief ArcFileSeekDataPtrReqでシークしたポインタからデータ読み込むリクエスト( 取り扱い注意！)(あらかじめ確保してあるメモリに読み込む版)  // 【使用禁止】代わりにArcFileLoadDataPieceBufReqを使って下さい！  // @fix CONNMCat[555] ArcFileSeekDataPtrReqとArcFileLoadDataPtrBufReqの間に他のリクエストが入ってしまうことがあるのでこれは使用禁止。
  class ArcFileLoadDataPtrBufReq  // (req09)
  {
  public:
    const char*            fileName;           //!< [in]  ファイル名。終端は'\0'になっていること。arcIdがARCID_NULLのときのみ使われる。
    ArcFile::ARCID         arcId;              //!< [in]  ARCID。ARCID_NULLのときはfileNameが使われる。
    size_t                 readSize;           //!< [in]  読み込みサイズ
    u8                     prio;               //!< [in]  リクエストの実行優先度(値が小さい方が優先度が高い)
    void*                  pBuf;               //!< [out] 呼び出し元であらかじめメモリを確保しておいたバッファ。ここにファイルから読み込んだ内容を入れる。  // ****キー****
    size_t                 bufSize;            //!< [in]  pBufのサイズ。readSize以上にして下さい。
    size_t*                pRealReadSize;      //!< [out] 実際に読み込んだバイト数。NULLでもよい。
    gfl2::heap::HeapBase*  heapForReq;         //!< [in]  リクエスト終了まで一時的に使用するヒープ。同期版ではNULLでよい。
    Result*                result;             //!< [out] 結果。NULLでもよい。リクエスト終了まで使われ続けるので一時変数を渡さないように注意して下さい。
  private:
    friend class FileThread;
    friend class OtherThread;
    friend class AsyncFileManager;
    gfl2::thread::Event*   m_syncReqFinishEvent;
  public:
    ArcFileLoadDataPtrBufReq(void)
      : fileName(NULL),
        arcId(ArcFile::ARCID_NULL),
        readSize(0),
        prio(REQ_PRIO_NORMAL),
        pBuf(NULL),
        bufSize(0),
        pRealReadSize(NULL),
        heapForReq(NULL),
        result(NULL),
        m_syncReqFinishEvent(NULL)
    {}
  };



public:
  //! @brief  コンストラクタ
  //! @param[in] heap               ヒープ。
  //! @param[in] mainDevice         非同期ファイルマネージャがメインで扱うデバイス。
  //! @param[in] hioDevice          HIOを使うときにmainDeviceの代わりに扱うデバイス。
  //                                NULLのときはHIOは使いません。
  //! @param[in] arcSrcDevice       ArcSrcファイルの読み込み時に使うデバイス。
  //                                NULLのときはArcSrcファイルを読み込むことはありません。
  //! @param[in] fileThreadPrio     ファイルスレッドの優先度(メインスレッドより優先度を高くする)。
  //! @param[in] otherThreadPrio    その他スレッドの優先度(メインスレッドと同じ優先度)。
  //! @param[in] heapForFsDebugLog  ファイルシステムのデバッグ用のログのヒープ。通常ヒープメモリでもデバイスメモリでもいい。 
  //                                NULLのときはデバッグ用のログを録りません。
  //! @param[in] fsDebugLogArcFileInfoElemNumMax  ArcFileのログFsDebugLogArcFileInfoの最大数。
  AsyncFileManager(gfl2::heap::HeapBase* heap, const DeviceBase* mainDevice, const DeviceBase* hioDevice, const DeviceBase* arcSrcDevice, int fileThreadPrio, int otherThreadPrio, gfl2::heap::HeapBase* heapForFsDebugLog, u32 fsDebugLogArcFileInfoElemNumMax=0);
  //! @brief  デストラクタ
  virtual ~AsyncFileManager();


  //! @name  Add/Cancel/IsFinished
  //@{

  void AddFileEasyReadReq           (const FileEasyReadReq&            req);  // (req01)
  void AddFileEasyWriteReq          (const FileEasyWriteReq&           req);  // (req02)
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
  //! @brief  必ずAddArcFileOpenReqマクロを使って下さい。 pAsyncFileManager->AddArcFileOpenReq (req);というふうに使えます。
  void __AddArcFileOpenReq          (const ArcFileOpenReq&             req, const char* callerFileName, u32 callerLineNo);  // (req03)
#else
  void AddArcFileOpenReq            (const ArcFileOpenReq&             req);  // (req03)
#endif
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
  //! @brief  必ずAddArcFileCloseReqマクロを使って下さい。pAsyncFileManager->AddArcFileCloseReq(req);というふうに使えます。
  void __AddArcFileCloseReq         (const ArcFileCloseReq&            req, const char* callerFileName, u32 callerLineNo);  // (req04)
#else
  void AddArcFileCloseReq           (const ArcFileCloseReq&            req);  // (req04)
#endif
  void AddArcFileLoadDataReq        (const ArcFileLoadDataReq&         req);  // (req05)
  void AddArcFileLoadDataBufReq     (const ArcFileLoadDataBufReq&      req);  // (req06)
  void AddArcFileLoadDataPieceBufReq(const ArcFileLoadDataPieceBufReq& req);  // (req07)
  //void AddArcFileSeekDataPtrReq     (const ArcFileSeekDataPtrReq&      req);  // (req08)
  //void AddArcFileLoadDataPtrBufReq  (const ArcFileLoadDataPtrBufReq&   req);  // (req09)

  //! @brief   リクエストをキャンセルする。
  //! @retval true   リクエストの処理を開始するのを待っているとき → キャンセル成功。リクエストは破棄されます。
  //! @retval false  (1) リクエストをAddしていないとき
  //                 (2) リクエストの処理が既に完了しているとき
  //                 (3) リクエストの処理を実行している最中のとき → キャンセル失敗。リクエストの処理が完了するのを待って下さい。
  //! @note  【使用方法】IsXxxFinishedのチェックを先に行えば、戻り値がfalseのときの(1)(2)は排除できます。
  bool CancelFileEasyReadReq           (void**         ppBuf   );  // (req01)
  bool CancelFileEasyWriteReq          (const char*    fileName);  // (req02)
  bool CancelArcFileOpenReq            (const char*    fileName);  // (req03)
  bool CancelArcFileOpenReq            (ArcFile::ARCID arcId   );  // (req03)
  bool CancelArcFileCloseReq           (const char*    fileName);  // (req04)
  bool CancelArcFileCloseReq           (ArcFile::ARCID arcId   );  // (req04)
  bool CancelArcFileLoadDataReq        (void**         ppBuf   );  // (req05)
  bool CancelArcFileLoadDataBufReq     (const void*    pBuf    );  // (req06)
  bool CancelArcFileLoadDataPieceBufReq(const void*    pBuf    );  // (req07)
  //bool CancelArcFileSeekDataPtrReq     (const char*    fileName);  // (req08)
  //bool CancelArcFileSeekDataPtrReq     (ArcFile::ARCID arcId   );  // (req08)
  //bool CancelArcFileLoadDataPtrBufReq  (const void*    pBuf    );  // (req09)

  //! @brief   全てのリクエストをキャンセルする。
  //           キャンセルできるリクエストは破棄されます。
  //           キャンセルできないリクエストは残ります。
  //! @retval true   処理を実行しているリクエストがなく、1つ以上のリクエストの処理を開始するのを待っているときに、その全てのリクエストのキャンセルに成功。
  //! @retval false  (1) 1つもリクエストをAddしていないとき
  //                 (2) 全てのリクエストの処理が既に完了しているとき
  //                 (3) 1つ以上のリクエストの処理を実行している最中のときに、それらのリクエストのキャンセルに失敗。
  //                     falseを返していますが、キャンセルできるリクエストは破棄している点に注意して下さい。
  //! @note  【使用方法】IsAllReqFinishedのチェックを先に行えば、戻り値がfalseのときの(1)(2)は排除できます。
  //! @note  CancelXxxReq関数と戻り値の意味を合わせてあります。
  bool CancelAllReq(void);

  //! @note  Addしていないものに関してはtrueが返ります。
  bool IsFileEasyReadFinished           (void**         ppBuf   ) const;  // (req01)
  bool IsFileEasyWriteFinished          (const char*    fileName) const;  // (req02)
  bool IsArcFileOpenFinished            (const char*    fileName) const;  // (req03)
  bool IsArcFileOpenFinished            (ArcFile::ARCID arcId   ) const;  // (req03)
  bool IsArcFileCloseFinished           (const char*    fileName) const;  // (req04)
  bool IsArcFileCloseFinished           (ArcFile::ARCID arcId   ) const;  // (req04)
  bool IsArcFileLoadDataFinished        (void**         ppBuf   ) const;  // (req05)
  bool IsArcFileLoadDataBufFinished     (const void*    pBuf    ) const;  // (req06)
  bool IsArcFileLoadDataPieceBufFinished(const void*    pBuf    ) const;  // (req07)
  //bool IsArcFileSeekDataPtrFinished     (const char*    fileName) const;  // (req08)
  //bool IsArcFileSeekDataPtrFinished     (ArcFile::ARCID arcId   ) const;  // (req08)
  //bool IsArcFileLoadDataPtrBufFinished  (const void*    pBuf    ) const;  // (req09)

  bool IsAllReqFinished(void) const;

  //! @note  同期でAdd*ReqとIs*Finishedを行う。
  void SyncFileEasyRead           (const FileEasyReadReq&            req);  // (req01)
  void SyncFileEasyWrite          (const FileEasyWriteReq&           req);  // (req02)
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
  //! @brief  必ずSyncArcFileOpenマクロを使って下さい。 pAsyncFileManager->SyncArcFileOpen (req);というふうに使えます。
  void __SyncArcFileOpen          (const ArcFileOpenReq&             req, const char* callerFileName, u32 callerLineNo);  // (req03)
#else
  void SyncArcFileOpen            (const ArcFileOpenReq&             req);  // (req03)
#endif
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
  //! @brief  必ずSyncArcFileCloseマクロを使って下さい。pAsyncFileManager->SyncArcFileClose(req);というふうに使えます。
  void __SyncArcFileClose         (const ArcFileCloseReq&            req, const char* callerFileName, u32 callerLineNo);  // (req04)  //! @note  同期版ではheapForReqは使いませんので、reqのheapForReqはNULLでOKです。
#else
  void SyncArcFileClose           (const ArcFileCloseReq&            req);  // (req04)  //! @note  同期版ではheapForReqは使いませんので、reqのheapForReqはNULLでOKです。
#endif
  void SyncArcFileLoadData        (const ArcFileLoadDataReq&         req);  // (req05)  //! @note  『「同期版」かつ「ArcSrcを使わないとき」』はheapForReqは使いませんので、reqのheapForReqはNULLでOKです。
  void SyncArcFileLoadDataBuf     (const ArcFileLoadDataBufReq&      req);  // (req06)  //! @note  『「同期版」かつ「ArcSrcを使わないとき」』はheapForReqは使いませんので、reqのheapForReqはNULLでOKです。
  void SyncArcFileLoadDataPieceBuf(const ArcFileLoadDataPieceBufReq& req);  // (req07)  //! @note  同期版ではheapForReqは使いませんので、reqのheapForReqはNULLでOKです。
  //void SyncArcFileSeekDataPtr     (const ArcFileSeekDataPtrReq&      req);  // (req08)  //! @note  同期版ではheapForReqは使いませんので、reqのheapForReqはNULLでOKです。
  //void SyncArcFileLoadDataPtrBuf  (const ArcFileLoadDataPtrBufReq&   req);  // (req09)  //! @note  同期版ではheapForReqは使いませんので、reqのheapForReqはNULLでOKです。

  //@}


  //! @brief  終了を要求する
  void RequestEnd(void);

  //! @brief   終了しているか。終了していればこのインスタンスを破棄できます。
  //! @return  終了しているか、していないか
  //! @retval  true   終了している
  //! @retval  false  まだ終了していない
  bool HasEnded(void) const;

  //! @brief  スレッド実行中か
  bool IsThreadRunning(void) const;


  //! @name  garcファイルの名前
  //@{

public:
  //! @brief garcファイルの名前テーブルを設定する
  //! @param type                  タイプ
  //! @param heap                  ヒープ
  //! @param arcFileNameTable      ARCID(*.garcファイル自体のID(ARCID_*))に対応するファイル名の配列。
  //!                              このクラスではポインタしか保持しないので、呼び出し元でarcFileNameTableを保持しておいて下さい。
  //! @param arcFileNum            arcFileNameTableの要素数
  //! @param arcFileIgnoreNameLen  最初の何文字を無視するか
  //! @param arcFilePrefixName     名前の最初に追加する文字列。追加する文字列がないときはNULLでもよい。
  //!                              終端は'\0'になっていること。
  //!                              区切りは\でも/でもよいが、ディレクトリ名を足したいなら終端は\か/にしておくこと。
  Result SetArcFileNameTable(ArcFileNameTableType::Tag type, gfl2::heap::HeapBase* heap, const char** arcFileNameTable, size_t arcFileNum, u32 arcFileIgnoreNameLen, const char* arcFilePrefixName);

  //! @brief ARCIDに対応するgarcファイル名を取得する(削除も追加も済んでいる名前)
  //! @param type   タイプ
  //! @param heap   名前用のメモリ確保に使うヒープ
  //! @param arcId  ARCID(*.garcファイル自体のID(ARCID_*))
  //! @return  arcIdが不正の場合、名前用のメモリ確保を行わずNULLを返す。
  //!          arcIdが正しい場合、名前用のメモリ確保をheapから行いそのバッファのポインタを返す。呼び出し元でGflHeapFreeMemory(戻り値のポインタ)で解放して下さい。
  char* GetUsableArcFileName(ArcFileNameTableType::Tag type, gfl2::heap::HeapBase* heap, ArcFile::ARCID arcId) const;

private:
  //! @brief ARCIDに対応するgarcファイル名を取得する(削除も追加もしていない名前)
  //! @param type   タイプ
  //! @param arcId  ARCID(*.garcファイル自体のID(ARCID_*))
  //! @return  arcIdが不正の場合NULLを返す。
  const char* getArcFileName(ArcFileNameTableType::Tag type, ArcFile::ARCID arcId) const;

  //! @brief garcファイルの名前の最初の何文字を無視するか
  //! @param type   タイプ
  u32 getArcFileIgnoreNameLen(ArcFileNameTableType::Tag type) const;

  //! @brief garcファイルの名前の最初に追加する文字列を取得する
  //! @return NULLのとき追加する文字列はない
  const char* getArcFilePrefixName(ArcFileNameTableType::Tag type) const;

  //@}



public:
  //! @brief リクエスト発行者の管理に使うメモリ。このメモリはきれいには使われないので専用のヒープを渡して下さい。
  void SetCallerHeap(gfl2::heap::HeapBase* heap); 

  //! @brief ArcFileロード間隔チェックに使うメモリ。このメモリはきれいには使われないので専用のヒープを渡して下さい。
  void SetIntervalCheckHeap(gfl2::heap::HeapBase* heap);

  //! @brief  ArcFileロード間隔チェックの警告レベル
  //! @param[in]  level  警告レベル(IntervalCheckLevel::Tagの値)
  void SetIntervalCheckLevel(s32 level);
  s32 GetIntervalCheckLevel(void) const;



  //! @name  CTRのHIO
  //@{
  
      //! @name  friendが呼び出し可能
      //@{
private:
  const DeviceBase* GetHioDevice(void) const;

  //! @brief  HIOを使って、ロム内にあるファイルではなくPC上のファイルを扱うか否か判定する(通常ファイルもgarcファイルも対象)
  //! @note  Validityとユーザ設定の両方を反映したもの
  bool GetHioUseJudgement(HioUseSetting::Tag hioUseSetting) const;
  
  //! @brief  HIOを使って、ロム内にあるファイルではなくPC上のファイルを扱うことが可能か否か判定する(通常ファイル(garcでないファイル)の扱いだけに限定)
  //! @note  Validityとユーザ設定の両方を反映したもの
  bool GetHioFileUseJudgement(HioUseSetting::Tag hioUseSetting) const;
      //@}

private:
  //! @brief  HIOを使って、ロム内にあるファイルではなくPC上のファイルを扱うことが可能か否か(通常ファイルもgarcファイルも対象)
  //! @note  ユーザ設定は関係なく、可能か否かだけを返す
  bool getHioUseValidity(void) const;

  //! @brief  HIOを使って、ロム内にあるファイルではなくPC上のファイルを扱うことが可能か否か(通常ファイル(garcでないファイル)の扱いだけに限定)
  //! @note  ユーザ設定は関係なく、可能か否かだけを返す
  bool getHioFileUseValidity(void) const;

public:
  //! @brief  HIOを使って、ロム内にあるファイルではなくPC上のファイルを扱うか否か(通常ファイルもgarcファイルも対象)
  //! @param[in]  useFlag  使う場合true、使わない場合false
  //! @note  Validityとユーザ設定の両方を反映したもの
  void SetHioUseFlag(bool useFlag);
  bool GetHioUseFlag(void) const;

  //@}




  //! @name  ArcSrcファイル関連
  //@{
 
      //! @name  friendが呼び出し可能
      //@{
private:
  const DeviceBase* GetArcSrcDevice(void) const;

  //! @brief  「garcのソースを記録したファイル」を読み込み、インスタンスやソーステーブルをつくることが可能か否か判定する
  //! @note  Validityとユーザ設定の両方を反映したもの
  bool GetArcSrcCreateJudgement(ArcSrcCreateSetting::Tag arcSrcCreateSetting) const;
  
  //! @brief  「garcのソースを記録したファイル」を使って、ソースを読み込むことが可能か否か判定する
  //! @note  Validityとユーザ設定の両方を反映したもの
  bool GetArcSrcUseJudgement(ArcSrcUseSetting::Tag arcSrcUseSetting) const;
      //@}

private:
  //! @brief  「garcのソースを記録したファイル」を読み込み、インスタンスやソーステーブルをつくることが可能か否か
  //! @note  ユーザ設定は関係なく、可能か否かだけを返す
  bool getArcSrcCreateValidity(void) const;
  
  //! @brief  「garcのソースを記録したファイル」を使って、ソースを読み込むことが可能か否か
  //! @note  ユーザ設定は関係なく、可能か否かだけを返す
  bool getArcSrcUseValidity(void) const;

public:
  //! @brief ArcSrcファイルの読み込みに使うデフォルトメモリを設定する
  void SetArcSrcDefaultHeap(gfl2::heap::HeapBase* heap);
  gfl2::heap::HeapBase* GetArcSrcDefaultHeap(void) const;

  //! @brief  「garcのソースを記録したファイル」を読み込み、インスタンスやソーステーブルをつくるか否か
  //! @param[in]  createFlag  つくる場合true、つくらない場合false
  //! @note  Validityとユーザ設定の両方を反映したもの
  void SetArcSrcCreateFlag(bool createFlag);
  bool GetArcSrcCreateFlag(void) const;

  //! @brief  「garcのソースを記録したファイル」を使って、ソースを読み込むか否か
  //! @param[in]  useFlag  使う場合true、使わない場合false
  //! @note  Validityとユーザ設定の両方を反映したもの
  void SetArcSrcUseFlag(bool useFlag);
  bool GetArcSrcUseFlag(void) const;

  //@}

  
  //! @name  DirectSdmcにあるgarcファイル関連
  //@{
  
public:
  //! @brief  DirectSdmcにgarcファイルがあるか否かのテーブルを作成する
  //! @note  DirectSdmc上のgarcファイルに同期アクセスしているcheckDirectSdmcArcFileExistを呼んでいる
  void CreateDirectSdmcArcFileExistTable(gfl2::heap::HeapBase* heap);

  //! @brief  ロム内にあるファイルではなくDirectSdmc上のgarcファイルを使うか否か
  //! @param[in]  useFlag  使う場合true、使わない場合false
  //! @note  Validityとユーザ設定の両方を反映したもの
  void SetDirectSdmcArcFileUseFlag(bool useFlag);
  bool GetDirectSdmcArcFileUseFlag(void) const;

private:
  //! @brief  DirectSdmcにgarcファイルがあるなら、そのUsableArcFileNameを取得する
  char* getUsableDirectSdmcArcFileName(gfl2::heap::HeapBase* heap, ArcFile::ARCID arcId) const;

  //! @brief  DirectSdmcにgarcファイルがあるか確認する(m_directSdmcArcFileExistTable使用版)
  bool checkDirectSdmcArcFileExistFast(ArcFile::ARCID arcId) const;

  //! @brief  DirectSdmcにgarcファイルがあるか確認する
  //! @param[in]  tmpHeap  この関数内で一時的に使用するヒープ。この関数を出るときには解放している。
  //! @note  DirectSdmc上のgarcファイルに同期アクセスしている
  bool checkDirectSdmcArcFileExist(gfl2::heap::HeapBase* tmpHeap, ArcFile::ARCID arcId) const;

  //@}


  //! @name  PcUtil
  //@{

      //! @name  friendが呼び出し可能
      //@{
private:
  //! @brief  AsyncFileManagerに設定されている「PCにあるファイルを扱うデバイス」を取得する。
  const DeviceBase* GetPcDevice(void) const;
      //@}
  //@}




public:
  //! @brief  デフォルトの対象言語を設定する
  void SetDefaultArcLang(ToolDefine::ArcLangType::Tag lang);
  //void SetDefaultArcLangByPmVersion(u32 lang);                  // pm_versionの言語コード   (ポケモンに埋め込む言語コード)    ??_project/prog/include/pm_version.h          #define LANG_NULL  #define LANG_JAPAN  #define LANG_ENGLISH  ...
  //void SetDefaultArcLangByPmLanguage(xy::MSGLANGID lang);  // pm_languageの言語コード  (メッセージスタジオの言語コード)  ??_project/prog/include/system/pm_language.h  enum MSGLANGID{ MSGLANGID_KANA, MSGLANGID_KANJI, MSGLANGID_ENGLISH, ... };



  //! @brief  garcファイルを取得する
  //! @param[in]  fileName  ファイル名。終端は'\0'になっていること。arcIdがARCID_NULLのときのみ使われる。  // ****キー****
  //! @param[in]  arcId     ARCID。ARCID_NULLのときはfileNameが使われる。  // ****キー****
  //! @return  ArcFileのconstポインタ。constポインタなので、データ数やデータサイズの取得しかできません。
  //! @retval  NULL以外  ArcFileのconstポインタ。オープンしている。
  //! @retval  NULL      オープンしているArcFileがない。
  //! @note  今現在オープンしているときだけポインタを返す。
  //         「オープンリクエストは出しているがまだオープンしていない」ときはNULLを返す。
  //         「クローズリクエストが出ているがまだクローズしていない」ときはポインタを返す。
  const ArcFile* GetArcFile(const char* fileName) const;
  const ArcFile* GetArcFile(ArcFile::ARCID arcId) const;




  ////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////
  //
  // 非公開
  //
  ////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////

private:
  static const u8 REQ_PRIO_OFFSET_SYNC  =  64;  //!< リクエストの実行優先度のオフセット  優先度高い。同期用。6ビット目を1にする。
  static const u8 REQ_PRIO_OFFSET_ASYNC = 128;  //!< リクエストの実行優先度のオフセット  優先度低い。非同期用。7ビット目を1にする。

private:
  //! @brief  garcファイルの名前テーブルの情報
  class ArcFileNameTableInfo
  {
  public:
    const char**    nameTable;
    size_t          num;
    u32             ignoreNameLen;
    char*           prefixName;     // 終端の'\0'の分までメモリを確保しているので、(prefixNameLen +1)ある。終端は'\0'になっている。区切りは/になっている。
    u32             prefixNameLen;  // 終端の'\0'は含まない長さ
  public:
    ArcFileNameTableInfo(void)
      : nameTable(NULL),
        num(0),
        ignoreNameLen(0),
        prefixName(NULL),
        prefixNameLen(0)
    {}
    virtual ~ArcFileNameTableInfo()
    {
      if(prefixName) GflHeapSafeFreeMemory(prefixName);
    }
  };


#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
  class CallerLists
  {
  public:
    List* arcFileOpenReqCallerList;
    List* arcFileCloseReqCallerList;
  public:
    CallerLists(void)
      : arcFileOpenReqCallerList(NULL),
        arcFileCloseReqCallerList(NULL)
    {}
  };
#endif

#if GFL_FS_DEBUG_CHECK_ARC_FILE_LOAD_INTERVAL
  //! @brief  ArcFileロード間隔チェック情報
  class IntervalCheckInfo
  {
  public:
    s64 firstAccessTime;
    s64 lastAccessTime;
  public:
    IntervalCheckInfo(void)
      : firstAccessTime(0),
        lastAccessTime(0)
    {}
  };
#endif


  //! @brief  ファイルのリスト要素のタイプ
  class FileElemType
  {
  public:
    enum Tag
    {
      ELEM_BASE,

      FILE,
      ARC_FILE,
    };
  };


  //! @brief  リクエストのリスト要素
  template<typename T_REQ, ReqElemType::Tag T_ELEM_TYPE>
  class ReqElemBase : public ElemBase
  {
    GFL_FORBID_COPY_AND_ASSIGN(ReqElemBase);

  private:
    static const ReqElemType::Tag ELEM_TYPE = T_ELEM_TYPE;
  public:
    ReqElemBase(void)
      : ElemBase(),
        m_deleteOnRemoveFlag(false)
    {}
    virtual ~ReqElemBase() {}
    virtual s32 GetElemType(void) const { return ELEM_TYPE; }
    void SetReq(const T_REQ& req) { m_req = req; }
    T_REQ* GetReq(void) { return &m_req; }
    void SetDeleteOnRemoveFlag(bool deleteOnRemoveFlag) { m_deleteOnRemoveFlag = deleteOnRemoveFlag; }
    bool GetDeleteOnRemoveFlag(void) const { return m_deleteOnRemoveFlag; }
  private:
    T_REQ m_req;
    bool  m_deleteOnRemoveFlag;  // リストから取り除くときにElemをdeleteする場合はtrue、deleteしない場合はfalse。
                                 // @note 非同期でのリクエストの場合true、同期でのリクエストの場合false、という使われ方になっているので、この使われ方が変わる場合は見直しをしたほうがいいかもしれない。
  };

  class FileEasyReadReqElem            : public ReqElemBase<FileEasyReadReq           , ReqElemType::FILE_EASY_READ_REQ              > {};  // (req01)
  class FileEasyWriteReqElem           : public ReqElemBase<FileEasyWriteReq          , ReqElemType::FILE_EASY_WRITE_REQ             > {};  // (req02)
  class ArcFileOpenReqElem             : public ReqElemBase<ArcFileOpenReq            , ReqElemType::ARC_FILE_OPEN_REQ               > {};  // (req03)
  class ArcFileCloseReqElem            : public ReqElemBase<ArcFileCloseReq           , ReqElemType::ARC_FILE_CLOSE_REQ              > {};  // (req04)
  class ArcFileLoadDataReqElem         : public ReqElemBase<ArcFileLoadDataReq        , ReqElemType::ARC_FILE_LOAD_DATA_REQ          > {};  // (req05)
  class ArcFileLoadDataBufReqElem      : public ReqElemBase<ArcFileLoadDataBufReq     , ReqElemType::ARC_FILE_LOAD_DATA_BUF_REQ      > {};  // (req06)
  class ArcFileLoadDataPieceBufReqElem : public ReqElemBase<ArcFileLoadDataPieceBufReq, ReqElemType::ARC_FILE_LOAD_DATA_PIECE_BUF_REQ> {};  // (req07)
  class ArcFileSeekDataPtrReqElem      : public ReqElemBase<ArcFileSeekDataPtrReq     , ReqElemType::ARC_FILE_SEEK_DATA_PTR_REQ      > {};  // (req08)
  class ArcFileLoadDataPtrBufReqElem   : public ReqElemBase<ArcFileLoadDataPtrBufReq  , ReqElemType::ARC_FILE_LOAD_DATA_PTR_BUF_REQ  > {};  // (req09)

  static bool getReqElemDeleteOnRemoveFlag(const ElemBase* elem);


  //! @brief  ファイルのリスト要素
  template<typename T_FILE, FileElemType::Tag T_ELEM_TYPE>
  class FileElemBase : public ElemBase
  {
    GFL_FORBID_COPY_AND_ASSIGN(FileElemBase);

  private:
    static const FileElemType::Tag ELEM_TYPE = T_ELEM_TYPE;
  public:
    FileElemBase(void)
      : ElemBase(),
        m_file(NULL)
    {}
    virtual ~FileElemBase() {}
    virtual s32 GetElemType(void) const { return ELEM_TYPE; }
    void SetFile(T_FILE* file) { m_file = file; }
    T_FILE* GetFile(void) const { return m_file; }
  public:
    T_FILE* m_file;
    // リストから取り除くときにElemをdeleteする
  };
  
  //! @brief  Fileのリスト要素 
  class FileElem    : public FileElemBase<File   , FileElemType::FILE    > {};

#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
  //! @brief  リクエスト発行者のリスト要素
  class CallerElem : public ElemBase
  {
    GFL_FORBID_COPY_AND_ASSIGN(CallerElem);
  
  public:
    CallerElem(void);
    virtual ~CallerElem();
  public:
    //! @brief  リクエスト発行者の情報を設定する。受け取ったポインタはこのクラスで管理しこのクラスで破棄する。
    //! @param[in] callerInfo  GFL_NEWで確保したポインタを渡して下さい。このインスタンスを破棄するときにGFL_DELETEで破棄します。
    void SetCallerInfo(CallerInfo* callerInfo); 
    //! @brief  リクエスト発行者の情報を取得する。
    const CallerInfo* GetCallerInfo(void) const;
  private:
    CallerInfo* m_callerInfo;
  };
#endif

  //! @brief  ArcFileのリスト要素
  class ArcFileElem : public FileElemBase<ArcFile, FileElemType::ARC_FILE>
  {
    GFL_FORBID_COPY_AND_ASSIGN(ArcFileElem);

  public:
    ArcFileElem(void);
    virtual ~ArcFileElem();
  public:
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
    void IncreaseCount(gfl2::heap::HeapBase* heap, CallerInfo* callerInfo);
    void DecreaseCount(gfl2::heap::HeapBase* heap, CallerInfo* callerInfo);
#else
    void IncreaseCount(void);
    void DecreaseCount(void);
#endif
    u32 GetCount(void) const { return m_count; }
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
    void CreateAllCallerList(gfl2::heap::HeapBase* heap);  // 破棄はGFL_SAFE_DELETEで行う。個々のリストの破棄はdestroyCallerListAndDeleteAllElemで行う。
    CallerElem* GetArcFileOpenReqCallerListHeadElem(void) const;
    CallerElem* GetArcFileCloseReqCallerListHeadElem(void) const;
#endif
#if GFL_FS_DEBUG_CHECK_ARC_FILE_LOAD_INTERVAL
    void CreateIntervalCheckInfo(gfl2::heap::HeapBase* heap);  // 破棄はGFL_SAFE_DELETEで行う。
    void LogIntervalCheckInfoAccessTime(bool accessFinish, s32 level);
#endif
  private:
    void increaseCount(void);
    void decreaseCount(void);
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
    static void destroyCallerListAndDeleteAllElem(List* list);
#endif
  private:
    u32 m_count;
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
    CallerLists* m_callerLists;
#endif
#if GFL_FS_DEBUG_CHECK_ARC_FILE_LOAD_INTERVAL
    IntervalCheckInfo* m_intervalCheckInfo;
#endif
  };


private:
  //! @name  friendが呼び出す関数
  //@{

  const DeviceBase* GetMainDevice(void) const;
  const DeviceBase* GetMainDevice(ArcFile::ARCID arcId) const;
  const DeviceBase* GetMainDevice(gfl2::heap::HeapBase* tmpHeap, const char* fileName) const;  // tmpHeapは一時的に使用するヒープ

  gfl2::thread::Event* GetReqListExecEvent(void) const { return m_reqListExecEvent; }
  gfl2::thread::Event* GetUncompListExecEvent(void) const { return m_uncompListExecEvent; }
  gfl2::thread::Event* GetSyncReqFinishEvent(void) const { return m_syncReqFinishEvent; }

  ElemBase*  GetReqListHeadElemAndSetReqListElemNow(void);
  void       DeleteReqListElemAndUnsetReqListElemNow(ElemBase* elem);

  ElemBase*  GetUncompListHeadElemAndSetUncompListElemNow(void);
  void       DeleteUncompListElemAndUnsetUncompListElemNow(ElemBase* elem);

  ElemBase* GetFileElem(const char* fileName) const;
  void      DeleteFileElem(const char* fileName);

  ElemBase* GetArcFileElem(ArcFile::ARCID arcId) const;
  void      DeleteArcFileElem(ArcFile::ARCID arcId);

#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  u32 GetArcFileCount(const char* fileName) const;  // friendであるMatchCheckReqElemからだけ呼び出す関数
  u32 GetArcFileCount(ArcFile::ARCID arcId) const;  // friendであるMatchCheckReqElemからだけ呼び出す関数
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST

  //! @param[in] req  uncompList用の要素を作成しreqの中身をそこへ渡したら、重複する情報はreqから消しておく。
  void AddFileEasyReadUncomp      (FileEasyReadReq*       req);  // (req01)
  void AddArcFileLoadDataUncomp   (ArcFileLoadDataReq*    req);  // (req05)
  void AddArcFileLoadDataBufUncomp(ArcFileLoadDataBufReq* req);  // (req06)

  void AddFile(gfl2::heap::HeapBase* heap, File* file);
  //! @note カウントは増やさないので、増やすなら呼び出し元で増やして下さい。
  ArcFileElem* AddArcFile(gfl2::heap::HeapBase* heap, ArcFile* file);

  //! @brief  garcファイルの対象言語を取得する(デフォルトの対象言語と引数で渡した対象言語から決める)
  ToolDefine::ArcLangType::Tag GetLangOfArcFile(ToolDefine::ArcLangType::Tag langOfArcFile) const;

#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
  gfl2::heap::HeapBase* GetCallerHeap(void) const { return m_callerHeap; }
#endif

  //@}


private:
  void addArcFileOpenReq  (const ArcFileOpenReq&  req);    // (req03)
  void addArcFileCloseReq (const ArcFileCloseReq& req);    // (req04)
  void syncArcFileOpen    (const ArcFileOpenReq&  req);    // (req03)
  void syncArcFileClose   (const ArcFileCloseReq& req);    // (req04)
  
  bool isAllReqFinished(void) const;
  
private:
  FileEasyReadReqElem*            getFileEasyReadReqListElem           (void**         ppBuf   ) const;  // (req01)
  FileEasyWriteReqElem*           getFileEasyWriteReqListElem          (const char*    fileName) const;  // (req02)
  ArcFileOpenReqElem*             getArcFileOpenReqListElem            (const char*    fileName) const;  // (req03)
  ArcFileOpenReqElem*             getArcFileOpenReqListElem            (ArcFile::ARCID arcId   ) const;  // (req03)
  ArcFileCloseReqElem*            getArcFileCloseReqListElem           (const char*    fileName) const;  // (req04)
  ArcFileCloseReqElem*            getArcFileCloseReqListElem           (ArcFile::ARCID arcId   ) const;  // (req04)
  ArcFileLoadDataReqElem*         getArcFileLoadDataReqListElem        (void**         ppBuf   ) const;  // (req05)
  ArcFileLoadDataBufReqElem*      getArcFileLoadDataBufReqListElem     (const void*    pBuf    ) const;  // (req06)
  ArcFileLoadDataPieceBufReqElem* getArcFileLoadDataPieceBufReqListElem(const void*    pBuf    ) const;  // (req07)
  ArcFileSeekDataPtrReqElem*      getArcFileSeekDataPtrReqListElem     (const char*    fileName) const;  // (req08)
  ArcFileSeekDataPtrReqElem*      getArcFileSeekDataPtrReqListElem     (ArcFile::ARCID arcId   ) const;  // (req08)
  ArcFileLoadDataPtrBufReqElem*   getArcFileLoadDataPtrBufReqListElem  (const void*    pBuf    ) const;  // (req09)

  FileEasyReadReqElem*            getFileEasyReadUncompListElem        (void**         ppBuf   ) const;  // (req01)
  ArcFileLoadDataReqElem*         getArcFileLoadDataUncompListElem     (void**         ppBuf   ) const;  // (req05)
  ArcFileLoadDataBufReqElem*      getArcFileLoadDataBufUncompListElem  (const void*    pBuf    ) const;  // (req06)

  void deleteReqListElem(ElemBase* elem);
  void deleteUncompListElem(ElemBase* elem);
        
  bool cancelReqOnly(ElemBase* reqElem);
  
  // 展開前に確保していたものを破棄する
  void deleteFileEasyReadReqObjBeforeUncomp      (FileEasyReadReq*       req);  // (req01)
  void deleteArcFileLoadDataReqObjBeforeUncomp   (ArcFileLoadDataReq*    req);  // (req05)
  void deleteArcFileLoadDataBufReqObjBeforeUncomp(ArcFileLoadDataBufReq* req);  // (req06) 

  // リスト要素全てを破棄する。
  // デストラクタから呼ぶので、破棄する要素が残っていたらASSERTにする。
  void deleteAllReqListElemAtDestructor(void);
  void deleteAllUncompListElemAtDestructor(void);
  // ファイルリストの要素とその中身を全て破棄する。
  // デストラクタから呼ぶので、破棄する要素が残っていたらASSERTにする。
  void deleteAllFileElemAndContentAtDestructor(void);

#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
  CallerInfo* createCallerInfo(const char* a_callerFileName, u32 a_callerLineNo);
#endif

#if GFL_FS_DEBUG_CHECK_ARC_FILE_LOAD_INTERVAL
  gfl2::heap::HeapBase* getIntervalCheckHeap(void) const { return m_intervalCheckHeap; }
#endif


private:
  // 似た記述をテンプレートにしておく
  
  template<typename T_REQ, typename T_REQ_ELEM>
  T_REQ_ELEM* addReqTemplate(List* list, const T_REQ& req, gfl2::heap::HeapBase* heap, u8 prioOffset)
  {
    T_REQ_ELEM* elem = GFL_NEW(heap) T_REQ_ELEM();
    elem->SetDeleteOnRemoveFlag(true);
    elem->SetReq(req);
    GFL_ASSERT(REQ_PRIO_HIGHEST<=req.prio && req.prio<=REQ_PRIO_LOWEST);  /* 開発中に気付かせるためのASSERT */
    elem->SetPrio(static_cast<s32>(req.prio+prioOffset));
    {
      list->Add(elem);
    }
    return elem;
  }

  template<typename T_REQ, typename T_REQ_ELEM, ReqElemType::Tag T_ELEM_TYPE>
  T_REQ_ELEM* getReqElemTemplate_ppBuf(List* list, void** ppBuf) const
  {
    ElemBase* elem = list->GetHead();
    while(elem)
    {
      if(elem->GetElemType() == T_ELEM_TYPE)
      {
        T_REQ_ELEM* el = static_cast<T_REQ_ELEM*>(elem);
        T_REQ* req = el->GetReq();
        if(req->ppBuf == ppBuf)
        {
          return el;
        }
      }
      elem = elem->GetNext();
    }
    return NULL;
  }
  
  template<typename T_REQ, typename T_REQ_ELEM, ReqElemType::Tag T_ELEM_TYPE>
  T_REQ_ELEM* getReqElemTemplate_fileName(List* list, const char*  fileName) const
  {
    ElemBase* elem = list->GetHead();
    while(elem)
    {
      if(elem->GetElemType() == T_ELEM_TYPE)
      {
        T_REQ_ELEM* el = static_cast<T_REQ_ELEM*>(elem);
        T_REQ* req = el->GetReq();
        if( Str::CheckFileNameIsSameStrictly(req->fileName, fileName)  )
        {
          return el;
        }
      }
      elem = elem->GetNext();
    }
    return NULL;
  }

  template<typename T_REQ, typename T_REQ_ELEM, ReqElemType::Tag T_ELEM_TYPE>
  T_REQ_ELEM* getReqElemTemplate_pBuf(List* list, const void*  pBuf) const
  {
    ElemBase* elem = list->GetHead();
    while(elem)
    {
      if(elem->GetElemType() == T_ELEM_TYPE)
      {
        T_REQ_ELEM* el = static_cast<T_REQ_ELEM*>(elem);
        T_REQ* req = el->GetReq();
        if(req->pBuf == pBuf)
        {
          return el;
        }
      }
      elem = elem->GetNext();
    }
    return NULL;
  }

  template<typename T_REQ, typename T_REQ_ELEM, ReqElemType::Tag T_ELEM_TYPE>
  T_REQ_ELEM* getReqElemTemplate_arcId(List* list, ArcFile::ARCID arcId) const
  {
    ElemBase* elem = list->GetHead();
    while(elem)
    {
      if(elem->GetElemType() == T_ELEM_TYPE)
      {
        T_REQ_ELEM* el = static_cast<T_REQ_ELEM*>(elem);
        T_REQ* req = el->GetReq();
        if(req->arcId == arcId)
        {
          return el;
        }
      }
      elem = elem->GetNext();
    }
    return NULL;
  }

  template<typename T_REQ, typename T_REQ_ELEM, typename T_KEY>
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  void addReqIsFinishedTemplate(const T_REQ& req, T_KEY key, AsyncFileManager* inst, bool (AsyncFileManager::*IsReqFinishedFunc)(T_KEY) const, MatchCheckManager* matchCheckManagerInst, void (MatchCheckManager::*MatchCheckManagerSetAddReqFunc)(const T_REQ&))
#else  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  void addReqIsFinishedTemplate(const T_REQ& req, T_KEY key, AsyncFileManager* inst, bool (AsyncFileManager::*IsReqFinishedFunc)(T_KEY) const)
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  {
    T_REQ_ELEM elem;
    elem.SetDeleteOnRemoveFlag(false);
    elem.SetReq(req);
    GFL_ASSERT(REQ_PRIO_HIGHEST<=req.prio && req.prio<=REQ_PRIO_LOWEST);  // 開発中に気付かせるためのASSERT
    elem.SetPrio(static_cast<s32>(req.prio+REQ_PRIO_OFFSET_SYNC));
    gfl2::thread::Event* syncReqFinishEvent = NULL;
    {
      T_REQ* l_req = elem.GetReq();  // m_reqListにAddした後l_reqの中身が書き換えられることもあるので、Add後にl_reqの中身にアクセスする場合は注意すること。
      {
        gfl2::thread::ScopedLock syncReqScopedLock(*m_syncReqCriticalSection);
        if(m_syncReqExistNow == false)
        {                                                      // syncReqFinishEventは同期でのリクエストの処理が終わったことを伝えるイベントのポインタ。実体はAsyncFileManagerのm_syncReqFinishEvent。
          m_syncReqExistNow = true;                            // 同期でのリクエストのときは、イベントが誰にも所有されていなければ設定される。
          l_req->m_syncReqFinishEvent = m_syncReqFinishEvent;  // 同期でのリクエストを出すスレッドが1つしか存在しないなら、所有者候補は常にそのスレッドしかあり得ないので必ず設定される。
          syncReqFinishEvent = m_syncReqFinishEvent;           // 同期でのリクエストを出すスレッドが複数存在するなら、他のスレッドに所有されているときは設定されない。
        }
        else
        {                 // 同期でのリクエストを出すスレッドが複数存在していることを伝えるために、ASSERTで止めている。
          GFL_ASSERT(0);  // 無視してもいいASSERT。このASSERTに引っ掛かったリクエストは、イベントではなく「ループ＋スリープ」で終了を待つことになる。
        }                 // このASSERTに引っ掛かるものがあまりに多い場合は、複数スレッドからの同期リクエストもイベントで処理できるよう、イベントを増やすことも検討したい。
      }
    }
    {
      gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);
      m_reqList->Add(&elem);
      m_reqListExecEvent->Signal();
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
     if(matchCheckManagerInst)
     {
       (matchCheckManagerInst->*MatchCheckManagerSetAddReqFunc)(req);
     }
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
    }
    if(syncReqFinishEvent)
    {
      syncReqFinishEvent->Wait();  // リクエストの処理が終わるまで待つ。
      syncReqFinishEvent->Clear();
      {
        gfl2::thread::ScopedLock syncReqScopedLock(*m_syncReqCriticalSection);
        m_syncReqExistNow = false;  // l_reqはelemが消滅するときに一緒に消滅するので、l_req->m_syncReqFinishEventにNULL代入はしなくても問題ない
      }
    }
    u32 endlessLoopCount = 0;
    while(!( (inst->*IsReqFinishedFunc)(key) ))  // m_syncReqFinishEventで終わるのを待っていた場合は、ここのIs...Finishedは初回判定時にtrueとなる。
    {
      Calc::SleepInLoop();
      if(++endlessLoopCount >= Define::ENDLESS_LOOP_COUNT_MAX_LONG)
      {
        GFL_ASSERT(0);  // 開発中に無限ループに気付かせるためのASSERT
        break;
      }
    }
  }


private:
  const DeviceBase* m_mainDevice;

  // garcファイルの名前テーブル
  ArcFileNameTableInfo* m_arcFileNameTableInfo[ArcFileNameTableType::NUM];

  // デフォルトの対象言語
  ToolDefine::ArcLangType::Tag m_defaultArcLang;

  // リスト
  List* m_reqList;
  List* m_uncompList;
  List* m_fileList;

  // 処理中
  ElemBase* m_reqListElemNow;     // 処理中のものがなければNULL。ただのポインタで本物はm_reqListにある。
  ElemBase* m_uncompListElemNow;  // 処理中のものがなければNULL。ただのポインタで本物はm_uncompListにある。
  bool      m_syncReqExistNow;    // 処理中のものがなければfalse。

  // 排他制御
  gfl2::thread::CriticalSection* m_reqListCriticalSection;     // m_reqListの同期オブジェクト
  gfl2::thread::CriticalSection* m_uncompListCriticalSection;  // m_uncompListの同期オブジェクト
  gfl2::thread::CriticalSection* m_fileListCriticalSection;    // m_fileListの同期オブジェクト
  gfl2::thread::CriticalSection* m_syncReqCriticalSection;     // m_syncReqFinishEventの同期オブジェクト

  // 同期機構
  gfl2::thread::Event* m_reqListExecEvent;     // m_reqListを実行するイベント
  gfl2::thread::Event* m_uncompListExecEvent;  // m_uncompListを実行するイベント
  gfl2::thread::Event* m_syncReqFinishEvent;   // 同期でのリクエストの処理が終わったことを伝えるイベント
  
  // スレッド
  FileThread*  m_fileThread;
  OtherThread* m_otherThread;

#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
  gfl2::heap::HeapBase* m_callerHeap;  // リクエスト発行者の管理に使うメモリ。このメモリはきれいには使われないので専用のヒープを渡して下さい。
#endif

#if GFL_FS_DEBUG_CHECK_ARC_FILE_LOAD_INTERVAL
  gfl2::heap::HeapBase* m_intervalCheckHeap;  // ArcFileロード間隔チェックに使うメモリ。このメモリはきれいには使われないので専用のヒープを渡して下さい。
  s32                   m_intervalCheckLevel;  // ArcFileロード間隔チェックの警告レベル(IntervalCheckLevel::Tagの値)
#endif

#if defined(GF_PLATFORM_CTR)
#if GFL_FS_HIO_ENABLE
  // CTRのHIO
  const DeviceBase* m_hioDevice;
  bool m_hioUseFlag;  // 使うか否かフラグ
#endif
#endif  // GF_PLATFORM_CTR

#if GFL_FS_GASC_ENABLE
  // 「garcのソースを記録したファイル」に関する変数
  const DeviceBase* m_arcSrcDevice;
  gfl2::heap::HeapBase* m_arcSrcDefaultHeap;  // ArcSrcファイルの読み込みに使うデフォルトメモリ
  bool m_arcSrcCreateFlag;  // インスタンスやソーステーブルをつくるか否かフラグ
  bool m_arcSrcUseFlag;     // 使うか否かフラグ
#endif

#if GFL_FS_DIRECT_SDMC_ARC_FILE_ENABLE
  u32* m_directSdmcArcFileExistTable;
  bool m_directSdmcArcFileUseFlag;
#endif




  //! @name  ArcFileElem
  //@{
  
public:
  //! @brief  ArcFileオープンクローズリクエスト発行者のリストをGFL_PRINTで出力する
  //! @note  現在オープンしているArcFileとそのオープン回数、
  //!        オープンクローズリクエストを発行したファイル名と行数を知りたいときに使って下さい。
  void PrintArcFileList(void) const;

  //@}


  //! @name  FsDebugLogArcFileInfo
  //@{

public:
  class FsDebugLogArcFileInfoStep
  {
  public:
    enum Tag
    {
      FILE_ADD      ,  // 0 ユーザがリクエストを追加したとき(リクエスト開始)
      FILE_START    ,  // 1 ファイル処理(オープン、クローズ、リード、ライト)の実行を開始したとき
      FILE_FINISH   ,  // 2 ファイル処理の実行が終了したとき(展開がなければこれでリクエスト終了)
      UNCOMP_ADD    ,  // 3 展開処理のリクエストを追加したとき
      UNCOMP_START  ,  // 4 展開処理の実行を開始したとき
      UNCOMP_FINISH ,  // 5 展開処理の実行が終了したとき(展開があればこれでリクエスト終了)
      NUM
    };
  };
  class FsDebugLogArcFileInfoTerm
  {
  public:
    enum Tag
    {
      FILE_FINISH_START,         // FILE_FINISH   - FILE_START
      FILE_FINISH_ADD,           // FILE_FINISH   - FILE_ADD
      UNCOMP_FINISH_START,       // UNCOMP_FINISH - UNCOMP_START
      UNCOMP_FINISH_ADD,         // UNCOMP_FINISH - UNCOMP_ADD
      UNCOMP_FINISH_FILE_START,  // UNCOMP_FINISH - FILE_START    // 展開がないときはFILE_FINISH_STARTと同じにしておく
      UNCOMP_FINISH_FILE_ADD,    // UNCOMP_FINISH - FILE_ADD      // 展開がないときはFILE_FINISH_ADDと同じにしておく
      NUM
    };
  };
  class FsDebugLogArcFileInfoKind
  {
  public:
    enum Tag
    {
      ASYNC,  // 非同期
      SYNC,   // 同期
      ANY,    // 非同期、同期どちらか
    };
  };

public: 
  struct FsDebugLogArcFileInfo
  {
    u32                totalIndex;
    s16                type;  // ReqElemType::Tag type;
    s16                kind;  // FsDebugLogArcFileInfoKind::Tag kind;
    ArcFile::ARCID     arcId;
    ArcFile::ARCDATID  datId;
    u32                compSize;  // 圧縮時のサイズ
    u32                size;      // 展開後のサイズ(圧縮していないときはこちらに代入)
    // 時間は全て[micro second]  // 0のときはその処理がなかった  // gfl2::system::Timer::PerformanceCounter::GetMicroSecond();
    s64                time[FsDebugLogArcFileInfoStep::NUM];
    u32                diffTime[FsDebugLogArcFileInfoTerm::NUM];
  };


#if GFL_FS_DEBUG_LOG

private:
      //! @name  friendが呼び出し可能
      //@{
  void logFsDebugLogArcFileInfo(ReqElemType::Tag type, FsDebugLogArcFileInfoKind::Tag kind, ArcFile::ARCID arcId, ArcFile::ARCDATID datId, FsDebugLogArcFileInfoStep::Tag step);
  void setFsDebugLogArcFileInfoCompSize(ReqElemType::Tag type, ArcFile::ARCID arcId, ArcFile::ARCDATID datId, size_t compSize);  // kindは使わない。もし必要になったら引数に追加。
  void setFsDebugLogArcFileInfoSize(ReqElemType::Tag type, ArcFile::ARCID arcId, ArcFile::ARCDATID datId, size_t size);  // kindは使わない。もし必要になったら引数に追加。
      //@}
  u32 FS_DEBUG_LOG_ARC_FILE_INFO_ELEM_NUM_MAX;  // 元は定数でした
  u32 m_fsDebugLogArcFileInfoElemTotalNum;    // 現在までの要素の総数
  u32 m_fsDebugLogArcFileInfoArrayNextIndex;  // 次の要素を入れるインデックス
  FsDebugLogArcFileInfo* m_fsDebugLogArcFileInfoArray;  // リングバッファ(numやindexはELEM_NUM_MAXまでの値。totalNumやtotalIndexは総数。)
private:
  void createFsDebugLogArcFileInfo(gfl2::heap::HeapBase* heapForFsDebugLog);
  void destroyFsDebugLogArcFileInfo(void);
  FsDebugLogArcFileInfo* getFsDebugLogArcFileInfoElem(ReqElemType::Tag type, ArcFile::ARCID arcId, ArcFile::ARCDATID datId) const;  // kindは使わない。もし必要になったら引数に追加。
  void calcFsDebugLogArcFileInfoOne(u32 reverseIndex);

#endif  // GFL_FS_DEBUG_LOG

public:
  //! @brief  ログをGFL_PRINTで出力する(全情報を出す)
  //! @note  CalcFsDebugLogArcFileInfoは本関数の内部で呼んでいるので呼ばなくていい。
  //! @note  ログの中身はFsDebugLogArcFileInfoの情報。
  //!        どんなリクエストを出していたか、処理時間はどれくらい掛かっていたか、を確認したいときに使って下さい。
  void PrintFsDebugLogArcFileInfo(void);

  //! @brief  ログを取得する
  //! @param[in] reverseIndex  何番目のデータを取得するか指定する。
  //!                          最新データを取得するならreverseIndex=0、1つ前のデータを取得するならreverseIndex=1となる。
  //!                          ログの個数はGetFsDebugLogArcFileInfoNumで得られる。
  //! @return  範囲外を指定したときはNULLが返る。
  //!          GFL_FS_DEBUG_LOGが定義されていないときはNULLが返る。
  //! @note  CalcFsDebugLogArcFileInfoは本関数の内部で呼んでいるので呼ばなくていい。
  const FsDebugLogArcFileInfo* GetFsDebugLogArcFileInfo(u32 reverseIndex) const;

  //! @brief  ログをbufにswprintfで出力する(情報量は少ない)
  //! @param[in] buf           ログを出力するバッファ
  //! @param[in] bufSize       bufのサイズ
  //! @param[in] reverseIndex  1個分のデータしか出力しないので、何番目のデータを出力するかの指定をここで行う。
  //!                          最新データを書くならreverseIndex=0、1つ前のデータを書くならreverseIndex=1となる。
  //!                          ログの個数はGetFsDebugLogArcFileInfoNumで得られる。
  //! @note  必ずCalcFsDebugLogArcFileInfoを呼んでから呼ぶこと。
  //!        連続で何回も本関数を呼ぶ場合は、CalcFsDebugLogArcFileInfoはその前に1回だけ呼んでおけばよい。
  //! @note  ログの中身はFsDebugLogArcFileInfoの情報。
  //!        どんなリクエストを出していたか、処理時間はどれくらい掛かっていたか、を確認したいときに使って下さい。
  void PrintFsDebugLogArcFileInfoBufLight(wchar_t* buf, size_t bufSize, u32 reverseIndex);

  //! @note  必ずPrintFsDebugLogArcFileInfoBufLightを呼ぶ前に呼ぶこと。
  //!        連続で何回もPrintFsDebugLogArcFileInfoBufLightを呼ぶ場合は、本関数はその前に1回だけ呼んでおけばよい。
  void CalcFsDebugLogArcFileInfo(void);
  //! @param  ログの個数を得る
  //! @note  PrintFsDebugLogArcFileInfoBufLightでreverseIndexを指定するときは、0始まりなので「本関数の戻り値-1」までとなる。
  u32 GetFsDebugLogArcFileInfoNum(void) const;
  //! @param  ログのクリア
  void ClearFsDebugLogArcFileInfo(void);

  //@}  // FsDebugLogArcFileInfo






  //! @name  MatchCheckManager
  //@{



#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST



private:
  //! @brief  リクエストのリスト要素
  class MatchCheckReqElem : public ElemBase
  {
    GFL_FORBID_COPY_AND_ASSIGN(MatchCheckReqElem);

  public:
    //! @brief  キーのタイプ
    enum KeyType
    {
      KEY_TYPE_PP_BUF,     //!< void**         ppBuf      // =0
      KEY_TYPE_P_BUF,      //!< const void*    pBuf       // =1
      KEY_TYPE_FILE_NAME,  //!< const char*    fileName   // =2
      KEY_TYPE_ARC_ID,     //!< ArcFile::ARCID arcId      // =3
    };
    //! @brief  arcファイルの指定の仕方のタイプ
    enum ArcInfoType
    {
      ARC_INFO_TYPE_NONE,       //!< arcファイルでない         // =0
      ARC_INFO_TYPE_FILE_NAME,  //!< const char*    fileName   // =1
      ARC_INFO_TYPE_ARC_ID,     //!< ArcFile::ARCID arcId      // =2
    };
    //! @brieff  Is...Finishedの状態
    enum IsFinishedState
    {
      IS_FINISHED_STATE_NOT_CALL,  //!< Is...Finishedが呼ばれていない。                  // =0
      IS_FINISHED_STATE_FALSE,     //!< Is...Finishedが呼ばれた。戻り値はfalseだった。   // =1
      IS_FINISHED_STATE_TRUE,      //!< Is...Finishedが呼ばれた。戻り値はtrueだった。    // =2
    };

  public:
    MatchCheckReqElem(void);
    virtual ~MatchCheckReqElem();

    // このインスタンスに「引数で渡したもの」から成るリクエストを設定する
    //                                (arcInfo                , key                    )
    void SetFileEasyReadReq           (                         void**         ppBuf   );  // (req01)
    void SetFileEasyWriteReq          (                         const char*    fileName);  // (req02)
    void SetArcFileOpenReq            (                         const char*    fileName);  // (req03)
    void SetArcFileOpenReq            (                         ArcFile::ARCID arcId   );  // (req03)
    void SetArcFileCloseReq           (                         const char*    fileName);  // (req04)
    void SetArcFileCloseReq           (                         ArcFile::ARCID arcId   );  // (req04)
    void SetArcFileLoadDataReq        (const char*    fileName, void**         ppBuf   );  // (req05)
    void SetArcFileLoadDataReq        (ArcFile::ARCID arcId   , void**         ppBuf   );  // (req05)
    void SetArcFileLoadDataBufReq     (const char*    fileName, const void*    pBuf    );  // (req06)
    void SetArcFileLoadDataBufReq     (ArcFile::ARCID arcId   , const void*    pBuf    );  // (req06)
    void SetArcFileLoadDataPieceBufReq(const char*    fileName, const void*    pBuf    );  // (req07)
    void SetArcFileLoadDataPieceBufReq(ArcFile::ARCID arcId   , const void*    pBuf    );  // (req07)
    void SetArcFileSeekDataPtrReq     (                         const char*    fileName);  // (req08)
    void SetArcFileSeekDataPtrReq     (                         ArcFile::ARCID arcId   );  // (req08)
    void SetArcFileLoadDataPtrBufReq  (const char*    fileName, const void*    pBuf    );  // (req09)
    void SetArcFileLoadDataPtrBufReq  (ArcFile::ARCID arcId   , const void*    pBuf    );  // (req09)

    // 「引数で渡したもの」が「このインスタンスが記憶しているもの」と同じリクエストだったときtrueを返す
    //                                     (key                    )
    bool IsSameAsFileEasyReadReq           (void**         ppBuf   ) const;  // (req01)
    bool IsSameAsFileEasyWriteReq          (const char*    fileName) const;  // (req02)
    bool IsSameAsArcFileOpenReq            (const char*    fileName) const;  // (req03)
    bool IsSameAsArcFileOpenReq            (ArcFile::ARCID arcId   ) const;  // (req03)
    bool IsSameAsArcFileCloseReq           (const char*    fileName) const;  // (req04)
    bool IsSameAsArcFileCloseReq           (ArcFile::ARCID arcId   ) const;  // (req04)
    bool IsSameAsArcFileLoadDataReq        (void**         ppBuf   ) const;  // (req05)
    bool IsSameAsArcFileLoadDataBufReq     (const void*    pBuf    ) const;  // (req06)
    bool IsSameAsArcFileLoadDataPieceBufReq(const void*    pBuf    ) const;  // (req07)
    bool IsSameAsArcFileSeekDataPtrReq     (const char*    fileName) const;  // (req08)
    bool IsSameAsArcFileSeekDataPtrReq     (ArcFile::ARCID arcId   ) const;  // (req08)
    bool IsSameAsArcFileLoadDataPtrBufReq  (const void*    pBuf    ) const;  // (req09)

    //! @name  MatchCheckManagerクラスの template<typename T_KEY> void checkRemainingReqElemTemplate(T_KEY key) const メンバ関数から呼び出す関数。使い方が特殊です。
    //@{
    // このインスタンスが記憶しているArcInfoと同じときtrueを返す。
    bool IsSameArcInfo(bool           dummy   ) const;  // 必ずtrueを返したいとき。
    bool IsSameArcInfo(const char*    fileName) const;  // ArcFileでFILE_NAMEが同じか否かの確認をしたいとき。
    bool IsSameArcInfo(ArcFile::ARCID arcId   ) const;  // ArcFileでARC_IDが同じか否かの確認をしたいとき。

    // 引数で指定したのと同じArcFileをAsyncFileManagerから得て、リファレンスカウンタを取得する。
    u32 GetArcFileCount(bool           dummy   , const AsyncFileManager* asyncFileManager) const;  // 必ず0を返したいとき。
    u32 GetArcFileCount(const char*    fileName, const AsyncFileManager* asyncFileManager) const;
    u32 GetArcFileCount(ArcFile::ARCID arcId   , const AsyncFileManager* asyncFileManager) const;

    // タイプがReqElemType::ARC_FILE_CLOSE_REQであるときtrueを返す。
    bool IsReqTypeArcFileClose(bool           dummy   ) const;  // 必ずfalseを返したいとき。
    bool IsReqTypeArcFileClose(const char*    fileName) const;
    bool IsReqTypeArcFileClose(ArcFile::ARCID arcId   ) const;
    //bool IsReqTypeArcFileClose(void) const;  // どのArcFileに対するクローズリクエストであるかの判別をしない場合。ARC_FILE_CLOSE_REQでありさえすればtrueを返す。
    //@}

    // Is...Finishedの戻り値を設定する
    void SetIsFinished(bool isFinished);

    // m_isArcFileClosedを設定する
    void SetArcFileClosedFlag(bool isClosed) { m_isArcFileClosed = isClosed; }
    // m_isArcFileClosedを取得する
    bool GetArcFileClosedFlag(void) const { return (m_isArcFileClosed!=0); }

    // このインスタンスが記憶しているものの情報をmesに出力する
    void GetInfoMes(char* mes, const u32 mesBufSize);

  private:
    void initKey(void**         ppBuf   );
    void initKey(const void*    pBuf    );
    void initKey(const char*    fileName);
    void initKey(ArcFile::ARCID arcId   );
    void initArcInfo(void                   );
    void initArcInfo(const char*    fileName);
    void initArcInfo(ArcFile::ARCID arcId   );

    bool isSameKey(void**         ppBuf   ) const;
    bool isSameKey(const void*    pBuf    ) const;
    bool isSameKey(const char*    fileName) const;
    bool isSameKey(ArcFile::ARCID arcId   ) const;

  private:
    union KeyUnion
    {
      void**         ppBuf;
      const void*    pBuf; 
      const char*    fileName;
      ArcFile::ARCID arcId;
    };
    union ArcInfoUnion
    {
      const char*    fileName;
      ArcFile::ARCID arcId;
    };

  private:
    KeyUnion          m_key;
    ArcInfoUnion      m_arcInfo;          // m_arcInfoTypeがNONEでないとき有効。

    u32               m_reqType         : 4;  // ReqElemType::Tag                                                                                             // 0-9  4ビット
    u32               m_keyType         : 2;  // KeyType                                                                                                      // 0-3  2ビット      
    u32               m_arcInfoType     : 2;  // ArcInfoType       // m_keyとm_arcInfoに同じ情報が入るときもある。m_keyと同じだからと言ってNONEにはしない。   // 0-2  2ビット  
    u32               m_isFinishedState : 2;  // IsFinishedState   // 対応するIs...Finishedの状態。                                                           // 0-2  2ビット
    u32               m_isArcFileClosed : 1;  // m_reqTypeがARC_FILE_CLOSE_REQのときのみ有効。クローズ済みのとき1。まだクローズされていないとき0。            // 0-1  1ビット
    u32               m_dummy           :21;
  };



  //! @brief  対応確認のマネージャクラス
  class MatchCheckManager
  {
    GFL_FORBID_COPY_AND_ASSIGN(MatchCheckManager);

  public:
    enum AnnounceLevel
    {
      ANNOUNCE_LEVEL_PRINT,
      ANNOUNCE_LEVEL_ASSERT,
    };

  public:
    //! @param[in] heap  MatchCheckReqElemの確保に使うヒープ。確保/破棄を繰り返すので専用のヒープを渡して下さい。
    //! @param[in] asyncFileManager  この「MatchCheckManagerのインスタンス」が所属する「AsyncFileManagerのインスタンス」を渡して下さい。
    MatchCheckManager(gfl2::heap::HeapBase* heap, const AsyncFileManager* asyncFileManager);
    virtual ~MatchCheckManager();

    // Add...Reqが呼ばれたことをセットする。Addされたリクエストをリストに覚えておく。
    void SetAddFileEasyReadReq           (const FileEasyReadReq&            req);  // (req01)
    void SetAddFileEasyWriteReq          (const FileEasyWriteReq&           req);  // (req02)
    void SetAddArcFileOpenReq            (const ArcFileOpenReq&             req);  // (req03)
    void SetAddArcFileCloseReq           (const ArcFileCloseReq&            req);  // (req04)
    void SetAddArcFileLoadDataReq        (const ArcFileLoadDataReq&         req);  // (req05)
    void SetAddArcFileLoadDataBufReq     (const ArcFileLoadDataBufReq&      req);  // (req06)
    void SetAddArcFileLoadDataPieceBufReq(const ArcFileLoadDataPieceBufReq& req);  // (req07)
    void SetAddArcFileSeekDataPtrReq     (const ArcFileSeekDataPtrReq&      req);  // (req08)
    void SetAddArcFileLoadDataPtrBufReq  (const ArcFileLoadDataPtrBufReq&   req);  // (req09)

    // Is...Finishedが呼ばれたことをセットする。対応するAddされたリクエストにisFinishedを設定する。isFinishedがtrueのとき対応するリクエストを消す。
    // 対応するリクエストが存在しないときfalseを返す、存在するときtrueを返す。
    // 存在しない理由  (1) Is...Finishedの書き間違い。  (2) isFinishedがtrueのときに既に呼んでおりそのときにリクエストを消した。
    //                                       (Is...Finishedの戻り値, key                    )
    bool SetIsFileEasyReadFinished           (bool isFinished      , void**         ppBuf   );  // (req01)
    bool SetIsFileEasyWriteFinished          (bool isFinished      , const char*    fileName);  // (req02)
    bool SetIsArcFileOpenFinished            (bool isFinished      , const char*    fileName);  // (req03)
    bool SetIsArcFileOpenFinished            (bool isFinished      , ArcFile::ARCID arcId   );  // (req03)
    bool SetIsArcFileCloseFinished           (bool isFinished      , const char*    fileName);  // (req04)
    bool SetIsArcFileCloseFinished           (bool isFinished      , ArcFile::ARCID arcId   );  // (req04)
    bool SetIsArcFileLoadDataFinished        (bool isFinished      , void**         ppBuf   );  // (req05)
    bool SetIsArcFileLoadDataBufFinished     (bool isFinished      , const void*    pBuf    );  // (req06)
    bool SetIsArcFileLoadDataPieceBufFinished(bool isFinished      , const void*    pBuf    );  // (req07)
    bool SetIsArcFileSeekDataPtrFinished     (bool isFinished      , const char*    fileName);  // (req08)
    bool SetIsArcFileSeekDataPtrFinished     (bool isFinished      , ArcFile::ARCID arcId   );  // (req08)
    bool SetIsArcFileLoadDataPtrBufFinished  (bool isFinished      , const void*    pBuf    );  // (req09)
    bool SetIsAllReqFinished                 (bool isFinished                               );

    // m_isArcFileClosedを立てる
    void SetArcFileClosedFlagOn(const char*    fileName);
    void SetArcFileClosedFlagOn(ArcFile::ARCID arcId   );

    // アナウンス方法
    void SetAnnounceLevel(AnnounceLevel announceLevel);
    AnnounceLevel GetAnnounceLevel(void) const; 

    //! @brief  対応確認を行うか否か(対応確認の記録を行うか否か)
    void SetCheckFlag(bool checkFlag);  // trueのとき対応確認を行う(対応確認の記録を行う)。途中で対応確認のON/OFFを切り替えると、対応していないものが続出するでしょうから注意。
    bool GetCheckFlag(void) const; 

    //! @brief  対応確認の記録を消去する
    void ClearRemainingReqElem(void);
    //! @brief  対応確認の結果、Finishが呼ばれないまま残っているものを出力する
    void CheckRemainingReqElem(void) const;
    //! @brief  リクエストの個数を出力する
    void OutputReqNum(void) const;

  private:
    // Addされたリクエストを記憶するためのリスト要素を作成する
    MatchCheckReqElem* createMatchCheckReqElem(void);

    // 不備があったときのアナウンス
    void announce(const char* mes) const;

  private:
    // 対応するFinishが呼ばれず残っているリクエストを出力する。
    void checkRemainingReqElem(void) const;
    void checkRemainingReqElem(const char*    fileName) const;
    void checkRemainingReqElem(ArcFile::ARCID arcId   ) const;
    
    // AddArcFileOpenReqを行った時点での「対応するFinishが呼ばれず残っているArcFileCloseReqリクエスト」を出力する。m_isArcFileClosedをチェックしている。
    void checkRemainingArcFileCloseReqElemAtAddArcFileOpenReq(const ArcFileOpenReq& req) const;

    // Addされたリクエストを破棄する
    void destroyMatchCheckReqElem(MatchCheckReqElem* elem);
    // Addされたリクエストを破棄する(Is...Finished時に呼ばれる)。checkRemainingReqElemFlagがtrueのとき、対応するFinishが呼ばれず残っているリクエストを出力する。
    // checkOnlyArcInfoFlagはcheckRemainingReqElemFlagがtrueのときだけ使用する。checkOnlyArcInfoFlagがtrueのとき、残っているリクエストのうちで出力するものをArcInfoで限定する。
    void destroyMatchCheckReqElemAtFinish(MatchCheckReqElem* elem, bool checkRemainingReqElemFlag, bool checkOnlyArcInfoFlag, void**         ppBuf   );
    void destroyMatchCheckReqElemAtFinish(MatchCheckReqElem* elem, bool checkRemainingReqElemFlag, bool checkOnlyArcInfoFlag, const void*    pBuf    );
    void destroyMatchCheckReqElemAtFinish(MatchCheckReqElem* elem, bool checkRemainingReqElemFlag, bool checkOnlyArcInfoFlag, const char*    fileName);
    void destroyMatchCheckReqElemAtFinish(MatchCheckReqElem* elem, bool checkRemainingReqElemFlag, bool checkOnlyArcInfoFlag, ArcFile::ARCID arcId   );

    // 対応するFinishが呼ばれず残っているリクエストを出力する。
    // onlyPrintForcedlyFlagがtrueのとき、ANNOUNCE_LEVEL_ASSERTでもASSERTにせず、PRINTしかしない。
    template<typename T_KEY>
    void checkRemainingReqElemTemplate(T_KEY key, bool onlyPrintForcedlyFlag=false) const
    {
      static const u32 MES_BUF_SIZE = 1024;
  
      bool isRemaining = false;
  
      // 1個1個ASSERTに引っ掛かるのは困るので、this->announceは使わずPRINTにする。
      //GFL_PRINT("FsMatchCheck: 対応するFinishが呼ばれていないリクエスト --------\n");  // 空でも文章が表示されるとユーザが混乱するのでコメントアウト。
  
      ElemBase* elem = m_matchCheckReqList->GetHead();
      while(elem)
      {
        MatchCheckReqElem* reqElem = static_cast<MatchCheckReqElem*>(elem);
        if( reqElem->IsSameArcInfo(key) )
        {
          ////////////////////////////////////////////////////////////////////////////
          //////// keyで示すArcFileのとき、もしくは、keyがboolのとき、ここに来る。 
          ////////////////////////////////////////////////////////////////////////////
          // [ArcFileの複数回オープンについての解説]
          //   ArcFileの場合は同じkeyのArcFileを複数回開いていることもある。
          if( reqElem->GetArcFileCount(key, m_asyncFileManager) == 0 )
          {
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //////// keyで示すArcFileの参照カウンタが0のとき(即ち、ArcFileインスタンスがナイとき)、もしくは、keyがboolのとき、ここに来る。
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // [ArcFileの複数回オープンについての解説]
            //   ArcFileをまだ開いている状態( > 0 )なら　残っているリクエストは別のところが残しているんだと判断して
            //   リクエストが残っていても問題ナシとする。
            //   ArcFileをもう開いていない状態( == 0 )なら　リクエストが残っているときは問題とする。
            if( !(reqElem->IsReqTypeArcFileClose(key)) )
            {
              ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
              //////// keyで示すArcFileへのリクエストがクローズリクエストでないとき、もしくは、keyがboolのとき、ここに来る。
              ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
              // [ArcFileの複数回オープンについての解説]
              //   ArcFileのクローズリクエストのときにここに来ると、ArcFileをもう開いていない状態( == 0 )でも
              //   「別のところが残しているArcFileのクローズリクエスト」が残っている。
              //   これを回避することはできないので、ArcFileのクローズリクエストは残っていても問題ナシとする。
              isRemaining = true; 
              char mes[MES_BUF_SIZE];                                       
              reqElem->GetInfoMes(mes, MES_BUF_SIZE);
              GFL_RELEASE_PRINT(mes);
            }
          }
        }
        elem = elem->GetNext();
      }
      
      //GFL_PRINT("FsMatchCheck: --------------------------------\n");  // 空でも文章が表示されるとユーザが混乱するのでコメントアウト。
   
      // 最後に、ASSERTにする必要があるならASSERTにする。
      if(isRemaining)
      {
        GFL_RELEASE_PRINT("FsMatchCheck: ^^^^ 上記が「対応するFinishが呼ばれていないリクエスト」 ^^^^\n");
        this->OutputReqNum();
        
        if( this->GetAnnounceLevel() == ANNOUNCE_LEVEL_ASSERT )
        {
          if(!onlyPrintForcedlyFlag)
          {
            GFL_ASSERT(0);
          }
        }
      }
    }

    // 対応するリクエストが存在しないときfalseを返す、存在するときtrueを返す。
    template<typename T_KEY>
    bool setIsFinishedTemplate(
        bool isFinished,
        T_KEY key,
        bool (MatchCheckReqElem::*IsSameAsReqFunc)(T_KEY) const,
        bool checkRemainingReqElemFlag,
        bool checkOnlyArcInfoFlag,  // checkRemainingReqElemFlagがtrueのときだけ使用する
        MatchCheckManager* inst,
        void (MatchCheckManager::*atFinishFunc)(MatchCheckReqElem*,bool,bool,T_KEY)
    )
    {
      bool ret = false;
      ElemBase* elem = m_matchCheckReqList->GetTail();  // 後ろから探す
      while(elem)
      {
        MatchCheckReqElem* reqElem = static_cast<MatchCheckReqElem*>(elem);
        if( (reqElem->*IsSameAsReqFunc)(key) )
        {
          reqElem->SetIsFinished(isFinished);
          if(isFinished)
          {
            (inst->*atFinishFunc)(reqElem, checkRemainingReqElemFlag, checkOnlyArcInfoFlag, key);
          }
          ret = true;
          break;
        }
        else
        {
          elem = elem->GetPrev();
        }
      }
      return ret;
    }

  private:
    static const u32 REQ_MEM_MANAGE_ARRAY_SIZE;  // u32の配列(要素数がREQ_MEM_MANAGE_ARRAY_SIZE)でリクエストに使うメモリを管理する。
    static const u32 REQ_NUM_MAX;                // リクエストの個数の最大。

  private:
    bool                    m_checkFlag;          // trueのとき対応確認を行う(対応確認の記録を行う)。
    AnnounceLevel           m_announceLevel;      // 不備があったときのアナウンス方法。
    gfl2::heap::HeapBase*   m_heap;               // MatchCheckReqElemの確保に使うヒープ。確保/破棄を繰り返すので専用のヒープを渡して下さい。
    const AsyncFileManager* m_asyncFileManager;   // この「MatchCheckManagerのインスタンス」が所属する「AsyncFileManagerのインスタンス」を渡して下さい。
    List*                   m_matchCheckReqList;  // Addされたリクエストのリスト。対応するFinishが呼ばれそれがtrueを返しているとき、リストからそのAddが消される。
    u32*                    m_reqMemBitManage;    // リクエストに使うメモリの使用状況をビットで管理する。u32 m_reqMemBitManage[REQ_MEM_MANAGE_ARRAY_SIZE]; となるようにメモリを確保する。
    void*                   m_reqMemTopAddr;      // リクエストに使うメモリの先頭アドレス。サイズ sizeof(MatchCheckReqElem) * REQ_NUM_MAX でメモリを確保する。
    s16                     m_currReqNum;         // 現在のリクエストの個数
    s16                     m_reqNumBiggest;      // リクエストの個数のこれまでの最大個数
  };



private:
  void destroyMatchCheckSystem(void);  // 生成はCreateMatchCheckSystem

private:
  MatchCheckManager*  m_matchCheckManager;  // 対応確認のマネージャ



#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST



public:
  //! @brief  「リクエスト発行」と「リクエスト終了待ち」が対応しているかを確認するシステムを生成する
  //! @param[in] heap  MatchCheckReqElemの確保に使うヒープ。確保/破棄を繰り返すので専用のヒープを渡して下さい。
  void CreateMatchCheckSystem(gfl2::heap::HeapBase* heap);  // 破棄はCreateMatchCheckSystem

  //! @brief  対応確認で不備があったときのアナウンスをASSERTにする
  //! @param[in] setAssert  trueのときASSERT、falseのときPRINT
  void SetMatchCheckAnnounceAssert(bool setAssert);
  //! @brief  対応確認で不備があったときのアナウンスがASSERTか
  //! @return  trueのときASSERT、falseのときPRINT
  bool IsMatchCheckAnnounceAssert(void) const;

  //! @brief  対応確認を行うか否か(対応確認の記録を行うか否か)
  //! @param[in] checkFlag  trueのとき対応確認を行う(対応確認の記録を行う)。途中で対応確認のON/OFFを切り替えると、対応していないものが続出するでしょうから注意。
  void SetMatchCheckFlag(bool checkFlag);
  //! @brief  対応確認を行っているか(対応確認の記録を行っているか)
  //! @return  trueのとき対応確認を行っている(対応確認の記録を行っている)
  bool GetMatchCheckFlag(void) const;

  //! @brief  対応確認の記録を消去する
  void ClearMatchCheckRemainingReq(void);
  //! @brief  対応確認の結果、Finishが呼ばれないまま残っているものを出力する
  void OutputMatchCheckRemainingReq(void) const;


#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  //! @name  friendが呼び出し可能
  //@{
private:
  //! @brief  m_isArcFileClosedを立てる
  void setMatchCheckArcFileClosedFlagOn(const char*    fileName);
  void setMatchCheckArcFileClosedFlagOn(ArcFile::ARCID arcId   );
  //@}
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST


  //@}  // MatchCheckManager






};


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL2_FSASYNCFILEMANAGER_H_INCLUDED

