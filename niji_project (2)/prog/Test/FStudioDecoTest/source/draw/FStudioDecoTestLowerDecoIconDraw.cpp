#if PM_DEBUG

//======================================================================
/**
 * @file    FStudioDecoTestLowerDecoIconDraw.cpp
 * @date    2016/10/28 11:51:59
 * @author  fukushima_yuya
 * @brief   ファインダースタジオデコテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FStudioDecoTestLowerDecoIconDraw.h"

#include <Test/FStudioDecoTest/source/FStudioDecoTestDrawListener.h>
#include <Test/FStudioDecoTest/source/deco/FStudioDecoTestItem.h>
#include <Test/FStudioDecoTest/source/util/FStudioDecoTestUIUtility.h>

#include <niji_conv_header/app/test/fukushima_test/fstudio_deco_test/FStudioDecoTest_anim_list.h>
// デコアイテムのデータをインクルード
#include <niji_conv_header/app/test/fukushima_test/fstudio_deco_test/DecoItemData.cdat>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FSDeco )

LowerDecoIconDraw::LowerDecoIconDraw( app::util::Heap* heap, app::util::AppRenderingManager* renderMan )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_pHeap( heap )
  , m_pAppRenderingManager( renderMan )
  , m_pUtil( NULL )
  , m_DecoType(DECO_ITEM_KIND_NUM)
  , m_isOpen(false)
  , m_pNullBGMask(NULL)
{
  ;
}



//----------------------------------------------------------------------
// @brief   初期化・終了処理
//----------------------------------------------------------------------
bool LowerDecoIconDraw::InitializeSystem( void )
{
  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  SetInputListener( this );

  return true;
}

bool LowerDecoIconDraw::Terminate( void )
{
  m_pUtil->ResetInputExclusiveKeyByBtnAnim();
  GFL_SAFE_DELETE( m_pUtil );

  Delete2D();
  DeleteGraphicsSystem();

  return true;
}

void LowerDecoIconDraw::Initialize2D( void* lytDatBuf )
{
  // アーカイブIDテーブル
  const app::util::G2DUtil::LytResData resTbl[] = {
    { lytDatBuf, 1, static_cast<u32>( app::util::G2DUtil::ATTACH_ROOT ) }
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    { // 下
      0,
      LYTRES_FSTUDIODECOTEST_DECOTEST_LOWDECOICON_BFLYT,
      LA_FSTUDIODECOTEST_DECOTEST_LOWDECOICON___NUM,
      lytDatBuf,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true
    }
  };

  // 2Dの生成
  Create2D(
    m_pHeap,
    NULL,
    1,
    resTbl,
    GFL_NELEMS( resTbl ),
    setupData,
    GFL_NELEMS( setupData ),
    this->UNUSE_MESSAGE_DATA );

  this->SetInputEnabled( false );

  m_pUtil = GFL_NEW( m_pHeap->GetSystemHeap() ) UIUtility( this );
  m_pUtil->SetPrevInputExclusiveKeyByBtnAnim();
  m_pUtil->SetInputExclusiveKeyByBtnAnim( true );
}

void LowerDecoIconDraw::InitializePane( void )
{
  m_pNullBGMask = m_pUtil->GetPane( PANENAME_DECOTEST_LOWDECOICON_PANE_NULL_DECOITEM );

  const u32 c_DecoNullPane[] = {
    PANENAME_DECOTEST_LOWDECOICON_PANE_NULL_STAMP_S,
    PANENAME_DECOTEST_LOWDECOICON_PANE_NULL_STAMP_L,
    PANENAME_DECOTEST_LOWDECOICON_PANE_NULL_TEXT,
    PANENAME_DECOTEST_LOWDECOICON_PANE_NULL_FRAME,
  };
  for( u32 i = 0; i < DECO_ITEM_KIND_NUM; ++i )
  {
    m_pNullDeco[i] = m_pUtil->GetPane( c_DecoNullPane[i] );
  }
}

void LowerDecoIconDraw::InitializeButtonManager( void )
{
  const app::ui::ButtonInfoEx c_DefInfo = {
    0, NULL, NULL,
    app::tool::ButtonManager::ANIMATION_NULL,
    app::tool::ButtonManager::ANIMATION_NULL,
    app::tool::ButtonManager::ANIMATION_NULL,
    app::tool::ButtonManager::ANIMATION_NULL,
    app::tool::ButtonManager::ANIMATION_NULL,
    app::tool::ButtonManager::ANIMATION_NULL,
  };

  app::ui::ButtonInfoEx* info =
    GFL_NEW_LOW_ARRAY( m_pHeap->GetSystemHeap() ) app::ui::ButtonInfoEx[DECO_ITEM_ID_NUM];

  for( u32 i = 0; i < DECO_ITEM_ID_NUM; ++i )
  {
    info[i]               = c_DefInfo;
    info[i].button_id     = i;
    info[i].picture_pane  = m_pUtil->GetPicture( c_DecoItemPane[i] );
    info[i].bound_pane    = m_pUtil->GetPicture( c_DecoItemPane[i] );
  }

  this->CreateButtonManager( m_pHeap, m_pUtil->GetLytWk(), info, DECO_ITEM_ID_NUM );

  GFL_DELETE( info );

  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();

  for( u32 i = 0; i < DECO_ITEM_ID_NUM; ++i )
  {
    btnMan->SetButtonLongPressFrame( i, ITEM_SELECT_FRAME );
    btnMan->SetButtonInputEnable( i, false );
  }
}



//----------------------------------------------------------------------
// @brief   更新・描画処理
//----------------------------------------------------------------------
void LowerDecoIconDraw::Update( void )
{
  app::ui::UIView::Update();
}

void LowerDecoIconDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  this->GetG2DUtil()->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
}



//----------------------------------------------------------------------
// @brief   開閉処理
//----------------------------------------------------------------------
void LowerDecoIconDraw::Open( DecoItemKind type )
{
  if( !m_isOpen )
  {
    // デコアイコン背景の表示
    m_pUtil->SetVisiblePane( m_pNullBGMask, true );

    // デコの種類を変更
    ChangeDecoType( type );

    this->SetModal( true );
    this->SetInputEnabled( true );

    m_isOpen = true;
  }
}

void LowerDecoIconDraw::Close( void )
{
  if( m_isOpen )
  {
    m_isOpen = false;

    this->SetModal( false );
    this->SetInputEnabled( false );

    // デコアイコン背景の表示
    m_pUtil->SetVisiblePane( m_pNullBGMask, false );
  }
}



//----------------------------------------------------------------------
// @brief   デコアイコンの種類を変更
//----------------------------------------------------------------------
void LowerDecoIconDraw::ChangeDecoType( DecoItemKind type )
{
  // 表示切替
  for( u32 i = 0; i < DECO_ITEM_KIND_NUM; ++i )
  {
    m_pUtil->SetVisiblePane( m_pNullDeco[i], false );
  }
  m_pUtil->SetVisiblePane( m_pNullDeco[type], true );

  // ボタンの入力
  for( u32 i = 0; i < DECO_ITEM_ID_NUM; ++i )
  {
    bool enable = ( c_DecoItemKind[i] == type );
    m_pUtil->GetButtonManager()->SetButtonInputEnable( i, enable );
  }

  m_DecoType = type;
}



//----------------------------------------------------------------------
// @brief   リスナー関数群
//----------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult LowerDecoIconDraw::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if( isTouch )
  {
    pTouchPanel->GetXY( &m_TouchPos );
  }

  return ENABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult LowerDecoIconDraw::OnKeyAction( gfl2::ui::Button* btn, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick )
{

  return ENABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult LowerDecoIconDraw::OnLayoutPaneLongPressEvent( const u32 paneID )
{
  GFL_PRINT( "OnLayoutPaneLongPressEvent : paneID[%d]\n", paneID );

  DecoItemKind kind = CAST_DECO_TYPE( c_DecoItemKind[paneID] );

  if( kind == DECO_ITEM_KIND_FRAME )
  {
    m_TouchPos = gfl2::math::VEC3( 0.0f, 0.0f, 0.0f );
  }

  DecoItem::SetDecoItemData( paneID, kind, m_TouchPos );
  m_pListener->OnAddDecoItem();

  // デコ選択画面を閉じる
  Close();

  return ENABLE_ACCESS;
}

GFL_NAMESPACE_END( FSDeco )
GFL_NAMESPACE_END( Test )

#endif // #if PM_DEBUG
