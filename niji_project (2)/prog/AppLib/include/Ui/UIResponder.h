//============================================================================================
/**
 * @file    UIResponder.h
 * @brief    UI操作レスポンス関連
 * @author  suzuki_hisanao
 */
//============================================================================================
#ifndef __APP_UI_RESPONDER_H__
#define __APP_UI_RESPONDER_H__
#pragma once

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Tool/app_tool_ButtonManager.h>
#include <Layout/include/gfl2_Layout.h>
#include <ui/include/gfl2_UI.h>
#include "AppLib/include/Ui/UIInputListener.h"


namespace app {
namespace ui {

//#define  UIRESPONDER_USE_UI_GYROSCOPE      // コメントを外すとジャイロセンサーが作成されます
//#define  UIRESPONDER_USE_UI_ACCELEROMETER  // コメントを外すと加速度センサーが作成されます

typedef struct
{
  app::tool::ButtonManager::ButtonId button_id;
  gfl2::lyt::LytPane* picture_pane;
  gfl2::lyt::LytPane* bound_pane;
  u32                 touch_anime_index;        //!< タッチ時のアニメ
  u32                 release_anime_index;      //!< タッチ決定時のアニメ
  u32                 cancel_anime_index;       //!< タッチ状態からのキャンセルアニメ
  u32                 key_select_anime_index;   //!< キー決定時のアニメ ( touch => release の連続アニメ )
  u32                 active_anime_index;       //!< アクティブアニメ
  u32                 passive_anime_index;      //!< パッシブアニメ
}ButtonInfoEx;

struct ButtonInfoLytWk
{
  app::tool::ButtonManager::ButtonId button_id;
  gfl2::lyt::LytPane* picture_pane;
  gfl2::lyt::LytPane* bound_pane;
  gfl2::lyt::LytWk*   lyt_wk;                   //!< レイアウトワーク
  u32                 touch_anime_index;        //!< タッチ時のアニメ
  u32                 release_anime_index;      //!< タッチ決定時のアニメ
  u32                 cancel_anime_index;       //!< タッチ状態からのキャンセルアニメ
  u32                 key_select_anime_index;   //!< キー決定時のアニメ ( touch => release の連続アニメ )
  u32                 active_anime_index;       //!< アクティブアニメ
  u32                 passive_anime_index;      //!< パッシブアニメ
};

typedef struct
{
  app::tool::ButtonManager*  m_button_manager;
  nw::ut::LinkListNode       node; // Iteratorで使用されるノード(ユーザーの使用禁止）
}ButtonManageEraseNode;


class UIView;

class UIResponder : public app::tool::ButtonManager::IButtonCallback
{
public:

  // アナログスティックの入力検知閾値
  static const float STICK_INPUT_VALUE;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief    コンストラクタ
   *
   * @param    heap    ヒープ
   */
  //--------------------------------------------------------------------------------------------
  UIResponder( app::util::Heap * heap );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief    デストラクタ
   */
  //--------------------------------------------------------------------------------------------
  virtual ~UIResponder(void);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   タッチパッド、ボタン、アナログキーの入力イベント取得用リスナの登録
   *
   * @param   listener イベントリスナへ
   */
  //--------------------------------------------------------------------------------------------
  void SetInputListener( app::ui::UIInputListener* listener );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   タッチパッド、ボタン、アナログキーの入力検出有効設定
   *
   * @param   isEnable 入力検出を有効化するならtrue、無効化するならfalseを指定
   */
  //--------------------------------------------------------------------------------------------
  void SetInputEnabled( bool isEnable );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   SetInputEnabled を呼び出した後にコールされます
   * @note    UIView側で、自身の子Viewのみの入力制御を行いたい場合等に使用します
   *
   * @param   isEnable 入力検出を有効化するならtrue、無効化するならfalseを指定
   */
  //--------------------------------------------------------------------------------------------
  virtual void OnSetInputEnabled( bool isEnable );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   タッチパッド、ボタン、アナログキーの入力検出有効判定
   *
   * @return  isEnable 入力検出が有効ならtrue、無効ならfalseを返却
   */
  //--------------------------------------------------------------------------------------------
  bool IsInputEnabled( void ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   タッチパッドの入力検出有効設定
   *
   * @param   isEnable タッチパッド入力検出を有効化するならtrue、無効化するならfalseを指定
   */
  //--------------------------------------------------------------------------------------------
  void SetTouchEnabled( bool isEnable );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   タッチパッドの入力検出有効判定
   *
   * @return  isEnable 入力検出が有効ならtrue、無効ならfalseを返却
   */
  //--------------------------------------------------------------------------------------------
  bool IsTouchEnabled( void ) const;

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  入力の排他設定を行う
  *         排他処理を有効にした場合、該当ウインドウ以降の親ウインドウの入力を全て無効化する
  *
  * @param  isExclusive 排他制御するならtrue、排他制御を解除するならfalseを指定
  */
  //--------------------------------------------------------------------------------------------
  void SetInputExclusive( bool isExclusive );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  入力の排他設定を取得する
  *
  * @return isExclusive 自身の親ウインドウ以降の入力を排他制御するならtrueを返却する
  */
  //--------------------------------------------------------------------------------------------
  bool IsInputExclusive( void ) const;

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  アニメ中に操作を排他するかどうかを設定する。(デフォルトはtrue)
  *
  * @param  isAnimeExclusive  trueならばアニメ中操作排他　falseならばアニメ中でも排他処理しない
  */
  //--------------------------------------------------------------------------------------------
  void SetAnimationExclusive( bool isAnimeExclusive );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  アニメ中に操作を排他するかどうかを取得する
  *
  * @return m_isAnimeExclusive trueならば排他　falseならば排他しない
  */
//--------------------------------------------------------------------------------------------
  bool IsAnimationExclusive( void ) const;

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  ボタンのアニメ状態を取得する
  *
  * @return trueアニメ中  falseアニメしていない
  *
  * @note この関数をオーバーロードすることで、アニメーション排他を外部で設定できる
  *       ただしボタンマネージャーのチェックがなくなるので、それも外部で行うこと
  * 
  *     例）
  *       bool MailListView::IsButtonAnimation( void ) const
  *       {
  *         bool ret = this->app::ui::UIResponder::IsButtonAnimation();
  *       
  *         if( ret || m_pane_list_result != app::tool::PaneList::RET_NONE )
  *         {
  *           return true;
  *         }
  *         else
  *         {
  *           return false;
  *         }
  *       }
  */
  //--------------------------------------------------------------------------------------------
  virtual bool IsButtonAnimation( void ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ボタンマネージャの生成
   *
   * @param   heap        ヒープクラス
   * @param   layout      レイアウトクラス
   * @param   paneListPtr 登録するベースペインＩＤリスト
   * @param   paneNum     登録するペイン数
   */
  //--------------------------------------------------------------------------------------------
  void CreateButtonManager( app::util::Heap* heap, gfl2::lyt::LytWk* layout, const u32* paneListPtr, u32 paneNum );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ボタンマネージャの生成
   *
   * @param   heap            ヒープクラス
   * @param   layout          レイアウトクラス
   * @param   paneInfoListPtr 登録するベースペイン情報リスト
   * @param   paneNum         登録するペイン数
   */
  //--------------------------------------------------------------------------------------------
  void CreateButtonManager( app::util::Heap* heap, gfl2::lyt::LytWk* layout, const ButtonInfoEx* paneListPtr, u32 paneNum );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ボタンマネージャの生成
   *
   * @param   heap            ヒープクラス
   * @param   paneInfoListPtr 登録するベースペイン情報リスト
   * @param   paneNum         登録するペイン数
   */
  //--------------------------------------------------------------------------------------------
  void CreateButtonManager( app::util::Heap* heap, const ButtonInfoLytWk* paneListPtr, u32 paneNum );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ボタンマネージャの解放
   */
  //--------------------------------------------------------------------------------------------
  void ReleaseButtonManager( void );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ボタンの有効設定
   *
   * @param   buttonID  ボタン番号
   * @param   isEnable  ボタン有効フラグ
   * @param   isAnime   trueの指定によりボタンアニメーションを再生
   */
  //--------------------------------------------------------------------------------------------
  void SetButtonEnable( u32 buttonID, bool isEnable, bool isAnime=true );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ボタンマネージャの再生成
   *
   * @param   heap        ヒープ
   * @param   layout      レイアウト
   * @param   paneListPtr 登録ペインテーブル
   * @param   paneNum     登録ペイン数
   */
  //--------------------------------------------------------------------------------------------
  void ReloadButtonManager( app::util::Heap* heap, gfl2::lyt::LytWk* layout, const u32* paneListPtr, u32 paneNum );
  void ReloadButtonManager( app::util::Heap* heap, gfl2::lyt::LytWk* layout, const ButtonInfoEx* paneListPtr, u32 paneNum );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ボタンマネージャの取得
   *
   * @return  ボタンマネージャのインスタンス
   */
  //--------------------------------------------------------------------------------------------
  app::tool::ButtonManager* GetButtonManager( void );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   UpdateResponse()実行時にて入力操作が行われたかを判定
   *
   * @return  入力操作が検知されたならtrueを返却する
   */
  //--------------------------------------------------------------------------------------------
  bool IsInputAction( void ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   カーソル
   *
   * @param   pkey   十字ボタン情報
   * @param   pStick アナログスティック情報
   *
   * @return  移動方向をあらわすビットフラグ
   *          gfl::ui::BUTTON_UP, gfl::ui::BUTTON_DOWN, gfl::ui::BUTTON_LEFT, gfl::ui::BUTTON_RIGHT
   *          のビットフラグが返却される
   */
  //--------------------------------------------------------------------------------------------
  static u32 GetDirectionFlag( const gfl2::ui::Button& pKey, const gfl2::ui::VectorDevice& pStick );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief 十字キー(CrossKey)、ボタン(Button)にスティック（Stick）の操作を加える
   *
   * @param is_on  加える（デフォルト） false 加えない
   */
  //--------------------------------------------------------------------------------------------
  void SetStickEmulationMode( bool is_on );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief ボタンアニメ中にキー入力を効かなくするかどうか
   *
   * @param is_on true  許可  false不許可（デフォルト）
   */
  //--------------------------------------------------------------------------------------------
  void SetInputExclusiveKeyByButtonAnimation( bool is_on );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief ボタンアニメ中にキー入力を効かなくするかどうかを取得する
   *
   * @return true  許可  false不許可（デフォルト）
   */
  //--------------------------------------------------------------------------------------------
  bool GetInputExclusiveKeyByButtonAnimation( void ){ return m_isKeyExclusiveByButtonAnime; };



public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief   タッチパネルのタッチ状態を判定
   *
   * @retval true  タッチパネルがタッチ状態
   * @retval false タッチパネルは未タッチ
   */
  //--------------------------------------------------------------------------------------------
  bool   IsHoldTouchPanel( void ) const;

public:
  // ボタンマネージャ用リスナ
  virtual void CallbackFuncOnButtonSelected( u32 button_id );
  virtual void CallbackFuncOnButtonLongPressed( u32 button_id );
  virtual void CallbackFuncOnButtonSelectedBeforeAnime( u32 buttonID );
  virtual void CallBackFuncOnButtonTouchTrigger( u32 buttonID );
protected:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief   UIResponder関連定期処理
   *
   * @param   isEnable 呼び出し側にて入力を禁止する場合はfalseを指定する
   */
  //--------------------------------------------------------------------------------------------
  void UpdateResponse( UIView* pUIViewThis ,bool isEnable=true );

private:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief    UIResponder関連作成
   *
   * @param    heap    ヒープ
   */
  //--------------------------------------------------------------------------------------------
  void Create( app::util::Heap * heap );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief    UIResponder関連削除
   */
  //--------------------------------------------------------------------------------------------
  void Delete( void );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ボタンマネージャのクリーンナップ
   */
  //--------------------------------------------------------------------------------------------
  void CleanupButtonManager( void );


private:
  typedef nw::ut::LinkList<ButtonManageEraseNode, offsetof(ButtonManageEraseNode,node)> ButtonManageEraseList;

  ButtonManageEraseList      m_buttonEraseList;

protected:
  app::tool::ButtonManager*  m_button_manager;    //!< ボタンマネージャ
  app::ui::UIInputListener*  m_uiListener;        //!< UIResponderイベントリスナ

  gfl2::ui::DeviceManager*    m_uiDeviceManager;   //!< 入力デバイス管理オブジェクト
  gfl2::ui::VectorDevice*     m_key;               //!< 十字キー
  gfl2::ui::VectorDevice*     m_stick;             //!< アナログスティック
  gfl2::ui::Button*           m_button;            //!< ボタン
  gfl2::ui::TouchPanel*       m_touchPanel;        //!< タッチパネル
  bool                       m_buttonLock;        //!< ボタン同時検出回避用フラグ
  bool                       m_isInputEnable ;    //!< 入力の有効フラグ
  bool                       m_isExclusive;       //!< 入力排他フラグ
  bool                       m_isAnimeExclusive;  //!< アニメ中の操作排他
  bool                       m_isTouchEnable;     //!< タッチパッド入力の有効フラグ
  bool                       m_isKeyExclusiveByButtonAnime;    //!< ボタンアニメ中にキー入力を効かなくするかどうか

#ifdef  UIRESPONDER_USE_UI_GYROSCOPE
  gfl::ui::Gyroscope * m_gyroscope;              // ジャイロ
#endif  // UIRESPONDER_USE_UI_GYROSCOPE
#ifdef  UIRESPONDER_USE_UI_ACCELEROMETER
  gfl::ui::Accelerometer * m_accelerometer;    // 加速度センサー
#endif  // UIRESPONDER_USE_UI_ACCELEROMETER
};


}    // ui
}    // app


#endif  // __APP_UI_RESPONDER_H__
