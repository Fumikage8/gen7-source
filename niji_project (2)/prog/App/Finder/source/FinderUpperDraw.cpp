//==============================================================================
/**
 * @file    FinderUpperDraw.cpp
 * @author  fukushima_yuya
 * @date    2015.05.26
 * @brief   ポケファインダー上画面
 */
//==============================================================================

// niji
#include "FinderUpperDraw.h"
#include "FinderDrawListener.h"
#include "FinderDrawerBase.h"

#include <Savedata/include/PokeFinderSave.h>
#include <Sound/include/Sound.h>
#include <system/include/GflUse.h>

// arc
#include <niji_conv_header/app/finder/finder.h>
#include <niji_conv_header/app/finder/finder_pane.h>
#include <niji_conv_header/app/finder/finder_anim_list.h>
#include <niji_conv_header/message/msg_pokefinder_camera.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Finder )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace {
  enum Sequence
  {
    SEQ_INIT,
    SEQ_READY,
    SEQ_IN,
    SEQ_MAIN,
    SEQ_OUT,
    SEQ_END
  };

  enum {
    ROTOM_SEQ_NONE = 0,
    ROTOM_SEQ_BEGIN,
    ROTOM_SEQ_KEEP,
    ROTOM_SEQ_OUT,
    ROTOM_SEQ_RESTART,
    ROTOM_SEQ_NUM,
  };

  enum {
    SIGHT_NORMAL,
    SIGHT_CROSS,
    SIGHT_NONE,
    SIGHT_NUM,
  };

  enum {
    SHUTTER_ANIM_NONE,
    SHUTTER_ANIM_CLOSE,
    SHUTTER_ANIM_OPEN,
    SHUTTER_ANIM_NUM,
  };

  // カメラズームアニメ  // @fix GFNMCat[1196]
  static const u32 c_ZoomAnim[] = {
    LA_FINDER_FINDER_MIN_UPP_000__ZOOMBAR_ZOOMANIM_PLUS_01,
    LA_FINDER_FINDER_MIN_UPP_000__ZOOMBAR_ZOOMANIM_PLUS_01,
    LA_FINDER_FINDER_MIN_UPP_000__ZOOMBAR_ZOOMANIM_PLUS_00,
    LA_FINDER_FINDER_MIN_UPP_000__ZOOMBAR_ZOOMANIM_PLUS,
    LA_FINDER_FINDER_MIN_UPP_000__ZOOMBAR_ZOOMANIM_PLUS,
  };

  static const u32 MIN_UPP_ANIM_NULL = LA_FINDER_FINDER_MIN_UPP_000___NUM;
}

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
FinderUpperDraw::FinderUpperDraw( app::util::Heap* pHeap,app::util::AppRenderingManager* pAppRanderingManager, Savedata::PokeFinderSave* finderSave )
  : app::ui::UIView( pHeap, pHeap->GetDeviceAllocator(), pHeap->GetDeviceAllocator() )
  , m_pHeap( pHeap )
  , m_uiListener( NULL )
  , m_pAppRenderingManager( pAppRanderingManager )
  , m_pFinderSave(finderSave)
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_pAppParam( NULL )
  , m_Seq( SEQ_INIT )
  , m_pDrawerBase( NULL )
  , m_Version(0)
  , m_SightID( SIGHT_NORMAL )
  , m_ShutterCount( 0 )
  , m_ShutterAnimSeq( SHUTTER_ANIM_NONE )
  , m_RotomSeq(ROTOM_SEQ_NONE)
  , m_RotomAnim(MIN_UPP_ANIM_NULL)
  , m_RotomFrame(0)
  , m_pInfoVisible(NULL)
  , m_pRotomComment(NULL)
{
  for( u32 i=0; i<SHUTTER_COUNT_MAX; ++i )
  {
    m_pShutterCntPane[i] = NULL;
  }
}

//------------------------------------------------------------------------------
/**
* @brief   このビュー専用のUIリスナー設定
* @param   pListener   UIリスナー
*/
//------------------------------------------------------------------------------
void FinderUpperDraw::SetUIListener( FinderDrawListener* pListener )
{
  m_uiListener = pListener;
}

//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 * @param   pAppParam
 * @param   pAppLytBuff   レイアウトバイナリ
 * @param   msgData       メッセージデータ
 * @param   wordSet       ワードセット
 */
//------------------------------------------------------------------------------
void FinderUpperDraw::Init( APP_PARAM* pAppParam, void* pAppLytBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  m_pAppParam = pAppParam;
  m_pMsgData  = msgData;
  m_pWordSet  = wordSet;

  m_Version   = m_pFinderSave->GetCameraVersion();

  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  Initialize2D( m_pHeap, pAppLytBuff );

  m_Seq = SEQ_READY;
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool FinderUpperDraw::End( void )
{
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
void FinderUpperDraw::Update( void )
{
  app::ui::UIView::Update();

  switch( m_Seq )
  {
  case SEQ_READY:
    {
      Open();
    } break;

  case SEQ_IN:
    {
      // 入力開始はイベントのセットアップが終わってから ikuta_junya
      // SetInputEnabled( true );

      m_Seq = SEQ_MAIN;
    } break;
  
  case SEQ_MAIN:
    {
      // カメラズーム処理
      CameraZoom();
      // シャッターを開くアニメの再生
      OpenShutter();
      // ロトムコメントの更新
      UpdateRotomComment();
    } break;

  case SEQ_OUT:
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
void FinderUpperDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* pG2D = this->GetG2DUtil();
  pG2D->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
}

//------------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 * @return  trueで破棄完了
 */
//------------------------------------------------------------------------------
bool FinderUpperDraw::IsExit( void )
{
  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   オープン
 */
//------------------------------------------------------------------------------
void FinderUpperDraw::Open( void )
{
  this->SetVisible( true );

  m_Seq = SEQ_IN;
}

//------------------------------------------------------------------------------
/**
 * @brief   クローズ
 */
//------------------------------------------------------------------------------
void FinderUpperDraw::Close( void )
{
  //this->SetVisible( false );
  this->SetInputEnabled( false );

  m_Seq = SEQ_OUT;
}

//------------------------------------------------------------------------------
/**
 * @brief   準備完了か
 */
//------------------------------------------------------------------------------
bool FinderUpperDraw::IsOpened( void ) const
{
  return m_Seq == SEQ_MAIN;
}

//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 * @param   pHeap         ヒープ
 * @param   pAppLytBuff   レイアウトバイナリデータ
 */
//------------------------------------------------------------------------------
void FinderUpperDraw::Initialize2D( app::util::Heap* pHeap, void* pAppLytBuff )
{
  // アーカイブIDテーブル
  const app::util::G2DUtil::LytResData resTbl[] = {
    { pAppLytBuff, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_FINDER_FINDER_MIN_UPP_000_BFLYT,
      LA_FINDER_FINDER_MIN_UPP_000___NUM,
      pAppLytBuff,
      true,
      app::util::G2DUtil::SETUP_UPPER,
      true
    },
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
    m_pMsgData,
    m_pWordSet );

  this->SetVisible( false );
  this->SetInputEnabled( false );

  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();

  // 描画共通
  m_pDrawerBase = GFL_NEW(devHeap) DrawerBase( this, m_pHeap, 0, m_pWordSet, m_pMsgData );
  GFL_ASSERT( m_pDrawerBase );

  static const u32 c_ShutterIcon[] = {
    PANENAME_FINDER_MIN_UPP_000_PANE_COUNTICON_00,
    PANENAME_FINDER_MIN_UPP_000_PANE_COUNTICON_01,
    PANENAME_FINDER_MIN_UPP_000_PANE_COUNTICON_02,
    PANENAME_FINDER_MIN_UPP_000_PANE_COUNTICON_03,
    PANENAME_FINDER_MIN_UPP_000_PANE_COUNTICON_04,
    PANENAME_FINDER_MIN_UPP_000_PANE_COUNTICON_05,
  };

  for( u32 i=0; i<SHUTTER_COUNT_MAX; ++i )
  {
    m_pShutterCntPane[i] = m_pDrawerBase->GetPane( c_ShutterIcon[i] );
  }

  m_pInfoVisible = m_pDrawerBase->GetPane( PANENAME_FINDER_MIN_UPP_000_PANE_NULL_VISIBLE );

  // ズーム
  {
    gfl2::lyt::LytPane* zoomPane = m_pDrawerBase->GetPane( PANENAME_FINDER_MIN_UPP_000_PANE_ZOOMBAR );
    
    // 表示切替
    m_pDrawerBase->SetVisiblePane( zoomPane, (m_Version != 0) );

    // アニメの再生
    m_pDrawerBase->StartAnim( c_ZoomAnim[m_Version], false );
  }

  m_pRotomComment = m_pDrawerBase->GetTextBox( PANENAME_FINDER_MIN_UPP_000_PANE_TEXTBOX_ROTOM );
}

//------------------------------------------------------------------------------
/**
 * @brief   シャッターアニメの再生
 */
//------------------------------------------------------------------------------
void FinderUpperDraw::StartCloseShutter( void )
{
  u32 shutter_count = m_pAppParam->shutter_count;

  if( shutter_count > 0 )
  {
#if PM_DEBUG
    if( !m_pAppParam->debug_shutter )
#endif
    {
      m_pDrawerBase->StartAnim( LA_FINDER_FINDER_MIN_UPP_000_SHUTTER );
    }

    m_pAppParam->shutter_close = false;

    // シャッター音再生 ikuta_junya
    Sound::PlaySE( SEQ_SE_PF_SHUTTER );

    // シャッターカウントアイコンを減らす
    m_pDrawerBase->SetVisiblePane( m_pShutterCntPane[shutter_count - 1], false );

    m_ShutterAnimSeq = SHUTTER_ANIM_CLOSE;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   シャッターアニメの再生
 */
//------------------------------------------------------------------------------
void FinderUpperDraw::OpenShutter( void )
{
  if( m_ShutterAnimSeq == SHUTTER_ANIM_CLOSE )
  {
    if( !m_pAppParam->shutter_close )
    {
      if( m_pDrawerBase->IsAnimEnd( LA_FINDER_FINDER_MIN_UPP_000_SHUTTER ) )
      {
        m_pAppParam->shutter_close = true;
      }
    }
    else if( m_pAppParam->shutter_open )
    {
      m_pDrawerBase->StartAnim( LA_FINDER_FINDER_MIN_UPP_000_SHUTTER_00 );

      m_pAppParam->shutter_close = false;   //!< 開いたので閉じるフラグを戻す

      StartRotomComment();  //!< ロトムコメントの表示

      m_ShutterAnimSeq = SHUTTER_ANIM_OPEN;
    }
  }
  else if( m_ShutterAnimSeq == SHUTTER_ANIM_OPEN )
  {
    if( m_pDrawerBase->IsAnimEnd( LA_FINDER_FINDER_MIN_UPP_000_SHUTTER_00 ) )
    {
      m_uiListener->EndShutterAnim();

      m_ShutterAnimSeq = SHUTTER_ANIM_NONE;

      if( m_pAppParam->shutter_count == 0 )
      {
        // 通常時と試練時でメッセージを分岐 @fix NMCat[1965]
        if( m_pAppParam->trial_mode )
        {
          m_uiListener->OpenSystemMessage( msg_pokefinder_05_01_trial, END_MODE_FIN_SHOOT );
        }
        else {
          m_uiListener->OpenSystemMessage( msg_pokefinder_05_01, END_MODE_FIN_SHOOT );
        }
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   照準の表示切替
 */
//------------------------------------------------------------------------------
void FinderUpperDraw::ChangeSight( void )
{
  static const u32 c_SightAnim[] = {
    LA_FINDER_FINDER_MIN_UPP_000_SIGHTPATERN_00,
    LA_FINDER_FINDER_MIN_UPP_000_SIGHTPATERN_01,
    LA_FINDER_FINDER_MIN_UPP_000_SIGHTPATERN_02,
  };

  m_SightID = (m_SightID + 1) % SIGHT_NUM;

  m_pDrawerBase->StartAnim( c_SightAnim[m_SightID] );
}

//------------------------------------------------------------------------------
/**
 * @brief   ピントの変更
 * @param   mode    ピントのモード
 */
//------------------------------------------------------------------------------
void FinderUpperDraw::SetPintMode( u8 mode )
{
  static const u32 c_PintAnim[] = {
    LA_FINDER_FINDER_MIN_UPP_000__PINTPLATE_PATERN_00,
    LA_FINDER_FINDER_MIN_UPP_000__PINTPLATE_PATERN_01,
    LA_FINDER_FINDER_MIN_UPP_000__PINTPLATE_PATERN_02,
    LA_FINDER_FINDER_MIN_UPP_000__PINTPLATE_PATERN_03,
    LA_FINDER_FINDER_MIN_UPP_000__PINTPLATE_PATERN_04,
    LA_FINDER_FINDER_MIN_UPP_000__PINTPLATE_PATERN_05,
    LA_FINDER_FINDER_MIN_UPP_000__PINTPLATE_PATERN_06,
    LA_FINDER_FINDER_MIN_UPP_000__PINTPLATE_PATERN_07,
  };

  m_pDrawerBase->StartAnim( c_PintAnim[mode] );
}

//------------------------------------------------------------------------------
/**
 * @brief   ズーム処理
 */
//------------------------------------------------------------------------------
void FinderUpperDraw::CameraZoom( void )
{
  if( m_Version == 0 ) return;    //!< 初期バージョンの場合何もしない

  // ズームアニメ
  const u32 anmID = c_ZoomAnim[m_Version];

  const f32 zoomMin   = m_pAppParam->zoom_min;
  const f32 zoomMax   = m_pAppParam->zoom_max;
  const f32 zoomVal   = (m_pAppParam->zoom_val > m_pAppParam->zoom_max) ? m_pAppParam->zoom_max : m_pAppParam->zoom_val;

  const f32 zoomNow   = zoomVal - zoomMin;
  const f32 zoomRange = zoomMax - zoomMin;

  const f32 maxFrame = m_pDrawerBase->GetAnimMaxFrame( anmID );
  f32 nowFrame = (maxFrame * zoomNow) / zoomRange;

  // ズームバーのカーソル位置の設定
  m_pDrawerBase->SetAnimFrame( anmID, nowFrame );
}

//------------------------------------------------------------------------------
/**
 * @brief   情報表示の切り替え
 */
//------------------------------------------------------------------------------
void FinderUpperDraw::ChangeVisibleInfo( void )
{
  bool visible = m_pDrawerBase->IsVisiblePane( m_pInfoVisible );

  m_pDrawerBase->SetVisiblePane( m_pInfoVisible, !visible );
}

//------------------------------------------------------------------------------
/**
 * @brief   ロトムコメントの開始
 */
//------------------------------------------------------------------------------
void FinderUpperDraw::StartRotomComment( void )
{
  u32 score = m_pAppParam->photo_score;

  u32 rand  = System::GflUse::GetPublicRand() % 4;

  u32 seid  = SEQ_SE_SYS_NJ_015;
  u32 msgID = msg_pokefinder_camcom_09 + rand;

  if( score < 500 )
  {
    seid  = SEQ_SE_SYS_NJ_013;
    msgID = msg_pokefinder_camcom_01 + rand;
  }
  else if( score < 3000 )
  {
    seid  = SEQ_SE_SYS_NJ_014;
    msgID = msg_pokefinder_camcom_05 + rand;
  }

  Sound::PlaySE( seid );
  
  if( m_pRotomComment )
  {
    app::ui::UIView* view = m_pDrawerBase->GetUIView();
    {
      view->SetTextboxPaneMessage( m_pRotomComment, msgID );
    }
  }

  if( m_RotomAnim == MIN_UPP_ANIM_NULL )
  {
    m_RotomAnim = LA_FINDER_FINDER_MIN_UPP_000_ROTOM_IN;
    m_pDrawerBase->StartAnim( m_RotomAnim );

    m_RotomSeq = ROTOM_SEQ_BEGIN;
  }
  else {
    m_RotomAnim = LA_FINDER_FINDER_MIN_UPP_000_ROTOM_COMMENT;
    m_pDrawerBase->StartAnim( m_RotomAnim );

    m_RotomSeq = ROTOM_SEQ_RESTART;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ロトムコメントの更新
 */
//------------------------------------------------------------------------------
void FinderUpperDraw::UpdateRotomComment( void )
{
  switch( m_RotomSeq )
  {
  case ROTOM_SEQ_BEGIN:
  case ROTOM_SEQ_RESTART:
    {
      if( m_pDrawerBase->IsAnimEnd( m_RotomAnim ) )
      {
        m_pDrawerBase->StopAnim( m_RotomAnim );
        m_RotomAnim = LA_FINDER_FINDER_MIN_UPP_000_ROTOM_KEEP;
        m_pDrawerBase->StartAnim( m_RotomAnim );

        m_RotomFrame = 0;
        m_RotomSeq   = ROTOM_SEQ_KEEP;
      }
    } break;

  case ROTOM_SEQ_KEEP:
    {
      m_RotomFrame++;

      if( m_RotomFrame >= 60 )
      {
        m_pDrawerBase->StopAnim( m_RotomAnim );
        m_RotomAnim = LA_FINDER_FINDER_MIN_UPP_000_ROTOM_OUT;
        m_pDrawerBase->StartAnim( m_RotomAnim );

        m_RotomSeq = ROTOM_SEQ_OUT;
      }
    } break;

  case ROTOM_SEQ_OUT:
    {
      if( m_pDrawerBase->IsAnimEnd( m_RotomAnim ) )
      {
        m_RotomAnim = MIN_UPP_ANIM_NULL;
        m_RotomSeq  = ROTOM_SEQ_NONE;
      }
    } break;
  }
}

GFL_NAMESPACE_END( Finder )
GFL_NAMESPACE_END( App )
