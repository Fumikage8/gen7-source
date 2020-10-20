//==============================================================================
/**
 @file    gfl2_FsAsyncFileManager.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   非同期ファイル読み込み書き込みマネージャ
 */
//==============================================================================


/*

「どのファイルを読みに行くか」解説

Win32版
    通常時・・・
        ・普通のファイルはmainDeviceから読み込む。
        ・ArcファイルもmainDeviceからArcFileNameTableType::MAINに書かれているgarcファイルを読み込む。
    ArcSrc時・・・
        ・普通のファイルはArcSrcは関係ありません。
        ・Arcファイルについては以下の通り。
              ・gascファイルについてはarcSrcDeviceからArcFileNameTableType::ARCSRCに書かれているgarcファイルを読み込む。
                関係するフラグはGetArcSrcCreateFlagです。
              ・「garcの元になったファイル」は、gascに書かれているファイルをPCから読み込む(PcUtilを使っておりdevice指定していない)。
                関係するフラグはGetArcSrcUseFlagだけです。GetArcSrcCreateFlagは関係ないですが、gascを読み込んでいなかったら「garcの元になったファイル」は読み込めません。
    注意・・・
        ・Win32版ではHIOは関係ありません。

CTR版
    通常時・・・
        ・普通のファイルはmainDeviceから読み込む。
        ・ArcファイルもmainDeviceからArcFileNameTableType::MAINに書かれているgarcファイルを読み込む。
    HIO時・・・
        ・普通のファイルはhioDeviceから読み込む。
          関係するフラグはGetHioUseFlagです。
        ・ArcファイルもhioDeviceからArcFileNameTableType::HIOに書かれているgarcファイルを読み込む。
          関係するフラグはGetHioUseFlagです。
    ArcSrc時・・・
        ・普通のファイルはArcSrcは関係ありません。
        ・Arcファイルについては以下の通り。
              ・gascファイルについてはarcSrcDeviceからArcFileNameTableType::ARCSRCに書かれているgarcファイルを読み込む。
                関係するフラグはGetArcSrcCreateFlagです。GetHioUseFlagは関係ないです。
              ・「garcの元になったファイル」は、gascに書かれているファイルをPCから読み込む(PcUtilを使っておりdevice指定していない)。
                関係するフラグはGetArcSrcUseFlagだけです。GetArcSrcCreateFlagは関係ないですが、gascを読み込んでいなかったら「garcの元になったファイル」は読み込めません。
                GetHioUseFlagは関係ないので、garcファイルをHIOを使わずromから読み込み、「garcの元になったファイル」をHIOを使ってPCから読み込む、ということも可能です。
    注意・・・
        ・arcSrcDeviceもHIOを使っていますが、ArcFileNameTableType::HIOやhioDeviceやGetHioUseFlagとは関係ありません。
          ArcFileNameTableType::HIOをArcFileNameTableType::REMOTE、hioDeviceをremoteDevice、GetHioUseFlagをGetRemoteUseFlagと改名し、
          「remoteもarcSrcもHIOを使っています！」としたほうが、混乱なく理解がし易いかも。
          この解説はそもそも「GetHioUseFlagがArcSrcにも関係しているのではないか」という疑問を解消するためのものでした。

以上。

*/

/*

用語

  Req            ... 「①ReqElemのメンバになるもの」という意味と「②ユーザが出したリクエスト」という意味がある。①のときはm_reqListだけでなくm_uncompListにも使う。②のときはm_reqListにしか使わない。
  ReqElem        ... ReqElem型。m_reqListだけでなくm_uncompListにも使う。
  ReqListElem    ... ReqElemをm_reqListの要素にしたもの。ユーザが出したリクエストのリストの要素。
  UncompListElem ... ReqElemをm_uncompListの要素にしたもの。解凍のリストの要素。UncompElemというものはない。

  FileElem ... 「[A]FileElem型」という意味と「[B]FileElemをm_fileListの要素にしたもの。ファイルのリストの要素。FileListElemという呼び方はしていない。m_reqListとm_uncompListのように複数リストがあるわけではないので。」という意味がある。




gfl2::thread::ScopedLockを使っているもの

  下記を取得するもの、下記のメンバ変数に変更を加えるもので使っている。
    ・m_reqList
    ・m_uncompList
    ・m_reqListElemNow
    ・m_uncompListElemNow
    ・m_syncReqExistNow

  具体的には、下記のメンバ関数で使っている。
    Add系(public)
    ・Add...Req関数
    ・Add...Uncomp関数

    Cancel系(public)
    ・Cancel...Req関数

    Is*Finished系(public)
    ・Is...Finished関数

    Sync系(public)
    ・addReqIsFinishedTemplateテンプレートのAdd部分(IsFinished部分は「Is...Finished関数」をテンプレート内から呼んでいる)

    デストラクタから呼ぶdelete系(private)
    ・deleteAllReqListElemAtDestructor
    ・deleteAllUncompListElemAtDestructor

    他スレッドで処理対象となるもの系(firendが呼び出すprivate)
    ・GetReqListHeadElemAndSetReqListElemNow
    ・DeleteReqListElemAndUnsetReqListElemNow
    ・GetUncompListHeadElemAndSetUncompListElemNow
    ・DeleteUncompListElemAndUnsetUncompListElemNow




gfl2::thread::ScopedLockを使わなければならないように思われるが、使わなかったもの


  下記を取得するもの、下記のメンバ変数に変更を加えるものについては、
  ScopedLockを使わなければならないように思われるが、使わなかった。
    ・m_fileList

  使わなかった理由
    ・m_fileListに変更を加えるのはm_fileThreadだけである。メインスレッドは変更を加えない。
      同時に複数のスレッドが変更を加えるということはないので、ScopedLockを使わなかった。
    ・m_fileListを取得するものはGetArcFileとPrintArcFileListだけである。
        - 「GetArcFileで得たconstポインタから値を取得している最中」にArcFileを破棄するということは、
          ポインタだけ保持しておいて別の場所で実体を破棄しているようなものなので、
          そのような使い方はユーザの誤りである。
          破棄シーケンスの最中にポインタを使わなければ問題は起きないので、
          ユーザが正しく使ってくれることを願う。
          防ごうにも、ポインタをユーザが持ってしまうので、防ぎようがないものでもある。
        - PrintArcFileListを呼んでいる最中にArcFileを破棄するということは、
          破棄シーケンスの最中にでもPrintArcFileListを呼ばない限りは起きないので、
          ユーザに気を付けて使ってもらえば問題ない。
          PrintArcFileListはGFL_DEBUGで括られているので、そんなに気にしていない。
    ・m_fileListを取得するものとしてGetArcFileCountもある。
      これはユーザが気を付けてどうにかなるものではない。
      しかしGetArcFileCountはGFL_DEBUGで括られているので、あまり気にしていない。
      GetArcFileCountの使い方を考慮した書き方をして、なるべく問題が起きないようにしているが、
      完璧ではないので、ArcFileを破棄した後にアクセスしてしまうこともあるかもしれない。
      これに関してはScopedLockを使ったほうがいいかもしれない、と言わざるを得ない。
        →「→心配だったのでm_fileListCriticalSectionを追加した。」における対応で
          問題が起こらないようにしました。


  →心配だったのでm_fileListCriticalSectionを追加した。
      下記のメンバ関数で使っている。
        Add系(m_fileThreadが呼んでいる)
        ・AddFile
        ・AddArcFile
        Delete系(m_fileThreadが呼んでいる)
        ・DeleteFileElem
        ・DeleteArcFileElem
        デストラクタから呼ぶdelete系(private)
        ・deleteAllFileElemAndContentAtDestructor
        ユーザが呼ぶ系(public)
        ・GetArcFile
        ・PrintArcFileList
        勝手に呼ばれる系(MatchCheckReqElemが呼んでいる)
        ・GetArcFileCount

      下記のメンバ関数では使っていない。
        Get系(上記のメンバ関数内から呼んでいるし、m_fileThreadからも呼んでいる)
        ・GetFileElem
        ・GetArcFileElem

      これで問題が起こらないと言える理由は下記の通り。
        ・m_fileListに対して追加削除を行うのはm_fileThreadだけなので、
          m_fileThread内でGetFileElemやGetArcFileElemで取得したものを使っているときは
          それらがなくなることは起こり得ない。
          (deleteAllFileElemAndContentAtDestructorでも削除を行うが、
           これが呼ばれるときはm_fileThreadが終了しているので問題ない。)
        ・問題が起こる唯一の場合は、
          GetArcFileで得たポインタをユーザが使っている最中にそれが削除されるときである。
          これは「実体を破棄した後に、保持していたポインタでアクセスする」ということをやっているのと同じことなので、
          普段からユーザがそれに気を付けているのと同様、これについてもユーザに気を付けてもらうことにする。
          ユーザを信じて何も対策は講じない。
        ・将来、複数のスレッドがm_fileListに対して追加削除を行うような改造をしたら、この対応のままでは問題が起こる。
          この対応で問題がないのは、
          m_fileThread１スレッドしか「追加削除、GetFileElemやGetArcFileElemで取得したものを使う」ということを行っていない
          ことが理由であるから。

*/


// gfl2のインクルード
#include <thread/include/gfl2_thread.h>
#include <thread/include/gfl2_threadstatic.h>
#include <thread/include/gfl2_Event.h>
#include <system/include/Timer/gfl2_PerformanceCounter.h>
#include <util/include/gfl2_std_string.h>

// fsのインクルード
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <fs/include/gfl2_FsSystem.h>
#include <fs/include/internal/gfl2_FsFile.h>
#include "internal/gfl2_FsDebug.h"
#include "gfl2_FsDeviceBase.h"
#include "internal/gfl2_FsCalc.h"
#include "internal/gfl2_FsList.h"
#include "internal/gfl2_FsFileThread.h"
#include "internal/gfl2_FsOtherThread.h"
 

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


AsyncFileManager::AsyncFileManager(gfl2::heap::HeapBase* heap, const DeviceBase* mainDevice, const DeviceBase* hioDevice, const DeviceBase* arcSrcDevice, int fileThreadPrio, int otherThreadPrio, gfl2::heap::HeapBase* heapForFsDebugLog, u32 fsDebugLogArcFileInfoElemNumMax)
  : m_mainDevice(mainDevice),
    m_defaultArcLang(ToolDefine::ArcLangType::JPN),
    m_reqList(NULL),
    m_uncompList(NULL),
    m_fileList(NULL),
    m_reqListElemNow(NULL),
    m_uncompListElemNow(NULL),
    m_syncReqExistNow(false),
    m_reqListCriticalSection(NULL),
    m_uncompListCriticalSection(NULL),
    m_fileListCriticalSection(NULL),
    m_syncReqCriticalSection(NULL),
    m_reqListExecEvent(NULL),
    m_uncompListExecEvent(NULL),
    m_syncReqFinishEvent(NULL),
    m_fileThread(NULL),
    m_otherThread(NULL)
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
    , m_callerHeap(NULL)
#endif
#if GFL_FS_DEBUG_CHECK_ARC_FILE_LOAD_INTERVAL
    , m_intervalCheckHeap(NULL)
    , m_intervalCheckLevel(IntervalCheckLevel::LEVEL_WARNING)
#endif
#if   defined(GF_PLATFORM_CTR)
#if GFL_FS_HIO_ENABLE
    , m_hioDevice(hioDevice)
    , m_hioUseFlag(false)
#endif
#endif  // GF_PLATFORM_CTR
#if GFL_FS_GASC_ENABLE
    , m_arcSrcDevice(arcSrcDevice)
    , m_arcSrcDefaultHeap(NULL)
    , m_arcSrcCreateFlag(false)
    , m_arcSrcUseFlag(false)
#endif
#if GFL_FS_DIRECT_SDMC_ARC_FILE_ENABLE
    , m_directSdmcArcFileExistTable(NULL)
    , m_directSdmcArcFileUseFlag(false)
#endif
#if GFL_FS_DEBUG_LOG
    , FS_DEBUG_LOG_ARC_FILE_INFO_ELEM_NUM_MAX(fsDebugLogArcFileInfoElemNumMax)
    , m_fsDebugLogArcFileInfoElemTotalNum(0)
    , m_fsDebugLogArcFileInfoArrayNextIndex(0)
    , m_fsDebugLogArcFileInfoArray(NULL)
#endif
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
    , m_matchCheckManager(NULL)
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
{
  for(s32 i=0; i<ArcFileNameTableType::NUM; ++i)
  {
    m_arcFileNameTableInfo[i] = NULL;
  }

  m_reqList    = GFL_NEW(heap) List();
  m_uncompList = GFL_NEW(heap) List();
  m_fileList   = GFL_NEW(heap) List();
 
  m_reqListCriticalSection    = GFL_NEW(heap) gfl2::thread::CriticalSection();
  m_uncompListCriticalSection = GFL_NEW(heap) gfl2::thread::CriticalSection();
  m_fileListCriticalSection   = GFL_NEW(heap) gfl2::thread::CriticalSection();
  m_syncReqCriticalSection    = GFL_NEW(heap) gfl2::thread::CriticalSection();
  m_reqListCriticalSection->Initialize();
  m_uncompListCriticalSection->Initialize();
  m_fileListCriticalSection->Initialize();
  m_syncReqCriticalSection->Initialize();

  m_reqListExecEvent = GFL_NEW(heap) gfl2::thread::Event(heap);
  m_uncompListExecEvent = GFL_NEW(heap) gfl2::thread::Event(heap);
  m_syncReqFinishEvent = GFL_NEW(heap) gfl2::thread::Event(heap);
  m_reqListExecEvent->Initialize();
  m_uncompListExecEvent->Initialize();
  m_syncReqFinishEvent->Initialize();

  m_fileThread  = GFL_NEW(heap) FileThread(heap, this, fileThreadPrio);
  m_otherThread = GFL_NEW(heap) OtherThread(heap, this, otherThreadPrio);

#if GFL_FS_DEBUG_LOG
  createFsDebugLogArcFileInfo(heapForFsDebugLog);
#endif
}

AsyncFileManager::~AsyncFileManager()
{
  // スレッドを終了させる
  {
    this->CancelAllReq();
    this->RequestEnd();
    u32 endlessLoopCount = 0;
    while(!this->HasEnded())
    {
      Calc::SleepInLoop();
 
      if(++endlessLoopCount >= Define::ENDLESS_LOOP_COUNT_MAX_LONG)
      {
        GFL_ASSERT(0);  // 開発中に無限ループに気付かせるためのASSERT
        break;
      }
    }
  }

  // リスト要素の後始末
  this->deleteAllReqListElemAtDestructor();
  this->deleteAllUncompListElemAtDestructor();
  this->deleteAllFileElemAndContentAtDestructor();

  // メモリ破棄
#if GFL_FS_DEBUG_LOG
  destroyFsDebugLogArcFileInfo();
#endif

#if GFL_FS_DIRECT_SDMC_ARC_FILE_ENABLE
  GFL_SAFE_DELETE_ARRAY(m_directSdmcArcFileExistTable);
#endif

  GFL_SAFE_DELETE(m_otherThread);
  GFL_SAFE_DELETE(m_fileThread);

  m_syncReqFinishEvent->Finalize();
  m_uncompListExecEvent->Finalize();
  m_reqListExecEvent->Finalize();
  GFL_SAFE_DELETE(m_syncReqFinishEvent);
  GFL_SAFE_DELETE(m_uncompListExecEvent);
  GFL_SAFE_DELETE(m_reqListExecEvent);

  GFL_SAFE_DELETE(m_syncReqCriticalSection);
  GFL_SAFE_DELETE(m_fileListCriticalSection);
  GFL_SAFE_DELETE(m_uncompListCriticalSection);
  GFL_SAFE_DELETE(m_reqListCriticalSection);

  GFL_SAFE_DELETE(m_fileList);
  GFL_SAFE_DELETE(m_uncompList);
  GFL_SAFE_DELETE(m_reqList);

  for(s32 i=0; i<ArcFileNameTableType::NUM; ++i)
  {
    if(m_arcFileNameTableInfo[i]) GFL_SAFE_DELETE(m_arcFileNameTableInfo[i]);
  }

#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  this->destroyMatchCheckSystem();
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
}


void AsyncFileManager::AddFileEasyReadReq(const FileEasyReadReq& req)
{
  GFL_FSDEBUG_PRINT_SEQ_FUNC();
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);
  /*ElemBase* elemBeforeAdd = m_reqList->GetHead();*/  // Signalを呼び過ぎてもいいつくりにスレッド側をしておいたので、elemBeforeAddの部分は消しておく。他のAdd関数内からも同様に消しておく。
  addReqTemplate<FileEasyReadReq, FileEasyReadReqElem>(m_reqList, req, req.heapForReq, REQ_PRIO_OFFSET_ASYNC);
  /*if(elemBeforeAdd==NULL)*/ m_reqListExecEvent->Signal();
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  if(m_matchCheckManager) m_matchCheckManager->SetAddFileEasyReadReq(req);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
}
void AsyncFileManager::AddFileEasyWriteReq(const FileEasyWriteReq& req)
{
  GFL_FSDEBUG_PRINT_SEQ_FUNC();
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);
  addReqTemplate<FileEasyWriteReq, FileEasyWriteReqElem>(m_reqList, req, req.heapForReq, REQ_PRIO_OFFSET_ASYNC);
  m_reqListExecEvent->Signal();
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  if(m_matchCheckManager) m_matchCheckManager->SetAddFileEasyWriteReq(req);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
}

#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
void AsyncFileManager::__AddArcFileOpenReq(const ArcFileOpenReq& req, const char* callerFileName, u32 callerLineNo)
{
  // gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection); はaddArcFileOpenReqで呼んでいる。
  ArcFileOpenReq l_req = req;
  if(l_req.memorizeCaller)
  {
    l_req.m_callerInfo = this->createCallerInfo(callerFileName, callerLineNo);  // この時点ではm_callerInfoの所有権はreqが持っている。
  }
  this->addArcFileOpenReq(l_req);
}
#else
void AsyncFileManager::AddArcFileOpenReq(const ArcFileOpenReq& req)
{
  // gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection); はaddArcFileOpenReqで呼んでいる。
  this->addArcFileOpenReq(req);
}
#endif
void AsyncFileManager::addArcFileOpenReq(const ArcFileOpenReq& req)
{
  GFL_FSDEBUG_PRINT_SEQ_FUNC();
  ArcFileOpenReq l_req = req;
  l_req.lang = this->GetLangOfArcFile(l_req.lang);  // スレッドで実際の処理が始まるまでの間に、デフォルトの対象言語が変更されるといけないので、ここでデフォルト以外の値に確定させておく。
#if GFL_FS_DEBUG_LOG
  this->logFsDebugLogArcFileInfo(ReqElemType::ARC_FILE_OPEN_REQ, FsDebugLogArcFileInfoKind::ASYNC, l_req.arcId, ArcFile::ARCDATID_NULL, FsDebugLogArcFileInfoStep::FILE_ADD);
#endif
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);
  addReqTemplate<ArcFileOpenReq, ArcFileOpenReqElem>(m_reqList, l_req, l_req.heapForReq, REQ_PRIO_OFFSET_ASYNC);
  m_reqListExecEvent->Signal();
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  if(m_matchCheckManager) m_matchCheckManager->SetAddArcFileOpenReq(l_req);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
}

#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
void AsyncFileManager::__AddArcFileCloseReq(const ArcFileCloseReq& req, const char* callerFileName, u32 callerLineNo)
{
  // gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection); はaddArcFileCloseReqで呼んでいる。
  ArcFileCloseReq l_req = req;
  if(l_req.memorizeCaller)
  {
    l_req.m_callerInfo = this->createCallerInfo(callerFileName, callerLineNo);  // この時点ではm_callerInfoの所有権はreqが持っている。
  }
  this->addArcFileCloseReq(l_req);
}
#else
void AsyncFileManager::AddArcFileCloseReq(const ArcFileCloseReq& req)
{
  // gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection); はaddArcFileCloseReqで呼んでいる。
  this->addArcFileCloseReq(req);
}
#endif
void AsyncFileManager::addArcFileCloseReq(const ArcFileCloseReq& req)
{
  GFL_FSDEBUG_PRINT_SEQ_FUNC();
#if GFL_FS_DEBUG_LOG
  this->logFsDebugLogArcFileInfo(ReqElemType::ARC_FILE_CLOSE_REQ, FsDebugLogArcFileInfoKind::ASYNC, req.arcId, ArcFile::ARCDATID_NULL, FsDebugLogArcFileInfoStep::FILE_ADD);
#endif
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);
  addReqTemplate<ArcFileCloseReq, ArcFileCloseReqElem>(m_reqList, req, req.heapForReq, REQ_PRIO_OFFSET_ASYNC);
  m_reqListExecEvent->Signal();
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  if(m_matchCheckManager) m_matchCheckManager->SetAddArcFileCloseReq(req);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
}

void AsyncFileManager::AddArcFileLoadDataReq(const ArcFileLoadDataReq& req)
{
  GFL_FSDEBUG_PRINT_SEQ_FUNC();
  ArcFileLoadDataReq l_req = req;
  l_req.lang = this->GetLangOfArcFile(l_req.lang);  // スレッドで実際の処理が始まるまでの間に、デフォルトの対象言語が変更されるといけないので、ここでデフォルト以外の値に確定させておく。
#if GFL_FS_DEBUG_LOG
  this->logFsDebugLogArcFileInfo(ReqElemType::ARC_FILE_LOAD_DATA_REQ, FsDebugLogArcFileInfoKind::ASYNC, l_req.arcId, l_req.datId, FsDebugLogArcFileInfoStep::FILE_ADD);
#endif
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);
  addReqTemplate<ArcFileLoadDataReq, ArcFileLoadDataReqElem>(m_reqList, l_req, l_req.heapForReq, REQ_PRIO_OFFSET_ASYNC);
  m_reqListExecEvent->Signal();
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  if(m_matchCheckManager) m_matchCheckManager->SetAddArcFileLoadDataReq(l_req);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
}
void AsyncFileManager::AddArcFileLoadDataBufReq(const ArcFileLoadDataBufReq& req)
{
  GFL_FSDEBUG_PRINT_SEQ_FUNC();
  ArcFileLoadDataBufReq l_req = req;
  l_req.lang = this->GetLangOfArcFile(l_req.lang);  // スレッドで実際の処理が始まるまでの間に、デフォルトの対象言語が変更されるといけないので、ここでデフォルト以外の値に確定させておく。
#if GFL_FS_DEBUG_LOG
  this->logFsDebugLogArcFileInfo(ReqElemType::ARC_FILE_LOAD_DATA_BUF_REQ, FsDebugLogArcFileInfoKind::ASYNC, l_req.arcId, l_req.datId, FsDebugLogArcFileInfoStep::FILE_ADD);
#endif
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);
  addReqTemplate<ArcFileLoadDataBufReq, ArcFileLoadDataBufReqElem>(m_reqList, l_req, l_req.heapForReq, REQ_PRIO_OFFSET_ASYNC);
  m_reqListExecEvent->Signal();
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  if(m_matchCheckManager) m_matchCheckManager->SetAddArcFileLoadDataBufReq(req);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
}
void AsyncFileManager::AddArcFileLoadDataPieceBufReq(const ArcFileLoadDataPieceBufReq& req)
{
  GFL_FSDEBUG_PRINT_SEQ_FUNC();
  ArcFileLoadDataPieceBufReq l_req = req;
  l_req.lang = this->GetLangOfArcFile(l_req.lang);  // スレッドで実際の処理が始まるまでの間に、デフォルトの対象言語が変更されるといけないので、ここでデフォルト以外の値に確定させておく。
#if GFL_FS_DEBUG_LOG
  this->logFsDebugLogArcFileInfo(ReqElemType::ARC_FILE_LOAD_DATA_PIECE_BUF_REQ, FsDebugLogArcFileInfoKind::ASYNC, l_req.arcId, l_req.datId, FsDebugLogArcFileInfoStep::FILE_ADD);
#endif
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);
  addReqTemplate<ArcFileLoadDataPieceBufReq, ArcFileLoadDataPieceBufReqElem>(m_reqList, l_req, l_req.heapForReq, REQ_PRIO_OFFSET_ASYNC);
  m_reqListExecEvent->Signal();
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  if(m_matchCheckManager) m_matchCheckManager->SetAddArcFileLoadDataPieceBufReq(req);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
}
/*
void AsyncFileManager::AddArcFileSeekDataPtrReq(const ArcFileSeekDataPtrReq& req)
{
  GFL_FSDEBUG_PRINT_SEQ_FUNC();
  ArcFileSeekDataPtrReq l_req = req;
  l_req.lang = this->GetLangOfArcFile(l_req.lang);  // スレッドで実際の処理が始まるまでの間に、デフォルトの対象言語が変更されるといけないので、ここでデフォルト以外の値に確定させておく。
#if GFL_FS_DEBUG_LOG
  this->logFsDebugLogArcFileInfo(ReqElemType::ARC_FILE_SEEK_DATA_PTR_REQ, FsDebugLogArcFileInfoKind::ASYNC, l_req.arcId, l_req.datId, FsDebugLogArcFileInfoStep::FILE_ADD);
#endif
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);
  addReqTemplate<ArcFileSeekDataPtrReq, ArcFileSeekDataPtrReqElem>(m_reqList, l_req, l_req.heapForReq, REQ_PRIO_OFFSET_ASYNC);
  m_reqListExecEvent->Signal();
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  if(m_matchCheckManager) m_matchCheckManager->SetAddArcFileSeekDataPtrReq(l_req);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
}
void AsyncFileManager::AddArcFileLoadDataPtrBufReq(const ArcFileLoadDataPtrBufReq& req)
{
  GFL_FSDEBUG_PRINT_SEQ_FUNC();
#if GFL_FS_DEBUG_LOG
  this->logFsDebugLogArcFileInfo(ReqElemType::ARC_FILE_LOAD_DATA_PTR_BUF_REQ, FsDebugLogArcFileInfoKind::ASYNC, req.arcId, ArcFile::ARCDATID_NULL, FsDebugLogArcFileInfoStep::FILE_ADD);
#endif
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);
  addReqTemplate<ArcFileLoadDataPtrBufReq, ArcFileLoadDataPtrBufReqElem>(m_reqList, req, req.heapForReq, REQ_PRIO_OFFSET_ASYNC);
  m_reqListExecEvent->Signal();
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  if(m_matchCheckManager) m_matchCheckManager->SetAddArcFileLoadDataPtrBufReq(req);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
}
*/

bool AsyncFileManager::CancelFileEasyReadReq           (void**         ppBuf   )
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);
  gfl2::thread::ScopedLock uncompListScopedLock(*m_uncompListCriticalSection);

  bool ret = false;

  // m_reqList
  FileEasyReadReqElem* reqElem = this->getFileEasyReadReqListElem(ppBuf);
  if(reqElem) 
  {
    if(reqElem != m_reqListElemNow)  // リクエストの処理を開始するのを待っているとき
    {
      this->deleteReqListElem(reqElem);
      ret = true;
    }
  }
  // m_uncompList
  else
  {
    reqElem = this->getFileEasyReadUncompListElem(ppBuf);
    if(reqElem)
    {
      if(reqElem != m_uncompListElemNow)  // リクエストの処理を開始するのを待っているとき
      {
        // 展開前に確保していたものを破棄する
        this->deleteFileEasyReadReqObjBeforeUncomp(reqElem->GetReq());
        this->deleteUncompListElem(reqElem);
        ret = true;
      }
    }
  }

  return ret;
}
bool AsyncFileManager::CancelFileEasyWriteReq          (const char*    fileName)
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);

  ElemBase* reqElem    = this->getFileEasyWriteReqListElem(fileName);
  return this->cancelReqOnly(reqElem);
}
bool AsyncFileManager::CancelArcFileOpenReq            (const char*    fileName)
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);
  
  ElemBase* reqElem    = this->getArcFileOpenReqListElem(fileName);
  return this->cancelReqOnly(reqElem);
}
bool AsyncFileManager::CancelArcFileOpenReq            (ArcFile::ARCID arcId   )
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);
  
  ElemBase* reqElem    = this->getArcFileOpenReqListElem(arcId);
  return this->cancelReqOnly(reqElem);
}
bool AsyncFileManager::CancelArcFileCloseReq           (const char*    fileName)
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);
  
  ElemBase* reqElem    = this->getArcFileCloseReqListElem(fileName);
  return this->cancelReqOnly(reqElem);
}
bool AsyncFileManager::CancelArcFileCloseReq           (ArcFile::ARCID arcId   )
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);
  
  ElemBase* reqElem    = this->getArcFileCloseReqListElem(arcId);
  return this->cancelReqOnly(reqElem);
}
bool AsyncFileManager::CancelArcFileLoadDataReq        (void**         ppBuf   )
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);
  gfl2::thread::ScopedLock uncompListScopedLock(*m_uncompListCriticalSection);

  bool ret = false;

  // m_reqList
  ArcFileLoadDataReqElem* reqElem = this->getArcFileLoadDataReqListElem(ppBuf);
  if(reqElem) 
  {
    if(reqElem != m_reqListElemNow)  // リクエストの処理を開始するのを待っているとき
    {
      this->deleteReqListElem(reqElem);
      ret = true;
    }
  }
  // m_uncompList
  else
  {
    reqElem = this->getArcFileLoadDataUncompListElem(ppBuf);
    if(reqElem)
    {
      if(reqElem != m_uncompListElemNow)  // リクエストの処理を開始するのを待っているとき
      {
        // 展開前に確保していたものを破棄する
        this->deleteArcFileLoadDataReqObjBeforeUncomp(reqElem->GetReq());
        this->deleteUncompListElem(reqElem);
        ret = true;
      }
    }
  }

  return ret;
}
bool AsyncFileManager::CancelArcFileLoadDataBufReq     (const void*    pBuf    )
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);
  gfl2::thread::ScopedLock uncompListScopedLock(*m_uncompListCriticalSection);

  bool ret = false;

  // m_reqList
  ArcFileLoadDataBufReqElem* reqElem = this->getArcFileLoadDataBufReqListElem(pBuf);
  if(reqElem) 
  {
    if(reqElem != m_reqListElemNow)  // リクエストの処理を開始するのを待っているとき
    {
      this->deleteReqListElem(reqElem);
      ret = true;
    }
  }
  // m_uncompList
  else
  {
    reqElem = this->getArcFileLoadDataBufUncompListElem(pBuf);
    if(reqElem)
    {
      if(reqElem != m_uncompListElemNow)  // リクエストの処理を開始するのを待っているとき
      {
        // 展開前に確保していたものを破棄する
        this->deleteArcFileLoadDataBufReqObjBeforeUncomp(reqElem->GetReq());
        this->deleteUncompListElem(reqElem);
        ret = true;
      }
    }
  }

  return ret;
}
bool AsyncFileManager::CancelArcFileLoadDataPieceBufReq(const void*    pBuf    )
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);

  ElemBase* reqElem    = this->getArcFileLoadDataPieceBufReqListElem(pBuf);
  return this->cancelReqOnly(reqElem);
}
/*
bool AsyncFileManager::CancelArcFileSeekDataPtrReq     (const char*    fileName)
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);
  
  ElemBase* reqElem    = this->getArcFileSeekDataPtrReqListElem(fileName);
  return this->cancelReqOnly(reqElem);
}
bool AsyncFileManager::CancelArcFileSeekDataPtrReq     (ArcFile::ARCID arcId   )
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);
  
  ElemBase* reqElem    = this->getArcFileSeekDataPtrReqListElem(arcId);
  return this->cancelReqOnly(reqElem);
}
bool AsyncFileManager::CancelArcFileLoadDataPtrBufReq     (const void*    pBuf    )
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);
  
  ElemBase* reqElem    = this->getArcFileLoadDataPtrBufReqListElem(pBuf);
  return this->cancelReqOnly(reqElem);
}
*/

bool AsyncFileManager::CancelAllReq(void)
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);
  gfl2::thread::ScopedLock uncompListScopedLock(*m_uncompListCriticalSection);

  bool ret = false;

  // m_reqList
  // 先頭
  ElemBase* reqElem = m_reqList->GetHead();
  if(reqElem)
  {
    if(reqElem != m_reqListElemNow)  // リクエストの処理を開始するのを待っているとき
    {
      ElemBase* delElem = reqElem;
      reqElem = reqElem->GetNext();
      this->deleteReqListElem(delElem);
      ret = true;  // 戻り値は先頭で決まる。
    }
    else
    {
      ret = false;  // 他のリストでtrueにしているかもしれないのでfalseを代入。
    }

    // 2番目以降
    while(reqElem)
    {
      ElemBase* delElem = reqElem;
      reqElem = reqElem->GetNext();
      this->deleteReqListElem(delElem);
    }
  }

  // m_uncompList
  // 先頭
  reqElem = m_uncompList->GetHead();
  if(reqElem)
  {
    if(reqElem != m_uncompListElemNow)  // リクエストの処理を開始するのを待っているとき
    {
      ElemBase* delElem = reqElem;
      reqElem = reqElem->GetNext();
      // 展開前に確保していたものを破棄する
      switch(delElem->GetElemType())
      {
      case ReqElemType::FILE_EASY_READ_REQ:          this->deleteFileEasyReadReqObjBeforeUncomp      ( (static_cast<FileEasyReadReqElem*>      (delElem))->GetReq() );  break;
      case ReqElemType::ARC_FILE_LOAD_DATA_REQ:      this->deleteArcFileLoadDataReqObjBeforeUncomp   ( (static_cast<ArcFileLoadDataReqElem*>   (delElem))->GetReq() );  break;
      case ReqElemType::ARC_FILE_LOAD_DATA_BUF_REQ:  this->deleteArcFileLoadDataBufReqObjBeforeUncomp( (static_cast<ArcFileLoadDataBufReqElem*>(delElem))->GetReq() );  break;
      }
      this->deleteUncompListElem(delElem);
      ret = true;  // 戻り値は先頭で決まる。
    }
    else
    {
      ret = false;  // 他のリストでtrueにしているかもしれないのでfalseを代入。
    }

    // 2番目以降
    while(reqElem)
    {
      ElemBase* delElem = reqElem;
      reqElem = reqElem->GetNext();
      // 展開前に確保していたものを破棄する
      switch(delElem->GetElemType())
      {
      case ReqElemType::FILE_EASY_READ_REQ:          this->deleteFileEasyReadReqObjBeforeUncomp      ( (static_cast<FileEasyReadReqElem*>      (delElem))->GetReq() );  break;
      case ReqElemType::ARC_FILE_LOAD_DATA_REQ:      this->deleteArcFileLoadDataReqObjBeforeUncomp   ( (static_cast<ArcFileLoadDataReqElem*>   (delElem))->GetReq() );  break;
      case ReqElemType::ARC_FILE_LOAD_DATA_BUF_REQ:  this->deleteArcFileLoadDataBufReqObjBeforeUncomp( (static_cast<ArcFileLoadDataBufReqElem*>(delElem))->GetReq() );  break;
      }
      this->deleteUncompListElem(delElem);
    }
  }

  return ret;
}

bool AsyncFileManager::IsFileEasyReadFinished(void** ppBuf) const
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);
  gfl2::thread::ScopedLock uncompListScopedLock(*m_uncompListCriticalSection);

  bool ret = ( (getFileEasyReadReqListElem(ppBuf) == NULL) && (getFileEasyReadUncompListElem(ppBuf) == NULL) );
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  if(m_matchCheckManager) m_matchCheckManager->SetIsFileEasyReadFinished(ret, ppBuf);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  return ret;
}
bool AsyncFileManager::IsFileEasyWriteFinished(const char* fileName) const
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);

  bool ret = (getFileEasyWriteReqListElem(fileName) == NULL);
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  if(m_matchCheckManager) m_matchCheckManager->SetIsFileEasyWriteFinished(ret, fileName);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  return ret;
}
bool AsyncFileManager::IsArcFileOpenFinished(const char* fileName) const
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);

  bool ret = (getArcFileOpenReqListElem(fileName) == NULL);
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  if(m_matchCheckManager) m_matchCheckManager->SetIsArcFileOpenFinished(ret, fileName);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  return ret;
}
bool AsyncFileManager::IsArcFileOpenFinished(ArcFile::ARCID arcId) const
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);

  bool ret = (getArcFileOpenReqListElem(arcId) == NULL);
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  if(m_matchCheckManager) m_matchCheckManager->SetIsArcFileOpenFinished(ret, arcId);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  return ret;
}
bool AsyncFileManager::IsArcFileCloseFinished(const char* fileName) const
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);

  bool ret = (getArcFileCloseReqListElem(fileName) == NULL);
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  if(m_matchCheckManager) m_matchCheckManager->SetIsArcFileCloseFinished(ret, fileName);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  return ret;
}
bool AsyncFileManager::IsArcFileCloseFinished(ArcFile::ARCID arcId) const
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);

  bool ret = (getArcFileCloseReqListElem(arcId) == NULL);
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  if(m_matchCheckManager) m_matchCheckManager->SetIsArcFileCloseFinished(ret, arcId);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  return ret;
}
bool AsyncFileManager::IsArcFileLoadDataFinished(void** ppBuf) const
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);
  gfl2::thread::ScopedLock uncompListScopedLock(*m_uncompListCriticalSection);

  bool ret = ( (getArcFileLoadDataReqListElem(ppBuf) == NULL) && (getArcFileLoadDataUncompListElem(ppBuf) == NULL) );
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  if(m_matchCheckManager) m_matchCheckManager->SetIsArcFileLoadDataFinished(ret, ppBuf);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  return ret;
}
bool AsyncFileManager::IsArcFileLoadDataBufFinished(const void* pBuf) const
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);
  gfl2::thread::ScopedLock uncompListScopedLock(*m_uncompListCriticalSection);

  bool ret = ( (getArcFileLoadDataBufReqListElem(pBuf) == NULL) && (getArcFileLoadDataBufUncompListElem(pBuf) == NULL) );
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  if(m_matchCheckManager) m_matchCheckManager->SetIsArcFileLoadDataBufFinished(ret, pBuf);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  return ret;
}
bool AsyncFileManager::IsArcFileLoadDataPieceBufFinished(const void* pBuf) const
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);

  bool ret = (getArcFileLoadDataPieceBufReqListElem(pBuf) == NULL);
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  if(m_matchCheckManager) m_matchCheckManager->SetIsArcFileLoadDataPieceBufFinished(ret, pBuf);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  return ret;
}
/*
bool AsyncFileManager::IsArcFileSeekDataPtrFinished(const char* fileName) const
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);

  bool ret = (getArcFileSeekDataPtrReqListElem(fileName) == NULL);
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  if(m_matchCheckManager) m_matchCheckManager->SetIsArcFileSeekDataPtrFinished(ret, fileName);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  return ret;
}
bool AsyncFileManager::IsArcFileSeekDataPtrFinished(ArcFile::ARCID arcId) const
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);

  bool ret = (getArcFileSeekDataPtrReqListElem(arcId) == NULL);
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  if(m_matchCheckManager) m_matchCheckManager->SetIsArcFileSeekDataPtrFinished(ret, arcId);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  return ret;
}
bool AsyncFileManager::IsArcFileLoadDataPtrBufFinished(const void* pBuf) const
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);

  bool ret = (getArcFileLoadDataPtrBufReqListElem(pBuf) == NULL);
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  if(m_matchCheckManager) m_matchCheckManager->SetIsArcFileLoadDataPtrBufFinished(ret, pBuf);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  return ret;
}
*/

bool AsyncFileManager::IsAllReqFinished(void) const
{
  //gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);        // isAllReqFinishedでScopedLockしている
  //gfl2::thread::ScopedLock uncompListScopedLock(*m_uncompListCriticalSection);  // isAllReqFinishedでScopedLockしている

  bool ret = this->isAllReqFinished();
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  if(m_matchCheckManager) m_matchCheckManager->SetIsAllReqFinished(ret);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  return ret;
}
bool AsyncFileManager::isAllReqFinished(void) const
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);
  gfl2::thread::ScopedLock uncompListScopedLock(*m_uncompListCriticalSection);

  bool ret = ( (m_reqList->GetHead() == NULL) && (m_uncompList->GetHead() == NULL) );
  return ret;
}


void AsyncFileManager::SyncFileEasyRead      (const FileEasyReadReq&       req)
{
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  addReqIsFinishedTemplate<FileEasyReadReq, FileEasyReadReqElem, void**>(req, req.ppBuf, this, &AsyncFileManager::IsFileEasyReadFinished, m_matchCheckManager, &MatchCheckManager::SetAddFileEasyReadReq);
#else  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  addReqIsFinishedTemplate<FileEasyReadReq, FileEasyReadReqElem, void**>(req, req.ppBuf, this, &AsyncFileManager::IsFileEasyReadFinished);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
}
void AsyncFileManager::SyncFileEasyWrite     (const FileEasyWriteReq&      req)
{
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  addReqIsFinishedTemplate<FileEasyWriteReq, FileEasyWriteReqElem, const char*>(req, req.fileName, this, &AsyncFileManager::IsFileEasyWriteFinished, m_matchCheckManager, &MatchCheckManager::SetAddFileEasyWriteReq);
#else  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  addReqIsFinishedTemplate<FileEasyWriteReq, FileEasyWriteReqElem, const char*>(req, req.fileName, this, &AsyncFileManager::IsFileEasyWriteFinished);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
}

#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
void AsyncFileManager::__SyncArcFileOpen     (const ArcFileOpenReq&        req, const char* callerFileName, u32 callerLineNo)
{
  ArcFileOpenReq l_req = req;
  if(l_req.memorizeCaller)
  {
    l_req.m_callerInfo = this->createCallerInfo(callerFileName, callerLineNo);  // この時点ではm_callerInfoの所有権はreqが持っている。
  }
  this->syncArcFileOpen(l_req);
}
#else
void AsyncFileManager::SyncArcFileOpen       (const ArcFileOpenReq&        req)
{
  this->syncArcFileOpen(req);
}
#endif
void AsyncFileManager::syncArcFileOpen       (const ArcFileOpenReq&        req)
{
  ArcFileOpenReq l_req = req;
  l_req.lang = this->GetLangOfArcFile(l_req.lang);  // スレッドで実際の処理が始まるまでの間に、デフォルトの対象言語が変更されるといけないので、ここでデフォルト以外の値に確定させておく。
#if GFL_FS_DEBUG_LOG
  this->logFsDebugLogArcFileInfo(ReqElemType::ARC_FILE_OPEN_REQ, FsDebugLogArcFileInfoKind::SYNC, l_req.arcId, ArcFile::ARCDATID_NULL, FsDebugLogArcFileInfoStep::FILE_ADD);
#endif
  if(l_req.arcId != ArcFile::ARCID_NULL)
  {
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
    addReqIsFinishedTemplate<ArcFileOpenReq, ArcFileOpenReqElem, ArcFile::ARCID>(l_req, l_req.arcId, this, &AsyncFileManager::IsArcFileOpenFinished, m_matchCheckManager, &MatchCheckManager::SetAddArcFileOpenReq);
#else  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
    addReqIsFinishedTemplate<ArcFileOpenReq, ArcFileOpenReqElem, ArcFile::ARCID>(l_req, l_req.arcId, this, &AsyncFileManager::IsArcFileOpenFinished);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  }
  else
  {
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
    addReqIsFinishedTemplate<ArcFileOpenReq, ArcFileOpenReqElem, const char*>(l_req, l_req.fileName, this, &AsyncFileManager::IsArcFileOpenFinished, m_matchCheckManager, &MatchCheckManager::SetAddArcFileOpenReq);
#else  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
    addReqIsFinishedTemplate<ArcFileOpenReq, ArcFileOpenReqElem, const char*>(l_req, l_req.fileName, this, &AsyncFileManager::IsArcFileOpenFinished);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  }
}

#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
void AsyncFileManager::__SyncArcFileClose    (const ArcFileCloseReq&       req, const char* callerFileName, u32 callerLineNo)
{
  ArcFileCloseReq l_req = req;
  if(l_req.memorizeCaller)
  {
    l_req.m_callerInfo = this->createCallerInfo(callerFileName, callerLineNo);  // この時点ではm_callerInfoの所有権はreqが持っている。
  }
  this->syncArcFileClose(l_req);
}
#else
void AsyncFileManager::SyncArcFileClose      (const ArcFileCloseReq&       req)
{
  this->syncArcFileClose(req);
}
#endif
void AsyncFileManager::syncArcFileClose      (const ArcFileCloseReq&       req)
{
#if GFL_FS_DEBUG_LOG
  this->logFsDebugLogArcFileInfo(ReqElemType::ARC_FILE_CLOSE_REQ, FsDebugLogArcFileInfoKind::SYNC, req.arcId, ArcFile::ARCDATID_NULL, FsDebugLogArcFileInfoStep::FILE_ADD);
#endif
  if(req.arcId != ArcFile::ARCID_NULL)
  {
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
    addReqIsFinishedTemplate<ArcFileCloseReq, ArcFileCloseReqElem, ArcFile::ARCID>(req, req.arcId, this, &AsyncFileManager::IsArcFileCloseFinished, m_matchCheckManager, &MatchCheckManager::SetAddArcFileCloseReq);
#else  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
    addReqIsFinishedTemplate<ArcFileCloseReq, ArcFileCloseReqElem, ArcFile::ARCID>(req, req.arcId, this, &AsyncFileManager::IsArcFileCloseFinished);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  }
  else
  {
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
    addReqIsFinishedTemplate<ArcFileCloseReq, ArcFileCloseReqElem, const char*>(req, req.fileName, this, &AsyncFileManager::IsArcFileCloseFinished, m_matchCheckManager, &MatchCheckManager::SetAddArcFileCloseReq);
#else  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
    addReqIsFinishedTemplate<ArcFileCloseReq, ArcFileCloseReqElem, const char*>(req, req.fileName, this, &AsyncFileManager::IsArcFileCloseFinished);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  }
}

void AsyncFileManager::SyncArcFileLoadData   (const ArcFileLoadDataReq&    req)
{
  ArcFileLoadDataReq l_req = req;
  l_req.lang = this->GetLangOfArcFile(l_req.lang);  // スレッドで実際の処理が始まるまでの間に、デフォルトの対象言語が変更されるといけないので、ここでデフォルト以外の値に確定させておく。
#if GFL_FS_DEBUG_LOG
  this->logFsDebugLogArcFileInfo(ReqElemType::ARC_FILE_LOAD_DATA_REQ, FsDebugLogArcFileInfoKind::SYNC, l_req.arcId, l_req.datId, FsDebugLogArcFileInfoStep::FILE_ADD);
#endif
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  addReqIsFinishedTemplate<ArcFileLoadDataReq, ArcFileLoadDataReqElem, void**>(l_req, l_req.ppBuf, this, &AsyncFileManager::IsArcFileLoadDataFinished, m_matchCheckManager, &MatchCheckManager::SetAddArcFileLoadDataReq);
#else  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  addReqIsFinishedTemplate<ArcFileLoadDataReq, ArcFileLoadDataReqElem, void**>(l_req, l_req.ppBuf, this, &AsyncFileManager::IsArcFileLoadDataFinished);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
}
void AsyncFileManager::SyncArcFileLoadDataBuf(const ArcFileLoadDataBufReq& req)
{
  ArcFileLoadDataBufReq l_req = req;
  l_req.lang = this->GetLangOfArcFile(l_req.lang);  // スレッドで実際の処理が始まるまでの間に、デフォルトの対象言語が変更されるといけないので、ここでデフォルト以外の値に確定させておく。
#if GFL_FS_DEBUG_LOG
  this->logFsDebugLogArcFileInfo(ReqElemType::ARC_FILE_LOAD_DATA_BUF_REQ, FsDebugLogArcFileInfoKind::SYNC, l_req.arcId, l_req.datId, FsDebugLogArcFileInfoStep::FILE_ADD);
#endif
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  addReqIsFinishedTemplate<ArcFileLoadDataBufReq, ArcFileLoadDataBufReqElem, const void*>(l_req, l_req.pBuf, this, &AsyncFileManager::IsArcFileLoadDataBufFinished, m_matchCheckManager, &MatchCheckManager::SetAddArcFileLoadDataBufReq);
#else  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  addReqIsFinishedTemplate<ArcFileLoadDataBufReq, ArcFileLoadDataBufReqElem, const void*>(l_req, l_req.pBuf, this, &AsyncFileManager::IsArcFileLoadDataBufFinished);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
}
void AsyncFileManager::SyncArcFileLoadDataPieceBuf(const ArcFileLoadDataPieceBufReq& req)
{
  ArcFileLoadDataPieceBufReq l_req = req;
  l_req.lang = this->GetLangOfArcFile(l_req.lang);  // スレッドで実際の処理が始まるまでの間に、デフォルトの対象言語が変更されるといけないので、ここでデフォルト以外の値に確定させておく。
#if GFL_FS_DEBUG_LOG
  this->logFsDebugLogArcFileInfo(ReqElemType::ARC_FILE_LOAD_DATA_PIECE_BUF_REQ, FsDebugLogArcFileInfoKind::SYNC, l_req.arcId, l_req.datId, FsDebugLogArcFileInfoStep::FILE_ADD);
#endif
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  addReqIsFinishedTemplate<ArcFileLoadDataPieceBufReq, ArcFileLoadDataPieceBufReqElem, const void*>(l_req, l_req.pBuf, this, &AsyncFileManager::IsArcFileLoadDataPieceBufFinished, m_matchCheckManager, &MatchCheckManager::SetAddArcFileLoadDataPieceBufReq);
#else  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  addReqIsFinishedTemplate<ArcFileLoadDataPieceBufReq, ArcFileLoadDataPieceBufReqElem, const void*>(l_req, l_req.pBuf, this, &AsyncFileManager::IsArcFileLoadDataPieceBufFinished);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
}
/*
void AsyncFileManager::SyncArcFileSeekDataPtr     (const ArcFileSeekDataPtrReq&      req)
{
  ArcFileSeekDataPtrReq l_req = req;
  l_req.lang = this->GetLangOfArcFile(l_req.lang);  // スレッドで実際の処理が始まるまでの間に、デフォルトの対象言語が変更されるといけないので、ここでデフォルト以外の値に確定させておく。
#if GFL_FS_DEBUG_LOG
  this->logFsDebugLogArcFileInfo(ReqElemType::ARC_FILE_SEEK_DATA_PTR_REQ, FsDebugLogArcFileInfoKind::SYNC, l_req.arcId, l_req.datId, FsDebugLogArcFileInfoStep::FILE_ADD);
#endif
  if(l_req.arcId != ArcFile::ARCID_NULL)
  {
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
    addReqIsFinishedTemplate<ArcFileSeekDataPtrReq, ArcFileSeekDataPtrReqElem, ArcFile::ARCID>(l_req, l_req.arcId, this, &AsyncFileManager::IsArcFileSeekDataPtrFinished, m_matchCheckManager, &MatchCheckManager::SetAddArcFileSeekDataPtrReq);
#else  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
    addReqIsFinishedTemplate<ArcFileSeekDataPtrReq, ArcFileSeekDataPtrReqElem, ArcFile::ARCID>(l_req, l_req.arcId, this, &AsyncFileManager::IsArcFileSeekDataPtrFinished);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  }
  else
  {
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
    addReqIsFinishedTemplate<ArcFileSeekDataPtrReq, ArcFileSeekDataPtrReqElem, const char*>(l_req, l_req.fileName, this, &AsyncFileManager::IsArcFileSeekDataPtrFinished, m_matchCheckManager, &MatchCheckManager::SetAddArcFileSeekDataPtrReq);
#else  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
    addReqIsFinishedTemplate<ArcFileSeekDataPtrReq, ArcFileSeekDataPtrReqElem, const char*>(l_req, l_req.fileName, this, &AsyncFileManager::IsArcFileSeekDataPtrFinished);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  }
}
void AsyncFileManager::SyncArcFileLoadDataPtrBuf  (const ArcFileLoadDataPtrBufReq&   req)
{
#if GFL_FS_DEBUG_LOG
  this->logFsDebugLogArcFileInfo(ReqElemType::ARC_FILE_LOAD_DATA_PTR_BUF_REQ, FsDebugLogArcFileInfoKind::SYNC, req.arcId, ArcFile::ARCDATID_NULL, FsDebugLogArcFileInfoStep::FILE_ADD);
#endif
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  addReqIsFinishedTemplate<ArcFileLoadDataPtrBufReq, ArcFileLoadDataPtrBufReqElem, const void*>(req, req.pBuf, this, &AsyncFileManager::IsArcFileLoadDataPtrBufFinished, m_matchCheckManager, &MatchCheckManager::SetAddArcFileLoadDataPtrBufReq);
#else  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  addReqIsFinishedTemplate<ArcFileLoadDataPtrBufReq, ArcFileLoadDataPtrBufReqElem, const void*>(req, req.pBuf, this, &AsyncFileManager::IsArcFileLoadDataPtrBufFinished);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
}
*/


void AsyncFileManager::RequestEnd(void)
{
  m_fileThread->RequestEnd();
  m_reqListExecEvent->Signal();
  
  m_otherThread->RequestEnd();
  m_uncompListExecEvent->Signal();
}
bool AsyncFileManager::HasEnded(void) const
{
  return ( m_fileThread->HasEnded() || m_otherThread->HasEnded() );
}
bool AsyncFileManager::IsThreadRunning(void) const
{
  // isAllReqFinishedでScopedLockしている
  bool ret = this->isAllReqFinished();
  return !ret;
}


Result AsyncFileManager::SetArcFileNameTable(ArcFileNameTableType::Tag type, gfl2::heap::HeapBase* heap, const char** arcFileNameTable, size_t arcFileNum, u32 arcFileIgnoreNameLen, const char* arcFilePrefixName)
{
  Result res;

  // 既に設定済みか否か確認する
  if(m_arcFileNameTableInfo[type])
  {
    // もし付け直しできるようにするならスレッドの排他制御をすること。
    res |= Result::Bit::ARC_FILE_NAME_TABLE_ALREADY_EXIST;
    GFL_ASSERT(0);  // 開発中に気付いてもらうためのASSERT
    GFL_SAFE_DELETE(m_arcFileNameTableInfo[type]);
  }

  // 設定する
  m_arcFileNameTableInfo[type] = GFL_NEW(heap) ArcFileNameTableInfo;

  m_arcFileNameTableInfo[type]->nameTable     = arcFileNameTable;
  m_arcFileNameTableInfo[type]->num           = arcFileNum;
  m_arcFileNameTableInfo[type]->ignoreNameLen = arcFileIgnoreNameLen;

  if(arcFilePrefixName)
  {
    res |= Str::CreateFileName(heap, &(m_arcFileNameTableInfo[type]->prefixNameLen), &(m_arcFileNameTableInfo[type]->prefixName), NULL, arcFilePrefixName);
    if(!res.IsSuccess())
    {
      GFL_FSDEBUG_ASYNC_FILE_MANAGER_ATTENTION(0);
    }
  }
  else
  {
    m_arcFileNameTableInfo[type]->prefixName = NULL;
    m_arcFileNameTableInfo[type]->prefixNameLen = 0;
  }

  return res;
}

char* AsyncFileManager::GetUsableArcFileName(ArcFileNameTableType::Tag type, gfl2::heap::HeapBase* heap, ArcFile::ARCID arcId) const
{
  char* usableName = NULL;

#if GFL_FS_DIRECT_SDMC_ARC_FILE_ENABLE
  {
    if(this->GetDirectSdmcArcFileUseFlag())
    {
      if(type == gfl2::fs::AsyncFileManager::ArcFileNameTableType::MAIN)
      {
        usableName = this->getUsableDirectSdmcArcFileName(heap, arcId);
        if(usableName) return usableName;
      }
    }
  }
#endif

  const char* fileName = this->getArcFileName(type, arcId);  // (arcId < m_arcFileNameTableInfo[type]->num)が真か偽かのチェックはgetArcFileNmae関数がやってくれる
  if(fileName)
  {
    Result res;
    u32 ignoreNameLen = this->getArcFileIgnoreNameLen(type);
    u32 usableNameLen = 0;
    res |= Str::CreateFileName(heap, &usableNameLen, &usableName, this->getArcFilePrefixName(type), &fileName[ignoreNameLen]);
    if(!res.IsSuccess())
    {
      GFL_FSDEBUG_ASYNC_FILE_MANAGER_ATTENTION(0);
    }
  }

  return usableName;
}

const char* AsyncFileManager::getArcFileName(ArcFileNameTableType::Tag type, ArcFile::ARCID arcId) const
{
  if(m_arcFileNameTableInfo[type] != NULL && m_arcFileNameTableInfo[type]->nameTable != NULL && arcId < m_arcFileNameTableInfo[type]->num)
  {
    return m_arcFileNameTableInfo[type]->nameTable[arcId];
  }
  else
  {
    GFL_ASSERT_MSG( ( m_arcFileNameTableInfo[type] != NULL && m_arcFileNameTableInfo[type]->nameTable != NULL ), "ファイル名テーブルが設定されていません。 type=%d, arcId=%d\n", type, arcId );  // 開発中に気付いてもらうためののASSERTなので残す
    GFL_ASSERT_MSG( ( arcId < m_arcFileNameTableInfo[type]->num ), "不正なARCID(%d)です\n", arcId );  // 開発中に気付いてもらうためののASSERTなので残す
  }
  return NULL;
}
u32 AsyncFileManager::getArcFileIgnoreNameLen(ArcFileNameTableType::Tag type) const
{
  if(m_arcFileNameTableInfo[type] != NULL)
  {
    return m_arcFileNameTableInfo[type]->ignoreNameLen;
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付いてもらうためのASSERT
    return 0;
  }
}
const char* AsyncFileManager::getArcFilePrefixName(ArcFileNameTableType::Tag type) const
{
  if(m_arcFileNameTableInfo[type] != NULL)
  {
    return m_arcFileNameTableInfo[type]->prefixName;
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付いてもらうためのASSERT
    return NULL;
  }
}


void AsyncFileManager::SetCallerHeap(gfl2::heap::HeapBase* heap)
{
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
  m_callerHeap = heap;
#endif
}
  
void AsyncFileManager::SetIntervalCheckHeap(gfl2::heap::HeapBase* heap)
{
#if GFL_FS_DEBUG_CHECK_ARC_FILE_LOAD_INTERVAL
  m_intervalCheckHeap = heap;
#endif
}

void AsyncFileManager::SetIntervalCheckLevel(s32 level)
{
#if GFL_FS_DEBUG_CHECK_ARC_FILE_LOAD_INTERVAL
  if(m_intervalCheckHeap)
  {
    m_intervalCheckLevel = level;
    if(m_intervalCheckLevel < 0) m_intervalCheckLevel = 0;
    else if(m_intervalCheckLevel >= IntervalCheckLevel::LEVEL_NUM) m_intervalCheckLevel = IntervalCheckLevel::LEVEL_NUM-1;
  }
#else
  GFL_UNUSED(level);
#endif
}
s32 AsyncFileManager::GetIntervalCheckLevel(void) const
{
#if GFL_FS_DEBUG_CHECK_ARC_FILE_LOAD_INTERVAL
  if(m_intervalCheckHeap)
  {
    return m_intervalCheckLevel;
  }
  else
  {
    return 0;  // ヒープを割り当てられていないときは機能していないので0を返す
  }
#else
  return 0;
#endif
}


const DeviceBase* AsyncFileManager::GetHioDevice(void) const
{
#if   defined(GF_PLATFORM_CTR)
  #if GFL_FS_HIO_ENABLE
    return m_hioDevice;
  #else
    return NULL;
  #endif
#else  // GF_PLATFORM_CTR
  return NULL;
#endif  // GF_PLATFORM_CTR
}

bool AsyncFileManager::GetHioUseJudgement(HioUseSetting::Tag hioUseSetting) const
{
#if   defined(GF_PLATFORM_CTR)
  #if GFL_FS_HIO_ENABLE
    bool judgement = false;
    switch(hioUseSetting)
    {
    case HioUseSetting::SAME_AS_MANAGER: judgement = this->GetHioUseFlag();     break;
    case HioUseSetting::ALWAYS_MAIN    : judgement = false;                     break;
    case HioUseSetting::ALWAYS_HIO     : judgement = this->getHioUseValidity(); break;
    }
    return judgement;
  #else
    GFL_UNUSED(hioUseSetting);
    return false;
  #endif
#else  // GF_PLATFORM_CTR
  GFL_UNUSED(hioUseSetting);
  return false;
#endif  // GF_PLATFORM_CTR
}
bool AsyncFileManager::GetHioFileUseJudgement(HioUseSetting::Tag hioUseSetting) const
{
#if   defined(GF_PLATFORM_CTR)
  #if GFL_FS_HIO_ENABLE
    bool judgement = false;
    switch(hioUseSetting)
    {
    case HioUseSetting::SAME_AS_MANAGER: judgement = this->GetHioUseFlag();         break;
    case HioUseSetting::ALWAYS_MAIN    : judgement = false;                         break;
    case HioUseSetting::ALWAYS_HIO     : judgement = this->getHioFileUseValidity(); break;
    }
    return judgement;
  #else
    GFL_UNUSED(hioUseSetting);
    return false;
  #endif
#else  // GF_PLATFORM_CTR
  GFL_UNUSED(hioUseSetting);
  return false;
#endif  // GF_PLATFORM_CTR
}

bool AsyncFileManager::getHioUseValidity(void) const
{
#if   defined(GF_PLATFORM_CTR)
  #if GFL_FS_HIO_ENABLE
    bool validity = false;
    // HIOファイルの名前テーブルが存在しなかったらtrueにはならない
    if(m_arcFileNameTableInfo[ArcFileNameTableType::HIO])
    {
      validity = this->getHioFileUseValidity();
    }
    return validity;
  #else
    return false;
  #endif
#else  // GF_PLATFORM_CTR
  return false; 
#endif  // GF_PLATFORM_CTR
}
bool AsyncFileManager::getHioFileUseValidity(void) const
{
#if   defined(GF_PLATFORM_CTR)
  #if GFL_FS_HIO_ENABLE
    bool validity = false;
    // System::GetHioEnableFlag()がtrueでなかったらtrueにならない
    // HIOデバイスがNULLだとtrueにならない
    bool enableFlag = false;
    Result res = System::GetHioEnableFlag(&enableFlag);
    if(res.IsSuccess())
    {
      validity =  (enableFlag && (m_hioDevice!=NULL));
    }
    return validity;
  #else
    return false; 
  #endif
#else  // GF_PLATFORM_CTR
  return false; 
#endif  // GF_PLATFORM_CTR
}

void AsyncFileManager::SetHioUseFlag(bool useFlag)
{
#if   defined(GF_PLATFORM_CTR)
  #if GFL_FS_HIO_ENABLE
    m_hioUseFlag = ((this->getHioUseValidity()) && useFlag);
    return;
  #else
    GFL_UNUSED(useFlag);
    return;
  #endif
#else  // GF_PLATFORM_CTR
  GFL_UNUSED(useFlag);
  return;
#endif  // GF_PLATFORM_CTR
}
bool AsyncFileManager::GetHioUseFlag(void) const
{
#if   defined(GF_PLATFORM_CTR)
  #if GFL_FS_HIO_ENABLE
    return m_hioUseFlag;
  #else
    return false;
  #endif
#else  // GF_PLATFORM_CTR
  return false;
#endif  // GF_PLATFORM_CTR
}


const DeviceBase* AsyncFileManager::GetArcSrcDevice(void) const
{
#if GFL_FS_GASC_ENABLE
  return m_arcSrcDevice;
#else
  return NULL;
#endif
}
void AsyncFileManager::SetArcSrcDefaultHeap(gfl2::heap::HeapBase* heap)
{
#if GFL_FS_GASC_ENABLE
  m_arcSrcDefaultHeap = heap;
#else
  GFL_UNUSED(heap);
#endif
}
gfl2::heap::HeapBase* AsyncFileManager::GetArcSrcDefaultHeap(void) const
{
#if GFL_FS_GASC_ENABLE
  return m_arcSrcDefaultHeap;
#else
  return NULL;
#endif
}

bool AsyncFileManager::GetArcSrcCreateJudgement(ArcSrcCreateSetting::Tag arcSrcCreateSetting) const
{
#if GFL_FS_GASC_ENABLE
  bool judgement = false;
  switch(arcSrcCreateSetting)
  {
  case ArcSrcCreateSetting::SAME_AS_MANAGER: judgement = this->GetArcSrcCreateFlag();     break;
  case ArcSrcCreateSetting::ALWAYS_CREATE:   judgement = this->getArcSrcCreateValidity(); break;
  case ArcSrcCreateSetting::NEVER_CREATE:    judgement = false;                           break;
  }
  return judgement;
#else
  GFL_UNUSED(arcSrcCreateSetting);
  return false;
#endif
}
bool AsyncFileManager::GetArcSrcUseJudgement(ArcSrcUseSetting::Tag arcSrcUseSetting) const
{
#if GFL_FS_GASC_ENABLE
  bool judgement = false;
  switch(arcSrcUseSetting)
  {
  case ArcSrcUseSetting::SAME_AS_MANAGER: judgement = this->GetArcSrcUseFlag();     break;
  case ArcSrcUseSetting::ALWAYS_USE:      judgement = this->getArcSrcUseValidity(); break;
  case ArcSrcUseSetting::NEVER_USE:       judgement = false;                        break;
  }
  return judgement;
#else
  GFL_UNUSED(arcSrcUseSetting);
  return false;
#endif
}

bool AsyncFileManager::getArcSrcCreateValidity(void) const
{
#if GFL_FS_GASC_ENABLE

  bool validity = false;

  // CTR版とWin32版で、ArcSrcが使えるか否かの前提条件が異なる。
  bool arcSrcCanCreateFlag = false;  // 前提条件
#if   defined(GF_PLATFORM_CTR)
#if GFL_FS_HIO_ENABLE
  // CTR版は、HIOが有効であることが前提条件となる。
  // System::GetHioEnableFlag()がtrueでなかったらtrueにならない
  bool enableFlag = false;
  Result res = System::GetHioEnableFlag(&enableFlag);
  if(res.IsSuccess())
  {
    arcSrcCanCreateFlag = enableFlag;
  }
#endif  // GFL_FS_HIO_ENABLE
#elif defined(GF_PLATFORM_WIN32)
  // Win32版は、特に前提条件はない。
  arcSrcCanCreateFlag = true;
#endif  // GF_PLATFORM_

  if(arcSrcCanCreateFlag)  // 前提条件をクリアしたか
  {
    // ArcSrcファイルの名前テーブルが存在しなかったらtrueにはならない
    // ArcSrcデバイスがNULLだとtrueにはならない
    if(m_arcFileNameTableInfo[ArcFileNameTableType::ARCSRC])
    {
      validity = (m_arcSrcDevice!=NULL);
    }
  }
  return validity;

#else  // GFL_FS_GASC_ENABLE
  
  return false;

#endif  // GFL_FS_GASC_ENABLE
}
bool AsyncFileManager::getArcSrcUseValidity(void) const
{
#if GFL_FS_GASC_ENABLE
  return this->getArcSrcCreateValidity();
#else  // GFL_FS_GASC_ENABLE
  return false;
#endif  // GFL_FS_GASC_ENABLE
}

void AsyncFileManager::SetArcSrcCreateFlag(bool createFlag)
{
#if GFL_FS_GASC_ENABLE
  m_arcSrcCreateFlag = ((this->getArcSrcCreateValidity()) && createFlag);
  // m_arcSrcCreateFlagがtrueでなかったらm_arcSrcUseFlagもtrueにならない
  if(!m_arcSrcCreateFlag) m_arcSrcUseFlag = false;
#else
  GFL_UNUSED(createFlag);
#endif
}
bool AsyncFileManager::GetArcSrcCreateFlag(void) const
{
#if GFL_FS_GASC_ENABLE
  return m_arcSrcCreateFlag;
#else
  return false;
#endif
}

void AsyncFileManager::SetArcSrcUseFlag(bool useFlag)
{
#if GFL_FS_GASC_ENABLE
  // m_arcSrcCreateFlagがtrueでなかったらtrueにならない
  m_arcSrcUseFlag = (m_arcSrcCreateFlag && useFlag);
#else
  GFL_UNUSED(useFlag);
#endif
}
bool AsyncFileManager::GetArcSrcUseFlag(void) const
{
#if GFL_FS_GASC_ENABLE
  return m_arcSrcUseFlag;
#else
  return false;
#endif
}




void AsyncFileManager::CreateDirectSdmcArcFileExistTable(gfl2::heap::HeapBase* heap)
{
#if GFL_FS_DIRECT_SDMC_ARC_FILE_ENABLE
  if(m_directSdmcArcFileExistTable == NULL)
  {
    ArcFileNameTableType::Tag type = ArcFileNameTableType::DIRECT_SDMC;
    if(m_arcFileNameTableInfo[type] != NULL)
    {
      size_t existTableElemNum = m_arcFileNameTableInfo[type]->num /32;
      if(m_arcFileNameTableInfo[type]->num %32 != 0) ++existTableElemNum;
      if(existTableElemNum > 0)
      {
        m_directSdmcArcFileExistTable = GFL_NEW_ARRAY(heap) u32[existTableElemNum];
        gfl2::heap::HeapBase* tmpHeap = heap->GetLowerHandle();

        for(size_t i=0; i<m_arcFileNameTableInfo[type]->num; ++i)
        {
          ArcFile::ARCID arcId = i;

          u32 existTableElemIndex = arcId /32;
          u32 existTableBit       = arcId %32;

          // いったんナシにして、、、
          m_directSdmcArcFileExistTable[existTableElemIndex] &= (~(1<<existTableBit));

          if( this->checkDirectSdmcArcFileExist(tmpHeap, arcId) )
          {
            // 、、、ファイルがあればアリにする。
            m_directSdmcArcFileExistTable[existTableElemIndex] |= (1<<existTableBit);
          }
        }
      }
      else
      {
        GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。garcファイルが0個。
      }
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。garcファイルの名前テーブルが作成されていない。
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。二度呼ばれている。
  }
#endif
}

void AsyncFileManager::SetDirectSdmcArcFileUseFlag(bool useFlag)
{
#if GFL_FS_DIRECT_SDMC_ARC_FILE_ENABLE
  m_directSdmcArcFileUseFlag = useFlag;
#else
  GFL_UNUSED(useFlag);
#endif
}
bool AsyncFileManager::GetDirectSdmcArcFileUseFlag(void) const
{
#if GFL_FS_DIRECT_SDMC_ARC_FILE_ENABLE
  if(m_arcFileNameTableInfo[ArcFileNameTableType::DIRECT_SDMC] != NULL)  // garcファイルの名前テーブルが作成されていなければならない
  {
    if(m_directSdmcArcFileExistTable)  // m_directSdmcArcFileExistTableが作成されていなければならない
    {
      return m_directSdmcArcFileUseFlag;
    }
  }
  return false;
#else
  return false;
#endif
}

char* AsyncFileManager::getUsableDirectSdmcArcFileName(gfl2::heap::HeapBase* heap, ArcFile::ARCID arcId) const
{
  char* usableName = NULL;
#if GFL_FS_DIRECT_SDMC_ARC_FILE_ENABLE
  if(this->checkDirectSdmcArcFileExistFast(arcId))
  {
    usableName = this->GetUsableArcFileName(ArcFileNameTableType::DIRECT_SDMC, heap, arcId);
  }
#endif
  return usableName;
}

bool AsyncFileManager::checkDirectSdmcArcFileExistFast(ArcFile::ARCID arcId) const
{
  bool existFlag = false;
#if GFL_FS_DIRECT_SDMC_ARC_FILE_ENABLE
  if(m_directSdmcArcFileExistTable)
  {
    if( arcId < m_arcFileNameTableInfo[ArcFileNameTableType::DIRECT_SDMC]->num )  // m_directSdmcArcFileExistTableが生成されているなら、m_arcFileNameTableInfo[ArcFileNameTableType::DIRECT_SDMC]がNULLのことはない。
    {
      u32 existTableElemIndex = arcId /32;
      u32 existTableBit       = arcId %32;
     
      if( m_directSdmcArcFileExistTable[existTableElemIndex] & (1<<existTableBit) )
      {
        existFlag = true;
      }
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。m_directSdmcArcFileExistTableが作成されていない。
  }
#endif
  return existFlag;
}

bool AsyncFileManager::checkDirectSdmcArcFileExist(gfl2::heap::HeapBase* tmpHeap, ArcFile::ARCID arcId) const
{
  bool existFlag = false;
#if GFL_FS_DIRECT_SDMC_ARC_FILE_ENABLE
  ArcFileNameTableType::Tag type = ArcFileNameTableType::DIRECT_SDMC;
  const char* fileName = this->getArcFileName(type, arcId);  // (arcId < m_arcFileNameTableInfo[type]->num)が真か偽かのチェックはgetArcFileNmae関数がやってくれる
  if(fileName)
  {
    char* usableName = NULL;
    Result res;
    u32 ignoreNameLen = this->getArcFileIgnoreNameLen(type);
    u32 usableNameLen = 0;
    res |= Str::CreateFileName(tmpHeap, &usableNameLen, &usableName, this->getArcFilePrefixName(type), &fileName[ignoreNameLen]);
    if(res.IsSuccess())
    {
      // ファイルの有無を確認しにいく
      DirectSdmcFile* file = System::CreateDirectSdmcFile(tmpHeap, usableName, Define::OpenMode::READ);
      if(file)
      {
        existFlag = true;
        System::DestroyDirectSdmcFile(file);
        GFL_PRINT("SDカードに\"%s\"がある\n", usableName);
      }
      else
      {
        //GFL_PRINT("SDカードに\"%s\"がない\n", usableName);
      }
      GflHeapSafeFreeMemory(usableName);
    }
    else
    {
      GFL_FSDEBUG_ASYNC_FILE_MANAGER_ATTENTION(0);
    }
  }
#endif
  return existFlag;
}




const DeviceBase* AsyncFileManager::GetPcDevice(void) const
{
  const DeviceBase* device = NULL;
#if   defined(GF_PLATFORM_CTR)
#if GFL_FS_HIO_ENABLE
  device = m_hioDevice;
#endif
#elif defined(GF_PLATFORM_WIN32)
  device = m_mainDevice;
#endif  // GF_PLATFORM_CTR, GF_PLATFORM_WIN32
  return device;
}




void AsyncFileManager::SetDefaultArcLang(ToolDefine::ArcLangType::Tag lang)
{
  m_defaultArcLang = lang;
}
//void AsyncFileManager::SetDefaultArcLangByPmVersion(u32 lang)
//{
//  langをToolDefine::ArcLangType::Tagに変換する
//}
//void SetDefaultArcLangByPmLanguage(xy::MSGLANGID lang)
//{
//  langをToolDefine::ArcLangType::Tagに変換する
//}

const ArcFile* AsyncFileManager::GetArcFile(const char* fileName) const
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_fileListCriticalSection);

  const ArcFile* arcFile = NULL;
  ElemBase* elem = this->GetFileElem(fileName);
  if(elem)
  {
    switch(elem->GetElemType())
    {
    case FileElemType::ARC_FILE:
      {
        ArcFileElem* el = static_cast<ArcFileElem*>(elem);
        arcFile = el->GetFile();
        break;
      }
    default:
      {
        GFL_ASSERT(0);  // 開発中に気付いてもらうためのASSERT。同じ名前のファイルがARC_FILEでないタイプで保持されている。
        break;
      }
    }
  }
  return arcFile;
}
const ArcFile* AsyncFileManager::GetArcFile(ArcFile::ARCID arcId) const
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_fileListCriticalSection);
  
  const ArcFile* arcFile = NULL;
  ElemBase* elem = this->GetArcFileElem(arcId);
  if(elem)
  {
    ArcFileElem* el = static_cast<ArcFileElem*>(elem);
    arcFile = el->GetFile();
  }
  return arcFile;
}

bool AsyncFileManager::getReqElemDeleteOnRemoveFlag(const ElemBase* elem)
{
  bool deleteOnRemoveFlag = false;
  switch(elem->GetElemType())
  {
  case ReqElemType::FILE_EASY_READ_REQ        :
    {
      const FileEasyReadReqElem*       el = static_cast<const FileEasyReadReqElem*      >(elem);
      deleteOnRemoveFlag = el->GetDeleteOnRemoveFlag();
      break;
    }
  case ReqElemType::FILE_EASY_WRITE_REQ       :
    {
      const FileEasyWriteReqElem*      el = static_cast<const FileEasyWriteReqElem*     >(elem);     
      deleteOnRemoveFlag = el->GetDeleteOnRemoveFlag();
      break;
    }
  case ReqElemType::ARC_FILE_OPEN_REQ         :
    {
      const ArcFileOpenReqElem*        el = static_cast<const ArcFileOpenReqElem*       >(elem);       
      deleteOnRemoveFlag = el->GetDeleteOnRemoveFlag();
      break;
    }
  case ReqElemType::ARC_FILE_CLOSE_REQ        :
    {
      const ArcFileCloseReqElem*       el = static_cast<const ArcFileCloseReqElem*      >(elem);      
      deleteOnRemoveFlag = el->GetDeleteOnRemoveFlag();
      break;
    }
  case ReqElemType::ARC_FILE_LOAD_DATA_REQ    :
    {
      const ArcFileLoadDataReqElem*    el = static_cast<const ArcFileLoadDataReqElem*   >(elem);   
      deleteOnRemoveFlag = el->GetDeleteOnRemoveFlag();
      break;
    }
  case ReqElemType::ARC_FILE_LOAD_DATA_BUF_REQ:
    {
      const ArcFileLoadDataBufReqElem* el = static_cast<const ArcFileLoadDataBufReqElem*>(elem);
      deleteOnRemoveFlag = el->GetDeleteOnRemoveFlag();
      break;
    }
  case ReqElemType::ARC_FILE_LOAD_DATA_PIECE_BUF_REQ:
    {
      const ArcFileLoadDataPieceBufReqElem* el = static_cast<const ArcFileLoadDataPieceBufReqElem*>(elem);
      deleteOnRemoveFlag = el->GetDeleteOnRemoveFlag();
      break;
    }
  case ReqElemType::ARC_FILE_SEEK_DATA_PTR_REQ:
    {
      const ArcFileSeekDataPtrReqElem* el = static_cast<const ArcFileSeekDataPtrReqElem*>(elem);
      deleteOnRemoveFlag = el->GetDeleteOnRemoveFlag();
      break;
    }
  case ReqElemType::ARC_FILE_LOAD_DATA_PTR_BUF_REQ:
    {
      const ArcFileLoadDataPtrBufReqElem* el = static_cast<const ArcFileLoadDataPtrBufReqElem*>(elem);
      deleteOnRemoveFlag = el->GetDeleteOnRemoveFlag();
      break;
    }
  }
  return deleteOnRemoveFlag;
}

const DeviceBase* AsyncFileManager::GetMainDevice(void) const
{
  return m_mainDevice;
}
const DeviceBase* AsyncFileManager::GetMainDevice(ArcFile::ARCID arcId) const
{
#if GFL_FS_DIRECT_SDMC_ARC_FILE_ENABLE
  {
    if(this->GetDirectSdmcArcFileUseFlag())
    {
      if(this->checkDirectSdmcArcFileExistFast(arcId))
      {
        return System::GetDirectSdmcDevice();
      }
    }
  }
#endif
  return m_mainDevice;
}
const DeviceBase* AsyncFileManager::GetMainDevice(gfl2::heap::HeapBase* tmpHeap, const char* fileName) const  // tmpHeapは一時的に使用するヒープ
{
#if GFL_FS_DIRECT_SDMC_ARC_FILE_ENABLE
  {
    if(this->GetDirectSdmcArcFileUseFlag())
    {
      for(size_t i=0; i<m_arcFileNameTableInfo[ArcFileNameTableType::DIRECT_SDMC]->num; ++i)
      {
        ArcFile::ARCID arcId = i;
        if( this->checkDirectSdmcArcFileExistFast(arcId) )
        {
          char* usableName = this->getUsableDirectSdmcArcFileName(tmpHeap, arcId);
          if(usableName)
          {
            if( Str::CheckFileNameIsSame(fileName, usableName) )
            {
              GflHeapSafeFreeMemory(usableName);
              return System::GetDirectSdmcDevice();
            }
            else
            {
              GflHeapSafeFreeMemory(usableName);
            }
          }
        }
      }
    }
  }
#endif
  return m_mainDevice;
}

ElemBase* AsyncFileManager::GetReqListHeadElemAndSetReqListElemNow(void)
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);
  m_reqListElemNow = m_reqList->GetHead();
  return m_reqListElemNow;
}
void AsyncFileManager::DeleteReqListElemAndUnsetReqListElemNow(ElemBase* elem)
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);
  GFL_ASSERT(elem == m_reqListElemNow);  // 開発中に気付かせるためのASSERT
  this->deleteReqListElem(elem);
  m_reqListElemNow = NULL;
}

ElemBase* AsyncFileManager::GetUncompListHeadElemAndSetUncompListElemNow(void)
{
  gfl2::thread::ScopedLock uncompListScopedLock(*m_uncompListCriticalSection);
  m_uncompListElemNow = m_uncompList->GetHead();
  return m_uncompListElemNow;
}
void AsyncFileManager::DeleteUncompListElemAndUnsetUncompListElemNow(ElemBase* elem)
{
  gfl2::thread::ScopedLock uncompListScopedLock(*m_uncompListCriticalSection);
  GFL_ASSERT(elem == m_uncompListElemNow);  // 開発中に気付かせるためのASSERT
  this->deleteUncompListElem(elem);
  m_uncompListElemNow = NULL;
}

ElemBase* AsyncFileManager::GetFileElem(const char* fileName) const
{
  ElemBase* elem = m_fileList->GetHead();
  while(elem)
  {
    switch(elem->GetElemType())
    {
    case FileElemType::FILE:     
      {
        FileElem* el = static_cast<FileElem*>(elem);
        bool sameFlag = false;
        Result res = el->GetFile()->CheckFileNameIsSame(&sameFlag, fileName);
        if(res.IsSuccess())
        {
          if(sameFlag) return elem;
        }
        else
        {
          GFL_FSDEBUG_ASYNC_FILE_MANAGER_ATTENTION(0);
        }
        break;
      }
    case FileElemType::ARC_FILE:
      {
        ArcFileElem* el = static_cast<ArcFileElem*>(elem);
        bool sameFlag = false;
        Result res = el->GetFile()->CheckFileNameIsSame(&sameFlag, fileName);
        if(res.IsSuccess())
        {
          if(sameFlag) return elem;
        }
        else
        {
          GFL_FSDEBUG_ASYNC_FILE_MANAGER_ATTENTION(0);
        }
        break;
      }
    default:
      {
        break;
      }
    }
    elem = elem->GetNext();
  }
  return NULL;
}

void AsyncFileManager::DeleteFileElem(const char* fileName)
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_fileListCriticalSection);
  
  ElemBase* elem = this->GetFileElem(fileName);
  m_fileList->Remove(elem);
  GFL_SAFE_DELETE(elem);
}

ElemBase* AsyncFileManager::GetArcFileElem(ArcFile::ARCID arcId) const
{
  ElemBase* elem = m_fileList->GetHead();
  while(elem)
  {
    switch(elem->GetElemType())
    {
    case FileElemType::ARC_FILE:
      {
        ArcFileElem* el = static_cast<ArcFileElem*>(elem);
        if( el->GetFile()->GetArcId() == arcId )
        {
          return elem;
        }
        break;
      }
    default:
      {
        break;
      }
    }
    elem = elem->GetNext();
  }
  return NULL;
}

void AsyncFileManager::DeleteArcFileElem(ArcFile::ARCID arcId)
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_fileListCriticalSection);
  
  ElemBase* elem = this->GetArcFileElem(arcId);
  m_fileList->Remove(elem);
  GFL_SAFE_DELETE(elem);
}

#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
u32 AsyncFileManager::GetArcFileCount(const char* fileName) const
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_fileListCriticalSection);
  
  u32 ret = 0;
  ElemBase* elem = this->GetFileElem(fileName);
  if(elem)
  {
    switch(elem->GetElemType())
    {
    case FileElemType::ARC_FILE:
      {
        ArcFileElem* el = static_cast<ArcFileElem*>(elem);
        ret = el->GetCount();
        if(ret == 0) ret = 1;  // elemが残っているのに参照カウンタが0ということは、他のスレッドでelemを破棄中である。破棄中なので残っている扱いにすべきだから1にしておく。
        break;
      }
    default:
      {
        GFL_ASSERT(0);  // 開発中に気付いてもらうためのASSERT。同じ名前のファイルがARC_FILEでないタイプで保持されている。
        break;
      }
    }
  }
  return ret;
}
u32 AsyncFileManager::GetArcFileCount(ArcFile::ARCID arcId) const
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_fileListCriticalSection);
  
  u32 ret = 0;
  ElemBase* elem = this->GetArcFileElem(arcId);
  if(elem)
  {
    ArcFileElem* el = static_cast<ArcFileElem*>(elem);
    ret = el->GetCount();
    if(ret == 0) ret = 1;  // elemが残っているのに参照カウンタが0ということは、他のスレッドでelemを破棄中である。破棄中なので残っている扱いにすべきだから1にしておく。
  }
  return ret;
}
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST

void AsyncFileManager::AddFileEasyReadUncomp      (FileEasyReadReq*       req)
{
  gfl2::thread::ScopedLock uncompListScopedLock(*m_uncompListCriticalSection);
  /*FileEasyReadReqElem* elem = */addReqTemplate<FileEasyReadReq, FileEasyReadReqElem>(m_uncompList, *req, req->heapForCompressed, 0);
  //elem->GetReq()->m_syncReqFinishEvent = req->m_syncReqFinishEvent;  // addReqTemplateで代入済みなのでコメントアウト。
  req->m_syncReqFinishEvent = NULL;  // 同期リクエストの処理の終了連絡は、解凍スレッドに任せたのでNULLにしておく。
  m_uncompListExecEvent->Signal();
}
void AsyncFileManager::AddArcFileLoadDataUncomp   (ArcFileLoadDataReq*    req)
{
#if GFL_FS_DEBUG_LOG
  this->logFsDebugLogArcFileInfo(ReqElemType::ARC_FILE_LOAD_DATA_REQ, FsDebugLogArcFileInfoKind::ANY, req->arcId, req->datId, FsDebugLogArcFileInfoStep::UNCOMP_ADD);
#endif
  gfl2::thread::ScopedLock uncompListScopedLock(*m_uncompListCriticalSection);
  /*ArcFileLoadDataReqElem* elem = */addReqTemplate<ArcFileLoadDataReq, ArcFileLoadDataReqElem>(m_uncompList, *req, req->heapForCompressed, 0);
  //elem->GetReq()->m_syncReqFinishEvent = req->m_syncReqFinishEvent;  // addReqTemplateで代入済みなのでコメントアウト。
  req->m_syncReqFinishEvent = NULL;  // 同期リクエストの処理の終了連絡は、解凍スレッドに任せたのでNULLにしておく。
  m_uncompListExecEvent->Signal();
}
void AsyncFileManager::AddArcFileLoadDataBufUncomp(ArcFileLoadDataBufReq* req)
{
#if GFL_FS_DEBUG_LOG
  this->logFsDebugLogArcFileInfo(ReqElemType::ARC_FILE_LOAD_DATA_BUF_REQ, FsDebugLogArcFileInfoKind::ANY, req->arcId, req->datId, FsDebugLogArcFileInfoStep::UNCOMP_ADD);
#endif
  gfl2::thread::ScopedLock uncompListScopedLock(*m_uncompListCriticalSection);
  /*ArcFileLoadDataBufReqElem* elem = */addReqTemplate<ArcFileLoadDataBufReq, ArcFileLoadDataBufReqElem>(m_uncompList, *req, req->heapForCompressed, 0);
  //elem->GetReq()->m_syncReqFinishEvent = req->m_syncReqFinishEvent;  // addReqTemplateで代入済みなのでコメントアウト。
  req->m_syncReqFinishEvent = NULL;  // 同期リクエストの処理の終了連絡は、解凍スレッドに任せたのでNULLにしておく。
  m_uncompListExecEvent->Signal();
}

/*
// 最大個数チェック。必ずコメントアウトしてコミットして下さい。
static s32 s_testFileNumCountMax = 0;
*/

void AsyncFileManager::AddFile(gfl2::heap::HeapBase* heap, File* file)
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_fileListCriticalSection);

  FileElem* elem = GFL_NEW(heap) FileElem();
  elem->SetFile(file);
  m_fileList->Add(elem);
/*
  // 最大個数チェック。必ずコメントアウトしてコミットして下さい。
  {
    s32 count = 0;
    ElemBase* testElem = m_fileList->GetHead();
    while(testElem)
    {
      ++count;
      testElem = testElem->GetNext();
    }
    if(count > s_testFileNumCountMax)
    {
      s_testFileNumCountMax = count;
      GFL_RELEASE_PRINT("s_testFileNumCountMax=%d\n", s_testFileNumCountMax);
    }
  }
*/
}
AsyncFileManager::ArcFileElem* AsyncFileManager::AddArcFile(gfl2::heap::HeapBase* heap, ArcFile* file)
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_fileListCriticalSection);
  
  ArcFileElem* elem = GFL_NEW(heap) ArcFileElem();
  elem->SetFile(file);
#if GFL_FS_DEBUG_CHECK_ARC_FILE_LOAD_INTERVAL
  elem->CreateIntervalCheckInfo(this->getIntervalCheckHeap());
#endif
  m_fileList->Add(elem);
/*
  // 最大個数チェック。必ずコメントアウトしてコミットして下さい。
  {
    s32 count = 0;
    ElemBase* testElem = m_fileList->GetHead();
    while(testElem)
    {
      ++count;
      testElem = testElem->GetNext();
    }
    if(count > s_testFileNumCountMax)
    {
      s_testFileNumCountMax = count;
      GFL_RELEASE_PRINT("s_testFileNumCountMax=%d\n", s_testFileNumCountMax);
    }
  }
*/
  return elem;
}

ToolDefine::ArcLangType::Tag AsyncFileManager::GetLangOfArcFile(ToolDefine::ArcLangType::Tag langOfArcFile) const
{
  if(langOfArcFile == ToolDefine::ArcLangType::DEFAULT_LANG) return m_defaultArcLang;
  return langOfArcFile;
}


AsyncFileManager::FileEasyReadReqElem*       AsyncFileManager::getFileEasyReadReqListElem      (void**       ppBuf   ) const
{
  return getReqElemTemplate_ppBuf<FileEasyReadReq, FileEasyReadReqElem, ReqElemType::FILE_EASY_READ_REQ>(m_reqList, ppBuf);
}
AsyncFileManager::FileEasyWriteReqElem*      AsyncFileManager::getFileEasyWriteReqListElem     (const char*  fileName) const
{
  return getReqElemTemplate_fileName<FileEasyWriteReq, FileEasyWriteReqElem, ReqElemType::FILE_EASY_WRITE_REQ>(m_reqList, fileName);
}
AsyncFileManager::ArcFileOpenReqElem*        AsyncFileManager::getArcFileOpenReqListElem       (const char*  fileName) const
{
  return getReqElemTemplate_fileName<ArcFileOpenReq, ArcFileOpenReqElem, ReqElemType::ARC_FILE_OPEN_REQ>(m_reqList, fileName);
}
AsyncFileManager::ArcFileOpenReqElem*        AsyncFileManager::getArcFileOpenReqListElem       (ArcFile::ARCID arcId) const
{
  return getReqElemTemplate_arcId<ArcFileOpenReq, ArcFileOpenReqElem, ReqElemType::ARC_FILE_OPEN_REQ>(m_reqList, arcId);
}
AsyncFileManager::ArcFileCloseReqElem*       AsyncFileManager::getArcFileCloseReqListElem      (const char*  fileName) const
{
  return getReqElemTemplate_fileName<ArcFileCloseReq, ArcFileCloseReqElem, ReqElemType::ARC_FILE_CLOSE_REQ>(m_reqList, fileName);
}
AsyncFileManager::ArcFileCloseReqElem*       AsyncFileManager::getArcFileCloseReqListElem      (ArcFile::ARCID arcId) const
{
  return getReqElemTemplate_arcId<ArcFileCloseReq, ArcFileCloseReqElem, ReqElemType::ARC_FILE_CLOSE_REQ>(m_reqList, arcId);
}
AsyncFileManager::ArcFileLoadDataReqElem*    AsyncFileManager::getArcFileLoadDataReqListElem   (void**       ppBuf   ) const
{
  return getReqElemTemplate_ppBuf<ArcFileLoadDataReq, ArcFileLoadDataReqElem, ReqElemType::ARC_FILE_LOAD_DATA_REQ>(m_reqList, ppBuf);
}
AsyncFileManager::ArcFileLoadDataBufReqElem* AsyncFileManager::getArcFileLoadDataBufReqListElem(const void*  pBuf    ) const
{
  return getReqElemTemplate_pBuf<ArcFileLoadDataBufReq, ArcFileLoadDataBufReqElem, ReqElemType::ARC_FILE_LOAD_DATA_BUF_REQ>(m_reqList, pBuf);
}
AsyncFileManager::ArcFileLoadDataPieceBufReqElem* AsyncFileManager::getArcFileLoadDataPieceBufReqListElem(const void*  pBuf    ) const
{
  return getReqElemTemplate_pBuf<ArcFileLoadDataPieceBufReq, ArcFileLoadDataPieceBufReqElem, ReqElemType::ARC_FILE_LOAD_DATA_PIECE_BUF_REQ>(m_reqList, pBuf);
}
AsyncFileManager::ArcFileSeekDataPtrReqElem*      AsyncFileManager::getArcFileSeekDataPtrReqListElem     (const char*  fileName) const
{
  return getReqElemTemplate_fileName<ArcFileSeekDataPtrReq, ArcFileSeekDataPtrReqElem, ReqElemType::ARC_FILE_SEEK_DATA_PTR_REQ>(m_reqList, fileName);
}
AsyncFileManager::ArcFileSeekDataPtrReqElem*      AsyncFileManager::getArcFileSeekDataPtrReqListElem     (ArcFile::ARCID arcId) const
{
  return getReqElemTemplate_arcId<ArcFileSeekDataPtrReq, ArcFileSeekDataPtrReqElem, ReqElemType::ARC_FILE_SEEK_DATA_PTR_REQ>(m_reqList, arcId);
}
AsyncFileManager::ArcFileLoadDataPtrBufReqElem*   AsyncFileManager::getArcFileLoadDataPtrBufReqListElem  (const void*  pBuf    ) const
{
  return getReqElemTemplate_pBuf<ArcFileLoadDataPtrBufReq, ArcFileLoadDataPtrBufReqElem, ReqElemType::ARC_FILE_LOAD_DATA_PTR_BUF_REQ>(m_reqList, pBuf);
}

AsyncFileManager::FileEasyReadReqElem*            AsyncFileManager::getFileEasyReadUncompListElem        (void**         ppBuf   ) const
{
  return getReqElemTemplate_ppBuf<FileEasyReadReq, FileEasyReadReqElem, ReqElemType::FILE_EASY_READ_REQ>(m_uncompList, ppBuf);
}
AsyncFileManager::ArcFileLoadDataReqElem*         AsyncFileManager::getArcFileLoadDataUncompListElem     (void**         ppBuf   ) const
{
  return getReqElemTemplate_ppBuf<ArcFileLoadDataReq, ArcFileLoadDataReqElem, ReqElemType::ARC_FILE_LOAD_DATA_REQ>(m_uncompList, ppBuf);
}
AsyncFileManager::ArcFileLoadDataBufReqElem*      AsyncFileManager::getArcFileLoadDataBufUncompListElem  (const void*    pBuf    ) const
{
  return getReqElemTemplate_pBuf<ArcFileLoadDataBufReq, ArcFileLoadDataBufReqElem, ReqElemType::ARC_FILE_LOAD_DATA_BUF_REQ>(m_uncompList, pBuf);
}

void AsyncFileManager::deleteReqListElem(ElemBase* elem)
{
  m_reqList->Remove(elem);
  bool deleteOnRemoveFlag = this->getReqElemDeleteOnRemoveFlag(elem);
  if(deleteOnRemoveFlag)
  {
    GFL_SAFE_DELETE(elem);
  }
}
void AsyncFileManager::deleteUncompListElem(ElemBase* elem)
{
  m_uncompList->Remove(elem);
  bool deleteOnRemoveFlag = this->getReqElemDeleteOnRemoveFlag(elem);
  if(deleteOnRemoveFlag)
  {
    GFL_SAFE_DELETE(elem);
  }
}

bool AsyncFileManager::cancelReqOnly(ElemBase* reqElem)
{
  bool ret = false;

  // m_reqList
  if(reqElem) 
  {
    if(reqElem != m_reqListElemNow)  // リクエストの処理を開始するのを待っているとき
    {
      this->deleteReqListElem(reqElem);
      ret = true;
    }
  }

  return ret;
}

void AsyncFileManager::deleteFileEasyReadReqObjBeforeUncomp      (FileEasyReadReq*       req)
{
  GflHeapSafeFreeMemory(*(req->ppBuf));
}
void AsyncFileManager::deleteArcFileLoadDataReqObjBeforeUncomp   (ArcFileLoadDataReq*    req)
{
  GflHeapSafeFreeMemory(*(req->ppBuf));
}
void AsyncFileManager::deleteArcFileLoadDataBufReqObjBeforeUncomp(ArcFileLoadDataBufReq* req)
{
  GflHeapSafeFreeMemory(req->pBuf);
}


void AsyncFileManager::deleteAllReqListElemAtDestructor(void)
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_reqListCriticalSection);
  if(m_reqList)
  {
    ElemBase* elem = m_reqList->GetHead();
    GFL_ASSERT(elem==NULL);  // 開発中に気付かせるためのASSERT。処理が済んでいないものがリストに残っている。
    while(elem)
    {
      ElemBase* nextElem = elem->GetNext();
      m_reqList->Remove(elem);
      GFL_SAFE_DELETE(elem);
      elem = nextElem;
    }
  }
}
void AsyncFileManager::deleteAllUncompListElemAtDestructor(void)
{
  gfl2::thread::ScopedLock uncompListScopedLock(*m_uncompListCriticalSection);
  if(m_uncompList)
  {
    ElemBase* elem = m_uncompList->GetHead();
    GFL_ASSERT(elem==NULL);  // 開発中に気付かせるためのASSERT。処理が済んでいないものがリストに残っている。
    while(elem)
    {
      ElemBase* nextElem = elem->GetNext();
      m_uncompList->Remove(elem);
      GFL_SAFE_DELETE(elem);
      elem = nextElem;
    }
  }
}
void AsyncFileManager::deleteAllFileElemAndContentAtDestructor(void)
{
  gfl2::thread::ScopedLock reqListScopedLock(*m_fileListCriticalSection);
  
  if(m_fileList)
  {
    ElemBase* elem = m_fileList->GetHead();
    //GFL_ASSERT(elem==NULL);  // 開発中に気付かせるためのASSERT。処理が済んでいないものがリストに残っている。  // @attention コメントアウトせずASSERTを有効にしたいので、todoとして残しておきたいもの。nijiでは「電源OFF or リセットする時の処理」にてひっかかるのでコメントアウト。この関数内で残っているファイルをクローズしているのでASSERTをなくしても問題ない。
    while(elem)
    {
      ElemBase* nextElem = elem->GetNext();
      FileBase* file = NULL;
     
      switch(elem->GetElemType())
      {
      case FileElemType::FILE:
        {
          FileElem* fileElem = static_cast<FileElem*>(elem);
          file = fileElem->GetFile();
          break;
        }
      case FileElemType::ARC_FILE:
        {
          ArcFileElem* arcFileElem = static_cast<ArcFileElem*>(elem);
          file = arcFileElem->GetFile();
          break;
        }
      }

      // FileElemの破棄
      m_fileList->Remove(elem);
      GFL_SAFE_DELETE(elem);
    
      // Contentの破棄
      GFL_SAFE_DELETE(file);

      elem = nextElem;
    }
  }
}


#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
AsyncFileManager::CallerInfo* AsyncFileManager::createCallerInfo(const char* a_callerFileName, u32 a_callerLineNo)
{
  if(m_callerHeap)
  {
    s32 allocatableSize = m_callerHeap->GetTotalAllocatableSize();
    s32 requireSize     = Calc::GetTypeSizeIncludedExtraSize<CallerInfo>();
    if(allocatableSize >= requireSize)  // 比較前に型を合わせておく
    {
      CallerInfo* callerInfo = GFL_NEW(m_callerHeap) CallerInfo;
      callerInfo->SetCaller(a_callerFileName, a_callerLineNo);
      return callerInfo;
    }
    else
    {
      GFL_PRINT("%s(%d) add open or close req, but it's not memorized\n", a_callerFileName, a_callerLineNo);
    }
  }
  return NULL;
}
#endif




#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
AsyncFileManager::CallerInfo::CallerInfo(void)
  : callerLineNo(0)
{
  callerFileName[0] = '\0';
}
void AsyncFileManager::CallerInfo::SetCaller(const char* a_callerFileName, u32 a_callerLineNo)
{
  Str::SetFileNameInBuf(callerFileName, CALLER_FILE_NAME_BUF_SIZE, a_callerFileName);
  callerLineNo = a_callerLineNo;
}
#endif

#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
AsyncFileManager::CallerElem::CallerElem(void)
  : ElemBase(),
    m_callerInfo(NULL)
{
  m_callerInfo = NULL;
}
AsyncFileManager::CallerElem::~CallerElem()
{
  GFL_SAFE_DELETE(m_callerInfo);  // m_callerInfoの所有権はこのクラスが持っているので、破棄する。
}
void AsyncFileManager::CallerElem::SetCallerInfo(CallerInfo* callerInfo)
{
  m_callerInfo = callerInfo;  // m_callerInfoの所有権ごともらった。
}
const AsyncFileManager::CallerInfo* AsyncFileManager::CallerElem::GetCallerInfo(void) const
{
  return m_callerInfo;
}
#endif


AsyncFileManager::ArcFileElem::ArcFileElem(void)
  : FileElemBase<ArcFile, FileElemType::ARC_FILE>(),
    m_count(0)
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
    , m_callerLists(NULL)
#endif
#if GFL_FS_DEBUG_CHECK_ARC_FILE_LOAD_INTERVAL
    , m_intervalCheckInfo(NULL)
#endif
{}
AsyncFileManager::ArcFileElem::~ArcFileElem()
{
#if GFL_FS_DEBUG_CHECK_ARC_FILE_LOAD_INTERVAL
  GFL_SAFE_DELETE(m_intervalCheckInfo);
#endif
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
  if(m_callerLists)
  {
    this->destroyCallerListAndDeleteAllElem(m_callerLists->arcFileCloseReqCallerList);
    this->destroyCallerListAndDeleteAllElem(m_callerLists->arcFileOpenReqCallerList);
    GFL_SAFE_DELETE(m_callerLists);
  }
#endif
}

#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
void AsyncFileManager::ArcFileElem::IncreaseCount(gfl2::heap::HeapBase* heap, CallerInfo* callerInfo)
{
  // heapにはm_callerHeapが渡されて来ているはず。
  
  // m_callerInfoの所有権が移動する
  bool setCallerInfoFlag = false;
  if(heap && callerInfo && m_callerLists && m_callerLists->arcFileOpenReqCallerList)
  {
    s32 allocatableSize = heap->GetTotalAllocatableSize();
    s32 requireSize     = Calc::GetTypeSizeIncludedExtraSize<CallerElem>();
    if(allocatableSize >= requireSize)  // 比較前に型を合わせておく
    {
      CallerElem* elem = GFL_NEW(heap) CallerElem;
      elem->SetCallerInfo(callerInfo);
      setCallerInfoFlag = true;
      m_callerLists->arcFileOpenReqCallerList->Add(elem);
    }
  }
  if(!setCallerInfoFlag)
  {
    // m_callerInfoの所有権を渡せなかったため、所有権が浮いているので破棄しておく。
    GFL_SAFE_DELETE(callerInfo);
  }
  this->increaseCount();
}
void AsyncFileManager::ArcFileElem::DecreaseCount(gfl2::heap::HeapBase* heap, CallerInfo* callerInfo)
{
  // heapにはm_callerHeapが渡されて来ているはず。
  
  // m_callerInfoの所有権が移動する
  bool setCallerInfoFlag = false;
  if(heap && callerInfo && m_callerLists && m_callerLists->arcFileCloseReqCallerList)
  {
    s32 allocatableSize = heap->GetTotalAllocatableSize();
    s32 requireSize     = Calc::GetTypeSizeIncludedExtraSize<CallerElem>();
    if(allocatableSize >= requireSize)  // 比較前に型を合わせておく
    {
      CallerElem* elem = GFL_NEW(heap) CallerElem;
      elem->SetCallerInfo(callerInfo);
      setCallerInfoFlag = true;
      m_callerLists->arcFileCloseReqCallerList->Add(elem);
    }
  }
  if(!setCallerInfoFlag)
  {
    // m_callerInfoの所有権を渡せなかったため、所有権が浮いているので破棄しておく。
    GFL_SAFE_DELETE(callerInfo);
  }
  this->decreaseCount();
}
#else
void AsyncFileManager::ArcFileElem::IncreaseCount(void)
{
  this->increaseCount();
}
void AsyncFileManager::ArcFileElem::DecreaseCount(void)
{
  this->decreaseCount();
}
#endif
void AsyncFileManager::ArcFileElem::increaseCount(void)
{
  ++m_count;
}
void AsyncFileManager::ArcFileElem::decreaseCount(void)
{
  if(m_count > 0)
  {
    --m_count;
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  }
}

#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
void AsyncFileManager::ArcFileElem::destroyCallerListAndDeleteAllElem(List* list)
{
  if(list)
  {
    ElemBase* elem = list->GetHead();
    while(elem)
    {
      ElemBase* nextElem = elem->GetNext();
      list->Remove(elem);
      GFL_SAFE_DELETE(elem);
      elem = nextElem;
    }
    GFL_DELETE(list);
  }
}
#endif

#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
void AsyncFileManager::ArcFileElem::CreateAllCallerList(gfl2::heap::HeapBase* heap)  // 破棄はGFL_SAFE_DELETEで行う。個々のリストの破棄はdestroyCallerListAndDeleteAllElemで行う。
{
  // heapにはm_callerHeapが渡されて来ているはず。

  if(!m_callerLists)
  {
    if(heap)
    {
      {
        s32 allocatableSize = heap->GetTotalAllocatableSize();
        s32 requireSize     = Calc::GetTypeSizeIncludedExtraSize<CallerLists>();
        if(allocatableSize >= requireSize)  // 比較前に型を合わせておく
        {
          m_callerLists = GFL_NEW(heap) CallerLists;
        }
      }

      if(m_callerLists)
      {
        {
          s32 allocatableSize = heap->GetTotalAllocatableSize();
          s32 requireSize     = Calc::GetTypeSizeIncludedExtraSize<List>();
          if(allocatableSize >= requireSize)  // 比較前に型を合わせておく
          {
            m_callerLists->arcFileOpenReqCallerList = GFL_NEW(heap) List;
          }
        }
       
        {
          s32 allocatableSize = heap->GetTotalAllocatableSize();
          s32 requireSize     = Calc::GetTypeSizeIncludedExtraSize<List>();
          if(allocatableSize >= requireSize)  // 比較前に型を合わせておく
          {
            m_callerLists->arcFileCloseReqCallerList = GFL_NEW(heap) List;
          }
        }
      }
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  }
}
AsyncFileManager::CallerElem* AsyncFileManager::ArcFileElem::GetArcFileOpenReqCallerListHeadElem(void) const
{
  if(m_callerLists && m_callerLists->arcFileOpenReqCallerList)
  {
    return reinterpret_cast<CallerElem*>(m_callerLists->arcFileOpenReqCallerList->GetHead());
  }
  return NULL;
}
AsyncFileManager::CallerElem* AsyncFileManager::ArcFileElem::GetArcFileCloseReqCallerListHeadElem(void) const
{
  if(m_callerLists && m_callerLists->arcFileCloseReqCallerList)
  {
    return reinterpret_cast<CallerElem*>(m_callerLists->arcFileCloseReqCallerList->GetHead());
  }
  return NULL;
}
#endif

#if GFL_FS_DEBUG_CHECK_ARC_FILE_LOAD_INTERVAL
void AsyncFileManager::ArcFileElem::CreateIntervalCheckInfo(gfl2::heap::HeapBase* heap)  // 破棄はGFL_SAFE_DELETEで行う。
{
  if(!m_intervalCheckInfo)
  {
    if(heap)
    {
      m_intervalCheckInfo = GFL_NEW(heap) IntervalCheckInfo;
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  }
}

void AsyncFileManager::ArcFileElem::LogIntervalCheckInfoAccessTime(bool accessFinish, s32 level)
{
  if(m_intervalCheckInfo)
  {
    s64 accessTime = gfl2::system::Timer::PerformanceCounter::GetMicroSecond();

    if(accessFinish == false)
    {
      // アクセス開始時
      
      if(m_intervalCheckInfo->firstAccessTime == 0)
      {
        // 最初のアクセス or 2秒経った後の最初のアクセス のとき
        // 最初のアクセスの時間を覚えておく
        m_intervalCheckInfo->firstAccessTime = accessTime;
      }

      if(m_intervalCheckInfo->lastAccessTime != 0)
      {
        // 2回目以降のアクセスのとき
        if(accessTime - m_intervalCheckInfo->lastAccessTime <= 2000000)
        {
          // 2秒以内
          if(accessTime - m_intervalCheckInfo->firstAccessTime >= 5000000)
          {
            // 「2秒以内に繰り返し同じファイルを読み込む」という挙動を5秒以上繰り返す
            if( level == IntervalCheckLevel::LEVEL_WARNING || level == IntervalCheckLevel::LEVEL_ASSERT )
            {
              GFL_RELEASE_PRINT("■WARNING: 連続でファイルにアクセスしました /");
              if( m_file->GetArcId() != ArcFile::ARCID_NULL )
              {
                GFL_RELEASE_PRINT("ARCID[%d]\n", m_file->GetArcId());
              }
              else
              {
                GFL_RELEASE_PRINT("FileName[%s]\n", m_file->GetFileName());
              }
              if( level == IntervalCheckLevel::LEVEL_ASSERT )
              {
                GFL_ASSERT(0);  // ASSERTで止めて気付かせる
              }
            }
          }
          // 5秒経っていないときは特に何もしない
        }
        else
        {
          // 2秒経ったとき
          // 最初のアクセスの時間をクリアしておく
          m_intervalCheckInfo->firstAccessTime = 0;
        }
      }
      // 最初のアクセスのときは特に何もしない
      
    }
    // アクセス終了時は特に何もしない

    // 最後のアクセスの時間を覚えておく
    m_intervalCheckInfo->lastAccessTime = accessTime;
  }
}
#endif


void AsyncFileManager::PrintArcFileList(void) const
{
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
  gfl2::thread::ScopedLock reqListScopedLock(*m_fileListCriticalSection);
  
  GFL_PRINT("----------------\n");
  ElemBase* elem = m_fileList->GetHead();
  while(elem)
  {
    switch(elem->GetElemType())
    {
    case FileElemType::ARC_FILE:
      {
        ArcFileElem* arcFileElem = static_cast<ArcFileElem*>(elem);
        ArcFile*     arcFile     = arcFileElem->GetFile();
        GFL_PRINT("ArcFile  "); 
        if( arcFile->GetArcId() != ArcFile::ARCID_NULL )
        {
          GFL_PRINT("arcId=%d", arcFile->GetArcId());
        }
        else
        {
          GFL_PRINT("fileName=%s", arcFile->GetFileName());
        }
        GFL_PRINT("  :count=%d", arcFileElem->GetCount());
        
        CallerElem* callerElem = NULL;
        enum
        {
          REQ_TYPE_OPEN,
          REQ_TYPE_CLOSE,
          REQ_TYPE_NUM
        };
        for(s32 i=0; i<REQ_TYPE_NUM; ++i)
        {
          if(i == REQ_TYPE_OPEN)
          {
            callerElem = arcFileElem->GetArcFileOpenReqCallerListHeadElem();
            if(callerElem)
            {
              GFL_PRINT("\n  open");
            }
          }
          else
          {
            callerElem = arcFileElem->GetArcFileCloseReqCallerListHeadElem();
            if(callerElem)
            {
              GFL_PRINT("\n  close");
            }
          }
          while(callerElem)
          {
            const char* fileName = "NULL";
            u32 lineNo = 0;
            const CallerInfo* callerInfo = callerElem->GetCallerInfo();
            if(callerInfo != NULL)
            {
              if(callerInfo->callerFileName != NULL) fileName = callerInfo->callerFileName;
              lineNo = callerInfo->callerLineNo;
            }
            GFL_PRINT("\n    %s(%d)", fileName, lineNo);
            callerElem = static_cast<CallerElem*>(callerElem->GetNext());
          }
        }
        GFL_PRINT("\n");
        break;
      }
    }
    elem = elem->GetNext();
  }
  GFL_PRINT("----------------\n");
#endif
}


#if GFL_FS_DEBUG_LOG
void AsyncFileManager::logFsDebugLogArcFileInfo(ReqElemType::Tag type, FsDebugLogArcFileInfoKind::Tag kind, ArcFile::ARCID arcId, ArcFile::ARCDATID datId, FsDebugLogArcFileInfoStep::Tag step)
{
  if(arcId != ArcFile::ARCID_NULL)
  {
    if(m_fsDebugLogArcFileInfoArray)
    {
      if(step == FsDebugLogArcFileInfoStep::FILE_ADD)
      {
        // 新規登録
        m_fsDebugLogArcFileInfoArray[m_fsDebugLogArcFileInfoArrayNextIndex].totalIndex = m_fsDebugLogArcFileInfoElemTotalNum;
        m_fsDebugLogArcFileInfoArray[m_fsDebugLogArcFileInfoArrayNextIndex].type       = static_cast<s16>(type);
        m_fsDebugLogArcFileInfoArray[m_fsDebugLogArcFileInfoArrayNextIndex].kind       = static_cast<s16>(kind);
        m_fsDebugLogArcFileInfoArray[m_fsDebugLogArcFileInfoArrayNextIndex].arcId      = arcId;
        m_fsDebugLogArcFileInfoArray[m_fsDebugLogArcFileInfoArrayNextIndex].datId      = datId;
        m_fsDebugLogArcFileInfoArray[m_fsDebugLogArcFileInfoArrayNextIndex].compSize   = 0;
        m_fsDebugLogArcFileInfoArray[m_fsDebugLogArcFileInfoArrayNextIndex].size       = 0;
        for(s32 j=0; j<FsDebugLogArcFileInfoStep::NUM; ++j)
        {
          m_fsDebugLogArcFileInfoArray[m_fsDebugLogArcFileInfoArrayNextIndex].time[j] = 0;
        }
        for(s32 j=0; j<FsDebugLogArcFileInfoTerm::NUM; ++j)
        {
          m_fsDebugLogArcFileInfoArray[m_fsDebugLogArcFileInfoArrayNextIndex].diffTime[j] = 0;
        }
        m_fsDebugLogArcFileInfoArray[m_fsDebugLogArcFileInfoArrayNextIndex].time[step] = gfl2::system::Timer::PerformanceCounter::GetMicroSecond();
 
        ++m_fsDebugLogArcFileInfoElemTotalNum;
        // インデックスを次に進めておく
        m_fsDebugLogArcFileInfoArrayNextIndex = (m_fsDebugLogArcFileInfoArrayNextIndex+1)%FS_DEBUG_LOG_ARC_FILE_INFO_ELEM_NUM_MAX;
      }
      else
      {
        // 既存に追記
        FsDebugLogArcFileInfo* elem = getFsDebugLogArcFileInfoElem(type, arcId, datId);
        if( elem )
        {
          elem->time[step] = gfl2::system::Timer::PerformanceCounter::GetMicroSecond();
        }
      }
    }
  }
}
void AsyncFileManager::setFsDebugLogArcFileInfoCompSize(ReqElemType::Tag type, ArcFile::ARCID arcId, ArcFile::ARCDATID datId, size_t compSize)  // kindは使わない。もし必要になったら引数に追加。
{
  FsDebugLogArcFileInfo* elem = getFsDebugLogArcFileInfoElem(type, arcId, datId);
  if(elem)
  {
    elem->compSize = static_cast<u32>(compSize);
  }
}
void AsyncFileManager::setFsDebugLogArcFileInfoSize(ReqElemType::Tag type, ArcFile::ARCID arcId, ArcFile::ARCDATID datId, size_t size)  // kindは使わない。もし必要になったら引数に追加。
{
  FsDebugLogArcFileInfo* elem = getFsDebugLogArcFileInfoElem(type, arcId, datId);
  if(elem)
  {
    elem->size = static_cast<u32>(size);
  }
}

void AsyncFileManager::createFsDebugLogArcFileInfo(gfl2::heap::HeapBase* heapForFsDebugLog)
{
  if(heapForFsDebugLog && FS_DEBUG_LOG_ARC_FILE_INFO_ELEM_NUM_MAX>0)
  {
    m_fsDebugLogArcFileInfoArray = GFL_NEW_ARRAY(heapForFsDebugLog) FsDebugLogArcFileInfo[FS_DEBUG_LOG_ARC_FILE_INFO_ELEM_NUM_MAX];
    m_fsDebugLogArcFileInfoElemTotalNum   = 0;
    m_fsDebugLogArcFileInfoArrayNextIndex = 0;
  }
}
void AsyncFileManager::destroyFsDebugLogArcFileInfo(void)
{
  GFL_SAFE_DELETE_ARRAY(m_fsDebugLogArcFileInfoArray);
}
AsyncFileManager::FsDebugLogArcFileInfo* AsyncFileManager::getFsDebugLogArcFileInfoElem(ReqElemType::Tag type, ArcFile::ARCID arcId, ArcFile::ARCDATID datId) const  // kindは使わない。もし必要になったら引数に追加。
{
  FsDebugLogArcFileInfo* elem = NULL;
  if(arcId != ArcFile::ARCID_NULL)
  {
    if(m_fsDebugLogArcFileInfoArray)
    {
      // 既存から探す
      for(u32 i=0; i<FS_DEBUG_LOG_ARC_FILE_INFO_ELEM_NUM_MAX; ++i)
      {
        // 後ろから探したほうが見付かり易いだろうから、後ろから探す
        u32 index = (m_fsDebugLogArcFileInfoArrayNextIndex + FS_DEBUG_LOG_ARC_FILE_INFO_ELEM_NUM_MAX - 1 - i) % FS_DEBUG_LOG_ARC_FILE_INFO_ELEM_NUM_MAX;  // 負にはならない
        if(index < m_fsDebugLogArcFileInfoElemTotalNum)
        {
          if(
                 m_fsDebugLogArcFileInfoArray[index].type  == type
              && m_fsDebugLogArcFileInfoArray[index].arcId == arcId
              && m_fsDebugLogArcFileInfoArray[index].datId == datId
          )
          {
            elem = &(m_fsDebugLogArcFileInfoArray[index]);
            break;
          }
        }
      }
    }
  }
  return elem;
}

void AsyncFileManager::calcFsDebugLogArcFileInfoOne(u32 reverseIndex)
{
  if(m_fsDebugLogArcFileInfoArray)
  {
    if(reverseIndex < FS_DEBUG_LOG_ARC_FILE_INFO_ELEM_NUM_MAX)
    {
      u32 index = (m_fsDebugLogArcFileInfoArrayNextIndex + FS_DEBUG_LOG_ARC_FILE_INFO_ELEM_NUM_MAX - 1 - reverseIndex) % FS_DEBUG_LOG_ARC_FILE_INFO_ELEM_NUM_MAX;  // 負にはならない
      if(index < m_fsDebugLogArcFileInfoElemTotalNum)
      {
        s64 fileFinish_Start       = 0;
        s64 fileFinish_Add         = 0;
        s64 uncompFinish_Start     = 0;
        s64 uncompFinish_Add       = 0;
        s64 uncompFinish_fileStart = 0;
        s64 uncompFinish_fileAdd   = 0;
            
        fileFinish_Start  = m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::FILE_FINISH  ] - m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::FILE_START   ];
        fileFinish_Add    = m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::FILE_FINISH  ] - m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::FILE_ADD     ];

        if(m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::UNCOMP_ADD   ] > 0)
        {
          uncompFinish_Start      = m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::UNCOMP_FINISH] - m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::UNCOMP_START ];
          uncompFinish_Add        = m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::UNCOMP_FINISH] - m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::UNCOMP_ADD   ];
          uncompFinish_fileStart  = m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::UNCOMP_FINISH] - m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::FILE_START   ];
          uncompFinish_fileAdd    = m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::UNCOMP_FINISH] - m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::FILE_ADD     ];
        }
        else
        {
          uncompFinish_fileStart = fileFinish_Start;
          uncompFinish_fileAdd   = fileFinish_Add;
        }

        m_fsDebugLogArcFileInfoArray[index].diffTime[FsDebugLogArcFileInfoTerm::FILE_FINISH_START       ] = static_cast<u32>(fileFinish_Start      );
        m_fsDebugLogArcFileInfoArray[index].diffTime[FsDebugLogArcFileInfoTerm::FILE_FINISH_ADD         ] = static_cast<u32>(fileFinish_Add        );
        m_fsDebugLogArcFileInfoArray[index].diffTime[FsDebugLogArcFileInfoTerm::UNCOMP_FINISH_START     ] = static_cast<u32>(uncompFinish_Start    );
        m_fsDebugLogArcFileInfoArray[index].diffTime[FsDebugLogArcFileInfoTerm::UNCOMP_FINISH_ADD       ] = static_cast<u32>(uncompFinish_Add      );
        m_fsDebugLogArcFileInfoArray[index].diffTime[FsDebugLogArcFileInfoTerm::UNCOMP_FINISH_FILE_START] = static_cast<u32>(uncompFinish_fileStart);
        m_fsDebugLogArcFileInfoArray[index].diffTime[FsDebugLogArcFileInfoTerm::UNCOMP_FINISH_FILE_ADD  ] = static_cast<u32>(uncompFinish_fileAdd  );
      }
    }
  }
}
#endif

void AsyncFileManager::PrintFsDebugLogArcFileInfo(void)
{
#if GFL_FS_DEBUG_LOG
  static const char* REQ_ELEM_TYPE_NAME[] =
  {
    "base",  // ELEM_BASE,
                                                               
    "filR",  // FILE_EASY_READ_REQ,                  // (req01)
    "filW",  // FILE_EASY_WRITE_REQ,                 // (req02)
    "open",  // ARC_FILE_OPEN_REQ,                   // (req03)
    "clos",  // ARC_FILE_CLOSE_REQ,                  // (req04)
    "load",  // ARC_FILE_LOAD_DATA_REQ,              // (req05)
    "loaB",  // ARC_FILE_LOAD_DATA_BUF_REQ,          // (req06)
    "lPiB",  // ARC_FILE_LOAD_DATA_PIECE_BUF_REQ,    // (req07)
    "sePt",  // ARC_FILE_SEEK_DATA_PTR_REQ,          // (req08)
    "lPtB",  // ARC_FILE_LOAD_DATA_PTR_BUF_REQ,      // (req09)
  };
  static const char* KIND_NAME[] =
  {
    "as",  // ASYNC,  // 非同期
    "sy",  // SYNC,   // 同期
    "an",  // ANY,    // 非同期、同期どちらか
  };

  this->CalcFsDebugLogArcFileInfo();

  if(m_fsDebugLogArcFileInfoArray)
  {
    GFL_PRINT("%s [micro second]\n", __FUNCTION__);
    GFL_PRINT("inde: ty type ki arc datId  compSize      size      fileAdd    fileStart   fileFinish      fileF-S    fileF-Add    uncompAdd  uncompStart uncompFinish    uncompF-S  uncompF-Add   uncoF-filS uncoF-filAdd\n");
    for(u32 i=0; i<FS_DEBUG_LOG_ARC_FILE_INFO_ELEM_NUM_MAX; ++i)
    {
      u32 index = (m_fsDebugLogArcFileInfoArrayNextIndex + i) % FS_DEBUG_LOG_ARC_FILE_INFO_ELEM_NUM_MAX;
      if(index < m_fsDebugLogArcFileInfoElemTotalNum)
      {
        //        "tIdx: typ na ki arcI datI comp size filAdd fStart fFinis fiF_S fiF_A uncADD uncSta uncFin unF_S unF_A uF_fS uF_fA\n"
        GFL_PRINT("%4ld: %2d %s %s %3ld %5ld %9ld %9ld %12lld %12lld %12lld %12ld %12ld %12lld %12lld %12lld %12ld %12ld %12ld %12ld\n",
            m_fsDebugLogArcFileInfoArray[index].totalIndex,
            m_fsDebugLogArcFileInfoArray[index].type,
            REQ_ELEM_TYPE_NAME[m_fsDebugLogArcFileInfoArray[index].type],
            KIND_NAME[m_fsDebugLogArcFileInfoArray[index].kind],
            m_fsDebugLogArcFileInfoArray[index].arcId,
            m_fsDebugLogArcFileInfoArray[index].datId,
            m_fsDebugLogArcFileInfoArray[index].compSize,
            m_fsDebugLogArcFileInfoArray[index].size,
            m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::FILE_ADD     ],
            m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::FILE_START   ],
            m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::FILE_FINISH  ],
            m_fsDebugLogArcFileInfoArray[index].diffTime[FsDebugLogArcFileInfoTerm::FILE_FINISH_START       ],
            m_fsDebugLogArcFileInfoArray[index].diffTime[FsDebugLogArcFileInfoTerm::FILE_FINISH_ADD         ],
            m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::UNCOMP_ADD   ],
            m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::UNCOMP_START ],
            m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::UNCOMP_FINISH],
            m_fsDebugLogArcFileInfoArray[index].diffTime[FsDebugLogArcFileInfoTerm::UNCOMP_FINISH_START     ],
            m_fsDebugLogArcFileInfoArray[index].diffTime[FsDebugLogArcFileInfoTerm::UNCOMP_FINISH_ADD       ],
            m_fsDebugLogArcFileInfoArray[index].diffTime[FsDebugLogArcFileInfoTerm::UNCOMP_FINISH_FILE_START],
            m_fsDebugLogArcFileInfoArray[index].diffTime[FsDebugLogArcFileInfoTerm::UNCOMP_FINISH_FILE_ADD  ]
        );
      }
    }
  }
#endif
}

const AsyncFileManager::FsDebugLogArcFileInfo* AsyncFileManager::GetFsDebugLogArcFileInfo(u32 reverseIndex) const
{
#if GFL_FS_DEBUG_LOG
  const FsDebugLogArcFileInfo* ret = NULL;
  if(m_fsDebugLogArcFileInfoArray)
  {
    if(reverseIndex < FS_DEBUG_LOG_ARC_FILE_INFO_ELEM_NUM_MAX)
    {
      u32 index = (m_fsDebugLogArcFileInfoArrayNextIndex + FS_DEBUG_LOG_ARC_FILE_INFO_ELEM_NUM_MAX - 1 - reverseIndex) % FS_DEBUG_LOG_ARC_FILE_INFO_ELEM_NUM_MAX;  // 負にはならない
      if(index < m_fsDebugLogArcFileInfoElemTotalNum)
      {
        ret = &(m_fsDebugLogArcFileInfoArray[index]);
      }
    }
  }
  return ret;
#else
  return NULL;
#endif
}

void AsyncFileManager::PrintFsDebugLogArcFileInfoBufLight(wchar_t* buf, size_t bufSize, u32 reverseIndex)
{
#if GFL_FS_DEBUG_LOG
  if(m_fsDebugLogArcFileInfoArray)
  {
    if(reverseIndex < FS_DEBUG_LOG_ARC_FILE_INFO_ELEM_NUM_MAX)
    {
      u32 index = (m_fsDebugLogArcFileInfoArrayNextIndex + FS_DEBUG_LOG_ARC_FILE_INFO_ELEM_NUM_MAX - 1 - reverseIndex) % FS_DEBUG_LOG_ARC_FILE_INFO_ELEM_NUM_MAX;  // 負にはならない
      if(index < m_fsDebugLogArcFileInfoElemTotalNum)
      {
        //::std::swprintf( buf, bufSize, L"%3ld:%1d(%3ld,%5ld)%7ld,%7ld(%7ld,%7ld,%7ld,%7ld)",
        //::std::swprintf( buf, bufSize, L"%3ld:%1d:%2ld,%4ld(%6ld,%6ld,%6ld,%6ld)",  CTR版で #181-D: argument is incompatible with corresponding format string conversion となるので下行の書き方に変えた
        ::std::swprintf( buf, bufSize, L"%3u:%1d:%2u,%4u(%6u,%6u,%6u,%6u)",
            m_fsDebugLogArcFileInfoArray[index].totalIndex,
            m_fsDebugLogArcFileInfoArray[index].type,
            m_fsDebugLogArcFileInfoArray[index].arcId,
            m_fsDebugLogArcFileInfoArray[index].datId,
            //m_fsDebugLogArcFileInfoArray[index].compSize,
            //m_fsDebugLogArcFileInfoArray[index].size,
            m_fsDebugLogArcFileInfoArray[index].diffTime[FsDebugLogArcFileInfoTerm::FILE_FINISH_START],
            m_fsDebugLogArcFileInfoArray[index].diffTime[FsDebugLogArcFileInfoTerm::UNCOMP_FINISH_START],
            m_fsDebugLogArcFileInfoArray[index].diffTime[FsDebugLogArcFileInfoTerm::UNCOMP_FINISH_FILE_START],
            m_fsDebugLogArcFileInfoArray[index].diffTime[FsDebugLogArcFileInfoTerm::UNCOMP_FINISH_FILE_ADD]
        );
      }
    }
  }
#endif
}
void AsyncFileManager::CalcFsDebugLogArcFileInfo(void)
{
#if GFL_FS_DEBUG_LOG
  if(m_fsDebugLogArcFileInfoArray)
  {
    for(u32 i=0; i<FS_DEBUG_LOG_ARC_FILE_INFO_ELEM_NUM_MAX; ++i)
    {
      u32 index = (m_fsDebugLogArcFileInfoArrayNextIndex + i) % FS_DEBUG_LOG_ARC_FILE_INFO_ELEM_NUM_MAX;
      if(index < m_fsDebugLogArcFileInfoElemTotalNum)
      {
        s64 fileFinish_Start       = 0;
        s64 fileFinish_Add         = 0;
        s64 uncompFinish_Start     = 0;
        s64 uncompFinish_Add       = 0;
        s64 uncompFinish_fileStart = 0;
        s64 uncompFinish_fileAdd   = 0;
            
        fileFinish_Start  = m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::FILE_FINISH  ] - m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::FILE_START   ];
        fileFinish_Add    = m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::FILE_FINISH  ] - m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::FILE_ADD     ];

        if(m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::UNCOMP_ADD   ] > 0)
        {
          uncompFinish_Start      = m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::UNCOMP_FINISH] - m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::UNCOMP_START ];
          uncompFinish_Add        = m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::UNCOMP_FINISH] - m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::UNCOMP_ADD   ];
          uncompFinish_fileStart  = m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::UNCOMP_FINISH] - m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::FILE_START   ];
          uncompFinish_fileAdd    = m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::UNCOMP_FINISH] - m_fsDebugLogArcFileInfoArray[index].time[FsDebugLogArcFileInfoStep::FILE_ADD     ];
        }
        else
        {
          uncompFinish_fileStart = fileFinish_Start;
          uncompFinish_fileAdd   = fileFinish_Add;
        }

        m_fsDebugLogArcFileInfoArray[index].diffTime[FsDebugLogArcFileInfoTerm::FILE_FINISH_START       ] = static_cast<u32>(fileFinish_Start      );
        m_fsDebugLogArcFileInfoArray[index].diffTime[FsDebugLogArcFileInfoTerm::FILE_FINISH_ADD         ] = static_cast<u32>(fileFinish_Add        );
        m_fsDebugLogArcFileInfoArray[index].diffTime[FsDebugLogArcFileInfoTerm::UNCOMP_FINISH_START     ] = static_cast<u32>(uncompFinish_Start    );
        m_fsDebugLogArcFileInfoArray[index].diffTime[FsDebugLogArcFileInfoTerm::UNCOMP_FINISH_ADD       ] = static_cast<u32>(uncompFinish_Add      );
        m_fsDebugLogArcFileInfoArray[index].diffTime[FsDebugLogArcFileInfoTerm::UNCOMP_FINISH_FILE_START] = static_cast<u32>(uncompFinish_fileStart);
        m_fsDebugLogArcFileInfoArray[index].diffTime[FsDebugLogArcFileInfoTerm::UNCOMP_FINISH_FILE_ADD  ] = static_cast<u32>(uncompFinish_fileAdd  );
      }
    }
  }
#endif
}
u32 AsyncFileManager::GetFsDebugLogArcFileInfoNum(void) const
{
  u32 num = 0;
#if GFL_FS_DEBUG_LOG
  if(m_fsDebugLogArcFileInfoArray)
  {
    if(m_fsDebugLogArcFileInfoElemTotalNum < FS_DEBUG_LOG_ARC_FILE_INFO_ELEM_NUM_MAX)
    {
      num = m_fsDebugLogArcFileInfoElemTotalNum;
    }
    else
    {
      num = FS_DEBUG_LOG_ARC_FILE_INFO_ELEM_NUM_MAX;
    }
  }
#endif
  return num;
}
void AsyncFileManager::ClearFsDebugLogArcFileInfo(void)
{
#if GFL_FS_DEBUG_LOG
  if(m_fsDebugLogArcFileInfoArray)
  {
    m_fsDebugLogArcFileInfoElemTotalNum = 0;
    m_fsDebugLogArcFileInfoArrayNextIndex = 0;
  }
#endif
}



#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
AsyncFileManager::MatchCheckReqElem::MatchCheckReqElem(void)
  : ElemBase(), 
    m_reqType(ReqElemType::ELEM_BASE),
    m_keyType(KEY_TYPE_PP_BUF),  // てきとーな値で初期化しています
    m_arcInfoType(ARC_INFO_TYPE_NONE),
    m_isFinishedState(IS_FINISHED_STATE_NOT_CALL),
    m_isArcFileClosed(0)
{
  m_key.arcId     = ArcFile::ARCID_NULL;
  m_arcInfo.arcId = ArcFile::ARCID_NULL;
}
AsyncFileManager::MatchCheckReqElem::~MatchCheckReqElem()
{}

void AsyncFileManager::MatchCheckReqElem::SetFileEasyReadReq           (                         void**         ppBuf   )
{
  m_reqType = ReqElemType::FILE_EASY_READ_REQ;
  this->initKey(ppBuf);
  this->initArcInfo();
}
void AsyncFileManager::MatchCheckReqElem::SetFileEasyWriteReq          (                         const char*    fileName)
{
  m_reqType = ReqElemType::FILE_EASY_WRITE_REQ;
  this->initKey(fileName);
  this->initArcInfo();
}
void AsyncFileManager::MatchCheckReqElem::SetArcFileOpenReq            (                         const char*    fileName)
{
  m_reqType = ReqElemType::ARC_FILE_OPEN_REQ;
  this->initKey(fileName);
  this->initArcInfo(fileName);
}
void AsyncFileManager::MatchCheckReqElem::SetArcFileOpenReq            (                         ArcFile::ARCID arcId   )
{
  m_reqType = ReqElemType::ARC_FILE_OPEN_REQ;
  this->initKey(arcId);
  this->initArcInfo(arcId);
}
void AsyncFileManager::MatchCheckReqElem::SetArcFileCloseReq           (                         const char*    fileName)
{
  m_reqType = ReqElemType::ARC_FILE_CLOSE_REQ;
  this->initKey(fileName);
  this->initArcInfo(fileName);
}
void AsyncFileManager::MatchCheckReqElem::SetArcFileCloseReq           (                         ArcFile::ARCID arcId   )
{
  m_reqType = ReqElemType::ARC_FILE_CLOSE_REQ;
  this->initKey(arcId);
  this->initArcInfo(arcId);
}
void AsyncFileManager::MatchCheckReqElem::SetArcFileLoadDataReq        (const char*    fileName, void**         ppBuf   )
{
  m_reqType = ReqElemType::ARC_FILE_LOAD_DATA_REQ;
  this->initKey(ppBuf);
  this->initArcInfo(fileName);
}
void AsyncFileManager::MatchCheckReqElem::SetArcFileLoadDataReq        (ArcFile::ARCID arcId   , void**         ppBuf   )
{
  m_reqType = ReqElemType::ARC_FILE_LOAD_DATA_REQ;
  this->initKey(ppBuf);
  this->initArcInfo(arcId);
}
void AsyncFileManager::MatchCheckReqElem::SetArcFileLoadDataBufReq     (const char*    fileName, const void*     pBuf    )
{
  m_reqType = ReqElemType::ARC_FILE_LOAD_DATA_BUF_REQ;
  this->initKey(pBuf);
  this->initArcInfo(fileName);
}
void AsyncFileManager::MatchCheckReqElem::SetArcFileLoadDataBufReq     (ArcFile::ARCID arcId   , const void*     pBuf    )
{                                                                                                              
  m_reqType = ReqElemType::ARC_FILE_LOAD_DATA_BUF_REQ;                                                         
  this->initKey(pBuf);
  this->initArcInfo(arcId);
}                                                                                                              
void AsyncFileManager::MatchCheckReqElem::SetArcFileLoadDataPieceBufReq(const char*    fileName, const void*     pBuf    )
{                                                                                                              
  m_reqType = ReqElemType::ARC_FILE_LOAD_DATA_PIECE_BUF_REQ;                                                   
  this->initKey(pBuf);
  this->initArcInfo(fileName);
}                                                                                                              
void AsyncFileManager::MatchCheckReqElem::SetArcFileLoadDataPieceBufReq(ArcFile::ARCID arcId   , const void*     pBuf    )
{
  m_reqType = ReqElemType::ARC_FILE_LOAD_DATA_PIECE_BUF_REQ;
  this->initKey(pBuf);
  this->initArcInfo(arcId);
}
void AsyncFileManager::MatchCheckReqElem::SetArcFileSeekDataPtrReq     (                         const char*    fileName)
{
  m_reqType = ReqElemType::ARC_FILE_SEEK_DATA_PTR_REQ;
  this->initKey(fileName);
  this->initArcInfo(fileName);
}
void AsyncFileManager::MatchCheckReqElem::SetArcFileSeekDataPtrReq     (                         ArcFile::ARCID arcId   )
{
  m_reqType = ReqElemType::ARC_FILE_SEEK_DATA_PTR_REQ;
  this->initKey(arcId);
  this->initArcInfo(arcId);
}
void AsyncFileManager::MatchCheckReqElem::SetArcFileLoadDataPtrBufReq  (const char*    fileName, const void*     pBuf    )
{                                                                                                              
  m_reqType = ReqElemType::ARC_FILE_LOAD_DATA_PTR_BUF_REQ;                                                     
  this->initKey(pBuf);
  this->initArcInfo(fileName);
}                                                                                                              
void AsyncFileManager::MatchCheckReqElem::SetArcFileLoadDataPtrBufReq  (ArcFile::ARCID arcId   , const void*     pBuf    )
{
  m_reqType = ReqElemType::ARC_FILE_LOAD_DATA_PTR_BUF_REQ;
  this->initKey(pBuf);
  this->initArcInfo(arcId);
}

bool AsyncFileManager::MatchCheckReqElem::IsSameAsFileEasyReadReq           (void**         ppBuf   ) const
{
  if(m_reqType == ReqElemType::FILE_EASY_READ_REQ)
  {
    return this->isSameKey(ppBuf);
  }
  return false;
}
bool AsyncFileManager::MatchCheckReqElem::IsSameAsFileEasyWriteReq          (const char*    fileName) const
{
  if(m_reqType == ReqElemType::FILE_EASY_WRITE_REQ)
  {
    return this->isSameKey(fileName);
  }
  return false;
}
bool AsyncFileManager::MatchCheckReqElem::IsSameAsArcFileOpenReq            (const char*    fileName) const
{
  if(m_reqType == ReqElemType::ARC_FILE_OPEN_REQ)
  {
    return this->isSameKey(fileName);
  }
  return false;
}
bool AsyncFileManager::MatchCheckReqElem::IsSameAsArcFileOpenReq            (ArcFile::ARCID arcId   ) const
{
  if(m_reqType == ReqElemType::ARC_FILE_OPEN_REQ)
  {
    return this->isSameKey(arcId);
  }
  return false;
}
bool AsyncFileManager::MatchCheckReqElem::IsSameAsArcFileCloseReq           (const char*    fileName) const
{
  if(m_reqType == ReqElemType::ARC_FILE_CLOSE_REQ)
  {
    return this->isSameKey(fileName);
  }
  return false;
}
bool AsyncFileManager::MatchCheckReqElem::IsSameAsArcFileCloseReq           (ArcFile::ARCID arcId   ) const
{
  if(m_reqType == ReqElemType::ARC_FILE_CLOSE_REQ)
  {
    return this->isSameKey(arcId);
  }
  return false;
}
bool AsyncFileManager::MatchCheckReqElem::IsSameAsArcFileLoadDataReq        (void**         ppBuf   ) const
{
  if(m_reqType == ReqElemType::ARC_FILE_LOAD_DATA_REQ)
  {
    return this->isSameKey(ppBuf);
  }
  return false;
}
bool AsyncFileManager::MatchCheckReqElem::IsSameAsArcFileLoadDataBufReq     (const void*     pBuf    ) const
{                                                                                          
  if(m_reqType == ReqElemType::ARC_FILE_LOAD_DATA_BUF_REQ)
  {
    return this->isSameKey(pBuf);
  }
  return false;                                                                            
}                                                                                          
bool AsyncFileManager::MatchCheckReqElem::IsSameAsArcFileLoadDataPieceBufReq(const void*     pBuf    ) const
{
  if(m_reqType == ReqElemType::ARC_FILE_LOAD_DATA_PIECE_BUF_REQ)
  {
    return this->isSameKey(pBuf);
  }
  return false;
}
bool AsyncFileManager::MatchCheckReqElem::IsSameAsArcFileSeekDataPtrReq     (const char*    fileName) const
{
  if(m_reqType == ReqElemType::ARC_FILE_SEEK_DATA_PTR_REQ)
  {
    return this->isSameKey(fileName);
  }
  return false;
}
bool AsyncFileManager::MatchCheckReqElem::IsSameAsArcFileSeekDataPtrReq     (ArcFile::ARCID arcId   ) const
{
  if(m_reqType == ReqElemType::ARC_FILE_SEEK_DATA_PTR_REQ)
  {
    return this->isSameKey(arcId);
  }
  return false;
}
bool AsyncFileManager::MatchCheckReqElem::IsSameAsArcFileLoadDataPtrBufReq  (const void*     pBuf    ) const
{
  if(m_reqType == ReqElemType::ARC_FILE_LOAD_DATA_PTR_BUF_REQ)
  {
    return this->isSameKey(pBuf);
  }
  return false;
}

bool AsyncFileManager::MatchCheckReqElem::IsSameArcInfo(bool           /*dummy*/   ) const
{
  return true;  // 必ずtrueを返す
}
bool AsyncFileManager::MatchCheckReqElem::IsSameArcInfo(const char*    fileName) const
{
  if(m_arcInfoType == ARC_INFO_TYPE_FILE_NAME)
  {
    if( Str::CheckFileNameIsSameStrictly(m_key.fileName, fileName) ) return true;
  }
  return false;
}
bool AsyncFileManager::MatchCheckReqElem::IsSameArcInfo(ArcFile::ARCID arcId   ) const
{
  if(m_arcInfoType == ARC_INFO_TYPE_ARC_ID)
  {
    if( m_arcInfo.arcId == arcId ) return true;
  }
  return false;
}

u32 AsyncFileManager::MatchCheckReqElem::GetArcFileCount(bool           /*dummy*/   , const AsyncFileManager* /*asyncFileManager*/) const
{
  return 0;  // 必ず0を返す
}
u32 AsyncFileManager::MatchCheckReqElem::GetArcFileCount(const char*    fileName, const AsyncFileManager* asyncFileManager) const
{
  return asyncFileManager->GetArcFileCount(fileName);
}
u32 AsyncFileManager::MatchCheckReqElem::GetArcFileCount(ArcFile::ARCID arcId   , const AsyncFileManager* asyncFileManager) const
{
  return asyncFileManager->GetArcFileCount(arcId);
}

bool AsyncFileManager::MatchCheckReqElem::IsReqTypeArcFileClose(bool           /*dummy*/   ) const
{
  return false;  // 必ずfalseを返す
}
bool AsyncFileManager::MatchCheckReqElem::IsReqTypeArcFileClose(const char*    /*fileName*/) const
{
  // fileNameで判別していないのでしたほうがいいかも。
  return (m_reqType == ReqElemType::ARC_FILE_CLOSE_REQ);
}
bool AsyncFileManager::MatchCheckReqElem::IsReqTypeArcFileClose(ArcFile::ARCID /*arcId*/   ) const
{
  // arcIdで判別していないのでしたほうがいいかも。
  return (m_reqType == ReqElemType::ARC_FILE_CLOSE_REQ);
}
//bool AsyncFileManager::MatchCheckReqElem::IsReqTypeArcFileClose(void) const
//{
//  // どのArcFileに対するクローズリクエストであるかの判別をしない場合。
//  return (m_reqType == ReqElemType::ARC_FILE_CLOSE_REQ);
//}

void AsyncFileManager::MatchCheckReqElem::SetIsFinished(bool isFinished)
{
  if(isFinished)
  {
    m_isFinishedState = IS_FINISHED_STATE_TRUE;
  }
  else
  {
    m_isFinishedState = IS_FINISHED_STATE_FALSE;
  }
}

void AsyncFileManager::MatchCheckReqElem::GetInfoMes(char* mes, const u32 mesBufSize)
{
  switch(m_reqType)
  {
  case ReqElemType::FILE_EASY_READ_REQ:
    {
      GFL_FS_STR_SPRINTF(mes, mesBufSize, "FsMatchCheck: FileEasyRead ppBuf=0x%X [%d]\n", m_key.ppBuf, m_isFinishedState);
      break;
    }
  case ReqElemType::FILE_EASY_WRITE_REQ:
    {
      GFL_FS_STR_SPRINTF(mes, mesBufSize, "FsMatchCheck: FileEasyWrite fileName=%s [%d]\n", m_key.fileName, m_isFinishedState);
      break;
    }
  case ReqElemType::ARC_FILE_OPEN_REQ:
    {
      if(m_keyType == KEY_TYPE_ARC_ID)
      {
        GFL_FS_STR_SPRINTF(mes, mesBufSize, "FsMatchCheck: ArcFileOpen arcId=%u [%d]\n", m_key.arcId, m_isFinishedState);
      }
      else
      {
        GFL_FS_STR_SPRINTF(mes, mesBufSize, "FsMatchCheck: ArcFileOpen fileName=%s [%d]\n", m_key.fileName, m_isFinishedState);
      }
      break;
    }
  case ReqElemType::ARC_FILE_CLOSE_REQ:
    {
      if(m_keyType == KEY_TYPE_ARC_ID)
      {
        GFL_FS_STR_SPRINTF(mes, mesBufSize, "FsMatchCheck: ArcFileClose arcId=%u [%d]\n", m_key.arcId, m_isFinishedState);
      }
      else
      {
        GFL_FS_STR_SPRINTF(mes, mesBufSize, "FsMatchCheck: ArcFileClose fileName=%s [%d]\n", m_key.fileName, m_isFinishedState);
      }
      break;
    }
  case ReqElemType::ARC_FILE_LOAD_DATA_REQ:
    {
      if(m_arcInfoType == ARC_INFO_TYPE_ARC_ID)
      {
        GFL_FS_STR_SPRINTF(mes, mesBufSize, "FsMatchCheck: ArcFileLoadData ppBuf=0x%X arcId=%u [%d]\n", m_key.ppBuf, m_arcInfo.arcId, m_isFinishedState);
      }
      else
      {
        GFL_FS_STR_SPRINTF(mes, mesBufSize, "FsMatchCheck: ArcFileLoadData ppBuf=0x%X fileName=%s [%d]\n", m_key.ppBuf, m_arcInfo.fileName, m_isFinishedState);
      }
      break;
    }
  case ReqElemType::ARC_FILE_LOAD_DATA_BUF_REQ:
    {
      if(m_arcInfoType == ARC_INFO_TYPE_ARC_ID)
      {
        GFL_FS_STR_SPRINTF(mes, mesBufSize, "FsMatchCheck: ArcFileLoadDataBuf pBuf=0x%X arcId=%u [%d]\n", m_key.pBuf, m_arcInfo.arcId, m_isFinishedState);
      }
      else
      {
        GFL_FS_STR_SPRINTF(mes, mesBufSize, "FsMatchCheck: ArcFileLoadDataBuf pBuf=0x%X fileName=%s [%d]\n", m_key.pBuf, m_arcInfo.fileName, m_isFinishedState);
      }
      break;
    }
  case ReqElemType::ARC_FILE_LOAD_DATA_PIECE_BUF_REQ:
    {
      if(m_arcInfoType == ARC_INFO_TYPE_ARC_ID)
      {
        GFL_FS_STR_SPRINTF(mes, mesBufSize, "FsMatchCheck: ArcFileLoadDataPieceBuf pBuf=0x%X arcId=%u [%d]\n", m_key.pBuf, m_arcInfo.arcId, m_isFinishedState);
      }
      else
      {
        GFL_FS_STR_SPRINTF(mes, mesBufSize, "FsMatchCheck: ArcFileLoadDataPieceBuf pBuf=0x%X fileName=%s [%d]\n", m_key.pBuf, m_arcInfo.fileName, m_isFinishedState);
      }
      break;
    }
  case ReqElemType::ARC_FILE_SEEK_DATA_PTR_REQ:
    {
      if(m_keyType == KEY_TYPE_ARC_ID)
      {
        GFL_FS_STR_SPRINTF(mes, mesBufSize, "FsMatchCheck: ArcFileSeekDataPtr arcId=%u [%d]\n", m_key.arcId, m_isFinishedState);
      }
      else
      {
        GFL_FS_STR_SPRINTF(mes, mesBufSize, "FsMatchCheck: ArcFileSeekDataPtr fileName=%s [%d]\n", m_key.fileName, m_isFinishedState);
      }
      break;
    }
  case ReqElemType::ARC_FILE_LOAD_DATA_PTR_BUF_REQ:
    {
      if(m_arcInfoType == ARC_INFO_TYPE_ARC_ID)
      {
        GFL_FS_STR_SPRINTF(mes, mesBufSize, "FsMatchCheck: ArcFileLoadDataPtrBuf pBuf=0x%X arcId=%u [%d]\n", m_key.pBuf, m_arcInfo.arcId, m_isFinishedState);
      }
      else
      {
        GFL_FS_STR_SPRINTF(mes, mesBufSize, "FsMatchCheck: ArcFileLoadDataPtrBuf pBuf=0x%X fileName=%s [%d]\n", m_key.pBuf, m_arcInfo.fileName, m_isFinishedState);
      }
      break;
    }
  }
}

void AsyncFileManager::MatchCheckReqElem::initKey(void**         ppBuf   )
{
  m_keyType         = KEY_TYPE_PP_BUF;
  m_key.ppBuf       = ppBuf;
  m_isFinishedState = IS_FINISHED_STATE_NOT_CALL;
}
void AsyncFileManager::MatchCheckReqElem::initKey(const void*     pBuf    )
{
  m_keyType         = KEY_TYPE_P_BUF;
  m_key.pBuf        = pBuf;
  m_isFinishedState = IS_FINISHED_STATE_NOT_CALL;
}
void AsyncFileManager::MatchCheckReqElem::initKey(const char*    fileName)
{
  m_keyType         = KEY_TYPE_FILE_NAME;
  m_key.fileName    = fileName;
  m_isFinishedState = IS_FINISHED_STATE_NOT_CALL;
}
void AsyncFileManager::MatchCheckReqElem::initKey(ArcFile::ARCID arcId   )
{
  m_keyType         = KEY_TYPE_ARC_ID;
  m_key.arcId       = arcId;
  m_isFinishedState = IS_FINISHED_STATE_NOT_CALL;
}

void AsyncFileManager::MatchCheckReqElem::initArcInfo(void                   )
{
  m_arcInfoType      = ARC_INFO_TYPE_NONE;
  m_arcInfo.arcId    = ArcFile::ARCID_NULL;
}
void AsyncFileManager::MatchCheckReqElem::initArcInfo(const char*    fileName)
{
  m_arcInfoType      = ARC_INFO_TYPE_FILE_NAME;
  m_arcInfo.fileName = fileName;
}
void AsyncFileManager::MatchCheckReqElem::initArcInfo(ArcFile::ARCID arcId   )
{
  m_arcInfoType      = ARC_INFO_TYPE_ARC_ID;
  m_arcInfo.arcId    = arcId;
}

bool AsyncFileManager::MatchCheckReqElem::isSameKey(void**         ppBuf   ) const
{
  if( m_keyType == KEY_TYPE_PP_BUF)
  {
    if( m_key.ppBuf == ppBuf ) return true;
  }
  return false; 
}
bool AsyncFileManager::MatchCheckReqElem::isSameKey(const void*     pBuf    ) const
{
  if( m_keyType == KEY_TYPE_P_BUF)
  {
    if( m_key.pBuf == pBuf ) return true;
  }
  return false;
}
bool AsyncFileManager::MatchCheckReqElem::isSameKey(const char*    fileName) const
{
  if( m_keyType == KEY_TYPE_FILE_NAME)
  {
    if( Str::CheckFileNameIsSameStrictly(m_key.fileName, fileName) ) return true;
  }
  return false;
}
bool AsyncFileManager::MatchCheckReqElem::isSameKey(ArcFile::ARCID arcId   ) const
{
  if( m_keyType == KEY_TYPE_ARC_ID)
  {
    if( m_key.arcId == arcId ) return true;
  }
  return false;
}




const u32 AsyncFileManager::MatchCheckManager::REQ_MEM_MANAGE_ARRAY_SIZE = 7;
const u32 AsyncFileManager::MatchCheckManager::REQ_NUM_MAX = 32 * REQ_MEM_MANAGE_ARRAY_SIZE;  // 32*7=224個

AsyncFileManager::MatchCheckManager::MatchCheckManager(gfl2::heap::HeapBase* heap, const AsyncFileManager* asyncFileManager)
  : m_checkFlag(false),
    m_announceLevel(ANNOUNCE_LEVEL_PRINT),
    m_heap(heap),
    m_asyncFileManager(asyncFileManager),
    m_matchCheckReqList(NULL),
    m_reqMemBitManage(NULL),
    m_reqMemTopAddr(NULL),
    m_currReqNum(0),
    m_reqNumBiggest(0)
{
  if(m_heap)
  {
    m_matchCheckReqList = GFL_NEW(heap) List();

    m_reqMemBitManage = GFL_NEW_ARRAY(heap) u32[REQ_MEM_MANAGE_ARRAY_SIZE];
    gfl2::std::MemClear(m_reqMemBitManage, sizeof(u32)*REQ_MEM_MANAGE_ARRAY_SIZE);
    
    m_reqMemTopAddr = GflHeapAllocMemory(heap, sizeof(MatchCheckReqElem) * REQ_NUM_MAX);

    //heap->Dump();
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  }
}
AsyncFileManager::MatchCheckManager::~MatchCheckManager()
{
  if(m_matchCheckReqList)
  {
    ElemBase* elem = m_matchCheckReqList->GetHead();
    while(elem)
    {
      ElemBase* nextElem = elem->GetNext();
      this->destroyMatchCheckReqElem(static_cast<MatchCheckReqElem*>(elem));
      elem = nextElem;
    }

    GflHeapSafeFreeMemory(m_reqMemTopAddr);
    GFL_SAFE_DELETE_ARRAY(m_reqMemBitManage);
    GFL_SAFE_DELETE(m_matchCheckReqList);
  }
}

void AsyncFileManager::MatchCheckManager::SetAddFileEasyReadReq           (const FileEasyReadReq&            req)
{
  MatchCheckReqElem* elem = this->createMatchCheckReqElem();
  if(elem)
  {
    elem->SetFileEasyReadReq(req.ppBuf);
  }
}
void AsyncFileManager::MatchCheckManager::SetAddFileEasyWriteReq          (const FileEasyWriteReq&           req)
{
  MatchCheckReqElem* elem = this->createMatchCheckReqElem();
  if(elem)
  {
    elem->SetFileEasyWriteReq(req.fileName);
  }
}
void AsyncFileManager::MatchCheckManager::SetAddArcFileOpenReq            (const ArcFileOpenReq&             req)
{
  // ArcFileのオープンリクエストの場合、同じArcFileに対するクローズリクエストのIs...Finishedが済んでいるか、を確認する
  this->checkRemainingArcFileCloseReqElemAtAddArcFileOpenReq(req);

  MatchCheckReqElem* elem = this->createMatchCheckReqElem();
  if(elem)
  {
    if(req.arcId != ArcFile::ARCID_NULL)
    {
      elem->SetArcFileOpenReq(req.arcId);
    }
    else
    {
      elem->SetArcFileOpenReq(req.fileName);
    }
  }
}
void AsyncFileManager::MatchCheckManager::SetAddArcFileCloseReq           (const ArcFileCloseReq&            req)
{
  MatchCheckReqElem* elem = this->createMatchCheckReqElem();
  if(elem)
  {
    if(req.arcId != ArcFile::ARCID_NULL)
    {
      elem->SetArcFileCloseReq(req.arcId);
    }
    else
    {
      elem->SetArcFileCloseReq(req.fileName);
    }
  }
}
void AsyncFileManager::MatchCheckManager::SetAddArcFileLoadDataReq        (const ArcFileLoadDataReq&         req)
{
  MatchCheckReqElem* elem = this->createMatchCheckReqElem();
  if(elem)
  {
    if(req.arcId != ArcFile::ARCID_NULL)
    {
      elem->SetArcFileLoadDataReq(req.arcId, req.ppBuf);
    }
    else
    {
      elem->SetArcFileLoadDataReq(req.fileName, req.ppBuf);
    }
  }
}
void AsyncFileManager::MatchCheckManager::SetAddArcFileLoadDataBufReq     (const ArcFileLoadDataBufReq&      req)
{
  MatchCheckReqElem* elem = this->createMatchCheckReqElem();
  if(elem)
  {
    if(req.arcId != ArcFile::ARCID_NULL)
    {
      elem->SetArcFileLoadDataBufReq(req.arcId, req.pBuf);
    }
    else
    {
      elem->SetArcFileLoadDataBufReq(req.fileName, req.pBuf);
    }
  }
}
void AsyncFileManager::MatchCheckManager::SetAddArcFileLoadDataPieceBufReq(const ArcFileLoadDataPieceBufReq& req)
{
  MatchCheckReqElem* elem = this->createMatchCheckReqElem();
  if(elem)
  {
    if(req.arcId != ArcFile::ARCID_NULL)
    {
      elem->SetArcFileLoadDataPieceBufReq(req.arcId, req.pBuf);
    }
    else
    {
      elem->SetArcFileLoadDataPieceBufReq(req.fileName, req.pBuf);
    }
  }
}
void AsyncFileManager::MatchCheckManager::SetAddArcFileSeekDataPtrReq     (const ArcFileSeekDataPtrReq&      req)
{
  MatchCheckReqElem* elem = this->createMatchCheckReqElem();
  if(elem)
  {
    if(req.arcId != ArcFile::ARCID_NULL)
    {
      elem->SetArcFileSeekDataPtrReq(req.arcId);
    }
    else
    {
      elem->SetArcFileSeekDataPtrReq(req.fileName);
    }
  }
}
void AsyncFileManager::MatchCheckManager::SetAddArcFileLoadDataPtrBufReq  (const ArcFileLoadDataPtrBufReq&   req)
{
  MatchCheckReqElem* elem = this->createMatchCheckReqElem();
  if(elem)
  {
    if(req.arcId != ArcFile::ARCID_NULL)
    {
      elem->SetArcFileLoadDataPtrBufReq(req.arcId, req.pBuf);
    }
    else
    {
      elem->SetArcFileLoadDataPtrBufReq(req.fileName, req.pBuf);
    }
  }
}

bool AsyncFileManager::MatchCheckManager::SetIsFileEasyReadFinished           (bool isFinished      , void**         ppBuf   )
{
  return setIsFinishedTemplate<void**>        (isFinished, ppBuf   , &MatchCheckReqElem::IsSameAsFileEasyReadReq           , false, false, this, &MatchCheckManager::destroyMatchCheckReqElemAtFinish);
}
bool AsyncFileManager::MatchCheckManager::SetIsFileEasyWriteFinished          (bool isFinished      , const char*    fileName)
{
  return setIsFinishedTemplate<const char*>   (isFinished, fileName, &MatchCheckReqElem::IsSameAsFileEasyWriteReq          , false, false, this, &MatchCheckManager::destroyMatchCheckReqElemAtFinish);
}
bool AsyncFileManager::MatchCheckManager::SetIsArcFileOpenFinished            (bool isFinished      , const char*    fileName)
{
  return setIsFinishedTemplate<const char*>   (isFinished, fileName, &MatchCheckReqElem::IsSameAsArcFileOpenReq            , false, false, this, &MatchCheckManager::destroyMatchCheckReqElemAtFinish);
}
bool AsyncFileManager::MatchCheckManager::SetIsArcFileOpenFinished            (bool isFinished      , ArcFile::ARCID arcId   )
{
  return setIsFinishedTemplate<ArcFile::ARCID>(isFinished, arcId   , &MatchCheckReqElem::IsSameAsArcFileOpenReq            , false, false, this, &MatchCheckManager::destroyMatchCheckReqElemAtFinish);
}
bool AsyncFileManager::MatchCheckManager::SetIsArcFileCloseFinished           (bool isFinished      , const char*    fileName)
{
  return setIsFinishedTemplate<const char*>   (isFinished, fileName, &MatchCheckReqElem::IsSameAsArcFileCloseReq           , true , true , this, &MatchCheckManager::destroyMatchCheckReqElemAtFinish);
}
bool AsyncFileManager::MatchCheckManager::SetIsArcFileCloseFinished           (bool isFinished      , ArcFile::ARCID arcId   )
{
  return setIsFinishedTemplate<ArcFile::ARCID>(isFinished, arcId   , &MatchCheckReqElem::IsSameAsArcFileCloseReq           , true , true , this, &MatchCheckManager::destroyMatchCheckReqElemAtFinish);
}
bool AsyncFileManager::MatchCheckManager::SetIsArcFileLoadDataFinished        (bool isFinished      , void**         ppBuf   )
{
  return setIsFinishedTemplate<void**>        (isFinished, ppBuf   , &MatchCheckReqElem::IsSameAsArcFileLoadDataReq        , false, false, this, &MatchCheckManager::destroyMatchCheckReqElemAtFinish);
}
bool AsyncFileManager::MatchCheckManager::SetIsArcFileLoadDataBufFinished     (bool isFinished      , const void*    pBuf    )
{                                                                                                                   
  return setIsFinishedTemplate<const void*>   (isFinished, pBuf    , &MatchCheckReqElem::IsSameAsArcFileLoadDataBufReq     , false, false, this, &MatchCheckManager::destroyMatchCheckReqElemAtFinish);
}                                                                                                                   
bool AsyncFileManager::MatchCheckManager::SetIsArcFileLoadDataPieceBufFinished(bool isFinished      , const void*    pBuf    )
{
  return setIsFinishedTemplate<const void*>   (isFinished, pBuf    , &MatchCheckReqElem::IsSameAsArcFileLoadDataPieceBufReq, false, false, this, &MatchCheckManager::destroyMatchCheckReqElemAtFinish);
}
bool AsyncFileManager::MatchCheckManager::SetIsArcFileSeekDataPtrFinished     (bool isFinished      , const char*    fileName)
{
  return setIsFinishedTemplate<const char*>   (isFinished, fileName, &MatchCheckReqElem::IsSameAsArcFileSeekDataPtrReq     , false, false, this, &MatchCheckManager::destroyMatchCheckReqElemAtFinish);
}
bool AsyncFileManager::MatchCheckManager::SetIsArcFileSeekDataPtrFinished     (bool isFinished      , ArcFile::ARCID arcId   )
{
  return setIsFinishedTemplate<ArcFile::ARCID>(isFinished, arcId   , &MatchCheckReqElem::IsSameAsArcFileSeekDataPtrReq     , false, false, this, &MatchCheckManager::destroyMatchCheckReqElemAtFinish);
}
bool AsyncFileManager::MatchCheckManager::SetIsArcFileLoadDataPtrBufFinished  (bool isFinished      , const void*    pBuf    )
{
  return setIsFinishedTemplate<const void*>   (isFinished, pBuf    , &MatchCheckReqElem::IsSameAsArcFileLoadDataPtrBufReq  , false, false, this, &MatchCheckManager::destroyMatchCheckReqElemAtFinish);
}
bool AsyncFileManager::MatchCheckManager::SetIsAllReqFinished                 (bool isFinished                               )
{
  // 全削除する
  if(isFinished)
  {
    GFL_PRINT("FsMatchCheck: AsyncFileManager::IsAllReqFinishedが呼ばれtrueになったので、\n");  // ClearRemainingReqElem関数に文章が続く。
    this->ClearRemainingReqElem();
  }
  return true;  // Allは特殊な事例なので何を返せばいいか迷うところだが、取り敢えずtrueを返しておく。
}

void AsyncFileManager::MatchCheckManager::SetArcFileClosedFlagOn(const char*    fileName)
{
  if(m_checkFlag)
  { 
    ElemBase* elem = m_matchCheckReqList->GetTail();  // 後ろから探す
    while(elem)
    {
      MatchCheckReqElem* reqElem = static_cast<MatchCheckReqElem*>(elem);
      if( reqElem->IsSameArcInfo(fileName) )
      {
        if( reqElem->IsReqTypeArcFileClose(fileName) )
        {
          if( !reqElem->GetArcFileClosedFlag() )  // 後ろにいるフラグが立っていないものを探しフラグを立てる。
          {
            reqElem->SetArcFileClosedFlag(true);
            break;
          }
        }
      }
      elem = elem->GetPrev();
    }
  }
}
void AsyncFileManager::MatchCheckManager::SetArcFileClosedFlagOn(ArcFile::ARCID arcId   )
{
  if(m_checkFlag)
  { 
    ElemBase* elem = m_matchCheckReqList->GetTail();  // 後ろから探す
    while(elem)
    {
      MatchCheckReqElem* reqElem = static_cast<MatchCheckReqElem*>(elem);
      if( reqElem->IsSameArcInfo(arcId) )
      {
        if( reqElem->IsReqTypeArcFileClose(arcId) )
        {
          if( !reqElem->GetArcFileClosedFlag() )  // 後ろにいるフラグが立っていないものを探しフラグを立てる。
          {
            reqElem->SetArcFileClosedFlag(true);
            break;
          }
        }
      }
      elem = elem->GetPrev();
    }
  }
}

void AsyncFileManager::MatchCheckManager::SetAnnounceLevel(AnnounceLevel announceLevel)
{
  m_announceLevel = announceLevel;
}
AsyncFileManager::MatchCheckManager::AnnounceLevel AsyncFileManager::MatchCheckManager::GetAnnounceLevel(void) const
{
  return m_announceLevel;
}

void AsyncFileManager::MatchCheckManager::SetCheckFlag(bool checkFlag)
{
  m_checkFlag = checkFlag;
}
bool AsyncFileManager::MatchCheckManager::GetCheckFlag(void) const
{
  return m_checkFlag;
}

void AsyncFileManager::MatchCheckManager::ClearRemainingReqElem(void)
{
  // 全削除する
  {
    GFL_PRINT("FsMatchCheck: この時点で残っていた「対応するFinishが呼ばれていないリクエスト」(下記)はクリアされます\n");
    checkRemainingReqElemTemplate<bool>(false, true);  // クリアされるリクエストをPRINTする(ASSERTにはしない)

    ElemBase* elem = m_matchCheckReqList->GetHead();
    while(elem)
    {
      ElemBase* nextElem = elem->GetNext();
      this->destroyMatchCheckReqElem(static_cast<MatchCheckReqElem*>(elem));
      elem = nextElem;
    }
  }
}
void AsyncFileManager::MatchCheckManager::CheckRemainingReqElem(void) const
{
  this->checkRemainingReqElem();
}
void AsyncFileManager::MatchCheckManager::OutputReqNum(void) const
{
  GFL_PRINT("FsMatchCheck: Req num=%d, max=%d\n", m_currReqNum, m_reqNumBiggest);
}

AsyncFileManager::MatchCheckReqElem* AsyncFileManager::MatchCheckManager::createMatchCheckReqElem(void)
{
  if(m_checkFlag)
  {
    /*
    if(m_heap)
    {
      s32 allocatableSize = m_heap->GetTotalAllocatableSize();  // マイナスを返すこともある。
      s32 requireSize     = sizeof(MatchCheckReqElem);  // sizeofが返すのがunsigndっていう処理系もあるらしい。
      // マイナスsignedとunsignedの比較はunsignedになってしまうので注意。
      if(allocatableSize >= requireSize)  // 比較前に型を合わせておく
      {
        MatchCheckReqElem* elem = GFL_NEW(m_heap) MatchCheckReqElem();
        m_matchCheckReqList->Add(elem);
        return elem;
      }
    }
    */

    if(m_reqMemTopAddr != NULL)
    {
      for(u32 bitIndex=0; bitIndex<REQ_NUM_MAX; ++bitIndex)
      {
        if( ( m_reqMemBitManage[bitIndex/32] & (1<<(bitIndex%32)) ) == 0 )
        {
          ++m_currReqNum;
          if(m_currReqNum > m_reqNumBiggest) m_reqNumBiggest = m_currReqNum;
          m_reqMemBitManage[bitIndex/32] |= (1<<(bitIndex%32));  // 使用することになった箇所のビットを1にする
          MatchCheckReqElem* elem = new(reinterpret_cast<void*>( (reinterpret_cast<uptr>(m_reqMemTopAddr)) + sizeof(MatchCheckReqElem)*bitIndex )) MatchCheckReqElem();  // placement new
          m_matchCheckReqList->Add(elem);
          return elem;
        }
      }
    }

    {
      // ASSERTで止める前に出せるだけ情報を出しておく(ASSERT後に同じ情報が出ますがご勘弁を)
      GFL_PRINT("FsMatchCheck: elemのメモリを確保できません。\n");
      GFL_PRINT("FsMatchCheck: vvvv 下記が「対応するFinishが呼ばれていないリクエスト」(空ならOK) vvvv\n");
      AnnounceLevel currAnnounceLevel = this->GetAnnounceLevel();
      this->SetAnnounceLevel(ANNOUNCE_LEVEL_PRINT); 
      this->CheckRemainingReqElem();
      this->SetAnnounceLevel(currAnnounceLevel); 
    }
    this->announce("FsMatchCheck: elemのメモリを確保できません。\n");
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
    GFL_PRINT("FsMatchCheck: vvvv 下記が「対応するFinishが呼ばれていないリクエスト」(空ならOK) vvvv\n");
    this->CheckRemainingReqElem();
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  }
  return NULL;
}

void AsyncFileManager::MatchCheckManager::announce(const char* mes) const
{
  switch(m_announceLevel)
  {
  case ANNOUNCE_LEVEL_PRINT:
    {
      GFL_PRINT(mes);
      break;
    }
  case ANNOUNCE_LEVEL_ASSERT:
    {
      GFL_ASSERT_MSG(0, mes);
      break;
    }
  }
}

void AsyncFileManager::MatchCheckManager::checkRemainingReqElem(void) const
{
  checkRemainingReqElemTemplate<bool>(false);
}
void AsyncFileManager::MatchCheckManager::checkRemainingReqElem(const char*    fileName) const
{
  checkRemainingReqElemTemplate<const char*>(fileName);
}
void AsyncFileManager::MatchCheckManager::checkRemainingReqElem(ArcFile::ARCID arcId   ) const
{
  checkRemainingReqElemTemplate<ArcFile::ARCID>(arcId);
}

void AsyncFileManager::MatchCheckManager::checkRemainingArcFileCloseReqElemAtAddArcFileOpenReq(const ArcFileOpenReq& req) const
{
  // SetAddArcFileOpenReqから呼ばれる

  // ArcFileのオープンリクエストの場合、同じArcFileに対するクローズリクエストのIs...Finishedが済んでいるか、を確認する
  if(m_checkFlag)
  {
    static const u32 MES_BUF_SIZE = 1024;
  
    bool isRemaining = false;
 
    char mes[MES_BUF_SIZE];                                       

    if(req.arcId != ArcFile::ARCID_NULL)
    {
      ElemBase* elem = m_matchCheckReqList->GetTail();  // 後ろから探す
      while(elem)
      {
        MatchCheckReqElem* reqElem = static_cast<MatchCheckReqElem*>(elem);
        if( reqElem->IsSameArcInfo(req.arcId) )
        {
          if( reqElem->IsReqTypeArcFileClose(req.arcId) )
          {
            if( reqElem->GetArcFileClosedFlag() )  // クローズが終わっているのに、Is...Finishedが呼ばれていない。
            {
              isRemaining = true; 
              reqElem->GetInfoMes(mes, MES_BUF_SIZE);
              GFL_RELEASE_PRINT(mes);
              // 全部確認するのでbreakしない
            }
          }
        }
        elem = elem->GetPrev();
      }
    }
    else
    {
      ElemBase* elem = m_matchCheckReqList->GetTail();  // 後ろから探す
      while(elem)
      {
        MatchCheckReqElem* reqElem = static_cast<MatchCheckReqElem*>(elem);
        if( reqElem->IsSameArcInfo(req.fileName) )
        {
          if( reqElem->IsReqTypeArcFileClose(req.fileName) )  // クローズが終わっているのに、Is...Finishedが呼ばれていない。
          {
            if( reqElem->GetArcFileClosedFlag() )
            {
              isRemaining = true; 
              reqElem->GetInfoMes(mes, MES_BUF_SIZE);
              GFL_RELEASE_PRINT(mes);
              // 全部確認するのでbreakしない
            }
          }
        }
        elem = elem->GetPrev();
      }
    }

    // 最後に、ASSERTにする必要があるならASSERTにする。
    if(isRemaining)
    {
      GFL_RELEASE_PRINT("FsMatchCheck: ^^^^ 上記が「対応するFinishが呼ばれていないリクエスト」 ^^^^\n");
      this->OutputReqNum();
      
      if( this->GetAnnounceLevel() == ANNOUNCE_LEVEL_ASSERT )
      {
        GFL_ASSERT(0);
      }
    }
  }
}

void AsyncFileManager::MatchCheckManager::destroyMatchCheckReqElem(MatchCheckReqElem* elem)
{
  m_matchCheckReqList->Remove(elem);
  /*
  GFL_SAFE_DELETE(elem);
  */
 
  elem->~MatchCheckReqElem();  // デストラクタの明示的な呼び出し
  u32 bitIndex = ( (reinterpret_cast<uptr>(elem)) - (reinterpret_cast<uptr>(m_reqMemTopAddr)) ) / sizeof(MatchCheckReqElem);
  m_reqMemBitManage[bitIndex/32] &= ~(1<<(bitIndex%32));  // 使用しなくなった箇所のビットを0にする
  --m_currReqNum;
}

void AsyncFileManager::MatchCheckManager::destroyMatchCheckReqElemAtFinish(MatchCheckReqElem* elem, bool checkRemainingReqElemFlag, bool checkOnlyArcInfoFlag, void**         /*ppBuf*/   )
{
  this->destroyMatchCheckReqElem(elem);
  if(checkRemainingReqElemFlag)
  {
    this->checkRemainingReqElem();
  }
}
void AsyncFileManager::MatchCheckManager::destroyMatchCheckReqElemAtFinish(MatchCheckReqElem* elem, bool checkRemainingReqElemFlag, bool checkOnlyArcInfoFlag, const void*    /*pBuf*/    )
{
  this->destroyMatchCheckReqElem(elem);
  if(checkRemainingReqElemFlag)
  {
    this->checkRemainingReqElem();
  }
}
void AsyncFileManager::MatchCheckManager::destroyMatchCheckReqElemAtFinish(MatchCheckReqElem* elem, bool checkRemainingReqElemFlag, bool checkOnlyArcInfoFlag, const char*    fileName)
{
  this->destroyMatchCheckReqElem(elem);
  if(checkRemainingReqElemFlag)
  {
    if(checkOnlyArcInfoFlag)
    {
      this->checkRemainingReqElem(fileName);
    }
    else
    {
      this->checkRemainingReqElem();
    }
  }
}
void AsyncFileManager::MatchCheckManager::destroyMatchCheckReqElemAtFinish(MatchCheckReqElem* elem, bool checkRemainingReqElemFlag, bool checkOnlyArcInfoFlag, ArcFile::ARCID arcId   )
{
  this->destroyMatchCheckReqElem(elem);
  if(checkRemainingReqElemFlag)
  {
    if(checkOnlyArcInfoFlag)
    {
      this->checkRemainingReqElem(arcId);
    }
    else
    {
      this->checkRemainingReqElem();
    }
  }
}

void AsyncFileManager::destroyMatchCheckSystem(void)
{
  GFL_SAFE_DELETE(m_matchCheckManager);
}
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST



void AsyncFileManager::CreateMatchCheckSystem(gfl2::heap::HeapBase* heap)
{
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  m_matchCheckManager = GFL_NEW(heap) MatchCheckManager(heap, this);
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
}
void AsyncFileManager::SetMatchCheckAnnounceAssert(bool setAssert)
{
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  if(m_matchCheckManager)
  {
    if(setAssert)
    {
      m_matchCheckManager->SetAnnounceLevel(MatchCheckManager::ANNOUNCE_LEVEL_ASSERT);
    }
    else
    {
      m_matchCheckManager->SetAnnounceLevel(MatchCheckManager::ANNOUNCE_LEVEL_PRINT);
    }
  }
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
}
bool AsyncFileManager::IsMatchCheckAnnounceAssert(void) const
{
  bool ret = false;
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  if(m_matchCheckManager)
  {
    MatchCheckManager::AnnounceLevel announceLevel = m_matchCheckManager->GetAnnounceLevel();
    ret = (announceLevel == MatchCheckManager::ANNOUNCE_LEVEL_ASSERT);
  }
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  return ret;
}
void AsyncFileManager::SetMatchCheckFlag(bool checkFlag)
{
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  if(m_matchCheckManager)
  {
    m_matchCheckManager->SetCheckFlag(checkFlag);
  }
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
}
bool AsyncFileManager::GetMatchCheckFlag(void) const
{
  bool ret = false;
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  if(m_matchCheckManager)
  {
    ret = m_matchCheckManager->GetCheckFlag();
  }
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  return ret;
}

void AsyncFileManager::ClearMatchCheckRemainingReq(void)
{
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  if(m_matchCheckManager)
  {
    m_matchCheckManager->ClearRemainingReqElem();
  }
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
}
void AsyncFileManager::OutputMatchCheckRemainingReq(void) const
{
#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  if(m_matchCheckManager)
  {
    GFL_PRINT("FsMatchCheck: vvvv 下記が「対応するFinishが呼ばれていないリクエスト」(空ならOK) vvvv\n");
    m_matchCheckManager->CheckRemainingReqElem();
    m_matchCheckManager->OutputReqNum();
  }
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
}


#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
void AsyncFileManager::setMatchCheckArcFileClosedFlagOn(const char*    fileName)
{
  if(m_matchCheckManager)
  {
    m_matchCheckManager->SetArcFileClosedFlagOn(fileName);
  }
}
void AsyncFileManager::setMatchCheckArcFileClosedFlagOn(ArcFile::ARCID arcId   )
{
  if(m_matchCheckManager)
  {
    m_matchCheckManager->SetArcFileClosedFlagOn(arcId);
  }
}
#endif  // GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)

