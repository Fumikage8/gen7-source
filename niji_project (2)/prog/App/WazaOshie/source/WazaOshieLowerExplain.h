#ifndef NIJI_PROJECT_APP_WAZAOSHIE_LOWER_EXPLAIN_H_INCLUDED
#define NIJI_PROJECT_APP_WAZAOSHIE_LOWER_EXPLAIN_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file WazaOshieLowerExplain.h
 * @date 2015/07/01 18:58:50
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <Layout/include/gfl2_Layout.h>

// pml
#include <pml/include/pmlib.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )
class AppCommonGrpIconData;
GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( WazaOshie )
class DrawerBase;
GFL_NAMESPACE_END( WazaOshie )
GFL_NAMESPACE_END( App )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( WazaOshie )

//------------------------------------------------------------------------------
// @brief   技説明画面
//------------------------------------------------------------------------------
class WazaOshieExplain
{
  GFL_FORBID_COPY_AND_ASSIGN( WazaOshieExplain );

public:
  struct SetupParam
  {
    WazaNo    wazaNo;
    u32       nowPP;
    u32       maxPP;
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   * @param   pBase   描画共通処理
   */
  //------------------------------------------------------------------
  WazaOshieExplain( DrawerBase* pBase, app::tool::AppCommonGrpIconData* pCmnIcon );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~WazaOshieExplain( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   */
  //------------------------------------------------------------------
  void Init( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   セットアップ
   */
  //------------------------------------------------------------------
  void Setup( u32 mode, SetupParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   表示切替
   * @param   vis   表示フラグ
   */
  //------------------------------------------------------------------
  void SetVisible( bool vis );

  //------------------------------------------------------------------
  /**
   * @brief   技IDの取得
   * @return  wazaNo
   */
  //------------------------------------------------------------------
  WazaNo GetWazaNo( void ) const;

public:
  static const u32 WAZA_PARAM_NUM = 3;
  static const u32 WAZA_PARAM_VALUE_NUM = 2;
  static const u32 BUTTON_NUM = 2;

private:
  DrawerBase* m_pBase;

  app::tool::AppCommonGrpIconData*    m_pCommonIcon;

  gfl2::str::MsgData*   m_pInfoMsg;
  gfl2::str::StrBuf*    m_pStrBuf;

  WazaNo  m_WazaNo;

  gfl2::lyt::LytPane*   m_pNullAll;

  gfl2::lyt::LytTextBox*    m_pWazaName;
  gfl2::lyt::LytTextBox*    m_pWazaPP;
  gfl2::lyt::LytTextBox*    m_pWazaPPNum;
  gfl2::lyt::LytTextBox*    m_pWazaPPMaxNum;
  gfl2::lyt::LytTextBox*    m_pWazaPPSlash;
  gfl2::lyt::LytPicture*    m_pWazaTypeIcon;

  gfl2::lyt::LytTextBox*    m_pWazaParamName[WAZA_PARAM_NUM];
  gfl2::lyt::LytTextBox*    m_pWazaParamNum[WAZA_PARAM_VALUE_NUM];

  gfl2::lyt::LytTextBox*    m_pWazaDesc;

  gfl2::lyt::LytTextBox*    m_pButtonText[BUTTON_NUM];
};

GFL_NAMESPACE_END( WazaOshie )
GFL_NAMESPACE_END( App )

#endif // NIJI_PROJECT_APP_WAZAOSHIE_LOWER_EXPLAIN_H_INCLUDED