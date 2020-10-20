//=============================================================================
/**
 * @file    TrainerPassDraw.h
 * @brief   トレーナーパス画面描画処理
 * @author  Hiroyuki Nakamura
 * @date    2015.11.06
 */
//=============================================================================
#if !defined( TRAINER_PASS_DRAW_H_INCLUDED )
#define TRAINER_PASS_DRAW_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// niji
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"
#include "AppLib/include/Tool/AppToolTrainerIconRendering.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(Savedata)
class ZukanData;
class GameTime;
class PlayTime;
class Misc;
GFL_NAMESPACE_END(Savedata)

GFL_NAMESPACE_BEGIN(Field)
class EventWork;
GFL_NAMESPACE_END(Field)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(TrainerPass)

//=============================================================================
/**
 * @class TrainerPassDraw
 * @brief トレーナーパス画面描画クラス
 * @date  2015.10.02
 */
//=============================================================================
class TrainerPassDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
public:
  //=============================================================================
  /**
   * @class TrainerPassDraw
   * @brief トレーナーパス画面描画リスナークラス
   * @date  2015.10.02
   */
  //=============================================================================
  class DrawListener
  {
  public:
    //! 動作ID
    enum ActionID
    {
      ACTION_ID_A_BUTTON = 0,   //!< Aボタン
      ACTION_ID_B_BUTTON,       //!< Bボタン
      ACTION_ID_STAMP_L,        //!< スタンプ左矢印
      ACTION_ID_STAMP_R,        //!< スタンプ右矢印
    };

    //-----------------------------------------------------------------------------
    /**
     * @func    OnAction
     * @brief   入力時のリスナー
     * @date    2015.11.06
     *
     * @param   id    動作ID
     */
    //-----------------------------------------------------------------------------
    virtual void OnAction( TrainerPassDraw::DrawListener::ActionID id ) = 0;
  };

  //! スタンプ表示ID
  enum StampPutID
  {
    PUT_COPY = 0,   //!< ページ切り替え時のコピー場所
    PUT_DEFAULT,    //!< 通常表示場所
  };

  static const u32 NEW_STAMP_NONE = 0xffffffff;   //!< 新規追加スタンプなし


private:
  //! レイアウトID
  enum LyoutID
  {
    LYTID_UPPER = 0,  //!< 上画面
    LYTID_LOWER,      //!< 下画面
    LYTID_MAX         //!< 最大数
  };

  //! ボタンID
  enum ButtonID
  {
    BTNID_RETURN = 0,   //!< 戻る
    BTNID_STAMP_L,      //!< スタンプ左矢印
    BTNID_STAMP_R,      //!< スタンプ右矢印
    BTNID_MAX           //!< 最大数
  };

  static const s32 STAMP_PAGE_MAX = 15;   //!< スタンプのページ数

  static const f32 STAMP_PUT_SE_FRAME;      //!< スタンプを押したときのSEを再生するアニメフレーム
  static const f32 STAMP_RELEASE_SE_FRAME;  //!< スタンプを離したときのSEを再生するアニメフレーム


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap        ヒープ
   * @param   render_man  描画マネージャ
   * @param   is_demo     true = スタンプデモ
   */
  //-----------------------------------------------------------------------------
  TrainerPassDraw( app::util::Heap * heap, app::util::AppRenderingManager * render_man, bool is_demo );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~TrainerPassDraw();

  //-----------------------------------------------------------------------------
  /**
   * @func    SetDrawListener
   * @brief   リスナーをセット
   * @date    2015.11.06
   *
   * @param   listener リスナー
   */
  //-----------------------------------------------------------------------------
  void SetDrawListener( DrawListener * listener );

  //-----------------------------------------------------------------------------
  /**
   * @func    Init
   * @brief   初期化処理
   * @date    2015.11.06
   *
   * @param   res_buf   レイアウトリソースバッファ
   * @param   msg_buf   メッセージリソースバッファ
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Init( void * res_buf, void * msg_buf );

  //-----------------------------------------------------------------------------
  /**
   * @func    End
   * @brief   終了処理
   * @date    2015.11.06
   *
   * @retval  true  = 処理完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool End(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Start
   * @brief   開始処理
   * @date    2015.11.06
   *
   * @param   page  表示ページ
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Start( s32 page );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsStart
   * @brief   開始処理が終了したかをチェック
   * @date    2015.11.06
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsStart(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    StartPageChangeAnime
   * @brief   ページ切り替えアニメ開始
   * @date    2015.11.09
   *
   * @param   mv  切り替え方向
   */
  //-----------------------------------------------------------------------------
  void StartPageChangeAnime( s32 mv );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsEndPageChangeAnime
   * @brief   ページ切り替えアニメ終了チェック
   * @date    2015.11.09
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsEndPageChangeAnime(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetPage
   * @brief   ページ設定
   * @date    2015.11.09
   *
   * @param   page  ページ番号
   */
  //-----------------------------------------------------------------------------
  void SetPage( s32 page );

  //-----------------------------------------------------------------------------
  /**
   * @func    PuttPage
   * @brief   ページ表示
   * @date    2015.11.09
   *
   * @param   id        表示ID
   * @param   page      ページ番号
   * @param   new_page  追加するページ（スタンプ演出時の新規追加ページ）
   *
   * @param   通常時は new_page = NEW_STAMP_NONE
   */
  //-----------------------------------------------------------------------------
  void PutPage( StampPutID id, u32 page, u32 new_page = NEW_STAMP_NONE );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeStamp
   * @brief   スタンプ切り替え
   * @date    2015.11.09
   *
   * @param   id        表示ID
   * @param   page      ページ番号
   * @param   new_page  追加するページ（スタンプ演出時の新規追加ページ）
   *
   * @param   通常時は new_page = NEW_STAMP_NONE
   */
  //-----------------------------------------------------------------------------
  void ChangeStamp( StampPutID id, u32 page, u32 new_page = NEW_STAMP_NONE );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetStampAnime
   * @brief   スタンプアニメ再生
   * @date    2015.11.09
   *
   * @param   flg   true = 押す, false = 離れる
   */
  //-----------------------------------------------------------------------------
  void StartStampAnime( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsEndStampAnime
   * @brief   スタンプアニメ終了チェック
   * @date    2015.11.09
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsEndStampAnime(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PlaySE_StampPut
   * @brief   スタンプを押したときのSEを再生
   * @date    2015.11.13
   */
  //-----------------------------------------------------------------------------
  void PlaySE_StampPut(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PlaySE_StampRelease
   * @brief   スタンプを離したときのSEを再生
   * @date    2015.11.13
   */
  //-----------------------------------------------------------------------------
  void PlaySE_StampRelease(void);


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    GetGameData
   * @brief   GameData関連取得
   * @date    2015.11.07
   */
  //-----------------------------------------------------------------------------
  void GetGameData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update
   * @brief   更新
   * @date    2015.11.06
   */
  //-----------------------------------------------------------------------------
  virtual void Update(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Draw
   * @brief   描画
   * @date    2015.11.06
   *
   * @param   displayNo   描画面
   */
  //-----------------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  //-----------------------------------------------------------------------------
  /**
   * @func    Initialize2D
   * @brief   2D関連初期化
   * @date    2015.11.06
   *
   * @param   res_buf   レイアウトリソースバッファ
   * @param   msg_buf   メッセージリソースバッファ
   */
  //-----------------------------------------------------------------------------
  void Initialize2D( void * res_buf, void * msg_buf );

  //-----------------------------------------------------------------------------
  /**
   * @func    Terminate2D
   * @brief   2D関連削除
   * @date    2015.11.06
   */
  //-----------------------------------------------------------------------------
  void Terminate2D(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    WriteDefaultSrring
   * @brief   デフォルト文字列描画
   * @date    2015.11.06
   */
  //-----------------------------------------------------------------------------
  void WriteDefaultString(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdatePlayTimee
   * @brief   プレイ時間表示更新
   * @date    2015.11.07
   */
  //-----------------------------------------------------------------------------
  void UpdatePlayTime(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateButton
   * @brief   ボタン生成
   * @date    2015.11.07
   */
  //-----------------------------------------------------------------------------
  void CreateButton(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateTrainerIcon
   * @brief   トレーナーアイコン生成
   * @date    2015.11.11
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool CreateTrainerIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteTrainerIcon
   * @brief   トレーナーアイコン削除
   * @date    2015.11.11
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteTrainerIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    OnLayoutPaneEvent
   * @brief   ペインイベントの検知
   * @date    2015.11.06
   *
   * @param   button_id   通知されたボタンID
   *
   * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
   * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
   * @retval  ENABLE_ACCESS         = それ以外
   *
   * @note  override UIInputListener
   */
  //-----------------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 button_id );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnKeyAction
   * @brief   キー入力の検知
   * @date    2015.11.06
   *
   * @param   button  ボタン
   * @param   key     十字キー
   * @param   stick   アナログスティック
   *
   * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
   * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
   * @retval  ENABLE_ACCESS         = それ以外
   *
   * @note  override UIInputListener
   */
  //-----------------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick );


private:
  app::util::Heap * m_heap;                             //!< ヒープ
  app::util::AppRenderingManager * m_renderingManager;  //!< 描画マネージャ

  bool m_isStampDemo;   //!< true = スタンプデモ

  DrawListener * m_listener;  //!< リスナー

  const Savedata::MyStatus * m_myStatus;    //!< セーブデータ：プレイヤーデータ
  const Savedata::ZukanData * m_zukanData;  //!< セーブデータ：図鑑データ
  const Savedata::GameTime * m_gameTime;    //!< セーブデータ：ゲーム内時間
  const Savedata::PlayTime * m_playTime;    //!< セーブデータ：プレイ時間
  const Savedata::Misc * m_misc;            //!< セーブデータ：未分類
  const Field::EventWork * m_eventWork;     //!< イベントワーク

  gfl2::str::MsgData * m_pMsgData;  //!< メッセージデータ
  print::WordSet * m_pWordSet;      //!< 単語セット

  app::tool::AppToolTrainerIconRendering * m_pTrainerIcon;              //!< トレーナーアイコン
  app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA m_iconData;  //!< トレーナーアイコンデータ

  u32 m_page;   //!< 現在のページ数

  u16 m_mainSeq;        //!< メインシーケンス
  u16 m_subSeq;         //!< サブシーケンス
  bool m_isDrawEnable;  //!< 描画フラグ
};


GFL_NAMESPACE_END(TrainerPass)
GFL_NAMESPACE_END(App)

#endif  // TRAINER_PASS_DRAW_H_INCLUDED
