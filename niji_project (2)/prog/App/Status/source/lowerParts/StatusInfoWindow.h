#if !defined( __APP_STATUS_INFOMATION_WINDOW_H_INCLUDED__ )
#define __APP_STATUS_INFOMATION_WINDOW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    StatusInfoWindow.h
 * @date    2015/12/03 20:46:27
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：インフォメーションウィンドウ(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowSheetTypeListener.h>
#include <AppLib/include/Util/AppRenderingManager.h>

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <str/include/gfl2_Str.h>

// pml
#include <pml/include/pmlib.h>


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
class InfoWindowSheetType;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)


//------------------------------------------------------------------------------
// @brief   インフォメーションウィンドウ(下)
//------------------------------------------------------------------------------
class InfoWindow
{
  GFL_FORBID_COPY_AND_ASSIGN( InfoWindow );

public:
  //------------------------------------------------------------------
  /**
   * @brief    コンストラクタ
   */
  //------------------------------------------------------------------
  InfoWindow( DrawerBase* base, app::util::AppRenderingManager* renderMan );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~InfoWindow( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   *
   * @param   tokuseiMsgData  特性メッセージデータ
   * @param   itemMsgData     アイテムメッセージデータ
   * @param   listener        インフォウィンドウリスナー
   */
  //------------------------------------------------------------------
  void Initialize( gfl2::str::MsgData* tokuseiMsgData, gfl2::str::MsgData* itemMsgData, app::tool::InfoWindowSheetTypeListener* listener );
  
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
   * @brief   文字列の格納バッファの生成
   */
  //------------------------------------------------------------------
  void CreateStrBuf( void );

  //------------------------------------------------------------------
  /**
   * @brief   インフォメーションウィンドウの生成
   */
  //------------------------------------------------------------------
  void CreateInfoWindow( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  void Update( void );
  
  //------------------------------------------------------------------
  /**
   * @brief   描画
   */
  //------------------------------------------------------------------
  void Draw( gfl2::gfx::CtrDisplayNo displayNo, u8 order, u32 layerNo );

  //------------------------------------------------------------------
  /**
   * @brief   準備完了か？
   */
  //------------------------------------------------------------------
  bool IsReady( void );

  //------------------------------------------------------------------
  /**
   * @brief   終了シーケンス
   *
   * @return  "true" == 終了
   * @return  "false" == 終了処理中
   */
  //------------------------------------------------------------------
  bool DeleteSequence( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   特性のインフォメーションウィンドウを開く
   */
  //------------------------------------------------------------------
  void OpenTokusei( void );

  //------------------------------------------------------------------
  /**
   * @brief   道具のインフォメーションウィンドウを開く
   */
  //------------------------------------------------------------------
  void OpenItem( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   インフォメーションウィンドウを開く
   *
   * @param   name  項目名
   * @param   desc  説明文
   */
  //------------------------------------------------------------------
  void OpenInfoWindow( gfl2::str::StrBuf* name, gfl2::str::StrBuf* desc );

private:
  DrawerBase*                               m_pBase;

  app::util::AppRenderingManager*           m_pAppRenderingManager;

  app::tool::InfoWindowSheetType*           m_pInfoWindow;

  app::tool::InfoWindowSheetTypeListener*   m_pInfoWindowListener;

  const pml::pokepara::PokemonParam*        m_pPokeParam;

private:
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

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

#endif // __APP_STATUS_INFOMATION_WINDOW_H_INCLUDED__
