//======================================================================
/**
 * @file FinderMenuCommentProductDraw.cpp
 * @date 2015/11/02 19:24:12
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "FinderMenuCommentProductDraw.h"
#include "FinderMenuDrawListener.h"
#include "FinderMenuDrawerBase.h"

#include <GameSys/include/GameData.h>
#include <Field/FieldRo/include/TrialModel/FieldFinderModel_define.h>
#include <Sound/include/Sound.h>

// arc
#include <niji_conv_header/app/finder_menu/finder_menu.h>
#include <niji_conv_header/app/finder_menu/finder_menu_pane.h>
#include <niji_conv_header/app/finder_menu/finder_menu_anim_list.h>
#include <niji_conv_header/message/msg_pokefinder.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderMenu)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace {
  enum Sequence
  {
    SEQ_INIT,
    SEQ_READY,
    SEQ_COMMENT_ANIM,
    SEQ_OUT,
    SEQ_END
  };

  //static const u8 ORDER_COMMENT_PRODUCT_DRAW = 129;
  static const u8 ORDER_COMMENT_PRODUCT_DRAW = 128;
}

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
FinderMenuCommentProductDraw::FinderMenuCommentProductDraw( app::util::Heap* pHeap,app::util::AppRenderingManager* pAppRanderingManager )
  : app::ui::UIView( pHeap, pHeap->GetDeviceAllocator(), pHeap->GetDeviceAllocator() )
  , m_pHeap( pHeap )
  , m_uiListener( NULL )
  , m_pAppRenderingManager( pAppRanderingManager )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_pAppParam( NULL )
  , m_Seq( SEQ_INIT )
  , m_pDrawerBase( NULL )
  , m_CommentAnim(0)
  , m_CommentCount(0)
  , m_pScoreText(NULL)
{
  for( u32 i=0; i<COMMENT_MAX; ++i )
  {
    m_pCommentText[i] = NULL;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   このビュー専用のUIリスナー設定
 * @param   pListener   UIリスナー
 */
//------------------------------------------------------------------------------
void FinderMenuCommentProductDraw::SetTitleViewUIListener( FinderMenuDrawListener* pListener )
{
  m_uiListener = pListener;
}

//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 * @param   appParam      外部設定パラメータ
 * @param   pAppLytBuff   レイアウトバイナリ
 * @param   msgData       メッセージデータ
 * @param   wordSet       ワードセット
 */
//------------------------------------------------------------------------------
void FinderMenuCommentProductDraw::Init( APP_PARAM* appParam, void* pAppLytBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  m_pMsgData  = msgData;
  m_pWordSet  = wordSet;
  m_pAppParam = appParam;

  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  Initialize2D( m_pHeap, pAppLytBuff );
  InitializeText();

  SetInputListener( this );

  m_Seq = SEQ_READY;
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool FinderMenuCommentProductDraw::End( void )
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
void FinderMenuCommentProductDraw::Update( void )
{
  app::ui::UIView::Update();

  switch( m_Seq )
  {
  case SEQ_COMMENT_ANIM:
    {
      if( IsPlayCommentSE() )
      {
        PlayCommentSE();
      }

      if( m_pDrawerBase->IsAnimEnd( m_CommentAnim ) )
      {
        if( m_uiListener )
        {
          m_uiListener->OpenCommentList();
        }
      }
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
void FinderMenuCommentProductDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* pG2D = this->GetG2DUtil();
  pG2D->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0, ORDER_COMMENT_PRODUCT_DRAW );
}

//------------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 * @return  trueで破棄完了
 */
//------------------------------------------------------------------------------
bool FinderMenuCommentProductDraw::IsExit( void )
{
  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   オープン
 */
//------------------------------------------------------------------------------
void FinderMenuCommentProductDraw::Open( u32 photoIdx )
{
  Setup( photoIdx );

  this->SetInputEnabled( true );
  this->SetVisible( true );

  m_Seq = SEQ_COMMENT_ANIM;
}

//------------------------------------------------------------------------------
/**
 * @brief   クローズ
 */
//------------------------------------------------------------------------------
void FinderMenuCommentProductDraw::Close( void )
{
  this->SetVisible( false );
  this->SetInputEnabled( false );

  m_Seq = SEQ_OUT;
}


//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 * @param   pHeap         ヒープ
 * @param   pAppLytBuff   レイアウトバイナリデータ
 */
//------------------------------------------------------------------------------
void FinderMenuCommentProductDraw::Initialize2D( app::util::Heap* pHeap, void* pAppLytBuff )
{
  // アーカイブIDテーブル
  const app::util::G2DUtil::LytResData resTbl[] = {
    { pAppLytBuff, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_FINDER_MENU_FINDER_COMMENTANIM_LOW_001_BFLYT,
      LA_FINDER_MENU_FINDER_COMMENTANIM_LOW_001___NUM,
      pAppLytBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
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
    m_pWordSet);

  this->SetVisible( false );
  this->SetInputEnabled( false );

  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();

  // 描画共通処理
  m_pDrawerBase = GFL_NEW(devHeap) DrawerBase( this, m_pHeap, 0, m_pWordSet, m_pMsgData );
  GFL_ASSERT( m_pDrawerBase );

  static const u32 c_CommentText[] = {
    PANENAME_FINDER_COMMENTANIM_LOW_001_PANE_COMMENT_00,
    PANENAME_FINDER_COMMENTANIM_LOW_001_PANE_COMMENT_01,
    PANENAME_FINDER_COMMENTANIM_LOW_001_PANE_COMMENT_02,
    PANENAME_FINDER_COMMENTANIM_LOW_001_PANE_COMMENT_03,
    PANENAME_FINDER_COMMENTANIM_LOW_001_PANE_COMMENT_04,
    PANENAME_FINDER_COMMENTANIM_LOW_001_PANE_COMMENT_05,
    PANENAME_FINDER_COMMENTANIM_LOW_001_PANE_COMMENT_06,
    PANENAME_FINDER_COMMENTANIM_LOW_001_PANE_COMMENT_07,
    PANENAME_FINDER_COMMENTANIM_LOW_001_PANE_COMMENT_09,
    PANENAME_FINDER_COMMENTANIM_LOW_001_PANE_COMMENT_10,
    PANENAME_FINDER_COMMENTANIM_LOW_001_PANE_COMMENT_11,
    PANENAME_FINDER_COMMENTANIM_LOW_001_PANE_COMMENT_12,
    PANENAME_FINDER_COMMENTANIM_LOW_001_PANE_COMMENT_13,
    PANENAME_FINDER_COMMENTANIM_LOW_001_PANE_COMMENT_14,
    PANENAME_FINDER_COMMENTANIM_LOW_001_PANE_COMMENT_15,
  };
  for( u32 i=0; i<COMMENT_MAX; ++i )
  {
    m_pCommentText[i] = m_pDrawerBase->GetTextBox( c_CommentText[i] );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト表示
 */
//------------------------------------------------------------------------------
void FinderMenuCommentProductDraw::InitializeText( void )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   セットアップ
 */
//------------------------------------------------------------------------------
void FinderMenuCommentProductDraw::Setup( u32 photoIdx )
{
  // 評価対象の写真データ
  Field::TrialModel::FinderPhotoData* photoData = m_pAppParam->photo_data[photoIdx];

  u32 cmtCnt = 0;

  for( u32 i=0; i<COMMENT_MAX; ++i )
  {
    u32 cmtIdx = photoData->data.comment[i];

    FUKUSHIMA_PRINT( "Comment: index[%d] id[%d]\n", i, cmtIdx );

    if( cmtIdx > 0 )
    {
      this->SetTextboxPaneMessage( m_pCommentText[i], cmtIdx );

      cmtCnt++;
    }
  }

  if( cmtCnt == 0 )
  {
    GFL_ASSERT(0);
    cmtCnt = 1;
    photoData->data.comment[0] = msg_pokefinder_comment_01_01;
  }

  static const u32 c_CommentAnim[] = {
    LA_FINDER_MENU_FINDER_COMMENTANIM_LOW_001_LIKE_00,
    LA_FINDER_MENU_FINDER_COMMENTANIM_LOW_001_LIKE_01,
    LA_FINDER_MENU_FINDER_COMMENTANIM_LOW_001_LIKE_02,
    LA_FINDER_MENU_FINDER_COMMENTANIM_LOW_001_LIKE_03,
    LA_FINDER_MENU_FINDER_COMMENTANIM_LOW_001_LIKE_04,
    LA_FINDER_MENU_FINDER_COMMENTANIM_LOW_001_LIKE_05,
    LA_FINDER_MENU_FINDER_COMMENTANIM_LOW_001_LIKE_06,
    LA_FINDER_MENU_FINDER_COMMENTANIM_LOW_001_LIKE_07,
    LA_FINDER_MENU_FINDER_COMMENTANIM_LOW_001_LIKE_08,
    LA_FINDER_MENU_FINDER_COMMENTANIM_LOW_001_LIKE_09,
    LA_FINDER_MENU_FINDER_COMMENTANIM_LOW_001_LIKE_10,
    LA_FINDER_MENU_FINDER_COMMENTANIM_LOW_001_LIKE_11,
    LA_FINDER_MENU_FINDER_COMMENTANIM_LOW_001_LIKE_12,
    LA_FINDER_MENU_FINDER_COMMENTANIM_LOW_001_LIKE_13,
    LA_FINDER_MENU_FINDER_COMMENTANIM_LOW_001_LIKE_14,
  };
  m_CommentAnim = c_CommentAnim[cmtCnt - 1];
  m_pDrawerBase->StartAnim( m_CommentAnim );

  // SE再生
  PlayCommentSE();
}

//------------------------------------------------------------------------------
/**
 * @brief   コメントSEの再生判定
 */
//------------------------------------------------------------------------------
bool FinderMenuCommentProductDraw::IsPlayCommentSE( void )
{
  if( m_CommentCount < COMMENT_MAX )
  {
    f32 nowFrame = m_pDrawerBase->GetAnimFrame( m_CommentAnim );
    f32 checkFrame = static_cast<f32>( m_CommentCount * 5 );

    FUKUSHIMA_PRINT(
      "IsPlayCommentSE()\n"
      "CommentCount[%d]\n"
      "nowFrame[%.2f]\n"
      "checkFrame[%.2f]\n"
      , m_CommentCount
      , nowFrame
      , checkFrame
    );

    return nowFrame >= checkFrame;
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   コメントSEの再生
 */
//------------------------------------------------------------------------------
void FinderMenuCommentProductDraw::PlayCommentSE( void )
{
  Sound::PlaySE( SEQ_SE_PF_COMMENT );

  m_CommentCount++;
}





//==============================================================================
/**
 *      ↓　以下がリスナー関連　↓
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   キー入力通知用リスナ
 * @param   pKey
 * @param   pStick
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult FinderMenuCommentProductDraw::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( m_uiListener == NULL ) return ENABLE_ACCESS;

  if( m_uiListener )
  {
    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      m_uiListener->OpenCommentList();
    }
  }

  return ENABLE_ACCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief   タッチパネルイベントの検知
 * @param   pTouchPanel
 * @param   isTouch
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult FinderMenuCommentProductDraw::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if( m_uiListener == NULL ) return ENABLE_ACCESS;

  if( m_uiListener )
  {
    if( pTouchPanel->IsTouchTrigger() )
    {
      m_uiListener->OpenCommentList();
    }
  }

  return ENABLE_ACCESS;
}


GFL_NAMESPACE_END(FinderMenu)
GFL_NAMESPACE_END(App)
