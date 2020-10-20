// ============================================================================
/*
 * @file LowerBGView.cpp
 * @brief バトルツリーアプリの下画面を表示するクラスです。
 * @date 2015.11.30
 * @author yuto_uchida
 */
// ============================================================================
#include "App/BattleTree/source/View/BattleTreeLowerBGView.h"

#include "App/BattleTree/source/BattleTreeResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

//  nijiのインクルード
#include "AppLib/include/util/app_util_GetPaneHelper.h"

//  リソースのインクルード
#include <arc_index/message.gaix>
#include <niji_conv_header/app/battletree/res2d/battletreeLyt.h>
#include <niji_conv_header/app/battletree/res2d/battletreeLyt_anim_list.h>
#include <niji_conv_header/app/battletree/res2d/battletreeLyt_pane.h>

//  メッセージのインクルード
#include <niji_conv_header/message/msg_battletrainer_select.h>
#include <niji_conv_header/sound/SoundMiddleID.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)
GFL_NAMESPACE_BEGIN(View)

  LowerBGView::LowerBGView( App::BattleTree::System::BattleTreeWork* pWork ) :
    NetAppLib::System::NetApplicationViewBase(
      pWork,
      App::BattleTree::BATTLE_TREE_RESOURCE_ID_LAYOUT,
      LYTRES_BATTLETREELYT_BATTLETREE_MIN_LOW_000_BFLYT,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_000___NUM,
      app::util::G2DUtil::SETUP_LOWER,
      pWork->GetMessageUtility(),
      GARC_message_battletrainer_select_DAT )
  {
    _Clear();

    GFL_ASSERT( pWork );
    m_pWork = pWork;

    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

    gfl2::lyt::LytMultiResID& rResourceID = pG2DUtil->GetLayoutResourceID( 0 );
    gfl2::lyt::LytWk* pLayoutWork = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID );
    GFL_ASSERT( pLayoutWork );

    app::util::GetPaneHelper  helper( pLayoutWork, &rResourceID );

    //  BGのループアニメ
    {
      pG2DUtil->StartAnime( LAYOUT_WORK_ID, LA_BATTLETREELYT_BATTLETREE_MIN_LOW_000__BG_00_BG_LOOP_00 );
    }
  }

  LowerBGView::~LowerBGView()
  {
  }

  void LowerBGView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    app::util::AppRenderingManager* pAppRenderingManager = m_pWork->GetAppRenderingManager();
    pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID );
  }

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END(App)
