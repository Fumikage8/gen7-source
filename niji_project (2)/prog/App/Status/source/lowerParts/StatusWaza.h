#if !defined( __APP_STATUS_WAZA_H_INCLUDED__ )
#define __APP_STATUS_WAZA_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    StatusWaza.h
 * @date    2015/12/04 17:53:51
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：技(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gflib2
#include <Layout/include/gfl2_Layout.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)
class DrawerBase;
GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class AppCommonGrpIconData;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(pml)
GFL_NAMESPACE_BEGIN(pokepara)
class PokemonParam;
GFL_NAMESPACE_END(pokepara)
GFL_NAMESPACE_END(pml)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   技(下)
//------------------------------------------------------------------------------
class Waza
{
  GFL_FORBID_COPY_AND_ASSIGN( Waza );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  Waza( DrawerBase* base, app::tool::AppCommonGrpIconData* cmnIcon );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~Waza( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   */
  //------------------------------------------------------------------
  void Initialize( void );

  //------------------------------------------------------------------
  /**
   * @brief   セットアップ
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void Setup( const pml::pokepara::PokemonParam* pp );

private:
  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト
   */
  //------------------------------------------------------------------
  void InitializeText( void );

  //------------------------------------------------------------------
  /**
   * @brief   技名の設定
   *
   * @param   index     プレートインデックス
   * @param   wazaNo    技No
   */
  //------------------------------------------------------------------
  void SetWazaName( u32 index, u32 wazaNo );

  //------------------------------------------------------------------
  /**
   * @brief   技PPの設定
   *
   * @param   index プレートインデックス
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetWazaPP( u32 index, const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   技タイプの設定
   *
   * @param   index     プレートインデックス
   * @param   wazaNo    技No
   */
  //------------------------------------------------------------------
  void SetWazaType( u32 index, u32 wazaNo );

public:
  //------------------------------------------------------------------
  /**
   * @brief   技Noの取得
   *
   * @param   index     プレートインデックス
   *
   * @return  技No
   */
  //------------------------------------------------------------------
  u8 GetWazaNo( u32 index ) const;

public:
  static const u32 PLATE_NUM = 4;         //!< 技プレートの数

private:
  DrawerBase*                           m_pBase;

  app::tool::AppCommonGrpIconData*      m_pCommonIcon;

  const pml::pokepara::PokemonParam*    m_pPokeParam;

private:
  u8      m_WazaCount;

private:
  struct PlatePaneData
  {
    gfl2::lyt::LytParts*    parent;
    gfl2::lyt::LytTextBox*  wazaName;
    gfl2::lyt::LytTextBox*  wazaPP;
    gfl2::lyt::LytTextBox*  wazaPPNow;
    gfl2::lyt::LytTextBox*  wazaPPSlash;
    gfl2::lyt::LytTextBox*  wazaPPMax;
    gfl2::lyt::LytParts*    iconParts;
    gfl2::lyt::LytPicture*  iconPane;
  };

  PlatePaneData     m_PlatePaneData[PLATE_NUM];
};

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

#endif // __APP_STATUS_WAZA_H_INCLUDED__
