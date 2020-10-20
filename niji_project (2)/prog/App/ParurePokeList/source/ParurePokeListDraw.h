//=============================================================================
/**
 * @file    ParurePokeListDraw.h
 * @brief   ポケパルレポケモン選択画面描画処理
 * @author  Hiroyuki Nakamura
 * @date    2015.11.12
 */
//=============================================================================
#if !defined( PARURE_POKELIST_DRAW_H_INCLUDED )
#define PARURE_POKELIST_DRAW_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// niji
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"
#include "AppLib/include/Util/EffectUtil.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class PokeIcon;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(poke_3d)
GFL_NAMESPACE_BEGIN(model)
class BaseCamera;
GFL_NAMESPACE_END(model)
GFL_NAMESPACE_END(poke_3d)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(PokeParure)

// 前方宣言
struct APP_PARAM;

//=============================================================================
/**
 * @class ParurePokeListDraw
 * @brief ポケパルレポケモン選択画面描画クラス
 * @date  2015.11.12
 */
//=============================================================================
class ParurePokeListDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
public:
  //=============================================================================
  /**
   * @class Listener
   * @brief 描画リスナークラス
   * @date  2015.11.12
   */
  //=============================================================================
  class Listener
  {
  public:
    //-----------------------------------------------------------------------------
    /**
     * @func    OnAction
     * @brief   入力時のリスナー
     * @date    2015.10.02
     *
     * @param   pos   選択位置
     */
    //-----------------------------------------------------------------------------
    virtual void OnAction( u32 pos ) = 0;
  };


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
    BTNID_POKEMON_00 = 0,   //!< ポケモン１匹目
    BTNID_POKEMON_01,       //!< ポケモン２匹目
    BTNID_POKEMON_02,       //!< ポケモン３匹目
    BTNID_POKEMON_03,       //!< ポケモン４匹目
    BTNID_POKEMON_04,       //!< ポケモン５匹目
    BTNID_POKEMON_05,       //!< ポケモン６匹目
    BTNID_CHANGE,           //!< いれかえ
    BTNID_RETURN,           //!< 戻る
    BTNID_MAX               //!< 最大数
  };

  //! ポケモンアイコンペインテーブル
  static const gfl2::lyt::LytPaneIndex PokeIconPaneTable[pml::PokeParty::MAX_MEMBERS];


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   app_param   外部設定パラメータ
   * @param   heap        ヒープ
   * @param   render_man  描画マネージャ
   */
  //-----------------------------------------------------------------------------
  ParurePokeListDraw( const APP_PARAM * app_param, app::util::Heap * heap, app::util::AppRenderingManager * render_man );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~ParurePokeListDraw();

  //-----------------------------------------------------------------------------
  /**
   * @func    SetListener
   * @brief   リスナーをセット
   * @date    2015.11.12
   *
   * @param   listener リスナー
   */
  //-----------------------------------------------------------------------------
  void SetListener( Listener * listener );

  //-----------------------------------------------------------------------------
  /**
   * @func    Init
   * @brief   初期化処理
   * @date    2015.11.12
   *
   * @param   res_buf   レイアウトリソースバッファ
   * @param   msg_buf   メッセージリソースバッファ
   * @param   eff_buf   パーティクルリソースバッファ
   */
  //-----------------------------------------------------------------------------
  void Init( void * res_buf, void * msg_buf, void * eff_buf );

  //-----------------------------------------------------------------------------
  /**
   * @func    End
   * @brief   終了処理
   * @date    2015.11.12
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
   * @date    2015.11.12
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Start(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsStart
   * @brief   開始処理が終了したかをチェック
   * @date    2015.11.12
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsStart(void);


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    GetGameData
   * @brief   GameData関連取得
   * @date    2015.11.12
   */
  //-----------------------------------------------------------------------------
  void GetGameData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update
   * @brief   更新
   * @date    2015.11.12
   */
  //-----------------------------------------------------------------------------
  virtual void Update(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Draw
   * @brief   描画
   * @date    2015.11.12
   *
   * @param   displayNo   描画面
   */
  //-----------------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  //-----------------------------------------------------------------------------
  /**
   * @func    Initialize2D
   * @brief   2D関連初期化
   * @date    2015.11.12
   *
   * @param   res_buf       レイアウトリソースバッファ
   * @param   msg_buf       メッセージリソースバッファ
   */
  //-----------------------------------------------------------------------------
  void Initialize2D( void * res_buf, void * msg_buf );

  //-----------------------------------------------------------------------------
  /**
   * @func    Terminate2D
   * @brief   2D関連削除
   * @date    2015.11.12
   */
  //-----------------------------------------------------------------------------
  void Terminate2D(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateButton
   * @brief   ボタン生成
   * @date    2015.11.18
   */
  //-----------------------------------------------------------------------------
  void CreateButton(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    WriteDefaultSrring
   * @brief   デフォルト文字列描画
   * @date    2015.11.12
   */
  //-----------------------------------------------------------------------------
  void WriteDefaultString(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutSelectIcon
   * @brief   選択中アイコン表示
   * @date    2015.11.18
   */
  //-----------------------------------------------------------------------------
  void PutSelectIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangePokemon
   * @brief   ポケモン切り替え
   * @date    2015.11.18
   *
   * @param   pos   選択位置
   */
  //-----------------------------------------------------------------------------
  void ChangePokemon( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetPutParamMarkMax
   * @brief   マーク表示数取得
   * @date    2015.11.18
   *
   * @param   prm   表示パラメータ
   *
   * @return  マークの数
   */
  //-----------------------------------------------------------------------------
  u32 GetPutParamMarkMax( u32 prm );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutPokeHeart
   * @brief   なかよし度表示
   * @date    2015.11.18
   *
   * @param   pp      ポケモンパラメータ
   * @param   is_egg  true = タマゴ
   */
  //-----------------------------------------------------------------------------
  void PutPokeHeart( const pml::pokepara::PokemonParam * pp, bool is_egg );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutPokeHungry
   * @brief   満腹度表示
   * @date    2015.11.18
   *
   * @param   pp      ポケモンパラメータ
   * @param   is_egg  true = タマゴ
   */
  //-----------------------------------------------------------------------------
  void PutPokeHungry( const pml::pokepara::PokemonParam * pp, bool is_egg );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutPokeNadenade
   * @brief   なでなで表示
   * @date    2015.11.18
   *
   * @param   pp      ポケモンパラメータ
   * @param   is_egg  true = タマゴ
   */
  //-----------------------------------------------------------------------------
  void PutPokeNadenade( const pml::pokepara::PokemonParam * pp, bool is_egg );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreatePokeIcon
   * @brief   ポケモンアイコン生成
   * @date    2015.11.24
   */
  //-----------------------------------------------------------------------------
  void CreatePokeIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeletePokeIcon
   * @brief   ポケモンアイコン削除
   * @date    2015.11.24
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeletePokeIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadPokeIcon
   * @brief   ポケモンアイコン読み込み
   * @date    2015.11.24
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool LoadPokeIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitPokeIcon
   * @brief   ポケモンアイコン初期化
   * @date    2015.11.24
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool InitPokeIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetPokeIconPicturePane
   * @brief   ポケモンアイコンのピクチャペインを取得
   * @date    2015.11.24
   *
   * @param   index   ポケモンの並び位置
   *
   * @return  ピクチャペイン
   */
  //-----------------------------------------------------------------------------
  gfl2::lyt::LytPicture * GetPokeIconPicturePane( u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateEffect
   * @brief   パーティクル関連生成
   * @date    2016.01.15
   *
   * @param   buf   リソースバッファ
   */
  //-----------------------------------------------------------------------------
  void CreateEffect( void * buf );

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteEffect
   * @brief   パーティクル関連削除
   * @date    2016.01.15
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteEffect(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    OnLayoutPaneEvent
   * @brief   ペインイベントの検知
   * @date    2015.11.18
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
   * @date    2015.11.12
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
  const APP_PARAM * m_appParam;                         //!< 外部設定パラメータ
  app::util::Heap * m_heap;                             //!< ヒープ
  app::util::AppRenderingManager * m_renderingManager;  //!< 描画マネージャ

  Listener * m_listener;  //!< リスナー

  const Savedata::MyStatus * m_myStatus;  //!< プレイヤーデータ
  pml::PokeParty * m_pokeParty;           //!< 手持ちポケモン

  app::util::EffectUtil * m_pEffectUtil;    //!< EffectUtil
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode * m_effectDrawEnvNodeOfCamera; //!< カメラを含む環境
  poke_3d::model::BaseCamera * m_effectCamera;  //!< カメラ

  gfl2::str::MsgData * m_pMsgData;  //!< メッセージデータ
  print::WordSet * m_pWordSet;      //!< 単語セット

  app::tool::PokeIcon * m_pPokeIcon;  //!< ポケモンアイコン
  u8 m_iconLoadCount;                 //!< ポケモンアイコンロードカウンタ
  u8 m_iconLoadSeq;                   //!< ポケモンアイコン読み込みシーケンス

  u8 m_pos;   //!< 選択しているポケモンの位置

  u8 m_subSeq;          //!< サブシーケンス
  bool m_isDrawEnable;  //!< 描画フラグ
};


GFL_NAMESPACE_END(PokeParure)
GFL_NAMESPACE_END(App)

#endif  // PARURE_POKELIST_DRAW_H_INCLUDED
