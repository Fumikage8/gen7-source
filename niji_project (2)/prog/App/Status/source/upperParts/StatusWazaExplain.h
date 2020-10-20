#if !defined( __APP_STATUS_WAZA_EXPLAIN_H_INCLUDED__ )
#define  __APP_STATUS_WAZA_EXPLAIN_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    StatusWazaExplain.h
 * @date    2015/12/01 17:28:04
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：技説明(上)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/Status/source/StatusLowerDraw.h>

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


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   技説明(上)
//------------------------------------------------------------------------------
class WazaExplain
{
  GFL_FORBID_COPY_AND_ASSIGN( WazaExplain );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  WazaExplain( DrawerBase* base, gfl2::str::MsgData* msgData );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~WazaExplain( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   */
  //------------------------------------------------------------------
  void Initialize( void );

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
   * @brief   技のセットアップ
   *
   * @param   wazaNo    技No
   */
  //------------------------------------------------------------------
  void Setup( u32 wazaNo );

  //------------------------------------------------------------------
  /**
   * @brief   開く
   *
   * @param   wazaNo    技No
   *
   * @return  "true  = 開くことに成功"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool Open( u32 wazaNo );

  //------------------------------------------------------------------
  /**
   * @brief   閉じる
   */
  //------------------------------------------------------------------
  void Close( void );

  //------------------------------------------------------------------
  /**
   * @brief   閉じ終わったか？
   *
   * @return  "true  == 閉じ終わった"
   * @return  "false == 継続"
   */
  //------------------------------------------------------------------
  bool IsClosed( void ) const;

private:
  //------------------------------------------------------------------
  /**
   * @brief   パラメータの設定
   * 
   * @param   wazaNo    技ID
   */
  //------------------------------------------------------------------
  void SetParameter( u32 wazaNo );

  //------------------------------------------------------------------
  /**
   * @brief   技タイプ色の設定
   * 
   * @param   wazaNo    技ID
   */
  //------------------------------------------------------------------
  void SetWazaTypeColor( u32 wazaNo );

  //------------------------------------------------------------------
  /**
   * @brief   技のダメージタイプの設定
   * 
   * @param   wazaNo    技ID
   */
  //------------------------------------------------------------------
  void SetWazaDamageType( u32 wazaNo );

  //------------------------------------------------------------------
  /**
   * @brief   技の説明文の設定
   * 
   * @param   wazaNo    技ID
   */
  //------------------------------------------------------------------
  void SetWazaDesc( u32 wazaNo );

private:
  static const u32 BUTTON_NUM = BTN_ID_NUM;

private:
  DrawerBase*                   m_pBase;

  gfl2::str::MsgData*           m_pMsgData;

private:
  u32     m_WazaNo;

  bool    m_isOpen;

private:
  gfl2::lyt::LytParts*          m_pPlateParts;

  gfl2::lyt::LytParts*          m_pTypeIconParts;

  gfl2::lyt::LytPicture*        m_pTypeIcon;

  gfl2::lyt::LytTextBox*        m_pTextPower;

  gfl2::lyt::LytTextBox*        m_pTextHit;

  gfl2::lyt::LytTextBox*        m_pTextWazaInfo;
};

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

#endif // __APP_STATUS_WAZA_EXPLAIN_H_INCLUDED__
