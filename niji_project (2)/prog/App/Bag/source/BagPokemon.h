//=============================================================================
/**
 * @file    BagPokemon.h
 * @brief   バッグ画面ポケモン関連
 * @author  Hiroyuki Nakamura
 * @date    2015.03.18
 */
//=============================================================================
#if !defined( BAG_POKEMON_H_INCLUDED )
#define BAG_POKEMON_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// niji
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"
#include "AppLib/include/Tool/AppToolGaugePartsLayout.h"
#include "App/PokeList/include/PokeListSetupParam.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class PokeIcon;
class AppCommonGrpIconData;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(bag)

// 前方宣言
class BagPokemonListener;

//=============================================================================
/**
 * @class BagPokemon
 * @brief バッグ画面ポケモン関連クラス
 * @date  2015.03.18
 */
//=============================================================================
class BagPokemon
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
public:
  //! 動作ID
  enum ActionID
  {
    ACTION_PLATE_ON = 0,    //!< プレートをタッチ中
    ACTION_PLATE_OFF,       //!< プレートをリリース
    ACTION_PLATE_SELECT,    //!< プレートを選択
    ACTION_GET_ITEM,        //!< ポケモンの道具を取得
    ACTION_PUT_ITEM,        //!< ポケモンの道具を配置
    ACTION_PUT_LIST_ITEM,   //!< リストの道具を配置
  };


private:
  //! ペインデータ
  struct PANE_DATA
  {
    gfl2::lyt::LytParts * plate;        //!< プレートの部品ペイン
    gfl2::lyt::LytBounding * bound;     //!< 境界ペイン

    gfl2::lyt::LytPicture * poke_icon;  //!< ポケモンアイコンのピクチャペイン
    gfl2::lyt::LytPicture * item_icon;  //!< アイテムアイコンのピクチャペイン
    gfl2::lyt::LytParts * sex_icon;     //!< 性別の部品ペイン
    gfl2::lyt::LytTextBox * name;       //!< 名前表示テキストペイン
    gfl2::lyt::LytTextBox * use;        //!< 使えるかを表示するテキストペイン

    gfl2::lyt::LytTextBox * lv;         //!< レベル表示テキストペイン

    gfl2::lyt::LytParts * st_base;      //!< 状態異常アイコンの部品ペイン
    gfl2::lyt::LytPicture * st_icon;    //!< 状態異常アイコンのピクチャペイン
  };

  //! アニメデータ
  struct ANIME_DATA
  {
    u16 sex_male;           //!< 性別表示を♂へ
    u16 sex_female;         //!< 性別表示を♀へ

    u16 normal_select_on;   //!< プレートを選択状態へ（通常）
    u16 normal_select_off;  //!< プレートを非選択状態へ（通常）

    u16 waza_select_on;     //!< プレートを選択状態へ（技マシンなど）
    u16 waza_select_off;    //!< プレートを非選択状態へ（技マシンなど）

    u16 egg_select_on;      //!< プレートを選択状態へ（タマゴ）
    u16 egg_select_off;     //!< プレートを非選択状態へ（タマゴ）
  };

  //! プレートの表示モード
  enum PlateMode
  {
    PLATE_NORMAL = 0,   //!< 通常
    PLATE_WAZA,         //!< 技マシンなど
    PLATE_EGG,          //!< タマゴ
  };

  //! プレート部品ペインインデックステーブル
  static const gfl2::lyt::LytPaneIndex PlatePartsTable[pml::PokeParty::MAX_MEMBERS];
  //! アニメデータテーブル
  static const ANIME_DATA AnimeDataTable[pml::PokeParty::MAX_MEMBERS];

  //! 操作モード
  enum CtrlMode
  {
    CTRL_NORMAL = 0,      //!< 通常
    CTRL_POKE_SELECT,     //!< ポケモン選択
    CTRL_ITEM_CATCH,      //!< 道具取得（ポケモンが持っていた道具）
    CTRL_LIST_ITEM_CATCH, //!< 道具取得（道具リストの道具）
  };

  static const u32 SINGLE_POS_INDEX = 2;  //!< シングル表示時の表示位置


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   pokemon       手持ちポケモン
   * @param   icon          ポケモンアイコン
   * @param   heap          ヒープ
   * @param   g2d           G2DUtil
   * @param   catch_enable  true = キャッチ処理有効, false = 無効
   * @param   single_pos    シングルモード時のポケモンの並び位置
   * @param   is_demo_mode  デモモードフラグ
   */
  //-----------------------------------------------------------------------------
  BagPokemon(
    App::PokeList::IntermediateData * pokemon,
    app::tool::PokeIcon * icon,
    app::tool::AppCommonGrpIconData * common_icon,
    app::util::Heap * heap,
    app::util::G2DUtil * g2d,
    bool catch_enable,
    u32 single_pos,
    const bool * is_demo_mode );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~BagPokemon();

  //-----------------------------------------------------------------------------
  /**
   * @func    SetInputEnabled
   * @brief   入力判定の切り替え
   * @date    2015.06.25
   *
   * @param   flg   true = 有効, false = 無効
   */
  //-----------------------------------------------------------------------------
  void SetInputEnabled( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    Init
   * @brief   初期化
   * @date    2015.04.10
   *
   * @retval  false = 初期化中
   * @retval  true  = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Init(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetListener
   * @brief   リスナーをセット
   * @date    2015.04.01
   *
   * @param   listener  セットするリスナー
   */
  //-----------------------------------------------------------------------------
  void SetListener( BagPokemonListener * listener );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsPutParty
   * @brief   手持ちポケモンの全てを表示するか（シングルモードか）
   * @date    2015.05.15
   *
   * @retval  true  = 手持ち全て表示（シングルモードじゃない）
   * @retval  false = １匹表示（シングルモード）
   */
  //-----------------------------------------------------------------------------
  bool IsPutParty(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitMode
   * @brief   表示状態の初期化
   * @date    2015.05.15
   *
   * @param   sel_pos   選択中の位置
   */
  //-----------------------------------------------------------------------------
  void InitMode( u32 sel_pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangePlateParty
   * @brief   シングルモード時の手持ち全表示切り替え
   * @date    2015.05.15
   *
   * @param   flg   true = 手持ち表示, false = １匹表示
   */
  //-----------------------------------------------------------------------------
  void ChangePlateParty( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    ReplaceTexture
   * @brief   ポケモンアイコンのテクスチャを変更
   * @date    2015.03.19
   *
   * @param   id  アイコンID
   * @param   pp  ポケモンパラメータ
   */
  //-----------------------------------------------------------------------------
  void ReplaceTexture( u32 id, const pml::pokepara::PokemonParam * pp );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitPaneCore
   * @brief   プレートの表示を初期化
   * @date    2015.04.17
   *
   * @param   pos   並び位置
   */
  //-----------------------------------------------------------------------------
  void InitPaneCore( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    ResetPane
   * @brief   全プレートの表示を初期化（ポケモンアイコンを読み込む）
   * @date    2015.04.17
   */
  //-----------------------------------------------------------------------------
  void ResetPane(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutItemIcon
   * @brief   アイテムアイコンの表示切り替え
   * @date    2015.03.23
   *
   * @param   pos   並び位置
   * @param   pp    ポケモンパラメータ
   */
  //-----------------------------------------------------------------------------
  void PutItemIcon( u32 pos, const pml::pokepara::PokemonParam * pp );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutItemIcon
   * @brief   アイテムアイコンの表示切り替え
   * @date    2015.05.01
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void PutItemIcon( u32 pos, bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetItemIconPos
   * @brief   アイテムアイコンの表示座標を取得
   * @date    2015.05.01
   *
   * @param   index   並び位置
   *
   * @return  表示座標
   */
  //-----------------------------------------------------------------------------
  gfl2::math::VEC3 GetItemIconPos( u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutLevel
   * @brief   レベル表示切り替え
   * @date    2015.03.23
   *
   * @param   pos   並び位置
   * @param   pp    ポケモンパラメータ
   */
  //-----------------------------------------------------------------------------
  void PutLevel( u32 pos, const pml::pokepara::PokemonParam * pp );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutHp
   * @brief   HP表示切り替え
   * @date    2015.03.23
   *
   * @param   pos   並び位置
   * @param   pp    ポケモンパラメータ
   */
  //-----------------------------------------------------------------------------
  void PutHp( u32 pos, const pml::pokepara::PokemonParam * pp );

  //-----------------------------------------------------------------------------
  /**
   * @func    StartHpRecoverAnime
   * @brief   HP回復アニメ開始
   * @date    2015.04.21
   *
   * @param   pos   並び位置
   * @param   pp    ポケモンパラメータ
   * @param   old   回復前のHP
   */
  //-----------------------------------------------------------------------------
  void StartHpRecoverAnime( u32 pos, const pml::pokepara::PokemonParam * pp, u32 old );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsPlayHpRecoverAnime
   * @brief   HP回復アニメを再生中か
   * @date    2015.04.21
   *
   * @param   pos   並び位置
   *
   * @retval  true  = 再生中
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsPlayHpRecoverAnime( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitPlate
   * @brief   プレートの表示を初期化（全体）
   * @date    2015.03.23
   *
   * @param   man           アイテムマネージャ
   * @param   item          選択中の道具
   * @param   select_off    true = 非選択状態にする
   */
  //-----------------------------------------------------------------------------
  void InitPlate( itemman::ITEM_MANAGER * man, u32 item, bool select_off=true );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitPlate
   * @brief   プレートの表示を初期化（個別）
   * @date    2015.03.23
   *
   * @param   pos           並び位置
   * @param   pp            ポケモンパラメータ
   * @param   man           アイテムマネージャ
   * @param   item          選択中の道具
   * @param   select_off    true = 非選択状態にする
   */
  //-----------------------------------------------------------------------------
  void InitPlate( u32 pos, const pml::pokepara::PokemonParam * pp, bool is_moudoku, itemman::ITEM_MANAGER * man, u32 item, bool select_off=true );

  //-----------------------------------------------------------------------------
  /**
   * @func    VisiblePlate
   * @brief   プレートの表示切り替え
   * @date    2015.05.15
   *
   * @param   pos   並び位置
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void VisiblePlate( u32 pos, bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutStatusIcon
   * @brief   状態異常アイコンを表示
   * @date    2015.07.02
   *
   * @param   pos         並び位置
   * @param   pp          ポケモンパラメータ
   * @param   is_moudoku  true = 猛毒
   */
  //-----------------------------------------------------------------------------
  void PutStatusIcon( u32 pos, const pml::pokepara::PokemonParam * pp, bool is_moudoku );

  //-----------------------------------------------------------------------------
  /**
   * @func    VanishStatusIcon
   * @brief   状態異常アイコンを非表示にする
   * @date    2015.04.21
   *
   * @param   pos   並び位置
   */
  //-----------------------------------------------------------------------------
  void VanishStatusIcon( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangePokeSelectMode
   * @brief   ポケモン選択モードを切り替え
   * @date    2015.04.13
   *
   * @param   flg               true = 選択中へ, false = 非選択へ
   * @param   pos               並び位置
   * @param   is_off_anm_lock   true = 非選択アニメをしない
   */
  //-----------------------------------------------------------------------------
  void ChangePokeSelectMode( bool flg, u32 pos, bool is_off_anm_lock=false );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetSelectPos
   * @brief   プレートの選択位置を取得
   * @date    2015.04.15
   *
   * @return  選択位置
   */
  //-----------------------------------------------------------------------------
  u32 GetSelectPos(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetSelectPos
   * @brief   プレートの選択位置を設定（シングルモード時のみ）
   * @date    2015.05.15
   */
  //-----------------------------------------------------------------------------
  void SetSelectPos(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetItem
   * @brief   ポケモンの所持道具を取得
   * @date    2015.05.01
   *
   * @param   pos   並び位置
   *
   * @return  所持道具
   */
  //-----------------------------------------------------------------------------
  u32 GetItem( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeListItemCatchMode
   * @brief   操作を道具リストキャッチモードへ変更
   * @date    2015.05.12
   */
  //-----------------------------------------------------------------------------
  void ChangeListItemCatchMode(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetPiece
   * @brief   ビーズに対応したピースを取得
   * @date    2015.05.15
   *
   * @param   man   アイテムマネージャ
   * @param   item  アイテム番号
   *
   * @return  アイテム番号
   */
  //-----------------------------------------------------------------------------
  u32 GetPiece( itemman::ITEM_MANAGER * man, u32 item );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetPokeIconPos
   * @brief   ポケモンアイコンの表示座標を取得
   * @date    2016.01.19
   *
   * @param   index   並び位置
   *
   * @return  表示座標
   */
  //-----------------------------------------------------------------------------
  gfl2::math::VEC3 GetPokeIconPos( u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetSingleModePos
   * @brief   シングルモード時の対象位置をセット
   * @date    2016.02.05
   *
   * @param   pos   並び位置
   */
  //-----------------------------------------------------------------------------
  void SetSingleModePos( u32 pos );


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    Update
   * @brief   更新
   * @date    2015.03.19
   */
  //-----------------------------------------------------------------------------
  virtual void Update(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Draw
   * @brief   描画
   * @date    2015.03.19
   */
  //-----------------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetMemberCount
   * @brief   ポケモンの数を取得
   * @date    2015.07.16
   *
   * @return  ポケモンの数
   */
  //-----------------------------------------------------------------------------
  u32 GetMemberCount(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetSingleSelectPos
   * @brief   シングルモードの表示プレートのY座標にデフォルト値をセット
   * @date    2015.05.15
   */
  //-----------------------------------------------------------------------------
  void SetSingleSelectPos(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitPokeIcon
   * @brief   ポケモンアイコン初期化
   * @date    2015.04.10
   *
   * @retval  false = 初期化中
   * @retval  true  = それ以外
   */
  //-----------------------------------------------------------------------------
  bool InitPokeIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateButton
   * @brief   ボタン生成
   * @date    2015.04.13
   */
  //-----------------------------------------------------------------------------
  void CreateButton(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateHpGauge
   * @brief   HPゲージクラス生成
   * @date    2015.04.01
   */
  //-----------------------------------------------------------------------------
  void CreateHpGauge(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    MakeHpGaugeData
   * @brief   HPゲージクラスのデータ生成
   * @date    2015.04.02
   *
   * @param   param   生成場所
   * @param   pane    親ペイン
   * @param   pos     並び位置
   * @param   anm     アニメテーブル
   */
  //-----------------------------------------------------------------------------
  void MakeHpGaugeData( app::tool::AppToolHPGaugePartsLayout::INIT_PARAM * param, gfl2::lyt::LytPaneIndex pane, u32 pos, const u32 * anm );

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteHpGauge
   * @brief   HPゲージクラス削除
   * @date    2015.04.01
   */
  //-----------------------------------------------------------------------------
  void DeleteHpGauge(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateHpGauge
   * @brief   HPゲージ更新
   * @date    2015.04.01
   */
  //-----------------------------------------------------------------------------
  void UpdateHpGauge(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetPane
   * @brief   ペインデータを取得
   * @date    2015.03.19
   */
  //-----------------------------------------------------------------------------
  void GetPane(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitPane
   * @brief   全プレートの表示を初期化
   * @date    2015.03.20
   */
  //-----------------------------------------------------------------------------
  void InitPane(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutSex
   * @brief   性別表示切り替え
   * @date    2015.03.23
   *
   * @param   pos   並び位置
   * @param   pp    ポケモンパラメータ
   */
  //-----------------------------------------------------------------------------
  void PutSex( u32 pos, const pml::pokepara::PokemonParam * pp );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutPlate
   * @brief   プレート配置（通常）
   * @date    2015.03.23
   *
   * @param   pos   並び位置
   * @param   pp    ポケモンパラメータ
   */
  //-----------------------------------------------------------------------------
  void PutPlate( u32 pos, const pml::pokepara::PokemonParam * pp, bool is_moudoku );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutEggPlate
   * @brief   プレート配置（タマゴ用）
   * @date    2015.03.23
   *
   * @param   pos   並び位置
   */
  //-----------------------------------------------------------------------------
  void PutEggPlate( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutWazaMachinePlate
   * @brief   プレート配置（技マシン用）
   * @date    2015.03.23
   *
   * @param   pos   並び位置
   * @param   pp    ポケモンパラメータ
   * @param   item  選択中の道具
   */
  //-----------------------------------------------------------------------------
  void PutWazaMachinePlate( u32 pos, const pml::pokepara::PokemonParam * pp, u32 item );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutEvolutionPlate
   * @brief   プレート配置（進化アイテム用）
   * @date    2015.03.23
   *
   * @param   pos   並び位置
   * @param   pp    ポケモンパラメータ
   * @param   item  選択中の道具
   */
  //-----------------------------------------------------------------------------
  void PutEvolutionPlate( u32 pos, const pml::pokepara::PokemonParam * pp, u32 item );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutBeadsPlate
   * @brief   プレート配置（ビーズ用）
   * @date    2015.11.05
   *
   * @param   pos   並び位置
   * @param   pp    ポケモンパラメータ
   * @param   man   アイテムマネージャ
   * @param   item  選択中の道具
   */
  //-----------------------------------------------------------------------------
  void PutBeadsPlate( u32 pos, const pml::pokepara::PokemonParam * pp, itemman::ITEM_MANAGER * man, u32 item );

  //-----------------------------------------------------------------------------
  /**
   * @func    PlateAnimeOff
   * @brief   プレートアニメを全て停止
   * @date    2015.04.02
   *
   * @param   pos   並び位置
   */
  //-----------------------------------------------------------------------------
  void PlateAnimeOff( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    PlateSelectOn
   * @brief   プレートを選択中にする
   * @date    2015.04.02
   *
   * @param   pos   並び位置
   */
  //-----------------------------------------------------------------------------
  void PlateSelectOn( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    PlateSelectOff
   * @brief   プレートを非選択にする
   * @date    2015.04.02
   *
   * @param   pos   並び位置
   */
  //-----------------------------------------------------------------------------
  void PlateSelectOff( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeSelectPlate
   * @brief   プレートの選択状態を切り替え
   * @date    2015.04.01
   *
   * @param   pos   並び位置
   * @param   flg   true = 選択中へ, false = 非選択へ
   */
  //-----------------------------------------------------------------------------
  void ChangeSelectPlate( u32 pos, bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeSelectPlate
   * @brief   選択プレートを切り替え
   * @date    2015.04.13
   *
   * @param   new_pos   新しい位置
   */
  //-----------------------------------------------------------------------------
  void ChangeSelectPlate( u32 new_pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeSelectPlateSingle
   * @brief   選択プレートを切り替え（シングルモード時）
   * @date    2015.05.15
   *
   * @param   new_pos   新しい位置
   *
   * @li  表示はそのままで、ボタンの切り替えのみ
   */
  //-----------------------------------------------------------------------------
  void ChangeSelectPlateSingle( u32 new_pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    MoveSelectPlate
   * @brief   十字キーでの選択プレート変更
   * @date    2015.04.13
   *
   * @param   mv      移動方向
   * @param   is_trg  true = トリガー入力
   *
   * @retval  true  = 変更した
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool MoveSelectPlate( s32 mv, bool is_trg );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsCatchItem
   * @brief   ポケモンの所持道具をキャッチできるか
   * @date    2015.05.01
   *
   * @retval  true  = 可
   * @retval  false = 不可
   */
  //-----------------------------------------------------------------------------
  bool IsCatchItem(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetTouchPokemon
   * @brief   タッチ中のポケモンの位置を取得
   * @date    2015.05.01
   *
   * @return  並び位置
   */
  //-----------------------------------------------------------------------------
  u32 GetTouchPokemon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetTouchPokemonSingle
   * @brief   タッチ中のポケモンの位置を取得（シングルモード時
   * @date    2015.05.15
   *
   * @return  並び位置
   */
  //-----------------------------------------------------------------------------
  u32 GetTouchPokemonSingle(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    OnTouchEvent
   * @brief   タッチパネルイベントの検知
   * @date    2015.03.31
   *
   * @param   touch_panel タッチパネル
   * @param   is_touch    true = タッチされている
   *
   * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
   * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
   * @retval  ENABLE_ACCESS         = それ以外
   *
   * @note  override UIInputListener
   */
  //-----------------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel * touch_panel, bool is_touch );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnLayoutPaneEvent
   * @brief   ペインイベントの検知
   * @date    2015.04.13
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
   * @date    2015.04.10
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

  //-----------------------------------------------------------------------------
  /**
   * @func    OnTouchEvent_Normal
   * @brief   タッチパネルイベント（通常）
   * @date    2015.05.01
   *
   * @param   tp        タッチパネル
   * @param   is_touch  true = タッチされている
   *
   * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
   * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
   * @retval  ENABLE_ACCESS         = それ以外
   */
  //-----------------------------------------------------------------------------
  app::ui::UIInputListener::ListenerResult OnTouchEvent_Normal( gfl2::ui::TouchPanel * tp, bool is_touch );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnTouchEvent_PokeSelect
   * @brief   タッチパネルイベント（ポケモン選択時）
   * @date    2015.05.01
   *
   * @param   tp        タッチパネル
   * @param   is_touch  true = タッチされている
   *
   * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
   * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
   * @retval  ENABLE_ACCESS         = それ以外
   */
  //-----------------------------------------------------------------------------
  app::ui::UIInputListener::ListenerResult OnTouchEvent_PokeSelect( gfl2::ui::TouchPanel * tp, bool is_touch );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnTouchEvent_ItemCatch
   * @brief   タッチパネルイベント（ポケモンの所持道具のキャッチムーブ時）
   * @date    2015.05.01
   *
   * @param   tp        タッチパネル
   * @param   is_touch  true = タッチされている
   *
   * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
   * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
   * @retval  ENABLE_ACCESS         = それ以外
   */
  //-----------------------------------------------------------------------------
  app::ui::UIInputListener::ListenerResult OnTouchEvent_ItemCatch( gfl2::ui::TouchPanel * tp, bool is_touch );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnTouchEvent_ListItemCatch
   * @brief   タッチパネルイベント（道具リストのキャッチムーブ時）
   * @date    2015.05.12
   *
   * @param   tp        タッチパネル
   * @param   is_touch  true = タッチされている
   *
   * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
   * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
   * @retval  ENABLE_ACCESS         = それ以外
   */
  //-----------------------------------------------------------------------------
  app::ui::UIInputListener::ListenerResult OnTouchEvent_ListItemCatch( gfl2::ui::TouchPanel * tp, bool is_touch );


private:
  app::util::Heap * m_heap;         //!< ヒープ
  app::util::G2DUtil * m_g2dUtil;   //!< G2DUtil

  App::PokeList::IntermediateData * m_pokemon;  //!< ポケモンデータ

  app::tool::AppCommonGrpIconData * m_commonIcon; //!< 状態異常アイコン

  app::tool::PokeIcon * m_pPokeIcon;              //!< ポケモンアイコン
  u32 m_initLoadCount;
  PANE_DATA m_pane[pml::PokeParty::MAX_MEMBERS];  //!< ペインデータ

  app::tool::AppToolHPGaugePartsLayout * m_pHpGauge[pml::PokeParty::MAX_MEMBERS*2];   //!< HPゲージ

  BagPokemonListener * m_listener;    //!< リスナー

  PlateMode m_plateMode[pml::PokeParty::MAX_MEMBERS];   //!< プレート表示モード

  s16 m_selectPos;      //!< 選択位置

  u8 m_subSeq;          //!< サブシーケンス
  
  u8 m_catchCount;      //!< 道具キャッチカウンタ

  CtrlMode m_ctrlMode;  //!< コントロールモード
  bool m_catchEnable;   //!< キャッチ操作を許可するか

  u32 m_singleModePos;  //!< シングルモード時に表示するポケモンの位置

  gfl2::math::VEC3 m_platePos[pml::PokeParty::MAX_MEMBERS];   //!< プレート表示座標

  bool m_isTouchStart;  //!< タッチを開始したか

  const bool * m_isDemoMode;  //!< デモモードフラグ
};


GFL_NAMESPACE_END(app)
GFL_NAMESPACE_END(bag)

#endif  // BAG_POKEMON_H_INCLUDED
