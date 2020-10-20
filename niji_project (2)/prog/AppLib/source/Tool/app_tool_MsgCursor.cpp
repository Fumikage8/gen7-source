//=============================================================================
/**
 * @file    app_tool_MsgCursor.cpp
 * @brief   メッセージカーソル
 * @author  Hiroyuki Nakamura
 * @date    12.04.25
 */
//=============================================================================

#include "AppLib/include/Tool/app_tool_MsgCursor.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   pane  カーソルペイン
 */
//-----------------------------------------------------------------------------
MsgCursor::MsgCursor( gfl2::lyt::LytPane * pane )
  : m_pane( pane )
  , m_visibleUser( false )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
MsgCursor::~MsgCursor(void)
{
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisible
 * @brief   表示切り替え
 * @date    2015.06.25
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void MsgCursor::SetVisible( bool flg )
{
  m_pane->SetVisible( flg );

/*
  if( pLytWk == NULL ){
    return;
  }
  gfl2::lyt::LytPane * pane = pLytWk->GetPane( paneIndex );
	pane->SetVisible( flg );
	if( flg == true ){
		nw::lyt::PaneList & list = pane->GetChildList();
		nw::lyt::PaneList::Iterator iter = list.GetBeginIter();
		nw::lyt::PaneList::Iterator end  = list.GetEndIter();
		while( 1 ){
			if( iter == end ){
				break;
			}
      gfl2::lyt::LytPane * child = dynamic_cast<gfl2::lyt::LytPane *>( &(*iter) );
			child->SetVisible( true );
			iter++;
		}
	}
*/

}

//-----------------------------------------------------------------------------
/**
 * @brief   手動で表示を操作する
 *
 * @param   flg   true = 手動
 */
//-----------------------------------------------------------------------------
void MsgCursor::SetVisibleUser( bool flg )
{
  m_visibleUser = flg;
}

//-----------------------------------------------------------------------------
/**
 * @brief   手動で表示を操作するか
 *
 * @retval  true  = 手動
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MsgCursor::GetVisibleUser(void)
{
  return m_visibleUser;
}


GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)
