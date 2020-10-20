//==============================================================================
/**
 * @file	  BoxLowBattleInDraw.h
 * @brief	  バトル選択時の挙動の管理
 * @author	k.ohno
 * @date	  2015.9.24
 */
// =============================================================================

#if !defined( __BOX_LOW_BATTLE_IN_DRAW_H__ )
#define	__BOX_LOW_BATTLE_IN_DRAW_H__
#pragma once

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/app_util_2d.h>
#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>

#include "BoxLowBgDraw.h"
#include "BoxLowBattleInDrawListener.h"
#include "AppLib/include/Util/app_util_heap.h"

#include "App/Box/include/BoxAppParam.h"
#include "Print/include/WordSet.h"
#include "str/include/gfl2_Str.h"

#include "AppLib/include/Tool/app_tool_PaneListView.h"

#include "AppLib/include/Tool/app_tool_MenuWindowListener.h"

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN(tool)
class PokeIcon;
class ItemIcon;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END( app )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

/// MARKビューはUIViewを継承しない
class BoxLowBattleInDraw : public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(BoxLowBattleInDraw);

private:
    enum
    {
      LYT_WORK_NORMAL=0,
      LYT_WORK_BG,
      LYT_WORK_UPPBG,
      LYT_WORK_MAX,
    };

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



  typedef enum{
    BTN_ID_CANCEL = 0,     ///< タイトルイン
    BTN_ID_Y,           ///< 
    BTN_ID_INFO,        ///< 
    BTN_ID_BOXLIST,        ///< 
    BTN_ID_R1,
    BTN_ID_L1,
    BTN_ID_R2,
    BTN_ID_L2,
    BTN_ID_JUDGE,   ///<ジャッジ画面

  } BUTTON_ID_NUM;


public:

  /**
   * @brief コンストラクタ
   * @param heap  ヒープ
   */
  BoxLowBattleInDraw( BoxLowBgDraw* pBoxLowBgDraw );

  /// デストラクタ
  virtual ~BoxLowBattleInDraw();




  
public:
  /// このビュー専用のUIリスナー設定
  void SetUIListener( BoxLowBattleInDrawListener *listener );

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


	int SetCursorPos(int no,int udlr );


public:
  /// タッチパネルイベントの検知
  virtual ::app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  /// ペインアクション通知用リスナ
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );
  /// キー入力通知用リスナ
  virtual ::app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick);

  void Init(void);

  bool End(void);
  
private:

  BoxLowBgDraw* mpBoxLowBgDraw;

	BoxLowBattleInDrawListener* mpUiListener;
  
  int mButtonPos;   //今のボタンの位置
  int mKeepButtonPos;  //覚えておくボタンの位置



};



GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )




#endif	/* __BOX_LOW_BATTLE_IN_DRAW_H__ */

