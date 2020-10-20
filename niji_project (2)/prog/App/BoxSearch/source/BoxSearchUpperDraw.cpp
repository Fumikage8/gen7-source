//======================================================================
/**
 * @file    BoxSearchUpperDraw.cpp
 * @date    2015/07/27 12:59:16
 * @author  fukushima_yuya
 * @brief   ボックス検索：上画面
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "BoxSearchUpperDraw.h"

#include "App/BoxSearch/source/BoxSearchConst.h"
#include "App/BoxSearch/source/BoxSearchDrawerBase.h"
#include "App/BoxSearch/source/BoxSearchDrawListener.h"
#include "App/BoxSearch/source/parts/BoxSearchUpperMessage.h"

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <Print/include/WordSet.h>
#include <Print/include/SystemFont.h>
#include <sound/include/Sound.h>

// gflib2
#include <Fade/include/gfl2_FadeManager.h>
#include <str/include/gfl2_Str.h>

// arc
#include <niji_conv_header/app/box/res2d/bgrupp.h>  //!< 背景
#include <niji_conv_header/app/box/res2d/bgrupp_pane.h>
#include <niji_conv_header/app/box/res2d/bgrupp_anim_list.h>
#include <niji_conv_header/app/box/res2d/srcupp.h>  //!< 検索上画面
#include <niji_conv_header/app/box/res2d/srcupp_pane.h>
#include <niji_conv_header/app/box/res2d/srcupp_anim_list.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( BoxSearch )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace
{
  enum Sequence
  {
    SEQ_INIT,
    SEQ_MAIN,
    SEQ_END
  };
}


//------------------------------------------------------------------------------
/**
 * @brief	  コンストラクタ
 */
//------------------------------------------------------------------------------
BoxSearchUpperDraw::BoxSearchUpperDraw( app::util::Heap* pHeap, app::util::AppRenderingManager* pAppRenderingManager )
  : app::ui::UIView( pHeap, pHeap->GetDeviceAllocator(), pHeap->GetDeviceAllocator() )
  , m_pHeap( pHeap )
  , m_pAppRenderingManager( pAppRenderingManager )
  , m_pAppParam( NULL )
  , m_pDrawerBase(NULL)
  , m_pUpperMsg( NULL )
  , m_Seq( SEQ_INIT )
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void BoxSearchUpperDraw::Init( InitParam& param )
{
  {
    m_pAppParam = param.appParam;
  }

  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  // 2D関連作成
  Initialize2D( param );
  // クラスの初期化
  InitializeClass( param );

  m_Seq = SEQ_MAIN;
}

//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void BoxSearchUpperDraw::Initialize2D( InitParam& param )
{
  const app::util::G2DUtil::LytResData resTbl[] = {
    { param.lytBuff, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_SRCUPP_BOX_SRC_UPP_000_BFLYT,
      LA_SRCUPP_BOX_SRC_UPP_000___NUM,
      param.lytBuff,
      true,
      app::util::G2DUtil::SETUP_UPPER,
      true
    },
  };

  // 2Dの生成
  Create2D(
    m_pHeap,
    NULL,
    1,
    resTbl,
    GFL_NELEMS(resTbl),
    setupData,
    GFL_NELEMS(setupData),
    param.msgData_Main,
    param.wordSet );

  this->SetInputEnabled( false );
}

//------------------------------------------------------------------------------
/**
 * @brief   クラスの初期化
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void BoxSearchUpperDraw::InitializeClass( InitParam& param )
{
  gfl2::heap::HeapBase* heap = m_pHeap->GetDeviceHeap();
  {
    // 描画共通処理
    m_pDrawerBase = GFL_NEW(heap) DrawerBase( this, m_pHeap, 0, param.msgData_Main, param.wordSet );
    {
      GFL_ASSERT( m_pDrawerBase );
    }

    // メッセージ
    m_pUpperMsg = GFL_NEW(heap) UpperMsg( m_pDrawerBase, param.msgData_Tokusei );
    {
      GFL_ASSERT( m_pUpperMsg );

      // 初期化
      m_pUpperMsg->Initialize();
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool BoxSearchUpperDraw::End( void )
{
  GFL_SAFE_DELETE( m_pUpperMsg );
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
void BoxSearchUpperDraw::Update( void )
{
  app::ui::UIView::Update();
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void BoxSearchUpperDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* g2d = UIView::GetG2DUtil();
  {
    g2d->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
  }
}





//==============================================================================
/**
 * @brief   上画面メッセージの設定
 */
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief   検索項目リスト用
 */
//------------------------------------------------------------------------------
void BoxSearchUpperDraw::SetMessage_ItemList( void )
{
  m_pUpperMsg->SetMessage_ItemList();
}

//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定(頭文字選択用)
 *
 * @param   listID    リストの種類
 */
//------------------------------------------------------------------------------
void BoxSearchUpperDraw::SetMessage_Initial( u32 listID )
{
  m_pUpperMsg->SetMessage_Initial( listID );
}

//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定(ガイド用)
 *
 * @param   listID    リストの種類
 */
//------------------------------------------------------------------------------
void BoxSearchUpperDraw::SetMessage_Guide( u32 listID )
{
  switch( listID )
  {
  case SRCID_POKE_NAME:
    m_pUpperMsg->SetMessage_PokeName();
    break;
  case SRCID_WAZA_NAME:
    m_pUpperMsg->SetMessage_WazaName();
    break;
  case SRCID_WAZAMACHINE:
    m_pUpperMsg->SetMessage_WazaMachine();
    break;
  case SRCID_GENDER:
    m_pUpperMsg->SetMessage_Gender();
    break;
  case SRCID_ITEM:
    m_pUpperMsg->SetMessage_Item();
    break;
  case SRCID_MARK:
    m_pUpperMsg->SetMessage_BoxMark();
    break;
  //case SRCID_ENTRY:  //!< 仕様削除20160222
  //  m_pUpperMsg->SetMessage_TeamEntry();
  //  break;
  case SRCID_TEAM:
    m_pUpperMsg->SetMessage_TeamName();
    break;
  default:
    GFL_ASSERT(0);
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定(タイプ用)
 *
 * @param   listID  リストの種類
 * @param   type1   タイプ１
 * @param   type2   タイプ２
 */
//------------------------------------------------------------------------------
void BoxSearchUpperDraw::SetMessage_Type( u32 listID, u8 type1, u8 type2 )
{
  m_pUpperMsg->SetMessage_Type( listID, type1, type2 );
}

//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定(特性用)
 *
 * @param   tokusei   特性No
 */
//------------------------------------------------------------------------------
void BoxSearchUpperDraw::SetMessage_Tokusei( TokuseiNo tokusei )
{
  m_pUpperMsg->SetMessage_Tokusei( tokusei );
}

//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定(性格用)
 *
 * @param   seikaku   性格ID
 */
//------------------------------------------------------------------------------
void BoxSearchUpperDraw::SetMessage_Seikaku( pml::pokepara::Seikaku seikaku )
{
  m_pUpperMsg->SetMessage_Seikaku( seikaku );
}



GFL_NAMESPACE_END( BoxSearch )
GFL_NAMESPACE_END( App )
