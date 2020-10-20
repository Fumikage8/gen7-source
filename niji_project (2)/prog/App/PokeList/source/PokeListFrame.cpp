//==============================================================================
/**
 * @file    PokeListFrame.cpp
 * @author  fukushima_yuya
 * @date    2015.03.03
 * @brief   ポケモンリスト画面フレーム
 */
//==============================================================================

// niji
#include "App/PokeList/include/PokeListProc.h"
#include "App/PokeList/source/PokeListDraw.h"
#include "App/PokeList/source/PokeListFrame.h"
#include "App/PokeList/source/work/PokeListLocalWork.h"
#include "App/PokeList/source/work/PokeListStaticWork.h"

#include <App/Bag/include/BagFrame.h>
#include <AppLib/include/Tool/AppToolItemEffect.h>
#include <AppLib/include/Util/AppCamera.h>
#include <GameSys/include/GameProcManager.h>

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/common_pokelist.gaix>
#include <niji_conv_header/message/msg_pokelist.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
PokeListFrame::PokeListFrame( void )
  : m_pPokeListDraw( NULL )
  , m_pItemEffect(NULL)
  , m_pAppCamera(NULL)
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
PokeListFrame::~PokeListFrame( void )
{
  ;
}



//------------------------------------------------------------------------------
/**
 * @brief   PokeListのセットアップ
 */
//------------------------------------------------------------------------------
void PokeListFrame::SetupPokeList( void )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   PokeListDrawの生成
 */
//------------------------------------------------------------------------------
void PokeListFrame::CreatePokeList( void )
{
  // フォルムチェンジエフェクトの生成
  CreateItemEffect();

  // AppCameraの生成
  CreateAppCamera();

  gfl2::heap::HeapBase * heap = m_pAppHeap->GetDeviceHeap();
  {
    m_pPokeListDraw = GFL_NEW(heap) PokeListDraw( m_pAppHeap, m_pAppRenderingManager, m_pItemEffect );
    {
      GFL_ASSERT( m_pPokeListDraw );

      m_pPokeListDraw->SetTitleViewUIListener( this );
      m_pPokeListDraw->Init( m_pAppParam, m_pArcReadBuff, m_pStaticWork, m_pLocalWork );

      this->SetBaseDraw( m_pPokeListDraw );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   PokeListDrawの破棄
 */
//------------------------------------------------------------------------------
bool PokeListFrame::EndPokeList( void )
{
  if( m_pPokeListDraw )
  {
    if( !m_pPokeListDraw->End() ) return false;

    GFL_SAFE_DELETE( m_pPokeListDraw );
  }

  // AppCameraの破棄
  DeleteAppCamera();

  // フォルムチェンジエフェクトの破棄
  DeleteItemEffect();

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   PokeListDrawの更新
 */
//------------------------------------------------------------------------------
void PokeListFrame::UpdatePokeList( void )
{
  if( m_pPokeListDraw ) m_pPokeListDraw->UpdateTree();

  if( m_pItemEffect )
  {
    m_pItemEffect->Update();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   PokeListDrawの描画
 */
//------------------------------------------------------------------------------
void PokeListFrame::DrawPokeList( gfl2::gfx::CtrDisplayNo no )
{
  if( m_pPokeListDraw ) m_pPokeListDraw->DrawTree( no );
}



//------------------------------------------------------------------------------
/**
 * @brief   フォルムチェンジエフェクトの生成
 *
 * @return  "true  = 完了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool PokeListFrame::CreateItemEffect( void )
{
  gfl2::heap::HeapBase* heap = m_pAppHeap->GetDeviceHeap();
  {
    m_pItemEffect = GFL_NEW(heap) App::Tool::ItemEffect( m_pAppHeap, m_pAppRenderingManager, app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY );
    {
      GFL_ASSERT( m_pItemEffect );

      // 初期化
      m_pItemEffect->Initialize( true );
    }
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   フォルムチェンジエフェクトの破棄
 *
 * @return  "true  = 完了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool PokeListFrame::DeleteItemEffect( void )
{
  if( m_pItemEffect )
  {
    m_pItemEffect->Terminate( true );

    GFL_SAFE_DELETE( m_pItemEffect );
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   AppCameraの生成
 *
 * @return  "true  = 完了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool PokeListFrame::CreateAppCamera( void )
{
  gfl2::heap::HeapBase* heap = m_pAppHeap->GetDeviceHeap();
  {
    m_pAppCamera = GFL_NEW(heap) app::util::AppCamera();
    {
      GFL_ASSERT( m_pAppCamera );

      m_pAppCamera->SetViewMatrix( gfl2::math::Vector3(0.0f,0.0f,80.0f), gfl2::math::Vector3(0.0f,0.0f,0.0f), 0.0f );
      m_pAppCamera->SetOrthoProjectionMatrix( 320.0f/240.0f, 240.0f, 0.01f, 1000.0f, true );
      m_pAppRenderingManager->SetEffectAppCamera( app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pAppCamera );
    }
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   AppCameraの破棄
 *
 * @return  "true  = 完了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool PokeListFrame::DeleteAppCamera( void )
{
  if( m_pAppCamera )
  {
    m_pAppRenderingManager->UnsetEffectAppCamera( app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY );

    GFL_SAFE_DELETE( m_pAppCamera );
  }

  return true;
}





//==============================================================================
/**
 *          PokeListDrawListener
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ポケモンリストの終了
 */
//------------------------------------------------------------------------------
void PokeListFrame::PokeListEnd( void )
{
  m_pContext->pokelist_wait = true;

  m_pStaticWork->SyncPokeOrder();

  SetSubSeq( MM_SEQ_END );
}

//------------------------------------------------------------------------------
/**
 * @brief   外部設定パラメータの設定
 * @param   endMode   終了モード
 */
//------------------------------------------------------------------------------
void PokeListFrame::SetupOutData( EndMode endMode )
{
  switch( endMode )
  {
  case END_MODE_SKILLUSE:
    {
      m_pContext->f_waza_wazano = m_pStaticWork->GetFieldWazaNo();

      m_pPokeListDraw->PokeListClose();
    } break;
  }
}


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )
