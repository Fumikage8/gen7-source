#if !defined( __APP_STATUS_INFORMATION_H_INCLUDED__ )
#define __APP_STATUS_INFORMATION_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    StatusInformation.h
 * @date    2015/12/03 11:51:49
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：情報(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <Layout/include/gfl2_Layout.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)
class DrawerBase;
GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )
class AppCommonGrpIconData;
GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN(pml)
GFL_NAMESPACE_BEGIN(pokepara)
class PokemonParam;
GFL_NAMESPACE_END(pokepara)
GFL_NAMESPACE_END(pml)


//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//! @brief  ボタンID
enum {
  BTN_ID_CANCEL,
  BTN_ID_TAB_WAZA,
  BTN_ID_TAB_MEMO,
  BTN_ID_BOX_MARK,
  BTN_ID_INFO_TOKUSEI,
  BTN_ID_INFO_ITEM,
  BTN_ID_WAZA_00,
  BTN_ID_WAZA_01,
  BTN_ID_WAZA_02,
  BTN_ID_WAZA_03,
  BTN_ID_NUM,
};

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   情報(下)リスナー
//------------------------------------------------------------------------------
class InformationListener
{
public:
  //------------------------------------------------------------------
  /**
   * @brief   道具インフォメーションボタンの入力切替
   *
   * @param   enable    入力フラグ(true = 有効、false = 無効)
   */
  //------------------------------------------------------------------
  virtual void SetInputEnableItemInfoBtn( bool enable ) { ; }
};

//------------------------------------------------------------------------------
// @brief   情報(下)リボン用リスナー
//------------------------------------------------------------------------------
class InformationRibbonListener
{
public:
  //------------------------------------------------------------------
  /**
   * @brief   リボンリストを開いているかどうか
   *
   * @return  "true  = 開いている"
   * @return  "false = 閉じている"
   */
  //------------------------------------------------------------------
  virtual bool IsRibbonListOpen( void ) const { return false; }
};

//------------------------------------------------------------------------------
// @brief   情報(下)
//------------------------------------------------------------------------------
class Information
{
  GFL_FORBID_COPY_AND_ASSIGN( Information );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  Information( DrawerBase* base, app::tool::AppCommonGrpIconData* cmnIcon );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~Information( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   *
   * @param   listener          リスナー
   * @param   ribbonListener    リボン用リスナー
   */
  //------------------------------------------------------------------
  void Initialize( InformationListener* listener, InformationRibbonListener* ribbonListener );

  //------------------------------------------------------------------
  /**
   * @brief   セットアップ
   *
   * @param   pp    ポケモンパラメータ
   * @param   lock  ロックフラグ
   */
  //------------------------------------------------------------------
  void Setup( const pml::pokepara::PokemonParam* pp, bool lock );

private:
  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト
   */
  //------------------------------------------------------------------
  void InitializeText( void );

  //------------------------------------------------------------------
  /**
   * @brief   ニックネームのセット
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetNickName( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   性別アイコンのセット
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetGenderIcon( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   レベルのセット
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetLevel( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   状態異常アイコンのセット
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetStateIcon( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   レアアイコンのセット
   *
   * @param   pp      ポケモンパラメータ
   * @param   isEgg   タマゴフラグ
   */
  //------------------------------------------------------------------
  void SetRareIcon( const pml::pokepara::PokemonParam* pp, bool isEgg );

  //------------------------------------------------------------------
  /**
   * @brief   ポケルスアイコンのセット
   *
   * @param   pp      ポケモンパラメータ
   * @param   isEgg   タマゴフラグ
   */
  //------------------------------------------------------------------
  void SetPokerusuIcon( const pml::pokepara::PokemonParam* pp, bool isEgg );

  //------------------------------------------------------------------
  /**
   * @brief   産地アイコンのセット
   *
   * @param   pp      ポケモンパラメータ
   * @param   isEgg   タマゴフラグ
   */
  //------------------------------------------------------------------
  void SetSanchiIcon( const pml::pokepara::PokemonParam* pp, bool isEgg );

  //------------------------------------------------------------------
  /**
   * @brief   特性のセット
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetTokusei( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   アイテムのセット
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetItem( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   ロックアイコンのセット
   *
   * @param   lock    ロックフラグ
   */
  //------------------------------------------------------------------
  void SetLockIcon( bool lock );

public:
  //------------------------------------------------------------------
  /**
   * @brief   ボックスマークのセット
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetBoxMark( const pml::pokepara::PokemonParam* pp );

public:
  //------------------------------------------------------------------
  /**
   * @brief   タマゴ専用モードの表示設定
   *
   * @param   flag    タマゴ専用モードフラグ(true = タマゴ専用、false = 通常モード)
   */
  //------------------------------------------------------------------
  void SetEggMode( bool flag );


private:
  DrawerBase*                           m_pBase;

  InformationListener*                  m_pListener;

  InformationRibbonListener*            m_pRibbonListener;

  const pml::pokepara::PokemonParam*    m_pPokeParam;

  app::tool::AppCommonGrpIconData*      m_pCommonIcon;

private:
  bool      m_HaveItem;

private:
  gfl2::lyt::LytTextBox*        m_pTextNickName;

  gfl2::lyt::LytTextBox*        m_pTextLevel;

  gfl2::lyt::LytPane*           m_pGenderIcon;

  gfl2::lyt::LytPicture*        m_pSickIcon;

  gfl2::lyt::LytPane*           m_pRareIcon;

  gfl2::lyt::LytPane*           m_pPokerusuIcon;

  gfl2::lyt::LytPane*           m_pSantiIcon;

  gfl2::lyt::LytTextBox*        m_pTextTokusei;

  gfl2::lyt::LytTextBox*        m_pTextItem;

  gfl2::lyt::LytPane*           m_pEggVisible;

  gfl2::lyt::LytPane*           m_pLockIcon;

  gfl2::lyt::LytPane*           m_pPaneItemPlate[3];
};

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

#endif // __APP_STATUS_INFORMATION_H_INCLUDED__
