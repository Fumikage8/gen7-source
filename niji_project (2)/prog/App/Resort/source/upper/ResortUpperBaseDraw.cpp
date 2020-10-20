//======================================================================
/**
 * @file    ResortUpperBaseDraw.cpp
 * @date    2016/02/10 18:52:51
 * @author  fukushima_yuya
 * @brief   ポケリゾート：上画面Drawのベースクラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "ResortUpperBaseDraw.h"

#include <App/Resort/source/ResortDrawerBase.h>

// arc
#include <niji_conv_header/app/resort/resort_pane.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   生成パラメータ
 */
//------------------------------------------------------------------------------
UpperBaseDraw::UpperBaseDraw( CreateParam& param )
  : app::ui::UIView( param.lytHeap, param.lytHeap->GetSystemAllocator(), param.lytHeap->GetDeviceAllocator() )
  , m_pLytHeap(param.lytHeap)
  , m_pIconHeap(param.iconHeap)
  , m_pAppRenderingManager(param.renderMan)
  , m_pAsyncFileManager(param.fileMan)
  , m_pInitLogic(param.initLogic)
  , m_pResortSave(param.resortSave)
  , m_pBoxPokeSave(param.boxPokeSave)
  , m_pZukanSave(param.zukanSave)
  , m_uiListener(NULL)
  , m_pDrawerBase(NULL)
{
  ;
}



//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool UpperBaseDraw::End( void )
{
  if( !Terminate() ) return false;

  if( m_pDrawerBase )
  {
    app::tool::PokeIconToolEx* pokeIcon = m_pDrawerBase->GetPokeIcon();
    {
      if( pokeIcon )
      {
        if( !pokeIcon->EndFunc() ) return false;
        
        m_pDrawerBase->DeletePokeIcon();
      }
    }
  
    app::tool::ItemIconToolEx* itemIcon = m_pDrawerBase->GetItemIcon();
    {
      if( itemIcon )
      {
        if( !itemIcon->EndFunc() ) return false;
        
        m_pDrawerBase->DeleteItemIcon();
      }
    }
    
    GFL_SAFE_DELETE( m_pDrawerBase );
  }

  Delete2D();
  DeleteGraphicsSystem();
  
  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void UpperBaseDraw::Init( InitParam& param )
{
  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pLytHeap, false );

  // 2D関連作成
  Initialize2D( param );
  //クラスの初期化
  InitializeClass( param );
  // ペインの初期化
  InitializePane();
  // 初期テキスト
  InitializeText();
  // 初期アニメ
  InitializeAnim();
}



//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void UpperBaseDraw::Update( void )
{
  app::ui::UIView::Update();
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void UpperBaseDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* g2d = UIView::GetG2DUtil();
  {
    g2d->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
  }
}

GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)
