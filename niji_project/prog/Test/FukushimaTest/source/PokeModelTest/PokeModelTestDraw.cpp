#if PM_DEBUG

//======================================================================
/**
 * @file PokeModelTestDraw.cpp
 * @date 2015/09/07 11:22:21
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <Test/FukushimaTest/source/PokeModelTest/PokeModelTestDraw.h>
#include <Test/FukushimaTest/source/PokeModelTest/PokeModelTestDrawListener.h>
#include <Test/FukushimaTest/source/PokeModelTest/PokeModelTestModel.h>

#include <AppLib/include/Tool/app_tool_PokeIcon.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <Print/include/SystemFont.h>

// gflib2
#include <Fade/include/gfl2_FadeManager.h>

// arc
#include <niji_conv_header/app/field_status/field_status.h>
#include <niji_conv_header/app/field_status/field_status_pane.h>
#include <niji_conv_header/app/field_status/field_status_anim_list.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace {
  enum Sequence {
    SEQ_INIT,
    SEQ_READY,
    SEQ_IN,
    SEQ_MAIN,
    SEQ_OUT,
    SEQ_END_READY,
    SEQ_END
  };

  //! ポケモンID
  static const MonsNo c_MonsNo[] = {
    MONSNO_PIKATYUU,
    MONSNO_HUSIGIBANA,
    MONSNO_HAPINASU,
    MONSNO_ZIGUZAGUMA,
  };
} // namespace


//------------------------------------------------------------------------------
/**
 * @brief	  コンストラクタ
 */
//------------------------------------------------------------------------------
PokeModelTestDraw::PokeModelTestDraw( app::util::Heap* pHeap, app::util::AppRenderingManager* pAppRenderingManager )
  : app::ui::UIView( pHeap, pHeap->GetDeviceAllocator(), pHeap->GetDeviceAllocator() )
  , m_pHeap( pHeap )
  , m_uiListener( NULL )
  , m_pAppRenderingManager( pAppRenderingManager )
  , m_pGameManager( GFL_SINGLETON_INSTANCE( GameSys::GameManager ) )
  , m_pFadeManager( GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager ) )
  , m_LayoutID( 0 )
  , m_Seq( SEQ_INIT )
  , m_pPokeModel( NULL )
  , m_ModelIndex( 0 )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief	  デストラクタ
 */
//------------------------------------------------------------------------------
PokeModelTestDraw::~PokeModelTestDraw( void )
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief	  このビュー専用のUIリスナー設定
 * @param   pListener   UIリスナー
 */
//------------------------------------------------------------------------------
void PokeModelTestDraw::SetTitleViewUIListener( Test::Fukushima::PokeModelTestDrawListener* pListener )
{
  m_uiListener = pListener;
}

//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 * @param   pAppParam
 * @param   pAppLytBuff   レイアウトバイナリ
 */
//------------------------------------------------------------------------------
void PokeModelTestDraw::Init( void* pAppLytBuff, gfl2::str::MsgData* pMsgData, print::WordSet* pWordSet )
{
  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  Initialize2D( m_pHeap, pAppLytBuff, pMsgData, pWordSet );

  SetInputListener( this );

  m_Seq = SEQ_READY;
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool PokeModelTestDraw::End( void )
{
  GFL_SAFE_DELETE( m_pPokeModel );

  print::SystemFont_ResetNumFontInfo(); //@fix GFNMCat[1083]
  print::SystemFont_DeleteNumFont();

  Delete2D();
  DeleteGraphicsSystem();

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void PokeModelTestDraw::Update( void )
{
  app::ui::UIView::Update();
  if( m_pPokeModel ) m_pPokeModel->Update();

  // シーケンス処理
  switch( m_Seq )
  {
  case SEQ_READY:
    {
      if( m_pPokeModel )
      {
        if( !m_pPokeModel->Initialize() ) break;

        m_pPokeModel->SetPokeModel( c_MonsNo[m_ModelIndex] );
      }

      // ステータス画面を開く
      Open();
    } break;

  case SEQ_IN:
    {
      if( !IsEndInOutAnim( true ) ) break;
      
      if( m_pPokeModel )
      {
        if( !m_pPokeModel->IsModelReady() ) break;

        m_pPokeModel->SetVisibleModel( true );
      }

      this->SetInputEnabled( true );

      m_Seq = SEQ_MAIN;
    } break;

  case SEQ_OUT:
    {
      if( !IsEndInOutAnim(false) ) break;

      m_Seq = SEQ_END_READY;
    } break;

  case SEQ_END_READY:
    {
      if( m_pPokeModel && !m_pPokeModel->Terminate() ) break;

      if( m_uiListener ) m_uiListener->OnEnd();

      m_Seq = SEQ_END;
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void PokeModelTestDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
 
  if( m_Seq > SEQ_READY )
  {
    pG2DUtil->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 * @return  trueで破棄完了
 */
//------------------------------------------------------------------------------
bool PokeModelTestDraw::IsExit( void )
{
  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   ステータス画面のオープン
 */
//------------------------------------------------------------------------------
void PokeModelTestDraw::Open( void )
{
  // フェードイン
  m_pFadeManager->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, 3 );
	
  m_Seq = SEQ_IN;
}

//------------------------------------------------------------------------------
/**
 * @brief   ステータス画面のクローズ
 */
//------------------------------------------------------------------------------
void PokeModelTestDraw::Close( void )
{
  this->SetInputEnabled( false );

  gfl2::math::Vector4 color( 0, 0, 0, 255 );
  m_pFadeManager->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );

  m_Seq = SEQ_OUT;
}

//------------------------------------------------------------------------------
/**
 * @brief   イン・アウトアニメが終了したかどうか
 * @param   in    インアニメかどうか
 * @return  true = 終了、false = 継続
 */
//------------------------------------------------------------------------------
bool PokeModelTestDraw::IsEndInOutAnim( bool in )
{
  return m_pFadeManager->IsEnd( gfl2::Fade::DISP_LOWER )
    && m_pFadeManager->IsEnd( gfl2::Fade::DISP_UPPER );
}


//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 * @param   pHeap         ヒープ
 * @param   pAppLytBuff   レイアウトバイナリデータ
 */
//------------------------------------------------------------------------------
void PokeModelTestDraw::Initialize2D( app::util::Heap* pHeap, void* pAppLytBuff, gfl2::str::MsgData* pMsgData, print::WordSet* pWordSet )
{
  // アーカイブIDテーブル
  const app::util::G2DUtil::LytResData resTbl[] = {
    { pAppLytBuff, 1, static_cast<u32>(app::util::G2DUtil::ATTACH_ROOT) },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    { // MIN_LOW
      0,
      LYTRES_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000_BFLYT,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000___NUM,
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
    pMsgData,
    pWordSet );

  this->SetInputEnabled( false );

  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();

  m_pPokeModel = GFL_NEW(devHeap) PokeModel( m_pHeap, m_pAppRenderingManager );
  GFL_ASSERT( m_pPokeModel );
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
void PokeModelTestDraw::OnPreLoadResource( void )
{
  // num_font.bffntの登録
  print::SystemFont_CreateNumFont( m_pHeap->GetDeviceHeap(), m_pHeap->GetDeviceHeap() );
  print::SystemFont_SetLytSystemNumFont();
}

//------------------------------------------------------------------------------
/**
 * @brief   キー入力通知用リスナ
 * @param   pKey
 * @param   pStick
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult PokeModelTestDraw::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( m_uiListener == NULL ) return DISABLE_ACCESS;

  if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    Close();
  }

  u8 modelIndex = m_ModelIndex;

  if( pButton->IsRepeat( gfl2::ui::BUTTON_UP ) )
  {
    if( m_ModelIndex == 0 ) m_ModelIndex = GFL_NELEMS(c_MonsNo) - 1;
    else                    m_ModelIndex--;
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_DOWN ) )
  {
    if( m_ModelIndex == (GFL_NELEMS(c_MonsNo) - 1) ) m_ModelIndex = 0;
    else                                             m_ModelIndex++;
  }

  if( modelIndex != m_ModelIndex )
  {
    if( m_pPokeModel )
    {
      MonsNo monsNo = c_MonsNo[m_ModelIndex];
      m_pPokeModel->SetPokeModel( monsNo );
    }
  }

  return ENABLE_ACCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインアクション通知用リスナ
 * @param   paneId
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult PokeModelTestDraw::OnLayoutPaneEvent( const u32 paneId )
{
  if( m_uiListener == NULL ) return DISABLE_ACCESS;

  return ENABLE_ACCESS;
}

GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

#endif // PM_DEBUG