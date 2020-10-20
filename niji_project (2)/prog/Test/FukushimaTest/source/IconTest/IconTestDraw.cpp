#if PM_DEBUG

//======================================================================
/**
 * @file IconTestDraw.cpp
 * @date 2015/07/17 16:41:57
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "Test/FukushimaTest/source/IconTest/IconTestDraw.h"
#include "Test/FukushimaTest/source/IconTest/IconTestDrawListener.h"
#include "Test/FukushimaTest/source/IconTest/IconTestPokeIcon.h"

#include <AppLib/include/Tool/app_tool_PokeIcon.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <Print/include/SystemFont.h>

// gflib2
#include <Fade/include/gfl2_FadeManager.h>

// arc
#include <niji_conv_header/app/common_pokeList/common_pokeList.h>
#include <niji_conv_header/app/common_pokeList/common_pokeList_pane.h>
#include <niji_conv_header/app/common_pokeList/common_pokeList_anim_list.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace {
#define PLATE_NUM       pml::PokeParty::MAX_MEMBERS
#define ICON_TYPE_NUM   3

  enum Sequence {
    SEQ_INIT,
    SEQ_READY,
    SEQ_IN,
    SEQ_MAIN,
    SEQ_OUT,
    SEQ_END_READY,
    SEQ_END
  };

  //! @brief  ボタンID
  enum ButtonID
  {
    BTN_ID_PLATE_TL,    //!< プレート左上
    BTN_ID_PLATE_TR,    //!< プレート右上
    BTN_ID_PLATE_ML,    //!< プレート左中
    BTN_ID_PLATE_MR,    //!< プレート右中
    BTN_ID_PLATE_BL,    //!< プレート左下
    BTN_ID_PLATE_BR,    //!< プレート右下
    BTN_ID_DECIDE,      //!< 決定
    BTN_ID_CANCEL,      //!< キャンセル
    BTN_ID_X,           //!< 機能ボタンＸ
    BTN_ID_Y,           //!< 機能ボタンＹ
    BTN_ID_NUM
  };
} // namespace


//------------------------------------------------------------------------------
/**
 * @brief	  コンストラクタ
 */
//------------------------------------------------------------------------------
IconTestDraw::IconTestDraw( app::util::Heap* pHeap, app::util::AppRenderingManager* pAppRenderingManager )
  : app::ui::UIView( pHeap, pHeap->GetDeviceAllocator(), pHeap->GetDeviceAllocator() )
  , m_pHeap( pHeap )
  , m_uiListener( NULL )
  , m_pAppRenderingManager( pAppRenderingManager )
  , m_pGameManager( GFL_SINGLETON_INSTANCE( GameSys::GameManager ) )
  , m_pFadeManager( GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager ) )
  , m_LayoutID( 0 )
  , m_Seq( SEQ_INIT )
  , m_pPokeIcon( NULL )
{
  //gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();

  //m_pPokeIcon = GFL_NEW(devHeap) PokeIconTool( m_pHeap, 6 );

  for( u32 i=0; i<PLATE_NUM; ++i )
  {
    m_IconType[i] = 0;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief	  デストラクタ
 */
//------------------------------------------------------------------------------
IconTestDraw::~IconTestDraw( void )
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief	  このビュー専用のUIリスナー設定
 * @param   pListener   UIリスナー
 */
//------------------------------------------------------------------------------
void IconTestDraw::SetTitleViewUIListener( Test::Fukushima::IconTestDrawListener* pListener )
{
  m_uiListener = pListener;
}

//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 * @param   pAppParam
 * @param   pAppLytBuff   レイアウトバイナリ
 */
//------------------------------------------------------------------------------
void IconTestDraw::Init( void* pAppLytBuff, gfl2::str::MsgData* pMsgData, print::WordSet* pWordSet )
{
  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  Initialize2D( m_pHeap, pAppLytBuff, pMsgData, pWordSet );

  SetInputListener( this );

  m_Seq = SEQ_READY;
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool IconTestDraw::End( void )
{
  GFL_SAFE_DELETE( m_pPokeIcon );

  print::SystemFont_ResetNumFontInfo(); //@fix GFNMCat[1083]
  print::SystemFont_DeleteNumFont();

  Delete2D();
  DeleteGraphicsSystem();

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void IconTestDraw::Update( void )
{
  app::ui::UIView::Update();

  // シーケンス処理
  switch( m_Seq )
  {
  case SEQ_READY:
    {
      if( !IsReplaceIcon() ) break;

      // ステータス画面を開く
      Open();
    } break;

  case SEQ_IN:
    {
      if( !IsEndInOutAnim( true ) ) break;
      
      this->SetInputEnabled( true );

      m_Seq = SEQ_MAIN;
    } break;

  case SEQ_OUT:
    {
      if( !IsEndInOutAnim(false) ) break;

      m_Seq = SEQ_END_READY;
    } break;

  case SEQ_END_READY:
    {
      //if( m_pPokeIcon && !m_pPokeIcon->DeleteFunc() ) break;
      if( m_pPokeIcon && !m_pPokeIcon->EndFunc() ) break;

      if( m_uiListener ) m_uiListener->OnEnd();

      m_Seq = SEQ_END;
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void IconTestDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
 
  if( m_Seq > SEQ_READY )
  {
    pG2DUtil->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 * @return  trueで破棄完了
 */
//------------------------------------------------------------------------------
bool IconTestDraw::IsExit( void )
{
  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   ステータス画面のオープン
 */
//------------------------------------------------------------------------------
void IconTestDraw::Open( void )
{
  // フェードイン
  m_pFadeManager->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, 3 );
	
  m_Seq = SEQ_IN;
}

//------------------------------------------------------------------------------
/**
 * @brief   ステータス画面のクローズ
 */
//------------------------------------------------------------------------------
void IconTestDraw::Close( void )
{
  this->SetInputEnabled( false );

  gfl2::math::Vector4 color( 0, 0, 0, 255 );
  m_pFadeManager->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );

  m_Seq = SEQ_OUT;
}

//------------------------------------------------------------------------------
/**
 * @brief   イン・アウトアニメが終了したかどうか
 * @param   in    インアニメかどうか
 * @return  true = 終了、false = 継続
 */
//------------------------------------------------------------------------------
bool IconTestDraw::IsEndInOutAnim( bool in )
{
  return m_pFadeManager->IsEnd( gfl2::Fade::DISP_LOWER )
    && m_pFadeManager->IsEnd( gfl2::Fade::DISP_UPPER );
}


//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 * @param   pHeap         ヒープ
 * @param   pAppLytBuff   レイアウトバイナリデータ
 */
//------------------------------------------------------------------------------
void IconTestDraw::Initialize2D( app::util::Heap* pHeap, void* pAppLytBuff, gfl2::str::MsgData* pMsgData, print::WordSet* pWordSet )
{
  // アーカイブIDテーブル
  const app::util::G2DUtil::LytResData resTbl[] = {
    { pAppLytBuff, 1, static_cast<u32>(app::util::G2DUtil::ATTACH_ROOT) },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    { // MIN_LOW
      0,
      LYTRES_COMMON_POKELIST_POKELIST_MIN_LOW_000_BFLYT,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000___NUM,
      pAppLytBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
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
    pMsgData,
    pWordSet );

  CreateButtonManager();

  this->SetInputEnabled( false );

  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();

  // ポケモンアイコン
  {
    app::tool::PokeIconToolEx::PokeIconToolParam param;
    param.pUIView = this;
    param.pHeap   = m_pHeap;
    param.iconMax = pml::PokeParty::MAX_MEMBERS * ICON_TYPE_NUM;
    param.sync    = true;
    m_pPokeIcon = GFL_NEW(devHeap) app::tool::PokeIconToolEx( param );
  }

  // 
  static const u32 c_PlatePartsIndex[] = {
    PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_00,
    PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_03,
    PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_01,
    PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_04,
    PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_02,
    PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_05,
  };

  app::util::G2DUtil* pG2D = this->GetG2DUtil();
  gfl2::lyt::LytWk* lytWk = pG2D->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID resID = pG2D->GetLayoutResourceID( 0 );

  for( u32 i=0; i<6; ++i )
  {
    gfl2::lyt::LytParts* plateParts = lytWk->GetPartsPane( c_PlatePartsIndex[i] );
    gfl2::lyt::LytParts* iconParts = lytWk->GetPartsPane( plateParts, PANENAME_POKELIST_PLT_LOW_000_PANE_PARTS_POKEICON, &resID );
    m_pIconPane[i] = lytWk->GetPicturePane( iconParts, PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON, &resID );
  }

  // ポケモンアイコンのセット
  LoadPokeIcon();
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンアイコンの読み込み
 */
//------------------------------------------------------------------------------
void IconTestDraw::LoadPokeIcon( void )
{
  GameSys::GameData* gd = m_pGameManager->GetGameData();
  pml::PokeParty* party = gd->GetPlayerParty();
  u32 memberCount = party->GetMemberCount();
  u32 formMax = 3;

  for( u32 i=0; i<memberCount; ++i )
  {
    pml::pokepara::PokemonParam* pp = party->GetMemberPointer( i );

    pml::FormNo form = pp->GetFormNo();   //!< 変更前のフォルムNoを取得

    for( u32 j=0; j<formMax; ++j )
    {
      pp->ChangeFormNo( static_cast<pml::FormNo>(j) );
      m_pPokeIcon->LoadPokeIcon( i + (j * 6), pp );
    }

    pp->ChangeFormNo( form );   //!< 変更前のフォルムNoに戻す
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   アイコンの置き換えが完了したか？
 */
//------------------------------------------------------------------------------
bool IconTestDraw::IsReplaceIcon( void )
{
  GameSys::GameData* gd = m_pGameManager->GetGameData();
  pml::PokeParty* party = gd->GetPlayerParty();
  u32 memberCount = party->GetMemberCount();

  bool result = true;

  for( u32 i=0; i<memberCount; ++i )
  {
    //if( !m_pPokeIcon->ReplaceTexture( i, m_pIconPane[i] ) )
    if( !m_pPokeIcon->IsLoadPokeIcon( i + 6 ) ) return false;
    if( !m_pPokeIcon->IsLoadPokeIcon( i + 12 ) ) return false;

    if( !m_pPokeIcon->ReplaceIcon( i, m_pIconPane[i] ) )
    {
      result = false;
      break;
    }
  }

  return result;
}






//==============================================================================
/**
 *      ↓　以下がリスナー処理　↓
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   リソース生成前に必要なデータを登録。フォントリソースなど
 */
//------------------------------------------------------------------------------
void IconTestDraw::OnPreLoadResource( void )
{
  // num_font.bffntの登録
  print::SystemFont_CreateNumFont( m_pHeap->GetDeviceHeap(), m_pHeap->GetDeviceHeap() );
  print::SystemFont_SetLytSystemNumFont();
}

//------------------------------------------------------------------------------
/**
 * @brief   キー入力通知用リスナ
 * @param   pKey
 * @param   pStick
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult IconTestDraw::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( m_uiListener == NULL ) return DISABLE_ACCESS;

  if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    Close();
  }

  return ENABLE_ACCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインアクション通知用リスナ
 * @param   paneId
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult IconTestDraw::OnLayoutPaneEvent( const u32 paneId )
{
  if( m_uiListener == NULL ) return DISABLE_ACCESS;

  switch( paneId )
  {
  case BTN_ID_PLATE_TL:
  case BTN_ID_PLATE_TR:
  case BTN_ID_PLATE_ML:
  case BTN_ID_PLATE_MR:
  case BTN_ID_PLATE_BL:
  case BTN_ID_PLATE_BR:
    {
      u32 index = paneId - BTN_ID_PLATE_TL;

      m_IconType[index] = (m_IconType[index] + 1) % ICON_TYPE_NUM;

      u32 iconID = index + (m_IconType[index] * PLATE_NUM);
      m_pPokeIcon->ReplaceIcon( iconID, m_pIconPane[index] );
    } break;
  }

  return ENABLE_ACCESS;
}




//==============================================================================
/**
 *      ↓　以下がリスナー処理　↓
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ボタンマネージャの生成
 */
//------------------------------------------------------------------------------
void IconTestDraw::CreateButtonManager( void )
{
  const app::ui::ButtonInfoEx c_DefInfo[] = {
    { // BTN_ID_PLATE_TL
      BTN_ID_PLATE_TL, NULL, NULL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00_TOUCH,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00_CANSEL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00_TOUCH_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00_ACTIVE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00_PASSIVE,
    },
    { // BTN_ID_PLATE_TR
      BTN_ID_PLATE_TR, NULL, NULL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03_TOUCH,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03_CANSEL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03_TOUCH_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03_ACTIVE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03_PASSIVE,
    },
    { // BTN_ID_PLATE_ML
      BTN_ID_PLATE_ML, NULL, NULL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01_TOUCH,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01_CANSEL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01_TOUCH_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01_ACTIVE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01_PASSIVE,
    },
    { // BTN_ID_PLATE_MR
      BTN_ID_PLATE_MR, NULL, NULL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04_TOUCH,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04_CANSEL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04_TOUCH_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04_ACTIVE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04_PASSIVE,
    },
    { // BTN_ID_PLATE_BL
      BTN_ID_PLATE_BL, NULL, NULL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02_TOUCH,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02_CANSEL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02_TOUCH_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02_ACTIVE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02_PASSIVE,
    },
    { // BTN_ID_PLATE_BR
      BTN_ID_PLATE_BR, NULL, NULL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05_TOUCH,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05_CANSEL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05_TOUCH_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05_ACTIVE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05_PASSIVE,
    },
    { // BTN_ID_DECIDE
      BTN_ID_DECIDE, NULL, NULL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_OK_TOUCH,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_OK_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_OK_CANSEL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_OK_TOUCH_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_OK_ACTIVE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_OK_PASSIVE,
    },
    { // BTN_ID_CANCEL
      BTN_ID_CANCEL, NULL, NULL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_BACK_TOUCH,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_BACK_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_BACK_CANSEL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_BACK_TOUCH_RELEASE,
      0,
      0,
    },
    { // BTN_ID_X
      BTN_ID_X, NULL, NULL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTSBUTTON_X_TOUCH,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTSBUTTON_X_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTSBUTTON_X_CANSEL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTSBUTTON_X_TOUCH_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTSBUTTON_X_ACTIVE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTSBUTTON_X_PASSIVE,
    },
    { // BTN_ID_Y
      BTN_ID_Y, NULL, NULL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTSBUTTON_Y_TOUCH,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTSBUTTON_Y_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTSBUTTON_Y_CANSEL,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTSBUTTON_Y_TOUCH_RELEASE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTSBUTTON_Y_ACTIVE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTSBUTTON_Y_PASSIVE,
    },
  };

  const gfl2::lyt::LytPaneIndex c_BtnPicPaneIndex[BTN_ID_NUM][2] = {
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
    GFL_NEW_LOW_ARRAY(m_pHeap->GetDeviceHeap()) app::ui::ButtonInfoEx[BTN_ID_NUM];
  gfl2::lyt::LytWk* pLytWk = this->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID resID = this->GetG2DUtil()->GetLayoutResourceID( 0 );

  for( u32 i=0; i<BTN_ID_NUM; ++i )
  {
    gfl2::lyt::LytParts* pParts = pLytWk->GetPartsPane( c_BtnPicPaneIndex[i][0] );

    info[i] = c_DefInfo[i];
    info[i].picture_pane = pLytWk->GetPane( c_BtnPicPaneIndex[i][0] );
    info[i].bound_pane = pLytWk->GetPane( pParts, c_BtnPicPaneIndex[i][1], &resID );
  }

  app::ui::UIView* pUIView = this;
  pUIView->CreateButtonManager( m_pHeap, pLytWk, info, BTN_ID_NUM );

  GFL_DELETE_ARRAY info;
}

GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

#endif // PM_DEBUG