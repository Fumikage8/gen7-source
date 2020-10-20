#ifndef NIJI_PROJECT_APP_WAZAOSHIE_INFO_WINDOW_H_INCLUDED
#define NIJI_PROJECT_APP_WAZAOSHIE_INFO_WINDOW_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file WazaOshieInfoWindow.h
 * @date 2015/07/16 12:20:57
 * @author fukushima_yuya
 * @brief 技教え・技忘れ画面のインフォメーションウィンドウ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "App/WazaOshie/source/WazaOshieConst.h"

#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowSheetTypeListener.h>

// pml
#include <pml/include/pmlib.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( WazaOshie )
class DrawerBase;
GFL_NAMESPACE_END( WazaOshie )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )
class InfoWindowSheetType;
GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( WazaOshie )

//------------------------------------------------------------------------------
// @brief   インフォメーションウィンドウリスナークラス
//------------------------------------------------------------------------------
class WazaOshieInfoWindowListener
  : public app::tool::InfoWindowSheetTypeListener
{
public:
  //------------------------------------------------------------------
  /**
   * @brief   閉じ終わった
   */
  //------------------------------------------------------------------
  virtual void OnClose( void ) { ; }
};

//------------------------------------------------------------------------------
// @brief   インフォメーションウィンドウクラス
//------------------------------------------------------------------------------
class WazaOshieInfoWindow
{
  GFL_FORBID_COPY_AND_ASSIGN( WazaOshieInfoWindow );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  WazaOshieInfoWindow( DrawerBase* pBase, app::util::AppRenderingManager* pAppRenderingManager, WazaOshieInfoWindowListener* listener );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~WazaOshieInfoWindow( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   準備完了か？
   */
  //------------------------------------------------------------------
  bool IsReady( void );

  //------------------------------------------------------------------
  /**
   * @brief   終了シーケンス
   * @return  "true" == 終了
   * @return  "false" == 終了処理中
   */
  //------------------------------------------------------------------
  bool DeleteSequence( void );

  //------------------------------------------------------------------
  /**
   * @brief   特性のインフォメーションウィンドウを開く
   * @param   tokusei   特性ID
   */
  //------------------------------------------------------------------
  void OpenTokusei( TokuseiNo tokusei );

  //------------------------------------------------------------------
  /**
   * @brief   道具のインフォメーションウィンドウを開く
   * @param   itemNo    道具ID
   */
  //------------------------------------------------------------------
  void OpenItem( u16 itemNo );

private:
  //------------------------------------------------------------------
  /**
   * @brief   インフォメーションウィンドウを開く
   * @param   name  項目名
   * @param   desc  説明文
   */
  //------------------------------------------------------------------
  void OpenInfoWindow( gfl2::str::StrBuf* name, gfl2::str::StrBuf* desc );

private:
  DrawerBase*   m_pBase;

  app::tool::InfoWindowSheetType*     m_pInfoWindow;

  WazaOshieInfoWindowListener*        m_pInfoWindowListener;

  u32   m_DeleteSeq;

  enum {
    MSG_TOKUSEI = 0,
    MSG_ITEM,
    MSG_NUM,

    STR_NAME = 0,
    STR_DESC,
    STR_NUM,
  };
  gfl2::str::MsgData*   m_pInfoMsg[MSG_NUM];
  gfl2::str::StrBuf*    m_pStrBuf[STR_NUM];

private:
  gfl2::math::Vector3   m_TokuseiInfoPos;

  gfl2::math::Vector3   m_ItemInfoPos;

  gfl2::math::Vector3   m_OpenInfoPos;
};


GFL_NAMESPACE_END( WazaOshie )
GFL_NAMESPACE_END( App )

#endif // NIJI_PROJECT_APP_WAZAOSHIE_INFO_WINDOW_H_INCLUDED