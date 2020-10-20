//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldSubWindowProcManager.h
 *	@brief  フィールドの下画面 イベント無しで動いている物
 *	@author	tomoya takahashi   => k.ohno
 *	@date		2011.06.02         => 2015.5.31
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __FIELDSUBWINDOWPROCMANAGER_H__
#define __FIELDSUBWINDOWPROCMANAGER_H__
#pragma once


#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <ro/include/gfl2_RoManager.h> // Ro
#include "AppLib/include/Util/app_util_heap.h"

// FieldMenuOutRequestID
#include "Field/FieldStatic/include/Subscreen/SubscreenRequestDef.h"

//-----------------------------------------------------------------------------
/**
 *					前方宣言
 */
//-----------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App );
GFL_NAMESPACE_BEGIN( Finder );
class FinderProc;
GFL_NAMESPACE_END( Finder );
GFL_NAMESPACE_END( App );


GFL_NAMESPACE_BEGIN( App );
GFL_NAMESPACE_BEGIN( FieldMenu );
class FieldMenuSubWindowProc;
GFL_NAMESPACE_END( FieldMenu );
GFL_NAMESPACE_END( App );

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
class JoinFestaSubScreenProc;
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

GFL_NAMESPACE_BEGIN( gfl2 );
GFL_NAMESPACE_BEGIN( proc );
class Manager;
GFL_NAMESPACE_END( proc );
GFL_NAMESPACE_END( gfl2 );



GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( SubScreen );
class FieldSubScreen;

//-----------------------------------------------------------------------------
/**
 *					定数宣言
 */
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
 */
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
 */
//-----------------------------------------------------------------------------


/**
 * @enum SubWindowId
 * 下画面の種類定義
 */
enum SubWindowId{
  WIN_NONE,             //! なし
  WIN_MENU_MAIN,        //! フィールド通常下画面
  WIN_JOIN_FESTA,       //! ジョインフェスタ
  WIN_FINDER,           //! ポケファインダー 
};


//-----------------------------------------------------------------------------
/**
 *    サブウィンドウマネージャ
 */
//-----------------------------------------------------------------------------
class FieldSubScreenProcManager
{
  GFL_FORBID_COPY_AND_ASSIGN(FieldSubScreenProcManager); //コピーコンストラクタ＋代入禁止


public:

  /**
   *	@brief  コンストラクタ
   */
  FieldSubScreenProcManager( gfl2::heap::HeapBase* pSys ,gfl2::heap::HeapBase* pDev );
  /**
   *	@brief  デストラクタ
   */
  ~FieldSubScreenProcManager( void );


  //----------------------------------------------------------------------------
  /**
   *	@brief    初期化
   *
   *	@param	p_fieldmap    フィールドマップ
   */
  //-----------------------------------------------------------------------------
  void Initialize( Field::SubScreen::FieldSubScreen* pFieldSub );

  //----------------------------------------------------------------------------
  /**
   *	@brief    破棄
   */
  //-----------------------------------------------------------------------------
  bool Finalize( void );


  //----------------------------------------------------------------------------
  /**
   *	@brief  一時停止
   */
  //-----------------------------------------------------------------------------
  void Suspend( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief  再始動
   */
  //-----------------------------------------------------------------------------
  void Resume( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief    更新
   */
  //-----------------------------------------------------------------------------
  void Update( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief    描画
   * @param displayNo 描画対象画面
   */
  //-----------------------------------------------------------------------------
  void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  //----------------------------------------------------------------------------
  /**
   *	@brief  下画面呼び出し
   *  @param  [in]subWindowId  SubWindowId定義
   *  @note   IsChangeComplateで下画面切り替わったかチェック出来る(WIN_NONEでも大丈夫）
   */
  //-----------------------------------------------------------------------------
  void CallWindow( SubWindowId windowId );

  //----------------------------------------------------------------------------
  /**
   *	@brief  ウィンドウ切り替えの完了まち
   *
   *	@retval true    ウィンドウ切り替え完了
   *	@retval false   ウィンドウ切り替え途中
   */
  //-----------------------------------------------------------------------------
  bool IsChangeComplate( void ) const;

  /* ----------------------------------------------------------------------------------------- */
  /**
   *	@brief  起動中の下画面の破棄リクエスト
   */
  /* ----------------------------------------------------------------------------------------- */
  void DeleteWindowRequest();

  /* ----------------------------------------------------------------------------------------- */
  /**
   *	@brief  起動中の下画面が破棄完了かどうか
   *  @return trueで破棄完了
   */
  /* ----------------------------------------------------------------------------------------- */
  bool IsDeleteWindowComplete();

  //----------------------------------------------------------------------------
  /**
   *	@brief  起動中の下画面種類のID 取得
   *  @return subWindowId  SubWindowId定義
   */
  //-----------------------------------------------------------------------------
  SubWindowId GetSubWindowId();

  //-----------------------------------------------------------------------------
  /**
   *    ウィンドウの呼び出し。
   *
   *    @retval NULL  まだ設定できない。
   */
  //-----------------------------------------------------------------------------
  template <class CallClass>
  inline CallClass* CallSubWindow( void )
  {
    CallClass* p_proc = GFL_NEW(m_pHeapCall) CallClass();
    return static_cast<CallClass*>(m_pManager->CallProc( p_proc ));
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  破棄が完了しているか？
   */
  //-----------------------------------------------------------------------------
  inline bool IsFinalized( void ) const { return (m_pManager == NULL); }


  App::FieldMenu::FieldMenuSubWindowProc* GetFieldMenuProc(void){ return m_pFieldMenuProc; }
  App::Finder::FinderProc* GetFienderProc(){ return m_pFinderProc; }
  NetApp::JoinFestaSubScreen::JoinFestaSubScreenProc* GetJoinFestaSubScreenProc(){ return m_pJoinFestaSubScreenProc; }

  /**
   *  @brief 下画面イベントリクエストチェック
   *  @retval true:イベント有り
   *  @retval false:イベント無し
   */
  bool IsEventRequest() const;

  /**
   *  @brief 下画面イベントリクエスト取得
   *  @return リクエストID
   */
  FieldMenuOutRequestID GetEventRequestID() const;


  //-----------------------------------------------------------------------------
  /**
   * @func    RemakeFrame
   * @brief   フレーム再生成リクエスト（フィールド通常下画面専用）
   * @date    2015.09.10
   *
   * @retval  true  = リクエスト成功
   * @retval  false = リクエスト失敗
   *
   * @note  下画面が無かったり、終了リクエストが発行されている場合は失敗します
   */
  //-----------------------------------------------------------------------------
  bool RemakeFrame(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsFrameRemake
   * @brief   フレーム再生成中か？（フィールド通常下画面専用）
   * @date    2016.05.17
   *
   * @retval  true  = 再生成中
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsFieldMenuFrameRemake(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsFieldMenuCreate
   * @brief   フィールドメニューの生成が終了したか（フィールド通常下画面専用）
   * @date    2015.11.04
   *
   * @retval  true  = した
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsFieldMenuCreate(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRotomEvent
   * @brief   ロトムメッセージ表示イベントを開始（フィールド通常下画面専用）
   * @date    2015.11.11
   *
   * @param   msg_id  メッセージID
   *
   * @retval  true  = 成功
   * @retval  false = 失敗
   */
  //-----------------------------------------------------------------------------
  bool SetRotomEvent( u32 msg_id );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsRotomEvent
   * @brief   ロトムメッセージ表示イベント中か（フィールド通常下画面専用）
   * @date    2015.11.11
   *
   * @retval  true  = イベント中
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsRotomEvent(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRotomPosEvent
   * @brief   ロトムメッセージ表示リクエスト（POSイベント用）
   * @date    2016.01.15
   *
   * @param   msg_id  メッセージID
   */
  //-----------------------------------------------------------------------------
  void SetRotomPosEvent( u32 msg_id );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsCanCreateFieldStaffList
   * @brief   FieldStaffListクラスを生成可能か
   * @date    2017.04.05
   *
   * @retval  true  = 可
   * @retval  false = 不可
   */
  //-----------------------------------------------------------------------------
  bool IsCanCreateFieldStaffList(void) const;

private:

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   下画面プロセス作成
   */
  /* ----------------------------------------------------------------------------------------- */
  void CreateSubScreenProc();

  /* ----------------------------------------------------------------------------------------- */
  /**
   *    今のPROCの終了リクエスト
   */
  /* ----------------------------------------------------------------------------------------- */
  void SubScreenProcEndReq(void);

private:

  //! サブウィンドウコール用ヒープ
  static const u32 SUBWINDOW_CALL_HEAP_SIZE = 0x1000;

private:

  App::FieldMenu::FieldMenuSubWindowProc* m_pFieldMenuProc;
  App::Finder::FinderProc* m_pFinderProc;
  NetApp::JoinFestaSubScreen::JoinFestaSubScreenProc* m_pJoinFestaSubScreenProc;

  Field::SubScreen::FieldSubScreen* mpSubScreen;
  
  gfl2::proc::Manager* m_pManager;   ///< 子プロセス管理マネージャ
  gfl2::heap::HeapBase* m_pHeap;     ///< 使用ヒープ
  gfl2::heap::HeapBase* m_pDevHeap;     ///< 使用ヒープ
  gfl2::heap::HeapBase* m_pHeapCall; ///< プロセスコール用のヒープ
  SubWindowId m_callWindowId; ///< リクエストしたウィンドウID
  int mEndSeq;  ///< 終了時のシーケンス

  bool m_bSubScreenProcEndRequest; ///< プロセスかえのリクエストチェック
  gfl2::ro::Module*     m_pRoModule;  ///< DLL 解放の為に保存

  bool m_isSuspend;   //! 一時停止フラグ

};


GFL_NAMESPACE_END( SubScreen );
GFL_NAMESPACE_END( Field );

#endif	// __FIELDSUBSCREENPROCMANAGER_H__

