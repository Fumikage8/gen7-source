//===================================================================
/**
 * @file    WeatherEffectManager.cpp
 * @brief   天候エフェクト管理
 * @author  saita_kazuki
 * @date    2015.03.06
 */
//===================================================================

#include "Test/FieldTest/include/Weather/WeatherEffectManager.h"

#include "Test/FieldTest/include/Weather/WeatherEffectShine.h"
#include "Test/FieldTest/include/Weather/WeatherEffectRain.h"
#include "Test/FieldTest/include/FieldEffectManager.h"

// デバッグ
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

// メモリ
#include <gfx/include/gfl2_GLMemory.h>

// @todo  // @todo パスへの登録に使うが・・・
#include "Test/FieldTest/include/FieldTestMyRenderingPipeLine.h" // @todo field用に置き換える 150215 ikuta_junya
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Weather )


// 更新処理テーブル
const WeatherEffectManager::UpdateFunc WeatherEffectManager::m_UpdateFunc[ STATE_MAX ] =
{
  &WeatherEffectManager::UpdateFunc_None,
  &WeatherEffectManager::UpdateFunc_Initialize,
  &WeatherEffectManager::UpdateFunc_Load,
  &WeatherEffectManager::UpdateFunc_UpdateIn,
  &WeatherEffectManager::UpdateFunc_UpdateMain,
  &WeatherEffectManager::UpdateFunc_UpdateOut,
  &WeatherEffectManager::UpdateFunc_Terminate,
  &WeatherEffectManager::UpdateFunc_Restart,
};


/**
 * @brief   コンストラクタ
 * @param   pSystemHeap インスタンス生成などに使用するヒープ
 * @param   pGLAllocator リソース確保用アロケータ
 * @param   pEffectManager フィールドエフェクト管理クラス
 */
WeatherEffectManager::WeatherEffectManager( gfl2::heap::HeapBase* pSystemHeap, gfl2::gfx::IGLAllocator* pGLAllocator, Field::Effect::Manager* pEffectManager) :
  m_seq( 0),
  m_fps( 30.0f),
  m_nowWeatherType( TYPE_NONE),
  m_nextWeatherType( TYPE_NONE),
  m_nowEffectState( STATE_NONE),
  m_nextEffectState( STATE_NONE),
  m_requestType( TYPE_NONE),
  m_changeTime( 0.0f),
  m_pEffect(),
  m_pHeap( pSystemHeap),
  m_pGLAllocator( pGLAllocator),
  m_pEffectManager( pEffectManager)
{
}

/**
 * @brief   デストラクタ
 */
WeatherEffectManager::~WeatherEffectManager()
{
}

/**
 * @brief   エフェクト変更リクエスト
 * @param   requestType 変更したい天候
 * @param   changeTime 遷移にかける時間(秒)
 */
void WeatherEffectManager::Request( Type requestType, f32 changeTime)
{
  m_requestType = requestType;
  m_changeTime = changeTime;
}

/**
 * @brief   生成
 */
void WeatherEffectManager::Create()
{
  m_pEffectManager->CreateResource( Field::Effect::Manager::RESOURCE_ID_WEATHER);
  m_pEffectManager->CreateModelResource( Field::Effect::Manager::RESOURCE_ID_WEATHER);

  m_pEffect[ TYPE_SHINE ] = GFL_NEW( m_pHeap) WeatherEffectShine( m_pHeap, m_pGLAllocator, m_pEffectManager);
  m_pEffect[ TYPE_RAIN  ] = GFL_NEW( m_pHeap) WeatherEffectRain( m_pHeap, m_pGLAllocator, m_pEffectManager);
}

/**
 * @brief   破棄
 * @retval  true  破棄完了
 * @retval  false 破棄実行中
 */
b8 WeatherEffectManager::Destroy()
{
  enum
  {
    TERMINATE_EFFECT,
    DESTROY_RESOURCE,
    FINISH,
  };

  b8 isFinish = true;

  switch( m_seq)
  {
  case TERMINATE_EFFECT:
    for( u32 i = 0; i < TYPE_MAX; ++i)
    {
      if( m_pEffect[i]->Terminate())
      {
        GFL_SAFE_DELETE( m_pEffect[i]);
      }
      else
      {
        isFinish = false;
      }
    }

    //if( isFinish)
    {
      m_seq++;
    }
    //break;

  case DESTROY_RESOURCE:
    if( m_pEffectManager->DestroyResource( Field::Effect::Manager::RESOURCE_ID_WEATHER) &&
        m_pEffectManager->DestroyModelResource( Field::Effect::Manager::RESOURCE_ID_WEATHER)
      )
    {
      m_seq++;
    }
    //break;

  case FINISH:
    return true;
    // break;

  default:
    GFL_ASSERT( 0);
    break;
  };

  return false;
}

/**
 * @brief   初期化
 * @param   desc 設定情報
 */
void WeatherEffectManager::Initialzie( const Description& desc)
{
  m_fps = desc.fps;
}

/**
 * @brief   更新
 */
void WeatherEffectManager::Update()
{
  // リクエスト処理
  this->UpdateRequest();

  // エフェクト処理
  this->UpdateEffect();
}

/**
 * @brief   描画
 */
void WeatherEffectManager::Draw()
{
}

/**
 * @brief   カメラ更新後に行う更新
 * @param   cameraPos カメラ位置
 */
void WeatherEffectManager::UpdateAfterCameraUpdate( gfl2::math::Vector3& cameraPos)
{
  if( this->IsEffectFinishInitialize( m_nowWeatherType, m_nowEffectState))
  {
    if( m_pEffect[m_nowWeatherType])
    {
      m_pEffect[m_nowWeatherType]->UpdateAfterCameraUpdate( cameraPos);
    }
  }
  if( this->IsEffectFinishInitialize( m_nextWeatherType, m_nextEffectState))
  {
    if( m_pEffect[m_nextWeatherType])
    {
      m_pEffect[m_nextWeatherType]->UpdateAfterCameraUpdate( cameraPos);
    }
  }
}

/**
 * @brief   自機位置の更新
 * @param   playerPos
 * @todo 仮対応
 */
void WeatherEffectManager::UpdatePlayerPos( gfl2::math::Vector3& playerPos)
{
  if( this->IsEffectFinishInitialize( m_nowWeatherType, m_nowEffectState))
  {
    if( m_pEffect[m_nowWeatherType])
    {
      m_pEffect[m_nowWeatherType]->UpdatePlayerPos( playerPos);
    }
  }
  if( this->IsEffectFinishInitialize( m_nextWeatherType, m_nextEffectState))
  {
    if( m_pEffect[m_nextWeatherType])
    {
      m_pEffect[m_nextWeatherType]->UpdatePlayerPos( playerPos);
    }
  }
}

/**
 * @brief   リクエスト更新
 */
void WeatherEffectManager::UpdateRequest()
{
  if( m_requestType != TYPE_NONE)
  {
    if( m_nowWeatherType == TYPE_NONE && m_nextWeatherType == TYPE_NONE)
    {
      // 空ならリクエストをすぐ採用
      this->SetNowWeatherType( m_requestType);
      this->SetNowEffectState( STATE_INITIALIZE);
      m_requestType = TYPE_NONE;
    }
    else
    {
      if( m_nextWeatherType != TYPE_NONE)
      {
        return; // 切り替えが済んでいないので処理しない
      }
      if( m_nowEffectState < STATE_UPDATE_MAIN)
      {
        return; // 現在の状態がメイン処理まで来ていないので処理しない
      }
      if( m_requestType == m_nowWeatherType)
      {
        // リクエストが同じ天候なので破棄
        m_requestType = TYPE_NONE;
        return;
      }

      // 現在のエフェクトを終了させ始める
      this->SetNowEffectState( STATE_UPDATE_OUT);

      // 次のエフェクトの準備に入らせる
      this->SetNextWeatherType( m_requestType);
      this->SetNextEffectState( STATE_INITIALIZE);
    }
  }
}

/**
 * @brief   エフェクト更新
 */
void WeatherEffectManager::UpdateEffect()
{
  // 現在の天候エフェクトを更新
  EffectState nowState = (this->*m_UpdateFunc[ m_nowEffectState ])( m_nowWeatherType);
  this->SetNowEffectState( nowState);

  if( m_nextEffectState != STATE_NONE)
  {
    // 次の天候エフェクトを更新
    EffectState nextState = (this->*m_UpdateFunc[ m_nextEffectState ])( m_nextWeatherType);
    this->SetNextEffectState( nextState);

    if( this->IsSwitchEffect())
    {
      // スイッチ可能になったら入れ替える
      this->SwitchEffectTypeAndState();
    }
  }
}

/**
 * @brief   なし
 * @param   type 天候の種類
 */
WeatherEffectManager::EffectState WeatherEffectManager::UpdateFunc_None( Type type)
{
  // 何もしない
  return STATE_NONE;
}

/**
 * @brief   初期化
 * @param   type 天候の種類
 */
WeatherEffectManager::EffectState WeatherEffectManager::UpdateFunc_Initialize( Type type)
{
  // @todo 何をさせるかは未定
  m_pEffect[type]->Initialize();
  return STATE_LOAD;
}

/**
 * @brief   読み込み
 * @param   type 天候の種類
 */
WeatherEffectManager::EffectState WeatherEffectManager::UpdateFunc_Load( Type type)
{
  if( m_pEffect[type]->Load())
  {

    for( u32 i = 0; i < WeatherEffectBase::EFFECT_UNDER_MAX; ++i)
    {
      gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstanceNode = m_pEffect[type]->GetEffectModelInstanceNode( i);
      if( pModelInstanceNode)
      {
        m_pRenderingPipeLine->AddEdgeRenderingTarget( pModelInstanceNode);
      }
    }

    return STATE_UPDATE_IN;
  }
  return STATE_LOAD;
}

/**
 * @brief   開始演出
 * @param   type 天候の種類
 */
WeatherEffectManager::EffectState WeatherEffectManager::UpdateFunc_UpdateIn( Type type)
{
  f32 intervalFrameTime = this->CalculateIntervalTimeFrameTime();
  if( m_pEffect[type]->UpdateIn( intervalFrameTime, m_changeTime))
  {
    return STATE_UPDATE_MAIN;
  }
  return STATE_UPDATE_IN;
}

/**
 * @brief   演出メイン
 * @param   type 天候の種類
 */
WeatherEffectManager::EffectState WeatherEffectManager::UpdateFunc_UpdateMain( Type type)
{
  m_pEffect[type]->UpdateMain();
  return STATE_UPDATE_MAIN;
}

/**
 * @brief   終了演出
 * @param   type 天候の種類
 */
WeatherEffectManager::EffectState WeatherEffectManager::UpdateFunc_UpdateOut( Type type)
{
  f32 intervalFrameTime = this->CalculateIntervalTimeFrameTime();
  if( m_pEffect[type]->UpdateOut( intervalFrameTime, m_changeTime))
  {

    for( u32 i = 0; i < WeatherEffectBase::EFFECT_UNDER_MAX; ++i)
    {
      gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstanceNode = m_pEffect[type]->GetEffectModelInstanceNode( i);
      if( pModelInstanceNode)
      {
        pModelInstanceNode->SetVisible( false);
        m_pRenderingPipeLine->RemoveEdgeRenderingTarget( pModelInstanceNode);
      }
    }

    return STATE_TERMINATE;
  }
  return STATE_UPDATE_OUT;
}

/**
 * @brief   破棄
 * @param   type 天候の種類
 */
WeatherEffectManager::EffectState WeatherEffectManager::UpdateFunc_Terminate( Type type)
{
  if( m_pEffect[type]->Terminate())
  {
    return STATE_NONE;
  }
  return STATE_TERMINATE;
}

/**
 * @brief   再開
 * @param   type 天候の種類
 */
WeatherEffectManager::EffectState WeatherEffectManager::UpdateFunc_Restart( Type type)
{
  m_pEffect[type]->Restart();
  return STATE_UPDATE_MAIN;
}

/**
 * @brief   エフェクトの種類、状態を現在のものから次のものに切り替える
 */
void WeatherEffectManager::SwitchEffectTypeAndState()
{
  this->SetNowWeatherType( m_nextWeatherType);
  this->SetNowEffectState( m_nextEffectState);
  this->SetNextWeatherType( TYPE_NONE);
  this->SetNextEffectState( STATE_NONE);
}

/**
 * @brief   エフェクトを現在のものから次のものに切り替えても良いか
 * @retval  true 切り替え可 (現在…終了、次…演出メイン処理)
 * @retval  false 切り替え不可 (まだ切り替える段階ではない)
 */
b8 WeatherEffectManager::IsSwitchEffect()
{
  return ( m_nowEffectState == STATE_NONE && m_nextEffectState == STATE_UPDATE_MAIN );
}

/**
 * @brief   エフェクトの準備が完了しているか
 * @param   type 天候種類
 * @param   state エフェクト更新状態
 * @retval  true 完了
 * @retval  false 初期化中
 */
b8 WeatherEffectManager::IsEffectFinishInitialize( Type type, EffectState state)
{
  return ( type != TYPE_NONE && state >= STATE_UPDATE_IN );
}

/**
 * @brief   現在の天候の種類を変更
 */
void WeatherEffectManager::SetNowWeatherType( Type type)
{
  if( m_nowWeatherType != type)
  {
    m_nowWeatherType = type;
  }
}

/**
 * @brief   次の天候の種類を変更
 */
void WeatherEffectManager::SetNextWeatherType( Type type)
{
  if( m_nextWeatherType != type)
  {
    m_nextWeatherType = type;
  }
}

/**
 * @brief   現在のエフェクトの状態を変更
 */
void WeatherEffectManager::SetNowEffectState( EffectState state)
{
  if( m_nowEffectState != state)
  {
    m_nowEffectState = state;
  }
}

/**
 * @brief   次のエフェクトの状態を変更
 */
void WeatherEffectManager::SetNextEffectState( EffectState state)
{
  if( m_nextEffectState != state)
  {
    m_nextEffectState = state;
  }
}

/**
 * @brief   次のエフェクトの状態を変更
 * @return  1Syncで進行する時間
 */
f32 WeatherEffectManager::CalculateIntervalTimeFrameTime()
{
  return ( 1.0f / m_fps );
}


GFL_NAMESPACE_END( Weather )
GFL_NAMESPACE_END( Field )

