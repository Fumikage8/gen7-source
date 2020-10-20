//======================================================================
/**
 * @file TimerIconView.cpp
 * @date 2015/10/13 13:33:17
 * @author uchida_yuto
 * @brief タイマーアイコンView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "AppLib/include/Tool/TimerIconView.h"

//  nijiのインクルード
#include "AppLib/include/Tool/app_tool_TimeIcon.h"
#include "System/include/GflUse.h"

//  timerレイアウトのインクルード
#include "niji_conv_header/app/timer/res2d/timerIcon.h"
#include "niji_conv_header/app/timer/res2d/timerIcon_anim_list.h"
#include "niji_conv_header/app/timer/res2d/timerIcon_pane.h"
#include <arc_index/timerIcon.gaix>

// garc
#include "System/include/ArcIdGetter.h"
#include "System/include/DressUp.h"
#include <arc_def_index/arc_def.h>

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  //  タイマーアイコンのリソースのarcidとdataidを取得
  u32 TimerIconUIView::GetArcId()
  {
    return  NIJI_ARCID_NUM( ARCID_TIMER_DATA );
  }

  u32 TimerIconUIView::GetArcDataId()
  {
    return  GARC_timerIcon_timerIcon_applyt_COMP;
  }

  bool TimerIconUIView::IsArcDataComp()
  {
    return  true;
  }

  TimerIconUIView::TimerIconUIView( 
  app::util::Heap * pHeap, 
  app::util::AppRenderingManager* pRenderingManager, 
  void* pLytBuff, 
  app::util::G2DUtil::SETUP_DISPLAY_TYPE displayType ) :
  UIView( pHeap, pHeap->GetDeviceAllocator(), pHeap->GetDeviceAllocator() )
  {
    _Clear();

    GFL_ASSERT( pHeap );
    GFL_ASSERT( pLytBuff );
    GFL_ASSERT( pRenderingManager );

    m_pRenderingManager = pRenderingManager;

    //  2D画面作成
    {
      // アーカイブIDテーブル
      const ::app::util::G2DUtil::LytResData resTbl[] =
      {
        {  pLytBuff, 8, ::app::util::G2DUtil::ATTACH_NONE, },
      };

      //  作成するレイアウトワーク
      const ::app::util::G2DUtil::LytwkData setupData[ 1 ] =
      {
          //  選択画面
          {
            0,
            LYTRES_TIMERICON_TIMER_ICN_000_BFLYT,
            LA_TIMERICON_TIMER_ICN_000___NUM,
            pLytBuff,
            true,
            displayType,
            true
          },
      };

      UIView::Create2D( pHeap, NULL, 8, resTbl, GFL_NELEMS( resTbl ), setupData, GFL_NELEMS( setupData ), UIView::UNUSE_MESSAGE_DATA );
      UIView::SetVisible( false );
    }

    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    gfl2::lyt::LytWk* pLayout = pG2DUtil->GetLayoutWork( 0 );
    m_pIconPane = pLayout->GetPane( PANENAME_TIMER_ICN_000_PANE_ALL );
    GFL_ASSERT( m_pIconPane );

    m_pRootPane = m_pIconPane->GetParent();
    GFL_ASSERT( m_pRootPane );

    m_pCenterLPane  = pLayout->GetPane( PANENAME_TIMER_ICN_000_PANE_NULL_CENTER_L );
    GFL_ASSERT( m_pCenterLPane );

    m_pCenterSPane  = pLayout->GetPane( PANENAME_TIMER_ICN_000_PANE_NULL_CENTER_S );
    GFL_ASSERT( m_pCenterSPane );

    {
      pLayout->BindAnime( LA_TIMERICON_TIMER_ICN_000_IN );
      m_fadeInAnimeState.data.fadeAnime.maxFrame  = pLayout->GetAnimeMaxFrame( LA_TIMERICON_TIMER_ICN_000_IN );
      pLayout->UnBindAnime( LA_TIMERICON_TIMER_ICN_000_IN );

      pLayout->BindAnime( LA_TIMERICON_TIMER_ICN_000_OUT);
      m_fadeOutAnimeState.data.fadeAnime.maxFrame  = pLayout->GetAnimeMaxFrame( LA_TIMERICON_TIMER_ICN_000_OUT );
      pLayout->UnBindAnime( LA_TIMERICON_TIMER_ICN_000_OUT );

      pLayout->BindAnime( LA_TIMERICON_TIMER_ICN_000_KEEP );
      m_keepAnimeState.data.keepAnime.maxFrame  = pLayout->GetAnimeMaxFrame( LA_TIMERICON_TIMER_ICN_000_KEEP );

      m_keepAnimeState.data.keepAnime.CenterLRotateMaxZ = 360;
      m_keepAnimeState.data.keepAnime.CenterSRotateMaxZ = 720;

      pLayout->UnBindAnime( LA_TIMERICON_TIMER_ICN_000_KEEP );
    }
  }

  TimerIconUIView::~TimerIconUIView()
  {
    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  更新処理
  *         子ウインドウについても自動的にコールする(子→親の順）
  */
  //--------------------------------------------------------------------------------------------
  void TimerIconUIView::Update()
  {
    //  フェードインアニメ
    {
      switch( m_fadeInAnimeState.state )
      {
        case 1:
        {
          u8 alpha  = 0;
          if( m_fadeInAnimeState.data.fadeAnime.nowFrame != 0 )
          {
            alpha = (u8)( (f32)m_fadeInAnimeState.data.fadeAnime.nowFrame / (f32)m_fadeInAnimeState.data.fadeAnime.maxFrame * 255.0f );
          }

          app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
          pG2DUtil->SetPaneAlpha( m_pIconPane, alpha );

          ++m_fadeInAnimeState.data.fadeAnime.nowFrame;
          if( 255 <= alpha )
          {
            m_fadeInAnimeState.state  = 0;
          }

          break;
        }
      }
    }

    //  キープアニメ
    {
      switch( m_keepAnimeState.state )
      {
        case 1:
        {
          gfl2::math::VEC3  centerLRotate, centerSRotate;
          centerSRotate = m_pCenterSPane->GetRotate();
          centerLRotate = m_pCenterLPane->GetRotate();

          centerLRotate.z = 0.0f;
          centerSRotate.z = 0.0f;

          if( m_keepAnimeState.data.keepAnime.nowFrame != 0 )
          {
            f32 rate  = (f32)m_keepAnimeState.data.keepAnime.nowFrame / (f32)m_keepAnimeState.data.keepAnime.maxFrame;
            centerLRotate.z = -rate * m_keepAnimeState.data.keepAnime.CenterLRotateMaxZ;
            centerSRotate.z = -rate * m_keepAnimeState.data.keepAnime.CenterSRotateMaxZ;
          }

          m_pCenterSPane->SetRotate( centerLRotate );
          m_pCenterLPane->SetRotate( centerSRotate );

          ++m_keepAnimeState.data.keepAnime.nowFrame;
          if( m_keepAnimeState.data.keepAnime.maxFrame <= m_keepAnimeState.data.keepAnime.nowFrame )
          {
            //  ループする
            m_keepAnimeState.data.keepAnime.nowFrame  = 0;
          }          
        }
      }
    }

    //  フェードアウトアニメ
    {
      switch( m_fadeOutAnimeState.state )
      {
      case 1:
        {
          u8 alpha = 0;
          if( m_fadeOutAnimeState.data.fadeAnime.nowFrame != 0 )
          {
            alpha = (u8)( (f32)m_fadeOutAnimeState.data.fadeAnime.nowFrame / (f32)m_fadeOutAnimeState.data.fadeAnime.maxFrame * 255.0f );
          }

          app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
          pG2DUtil->SetPaneAlpha( m_pIconPane, 255 - alpha );

          ++m_fadeOutAnimeState.data.fadeAnime.nowFrame;
          if( 255 <= alpha )
          {
            m_fadeOutAnimeState.state  = 0;
          }

          break;
        }
      }
    }

    UIView::Update();
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  描画処理
  *         子ウインドウについても自動的にコールする(親→子の順）
  */
  //--------------------------------------------------------------------------------------------
  void TimerIconUIView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    pG2DUtil->AddDrawableLytWkForOneFrame( m_pRenderingManager, displayNo, 0, m_layoutOrder, m_layoutNo );
  }

  //  レイアウトパラメータ設定
  void TimerIconUIView::SetLayoutParam( const u32 layoutOrder, const u32 layoutNo )
  {
    m_layoutOrder = layoutOrder; 
    m_layoutNo    = layoutNo;    
  }

  void TimerIconUIView::StartAnime()
  {
    //  終了のEndAnime()を読んでないと連続で呼べない
    if( m_bVisible == false )
    {
//      app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    //  pG2DUtil->StopAnime( 0, LA_TIMERICON_TIMER_ICN_000_OUT );
   //   pG2DUtil->StartAnime( 0, LA_TIMERICON_TIMER_ICN_000_IN );

      //  はりの位置をランダムにする
     // gfl2::lyt::LytWk* pLayout = pG2DUtil->GetLayoutWork( 0 );
    //  GFL_ASSERT( pLayout );
   //   App::Tool::TimeIcon::StartAnime( pLayout, LA_TIMERICON_TIMER_ICN_000_KEEP );

      //  最初に始めるフレーム数をランダムで決める（app_tool_TimeIcon.cppのロジックをコピペ）
      {
        static const u32 ANIME_START_PATTERN = 17;  
        u32	rnd = System::GflUse::GetPublicRand( ANIME_START_PATTERN );
        m_keepAnimeState.data.keepAnime.nowFrame  = m_keepAnimeState.data.keepAnime.maxFrame / ANIME_START_PATTERN * rnd;
      }

      m_fadeOutAnimeState.state  = 0;

      m_fadeInAnimeState.state  = 1;
      m_fadeInAnimeState.data.fadeAnime.nowFrame  = 0;

      m_keepAnimeState.state  = 1;

      m_bVisible = true;

      UIView::SetVisible( m_bVisible );
    }
  }

  void TimerIconUIView::EndAnime()
  {
    m_bVisible    = false;

    m_fadeInAnimeState.state  = 0;

    m_fadeOutAnimeState.state  = 1;
    m_fadeOutAnimeState.data.fadeAnime.nowFrame  = 0;

//    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
//    pG2DUtil->StopAnime( 0, LA_TIMERICON_TIMER_ICN_000_IN );
//    pG2DUtil->StartAnime( 0, LA_TIMERICON_TIMER_ICN_000_OUT );
  }

  void TimerIconUIView::SetIconPos( gfl2::math::VEC3& rPos )
  {
    //  行列設定を無効に
    m_pIconPane->ResetMtx();
    m_pIconPane->SetTranslate( rPos );
  }

  //  STR行列を設定
  void TimerIconUIView::SetSRTMtx( const gfl2::math::MTX34& rMat )
  {
    m_SRTMat  = rMat;
    m_pIconPane->SetMtx( &m_SRTMat );
  }

  //  指定したペインをルートとしてアタッチする
  void TimerIconUIView::AttachPane( gfl2::lyt::LytPane* pAttachPane )
  {
    if( pAttachPane == NULL )
    {
      GFL_ASSERT( 0 );
      return;
    }

    m_pRootPane->RemoveChild( m_pIconPane );

    pAttachPane->AppendChild( m_pIconPane );
  }

  void TimerIconUIView::RemovePane()
  {
    gfl2::lyt::LytPane* pRootPane = m_pIconPane->GetParent();
    if( pRootPane == NULL )
    {
      return;
    }
    else
    {    
      pRootPane->RemoveChild( m_pIconPane );
    }

    m_pRootPane->AppendChild( m_pIconPane );
  }


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)
