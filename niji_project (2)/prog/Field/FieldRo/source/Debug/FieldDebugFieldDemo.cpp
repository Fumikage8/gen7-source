//======================================================================
/**
 * @file FieldDebugFieldDemo.cpp
 * @date 2015/11/12 14:55:22
 * @author saita_kazuki
 * @brief デバッグ：FieldDemo フィールドデモイベント
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#include "Field/FieldRo/include/Debug/FieldDebugFieldDemo.h"

// gfl2
#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include <Fade/include/gfl2_FadeTypes.h>
#include <Fade/include/gfl2_FadeManager.h>
// GameSys
#include "GameSys/include/GameManager.h"
// field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/Event/FieldEventFieldDemo.h"

// conv_header
#include <niji_conv_header/message/debug/msg_debug_menu.h>

// arc
#include <arc_index/demo_seq_script.gaix>

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)


//------------------------------------------------------------------------------
// STATIC
//------------------------------------------------------------------------------

// アイテムの操作値更新共通処理
void ButtonControl_S32( s32* pValue, gfl2::ui::Button* pButton, s32 min, s32 max, s32 step)
{
  if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
  {
    step *= 10;
  }

  if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    (*pValue) += step;
    if( (*pValue) >= max)
    {
      (*pValue) -= max;
    }
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    (*pValue) -= step;
    if( (*pValue) < min)
    {
      (*pValue) += max;
    }
  }
}

// デモ開始アイテム更新
void UpdateFunc_DemoStart( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugFieldDemo* pThis = (DebugFieldDemo*)userWork;
  gfl2::ui::Button* pButton = gfl2::debug::DebugWin_GetUiButton();

  ButtonControl_S32( &pThis->m_demoID, pButton, 0, GARC_demo_seq_script_DATA_NUM, 1 );

  // デモイベント実行
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );

    EventFieldDemo* p_event;
    p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldDemo > ( pGameManager->GetGameEventManager( ) );

    EventFieldDemo::Description desc;
    desc.pFieldmap        = pThis->m_pFieldmap;
    desc.demoID           = pThis->m_demoID;
    desc.option           = pThis->m_option;
    desc.unUseOption      = pThis->m_unUseOption;
    desc.renderUseOption  = pThis->m_renderUseOption;
    desc.isForceOneStep   = pThis->m_isForceOneStep;
    desc.textureIndex     = pThis->m_textureIndex;
    desc.pTextureNode     = pThis->m_pTextureNode;

    p_event->Initialize( desc);
  }

}

// デモ開始アイテム描画
wchar_t* DrawFunc_DemoStart( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugFieldDemo* pThis = (DebugFieldDemo*)userWork;
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %d ]", item->GetName(), pThis->m_demoID );

  return workStr;
}

// デモ実行オプション追加更新
void UpdateFunc_AddOption( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugFieldDemo* pThis = (DebugFieldDemo*)userWork;
  gfl2::ui::Button* pButton = gfl2::debug::DebugWin_GetUiButton();

  ButtonControl_S32( &pThis->m_controlOption, pButton, 0, 1, 1 );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pThis->m_option |= (0x01 << pThis->m_controlOption);
  }
}

// デモ実行オプション削除更新
void UpdateFunc_DelOption( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugFieldDemo* pThis = (DebugFieldDemo*)userWork;
  gfl2::ui::Button* pButton = gfl2::debug::DebugWin_GetUiButton();

  ButtonControl_S32( &pThis->m_controlOption, pButton, 0, 1, 1 );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pThis->m_option &= (~(0x01 << pThis->m_controlOption));
  }
}

// デモ実行オプションアイテム描画
wchar_t* DrawFunc_Option( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugFieldDemo* pThis = (DebugFieldDemo*)userWork;
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  const wchar_t* words[] =
  {
    L"USE_FIELD_DRAW_CAPTURE",
  };

  bool isON = false;
  if( (pThis->m_option & (0x01 << pThis->m_controlOption)) )
  {
    isON = true;
  }

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %ls : %d ]", item->GetName(), words[ pThis->m_controlOption ], isON );

  return workStr;
}

// デモシステム未使用オプション追加アイテム更新
void UpdateFunc_AddUnUseOption( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugFieldDemo* pThis = (DebugFieldDemo*)userWork;
  gfl2::ui::Button* pButton = gfl2::debug::DebugWin_GetUiButton();

  ButtonControl_S32( &pThis->m_controlUnUseOption, pButton, 0, 3, 1 );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pThis->m_unUseOption |= (0x01 << pThis->m_controlUnUseOption);
  }
}

// デモシステム未使用オプション削除アイテム更新
void UpdateFunc_DelUnUseOption( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugFieldDemo* pThis = (DebugFieldDemo*)userWork;
  gfl2::ui::Button* pButton = gfl2::debug::DebugWin_GetUiButton();

  ButtonControl_S32( &pThis->m_controlUnUseOption, pButton, 0, 3, 1 );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pThis->m_unUseOption &= (~(0x01 << pThis->m_controlUnUseOption));
  }
}

// デモ実行オプションアイテム描画
wchar_t* DrawFunc_UnUseOption( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugFieldDemo* pThis = (DebugFieldDemo*)userWork;
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  const wchar_t* words[] =
  {
    L"UN_USE_CHARA_MODEL",
    L"UN_USE_TEXT_WINDOW",
    L"UN_USE_POKEMON",
  };

  bool isON = false;
  if( (pThis->m_unUseOption & (0x01 << pThis->m_controlUnUseOption)) )
  {
    isON = true;
  }

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %ls : %d ]", item->GetName(), words[ pThis->m_controlUnUseOption ], isON );

  return workStr;
}

// デモ描画使用オプションアイテム追加更新
void UpdateFunc_AddRenderOption( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugFieldDemo* pThis = (DebugFieldDemo*)userWork;
  gfl2::ui::Button* pButton = gfl2::debug::DebugWin_GetUiButton();

  ButtonControl_S32( &pThis->m_controlRenderUseOption, pButton, 0, 7, 1 );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    switch( pThis->m_controlRenderUseOption)
    {
    case 0:
      pThis->m_renderUseOption.isUseModelRenderPathAfterEdge = true;
      break;
    case 1:
      pThis->m_renderUseOption.isUseSkyRenderPath = true;
      break;
    case 2:
      pThis->m_renderUseOption.isUseEdgeMapPath = true;
      break;
    case 3:
      pThis->m_renderUseOption.isUseModelOutLinePath = true;
      break;
    case 4:
      pThis->m_renderUseOption.isUseLayoutRenderPath = true;
      break;
    case 5:
      pThis->m_renderUseOption.isUseScreenEffectRenderPath = true;
      break;
    case 6:
      pThis->m_renderUseOption.isUseBloomRenderPath = true;
      break;
    }
  }
}

// デモ描画使用オプションアイテム削除更新
void UpdateFunc_DelRenderOption( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugFieldDemo* pThis = (DebugFieldDemo*)userWork;
  gfl2::ui::Button* pButton = gfl2::debug::DebugWin_GetUiButton();

  ButtonControl_S32( &pThis->m_controlRenderUseOption, pButton, 0, 7, 1 );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    switch( pThis->m_controlRenderUseOption)
    {
    case 0:
      pThis->m_renderUseOption.isUseModelRenderPathAfterEdge = false;
      break;
    case 1:
      pThis->m_renderUseOption.isUseSkyRenderPath = false;
      break;
    case 2:
      pThis->m_renderUseOption.isUseEdgeMapPath = false;
      break;
    case 3:
      pThis->m_renderUseOption.isUseModelOutLinePath = false;
      break;
    case 4:
      pThis->m_renderUseOption.isUseLayoutRenderPath = false;
      break;
    case 5:
      pThis->m_renderUseOption.isUseScreenEffectRenderPath = false;
      break;
    case 6:
      pThis->m_renderUseOption.isUseBloomRenderPath = false;
      break;
    }
  }
}

// デモ描画使用オプションアイテム描画
wchar_t* DrawFunc_RenderOption( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugFieldDemo* pThis = (DebugFieldDemo*)userWork;
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  const wchar_t* words[] =
  {
    L"UseModelRenderPathAfterEdge",
    L"UseSkyRenderPath",
    L"UseEdgeMapPath",
    L"UseModelOutLinePath",
    L"UseLayoutRenderPath",
    L"UseScreenEffectRenderPath",
    L"UseBloomRenderPath",
  };

  bool isEnable = false;
  switch( pThis->m_controlRenderUseOption)
  {
  case 0:
    isEnable = pThis->m_renderUseOption.isUseModelRenderPathAfterEdge;
    break;
  case 1:
    isEnable = pThis->m_renderUseOption.isUseSkyRenderPath;
    break;
  case 2:
    isEnable = pThis->m_renderUseOption.isUseEdgeMapPath;
    break;
  case 3:
    isEnable = pThis->m_renderUseOption.isUseModelOutLinePath;
    break;
  case 4:
    isEnable = pThis->m_renderUseOption.isUseLayoutRenderPath;
    break;
  case 5:
    isEnable = pThis->m_renderUseOption.isUseScreenEffectRenderPath;
    break;
  case 6:
    isEnable = pThis->m_renderUseOption.isUseBloomRenderPath;
    break;
  }

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %ls : %d ]", item->GetName(), words[ pThis->m_controlRenderUseOption ], isEnable );

  return workStr;
}

// フェードイン
void UpdateFunc_FadeIn( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugFieldDemo* pThis = (DebugFieldDemo*)userWork;
  gfl2::ui::Button* pButton = gfl2::debug::DebugWin_GetUiButton();

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA );
  }
}

// フェードアウト
void UpdateFunc_FadeOut( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugFieldDemo* pThis = (DebugFieldDemo*)userWork;
  gfl2::ui::Button* pButton = gfl2::debug::DebugWin_GetUiButton();

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    gfl2::math::Vector4 start_col( 0, 0, 0, 0.0f);
    gfl2::math::Vector4 end_col( 0, 0, 0, 255.0f);
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager)->RequestOut( gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, &start_col, &end_col);
  }
}

// リセット(初期状態に戻す)
void UpdateFunc_Reset( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugFieldDemo* pThis = (DebugFieldDemo*)userWork;
  gfl2::ui::Button* pButton = gfl2::debug::DebugWin_GetUiButton();

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pThis->m_demoID = 0;
    pThis->m_option = 0;
    pThis->m_unUseOption = 0;
    pThis->m_renderUseOption = DemoLib::Sequence::SequenceRenderingPipeLine::UseOption();
    pThis->m_isForceOneStep = false;
    pThis->m_textureIndex = 0;
    pThis->m_pTextureNode = NULL;
  }
}

/**
 *  @brief  Group ctor
 */
static void CreateDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugFieldDemo *pDebug = reinterpret_cast<DebugFieldDemo*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    // 実行オプションフラグ追加
    // デモシステム未使用オプションフラグ追加
    // デモ描画使用オプション追加
    // 実行オプションフラグ削除
    // デモシステム未使用オプションフラグ削除
    // デモ描画使用オプション削除
    // 強制1フレームモード
    // フェードイン
    // フェードアウト
    // 初期状態に戻す
    // デモイベント開始

    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pDebug->m_pHeap, pDebug->m_pMsgData, msg_dmenu_field_field_demo_option_add,        pDebug, UpdateFunc_AddOption,       DrawFunc_Option );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pDebug->m_pHeap, pDebug->m_pMsgData, msg_dmenu_field_field_demo_unuse_option_add,  pDebug, UpdateFunc_AddUnUseOption,  DrawFunc_UnUseOption );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pDebug->m_pHeap, pDebug->m_pMsgData, msg_dmenu_field_field_demo_render_option_add, pDebug, UpdateFunc_AddRenderOption, DrawFunc_RenderOption );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pDebug->m_pHeap, pDebug->m_pMsgData, msg_dmenu_field_field_demo_option_del,        pDebug, UpdateFunc_DelOption,       DrawFunc_Option );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pDebug->m_pHeap, pDebug->m_pMsgData, msg_dmenu_field_field_demo_unuse_option_del,  pDebug, UpdateFunc_DelUnUseOption,  DrawFunc_UnUseOption );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pDebug->m_pHeap, pDebug->m_pMsgData, msg_dmenu_field_field_demo_render_option_del, pDebug, UpdateFunc_DelRenderOption, DrawFunc_RenderOption );
    gfl2::debug::DebugWin_AddItemValueBool(  pGroup, pDebug->m_pHeap, pDebug->m_pMsgData, msg_dmenu_field_field_demo_force_1frame,      &pDebug->m_isForceOneStep );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pDebug->m_pHeap, pDebug->m_pMsgData, msg_dmenu_field_field_demo_fade_in,           pDebug, UpdateFunc_FadeIn,          NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pDebug->m_pHeap, pDebug->m_pMsgData, msg_dmenu_field_field_demo_fade_out,          pDebug, UpdateFunc_FadeOut,         NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pDebug->m_pHeap, pDebug->m_pMsgData, msg_dmenu_field_field_demo_reset,             pDebug, UpdateFunc_Reset,           NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pDebug->m_pHeap, pDebug->m_pMsgData, msg_dmenu_field_field_demo_start,             pDebug, UpdateFunc_DemoStart,       DrawFunc_DemoStart );
  }
}

/**
 *  @brief  Group dtor
 */
static void DestroyDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pGroup )
  {
    pGroup->RemoveAllItem();
  }
}

//------------------------------------------------------------------------------
// DebugFieldDemo
//------------------------------------------------------------------------------

/**
 *  @brief  コンストラクタ
 */
DebugFieldDemo::DebugFieldDemo( void )
  : m_pHeap( NULL)
  , m_pFieldmap( NULL)
  , m_demoID( 0)
  , m_option( 0)
  , m_unUseOption( 0)
  , m_renderUseOption()
  , m_isForceOneStep( false)
  , m_textureIndex( 0)
  , m_pTextureNode( NULL)
{
}

/**
 *  @brief  デストラクタ
 */
DebugFieldDemo::~DebugFieldDemo( void )
{
  Terminate();
}

//------------------------------------------------------------------------------
// Public Functions
//------------------------------------------------------------------------------

/**
 *  @brief  初期化
 */
void DebugFieldDemo::Initialize(
  gfl2::heap::HeapBase* pHeap,
  gfl2::str::MsgData* pMsgData,
  Fieldmap* pFieldmap
  )
{
  m_pHeap = pHeap;
  m_pFieldmap = pFieldmap;
  m_pMsgData = pMsgData;
}

/**
 *  @brief  破棄
 */
void DebugFieldDemo::Terminate( void )
{
}

/**
 *  @brief  メニューの作成
 */
void DebugFieldDemo::CreateDebugMenu( gfl2::debug::DebugWinGroup *root )
{
  gfl2::debug::DebugWinGroup *pGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_field_field_demo_title, root );
  pGroup->SetCreateDestroyGroupFunc( this, CreateDebugGroupFunc, DestroyDebugGroupFunc );
}

//------------------------------------------------------------------------------
// PRIVATE
//------------------------------------------------------------------------------




GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // PM_DEBUG

