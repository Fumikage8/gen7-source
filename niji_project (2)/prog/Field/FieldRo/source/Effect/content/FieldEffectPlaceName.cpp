//======================================================================
/**
 * @file FieldEffectPlaceName.cpp
 * @date 2015/09/02 12:12:48
 * @author miyachi_soichi
 * @brief 地名表示
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// self
#include "Field/FieldRo/include/Effect/content/FieldEffectPlaceName.h"
#include "Field/FieldRo/include/Window/FieldWindowManager.h"
// arc
#include <niji_conv_header/field/effect/placename.h>
#include <niji_conv_header/field/effect/placename_anim_list.h>
#include <niji_conv_header/field/effect/placename_pane.h>
#include <arc_index/message.gaix>

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

static const s32 EFFECT_LAYOUT_HEAP_SIZE    = 0x3000;
static const s32 EFFECT_LAYOUT_DEVHEAP_SIZE = 0xC000;

/**
 *  @brief  コンストラクタ
 */
EffectPlaceName::EffectPlaceName( void )
: IEffectBase()
, m_appUtilHeap()
, m_pLayout( NULL )
, m_pWindowManager( NULL )
, m_pWordSet( NULL )
, m_pStrBuff( NULL )
{
}

/**
 *  @brief  デストラクタ
 */
EffectPlaceName::~EffectPlaceName( void )
{
}

/**
 *  @brief  初期化
 */
void EffectPlaceName::Initialize( SetupData &setupData )
{
  m_pWindowManager = setupData.pWinMgr;
  // ローカルヒープ作成
  m_appUtilHeap.LocalHeapCreate( setupData.pHeap, setupData.pHeap, EFFECT_LAYOUT_HEAP_SIZE, EFFECT_LAYOUT_DEVHEAP_SIZE );
  // テンポラリ
  m_pStrBuff = GFL_NEW( setupData.pHeap ) gfl2::str::StrBuf( 64, setupData.pHeap );
  m_pStrBuff_follow = GFL_NEW( setupData.pHeap ) gfl2::str::StrBuf( 64, setupData.pHeap );
  // レイアウト生成
  m_pWordSet = GFL_NEW( setupData.pHeap ) print::WordSet( setupData.pHeap );
  m_pWordSet->SetWordSetLoader( setupData.pWordSetLoader );
  gfl2::str::MsgData *pMsgData = setupData.pWordSetLoader->GetMsgData( GARC_message_place_name_DAT );
  app::util::AppRenderingManager *pRender = m_pWindowManager->GetRenderingManager();
  m_pLayout = GFL_NEW( setupData.pHeap ) Field::Effect::EffectLayout( &m_appUtilHeap, pRender );
  // レイアウト初期化
  {
    app::util::G2DUtil::LytResData resTbl[] = 
    {
      { setupData.pResource, 2, app::util::G2DUtil::ATTACH_ROOT },
    };
    app::util::G2DUtil::LytwkData dataTbl[] =
    {
      {
        0,
        LYTRES_PLACENAME_PLACE_NAME_BFLYT,
        LA_PLACENAME_PLACE_NAME___NUM,
        setupData.pResource,
        false,
        app::util::G2DUtil::SETUP_UPPER,
        true
      },
    };
    Field::Effect::EffectLayout::SetupParam param;
    param.layoutNum = 1;
    param.lytResTbl = resTbl;
    param.resTblNum = GFL_NELEMS( resTbl );
    param.lytDataTbl = dataTbl;
    param.dataTblNum = GFL_NELEMS( dataTbl );
    m_pLayout->Initialize( param, pMsgData, m_pWordSet );
  }
  // 登録
  m_pWindowManager->GetUIView()->AddSubView( m_pLayout );
  m_pLayout->SetVisible( false );

  m_eState = STATE_NONE;
}

/**
 *  @brief  破棄
 */
bool EffectPlaceName::Terminate( void )
{
  Delete(); // 大野さん依頼：UIView切り離し呼出
  GFL_SAFE_DELETE( m_pLayout );
  GFL_SAFE_DELETE( m_pWordSet );
  GFL_SAFE_DELETE( m_pStrBuff );
  GFL_SAFE_DELETE( m_pStrBuff_follow );
  m_appUtilHeap.Delete();
  return true;
}

/**
 *  @brief  更新
 */
void EffectPlaceName::updateNormal( void )
{
  app::util::G2DUtil *pUtil = m_pLayout->GetG2DUtil();
  if( !pUtil ){ return; }

  switch( m_eState )
  {
  case STATE_IN:
    if( pUtil->IsAnimeEnd( 0, LA_PLACENAME_PLACE_NAME_CUTIN_00 ) )
    {
      m_eState = STATE_OUT;
      pUtil->StartAnime( 0, LA_PLACENAME_PLACE_NAME_EXIT_00 );
    }
    break;
  case STATE_OUT:
    if( pUtil->IsAnimeEnd( 0, LA_PLACENAME_PLACE_NAME_EXIT_00 ) )
    {
      m_eState = STATE_NONE;
      m_pLayout->SetVisible( false );
    }
    break;
  }
}

/**
 *  @brief  終了しているか.
 */
bool EffectPlaceName::IsAnimationLastFrame( void )
{
  return true;
}

/**
 *  @brief  各エフェクトで必要な破棄作業
 */
void EffectPlaceName::Delete( void )
{
  // 登録は親からするが解除は子からする
  m_pLayout->RemoveFromSuperView();
}

/**
 *  @brief  開始
 */
void EffectPlaceName::Start( u32 strID, u32 old_strID, bool bForce )
{
  u32 strID_follow = strID + 1;
  u32 old_strID_follow = old_strID + 1;

  bool bExec = true;
  if( !bForce )
  {
    // 同一エリア名での移動は表示しない
    if( strID == old_strID &&
        strID_follow == old_strID_follow ){ bExec = false; }
  }

  // 表示しなくていい
  if( !bExec ){ return; }
  app::util::G2DUtil *pUtil = m_pLayout->GetG2DUtil();
  if( !pUtil ){ return; }
  gfl2::lyt::LytWk *pWork = pUtil->GetLayoutWork( 0 );
  if( !pWork ){ return; }
  gfl2::lyt::LytTextBox *pBox;

  // single or double
  gfl2::str::MsgData *pMsgData = pUtil->GetMsgData();
  pMsgData->GetString( strID_follow, *m_pStrBuff_follow );
  pMsgData->GetString( strID, *m_pStrBuff );
  if( m_pStrBuff_follow->GetLength() == 0 )
  {
    // 1列表示
    pBox = pWork->GetTextBoxPane( PANENAME_PLACE_NAME_PANE_TEXT_02 );
    pUtil->SetTextBoxPaneStringExpand( pBox, strID );
    pUtil->SetPaneVisible( 0, PANENAME_PLACE_NAME_PANE_TEXT_1_LINE, true );
    pUtil->SetPaneVisible( 0, PANENAME_PLACE_NAME_PANE_TEXT_2_LINE, false );
    pUtil->StartAnime( 0, LA_PLACENAME_PLACE_NAME_CUTIN_00 );
    m_pLayout->SetVisible( true );
  }
  else
  {
    // 2列表示
    pBox = pWork->GetTextBoxPane( PANENAME_PLACE_NAME_PANE_TEXT_01 );
    pUtil->SetTextBoxPaneStringExpand( pBox, strID_follow );
    pBox = pWork->GetTextBoxPane( PANENAME_PLACE_NAME_PANE_TEXT_00 );
    pUtil->SetTextBoxPaneStringExpand( pBox, strID );
    pUtil->SetPaneVisible( 0, PANENAME_PLACE_NAME_PANE_TEXT_1_LINE, false );
    pUtil->SetPaneVisible( 0, PANENAME_PLACE_NAME_PANE_TEXT_2_LINE, true );
    pUtil->StartAnime( 0, LA_PLACENAME_PLACE_NAME_CUTIN_00 );
    m_pLayout->SetVisible( true );
  }

  m_eState = STATE_IN;
}

/**
 *  @brief  終了
 */
void EffectPlaceName::End( void )
{
  m_pLayout->SetVisible( false );
  m_eState = STATE_NONE;
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)
