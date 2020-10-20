//=============================================================================
/**
 * @file    BagInfoField.h
 * @brief   バッグ画面フィールド用アイテム情報
 * @author  Hiroyuki Nakamura
 * @date    2015.03.26
 */
//=============================================================================
#if !defined( BAG_INFO_FIELD_H_INCLUDED )
#define BAG_INFO_FIELD_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// niji
#include "Savedata/include/MyItemSave.h"
#include "Print/include/ItemMessage.h"
#include "AppLib/include/Ui/UIView.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class AppCommonGrpIconData;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(bag)

//=============================================================================
/**
 * @class BagPokemon
 * @brief バッグ画面ポケモン関連クラス
 * @date  2015.03.18
 */
//=============================================================================
class BagInfoField
  : public app::ui::UIView
{
public:
  static const u32 NUM_PUT_NONE = 0xffffffff; //!< 情報に個数を表示しない


private:
  static const u32 START_PUT_ITEM = 0xffffffff;   //!< m_putItemの初期値


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap        ヒープ
   * @param   item_man    アイテムマネージャ
   * @param   render_man  描画マネージャ
   */
  //-----------------------------------------------------------------------------
  BagInfoField(
    app::util::Heap * heap,
    itemman::ITEM_MANAGER * item_man,
    app::util::AppRenderingManager * render_man,
    app::tool::AppCommonGrpIconData * common_icon );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~BagInfoField();

  //-----------------------------------------------------------------------------
  /**
   * @func    Setup
   * @brief   セットアップ
   * @date    2015.04.10
   *
   * @param   lyt_buf   レイアウトリソースバッファ
   * @param   msg_info  道具説明のメッセージリソースバッファ
   * @param   msg_data  バッグ画面のメッセージデータ
   * @param   wset      単語セット
   */
  //-----------------------------------------------------------------------------
  void Setup( void * lyt_buf, void * msg_info, gfl2::str::MsgData * msg_data, print::WordSet * wset, WordSetLoader * wset_loader );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsEnd
   * @brief   終了できるかチェック
   * @date    2015.08.21
   *
   * @retval  true  = 可
   * @retval  false = 不可
   */
  //-----------------------------------------------------------------------------
  bool IsEnd(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    StartInAnime
   * @brief   開始アニメ開始
   * @date    2015.06.02
   */
  //-----------------------------------------------------------------------------
  void StartInAnime(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsInAnime
   * @brief   開始アニメ終了チェック
   * @date    2015.06.02
   *
   * @retval  false = 処理中
   * @retval  true  = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsInAnime(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    StartOutAnime
   * @brief   終了アニメ開始
   * @date    2015.06.02
   */
  //-----------------------------------------------------------------------------
  void StartOutAnime(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsOutAnime
   * @brief   終了アニメ終了チェック
   * @date    2015.06.02
   *
   * @retval  false = 処理中
   * @retval  true  = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsOutAnime(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetItemIcon
   * @brief   アイテムアイコンのピクチャペインを取得
   * @date    2015.03.27
   *
   * @return  ピクチャペイン
   */
  //-----------------------------------------------------------------------------
  gfl2::lyt::LytPicture * GetItemIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    VisibleItemIcon
   * @brief   アイテムアイコンの表示切り替え
   * @date    2015.03.27
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void VisibleItemIcon( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateInfo
   * @brief   アイテム情報表示更新
   * @date    2015.03.27
   *
   * @param   id    アイテムID
   * @param   num   所持数
   */
  //-----------------------------------------------------------------------------
  void UpdateInfo( u32 id, u32 num=NUM_PUT_NONE );

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateInfo
   * @brief   アイテム情報表示更新
   * @date    2015.05.07
   *
   * @param   item  アイテムデータ
   */
  //-----------------------------------------------------------------------------
  void UpdateInfo( const Savedata::MyItem::ITEM_ST * item );

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateItemNum
   * @brief   アイテム所持数更新
   * @date    2015.04.15
   *
   * @param   num   所持数
   */
  //-----------------------------------------------------------------------------
  void UpdateItemNum( u32 num );

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdatePocketName
   * @brief   ポケット名表示更新
   * @date    2015.03.27
   *
   * @param   pocket  ポケット番号
   */
  //-----------------------------------------------------------------------------
  void UpdatePocketName( u32 pocket );

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdatePokeItemCategory
   * @brief   カテゴリ表示をポケモンの所持道具に更新
   * @date    2015.05.08
   */
  //-----------------------------------------------------------------------------
  void UpdatePokeItemCategory(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateMyGold
   * @brief   所持金表示更新
   * @date    2015.03.27
   *
   * @param   gold  所持金
   */
  //-----------------------------------------------------------------------------
  void UpdateMyGold( u32 gold );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisibleXButton
   * @brief   Xボタンの表示を切り替え
   * @date    2015.05.15
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisibleXButton( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    VanishYButton
   * @brief   Yボタンの表示を切り替え
   * @date    2015.07.16
   */
  //-----------------------------------------------------------------------------
  void VanishYButton(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangePocketBg
   * @brief   背景アニメ切り替え
   * @date    2015.08.28
   *
   * @param   pocket  ポケット番号
   */
  //-----------------------------------------------------------------------------
  void ChangePocketBg( u32 pocket );


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    Update
   * @brief   更新
   * @date    2015.03.26
   */
  //-----------------------------------------------------------------------------
  virtual void Update(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Draw
   * @brief   描画
   * @date    2015.03.26
   */
  //-----------------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateLayout
   * @brief   レイアウト生成
   * @date    2015.03.26
   */
  //-----------------------------------------------------------------------------
  void CreateLayout( void * buf, gfl2::str::MsgData * msg_data, print::WordSet * wset, WordSetLoader * wset_loader );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateItemInfo
   * @brief   道具説明メッセージクラス生成
   * @date    2015.04.24
   *
   * @param   buf   メッセージリソースバッファ
   */
  //-----------------------------------------------------------------------------
  void CreateItemInfo( void * buf );

  //-----------------------------------------------------------------------------
  /**
   * @func    DleteItemInfo
   * @brief   道具説明メッセージクラス破棄
   * @date    2015.04.24
   */
  //-----------------------------------------------------------------------------
  void DleteItemInfo(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetDefaultString
   * @brief   デフォルト文字列書き込み
   * @date    2015.03.27
   */
  //-----------------------------------------------------------------------------
  void SetDefaultString(void);


private:
  app::util::Heap * m_heap;                             //!< ヒープ
  itemman::ITEM_MANAGER * m_itemManager;                //!< アイテムマネージャ
  app::util::AppRenderingManager * m_renderingManager;  //!< 描画マネージャ
  app::tool::AppCommonGrpIconData * m_commonIcon;       //!< タイプアイコン

  print::ItemMessage * m_pItemMessage;    //!< 道具説明メッセージクラス

  u32 m_putItem;  //!< 表示中のアイテム番号

  bool m_isDrawEnable;  //!< 描画許可フラグ
};


GFL_NAMESPACE_END(app)
GFL_NAMESPACE_END(bag)

#endif  // BAG_INFO_FIELD_H_INCLUDED
