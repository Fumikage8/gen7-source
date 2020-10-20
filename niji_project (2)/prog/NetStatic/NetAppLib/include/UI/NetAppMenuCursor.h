// ============================================================================
/*
 * @file NetAppMenuCursor.h
 * @brief 通信アプリ用メニューカーソルです。
 * @date 2015.06.05
 */
// ============================================================================
#if !defined( NETAPPMENUCURSOR_H_INCLUDE )
#define NETAPPMENUCURSOR_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "AppLib/include/tool/app_tool_MenuCursor.h"
#include "AppLib/include/util/app_util_2d.h"
#include "niji_conv_header/sound/SoundMiddleID.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(UI)


class NetAppMenuCursor
{
  GFL_FORBID_COPY_AND_ASSIGN( NetAppMenuCursor );
public:

  NetAppMenuCursor( NetAppLib::System::ApplicationWorkBase* pWork );

  NetAppMenuCursor(
    NetAppLib::System::ApplicationWorkBase*  pWork,
    app::util::G2DUtil*                      pG2DUtil,
    NetAppLib::System::ResourceID            menuCursorResourceID,
    u32                                      layoutResourceIndex,
    u32                                      layoutWorkID );

  NetAppMenuCursor(
    NetAppLib::System::ApplicationWorkBase*  pWork,
    app::util::G2DUtil*                      pG2DUtil,
    NetAppLib::System::ResourceID            menuCursorResourceID,
    u32                                      layoutResourceIndex,
    u32                                      layoutWorkID,
    const u32*                               pButtonPartsPaneIDTable,
    const u32*                               pCursorPaneIDTable,
    s32                                      cursorPositionMax );

  NetAppMenuCursor(
    NetAppLib::System::ApplicationWorkBase*  pWork,
    app::util::G2DUtil*                      pG2DUtil,
    NetAppLib::System::ResourceID            menuCursorResourceID,
    u32                                      layoutResourceIndex,
    u32                                      layoutWorkID,
    gfl2::lyt::LytPane*                      pCursorPosPanes[],
    s32                                      cursorPositionMax );

  virtual~NetAppMenuCursor();









  void Create(
    app::util::G2DUtil*                      pG2DUtil,
    u32                                      layoutResourceIndex,
    u32                                      layoutWorkID,
    NetAppLib::System::ResourceID            menuCursorResourceID = NetAppLib::System::COMMON_RESOURCE_ID_MENU_CURSOR );

  void Create(
    app::util::G2DUtil*                      pG2DUtil,
    u32                                      layoutResourceIndex,
    u32                                      layoutWorkID,
    const u32*                               pButtonPartsPaneIDTable,
    const u32*                               pCursorPaneIDTable,
    s32                                      cursorPositionMax,
    NetAppLib::System::ResourceID            menuCursorResourceID = NetAppLib::System::COMMON_RESOURCE_ID_MENU_CURSOR );

  void Create(
    app::util::G2DUtil*                      pG2DUtil,
    u32                                      layoutResourceIndex,
    u32                                      layoutWorkID,
    gfl2::lyt::LytPane*                      pCursorPosPanes[],
    s32                                      cursorPositionMax,
    NetAppLib::System::ResourceID            menuCursorResourceID = NetAppLib::System::COMMON_RESOURCE_ID_MENU_CURSOR );




  //------------------------------------------------------------------
  /**
    *  @brief    カーソル位置Pane列を設定
    *  @note     CursorPositionは0にされる
    */
  //------------------------------------------------------------------
  void  SetCusorPosPanes(gfl2::lyt::LytPane* pCursorPosPanes[], const s32 cursorPositionMax);
  void  SetCusorPosPanes(const u32*  pButtonPartsPaneIDTable, const u32* pCursorPaneIDTable, const s32 cursorPositionMax);

  bool CanDestroy() const;
  void Destroy();

  void Update();
  void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  void MoveUp( SoundMiddleID ePlaySEID = SMID_NULL );
  void MoveDown( SoundMiddleID ePlaySEID = SMID_NULL );

  s32 GetCursorPosition() { return m_CursorPosition; }

  app::tool::MenuCursor* GetAppToolMenuCursor() { return m_pMenuCursor; }

  void SetVisible( bool bVisible ) { m_pMenuCursor->SetVisible( bVisible ); }
  bool IsVisible() const { return m_pMenuCursor->IsVisible(); }

private:

  void setupMenuCursor(
    NetAppLib::System::ApplicationWorkBase*             pWork,
    app::util::G2DUtil*                                 pG2DUtil,
    NetAppLib::System::ResourceID                       menuCursorResourceID,
    u32                                                 layoutResourceIndex,
    u32                                                 layoutWorkID
  );

  void setMenuCursorPosition( s32 cursorPosition );

  NetAppLib::System::ApplicationWorkBase*               m_pWork;
  app::tool::MenuCursor*                                m_pMenuCursor;
  gfl2::lyt::LytMultiResID                              m_LayoutResourceID;
  gfl2::lyt::LytWk*                                     m_pLayoutWork;
  gfl2::lyt::LytPane**                                  m_pCursorPosPanes;       /**<  カーソル位置を定義するPane  */
  s32                                                   m_CursorPositionMax;
  s32                                                   m_CursorPosition;

};


GFL_NAMESPACE_END(UI)
GFL_NAMESPACE_END(NetAppLib)

#endif // NETAPPMENUCURSOR_H_INCLUDE
