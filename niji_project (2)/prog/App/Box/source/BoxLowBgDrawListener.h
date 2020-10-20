//==============================================================================
/**
 * @file	  BoxLowBgDrawListener.h
 * @brief	  描画からファンクションへリスナー
 * @author	k.ohno
 * @date	  February 18, 2015
 */
// =============================================================================

#if !defined( __BOX_LOW_BG_DRAW_LISTENER_H__ )
#define	__BOX_LOW_BG_DRAW_LISTENER_H__
#pragma once

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

class BoxLowBgDrawListener {


public:

	enum{
    LOWBG_ACTION_CANCEL,
    LOWBG_ACTION_DECIDE,
    LOWBG_ACTION_UP,
    LOWBG_ACTION_DOWN,
    LOWBG_ACTION_LEFT,
    LOWBG_ACTION_RIGHT,
    LOWBG_ACTION_L_CHANGE,
    LOWBG_ACTION_R_CHANGE,
    LOWBG_ACTION_L_TEAM,
    LOWBG_ACTION_R_TEAM,
    LOWBG_ACTION_MODE_CHANGE,
    LOWBG_ACTION_MODE_CHANGE0,
    LOWBG_ACTION_MODE_CHANGE1,
    LOWBG_ACTION_MODE_CHANGE2,
    LOWBG_ACTION_SELECT_CHANGE,
    LOWBG_ACTION_JUDGE,
    LOWBG_ACTION_FIND,
    LOWBG_ACTION_FOLDER,
    LOWBG_ACTION_INFO,
    LOWBG_ACTION_CHAT_CHANGE,
    LOWBG_ACTION_CHAT,
    LOWBG_ACTION_BOX,
    LOWBG_ACTION_TEAM_NAME,
    LOWBG_ACTION_BOXBAR,
	};

  enum{
    SCROLL_TEAM_LEFT,
    SCROLL_TEAM_RIGHT,
    SCROLL_TEAM_RESET,
    SCROLL_PAGE_LEFT,
    SCROLL_PAGE_RIGHT,
    SCROLL_PAGE_RESET,
  };

  enum{
    LOWBG_FOLDER_ACTION_TOP = 0x100,
    LOWBG_FOLDER_ACTION_MARK0 = 0x100,
    LOWBG_FOLDER_ACTION_MARK1,
    LOWBG_FOLDER_ACTION_MARK2,
    LOWBG_FOLDER_ACTION_MARK3,
    LOWBG_FOLDER_ACTION_MARK4,
    LOWBG_FOLDER_ACTION_MARK5,
    LOWBG_FOLDER_ACTION_OK,
    LOWBG_FOLDER_ACTION_CANCEL,
    LOWBG_FOLDER_ACTION_UP,
    LOWBG_FOLDER_ACTION_DOWN,
    LOWBG_FOLDER_ACTION_LEFT,
    LOWBG_FOLDER_ACTION_RIGHT,
  };

  
  //--------------------------------------------------------------------------
  /**
   * @brief   ボタン押下通知
   */
  //--------------------------------------------------------------------------
  virtual void OnButtonAction( int button_no ) = 0;

  //--------------------------------------------------------------------------
  /**
   * @brief   ボタン押下通知の事前検査
   */
  //--------------------------------------------------------------------------
  virtual bool PreButtonAction( int button_no ){ return false; }

  
  //--------------------------------------------------------------------------
  /**
   * @brief   キー操作通知
   * @param		dir  キー方向
   * @retval  true = 操作実行
   * @retval  false = 操作失敗
   */
  //--------------------------------------------------------------------------
  virtual bool OnKeyAction( u32 dir ){ return false; }




  //--------------------------------------------------------------------------
  /**
   * @brief   初期位置キープ開始
   */
  //--------------------------------------------------------------------------
  virtual void OnMousePokemonStart(int x,int y){};
  
  //--------------------------------------------------------------------------
  /**
   * @brief   掴みに変更
   */
  //--------------------------------------------------------------------------
  virtual void OnMousePokemonHand(int x,int y, int cursor){};

  
  //--------------------------------------------------------------------------
  /**
   * @brief   配置   //ペイン同士で判定
   * @return  何も行わない場合 false
   */
  //--------------------------------------------------------------------------
  virtual bool OnMousePokemonPut(int cursorno,int targetno){ return false; };


  //--------------------------------------------------------------------------
  /**
   * @brief   タッチの直後
   */
  //--------------------------------------------------------------------------
  virtual bool OnMousePokemonFirstTouch(int cursorno){return false;};
  
  //--------------------------------------------------------------------------
  /**
   * @brief   メニューをひらく
   */
  //--------------------------------------------------------------------------
  virtual void OnMouseMenuOpen(int cursorno){};

  //--------------------------------------------------------------------------
  /**
   * @brief   カーソル位置変更
   */
  //--------------------------------------------------------------------------
  virtual bool OnMousePokemonCurpos(int cursorno){ return false; };

  //--------------------------------------------------------------------------
  /**
   * @brief   ドラッグ中カーソル位置変更
   */
  //--------------------------------------------------------------------------
  virtual void OnMouseDragCurpos(int cursorno){};

  //--------------------------------------------------------------------------
  /**
   * @brief   何も無いところをタッチして話した時
   */
  //--------------------------------------------------------------------------
  virtual void OnNoTouchRelease(int cursorno){};


  //--------------------------------------------------------------------------
  /**
   * @brief   タッチの最初の検査
   * @param   touchしたかどうか
   * @return  これ以上処理しない場合true 通常false
   */
  //--------------------------------------------------------------------------
  virtual bool OnTouchFirstHook(bool ){ return false; }

  //--------------------------------------------------------------------------
  /**
   * @brief   インプットイネーブルの操作
   * @return  操作用リスナーならtrue 通常true
   */
  //--------------------------------------------------------------------------
  virtual bool GetInputEnableFlg(void){ return true; }

  //--------------------------------------------------------------------------
  /**
   * @brief   リリース時制御フラグ
   * @return  return trueならResetMainCursor を呼ぶ falseなら呼ばない
   */
  //--------------------------------------------------------------------------
  virtual bool IsReleaseControl(void){ return true; }



  
#if PM_DEBUG
  virtual void  DebugDisp(void)=0;
#endif
  
};

GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )

#endif	/* __BOX_LOW_BG_DRAW_LISTENER_H__ */
