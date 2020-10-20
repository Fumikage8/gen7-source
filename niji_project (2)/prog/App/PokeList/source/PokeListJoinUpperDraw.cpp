//==============================================================================
/**
 * @file    PokeListJoinUpperDraw.cpp
 * @author  fukushima_yuya
 * @date    2015.05.13
 * @brief   ポケモンリスト参加選択上画面
 */
//==============================================================================

// niji
#include "App/PokeList/source/PokeListJoinUpperDraw.h"
#include "App/PokeList/source/parts/PokeListDrawerBase.h"
#include "App/PokeList/source/parts/PokeListJoinUpperParts.h"
#include "App/PokeList/source/work/PokeListLocalWork.h"
#include "App/PokeList/source/work/PokeListStaticWork.h"

// gflib2
#include <Fade/include/gfl2_FadeManager.h>

// arc
#include <niji_conv_header/app/common_pokeList/common_pokeList.h>
#include <niji_conv_header/app/common_pokelist/common_pokelist_pane.h>
#include <niji_conv_header/app/common_pokelist/common_pokeList_anim_list.h>
#include <niji_conv_header/message/msg_pokelist.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace {
  //! プレートパーツ
  static const u32 c_PlateParts[] = {
    PANENAME_POKELIST_OPP_UPP_000_PANE_PARTSMAINPLT_00,
    PANENAME_POKELIST_OPP_UPP_000_PANE_PARTSMAINPLT_01,
    PANENAME_POKELIST_OPP_UPP_000_PANE_PARTSMAINPLT_02,
  };

  //! 待機アニメーション
  static const u32 c_WaitingAnim[] = {
    LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_00_TEXT_KEEP,
    LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_01_TEXT_KEEP,
    LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_02_TEXT_KEEP,
  };

  static const u32 OPP_UPP_ANIM_NULL = LA_COMMON_POKELIST_POKELIST_OPP_UPP_000___NUM;
}

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
PokeListJoinUpperDraw::PokeListJoinUpperDraw( app::util::Heap* pHeap,app::util::AppRenderingManager* pAppRanderingManager )
  : app::ui::UIView( pHeap, pHeap->GetSystemAllocator(), pHeap->GetDeviceAllocator() )
  , m_pHeap( pHeap )
  , m_pAppRenderingManager( pAppRanderingManager )
  , m_pFadeManager( NULL )
  , m_Seq( SEQ_INIT )
  , m_pDrawerBase( NULL )
  , m_pStaticWork( NULL )
  , m_pLocalWork( NULL )
  , m_pTimeLimit( NULL )
  , m_PartyCount( 0 )
  , m_AlartFlag( false )
{
  for( u8 i=0; i<UPP_JOIN_PARTY_MAX; ++i )
  {
    m_pPokePlate[i] = NULL;
  }

  for( u8 i=0; i<PARTY_NUM; ++i )
  {
    m_PartyID[i] = PARTY_SELF;
    m_WaitFlag[i] = false;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 * @param   pAppParam
 * @param   pAppLytBuff   レイアウトバイナリ
 * @param   pStaticWork   スタティックワーク
 */
//------------------------------------------------------------------------------
void PokeListJoinUpperDraw::Init( APP_PARAM* pAppParam, void* pAppLytBuff, StaticWork* pStaticWork, LocalWork* pLocalWork )
{
  m_pLocalWork = pLocalWork;
  m_pStaticWork = pStaticWork;

  m_pFadeManager = pAppParam->mpFadeManager;

  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  Initialize2D( m_pHeap, pAppLytBuff );
  InitializeText();

  m_Seq = SEQ_READY;
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool PokeListJoinUpperDraw::End( void )
{
  if( m_pDrawerBase )
  {
    app::tool::PokeIconToolEx* pokeIcon = m_pDrawerBase->GetPokeIcon();

    if( pokeIcon && !pokeIcon->EndFunc() ) return false;

    m_pDrawerBase->DeletePokeIcon();
  }

  for( u8 i=0; i<3; ++i )
  {
    GFL_SAFE_DELETE( m_pPokePlate[i] );
  }
  GFL_SAFE_DELETE( m_pTimeLimit );
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
void PokeListJoinUpperDraw::Update( void )
{
  app::ui::UIView::Update();

  switch( m_Seq )
  {
  case SEQ_READY:
    {
      // ポケモンリストを開く
      PokeListOpen();
    } break;

  case SEQ_IN:
    {
      if( !IsEndInOutAnim(true) ) break;

      m_Seq = SEQ_MAIN;
    } break;

  case SEQ_MAIN:
    {
      if( m_pTimeLimit ) m_pTimeLimit->UpdateTime();

      // アラート
      StartAlartAnim();

      // 待機アニメーション
      StartWaitingAnim();
    } break;

  case SEQ_OUT:
    {
      if( !IsEndInOutAnim(false) ) break;

      m_Seq = SEQ_END_READY;
    } break;

  case SEQ_END_READY:
    {
      m_Seq = SEQ_END;
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void PokeListJoinUpperDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* pG2DUtil = m_pDrawerBase->GetG2D();
  pG2DUtil->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
}

//-----------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 */
//-----------------------------------------------------------------------------
bool PokeListJoinUpperDraw::IsExit( void )
{
  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンリスト画面のオープン
 */
//------------------------------------------------------------------------------
void PokeListJoinUpperDraw::PokeListOpen( void )
{
  //this->SetVisible( true );

  //m_pDrawerBase->StartAnim( c_JOIN_UPP_IN_ANIM );

  m_Seq = SEQ_IN;
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンリスト画面のクローズ
 */
//------------------------------------------------------------------------------
void PokeListJoinUpperDraw::PokeListClose( void )
{
  //m_pDrawerBase->StartAnim( c_JOIN_UPP_OUT_ANIM );

  //this->SetInputEnabled( false );

  m_Seq = SEQ_OUT;
}

//------------------------------------------------------------------------------
/**
 * @brief   イン・アウトアニメが終了したかどうか
 * @param   in    インアニメかどうか
 * @return  true = 終了、false = 継続
 */
//------------------------------------------------------------------------------
bool PokeListJoinUpperDraw::IsEndInOutAnim( bool in )
{
  //u32 anmID = c_JOIN_UPP_IN_ANIM;
  //if( !in ) anmID = c_JOIN_UPP_OUT_ANIM;

  //return m_pDrawerBase->IsAnimEnd( anmID );
  return m_pFadeManager->IsEnd( gfl2::Fade::DISP_UPPER );
}

//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 * @param   pHeap         ヒープ
 * @param   pAppLytBuff   レイアウトバイナリデータ
 */
//------------------------------------------------------------------------------
void PokeListJoinUpperDraw::Initialize2D( app::util::Heap* pHeap, void* pAppLytBuff )
{
  // アーカイブIDテーブル
  const app::util::G2DUtil::LytResData resTbl[] = {
    { pAppLytBuff, 2, static_cast<u32>(app::util::G2DUtil::ATTACH_ROOT) }
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    { // 上
      0,
      LYTRES_COMMON_POKELIST_POKELIST_OPP_UPP_000_BFLYT,
      LA_COMMON_POKELIST_POKELIST_OPP_UPP_000___NUM,
      pAppLytBuff,
      true,
      app::util::G2DUtil::SETUP_UPPER,
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

  //this->SetVisible( false );
  this->SetInputEnabled( false ); //!< 入力を無効に

  // パーティの表示順の設定
  SetPartyOrder();

  {
    gfl2::heap::HeapBase* pDevHeap = m_pHeap->GetDeviceHeap();

    // 共通描画処理
    m_pDrawerBase = GFL_NEW(pDevHeap) DrawerBase( this, m_pHeap, 0, m_pStaticWork, m_pLocalWork );
    {
      GFL_ASSERT( m_pDrawerBase );

      m_pDrawerBase->StartAnim( LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__BG_00_BG_LOOP_00 );
    }

    // ポケモンアイコンの生成
    app::tool::PokeIconToolEx::PokeIconToolParam poke_icon_param;
    {
      poke_icon_param.pUIView = this;
      poke_icon_param.pHeap   = m_pHeap;
      poke_icon_param.iconMax = POKE_ICON_NUM * 3;
      poke_icon_param.sync    = true;
    }
    m_pDrawerBase->CreatePokeIcon( poke_icon_param );

    // 制限時間
    m_pTimeLimit = GFL_NEW(pDevHeap) JoinTimeLimit( m_pDrawerBase );
    GFL_ASSERT( m_pTimeLimit );

    // プレートの生成
    CreatePartyPlate();
  }

  // 待機中
  for( u32 i=0; i<UPP_JOIN_PARTY_MAX; ++i )
  {
    gfl2::lyt::LytParts* parts = m_pDrawerBase->GetParts( c_PlateParts[i] );
    gfl2::lyt::LytPane* null = m_pDrawerBase->GetPane( PANENAME_POKELIST_PLT_LOW_001_PANE_NULL_WAITING, parts );

    m_pDrawerBase->SetAlpha( null, 0 );
    m_pDrawerBase->SetVisiblePane( null, true );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト表示
 */
//------------------------------------------------------------------------------
void PokeListJoinUpperDraw::InitializeText( void )
{
  // 待機中
  for( u32 i=0; i<UPP_JOIN_PARTY_MAX; ++i )
  {
    gfl2::lyt::LytParts* parts = m_pDrawerBase->GetParts( c_PlateParts[i] );
    gfl2::lyt::LytTextBox* text = m_pDrawerBase->GetTextBox( PANENAME_POKELIST_PLT_LOW_001_PANE_TEXTBOX_01, parts );

    this->SetTextboxPaneMessage( text, mes_pokelist_13_09 );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   パーティの表示順の設定
 */
//------------------------------------------------------------------------------
void PokeListJoinUpperDraw::SetPartyOrder( void )
{
  static const PartyID c_SingleOrder[] = {
    PARTY_SELF,     //!< 自分
    PARTY_ENEMY0,   //!< 相手
  };
  static const PartyID c_OtherOrder[] = {
    PARTY_FRIEND,   //!< 味方
    PARTY_ENEMY0,   //!< 相手１
    PARTY_ENEMY1,   //!< 相手２
  };

  // レギュレーションで見せ合いOFFならば、表示なし
  if( m_pStaticWork->GetRegulationShowPoke() == false )
  {
    m_PartyCount = 0;
    return;
  }

  // パーティの表示順を設定
  ModeContext context = m_pStaticWork->GetModeContext();
  JoinUppMode uppMode = static_cast<JoinUppMode>(context.join.join_upp_mode);
  const PartyID* partyTbl = NULL;

  switch( uppMode )
  {
  case JOIN_UPP_MODE_TAG:
  case JOIN_UPP_MODE_ROYALE:
    {
      m_PartyCount = GFL_NELEMS(c_OtherOrder);
      partyTbl = c_OtherOrder;
    } break;

  case JOIN_UPP_MODE_SINGLE:
    {
      m_PartyCount = GFL_NELEMS(c_SingleOrder);
      partyTbl = c_SingleOrder;
    } break;

  default:
    {
      m_PartyCount = 0;
    }
  };

  for( u8 i=0; i<m_PartyCount; ++i )
  {
    m_PartyID[i] = static_cast<u8>( partyTbl[i] );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   パーティの表示プレートの生成
 */
//------------------------------------------------------------------------------
void PokeListJoinUpperDraw::CreatePartyPlate( void )
{
  static const JoinPokePlate::PlateColor c_SinglePlateColor[] = {
    JoinPokePlate::COL_BLUE,
    JoinPokePlate::COL_RED,
  };
  static const JoinPokePlate::PlateColor c_TagPlateColor[] = {
    JoinPokePlate::COL_BLUE,
    JoinPokePlate::COL_RED,
    JoinPokePlate::COL_RED,
  };
  static const JoinPokePlate::PlateColor c_RoyalPlateColor[] = {
    JoinPokePlate::COL_RED,
    JoinPokePlate::COL_RED,
    JoinPokePlate::COL_RED,
  };

  gfl2::heap::HeapBase* pDevHeap = m_pHeap->GetDeviceHeap();
  ModeContext context = m_pStaticWork->GetModeContext();
  JoinUppMode uppMode = static_cast<JoinUppMode>(context.join.join_upp_mode);
  const JoinPokePlate::PlateColor* plateCol = NULL;
  u32 animID = OPP_UPP_ANIM_NULL;

  switch( uppMode )
  {
  case JOIN_UPP_MODE_TAG:
    {
      plateCol = c_TagPlateColor;
      animID = LA_COMMON_POKELIST_POKELIST_OPP_UPP_000_POS_02;
    } break;

  case JOIN_UPP_MODE_ROYALE:
    {
      plateCol = c_RoyalPlateColor;
      animID = LA_COMMON_POKELIST_POKELIST_OPP_UPP_000_POS_02;
    } break;
    
  case JOIN_UPP_MODE_SINGLE:
    {
      plateCol = c_SinglePlateColor;
      animID = LA_COMMON_POKELIST_POKELIST_OPP_UPP_000_POS_01;
    } break;

  default:
    {
      gfl2::lyt::LytPane* plate = m_pDrawerBase->GetPane( c_PlateParts[0] );
      m_pDrawerBase->SetVisiblePane( plate, false );
    }
  }

  if( plateCol != NULL )    // @fix cov_ctr[10101]
  {
    for( u8 i=0; i<m_PartyCount; ++i )
    {
      PartyID partyID = static_cast<PartyID>(m_PartyID[i]);
      IntermediateData* partyData = m_pStaticWork->GetPokeParty( partyID );

      m_pPokePlate[i] = GFL_NEW(pDevHeap) JoinPokePlate( m_pDrawerBase, partyID, partyData, i, uppMode );
      GFL_ASSERT( m_pPokePlate[i] );

      m_pPokePlate[i]->SetPlateColor( plateCol[i] );
    }
  }

  if( animID != OPP_UPP_ANIM_NULL )
  {
    m_pDrawerBase->StartAnim( animID );   //!< プレート位置のアニメ
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   待機アニメーションの再生
 */
//------------------------------------------------------------------------------
void PokeListJoinUpperDraw::StartWaitingAnim( void )
{
  ModeContext context = m_pStaticWork->GetModeContext();

  for( u8 i=0; i<m_PartyCount; ++i )
  {
    u8 id = m_PartyID[i];
    u8 wait = context.join.selected_wait[id];

    if( wait == JOIN_SELECT_WAIT_NONE ) continue;
    if( m_WaitFlag[id] ) continue;

    if( m_WaitFlag[id] != wait )
    {
      m_pDrawerBase->StartAnim( c_WaitingAnim[i] );
      m_WaitFlag[id] = true;
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   制限時間アラーとの表示
 */
//------------------------------------------------------------------------------
void PokeListJoinUpperDraw::StartAlartAnim( void )
{
  if( m_pStaticWork->GetTimeLimitClass() == NULL ) return;

  if( m_pStaticWork->GetTimeLimit() <= 10 )
  {
    if( !m_AlartFlag )
    {
      m_pDrawerBase->StartAnim( LA_COMMON_POKELIST_POKELIST_OPP_UPP_000_ALART );

      m_AlartFlag = true;
    }
  }
}


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )
