//======================================================================
/**
 * @file    ResortCharaModel.cpp
 * @date    2016/03/04 10:54:21
 * @author  fukushima_yuya
 * @brief   ポケリゾート：管理人モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "ResortCharaModel.h"

#include <AppLib/include/Util/app_util_Heap.h>
#include <System/include/GflUse.h>

//gflib2
#include <fs/include/gfl2_fs.h>
#include <model/include/gfl2_BaseCamera.h>
#include <model/include/gfl2_CharaModelFactory.h>

// arc
#include <arc_def_index/arc_def.h>
#include <niji_conv_header/battle/chara/chara_model_id.h>
#include <niji_conv_header/battle/chara/tr0007_00_resort.h>

#include "../ResortDebugInfo.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace
{
  // キャラアニメシーケンス
  enum {
    CHARA_ANIM_SEQ_IDLE,
    CHARA_ANIM_SEQ_START,
    CHARA_ANIM_SEQ_LOOP,
    CHARA_ANIM_SEQ_END,
  };
  // 口パクアニメシーケンス
  enum {
    MOUTH_ANIM_SEQ_IDLE,                //!< 待機
    MOUTH_ANIM_SEQ_OPEN_BEGIN,          //!< 開く：設定
    MOUTH_ANIM_SEQ_OPEN_END,            //!< 開く：待機
    MOUTH_ANIM_SEQ_HALF_CLOSE_BEGIN,    //!< 半開き：設定
    MOUTH_ANIM_SEQ_HALF_CLOSE_END,      //!< 半開き：待機
    MOUTH_ANIM_SEQ_CLOSE_BEGIN,         //!< 閉じる：設定
    MOUTH_ANIM_SEQ_CLOSE_END,           //!< 閉じる：待機
    MOUTH_ANIM_SEQ_HALF_OPEN_BEGIN,     //!< 半開き：設定
    MOUTH_ANIM_SEQ_HALF_OPEN_END,       //!< 半開き：待機
  };
}


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
 //------------------------------------------------------------------------------
ResortCharaModel::ResortCharaModel( void )
  : m_heap(NULL)
  , m_modelAllocator(NULL)
  , m_renMan(NULL)
  , m_modelFactory(NULL)
  , m_modelFactoryHeap(NULL)
  , m_charaModel(NULL)
  , m_setupParam()
  , m_arcID(0)
  , m_terminate(false)
  , m_seqMain(0)
  , m_seqSub(0)
  , m_seqDel(0)
  , m_charaID(-1)
  , m_loadCharaID(-1)
  , m_autoAnimSeq(0)
  , m_autoAnimFrame(0)
  , m_autoAnimInterval(0)
  , m_stopMouthAnim(false)
  , m_charaAnimSeq(0)
  , m_cameraEnvNode(NULL)
  , m_baseCamera(NULL)
  , m_light(NULL)
  , m_position()
{
  ;
}



//------------------------------------------------------------------------------
/**
 * @brief   初期化
 *
 * @param   param   セットアップパラメータ
 */
//------------------------------------------------------------------------------
void ResortCharaModel::Initialize( SETUP_PARAM& param )
{
  m_arcID       = param.arcID;
  m_renMan      = param.renMan;
  m_setupParam  = param;

  // 専用ヒープの生成
  {
    gfl2::heap::HeapBase* heap = param.heap->GetDeviceHeap();
    {
      // 専用ヒープ
      {
        const u32 heapSize = 1 * 1024 * 1024;
        m_heap = GFL_CREATE_LOCAL_HEAP( heap, heapSize, gfl2::heap::HEAP_TYPE_EXP, false );
      }

      // モデルファクトリ用ヒープの生成
      {
        const u32 heapSize = 0xC0000; // app::tool::CharaSimpleModel::ONE_MODEL_BIN_HEAP_SIZE
        m_modelFactoryHeap = GFL_CREATE_LOCAL_HEAP( heap, heapSize, gfl2::heap::HEAP_TYPE_EXP, false );
      }
    }
  }

  // カメラ
  {
    // デフォルトカメラの生成
    app::util::AppRenderingManager::CreateDefaultCamera( param.heap, &m_cameraEnvNode, &m_baseCamera);
    // アスペクトの設定
    m_baseCamera->SetAspectRatio( 400.0f / 240.0f );
    // カメラのセット
    m_renMan->SetCamera( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_baseCamera );
  }

  // 上画面用ライト
  {
    m_light = app::util::AppRenderingManager::CreateDefaultDrawEnv( param.heap, app::util::AppRenderingManager::PresetType::TRAINER );
    m_light->GetLightSet(2)->GetLightNode(0)->SetColor( gfl2::math::Vector3( 1.0f*0.85f, 1.0f*0.85f, 1.0f*0.85f ) );
    m_light->GetLightSet(2)->GetLightNode(1)->SetColor( gfl2::math::Vector3( 0.2f*0.85f, 0.2f*0.85f, 0.2f*0.85f ) );
    m_renMan->AddDrawEnv( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_light );
  }

  {
    // モデル用アロケータの生成
    m_modelAllocator = GFL_NEW(m_heap) gfl2::util::GLHeapAllocator( m_heap );

    // モデルファクトリの生成
    m_modelFactory = GFL_NEW(m_heap) poke_3d::model::CharaModelFactory();
    {
      GFL_ASSERT( m_modelFactory );

#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTime(ResortDebugInfo::TIME_PHASE_RESORT_CHARA_FILE_OPEN_START);
#endif
      m_modelFactory->Initialize( param.afm, m_modelFactoryHeap, m_arcID );
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTime(ResortDebugInfo::TIME_PHASE_RESORT_CHARA_FILE_OPEN_END);
#endif
    }

    // キャラクタモデルの生成
    m_charaModel = GFL_NEW(m_heap) poke_3d::model::CharaModel();
    {
      GFL_ASSERT( m_charaModel );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
void ResortCharaModel::Terminate( void )
{
  GFL_SAFE_DELETE( m_charaModel );

  GFL_SAFE_DELETE( m_modelFactory );

  GFL_SAFE_DELETE( m_modelAllocator );
      
  GFL_SAFE_DELETE( m_light );

  GFL_SAFE_DELETE( m_cameraEnvNode );

  GFL_SAFE_DELETE( m_baseCamera );

  GFL_DELETE_HEAP( m_modelFactoryHeap );

  GFL_DELETE_HEAP( m_heap );
}

//------------------------------------------------------------------------------
/**
 * @brief   破棄可能か？
 *
 * @return  "true  == 破棄可"
 * @return  "false == 破棄不可"
 */
//------------------------------------------------------------------------------
bool ResortCharaModel::CanTerminate( void )
{
  Update();

  if( m_seqDel == 0 )
  {
    StopMotion();

    m_terminate = true;
    m_seqDel++;
  }
  else if( m_seqDel == 1 )
  {
    if( m_seqMain == 0 )
    {
      m_modelFactory->Finalize();

      m_seqDel++;
    }
  }
  else if( m_seqDel == 2 )
  {
    m_renMan->RemoveDrawEnv( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_light );
    m_renMan->UnsetCamera( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY );

    m_seqDel++;
  }
  else {
    return true;
  }

  return false;
}



//------------------------------------------------------------------------------
/**
 * @brief   ロードリクエスト
 *
 * @param   charaID   キャラID
 */
//------------------------------------------------------------------------------
void ResortCharaModel::RequestLoad( const s32 charaID )
{
  if( m_terminate )
  {
    GFL_ASSERT(0);
    return;
  }

  GFL_ASSERT( charaID != 0xFFFFFFFFF );

  m_loadCharaID = charaID;
}


//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void ResortCharaModel::Update( void )
{
  if( m_seqMain == 0 )
  {
    if( !m_terminate )
    {
      if( m_loadCharaID != -1 )
      {
        m_charaID = m_loadCharaID;
        m_loadCharaID = -1;

        m_seqSub = 0;
        m_seqMain = 1;  //!< ロード
      }
    }
  }

  if( m_seqMain == 1 )
  {
    if( m_seqSub == 0 )
    {
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTime(ResortDebugInfo::TIME_PHASE_RESORT_CHARA_FILE_LOAD_START);
#endif
      m_modelFactory->LoadModelAsync( m_charaID );

      m_seqSub++;
    }
    else if( m_seqSub == 1 )
    {
      if( m_modelFactory->IsModelLoaded( m_charaID ) )
      {
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
        ResortDebugInfo::s_ResortDebugInfo.SetCurrTime(ResortDebugInfo::TIME_PHASE_RESORT_CHARA_FILE_LOAD_END);
#endif
        if( !m_modelFactory->SetupModel( m_modelAllocator, m_charaID ) )
        {
          GFL_ASSERT(0);
        }
        if( !m_modelFactory->CreateModel( m_modelAllocator, m_heap, m_charaModel, m_charaID ) ) 
        {
          GFL_ASSERT(0);
        }

        // レンダリングマネージャに登録
        for( u32 i=0; i<GFL_NELEMS(m_setupParam.placeType); ++i )
        {
          app::util::AppRenderingManager::RenderPlaceType::Tag placeType = m_setupParam.placeType[i];
          if( placeType == app::util::AppRenderingManager::RenderPlaceType::NUM ) continue;

          m_renMan->AddCharaModel( placeType, m_charaModel );
        }

        // 破棄ステートへ
        if( m_terminate || m_loadCharaID != -1 )
        {
          m_seqSub = 0;
          m_seqMain = 3;  //!< 破棄
        }
        else {
          m_charaModel->SetVisible( false );
          //          m_charaModel->SetPosition( 0.0f, -150.0f, 850.0f );
          m_charaModel->SetPosition( 0.0f, -120.0f, 700.0f );
          m_charaModel->ChangeAnimation( TR0007_00_BA030_RESORTWAIT01 );
          m_charaModel->SetAnimationLoop( true );
          m_charaModel->SetEyeIndex( poke_3d::model::CharaModel::EYE_INDEX_NONE );

          poke_3d::model::CharaModel::AutoBlinkMode autoBlinkMode =
            poke_3d::model::CharaModel::AUTO_BLINK_MODE_BATTLE;
          m_charaModel->SetAutoBlinkMode( autoBlinkMode );
          m_charaModel->SetupRandomFuc( System::GflUse::GetPublicRand );

          m_seqSub = 0;
          m_seqMain = 2;  //!< 更新
        }
      }
    }
  }

  if( m_seqMain == 2 )
  {
    if( m_terminate || m_loadCharaID != -1 )
    {
      m_seqSub = 0;
      m_seqMain = 3;  //!< 破棄
    }
    else {
      UpdateAnimChara();
      UpdateAnimMouth();

      m_charaModel->UpdateEye();
      m_charaModel->UpdateMouth();
      m_charaModel->UpdateAnimation();
    }
  }

  if( m_seqMain == 3 )
  {
    if( m_seqSub == 0 )
    {
      // レンダリングマネージャの登録解除
      for( u32 i=0; i<GFL_NELEMS(m_setupParam.placeType); ++i )
      {
        app::util::AppRenderingManager::RenderPlaceType::Tag placeType = m_setupParam.placeType[i];
        if( placeType == app::util::AppRenderingManager::RenderPlaceType::NUM ) continue;

        m_renMan->RemoveCharaModel( placeType, m_charaModel );
      }

      m_seqSub++;
    }
    else if( m_seqSub == 1 )
    {
      if( m_charaModel->CanDestroy() )
      {
        m_charaModel->Destroy();

        GFL_SAFE_DELETE( m_charaModel );

        m_seqSub++;
      }
    }
    else if( m_seqSub == 2 )
    {
      if( m_modelFactory->CanUnloadModel( m_charaID ) )
      {
        m_modelFactory->UnloadModel( m_charaID );

        m_seqSub++;
      }
    }
    else if( m_seqSub == 3 )
    {
      m_seqMain = 0;
      m_seqSub  = 0;  //!< 待機
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   表示切替
 *
 * @param   visible   表示フラグ
 */
//------------------------------------------------------------------------------
void ResortCharaModel::SetVisible( bool visible )
{
  if( m_charaModel )
  {
    if( m_seqMain == 2 )
    {
      m_charaModel->SetVisible( visible );
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   表示切替
 *
 * @param   visible   表示フラグ
 */
//------------------------------------------------------------------------------
void ResortCharaModel::SetPosition( gfl2::math::Vector3 pos )
{
  m_position = pos;

  if( !m_charaModel ) return;
  if( m_seqMain != 2 ) return;

  m_charaModel->SetPosition( m_position );
}


//------------------------------------------------------------------------------
/**
 * @brief   モーションの再生
 *
 * @param   animID    アニメID
 * @param   loop      ループフラグ
 */
//------------------------------------------------------------------------------
void ResortCharaModel::PlayMotion( const u32 animID, const bool loop )
{
  if( !m_charaModel ) return;
  if( m_seqMain != 2 ) return;

  m_charaModel->ChangeAnimation( animID );
  m_charaModel->SetAnimationFrame( 0.0f );
  m_charaModel->SetAnimationLoop( loop );
}

//------------------------------------------------------------------------------
/**
 * @brief   モーションの停止
 */
//------------------------------------------------------------------------------
void ResortCharaModel::StopMotion( void )
{
  if( !m_charaModel ) return;
  if( m_seqMain != 2 ) return;

  m_charaModel->SetAnimationStepFrame( 0.0f );
}



//------------------------------------------------------------------------------
/**
 * @brief   キャラアニメの更新
 */
//------------------------------------------------------------------------------
void ResortCharaModel::UpdateAnimChara( void )
{
  switch( m_charaAnimSeq )
  {
  case CHARA_ANIM_SEQ_START:
    {
      if( m_charaModel->IsAnimationLastFrame() )
      {
        PlayMotion( TR0007_00_BA032_RESORTSPEAK01_LOOP, true );

        m_charaAnimSeq = CHARA_ANIM_SEQ_LOOP;
      }
    } break;

  case CHARA_ANIM_SEQ_END:
    {
      // @fix GFNMCat[3111]
      //if( m_charaModel->IsAnimationLastFrame() )
      {
        m_charaAnimSeq = CHARA_ANIM_SEQ_IDLE;
      }
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   口パクアニメの更新
 */
//------------------------------------------------------------------------------
void ResortCharaModel::UpdateAnimMouth( void )
{
  const u32 ONE_STATE_FRAME = 2;
  const u32 INTERVAL_FRAME  = 4;

  switch( m_autoAnimSeq )
  {
  case MOUTH_ANIM_SEQ_OPEN_BEGIN: //!< 開ける
    {
      m_charaModel->SetMouthIndex( poke_3d::model::CharaModel::MOUTH_INDEX_OPEN );

      m_autoAnimSeq   = MOUTH_ANIM_SEQ_OPEN_END;
      m_autoAnimFrame = 0;

      if( m_charaModel->isSetupRandomFunc() )
      {
        m_autoAnimInterval = m_charaModel->GetRandomValue(INTERVAL_FRAME) + ONE_STATE_FRAME;
      }
      else {
        m_autoAnimInterval = INTERVAL_FRAME;
      }
    } // no break;

  case MOUTH_ANIM_SEQ_OPEN_END: //!< 開ける
    {
      m_autoAnimFrame++;

      if( m_autoAnimFrame > m_autoAnimInterval )
      {
        m_autoAnimSeq   = MOUTH_ANIM_SEQ_HALF_CLOSE_BEGIN;
        m_autoAnimFrame = 0;
      }
    } break;

  case MOUTH_ANIM_SEQ_HALF_CLOSE_BEGIN: //!< 半開け
    {
      m_charaModel->SetMouthIndex( poke_3d::model::CharaModel::MOUTH_INDEX_HALF_OPEN );

      m_autoAnimSeq      = MOUTH_ANIM_SEQ_HALF_CLOSE_END;
      m_autoAnimFrame    = 0;
      m_autoAnimInterval = ONE_STATE_FRAME;
    } // no break;

  case MOUTH_ANIM_SEQ_HALF_CLOSE_END: //!< 半開け
    {
      m_autoAnimFrame++;

      if( m_autoAnimFrame > m_autoAnimInterval )
      {
        m_autoAnimSeq   = MOUTH_ANIM_SEQ_CLOSE_BEGIN;
        m_autoAnimFrame = 0;
      }
    } break;

  case MOUTH_ANIM_SEQ_CLOSE_BEGIN: //!< 閉じる
    {
      m_charaModel->SetMouthIndex( poke_3d::model::CharaModel::MOUTH_INDEX_CLOSE );

      m_autoAnimSeq      = MOUTH_ANIM_SEQ_CLOSE_END;
      m_autoAnimFrame    = 0;
      m_autoAnimInterval = ONE_STATE_FRAME;
    } // no break;

  case MOUTH_ANIM_SEQ_CLOSE_END: //!< 閉じる
    {
      if( !m_stopMouthAnim )
      {
        m_autoAnimFrame++;

        if( m_autoAnimFrame > m_autoAnimInterval )
        {
          m_autoAnimSeq   = MOUTH_ANIM_SEQ_HALF_OPEN_BEGIN;
          m_autoAnimFrame = 0;
        }
      }
    } break;

  case MOUTH_ANIM_SEQ_HALF_OPEN_BEGIN: //!< 半開け
    {
      m_charaModel->SetMouthIndex( poke_3d::model::CharaModel::MOUTH_INDEX_HALF_OPEN );

      m_autoAnimSeq      = MOUTH_ANIM_SEQ_HALF_OPEN_END;
      m_autoAnimFrame    = 0;
      m_autoAnimInterval = ONE_STATE_FRAME;
    } // no break;

  case MOUTH_ANIM_SEQ_HALF_OPEN_END:
    {
      m_autoAnimFrame++;

      if( m_autoAnimFrame > m_autoAnimInterval )
      {
        m_autoAnimSeq   = MOUTH_ANIM_SEQ_OPEN_BEGIN;  //!< 最初へ
        m_autoAnimFrame = 0;
      }
    } break;
  }
}




//==============================================================================
/**
 *      ResortCharaModelListener
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   キャラアニメの開始リクエスト
 */
//------------------------------------------------------------------------------
void ResortCharaModel::RequestStartCharaAnim( void )
{
  if( m_charaAnimSeq == CHARA_ANIM_SEQ_IDLE )
  {
    FUKUSHIMA_PRINT( "RequestStartCharaAnim\n" );

    // アニメの再生
    PlayMotion( TR0007_00_BA031_RESORTSPEAK01_START, false );

    m_charaAnimSeq = CHARA_ANIM_SEQ_START;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   キャラアニメの終了リクエスト
 */
//------------------------------------------------------------------------------
void ResortCharaModel::RequestEndCharaAnim( void )
{
  FUKUSHIMA_PRINT( "RequestEndCharaAnim\n" );

  m_charaAnimSeq = CHARA_ANIM_SEQ_END;

  // アニメの再生
  PlayMotion( TR0007_00_BA033_RESORTSPEAK01_END, false );
}

//------------------------------------------------------------------------------
/**
 * @brief   口パクの開始リクエスト
 */
//------------------------------------------------------------------------------
void ResortCharaModel::RequestStartMouthAnim( void )
{
  FUKUSHIMA_PRINT( "RequestStartMouthAnim\n" );

  m_stopMouthAnim = false;

  if( m_autoAnimSeq == MOUTH_ANIM_SEQ_IDLE )
  {
    m_autoAnimSeq = MOUTH_ANIM_SEQ_OPEN_BEGIN;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   口を閉じるリクエスト
 */
//------------------------------------------------------------------------------
void ResortCharaModel::RequestCloseMouth( void )
{
  if( !m_stopMouthAnim )
  {
    FUKUSHIMA_PRINT( "RequestCloseMouth\n" );

    m_stopMouthAnim = true;
  }
}

GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)
