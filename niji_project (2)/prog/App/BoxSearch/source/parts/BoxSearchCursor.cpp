//======================================================================
/**
 * @file BoxSearchCursor.cpp
 * @date 2015/07/28 16:49:57
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/BoxSearch/source/BoxSearchConst.h>
#include <App/BoxSearch/source/BoxSearchDrawerBase.h>
#include <App/BoxSearch/source/parts/BoxSearchCursor.h>

#include <AppLib/include/Util/app_util_2d.h>

// arc
#include <niji_conv_header/app/common_status/common_status_pane.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( BoxSearch )

static const u32 c_SkillBtnIndex[] = {
  PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_PARTSSKILLPLT_00,
  PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_PARTSSKILLPLT_01,
  PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_PARTSSKILLPLT_02,
  PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_PARTSSKILLPLT_03,
  PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_PARTSSKILLPLT_04,
};

static const u32 c_DescBtnIndex[] = {
  PANENAME_COMMONSTATUS_DSC_LOW_000_PANE_PARTSYN_00,
  PANENAME_COMMONSTATUS_DSC_LOW_000_PANE_PARTSYN_01,
};

//==============================================================================
// @brief   カーソル
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
BoxSearchCursor::BoxSearchCursor( DrawerBase* pBase, app::util::AppRenderingManager* pRenderingManager )
  : m_pBase( pBase )
  , m_pAppRenderingManager( pRenderingManager )
  , m_pMenuCursor( NULL )
  , m_pCursorBuffer( NULL )
  , m_Seq( SEQ_LOAD )
  , m_DeleteSeq(0)
{
  gfl2::heap::HeapBase* devHeap = m_pBase->GetDeviceHeap();
  app::util::Heap* heap = m_pBase->GetHeap();

  m_pMenuCursor = GFL_NEW(devHeap) app::tool::MenuCursor( heap, &m_pCursorBuffer );
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
BoxSearchCursor::~BoxSearchCursor( void )
{
  GFL_DELETE m_pMenuCursor;
}



//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void BoxSearchCursor::Update( void )
{
  if( m_pMenuCursor == NULL ) return;

  m_pMenuCursor->Update();

  switch( m_Seq )
  {
  case SEQ_LOAD:
    {
      if( !m_pMenuCursor->LoadResource() ) break;

      StartMenuCursor();  //! メニューカーソルの開始

      m_Seq = SEQ_MAIN;
    } break;

  case SEQ_DELETE:
    {
      if( m_DeleteSeq == 0 )
      {
        m_DeleteSeq++;
      }
      else if( m_DeleteSeq == 1 )
      {
        m_pMenuCursor->DeleteLayoutWork();
        m_pMenuCursor->DeleteResource();

        m_DeleteSeq++;
      }
      else {
        if( !m_pMenuCursor->IsModuleFree() ) break;
        m_Seq = SEQ_END;
      }
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void BoxSearchCursor::Draw( gfl2::gfx::CtrDisplayNo no )
{
  if( m_pMenuCursor == NULL ) return;
  if( m_Seq != SEQ_MAIN ) return;

  m_pMenuCursor->Draw( m_pAppRenderingManager, no, gfl2::lyt::DISPLAY_LOWER );
}

//------------------------------------------------------------------------------
/**
 * @brief   準備完了か？
 * @return  true == 完了
 */
//------------------------------------------------------------------------------
bool BoxSearchCursor::IsReady( void )
{
  return m_Seq == SEQ_MAIN;
}

//------------------------------------------------------------------------------
/**
 * @brief   終了可能か？
 * @return  true == 終了、false == 否
 */
//------------------------------------------------------------------------------
bool BoxSearchCursor::IsEnd( void )
{
  return m_Seq == SEQ_END;
}


//------------------------------------------------------------------------------
/**
 * @brief   メニューカーソルの開始
 */
//------------------------------------------------------------------------------
void BoxSearchCursor::StartMenuCursor( void )
{
  app::util::G2DUtil* pG2D = m_pBase->GetG2D();

  // メニューカーソルの作成
  m_pMenuCursor->CreateLayoutWork(
    pG2D->GetLayoutSystem(),
    pG2D->GetLayoutWorkSetup( gfl2::lyt::DISPLAY_LOWER ),
    pG2D->GetAppLytAccessor(),
    app::tool::MenuCursor::SIZE_SMALL,
    false );

  // カーソルを非表示
  SetVisible( false, app::tool::MenuCursor::CURSOR_ALL );

  // アニメの開始
  m_pMenuCursor->StartAnime();
}

//------------------------------------------------------------------------------
/**
 * @brief   メニューカーソルの終了
 */
//------------------------------------------------------------------------------
void BoxSearchCursor::EndMenuCursor( void )
{
  SetVisible( false, app::tool::MenuCursor::CURSOR_ALL );

  m_DeleteSeq = 0;
  m_Seq = SEQ_DELETE;
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソルの表示切替
 * @param   visible   表示フラグ
 * @param   id        カーソルID(デフォルト = CURSOR_LEFT)
 */
//------------------------------------------------------------------------------
void BoxSearchCursor::SetVisible( bool visible, u32 id )
{
  m_pMenuCursor->SetVisible( visible, id );
}

//------------------------------------------------------------------------------
/**
 * @brief   メニューカーソルの取得
 * @return  app::tool::MenuCursorクラスポインタ
 */
//------------------------------------------------------------------------------
app::tool::MenuCursor* BoxSearchCursor::GetMenuCursor( void )
{
  return m_pMenuCursor;
}

GFL_NAMESPACE_END( BoxSearch )
GFL_NAMESPACE_END( App )
