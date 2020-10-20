//======================================================================
/**
 * @file    StatusUpperDraw.cpp
 * @date    2015/11/30 18:16:35
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：メイン画面(上)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "StatusUpperDraw.h"
#include "StatusDrawListener.h"
#include "StatusDrawerBase.h"

#include <App/Status/include/StatusAppParam.h>
#include <App/Status/source/upperParts/StatusWazaExplain.h>

#include <App/StatusUpper/include/AppToolStatusUpperDraw.h>
#include <Sound/include/Sound.h>

// gflib2
#include <Fade/include/gfl2_FadeManager.h>

// arc
#include <niji_conv_header/app/field_status/field_status.h>
#include <niji_conv_header/app/field_status/field_status_pane.h>
#include <niji_conv_header/app/field_status/field_status_anim_list.h>

#include <niji_conv_header/message/msg_pokemon_status.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace {
  //! @brief  シーケンスID
  enum {
    SEQ_INIT,
    SEQ_READY,
    SEQ_IDLE,
    SEQ_IN,
    SEQ_MAIN,
    SEQ_OUT,
    SEQ_END,
    SEQ_NUM,
  };
}


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
UpperMainDraw::UpperMainDraw( CreateParam& param )
  : app::ui::UIView(param.heap, param.heap->GetDeviceAllocator(), param.heap->GetDeviceAllocator())
  , m_pHeap(param.heap)
  , m_pAppRenderingManager(param.renderMan)
  , m_pFadeManager(param.fadeMan)
  , m_uiListener(NULL)
  , m_pCommonIcon(param.cmnIcon)
  , m_pTrainerIcon(param.trainerIconPtr)
  , m_pUpperDraw(NULL)
  , m_Seq(SEQ_INIT)
  , m_pAppParam(NULL)
  , m_pDrawerBase(NULL)
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   このビュー専用のUIリスナー設定
 *
 * @param   pListener   UIリスナー
 */
//------------------------------------------------------------------------------
void UpperMainDraw::SetTitleViewUIListener( StatusDrawListener* listener )
{
  m_uiListener = listener;
}


//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void UpperMainDraw::Init( InitParam& param )
{
  m_pAppParam = param.appParam;

  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  Initialize2D( param );

  m_Seq = SEQ_READY;
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool UpperMainDraw::End( void )
{
  GFL_SAFE_DELETE( m_pWazaExplain );
  GFL_SAFE_DELETE( m_pDrawerBase );

  if( m_pUpperDraw )
  {
    if( !m_pUpperDraw->EndFunc() ) return false;

    m_pUpperDraw->RemoveFromSuperView();    // @fix GFNMCat[4752]

    GFL_SAFE_DELETE( m_pUpperDraw );
  }

  //Delete2D();
  DeleteGraphicsSystem();

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void UpperMainDraw::Update( void )
{
  app::ui::UIView::Update();

  switch( m_Seq )
  {
  case SEQ_READY:
    {
      // 準備シーケンス
      if( !Sequence_Ready() ) break;

      m_Seq = SEQ_IDLE;
    } break;

  case SEQ_IN:
    {
      if( !IsOpen() ) break;

      m_Seq = SEQ_MAIN;
    } break;

  case SEQ_OUT:
    {
      // ステータス画面の終了へ
      m_uiListener->EndStatus();

      m_Seq = SEQ_END;
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   準備完了かどうか
 *
 * @return  "true  = 完了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool UpperMainDraw::IsReady( void ) const
{
  return m_Seq == SEQ_IDLE;
}

//------------------------------------------------------------------------------
/**
 * @brief   メインシーケンスかどうか
 *
 * @return  "true  = メインシーケンス"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool UpperMainDraw::IsMainSeq( void ) const
{
  return m_Seq == SEQ_MAIN;
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンパラメータのセット
 *
 * @param   pp    表示する対象のポケモンパラメータ
 */
//------------------------------------------------------------------------------
void UpperMainDraw::SetPokemonParam(const pml::pokepara::PokemonParam* pp, const BattleFesTool::BFCTrainerInfo* pBFCTrainerInfo)
{
  if( m_pUpperDraw )
  {
    m_pUpperDraw->SetPokemonParam( pp, pBFCTrainerInfo );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   オープン
 */
//------------------------------------------------------------------------------
void UpperMainDraw::Open( void )
{
  // フェードイン
  if( m_pFadeManager )
  {
    m_pFadeManager->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, gfl2::Fade::FADE_DEFAULT_SYNC );
  }

  m_Seq = SEQ_IN;
}

//------------------------------------------------------------------------------
/**
 * @brief   クローズ
 */
//------------------------------------------------------------------------------
void UpperMainDraw::Close( void )
{
  // 入力を無効
  this->SetInputEnabled( false );

  // フェードアウト
  if( m_pFadeManager )
  {
    gfl2::math::Vector4 color( 0, 0, 0, 255 );

    m_pFadeManager->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );
  }

  m_Seq = SEQ_OUT;
}

//------------------------------------------------------------------------------
/**
 * @brief   オープン中？
 *
 * @return  "true  = 終了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool UpperMainDraw::IsOpen( void )
{
  if( m_pFadeManager )
  {
    return m_pFadeManager->IsEnd( gfl2::Fade::DISP_LOWER )
      && m_pFadeManager->IsEnd( gfl2::Fade::DISP_UPPER );
  }

  return true;
}


//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void UpperMainDraw::Initialize2D( InitParam& param )
{
  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();

  App::Tool::FieldStatusUpperDraw::CreateParam createParam;
  {
    createParam.heap                  = m_pHeap;
    createParam.renderMan             = m_pAppRenderingManager;
    createParam.cmnIcon               = m_pCommonIcon;
    createParam.trainerIconPtr        = m_pTrainerIcon;
    createParam.msgData               = param.baseMsgData;
    createParam.wordSet               = param.wordSet;
    createParam.lytBuff               = param.appLytBuff;
    createParam.helpFlag              = true;
    createParam.battleFesCircleLayout = m_pAppParam->IsBFCMode();
  }
  m_pUpperDraw = GFL_NEW(devHeap) App::Tool::FieldStatusUpperDraw( createParam );
  {
    GFL_ASSERT(m_pUpperDraw);

    this->AddSubView( m_pUpperDraw );
  }

  {
    // 描画共通処理の生成
    m_pDrawerBase = GFL_NEW(devHeap) DrawerBase( m_pUpperDraw, m_pHeap, 0, param.baseMsgData, param.wordSet );
    {
      GFL_ASSERT(m_pDrawerBase);
    }

    // 技説明
    m_pWazaExplain = GFL_NEW(devHeap) WazaExplain( m_pDrawerBase, param.wazaMsgData );
    {
      GFL_ASSERT(m_pWazaExplain);

      // 初期化
      m_pWazaExplain->Initialize();
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   準備シーケンス
 *
 * @return  "true  = 準備完了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool UpperMainDraw::Sequence_Ready( void )
{
  if( m_pUpperDraw )
  {
    if( !m_pUpperDraw->InitFunc() ) return false;
  }

  return true;
}


//------------------------------------------------------------------------------
/**
 * @brief   技の説明を開く
 *
 * @param   wazaNo    技No
 *
 * @return  "true  = 開くことに成功"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool UpperMainDraw::OpenWazaExplain( u32 wazaNo )
{
  if( m_pWazaExplain )
  {
    return m_pWazaExplain->Open( wazaNo );
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   技の説明を閉じる
 */
//------------------------------------------------------------------------------
void UpperMainDraw::CloseWazaExplain( void )
{
  if( m_pWazaExplain )
  {
    m_pWazaExplain->Close();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   表示する技の設定
 *
 * @param   wazaNo    技No
 */
//------------------------------------------------------------------------------
void UpperMainDraw::SetWazaExplain( u32 wazaNo )
{
  if( m_pWazaExplain )
  {
    m_pWazaExplain->Setup( wazaNo );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   技説明ウィンドウが閉じきったかどうか
 *
 * @return  "true  == 閉じ終わった"
 * @return  "false == 継続"
 */
//------------------------------------------------------------------------------
bool UpperMainDraw::IsClosedWazaExplain( void ) const
{
  if( m_pWazaExplain )
  {
    return m_pWazaExplain->IsClosed();
  }

  return true;
}


//------------------------------------------------------------------------------
/**
 * @brief   レーダーチャートを表示
 */
//------------------------------------------------------------------------------
void UpperMainDraw::VisibleRadarChart( void )
{
  if( m_pUpperDraw )
  {
    m_pUpperDraw->SetVisibleRadarChart( true );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   レーダーチャートを非表示
 */
//------------------------------------------------------------------------------
void UpperMainDraw::InvisibleRadarChart( void )
{
  if( m_pUpperDraw )
  {
    m_pUpperDraw->SetVisibleRadarChart( false );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   レーダーチャートの表示モード切替
 */
//------------------------------------------------------------------------------
void UpperMainDraw::ChangeRadarChartMode( void )
{
  if( m_pUpperDraw )
  {
    m_pUpperDraw->ChangeRadarChartMode();
  }
}

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)
