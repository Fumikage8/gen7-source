#if !defined( __APP_STATUS_RIBBON_EXPLAIN_H_INCLUDED__ )
#define  __APP_STATUS_RIBBON_EXPLAIN_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    StatusRibbonExplain.h
 * @date    2015/12/02 17:01:51
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：リボン説明(上)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gflib2
#include <str/include/gfl2_Str.h>
#include <Layout/include/gfl2_Layout.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)
class DrawerBase;
class StatusDrawListener;
GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(pml)
GFL_NAMESPACE_BEGIN(pokepara)
class PokemonParam;
GFL_NAMESPACE_END(pokepara)
GFL_NAMESPACE_END(pml)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   リボン説明(上)
//------------------------------------------------------------------------------
class RibbonExplain
{
  GFL_FORBID_COPY_AND_ASSIGN( RibbonExplain );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  RibbonExplain( DrawerBase* base, gfl2::str::MsgData* msgData );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~RibbonExplain( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   *
   * @param   listener    リスナー
   */
  //------------------------------------------------------------------
  void Initialize( StatusDrawListener* listener );

private:
  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト
   */
  //------------------------------------------------------------------
  void InitializeText( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   セットアップ
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void Setup( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   リボンのセットアップ
   *
   * @param   ribbonID    リボンのID
   */
  //------------------------------------------------------------------
  void SetupRibbon( u32 ribbonID );

  //------------------------------------------------------------------
  /**
   * @brief   開く
   *
   * @param   ribbonID    リボンのID
   *
   * @param   "true  = 開くことに成功"
   * @param   "false = それ以外"
   */
  //------------------------------------------------------------------
  bool Open( u32 ribbonID );

  //------------------------------------------------------------------
  /**
   * @brief   閉じる
   */
  //------------------------------------------------------------------
  void Close( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   リボン名のセット
   *
   * @param   ribbonID    リボンのID
   */
  //------------------------------------------------------------------
  void SetRibbonName( u32 ribbonID );

  //------------------------------------------------------------------
  /**
   * @brief   リボン説明文のセット
   *
   * @param   ribbonID    リボンのID
   */
  //------------------------------------------------------------------
  void SetRibbonDesc( u32 ribbonID );

  //------------------------------------------------------------------
  /**
   * @brief   リボン所持数のセット
   *
   * @param   ribbonID    リボンのID
   */
  //------------------------------------------------------------------
  void SetRibbonCount( u32 ribbonID );

private:
  DrawerBase*                           m_pBase;

  gfl2::str::MsgData*                   m_pMsgData;

  StatusDrawListener*            m_pListener;

  const pml::pokepara::PokemonParam*    m_pPokeParam;

private:
  u32     m_RibbonID;

  bool    m_isOpen;

  bool    m_isRibbonNone;

private:
  gfl2::lyt::LytTextBox*        m_pTextName;

  gfl2::lyt::LytTextBox*        m_pTextDesc1;

  gfl2::lyt::LytTextBox*        m_pTextDesc2;

  gfl2::lyt::LytTextBox*        m_pTextCount;

  gfl2::lyt::LytParts*          m_pHelpParts;
};

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

#endif // __APP_STATUS_RIBBON_EXPLAIN_H_INCLUDED__
