//======================================================================
/**
 * @file    RotomFade.cpp
 * @date    2016/03/17 17:29:57
 * @author  ichiraku_katsuhiko
 * @brief   ロトムフェード (RotomFadeのコピー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "NetApp/JoinFestaSubScreen/source/View/RotomFade.h"

#include <AppLib/include/Ui/UIView.h>
#include <sound/include/Sound.h>
#include <Fade/include/gfl2_FadeManager.h>

// arc
#include "arc_def_index/arc_def.h"
#include <arc_index/rotom_fade.gaix>
#include <niji_conv_header/app/rotom_fade/rotom_fade.h>
#include <niji_conv_header/app/rotom_fade/rotom_fade_pane.h>
#include <niji_conv_header/app/rotom_fade/rotom_fade_anim_list.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(View)

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
RotomFade::RotomFade( app::util::Heap* heap, app::util::AppRenderingManager* renderMan, void* pAppLytBuff )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_pHeap( heap )
  , m_pAppRenderingManager( renderMan )
  , m_seq( 0 ) 
{
  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  Initialize2D( pAppLytBuff );
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
RotomFade::~RotomFade( void )
{
  Delete2D();
  DeleteGraphicsSystem();
}

//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 */
//------------------------------------------------------------------------------
void RotomFade::Initialize2D( void* pAppLytBuff )
{
  gfl2::heap::HeapBase* pHeap = m_pHeap->GetDeviceHeap();

  // アーカイブIDテーブル
  const app::util::G2DUtil::LytResData resTbl[] = {
    { pAppLytBuff, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_ROTOM_FADE_ROTOM_FADE_UPP_000_BFLYT,
      LA_ROTOM_FADE_ROTOM_FADE_UPP_000___NUM,
      pAppLytBuff,
      true,
      app::util::G2DUtil::SETUP_UPPER,
      true
    },
    {
      0,
      LYTRES_ROTOM_FADE_ROTOM_FADE_LOW_000_BFLYT,
      LA_ROTOM_FADE_ROTOM_FADE_LOW_000___NUM,
      pAppLytBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
  };


  // 2Dの生成
  Create2D(
    m_pHeap,
    NULL,
    GFL_NELEMS(setupData),
    resTbl,
    GFL_NELEMS(resTbl),
    setupData,
    GFL_NELEMS(setupData),
    this->UNUSE_MESSAGE_DATA );

  //this->SetVisible( false );
  this->SetVisible( true );
  this->SetInputEnabled( false );
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void RotomFade::Update( void )
{
  app::ui::UIView::Update();

  switch( m_seq ){
  case SEQ_IDLE:
    break;
  case SEQ_FADE_IN:
    {
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, 1 );
      m_seq = SEQ_FADE_WAIT;
    }
    break;
  case SEQ_FADE_WAIT:
    {
      if( !GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd( gfl2::Fade::DISP_DOUBLE ) )
      {
        break;
      }
      m_seq = SEQ_ROTOM_IN_ANIME_START;
    }
    /* break through */
  case SEQ_ROTOM_IN_ANIME_START:
    {
      StartRotomInAnime();
      m_seq = SEQ_ROTOM_IN_ANIME_WAIT;
    }
    break;
  case SEQ_ROTOM_IN_ANIME_WAIT:
    {
      if( InEndRotomInAnime() )
      {
        m_seq = SEQ_END;
      }
    }
    break;
  case SEQ_END:
    break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void RotomFade::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  pG2DUtil->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, LAYOUT_WORK_ID_UPPER, 130 );
  pG2DUtil->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, LAYOUT_WORK_ID_LOWER, 130 );
}

//------------------------------------------------------------------------------
/**
 * @brief   ロトムフェードイン開始
 */
//------------------------------------------------------------------------------
void RotomFade::StartRotomFade( void )
{
  GFL_ASSERT( m_seq==SEQ_IDLE );
  m_seq = SEQ_FADE_IN;
}

//------------------------------------------------------------------------------
/**
 * @brief   ロトムフェードイン終了したか
 *
 * @return  "true  = 終了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool RotomFade::IsEndRotomFade( void )
{
  return ( m_seq == SEQ_END );
}


//------------------------------------------------------------------------------
/**
 * @brief   アニメの開始
 */
//------------------------------------------------------------------------------
void RotomFade::StartRotomInAnime( void )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  // 上画面
  pG2DUtil->StartAnime( LAYOUT_WORK_ID_UPPER, LA_ROTOM_FADE_ROTOM_FADE_UPP_000_FADE_00 );
  // 下画面
  pG2DUtil->StartAnime( LAYOUT_WORK_ID_LOWER, LA_ROTOM_FADE_ROTOM_FADE_LOW_000_FADE_00 );

  Sound::PlaySE( SEQ_SE_SYS_NJ_028 );
  Sound::PlaySE( SEQ_SE_SYS_NJ_029 );

  //this->SetVisible( true );
}

//------------------------------------------------------------------------------
/**
 * @brief   アニメの終了
 *
 * @return  "true  = 終了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool RotomFade::InEndRotomInAnime( void )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  bool isEnd = true;

  if( !pG2DUtil->IsAnimeEnd( LAYOUT_WORK_ID_UPPER, LA_ROTOM_FADE_ROTOM_FADE_UPP_000_FADE_00 ) )
  {
    isEnd = false;
  }
  if( !pG2DUtil->IsAnimeEnd( LAYOUT_WORK_ID_LOWER, LA_ROTOM_FADE_ROTOM_FADE_UPP_000_FADE_00 ) )
  {
    isEnd = false;
  }

  if( isEnd == true )
  {
    //this->SetVisible( false );
  }
  return isEnd;
}

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)
