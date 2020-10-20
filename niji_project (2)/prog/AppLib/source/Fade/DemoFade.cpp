//======================================================================
/**
 * @file DemoFade.cpp
 * @date 2015/08/19 12:22:10
 * @author miyachi_soichi
 * @brief 戦闘導入演出
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "DemoLib/SequencePlayer/include/SequencePlayerCore.h"
#include "AppLib/include/Fade/DemoFade.h"
// gfl2
#include <System/include/Timer/gfl2_PerformanceCounter.h>
#include <Fade/include/gfl2_FadeManager.h>

GFL_NAMESPACE_BEGIN(AppLib)
GFL_NAMESPACE_BEGIN(Fade)

/**
 *  @class  DemoFade
 *  @brief  戦闘導入演出
 */
/**
 *  @brief  コンストラクタ
 */
DemoFade::DemoFade( void )
: gfl2::Fade::FadeSuper()
, m_pSequenceViewSystem( NULL )
, m_pHeap( NULL )
, m_eState( STATE_NONE )
, m_nDemoID( 1 )
, m_bPause( false )
, m_nIndex( 0 )
, m_pTextureNode( NULL )
, m_isSequenceStartupWaitMode( false)
{
}

/**
 *  @brief  デストラクタ
 */
DemoFade::~DemoFade( void )
{
  Terminate();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void DemoFade::Initialize( gfl2::heap::HeapBase *pHeap, s32 nSysHeapSize, s32 nDevHeapSize )
{
  m_pHeap = pHeap;
  m_nSysHeapSize = nSysHeapSize;
  m_nDevHeapSize = nDevHeapSize;
}

/**
 *  @brief  解放
 */
void DemoFade::Terminate( void )
{
}

/**
 *  @brief  更新処理
 */
void DemoFade::UpdateFunc( void )
{
  if( m_bPause ){ return; }

  switch( m_eState )
  {
  case STATE_create:
    if( !m_pSequenceViewSystem )
    {
      m_pSequenceViewSystem = GFL_NEW( m_pHeap ) DemoLib::Sequence::SequenceViewSystem();
      m_pSequenceViewSystem->SetUseEffectSystem(false);
      m_pSequenceViewSystem->SetUseCharaModel(false);
      m_pSequenceViewSystem->SetUseCommonTextWindow(false);
      m_pSequenceViewSystem->SetUsePokemonSystem(false);
      m_pSequenceViewSystem->SetForceOneStep(true);

      DemoLib::Sequence::SequenceRenderingPipeLine::UseOption useOption;
      useOption.isUseModelRenderPathAfterEdge = false;
      useOption.isUseSkyRenderPath = false;
      useOption.isUseEdgeMapPath = false;
      useOption.isUseModelOutLinePath = false;
      useOption.isUseLayoutRenderPath = false;
      useOption.isUseScreenEffectRenderPath = false;
      useOption.isUseBloomRenderPath = false;
      useOption.isUseInframePath = false;
      m_pSequenceViewSystem->SetUseOption( useOption);

      m_pSequenceViewSystem->Initialize( m_pHeap, m_pHeap, m_nSysHeapSize, m_nDevHeapSize );
      // テクスチャ差し替え
      if( m_pTextureNode )
      {
        m_pSequenceViewSystem->SetReplaceTexture( m_nIndex, m_pTextureNode );
      }
      // シーケンス再生待機モードの設定
      m_pSequenceViewSystem->SetStartupWaitMode( m_isSequenceStartupWaitMode);
    }
    m_eState = STATE_first;
    break;
  case STATE_first:
    m_pSequenceViewSystem->SeqComFunc_PlaySequenceFile( m_nDemoID );
    m_eState = STATE_running;
    /* fallthrough */

  case STATE_running:
    m_pSequenceViewSystem->Update();
    // 終了検知
    if( m_pSequenceViewSystem->SeqComFunc_IsFinishSequenceFile() )
    {
      m_eState = STATE_finish;
    }
    break;
  case STATE_finish:
    if( m_pSequenceViewSystem->Finalize() )
    {
      GFL_SAFE_DELETE( m_pSequenceViewSystem );
      m_eState = STATE_NONE;
    }
    break;
  }
}

/**
 *  @brief  描画準備
 */
void DemoFade::PreDrawFunc( void )
{
  if( m_eState == STATE_running && m_pSequenceViewSystem )
  {
    m_pSequenceViewSystem->PreDraw();
  }
}

/**
 *  @brief  描画処理
 */
void DemoFade::DrawFunc( gfl2::gfx::CtrDisplayNo displayNo )
{
  if( m_eState == STATE_running && m_pSequenceViewSystem )
  {
    m_pSequenceViewSystem->Draw( displayNo );
  }
}

/**
 *  @brief  画面遷移  終了検知
 */
bool DemoFade::IsEnd( void )
{
  bool bResult = true;

  if( m_pSequenceViewSystem )
  {
    //if( !m_pSequenceViewSystem->SeqComFunc_IsFinishSequenceFile() )
    {
      bResult = false;
    }
  }

  return bResult;
}

/**
 *  @brief  画面遷移ステータスが終了しているかどうか
 */
bool DemoFade::IsEndStatus( void )
{
  return ( m_eState == STATE_NONE );
}

/**
 *  @brief  ポーズON/OFF
 */
void DemoFade::SetPause( bool is_pause )
{
  m_bPause = is_pause;
}

/**
 *  @brief  ポーズ状態取得
 */
bool DemoFade::IsPause( void ) const
{
  return m_bPause;
}

/**
 *  @brief  テクスチャ差し替え
 */
void DemoFade::SetReplaceTexture( s32 index, gfl2::renderingengine::scenegraph::DagNode *pTexture )
{
  m_nIndex = index;
  m_pTextureNode = pTexture;

  if( m_pSequenceViewSystem)
  {
    m_pSequenceViewSystem->SetReplaceTexture( m_nIndex, m_pTextureNode );
  }
}

/**
 * @brief シーケンスプレイヤーの初期化完了後にシーケンス再生を待機させるモードの設定
 * @param trueにするとシーケンスプレイヤー初期化完了後にシーケンスが再生されなくなる
 * @note デフォルトはfalseです。
 * @note trueに設定した場合、明示的にfalseを設定しないとシーケンスが再生されません
 */
void DemoFade::SetSequenceStartupWaitMode( bool isEnable)
{
  m_isSequenceStartupWaitMode = isEnable;

  if( m_pSequenceViewSystem)
  {
    m_pSequenceViewSystem->SetStartupWaitMode( m_isSequenceStartupWaitMode );
  }
}


GFL_NAMESPACE_END(Fade)
GFL_NAMESPACE_END(AppLib)
