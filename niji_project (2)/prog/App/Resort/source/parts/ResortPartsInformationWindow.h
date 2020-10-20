#if !defined( __RESORT_PARTS_INFORMATION_WINDOW_H__ )
#define  __RESORT_PARTS_INFORMATION_WINDOW_H__
#pragma once
//======================================================================
/**
 * @file ResortPartsInformationWindow.h
 * @date 2015/10/02 15:05:24
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowSheetTypeListener.h>
#include <AppLib/include/Util/AppRenderingManager.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Resort )
class DrawerBase;
class ResortPaneList;
GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )
class InfoWindowSheetType;
GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( str )
class MsgData;
class StrBuf;
GFL_NAMESPACE_END( str )
GFL_NAMESPACE_END( gfl2 )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Resort )

//------------------------------------------------------------------------------
// @brief   インフォメーションウィンドウリスナークラス
//------------------------------------------------------------------------------
class ResortInfoWindowListener
  : public app::tool::InfoWindowSheetTypeListener
{
public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  ResortInfoWindowListener( DrawerBase* pBase, ResortPaneList* paneList );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~ResortInfoWindowListener( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   閉じ終わった
   */
  //------------------------------------------------------------------
  virtual void OnClose( void );

private:
  DrawerBase*       m_pBase;

  ResortPaneList*   m_pPaneList;
};

//------------------------------------------------------------------------------
// @brief   インフォメーションウィンドウクラス
//------------------------------------------------------------------------------
class ResortInfoWindow
{
  GFL_FORBID_COPY_AND_ASSIGN( ResortInfoWindow );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  ResortInfoWindow(
    DrawerBase* pBase,
    app::util::AppRenderingManager* pAppRenderingManager,
    ResortPaneList* paneList,
    gfl2::str::MsgData* itemInfoMsgData );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~ResortInfoWindow( void );

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
   * @return  "true" == 終了
   * @return  "false" == 終了処理中
   */
  //------------------------------------------------------------------
  bool DeleteSequence( void );

  //------------------------------------------------------------------
  /**
   * @brief   インフォメーションウィンドウを開く
   * @param   itemNo    アイテムNo
   * @param   btnPos    ボタンの位置座標
   */
  //------------------------------------------------------------------
  void Open( u16 itemNo, gfl2::math::Vector3 btnPos );

private:
  enum {
    STR_ITEM_NAME,
    STR_ITEM_INFO,
    STR_NUM,
  };
private:
  DrawerBase*   m_pBase;

  app::tool::InfoWindowSheetType*     m_pInfoWindow;

  ResortInfoWindowListener*           m_pInfoWindowListener;

  ResortPaneList*                     m_pPaneList;

  gfl2::str::MsgData*                 m_pItemInfo;

  gfl2::str::StrBuf*                  m_pStrBuf[STR_NUM];

  u32   m_DeleteSeq;
};

GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )

#endif // __RESORT_PARTS_INFORMATION_WINDOW_H__