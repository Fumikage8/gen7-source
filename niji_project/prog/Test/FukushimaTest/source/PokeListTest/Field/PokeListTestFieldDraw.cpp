#if PM_DEBUG

//==============================================================================
/**
 * @file    PokeListTestFieldDraw.cpp
 * @author  fukushima_yuya
 * @date    2015.06.22
 * @brief   フィールド用ポケモンリスト
 */
//==============================================================================

// niji
#include "Test/FukushimaTest/source/PokeListTest/Field/PokeListTestFieldDraw.h"
#include "Test/FukushimaTest/source/PokeListTest/Parts/PokeListTestPlateDraw.h"

#include <App/PokeList/source/PokeListDrawListener.h>
#include <App/PokeList/source/parts/PokeListDrawerBase.h>
#include <App/PokeList/source/parts/PokeListPlateDraw.h>
#include <App/PokeList/source/work/PokeListLocalWork.h>
#include <App/PokeList/source/work/PokeListStaticWork.h>
#include <Print/include/SystemFont.h>

// arc
#include <niji_conv_header/app/common_pokeList/common_pokeList.h>
#include <niji_conv_header/app/common_pokelist/common_pokelist_pane.h>
#include <niji_conv_header/app/common_pokelist/common_pokeList_anim_list.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )

//------------------------------------------------------------------------------
/**
 * @brief	  コンストラクタ
 */
//------------------------------------------------------------------------------
TestPokeListFieldDraw::TestPokeListFieldDraw( app::util::Heap * heap,app::util::AppRenderingManager* pAppRanderingManager )
  : TestPokeListBaseDraw( heap, pAppRanderingManager )
  , m_animIndex( 0 )
  , m_prevAnimIndex( 0 )
{
  for( u32 i=0; i<App::PokeList::PLATE_NUM; ++i )
  {
    m_pPokePlate[i] = NULL;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
TestPokeListFieldDraw::~TestPokeListFieldDraw( void )
{
  ;
}



//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool TestPokeListFieldDraw::EndFunc( void )
{
  if( m_pDrawerBase )
  {
    app::tool::PokeIconToolEx* pokeIcon = m_pDrawerBase->GetPokeIcon();

    if( pokeIcon && !pokeIcon->EndFunc() ) return false;

    m_pDrawerBase->DeletePokeIcon();
  }

  for( u32 i=0; i<App::PokeList::PLATE_NUM; ++i )
  {
    GFL_SAFE_DELETE( m_pPokePlate[i] );
  }

  print::SystemFont_ResetNumFontInfo(); //@fix GFNMCat[1083]
  print::SystemFont_DeleteNumFont();

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新処理
 */
//------------------------------------------------------------------------------
void TestPokeListFieldDraw::UpdateFunc( void )
{
  for( u32 i=0; i<App::PokeList::PLATE_NUM; ++i )
  {
    if( m_pPokePlate[i] ) m_pPokePlate[i]->Update();
  }

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

      // 入力の有効化
      this->SetInputEnabled( true );

      m_Seq = SEQ_MAIN;
    } break;

  case SEQ_OUT:
    {
      //if( !IsEndInOutAnim(false) ) break;

      m_Seq = SEQ_END_READY;
    } break;

  case SEQ_END_READY:
    {
      if( m_uiListener ) m_uiListener->PokeListEnd();    //!< ポケモンリストの終了を通知

      m_Seq = SEQ_END;
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   描画処理
 */
//------------------------------------------------------------------------------
void TestPokeListFieldDraw::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* pG2D = m_pDrawerBase->GetG2D();

  pG2D->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
}

//------------------------------------------------------------------------------
/**
 * @brief   UIのセットアップ
 */
//------------------------------------------------------------------------------
void TestPokeListFieldDraw::SetupUI( void )
{
  gfl2::heap::HeapBase* devHeap = m_pDrawerBase->GetDeviceHeap();
  u8 memberCount = m_pStaticWork->GetPokeParty( App::PokeList::PARTY_SELF )->GetMemberCount();

    // ポケモンアイコンの生成
    app::tool::PokeIconToolEx::PokeIconToolParam poke_icon_param;
    {
      poke_icon_param.pUIView = this;
      poke_icon_param.pHeap   = m_pHeap;
      poke_icon_param.iconMax = App::PokeList::POKE_ICON_NUM;
      poke_icon_param.sync    = true;
    }
    m_pDrawerBase->CreatePokeIcon( poke_icon_param );

  // プレートの生成
  for( u8 i=0; i<memberCount; ++i )
  {
    m_pPokePlate[i] = GFL_NEW(devHeap) TestPokePlate( m_pDrawerBase, App::PokeList::CPOS_PLATE_TL );
    m_pPokePlate[i]->Setup( App::PokeList::PLATE_MODE_PARAM );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ボタンのセットアップ
 */
//------------------------------------------------------------------------------
void TestPokeListFieldDraw::SetupButton( app::tool::ButtonManager* pBtnMan )
{
  if( pBtnMan )
  {
    //pBtnMan->SetButtonBindKey( BTN_ID_X, gfl2::ui::BUTTON_X, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
    //pBtnMan->SetButtonBindKey( BTN_ID_Y, gfl2::ui::BUTTON_Y, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );

    pBtnMan->SetButtonInputEnable( App::PokeList::BTN_ID_DECIDE, false );
  }
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
void TestPokeListFieldDraw::OnPreLoadResource( void )
{
  // num_font.bffntの登録
  print::SystemFont_CreateNumFont( m_pHeap->GetDeviceHeap(), m_pHeap->GetDeviceHeap() );
  print::SystemFont_SetLytSystemNumFont();
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインアクション通知用リスナ
 * @param   paneId
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult TestPokeListFieldDraw::OnLayoutPaneEvent( const u32 paneId )
{
  if( m_uiListener == NULL ) return DISABLE_ACCESS;

  if( paneId == App::PokeList::BTN_ID_CANCEL )
  {
    PokeListClose();
  }

  return ENABLE_ACCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief   キー入力通知用リスナ
 * @param   pKey
 * @param   pStick
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult TestPokeListFieldDraw::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( m_uiListener == NULL ) return DISABLE_ACCESS;

  //static const u32 c_AnimIndex[7] = {
  //  LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00__PARTS_POKEICON_JAMP_HIGH,      //!< 元気な時
  //  LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00__PARTS_POKEICON_JAMP_MIDDLE,    //!< 体力が半分以下
  //  LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00__PARTS_POKEICON_JAMP_LOW,       //!< 体力が少ない
  //  LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00__PARTS_POKEICON_DIE,            //!< 瀕死時
  //  LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00__PARTS_POKEICON_SICK,           //!< 異常状態時
  //  LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00__PARTS_POKEICON_JAMP_SELECT,    //!< 選択時
  //  LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00__PARTS_POKEICON_STOP,           //!< 色戻し用
  //};
  //static const char* c_AnimName[] = {
  //  "ANIM_JAMP_HIGH",
  //  "ANIM_JAMP_MIDDLE",
  //  "ANIM_JAMP_LOW",
  //  "ANIM_DIE",
  //  "ANIM_SICK",
  //  "ANIM_JAMP_SELECT",
  //  "ANIM_STOP",
  //};

  //if( pButton->IsTrigger( gfl2::ui::BUTTON_UP ) )
  //{
  //  if( m_animIndex == 0 ) m_animIndex = GFL_NELEMS(c_AnimIndex) - 1;
  //  else m_animIndex--;

  //  GFL_PRINT( "SelectAnim[%s]\n", c_AnimName[m_animIndex] );

  //}
  //else if( pButton->IsTrigger( gfl2::ui::BUTTON_DOWN ) )
  //{
  //  m_animIndex = (m_animIndex + 1) % GFL_NELEMS(c_AnimIndex);

  //  GFL_PRINT( "SelectAnim[%s]\n", c_AnimName[m_animIndex] );
  //}
  //else if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  //{
  //  m_pDrawerBase->StopAnim( c_AnimIndex[m_prevAnimIndex] );
  //  m_pDrawerBase->StartAnim( c_AnimIndex[m_animIndex] );

  //  GFL_PRINT( "StopAnim : Clear[false]\n" );
  //  GFL_PRINT( "StopAnim[%s] -> PlayAnim[%s]\n", c_AnimName[m_prevAnimIndex], c_AnimName[m_animIndex] );

  //  m_prevAnimIndex = m_animIndex;
  //}
  //
  //if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) )
  //{
  //  m_pDrawerBase->StartAnim( LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00__PARTS_POKEICON_JAMP_SELECT );
  //}
  //else if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) )
  //{
  //  m_pDrawerBase->StopAnim( LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00__PARTS_POKEICON_JAMP_SELECT );
  //}
  //else if( pButton->IsTrigger( gfl2::ui::BUTTON_START ) )
  //{
  //  gfl2::lyt::LytWk* lytWk = this->GetLayoutWork( 0 );
  //  bool play = lytWk->IsBindAnime( LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00__PARTS_POKEICON_JAMP_SELECT );

  //  static const char* c_DebugStr[] = {
  //    "FALSE",
  //    "TRUE",
  //  };
  //  GFL_PRINT( "JAMP_SELECT = %s\n", c_DebugStr[play] );
  //}

  //app::tool::PokeIconTool* pPokeIcon = m_pDrawerBase->GetPokeIcon();

  //if( pButton->IsTrigger( gfl2::ui::BUTTON_L ) )
  //{
  //  m_animIndex = 3;

  //  m_pPokeStatus->SetupPokeDisplay( m_pStaticWork->GetPoke( CPOS_PLATE_TR ) );

  //  m_pDrawerBase->StopAnim( c_AnimIndex[m_prevAnimIndex] );
  //  m_pDrawerBase->StartAnim( c_AnimIndex[m_animIndex] );

  //  GFL_PRINT( "StopAnim : Clear[false]\n" );
  //  GFL_PRINT( "StopAnim[%s] -> PlayAnim[%s]\n", c_AnimName[m_prevAnimIndex], c_AnimName[m_animIndex] );

  //  m_prevAnimIndex = m_animIndex;
  //}
  //else if( pButton->IsTrigger( gfl2::ui::BUTTON_R ) )
  //{
  //  m_animIndex = 0;

  //  m_pPokeStatus->SetupPokeDisplay( m_pStaticWork->GetPoke( CPOS_PLATE_TL ) );

  //  m_pDrawerBase->StopAnim( c_AnimIndex[m_prevAnimIndex] );
  //  m_pDrawerBase->StartAnim( c_AnimIndex[m_animIndex] );

  //  GFL_PRINT( "StopAnim : Clear[false]\n" );
  //  GFL_PRINT( "StopAnim[%s] -> PlayAnim[%s]\n", c_AnimName[m_prevAnimIndex], c_AnimName[m_animIndex] );

  //  m_prevAnimIndex = m_animIndex;
  //}

  return ENABLE_ACCESS;
}



GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

#endif // PM_DEBUG