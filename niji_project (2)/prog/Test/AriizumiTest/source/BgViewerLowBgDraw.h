//==============================================================================
/**
 * @file	  BgViewerLowBgDraw.h
 * @brief	  表示のサンプル
 * @author	k.ohno
 * @date	  January 18, 2015
 */
// =============================================================================

#if !defined( __BOX_LOW_BG_DRAW_H__ )
#define	__BOX_LOW_BG_DRAW_H__
#pragma once
#if PM_DEBUG

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/app_util_2d.h>
#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>

#include "BgViewerLowBgDrawListener.h"
#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Util/AppRenderingManager.h"

#include "Test/AriizumiTest/include/BgViewAppParam.h"
#include "Print/include/WordSet.h"
#include "str/include/gfl2_Str.h"



GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Ariizumi )

/// タイトル用ビュー
class BgViewerLowBgDraw : public app::ui::UIView, public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(BgViewerLowBgDraw);

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
//  BgViewerLowBgDraw( app::util::Heap * heap,gfl2::lyt::LayoutRenderPath* pLayoutRenderPath );
  BgViewerLowBgDraw( app::util::Heap * heap,app::util::AppRenderingManager* pAppRanderingManager );

  /// デストラクタ
  virtual ~BgViewerLowBgDraw();




  
public:
  /// このビュー専用のUIリスナー設定
  void SetUIListener( BgViewerLowBgDrawListener *listener );

public:
  /// 更新
  virtual void Update( void );
  /// 描画
  virtual void Draw( gfl2::gfx::CtrDisplayNo no );

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


  void SkyboxOnOff( bool bOn );
  void AnimChange( int no );
  void BackGroundChange( int no , int pos);
  void AnimStartOnOff(bool bAnim);

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




  void MyCreateButtonManager( void );



private:
  app::util::Heap*          mpAppHeap;           ///< ヒープ
  BgViewerLowBgDrawListener*        m_uiListener;     ///< UIイベントリスナー
  app::util::AppRenderingManager* mpAppRenderingManager;
  gfl2::str::MsgData* mpMsgData;       ///< テストメッセージデータ
  gfl2::str::StrBuf*   mpTempStrBuf;
  gfl2::str::StrBuf*   mpTempStrBuf2;
	print::WordSet* mpWordSet;

  APP_PARAM* mpAppParam;


  
};



GFL_NAMESPACE_END( Ariizumi )
GFL_NAMESPACE_END( Test )



#endif //PM_DEBUG

#endif	/* __BOX_LOW_BG_DRAW_H__ */

