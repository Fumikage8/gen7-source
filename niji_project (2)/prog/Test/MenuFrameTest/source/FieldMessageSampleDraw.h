//==============================================================================
/**
 * @file	  FieldMessageSampleDraw.h
 * @brief	  表示のサンプル
 * @author	k.ohno
 * @date	  January 18, 2015
 */
// =============================================================================
#if PM_DEBUG

#if !defined( __FIELD_MESSAGE_SAMPLE_DRAW_H__ )
#define	__FIELD_MESSAGE_SAMPLE_DRAW_H__
#pragma once

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/app_util_2d.h>
#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>

#include "FieldMessageSampleDrawListener.h"
#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Util/AppRenderingManager.h"

#include "Test/MenuFrameTest/include/FieldMessageSampleAppParam.h"
#include "Print/include/WordSet.h"
#include "str/include/gfl2_Str.h"


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FieldMessageTest )

/// タイトル用ビュー
class FieldMessageSampleDraw : public app::ui::UIView, public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(FieldMessageSampleDraw);

private:

  static const int TEMP_STRBUF_LENGTH = 200;
  
  /// レイアウトID
  typedef enum{
    LAYOUT_ID_UPPER,
    LAYOUT_ID_LOWER,
    LAYOUT_ID_MAX,
  }LAYOUT_ID;

  /// 上画面アニメ
  typedef enum{
    ANMID_UPPER_IN = 0,     ///< タイトルイン
    ANMID_UPPER_LOOP,       ///< 背景ループ
    ANMID_UPPER_OUT,        ///< タイトルアウト

    ANMID_UPPER_MAX,
  }ANIMID_UPPER;

  /// 下画面アニメ
  typedef enum{
    ANMID_LOWER_IN = 0,     ///< タイトルイン
    ANMID_LOWER_LOOP,       ///< 背景ループ
    ANMID_LOWER_OUT,        ///< タイトルアウト

    ANMID_LOWER_MAX,
  }ANIMID_LOWER;

public:

  /**
   * @brief コンストラクタ
   * @param heap  ヒープ
   */
//  FieldMessageSampleDraw( app::util::Heap * heap,gfl2::lyt::LayoutRenderPath* pLayoutRenderPath );
  FieldMessageSampleDraw( app::util::Heap * heap,app::util::AppRenderingManager* pAppRanderingManager );

  /// デストラクタ
  virtual ~FieldMessageSampleDraw();




  
public:
  /// このビュー専用のUIリスナー設定
  void SetTitleViewUIListener( FieldMessageSampleDrawListener *listener );

public:
  /// 更新
  virtual void Update( void );
  /// 描画
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  /**
   * @brief   破棄チェック
   * @return  trueで破棄完了
   */
  bool IsExit( void );


public:
  /// タッチパネルイベントの検知
  virtual ::app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  /// ペインアクション通知用リスナ
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );
  /// キー入力通知用リスナ
  virtual ::app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick);

  
  void Init(APP_PARAM* pAppParam, void* pAppLytBuff);
  bool End(void);
  //----------------------------------------------------------------------
  /**
   *	@brief  テキスト表示
   *  @param messageid
   */
  //----------------------------------------------------------------------
  void SetMessage( int msgid );

private:
  //----------------------------------------------------------------------
  /**
   *	@brief  2D関連作成
   *  @param  heap    ヒープ
   */
  //----------------------------------------------------------------------
  void Initialize2D( app::util::Heap *heap, void* pAppLytBuff );

  //----------------------------------------------------------------------
  /**
   *	@brief  初期テキスト表示
   */
  //----------------------------------------------------------------------
  void InitializeText( app::util::Heap *heap );
  //----------------------------------------------------------------------
  /**
   *	@brief  初期テキスト表示
   */
  //----------------------------------------------------------------------
  void FinalizeText( void );


  bool GetTrainerNo( const gfl2::str::StrBuf* buf, int* args );

  void DispFieldName(void );

  
private:
  app::util::Heap*          m_heap;           ///< ヒープ
  FieldMessageSampleDrawListener*        m_uiListener;     ///< UIイベントリスナー
  app::util::AppRenderingManager* mpAppRenderingManager;
  gfl2::str::MsgData* mpMsgData;       ///< テストメッセージデータ
  gfl2::str::MsgData* mpTrNameMsgData;       ///< テストメッセージデータ
  gfl2::str::StrBuf*   mpTempStrBuf;
  gfl2::str::StrBuf*   mpTempStrBuf2;
  gfl2::str::StrBuf*   mpTrainerStrBuf;
	print::WordSet* mpWordSet;

	gfl2::str::MsgData* mpScriptMsgData;
	print::WordSet* mpScriptWordSet;

};

GFL_NAMESPACE_END( FieldMessageTest )
GFL_NAMESPACE_END( Test )

#endif	/* __FIELD_MESSAGE_SAMPLE_DRAW_H__ */

#endif // PM_DEBUG
