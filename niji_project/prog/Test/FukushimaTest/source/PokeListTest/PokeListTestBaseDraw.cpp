#if PM_DEBUG

//==============================================================================
/**
 * @file    PokeListTestBaseDraw.cpp
 * @author  fukushima_yuya
 * @date    2015.06.22
 * @brief   ポケモンリストベース描画
 */
//==============================================================================

// niji
#include "Test/FukushimaTest/source/PokeListTest/PokeListTestBaseDraw.h"

#include <App/PokeList/include/PokeListAppParam.h>
#include <App/PokeList/source/parts/PokeListDrawerBase.h>
#include <App/PokeList/source/work/PokeListLocalWork.h>
#include <AppLib/include/Ui/UIView.h>
#include <Sound/include/Sound.h>

// gflib2
#include <Fade/include/gfl2_FadeManager.h>

// arc
#include <niji_conv_header/app/common_pokeList/common_pokeList.h>
#include <niji_conv_header/app/common_pokelist/common_pokelist_pane.h>
#include <niji_conv_header/app/common_pokelist/common_pokeList_anim_list.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 * @param   heap    ヒープ
 * @param   renderingManager    レンダリングマネージャ
 */
//------------------------------------------------------------------------------
TestPokeListBaseDraw::TestPokeListBaseDraw( app::util::Heap * heap, app::util::AppRenderingManager * renderingManager )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_pHeap( heap )
  , m_uiListener( NULL )
  , m_pAppParam( NULL )
  , m_pAppRenderingManager( renderingManager )
  , m_pFadeManager( NULL )
  , m_LayoutID( 0 )
  , m_Seq( SEQ_INIT )
  , m_pDrawerBase( NULL )
  , m_pStaticWork( NULL )
  , m_pLocalWork( NULL )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
TestPokeListBaseDraw::~TestPokeListBaseDraw( void )
{
  ;
}



//------------------------------------------------------------------------------
/**
 * @brief	  このビュー専用のUIリスナー設定
 * @param   pListener   UIリスナー
 */
//------------------------------------------------------------------------------
void TestPokeListBaseDraw::SetTitleViewUIListener( App::PokeList::PokeListDrawListener* pListener )
{
  m_uiListener = pListener;
}

//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 */
//------------------------------------------------------------------------------
void TestPokeListBaseDraw::Init( App::PokeList::APP_PARAM* pAppParam, void* pAppLytBuff, App::PokeList::StaticWork* pStaticWork, App::PokeList::LocalWork* pLocalWork )
{
  m_pAppParam = pAppParam;
  m_pAppRenderingManager = pAppParam->mpAppRenderingManager;
  m_pFadeManager = pAppParam->mpFadeManager;

  m_pStaticWork = pStaticWork;
  m_pLocalWork = GFL_NEW(m_pHeap->GetDeviceHeap()) App::PokeList::LocalWork( m_pHeap->GetDeviceHeap(), NULL, NULL, NULL );

  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  Initialize2D( m_pHeap, pAppLytBuff );
  InitializeText();

  SetInputListener( this );

  m_Seq = SEQ_READY;
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool TestPokeListBaseDraw::End( void )
{
  if( !EndFunc() ) return false;

  GFL_SAFE_DELETE( m_pLocalWork );
  GFL_SAFE_DELETE( m_pDrawerBase );

  Delete2D();
  DeleteGraphicsSystem();

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void TestPokeListBaseDraw::Update( void )
{
  app::ui::UIView::Update();

  UpdateFunc();
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void TestPokeListBaseDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  DrawFunc( no );
}

//-----------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 */
//-----------------------------------------------------------------------------
bool TestPokeListBaseDraw::IsExit( void )
{
  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンリスト画面のオープン
 */
//------------------------------------------------------------------------------
void TestPokeListBaseDraw::PokeListOpen( void )
{
  m_pFadeManager->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, 3 );

  m_Seq = SEQ_IN;
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンリスト画面のクローズ
 */
//------------------------------------------------------------------------------
void TestPokeListBaseDraw::PokeListClose( void )
{
  gfl2::math::Vector4 color( 0, 0, 0, 255 );
  m_pFadeManager->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );

  this->SetInputEnabled( false );

  m_Seq = SEQ_OUT;
}

//------------------------------------------------------------------------------
/**
 * @brief   イン・アウトアニメが終了したかどうか
 * @param   in    インアニメかどうか
 * @return  true = 終了、false = 継続
 */
//------------------------------------------------------------------------------
bool TestPokeListBaseDraw::IsEndInOutAnim( bool in )
{
  return m_pFadeManager->IsEnd( gfl2::Fade::DISP_LOWER ) && m_pFadeManager->IsEnd( gfl2::Fade::DISP_UPPER );
}

//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 * @param   pHeap         ヒープ
 * @param   pAppLytBuff   レイアウトバイナリデータ
 */
//------------------------------------------------------------------------------
void TestPokeListBaseDraw::Initialize2D( app::util::Heap* pHeap, void* pAppLytBuff )
{
  // アーカイブIDテーブル
  const app::util::G2DUtil::LytResData resTbl[] = {
    { pAppLytBuff, 1, static_cast<u32>(app::util::G2DUtil::ATTACH_ROOT) }
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    { // 下
      0,
      LYTRES_COMMON_POKELIST_POKELIST_MIN_LOW_000_BFLYT,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000___NUM,
      pAppLytBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true
    }
  };

  // 2Dの生成
  Create2D(
    pHeap,
    NULL,
    1,
    resTbl,
    GFL_NELEMS(resTbl),
    setupData,
    GFL_NELEMS(setupData),
    this->UNUSE_MESSAGE_DATA );

  this->SetInputEnabled( false ); //!< 入力を無効に

  {
    gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();

    // 共通描画処理
    m_pDrawerBase = GFL_NEW(devHeap) App::PokeList::DrawerBase( this, m_pHeap, m_LayoutID, m_pStaticWork, m_pLocalWork );
    GFL_ASSERT( m_pDrawerBase );

    CreateButtonManager();

    // 各ポケモンリスト画面のセットアップ
    SetupUI();
  }
}






//==============================================================================
/**
 *      ↓　以下がボタンマネージャ関連　↓
 */
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief   ボタンマネージャの生成
 */
//------------------------------------------------------------------------------
void TestPokeListBaseDraw::CreateButtonManager( void )
{
  const app::ui::ButtonInfoEx c_DefInfo[] = {
    { // BTN_ID_PLATE_TL
      App::PokeList::BTN_ID_PLATE_TL, NULL, NULL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00_TOUCH,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00_CANSEL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00_TOUCH_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00_ACTIVE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00_PASSIVE,
    },
    { // BTN_ID_PLATE_TR
      App::PokeList::BTN_ID_PLATE_TR, NULL, NULL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03_TOUCH,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03_CANSEL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03_TOUCH_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03_ACTIVE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03_PASSIVE,
    },
    { // BTN_ID_PLATE_ML
      App::PokeList::BTN_ID_PLATE_ML, NULL, NULL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01_TOUCH,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01_CANSEL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01_TOUCH_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01_ACTIVE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01_PASSIVE,
    },
    { // BTN_ID_PLATE_MR
      App::PokeList::BTN_ID_PLATE_MR, NULL, NULL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04_TOUCH,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04_CANSEL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04_TOUCH_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04_ACTIVE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04_PASSIVE,
    },
    { // BTN_ID_PLATE_BL
      App::PokeList::BTN_ID_PLATE_BL, NULL, NULL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02_TOUCH,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02_CANSEL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02_TOUCH_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02_ACTIVE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02_PASSIVE,
    },
    { // BTN_ID_PLATE_BR
      App::PokeList::BTN_ID_PLATE_BR, NULL, NULL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05_TOUCH,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05_CANSEL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05_TOUCH_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05_ACTIVE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05_PASSIVE,
    },
    { // BTN_ID_DECIDE
      App::PokeList::BTN_ID_DECIDE, NULL, NULL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_OK_TOUCH,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_OK_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_OK_CANSEL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_OK_TOUCH_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_OK_ACTIVE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_OK_PASSIVE,
    },
    { // BTN_ID_CANCEL
      App::PokeList::BTN_ID_CANCEL, NULL, NULL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_BACK_TOUCH,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_BACK_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_BACK_CANSEL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_BACK_TOUCH_RELEASE,
      0,
      0,
    },
    { // BTN_ID_X
      App::PokeList::BTN_ID_X, NULL, NULL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTSBUTTON_X_TOUCH,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTSBUTTON_X_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTSBUTTON_X_CANSEL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTSBUTTON_X_TOUCH_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTSBUTTON_X_ACTIVE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTSBUTTON_X_PASSIVE,
    },
    { // BTN_ID_Y
      App::PokeList::BTN_ID_Y, NULL, NULL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTSBUTTON_Y_TOUCH,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTSBUTTON_Y_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTSBUTTON_Y_CANSEL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTSBUTTON_Y_TOUCH_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTSBUTTON_Y_ACTIVE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTSBUTTON_Y_PASSIVE,
    },
  };

  const gfl2::lyt::LytPaneIndex c_BtnPicPaneIndex[App::PokeList::BTN_ID_NUM][2] = {
    { PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_00, PANENAME_POKELIST_PLT_LOW_000_PANE_BOUND_00    },
    { PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_03, PANENAME_POKELIST_PLT_LOW_000_PANE_BOUND_00    },
    { PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_01, PANENAME_POKELIST_PLT_LOW_000_PANE_BOUND_00    },
    { PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_04, PANENAME_POKELIST_PLT_LOW_000_PANE_BOUND_00    },
    { PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_02, PANENAME_POKELIST_PLT_LOW_000_PANE_BOUND_00    },
    { PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_05, PANENAME_POKELIST_PLT_LOW_000_PANE_BOUND_00    },
    { PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_OK, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00      },
    { PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_BACK, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00    },
    { PANENAME_POKELIST_MIN_LOW_000_PANE_PARTSBUTTON_X, PANENAME_COMMON_BTN_LOW_009_PANE_BOUND_00 },
    { PANENAME_POKELIST_MIN_LOW_000_PANE_PARTSBUTTON_Y, PANENAME_COMMON_BTN_LOW_009_PANE_BOUND_00 },
  };

  app::ui::ButtonInfoEx* info =
    GFL_NEW_LOW_ARRAY(m_pDrawerBase->GetDeviceHeap()) app::ui::ButtonInfoEx[App::PokeList::BTN_ID_NUM];
  gfl2::lyt::LytWk* pLytWk = m_pDrawerBase->GetLytWk();

  for( u32 i=0; i<App::PokeList::BTN_ID_NUM; ++i )
  {
    gfl2::lyt::LytParts* pParts = m_pDrawerBase->GetParts( c_BtnPicPaneIndex[i][0] );

    info[i] = c_DefInfo[i];
    info[i].picture_pane = m_pDrawerBase->GetPane( c_BtnPicPaneIndex[i][0] );;
    info[i].bound_pane = m_pDrawerBase->GetPane( c_BtnPicPaneIndex[i][1], pParts );
  }

  app::ui::UIView* pUIView = m_pDrawerBase->GetUIView();
  pUIView->CreateButtonManager( m_pHeap, pLytWk, info, App::PokeList::BTN_ID_NUM );

  app::tool::ButtonManager* pBtnMan = m_pDrawerBase->GetButtonManager();
  pBtnMan->SetButtonBindKey( App::PokeList::BTN_ID_CANCEL, gfl2::ui::BUTTON_B, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );

  for( u32 i=0; i<App::PokeList::BTN_ID_NUM; ++i )
  {
    if( i != App::PokeList::BTN_ID_CANCEL )
    {
      pBtnMan->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
    }
    else {
      pBtnMan->SetButtonSelectSE( i, SEQ_SE_CANCEL1 );
    }
  }

  SetupButton( pBtnMan );

  GFL_DELETE_ARRAY info;
}




GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

#endif // PM_DEBUG