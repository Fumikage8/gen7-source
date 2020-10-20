//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		GameClearSaveView.h
 *	@brief  ゲームクリアー時のセーブ画面描画クラス
 *	@author	Toru=Nagihashi
 *	@date		2013.01.29
 *	@author	koji_kawada@gamefreak.co.jp
 *	@date		2016.01.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#if !defined( __APP_GAMECLEARSAVE_PARENTVIEW_H__)
#define __APP_GAMECLEARSAVE_PARENTVIEW_H__

#include <fs/include/gfl2_Fs.h>

#include <AppLib/include/Ui/UIView.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Tool/app_tool_MenuCursor.h>
#include <AppLib/include/Tool/app_tool_TimeIcon.h>
#include <niji_conv_header/message/msg_report.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(uool)
  class AppRenderingManager;
GFL_NAMESPACE_END(uool)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(game_clear_save)


// 前方宣言
class MessageWindowView;
class MessageWindowViewEventHandler;


class View : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(View);

  friend class MessageWindowViewEventHandler;

public:
  //---------------------------------------------------------------------
  // 定数
  //---------------------------------------------------------------------
  /**
   * @brief メッセージ描画タイプ
   */
  enum MessageType
  {
    MESSAGE_TYPE_TIMER_ICON,    ///<タイマーアイコンを表示し消えない
    MESSAGE_TYPE_USER_INPUT,    ///<ユーザー入力
    MESSAGE_TYPE_WAIT,    ///<秒数待ち
  };
  //---------------------------------------------------------------------
  // コンストラクタ・デストラクタ
  //---------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   *
   * @param heap  ヒープ
   * @param file_read_manager    ファイル読み込みシステム
   */
  View( app::util::Heap * heap, gfl2::fs::AsyncFileManager *file_manager );
  /**
   * @brief デストラクタ
   */
  virtual ~View();
  /**
   * @brief 初期化処理
   *
   * @return true 初期化処理完了 false  初期化処理中
   */
  bool Initialize( void );
  /**
   * @brief 解放処理
   *
   * @return true 解放処理完了 false  開放処理中
   */
  bool Finalize( void );
  //---------------------------------------------------------------------
  // システム
  //---------------------------------------------------------------------
  /**
   * @brief 更新
   */
  virtual void Update( void );
  /**
   * @brief 描画
   */
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );
  app::util::AppRenderingManager* GetAppRnderingManager(void) const { return m_appRenderingManager; }
  //---------------------------------------------------------------------
  // メッセージ
  //---------------------------------------------------------------------
  /**
   * @brief 上画面ウィンドウへメッセージ表示
   *
   * @param message_id  メッセージ
   * @param message_type  メッセージタイプ
   */
  void StartMessage( u32 message_id, MessageType message_type );
  /**
   * @brief メッセージ終了検知
   *
   * @return trueメッセージ終了 falseメッセージ処理中
   */
  bool IsFinishMessage( void ) const;

private:
  /**
   * @brief メッセージ終了をリクエストする
   * @note friendからアクセスする
   */
  void RequestFinishMessage( void );

private:
  //生成
  void CreateLayout( app::util::Heap * heap );
  void DeleteLayout( void );
  
  void UpdateMessage( void );
  
private:
  MessageType m_message_type;
  bool m_is_message_end;
  
  app::util::Heap*                    m_appHeap;
  gfl2::fs::AsyncFileManager*         m_fileManager;
  s32                                 m_createState;
  app::util::AppRenderingManager*     m_appRenderingManager;
  gfl2::fs::BinLinkerAccessor         m_binLinker;
  void*                               m_dataBuf;
  void*                               m_appLytBuf;
  void*                               m_menuWindowBuf;
  MessageWindowView*                  m_messageWindowView;
  MessageWindowViewEventHandler*      m_messageWindowViewEventHandler;
  bool                                m_requestMessageEnd;
};


GFL_NAMESPACE_END(game_clear_save)
GFL_NAMESPACE_END(app)
#endif		// __APP_GAMECLEARSAVE_PARENTVIEW_H__

