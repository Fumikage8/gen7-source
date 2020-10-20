//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		app_tool_ScrollPaneManager.h
 *	@brief  スクロールペインマネージャー
 *	@author	Toru=Nagihashi
 *	@date		2012.06.28
 *	
 *	@note セルリストとはペインリストとは異なり横と縦それぞれに選択マスがあり、
 *	      かつスクロールするリストのことです。
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#if !defined( __APP_TOOL_SCROLLPANEMANAGER_H__)
#define __APP_TOOL_SCROLLPANEMANAGER_H__

#include "AppLib/include/Tool/app_tool_ScrollPane.h"
#include "AppLib/include/Tool/app_tool_ButtonManager.h"
#include "AppLib/include/Tool/app_tool_MenuCursor.h"
#include "AppLib/include/Tool/app_tool_ScrollPane.h"

#include <AppLib/include/Ui/UIInputListener.h>
#include <AppLib/include/Ui/UIView.h>

#include <AppLib/include/Util/app_util_heap.h>

#include <Layout/include/gfl2_Layout.h>
#include <math/include/gfl2_math.h>
#include <math/include/gfl2_Transporter.h>
#include <Prog/GameSys/include/GameFrameMode.h>
#include <ui/include/gfl2_UI.h>

namespace app { namespace tool { class TouchPanelOperator; } }


GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )

class ScrollPaneManager : public app::ui::UIView, public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(ScrollPaneManager);
public:
  //-----------------------------------------------------------------------
  // 設定用構造体
  //-----------------------------------------------------------------------
  /**
   * @brief リスナー
   */
  class Listener
  {
  public:
    /**
     * @brief カーソル移動時のコールバック関数
     *
     * @param pane_index  カーソルがあっているペインのインデックス
     * @param dataID  データID
     */
		virtual void OnCursor( u32 pane_index, u32 dataID ){}
    
    // リスト決定時のコールバック
    /**
     * @brief リスト決定時のコールバック関数
     *
     * @param pane_index  カーソルがあっているペインのインデックス
     * @param dataID  データID
     */
    virtual void Decide( u32 pane_index, u32 dataID ){}
    
    /**
     * @brief リスト決定アニメ前のコールバック関数
     *
     * @param pane_index  カーソルがあっているペインのインデックス
     * @param dataID  データID
     */
    virtual void DecideBeforeAnime( u32 pane_index, u32 dataID ){}
    /**
     * @brief スクロール中に呼ばれ続けるコールバック
     * @param move  移動量
     *  
     * @note  ボタンスクロール、ドラッグ、ドラッグ後の慣性
     */
    virtual void OnScroll( f32 move ){}
    
    /**
     * @brief キー移動完了
     *
     * @param pane_index  カーソルがあっているペインのインデックス
     * @param dataID  データID
     */
    virtual void OnFinishKeyMove( u32 pane_index, u32 dataID ){}
    /**
     * @brief ペインへの書き込み時、非同期読み込みを使うかどうか
     *
     * @return true 使う  false 使わない
     * 
     * @note  
     *      ・非同期を使う場合
     *          ペインが画面内にでる瞬間にStartLoadTextureが呼ばれます。
     *          StartLoadTexture内でテクスチャの読み込み開始をしてください。
     *          StartLoadTexture直後にWriteTextureが呼ばれますが、
     *          これはis_end_asyncがfalseです。
     *          このWriteTexture内でダミーだったり非同期しないものを
     *          書いたりしてください。
     *          IsLoadedTextureに非同期読みが完了したかを書いてください。
     *          非同期読みが完了するとWriteTexture(is_end_async=true)が呼ばれます。
     * 
     *      ・使わない場合
     *          WriteTexture(is_end_async=true)がペインが画面内にでる瞬間に呼ばれるので
     *          書き込んでください。
     *          IsLoadedTextureとStartLoadTextureは使わなくてよいです
     *
     */
    virtual bool IsAsyncWrite( void ){ return false; }
    /**
     * @brief リストの項目を描画するコールバック関数
     *
     * @param pane_index  カーソルがあっているペインのインデックス
     * @param dataID  データID
     * @param is_end_async  true非同期ではないor非同期で読み込みが終わっている
     *                      false非同期読み込み終わっていない
     */
		virtual void WriteTexture( u32 pane_index, u32 dataID, bool is_end_async ){}
    /**
     * @brief 書き込み用の非同期読み込みが完了したかのコールバック関数
     *
     * @param pane_index  カーソルがあっているペインのインデックス
     * @param dataID  データID
     * 
     * @return true読み込み完了 false読み込み中
     */
    virtual bool IsLoadedTexture( u32 pane_index, u32 dataID ) { return true; }
    /**
     * @brief 書き込み用の非同期読み込みを開始するコールバック関数
     *
     * @param pane_index  カーソルがあっているペインのインデックス
     * @param dataID  データID
     * 
     * @retval true 読み込みリクエストを受け付けた false 受付なかった
     *
     * @note リクエストを受け付けなかった時は、何度も開始処理を試みますので、
     *      この中でリクエスト開始以外を行うと重くなります。
     */
    virtual bool StartLoadTexture( u32 pane_index, u32 dataID ){ return true; }
    
    /**
     * @brief ForEach関数を読んだ時に呼ばれるコールバック
     *
     * @param param ForEachに渡した値（ForEachを用途別で使いたいときの認識コードを想定）
     * @param pane_index  ペインのインデックス（無しの場合もある）
     * @param dataID      データID
     */
    virtual void ForEachUpdate( int param, bool is_use_pane, u32 pane_index, u32 dataID ){ }
  };
 
  /**
   * @brief スクロールさせるペインの情報
   */
  struct PaneInfo
  {
    gfl2::lyt::LytPane* root_pane;      // 位置、表示設定用
    gfl2::lyt::LytPane* picture_pane;   // テクスチャを書き込むペイン
    gfl2::lyt::LytPane* bounding_pane;  // 当たり判定ペイン
    gfl2::lyt::LytPane* cursor_pane;    // カーソル位置ペイン
    u32                         hold_anime_index;     //トリガ時のアニメ
    u32                         release_anime_index;  //タッチで選んだ時のアニメ
    u32                         cancel_anime_index;   //ホールドしながら枠外へスライドしたときのアニメ
    u32                         select_anime_index;   //キーで選んだ時のアニメ
  };
  
  /**
   * @brief 動作モード
   */
  enum Mode
  {
    MODE_NORMAL,        //一度タッチすると決定されるモード
    MODE_DOUBLE_TOUCH,  //一度タッチするとカーソルがあい、
                        //カーソルがあったものにタッチすると決定されるモード
  };
    
  /**
   * @brief 設定構造体
   */
  struct Description
  {
    app::util::Heap * heap;
		gfl2::lyt::LytWk * layout_work;		///<レイアウトワーク
    gfl2::ui::DeviceManager * ui_device_manager; ///<デバイスマネージャ  使わない

		PaneInfo * pane_table;	//ペインデータ
    u32 pane_table_num;           //ペインデータの数

    s32 list_data_num;  //リストに表示する数
    s32 col;            //列→方向個数（X) 最低1以上
    // s32 row;         //行↓方向（Y）
    
		s32	pane_start_x;		// ペインのデフォルトX座標 ( listPos = 0 の位置のペイン座標、この位置からcol個並ぶ )
		s32	pane_start_y;		// ペインのデフォルトY座標 ( listPos = 0 の位置のペイン座標、この位置からならぶ )
		s16	pane_width;			// ペインのXサイズ
		s16	pane_height;		// ペインのYサイズ
    
    //下記はペイン表示エリア  エリア外へいくと
    s16 pane_draw_area_left;  //ペインを表示する領域  左（レイアウト座標）
    s16 pane_draw_area_top;   //ペインを表示する領域  上（レイアウト座標）
    s16 pane_draw_area_down;  //ペインを表示する領域  下（レイアウト座標）
    s16 pane_draw_area_right; //ペインを表示する領域  右（レイアウト座標）
    
    //下記は入力エリアの開始座標(左上の座標値を指定)
    s32 pane_input_start_x;   // ペイン入力エリアの開始X座標(レイアウト座標)
    s32 pane_input_start_y;   // ペイン入力エリアの開始Y座標(レイアウト座標)

    //下記はペインへの入力エリア
    s16 pane_input_limit_col; //ペインへ入力できる領域  行
    s16 pane_input_limit_row;  //ペインへ入力できる領域  列

    GameSys::FrameMode framerate;  //ゲームフレームレート
    
    app::tool::MenuCursor *menu_cursor; //メニューカーソル
    bool put_cursor_visible;            //カーソルの位置設定時にカーソルを表示するかどうか(true = Put, false = PutNonVisible)

    Listener  *listener;  //リスナー
    
    u16 default_cursor_pos; //デフォルト位置

    Mode  mode;
	};

public:
  //-----------------------------------------------------------------------
  // 生成・破棄
  //-----------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   *
   * @param desc  設定構造体
   */
  ScrollPaneManager( const Description & desc );
  /**
   * @brief デストラクタ
   */
  virtual ~ScrollPaneManager();
  //-----------------------------------------------------------------------
  // 更新
  //-----------------------------------------------------------------------
  /**
   * @brief 更新
   */
  virtual void Update( void );
  /**
   * @brief 描画
   */
  virtual void Draw( gfl2::gfx::CtrDisplayNo no ){ GFL_UNUSED( no ); }
  
  //-----------------------------------------------------------------------
  // 操作
  //-----------------------------------------------------------------------
  /**
   * @brief   タッチパッド、ボタン、アナログキーの入力検出有効設定
   *
   * @param   isEnable 入力検出を有効化するならtrue、無効化するならfalseを指定
   *
   * @return  none
   */
  void SetInputEnabled( bool isEnable );
  //-----------------------------------------------------------------------
  /**
   * @brief   ビュー自体の入力可能状態を設定する
   *
   * @param   isEnable falseで入力を検出した状態
   */
  void SetViewInputEnabled( bool isEnable );

  /**
   * @brief   アナログキーの入力検出有効設定
   *
   * @param   isEnable 入力検出を有効化するならtrue、無効化するならfalseを指定
   *
   * @return  none
   */
  void SetInputKeyEnabled( bool isEnable );


  /**
   * @brief スクロール現在値の取得
   *
   * @return スクロール現在値の取得
   * 
   * @note ScrollBarにセットするための値です。
   */
  s16 GetScrollValue( void ) const;
  /**
   * @brief スクロール値の設定
   *
   * @param scroll  0〜GetScrollMaxまでの値
   * 
   * @note  スクロールバーをドラッグした時などに設定してください
   *        →RequestScrollを使用すること
   */
  // void SetScrollValue( s16 scroll );
  /**
   * @brief スクロール最大値の取得
   *
   * @return スクロール最大値
   * 
   * @note  ScrollBarにセットするための値です。
   */
  s16 GetScrollMax( void ) const;
  /**
   * @brief リストすべてを呼ぶ
   *
   * @param param ForEachUpdate関数に渡る値
   */
  void ForEach( int param );
  /**
   * @brief 入力検知フラグセット
   */
  void SetButtonInputEnable( u32 dataID, bool is_enable );
  
  /**
   * @brief じょうたいを取得
   *
   * @return State
   */
  enum State
  {
    STATE_INPUT_WAIT,   ///<入力待ちスクロールしていない
    STATE_KEY_SCROLL,   ///<十字キーでの動作
    STATE_DRAG_SCROLL,  ///<ドラッグでの動作
    STATE_AUTO_SCROLL,  ///<ドラッグで離した時等の動作
    STATE_REQ_SCROLL,   ///<RequestScrollを使ったときの動作
    STATE_DECIDE_ANIME, ///<決定やAボタン時等のボタンアニメ時
  };
  State GetState( void ) const;

  /**
   * @brief メニューカーソルの表示
   *
   * @param is_visible true表示 false非表示
   */
  void SetVisibleMenuCursor( bool is_visible );

  /**
   * @brief スクロール値の設定リクエスト
   *
   * @param scroll  0〜GetScrollMaxまでの値
   * 
   * @note  スクロールバーをドラッグした時などに設定してください
   */
  void RequestScroll( s16 scroll );

  /**
   * @brief スクロール可能かどうか
   *
   * @return true可能 false不可能
   */
  bool IsScrollable( void ) const;
  
  /**
   * @brief 更新フラグ
   *
   * @param is_enable
   */
  void SetUpdateEnableFlag( bool is_enable );
  
  /**
   * @brief ボタンのホールド状況をリセットする
   */
  void ResetHoldState( void );
public:
  //継承
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );
  virtual ::app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );
  virtual ::app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  virtual void OnLayoutPaneStartingAnimeEvent( const u32 paneId );
  void OnLayoutPaneTouchTriggerEventMine( const u32 paneId ); //->そもそもpassiveは検知できないので自前で動かす

private:
  //状態定義
  typedef void (ScrollPaneManager::*StateFunction)( void );
  typedef nw::ut::LinkList<ScrollPane, offsetof(ScrollPane,node)>  PaneList;
  
  static const int KEY_HOLD_SPEED_LEN = 4;
  static const int KEY_HOLD_SPEED[ KEY_HOLD_SPEED_LEN ];
  static const u32 KEY_HOLD_SPEED_CHANGE_CNT = 6;

private:
  //-----------------------------------------------------------------------
  // PRIVATE 生成・破棄
  //-----------------------------------------------------------------------
  void CreateButtonManager( void );
  void CreateButton( void );
  void CreateScrollPane( void );
  void DeleteScrollPane( void );
  //-----------------------------------------------------------------------
  // PRIVATE 状態
  //-----------------------------------------------------------------------
  void UpdateState( void );
  //状態
  void StateInputWait( void );  ///<入力待ち
  void StateKeyScroll( void );  ///<十字キーでの動作
  void StateDragScroll( void ); ///<ドラッグなどでの動作
  void StateAutoScroll( void ); ///<ドラッグではじいたときなどでの動作
  void StateRequestScroll( void ); ///<RequestScrollを使った時の動作
  void StateDecideAnime( void );  ///決定アニメ待ち
  
  //状態制御
  void NextState( StateFunction seqence );
  bool IsCompareState( StateFunction seqence ) const;
  
  //状態内シーケンス制御
  void IncStateSeq( void );
  u32 GetStateSeq( void ) const;
  //-----------------------------------------------------------------------
  // PRIVATE スクロールペインの管理
  //-----------------------------------------------------------------------
  void AddActivePane( u32 dataID );
  void SetScrollPanePosition( void );
  void UpdateScrollPane( void );
  void AssignUsingCharaIcon( ScrollPane* charaPane );
  void RemoveUsingCharaIcon( ScrollPane* charaPane );
  void UpdateEnableButton( ScrollPane* charaPane );
  void UpdateImagePanePosition( ScrollPane* charaPane );
  bool AssignImagePane( ScrollPane* charaPane );
  void RemoveImagePane( ScrollPane* charaPane );
  void ReplacePlayerIconTexture( ScrollPane* charaPane );
  
  //-----------------------------------------------------------------------
  // PRIVATE スクロール関係
  //-----------------------------------------------------------------------
  
  //カーソルを次に移動させる
  void SetCursor( s16 next_col, s16 next_row );
  void UpdateCursorPos( void );
  bool CanMoveCursor( s16 next_col, s16 next_row ) const; ///<カーソル移動できるか
  bool IsMoveLimitCursor( s16 next_col, s16 next_row ) const;

  //スクロール処理
  void SetupScroll( s16 move_col, s16 move_row );
  void SetupScrollLast( void );
  void StartScroll( void );
  void UpdateScroll( void );
  bool IsEndScroll( void ) const;
  s16 GetCursorCol( void ) const;
  s16 GetCursorRow( void ) const;
  s16 GetGrobalCursor( void ) const;
  f32 GetIneriaRate( void ) const;
  
  void UpdateDragScroll( void );
  void UpdateGridScroll( bool is_force );
  bool IsOnGrid( void ) const; 
  void CalcCursorScroll( s16 global_cursor );

  void AddScrollValue( s16 value, bool play_se );

  //-----------------------------------------------------------------------
  // PRIVATE 入力関係
  //-----------------------------------------------------------------------
  gfl2::ui::Button * GetButton(void) const;
  gfl2::ui::VectorDevice * GetKey(void) const;

  bool IsTouchArea( s16 touch_x, s16 touch_y ) const; ///<タッチエリア内チェック
  void IncKeyHoldCount( void ); //キー押し続け速度
  void ResetKeyHoldCound( void );
  void Drag( s16 moveX, s16 moveY, bool isTouchTrigger );

  //-----------------------------------------------------------------------
  // PRIVATE ペイン操作
  //-----------------------------------------------------------------------
  void SetPanePos( gfl2::lyt::LytPaneIndex index, const gfl2::math::VEC3 & pos );
  void SetPaneVisible( gfl2::lyt::LytPaneIndex index, bool is_visible );
  bool GetPaneVisible( gfl2::lyt::LytPaneIndex index ) const;
  
  //-----------------------------------------------------------------------
  // PRIVATE 変換系
  //-----------------------------------------------------------------------
  u32 GetPaneIDByPos( s16 pos );
  ScrollPane * GetScrollPaneByPos( s16 pos );

  ScrollPanePosition GetPositionByPos( s16 pos ) const;

  s16 GetCol( s16 pos ) const;
  s16 GetRow( s16 pos ) const;
  s16 GetPos( s16 col, s16 row ) const;
  s16 GetGrobalPos( s16 pos ) const;
  s16 GetLocalPos( s16 pos ) const;
  
  //-----------------------------------------------------------------------
  // PRIVATE 変換系
  //-----------------------------------------------------------------------
  bool GetTriggerPane( s16 x, s16 y, u32 * pane );

private:
  Description m_desc;
  
  //状態遷移関係
  StateFunction  m_state; ///<状態
  u32 m_state_seq;  ///<状態内シーケンス
  
  //スクロール関係
  s16 m_scroll;         //スクロール値（ディスプレイ座標）
  s16 m_scroll_target;  //スクロール値（RequestScroll専用）
  s16 m_scroll_add;
  s16 m_inertia;        //慣性値 （ディスプレイ座標）
  gfl2::math::TransLiner<f32>  m_scroll_trans;  //スクロール移動用

  s16 m_next_scroll_cursor_pos; //スクロール移動後のカーソル位置
  
  s16 m_cursor_pos; //カーソルインデックス  カーソルは0~ GetPos( pane_input_limit_col, pane_input_limit_row )まで動く
                    // m_cursor_pos + (m_scroll / m_desc.pane_height)*m_desc.col が絶対座標(ただしm_scrollが止まっている時）
  ScrollPaneDrawArea  m_draw_area;
  ScrollPaneDrawArea  m_input_area;

  //入力関係
  u32 m_key_hold_count; ///<キー操作押し続けカウント
  u32 m_key_hold_speed_index; ///<キー操作押し続けカウント

  bool m_is_drag;
  bool m_is_req_cursor_pos;
  bool m_is_finish_key_move;
  bool m_is_update_enable;
  bool m_is_input_key_enable;

  PaneList  m_active_list;
  u8  * m_use_pane_flag;
  ScrollPane  *m_scroll_pane;
 
  gfl2::ui::Button * m_on_button_action;
  gfl2::ui::VectorDevice * m_on_key_action;
  s16   m_scroll_dir;
};

GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )

#endif		// __APP_TOOL_SCROLLPANEMANAGER_H__
