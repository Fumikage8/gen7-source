#if PM_DEBUG
//=============================================================================
/**
 * @file    LauncherProc.h
 * @brief   デバッグ用ランチャー処理
 * @author  Hiroyuki Nakamura
 * @date    2015.02.05
 */
//=============================================================================
#if !defined( LAUNCHER_PROC_H_INCLUDED )
#define LAUNCHER_PROC_H_INCLUDED

#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <heap/include/gfl2_NwAllocator.h>
//#include <proc/include/gfl2_Proc.h>
#include <Layout/include/gfl2_Layout.h>

#include <GameSys/include/GameProc.h>
#include <Debug/Launcher/source/LauncherSub.h>


GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(launcher)

// 前方宣言
class ListData;


//=============================================================================
/**
 * @class   LauncherProc
 * @brief   ランチャークラス
 * @date    2015.02.05
 */
//=============================================================================
class LauncherProc
  : public GameSys::GameProc,
    public LauncherSub::EventListener
{
  GFL_FORBID_COPY_AND_ASSIGN( LauncherProc );

public:
  //! セットアップパラメータ
  struct CallParam
  {
    /*
      "heap = NULL"の場合、heapIDのヒープを作成します。
      "heapID = HEAPID_NULL"の場合、内部の固定IDからヒープを作成します。

      "devHeap = NULL"の場合、devHeapIDのヒープを作成します。
      "devHeapID = HEAPID_NULL"の場合、内部の固定IDからヒープを作成します。

      各アロケータがNULLの場合、作成したヒープから作成します。
    */
  	gfl2::heap::HeapBase * heap;              //!< [in] ヒープ
  	gfl2::heap::HeapBase * devHeap;           //!< [in] デバイスヒープ
  	gfl2::heap::NwAllocator * allocator;      //!< [in] ヒープアロケータ
  	gfl2::heap::NwAllocator * devAllocator;   //!< [in] デバイスヒープアロケータ
    u32 heapID;                               //!< [in] ヒープID
    u32 devHeapID;                            //!< [in] デバイスヒープアロケータ
  
    u32 cursorPos;        //!< [out] 終了時のカーソル位置 ( キャンセル時 = CURSOR_POS_CANCEL )
  };

  static const u32 HEAPID_NULL = 0xffffffff;        //!< ヒープIDの無効値

  static const u32 CURSOR_POS_CANCEL = 0xffffffff;  //!< キャンセルのカーソル位置


private:
  //! シーケンスID
  enum SequensID
  {
    SEQID_SELECT = 0,   //!< 項目選択
    SEQID_SELECT_WAIT,
    SEQID_SCROLL_MAIN,  //!< ページスクロールメイン
    SEQID_SCROLL_END,   //!< ページスクロール終了
    SEQID_WAIT,         //!< 待機
  };


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   prm   セットアップパラメータ
   */
  //-----------------------------------------------------------------------------
  LauncherProc( CallParam * prm );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~LauncherProc();

  //-----------------------------------------------------------------------------
  /**
   * @func    InitSetupParam
   * @brief   外部設定パラメータを初期化
   * @date    2015.02.05
   *
   * @param   prm   セットアップパラメータ
   */
  //-----------------------------------------------------------------------------
  static void InitCallParam( CallParam * prm );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitFunc
   * @brief   LauncherProc初期化処理
   * @date    2015.02.05
   *
   * @param   pManager    プロセス管理マネージャー
   *
   * @return  gfl::proc::Result
   */
  //-----------------------------------------------------------------------------
  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager * pManager );

  //-----------------------------------------------------------------------------
  /**
   * @func    EndFunc
   * @brief   LauncherProc終了処理
   * @date    2015.02.05
   *
   * @param   pManager    プロセス管理マネージャー
   *
   * @return  gfl::proc::Result
   */
  //-----------------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager * pManager );

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateFunc
   * @brief   LauncherProcアップデート処理
   * @date    2015.02.05
   *
   * @param   pManager    プロセス管理マネージャー
   *
   * @return  gfl::proc::Result
   */
  //-----------------------------------------------------------------------------
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager * pManager );

  //-----------------------------------------------------------------------------
  /**
   * @func    PreDrawFunc
   * @brief   LauncherProc描画処理(下準備)
   * @date    2015.02.19
   *
   * @param   pManager    プロセス管理マネージャー
   */
  //-----------------------------------------------------------------------------
  virtual void PreDrawFunc( gfl2::proc::Manager * pManager );

  //-----------------------------------------------------------------------------
  /**
   * @func    DrawFunc
   * @brief   LauncherProc描画処理
   * @date    2015.02.05
   *
   * @param   pManager    プロセス管理マネージャー
   * @param   displayNo   描画対象画面
   */
  //-----------------------------------------------------------------------------
  virtual void DrawFunc( gfl2::proc::Manager * pManager, gfl2::gfx::CtrDisplayNo displayNo );

  //------------------------------------------------------------------
  /**
    * @brief   PROCのイベント起動検知処理
    *
    *  以下のタイミングで行うべき、イベント起動処理
    *  　GameProc->EventCheckFunc
    *  　EventManager->Main
    *  　GameProc->Main
    */
  //------------------------------------------------------------------
  virtual void EventCheckFunc(GameSys::GameManager* pManager);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateStartListData
   * @brief   ランチャー開始時のリストデータを作成
   * @date    2015.02.19
   *
   * @note    リスナーで切り替える場合はCreateListData()を使用すること！
   */
  //-----------------------------------------------------------------------------
  template< class ListenerClass > void CreateStartListData(void)
  {
    if( m_pListData != NULL )
    {
      GFL_ASSERT_STOP( 0 );
    }
    m_pListData = GFL_NEW(m_pSysHeap) ListenerClass();
  };

  //-----------------------------------------------------------------------------
  /**
   * @func    CreatetListData
   * @brief   リスト切り替え時のリストデータを作成
   * @date    2015.02.19
   *
   * @note    ランチャー開始時はCreateStartListData()を使用すること！
   */
  //-----------------------------------------------------------------------------
  template< class ListenerClass > void CreateListData(void)
  {
    if( m_pTmpListData != NULL )
    {
      GFL_ASSERT_STOP( 0 );
    }
    if( m_isListCreateLow == false )
    {
      m_pTmpListData = GFL_NEW_LOW(m_pSysHeap) ListenerClass();
    }
    else
    {
      m_pTmpListData = GFL_NEW(m_pSysHeap) ListenerClass();
    }
    m_isListCreateLow ^= 1;
  };


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    CreateSystemHeap
   * @brief   システムヒープを作成
   * @date    2015.02.18
   */
  //-----------------------------------------------------------------------------
  void CreateSystemHeap(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteSystemHeap
   * @brief   システムヒープを削除
   * @date    2015.02.18
   */
  //-----------------------------------------------------------------------------
  void DeleteSystemHeap(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteListData
   * @brief   リストデータを削除
   * @date    2015.02.18
   */
  //-----------------------------------------------------------------------------
  void DeleteListData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateCore
   * @brief   コアデータ生成
   * @date    2015.02.05
   */
  //-----------------------------------------------------------------------------
  void CreateCore(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateCore
   * @brief   コアデータ削除
   * @date    2015.02.05
   */
  //-----------------------------------------------------------------------------
  void ReleaseCore(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateHeap
   * @brief   ヒープ作成
   * @date    2015.02.05
   */
  //-----------------------------------------------------------------------------
	void CreateHeap(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteHeap
   * @brief   ヒープ削除
   * @date    2015.02.05
   */
  //-----------------------------------------------------------------------------
	void DeleteHeap(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SeaquenceSelect
   * @brief   シーケンス：項目選択
   * @date    2015.02.05
   *
   * @retval  true  = 継続
   * @retval  false = 終了へ
   */
  //-----------------------------------------------------------------------------
  bool SequenceSelect(void);
  bool SequenceSelectWait(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SeaquenceScrollMain
   * @brief   シーケンス：ページスクロール処理
   * @date    2015.02.05
   */
  //-----------------------------------------------------------------------------
  void SequenceScrollMain(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SeaquenceScrollEnd
   * @brief   シーケンス：ページスクロール終了処理
   * @date    2015.02.05
   */
  //-----------------------------------------------------------------------------
  void SequenceScrollEnd(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SelectPos
   * @brief   項目決定処理
   * @date    2015.02.05
   *
   * @param   pos   項目位置 ( キャンセル = CURSOR_POS_CANCEL )
   *
   * @retval  true  = 継続
   * @retval  false = 終了へ
   */
  //-----------------------------------------------------------------------------
  bool SelectPos( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnPrintString
   * @brief   SubのOnPrintString と ListData をブリッジ
   */
  //-----------------------------------------------------------------------------
  virtual void OnPrintString( u32 pos, gfl2::str::StrBuf * strBuf );

private:
  gfl2::heap::HeapBase * m_pSysHeap;  //!< システムヒープ
  bool m_createSysHeap;               //!< システムヒープを生成したか

  ListData * m_pListData;     //!< リストデータ
  ListData * m_pTmpListData;  //!< テンポラリリストデータ
  bool m_isListCreateLow;     //!< リストデータを後方メモリ確保で生成したか

  gfl2::heap::HeapBase * m_pHeap;                 //!< ヒープ
  gfl2::heap::HeapBase * m_pDevHeap;              //!< デバイスヒープ
  gfl2::heap::NwAllocator * m_pHeapAllocator;     //!< ヒープアロケータ
  gfl2::heap::NwAllocator * m_pDevHeapAllocator;  //!< デバイスヒープアロケータ

  LauncherSub * m_pLauncherSub;   //!< サブルーチンクラス

  s16 m_page;   //!< ページ番号
  s16 m_pos;    //!< カーソル位置

  u32 m_selectPos;

  CallParam  m_CallParamOrg;  ///!< セットアップパラメータオリジナル
  CallParam * m_pCallParam;   //!< セットアップパラメータ

};


GFL_NAMESPACE_END(launcher)
GFL_NAMESPACE_END(debug)

#endif  // LAUNCHER_PROC_H_INCLUDED

#endif  // PM_DEBUG
