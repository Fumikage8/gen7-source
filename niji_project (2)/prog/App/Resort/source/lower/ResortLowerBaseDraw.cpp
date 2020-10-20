//======================================================================
/**
 * @file    ResortLowerBaseDraw.cpp
 * @date    2016/02/08 20:24:53
 * @author  fukushima_yuya
 * @brief   ポケリゾート：下画面Drawのベースクラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "ResortLowerBaseDraw.h"

#include <App/Resort/source/ResortConst.h>
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
LowerBaseDraw::LowerBaseDraw( CreateParam& param )
  : app::ui::UIView( param.lytHeap, param.lytHeap->GetSystemAllocator(), param.lytHeap->GetDeviceAllocator() )
  , m_pLytHeap(param.lytHeap)
  , m_pIconHeap(param.iconHeap)
  , m_pAppRenderingManager(param.renderMan)
  , m_pInitLogic(param.initLogic)
  , m_pResortSave(param.resortSave)
  , m_pBoxPokeSave(param.boxPokeSave)
  , m_pItemSave(param.itemSave)
  , m_uiListener(NULL)
  , m_pDrawerBase(NULL)
  , m_Result(RESULT_NONE)
{
  ;
}



//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool LowerBaseDraw::End( void )
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
void LowerBaseDraw::Init( InitParam& param )
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

  SetInputListener( this );
}



//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void LowerBaseDraw::Update( void )
{
  app::ui::UIView::Update();
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void LowerBaseDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* g2d = UIView::GetG2DUtil();
  {
    g2d->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
  }
}




//------------------------------------------------------------------------------
/**
 * @brief   開く
 */
//------------------------------------------------------------------------------
void LowerBaseDraw::Open( void )
{
  // リザルトの初期化
  InitResult();

  this->SetVisible( true );
  this->SetInputEnabled( true );
}

//------------------------------------------------------------------------------
/**
 * @brief   閉じる
 */
//------------------------------------------------------------------------------
void LowerBaseDraw::Close( void )
{
  this->SetInputEnabled( false );
  this->SetVisible( false );
}


//------------------------------------------------------------------------------
/**
 * @brief   リザルトの初期化
 */
//------------------------------------------------------------------------------
void LowerBaseDraw::InitResult( void )
{
  m_Result = RESULT_CONTINUE;
}





//------------------------------------------------------------------------------
/**
 * @brief   ボタンマネージャの生成
 *
 * @param   defInfo     ボタンインフォのデフォルトテーブル
 * @param   baseTable   ペインテーブル
 * @param   boundTable  ペインテーブル
 * @param   btnNum      ボタンの数
 */
//------------------------------------------------------------------------------
void LowerBaseDraw::CreateButtonManager( const app::ui::ButtonInfoEx* defInfo, const u32* baseTable, const u32* boundTable, const u32 btnNum )
{
  gfl2::heap::HeapBase* heap = m_pDrawerBase->GetDeviceHeap();
  {
    GFL_ASSERT( m_pDrawerBase );
    
    // ボタンマネージャに登録するパラメータの設定
    app::ui::ButtonInfoEx* info = GFL_NEW_LOW_ARRAY(heap) app::ui::ButtonInfoEx[btnNum];
    {
      GFL_ASSERT( info );
      
      for( u32 i=0; i<btnNum; ++i )
      {
        info[i] = defInfo[i];
        {
          gfl2::lyt::LytParts* base = m_pDrawerBase->GetParts( baseTable[i] );
          GFL_ASSERT( base );
          gfl2::lyt::LytPane* bound = m_pDrawerBase->GetPane( boundTable[i], base );
          GFL_ASSERT( bound );
          
          info[i].picture_pane = base;
          info[i].bound_pane   = bound;
        }
      }
    }
    
    // ボタンマネージャの生成
    app::ui::UIView* view = m_pDrawerBase->GetUIView();
    {
      gfl2::lyt::LytWk* lytWk = m_pDrawerBase->GetLytWk();
      {
        view->CreateButtonManager( m_pLytHeap, lytWk, info, btnNum );
      }
    }
    
    GFL_DELETE_ARRAY info;
  }
  
  // ボタンの個別セットアップ
  SetupButton( m_pDrawerBase->GetButtonManager() );
}

//------------------------------------------------------------------------------
/**
 * @brief   ボタンマネージャの生成
 *
 * @param   defInfo     ボタンインフォのデフォルトテーブル
 * @param   baseTable   ペインテーブル
 * @param   partsTable  ペインテーブル
 * @param   boundTable  ペインテーブル
 * @param   btnNum      ボタンの数
 */
//------------------------------------------------------------------------------
void LowerBaseDraw::CreateButtonManager( const app::ui::ButtonInfoEx* defInfo, const u32* baseTable, const u32* partsTable, const u32* boundTable, const u32 btnNum )
{
  gfl2::heap::HeapBase* heap = m_pDrawerBase->GetDeviceHeap();
  {
    GFL_ASSERT( m_pDrawerBase );
    
    // ボタンマネージャに登録するパラメータの設定
    app::ui::ButtonInfoEx* info = GFL_NEW_LOW_ARRAY(heap) app::ui::ButtonInfoEx[btnNum];
    {
      GFL_ASSERT( info );
      
      for( u32 i=0; i<btnNum; ++i )
      {
        info[i] = defInfo[i];
        {
          gfl2::lyt::LytParts* base = m_pDrawerBase->GetParts( baseTable[i] );
          GFL_ASSERT( base );
          
          if( partsTable[i] != LYTRES_PANENAME_RESORT_PANE_END )
          {
            base = m_pDrawerBase->GetParts( partsTable[i], base );
            GFL_ASSERT( base );
          }
          
          gfl2::lyt::LytPane* bound  = m_pDrawerBase->GetPane( boundTable[i], base );
          GFL_ASSERT( bound );
          
          info[i].picture_pane = base;
          info[i].bound_pane   = bound;
        }
      }
    }
    
    // ボタンマネージャの生成
    app::ui::UIView* view = m_pDrawerBase->GetUIView();
    {
      gfl2::lyt::LytWk* lytWk = m_pDrawerBase->GetLytWk();
      {
        view->CreateButtonManager( m_pLytHeap, lytWk, info, btnNum );
      }
    }
    
    GFL_DELETE_ARRAY info;
  }
  
  // ボタンの個別セットアップ
  SetupButton( m_pDrawerBase->GetButtonManager() );
}

GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)
