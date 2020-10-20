//==============================================================================
/**
 * @file    PokeListBaseDraw.cpp
 * @author  fukushima_yuya
 * @date    2015.06.22
 * @brief   ポケモンリストベース描画
 */
//==============================================================================

// niji
#include "PokeListBaseDraw.h"

#include <App/PokeList/include/PokeListAppParam.h>
#include <App/PokeList/source/parts/PokeListCursorDraw.h>
#include <App/PokeList/source/parts/PokeListPlateDraw.h>
#include <App/PokeList/source/parts/PokeListWindow.h>
#include <App/PokeList/source/parts/PokeListDrawerBase.h>
#include <App/PokeList/source/work/PokeListStaticWork.h>
#include <App/PokeList/source/work/PokeListLocalWork.h>

#include <AppLib/include/Ui/UIView.h>
#include <AppLib/include/Util/app_util_KeyRepeatController.h>
#include <Print/include/SystemFont.h>
#include <Sound/include/Sound.h>

// gflib2
#include <Fade/include/gfl2_FadeManager.h>

// arc
#include <niji_conv_header/app/common_pokeList/common_pokeList.h>
#include <niji_conv_header/app/common_pokelist/common_pokelist_pane.h>
#include <niji_conv_header/app/common_pokelist/common_pokeList_anim_list.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 * @param   heap    ヒープ
 * @param   renderingManager    レンダリングマネージャ
 */
//------------------------------------------------------------------------------
PokeListBaseDraw::PokeListBaseDraw( app::util::Heap * heap, app::util::AppRenderingManager * renderingManager )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_pHeap( heap )
  , m_uiListener( NULL )
  , m_pAppParam( NULL )
  , m_pAppRenderingManager( renderingManager )
  , m_pFadeManager( NULL )
  , m_bCreatedNumFont( false )
  , m_LayoutID( 0 )
  , m_Seq( SEQ_INIT )
  , m_MemberCount( pml::PokeParty::MAX_MEMBERS )
  , m_pDrawerBase( NULL )
  , m_pStaticWork( NULL )
  , m_pLocalWork( NULL )
  , m_pCursor( NULL )
  , m_pMessageWindow( NULL )
  , m_pPokeWindow( NULL )
  , m_pKeyRepeatLeft( NULL )
  , m_pKeyRepeatRight( NULL )
  , m_CursorPosMaxX( 0 )
  , m_CursorPosMaxY( 0 )
  , m_CursorPosMaxRY( 0 )
  , m_PrevCurPosX( 0 )
  , m_PrevCurPosY( 0 )
  , m_isEndCursor( false )
{
  for( u8 i=0; i<pml::PokeParty::MAX_MEMBERS; ++i )
  {
    m_pPokePlate[i] = NULL;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
PokeListBaseDraw::~PokeListBaseDraw( void )
{
  ;
}



//------------------------------------------------------------------------------
/**
 * @brief	  このビュー専用のUIリスナー設定
 * @param   pListener   UIリスナー
 */
//------------------------------------------------------------------------------
void PokeListBaseDraw::SetTitleViewUIListener( PokeListDrawListener* pListener )
{
  m_uiListener = pListener;
}

//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 */
//------------------------------------------------------------------------------
void PokeListBaseDraw::Init( APP_PARAM* pAppParam, void* pAppLytBuff, StaticWork* pStaticWork, LocalWork* pLocalWork )
{
  m_pAppParam = pAppParam;
  m_pAppRenderingManager = pAppParam->mpAppRenderingManager;
  m_pFadeManager = pAppParam->mpFadeManager;

  m_pStaticWork = pStaticWork;
  m_pLocalWork = pLocalWork;

  if( m_pStaticWork && m_pStaticWork->GetProcMode() == MODE_BATTLE )
  {
    if( m_pFadeManager == NULL )
    {
      m_pFadeManager = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
    }
  }

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
bool PokeListBaseDraw::End( void )
{
  if( !EndFunc() ) return false;

  if( m_pPokeWindow )
  {
    if( !m_pPokeWindow->Terminate() )
    {
      return false;
    }

    GFL_SAFE_DELETE( m_pPokeWindow );
  }

  GFL_SAFE_DELETE( m_pMessageWindow );

  if( !IsEndCursor() ) return false;

  for( u8 i=0; i<pml::PokeParty::MAX_MEMBERS; ++i )
  {
    GFL_SAFE_DELETE( m_pPokePlate[i] );
  }

  GFL_SAFE_DELETE( m_pKeyRepeatLeft );
  GFL_SAFE_DELETE( m_pKeyRepeatRight );

  GFL_SAFE_DELETE( m_pDrawerBase );

  Delete2D();
  DeleteGraphicsSystem();

  if( m_bCreatedNumFont )
  {
    print::SystemFont_ResetNumFontInfo(); //@fix GFNMCat[1083]
    print::SystemFont_DeleteNumFont();
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void PokeListBaseDraw::Update( void )
{
  // UIViewの更新
  app::ui::UIView::Update();

  // プレートの更新
  for( u8 i=0; i<pml::PokeParty::MAX_MEMBERS; ++i )
  {
    if( m_pPokePlate[i] ) m_pPokePlate[i]->Update();
  }

  // カーソルの更新
  if( m_pCursor ) m_pCursor->Update();

  // ウィンドウの更新
  if( m_pPokeWindow ) m_pPokeWindow->Update();

  // 各ポケモンリストの更新
  UpdateFunc();
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void PokeListBaseDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  DrawFunc( no );
}

//-----------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 */
//-----------------------------------------------------------------------------
bool PokeListBaseDraw::IsExit( void )
{
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief   カーソルの終了処理
 *
 * @return  "true  = 終了"
 * @return  "false = それ以外"
 */
//-----------------------------------------------------------------------------
bool PokeListBaseDraw::IsEndCursor( void )
{
  if( m_pCursor )
  {
    m_pCursor->Update();

    if( !m_isEndCursor )
    {
      m_pCursor->EndMenuCursor();
      m_isEndCursor = true;

      return false;
    }
    else if( !m_pCursor->IsEnd() )
    {
      return false;
    }
    else {
      GFL_SAFE_DELETE( m_pCursor );
    }
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンリスト画面のオープン
 */
//------------------------------------------------------------------------------
void PokeListBaseDraw::PokeListOpen( void )
{
  m_pFadeManager->RequestIn(
    gfl2::Fade::DISP_DOUBLE,
    gfl2::Fade::FADE_TYPE_ALPHA,
    gfl2::Fade::FADE_DEFAULT_SYNC );

  m_Seq = SEQ_IN;
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンリスト画面のクローズ
 * @param   endMode
 */
//------------------------------------------------------------------------------
void PokeListBaseDraw::PokeListClose( EndMode endMode )
{
  gfl2::math::Vector4 color( 0, 0, 0, 255 );

  Mode mode = m_pStaticWork->GetProcMode();

  // 入力を無効
  this->SetInputEnabled( false );

  // 終了モードのセット
  if( m_pStaticWork ) m_pStaticWork->SetEndMode( endMode );

  if( mode != MODE_BATTLE )
  {
    m_pFadeManager->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );
    m_Seq = SEQ_OUT;
  }
  else {
    const ModeContext& context = m_pStaticWork->GetModeContext();
    BattleChangeMode changeMode = static_cast<BattleChangeMode>( *context.battle.btl_change_mode );

    if( changeMode == BTL_CHANGE_SINGLE ||
       (endMode == END_MODE_TIMEUP)     ||    // @fix NMCat[4207]
       (changeMode == BTL_CHANGE_DOUBLE && endMode == END_MODE_SELECT)
    ) {
      m_Seq = SEQ_OUT;
    }

    if( endMode != END_MODE_CANCEL )
    {
      m_pFadeManager->ForceOut( gfl2::Fade::DISP_LOWER, &color );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   イン・アウトアニメが終了したかどうか
 * @param   in    インアニメかどうか
 * @return  true = 終了、false = 継続
 */
//------------------------------------------------------------------------------
bool PokeListBaseDraw::IsEndInOutAnim( bool in )
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
void PokeListBaseDraw::Initialize2D( app::util::Heap* pHeap, void* pAppLytBuff )
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
    m_pDrawerBase = GFL_NEW(devHeap) DrawerBase( this, m_pHeap, m_LayoutID, m_pStaticWork, m_pLocalWork );
    {
      GFL_ASSERT( m_pDrawerBase );

      // BGアニメの再生
      m_pDrawerBase->StartAnim( LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__BG_00_BG_LOOP_00 );
      m_pDrawerBase->StartAnim( LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__BG_02_BG_LOOP_00 );
    }

    CreateButtonManager();

    // 各ポケモンリスト画面のセットアップ
    SetupUI();

    // キーリピート
    {
      m_CursorPosMaxX = m_MemberCount;
      m_CursorPosMaxY = static_cast<u32>((m_MemberCount + 1) / 2);

      if( m_pStaticWork->GetProcMode() == MODE_JOIN )
      {
        m_CursorPosMaxX = pml::PokeParty::MAX_MEMBERS + 1;
        m_CursorPosMaxY = (pml::PokeParty::MAX_MEMBERS + 2) / 2;
      }

      m_pKeyRepeatLeft = GFL_NEW(devHeap) app::util::KeyRepeatController( m_CursorPosMaxY-1, m_CursorPosMaxX-1 );
      GFL_ASSERT( m_pKeyRepeatLeft );

      m_CursorPosMaxRY = ((m_MemberCount % 2) == 0) ? m_CursorPosMaxY : m_CursorPosMaxY - 1;
      m_pKeyRepeatRight = GFL_NEW(devHeap) app::util::KeyRepeatController( m_CursorPosMaxRY-1, m_CursorPosMaxX-1 );
      GFL_ASSERT( m_pKeyRepeatRight );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソル移動
 * @param   key
 */
//------------------------------------------------------------------------------
void PokeListBaseDraw::MoveCursor( gfl2::ui::VectorDevice* key )
{
  GFL_ASSERT( m_pStaticWork );
  GFL_ASSERT( m_pKeyRepeatLeft );
  GFL_ASSERT( m_pKeyRepeatRight );

  const u32 ROW_MAX = 2;
  const u32 COL_MAX = m_CursorPosMaxY;

  CursorID cid = m_pStaticWork->GetCursorID();
  u32 curPos = m_pStaticWork->GetCursorPos( cid );
  const u32 nowCurPos = curPos;

  u32 curPosX = static_cast<u32>(curPos % ROW_MAX);
  u32 curPosY = static_cast<u32>(curPos / ROW_MAX);

  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
  const bool isDecideBtn = btnMan->IsButtonActive( BTN_ID_DECIDE );

  m_pKeyRepeatLeft->UpdateInfo( key, curPosY, curPos );
  m_pKeyRepeatRight->UpdateInfo( key, curPosY, curPos );


  if( m_pKeyRepeatLeft->IsRepeat( gfl2::ui::DIR_UP ) )
  {
    if( curPosX == 0 )
    {
      curPosY = (curPosY + COL_MAX - 1) % COL_MAX;
    }
    else if( curPosX == 1 && m_pKeyRepeatRight->IsRepeat( gfl2::ui::DIR_UP ) )
    {
      curPosY = (curPosY + m_CursorPosMaxRY - 1) % m_CursorPosMaxRY;
    }

    curPos = (curPosY * ROW_MAX) + curPosX;

    if( curPos >= m_MemberCount )
    {
      if( isDecideBtn )
      {
        curPos = 6;
      }
      else {
        curPosY = static_cast<u32>((m_MemberCount + 1) / 2) - 1;
        curPos = (curPosY * ROW_MAX) + curPosX;
      }
    }
  }
  else if( m_pKeyRepeatLeft->IsRepeat( gfl2::ui::DIR_DOWN ) )
  {
    if( curPosX == 0 )
    {
      curPosY = (curPosY + 1) % COL_MAX;
    }
    else if( curPosX == 1 && m_pKeyRepeatRight->IsRepeat( gfl2::ui::DIR_DOWN ) )
    {
      curPosY = (curPosY + 1) % m_CursorPosMaxRY;
    }

    curPos = (curPosY * ROW_MAX) + curPosX;

    if( curPos >= m_MemberCount )
    {
      if( isDecideBtn )
      {
        curPos = 6;
      }
      else {
        curPosY = 0;
        curPos = (curPosY * ROW_MAX) + curPosX;
      }
    }
  }
  else if( m_pKeyRepeatLeft->IsRepeat( gfl2::ui::DIR_LEFT ) )
  {
    if( curPos == 6 )
    {
      curPosX = m_MemberCount % 2;
      curPosY = (m_MemberCount > 1) ? (m_MemberCount - 1) / 2 : 0;
    }
    else if( curPosX == 0 )
    {
      curPosX = ROW_MAX - 1;
      curPosY = (curPosY + COL_MAX - 1) % COL_MAX;
    }
    else {
      curPosX--;
    }

    curPos = (curPosY * ROW_MAX) + curPosX;

    if( curPos >= m_MemberCount )
    {
      if( isDecideBtn )
      {
        curPos = 6;
      }
      else {
        curPosX = (m_MemberCount + 1) % 2;
        curPosY = static_cast<u32>((m_MemberCount + 1) / 2) - 1;
        curPos = (curPosY * ROW_MAX) + curPosX;
      }
    }
  }
  else if( m_pKeyRepeatLeft->IsRepeat( gfl2::ui::DIR_RIGHT ) )
  {
    if( curPos == 6 )
    {
      curPosX = 0;
      curPosY = 0;
    }
    else if( curPosX == (ROW_MAX - 1) )
    {
      curPosX = 0;
      curPosY = (curPosY + 1) % COL_MAX;
    }
    else {
      curPosX++;
    }

    curPos = (curPosY * ROW_MAX) + curPosX;

    if( curPos >= m_MemberCount )
    {
      if( isDecideBtn )
      {
        curPos = 6;
      }
      else {
        curPosX = 0;
        curPosY = 0;
        curPos = (curPosY * ROW_MAX) + curPosX;
      }
    }
  }

  if( curPos != nowCurPos )
  {
    // m_pCursor->SetCursorPos(  );

    m_pStaticWork->SetCursorPos( cid, static_cast<CursorPos>(curPos) );

    Sound::PlaySE( SEQ_SE_SELECT1 );  //!< カーソル移動音
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   プレートの有効設定
 */
//------------------------------------------------------------------------------
void PokeListBaseDraw::SetPlateEnable( void )
{
  StaticWork* staticWork = m_pDrawerBase->GetStaticWork();

  if( !staticWork->IsMultiBattle() )
  {
    for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS; ++i )
    {
      m_PlateEnable[i] = (i < m_MemberCount);
    }
  }
  else {
    u32 memberCount_Myself = staticWork->GetMemberCount( PARTY_SELF );
    u32 memberCount_Friend = staticWork->GetMemberCount( PARTY_FRIEND );

    const u32 c_PlateIndex_Left[] = {
      PLATE_00,
      PLATE_02,
      PLATE_04,
    };
    const u32 c_PlateIndex_Right[] = {
      PLATE_01,
      PLATE_03,
      PLATE_05,
    };

    u32 mode = staticWork->GetModeContext().battle.btl_mode;

    for( u32 i=0; i<3; ++i )
    {
      if( mode == BTL_MODE_MULTI_LEFT )
      {
        u32 leftIndex = c_PlateIndex_Left[i];
        m_PlateEnable[leftIndex] = (i < memberCount_Myself);

        u32 rightIndex = c_PlateIndex_Right[i];
        m_PlateEnable[rightIndex] = (i < memberCount_Friend);
      }
      else {
        u32 leftIndex = c_PlateIndex_Left[i];
        m_PlateEnable[leftIndex] = (i < memberCount_Friend);

        u32 rightIndex = c_PlateIndex_Right[i];
        m_PlateEnable[rightIndex] = (i < memberCount_Myself);
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンリストの再起動
 */
//------------------------------------------------------------------------------
void PokeListBaseDraw::RebootPokeList( App::PokeList::CONTEXT_PARAM& rContextParam )
{
  GFL_ASSERT( m_pStaticWork );
  GFL_ASSERT( m_pStaticWork->GetProcMode() == MODE_BATTLE );

  // キャンセルボタンの表示切替
  {
    app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();

    bool isCancel = GFL_BOOL_CAST(rContextParam.cancel_flag);//context.battle.cancel_flag;      //!< バトル用のキャンセルフラグをセット

    if( !isCancel )
    {
      btnMan->SetButtonInputEnable( BTN_ID_CANCEL, false );
      btnMan->SetButtonInvisible( BTN_ID_CANCEL );
    }
    else {
      btnMan->SetButtonInputEnable( BTN_ID_CANCEL, true );
      btnMan->SetButtonVisible( BTN_ID_CANCEL );
    }
  }

  // 終了モードの初期化
  m_pStaticWork->SetEndMode( END_MODE_CANCEL );

  // 入力を有効に
  this->SetInputEnabled( true );
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
void PokeListBaseDraw::CreateButtonManager( void )
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
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_BACK_ACTIVE,
      LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_BACK_PASSIVE,
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
    GFL_NEW_LOW_ARRAY(m_pDrawerBase->GetDeviceHeap()) app::ui::ButtonInfoEx[BTN_ID_NUM];
  gfl2::lyt::LytWk* pLytWk = m_pDrawerBase->GetLytWk();

  for( u32 i=0; i<BTN_ID_NUM; ++i )
  {
    gfl2::lyt::LytParts* pParts = m_pDrawerBase->GetParts( c_BtnPicPaneIndex[i][0] );

    info[i] = c_DefInfo[i];
    info[i].picture_pane = m_pDrawerBase->GetPane( c_BtnPicPaneIndex[i][0] );;
    info[i].bound_pane = m_pDrawerBase->GetPane( c_BtnPicPaneIndex[i][1], pParts );
  }

  app::ui::UIView* pUIView = m_pDrawerBase->GetUIView();
  pUIView->CreateButtonManager( m_pHeap, pLytWk, info, BTN_ID_NUM );

  app::tool::ButtonManager* pBtnMan = m_pDrawerBase->GetButtonManager();
  pBtnMan->SetButtonBindKey( BTN_ID_CANCEL, gfl2::ui::BUTTON_B, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );

  // サウンド設定
  if( m_pStaticWork->IsPlaySound() )  // @fix GFNMCat[1363]：サウンドを鳴らすのか判定
  {
    for( u32 i=0; i<BTN_ID_NUM; ++i )
    {
      if( i != BTN_ID_CANCEL )
      {
        pBtnMan->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
      }
      else {
        pBtnMan->SetButtonSelectSE( i, SEQ_SE_CANCEL1 );
      }
    }
  }

  SetupButton( pBtnMan );

  GFL_DELETE_ARRAY info;
}


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )
