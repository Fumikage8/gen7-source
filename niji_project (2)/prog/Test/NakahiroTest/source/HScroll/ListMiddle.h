#if PM_DEBUG
//=============================================================================
/**
 * @file    ListMiddle.h
 * @brief   Newペインリスト(中)サンプル
 * @author  Hiroyuki Nakamura
 * @date    2015.07.30
 */
//=============================================================================

#if !defined( LIST_MIDDLE_H_INCLUDED )
#define LIST_MIDDLE_H_INCLUDED

#pragma once


// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// niji
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"
#include "AppLib/include/Tool/AppToolNewPaneList.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class ItemIcon;
class MenuCursor;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Nakahiro)


class ListMiddle
  : public App::Tool::NewPaneList::ListListener
{
private:
  static const u32 PANE_MAX = 4;

  static const u32 LIST_DATA_MAX = 20;

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //-----------------------------------------------------------------------------
  ListMiddle( app::util::Heap * heap, app::util::G2DUtil * g2d, app::tool::ItemIcon * icon, app::tool::MenuCursor * cursor );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~ListMiddle();

  App::Tool::NewPaneList * GetNewPaneList(void);

  app::tool::MenuCursor * GetMenuCursor(void);

  void UpdateItemIcon(void);

  
private:
  void InitListData(void);
  void CreatePaneList(void);
  void DeletePaneList(void);
  virtual void PaneListListener_Write( const App::Tool::NewPaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  virtual void PaneListListener_MoveCursor( const App::Tool::NewPaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  virtual void PaneListListener_Decide( u32 pos );
  virtual void PaneListListener_StartItemChange( const App::Tool::NewPaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  virtual void PaneListListener_EndItemChange( const App::Tool::NewPaneList::PANE_DATA * pane, u32 pane_index, u32 pos1, u32 pos2 );

  void LoadIconRequest( u32 id, u32 item );


private:
  app::util::Heap * m_heap;
  app::util::G2DUtil * m_g2dUtil;

  app::tool::ItemIcon * m_itemIcon;
  app::tool::MenuCursor * m_menuCursor;

  App::Tool::NewPaneList * m_pNewPaneList;
  App::Tool::NewPaneList::PANE_DATA m_listPaneData[PANE_MAX];

  gfl2::lyt::LytWk * m_lytwk;

  u16 m_itemIconReq[PANE_MAX];
  u16 m_itemIconLoad[PANE_MAX];
  u16 m_itemIconSeq[PANE_MAX];

  u8 m_listData[LIST_DATA_MAX];
};


GFL_NAMESPACE_END(Nakahiro)
GFL_NAMESPACE_END(Test)


#endif  // LIST_MIDDLE_H_INCLUDED

#endif  // PM_DEBUG
