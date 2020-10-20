/**
* @brief  フィールドカメラ、1制御単位クラス
* @file   FieldCameraUnit.cpp
* @author ikuta_junya
* @data   2015.05.21
*/


// gfl2
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>

// field
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h" // @note フィールド上のデバック定義は可能な限りここに集める


// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );

// プログラムによる初期値、※付きはこの後リソースデータによって変更される可能性がある
static const f32 DEFAULT_PARAM_NEAR( 30.0f );                     // ニア(※)
static const f32 DEFAULT_PARAM_FAR( 30000.0f );                   // ファー(※)
static const f32 DEFAULT_PARAM_ASPECT_RATIO( 400.0f / 240.0f );   // アスペクト比
static const f32 DEFAULT_PARAM_FOVY_RAD( gfl2::math::ConvDegToRad( 35.0f ) ); // 画角


/**
* @brief コンストラクタ
*
* @return 無し
*/ 
CameraUnit::CameraUnit( void ) :

  m_IsUpdate( false )

  // 内部オブジェクト
  ,m_pBaseCamera( NULL )
  ,m_pController( NULL )
  ,m_pDrawEnvNode( NULL )

  // デバッグ用
#if PM_DEBUG
  ,m_IsUseSwap( false )
#endif // PM_DEBUG
{
}

/**
* @brief デストラクタ
*
* @return 無し
*/
CameraUnit::~CameraUnit( void )
{
  Terminate();
}

/**
* @brief 初期化処理
*
* @param  rInitDesc   初期化設定
*
* @return 無し
*/
void CameraUnit::Initialize( const InitializeDescription& rInitDesc )
{
  // カメラノード作成 @note gfl2仕様、カメラアニメーションを行う場合はカメラノードが必要
  {
    m_pDrawEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(rInitDesc.m_pDeviceAllocator);
    gfl2::renderingengine::scenegraph::instance::CameraNode* pCameraNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateCameraNode(rInitDesc.m_pDeviceAllocator);

    //m_pDrawEnvNode->SetNodeName( "DrawEnv1" );
    pCameraNode->SetNodeName( "EnvCamera1" );

    // BaseCamera仕様、カメラノードから初期値を設定するので、カメラノードに初期値を設定
    pCameraNode->SetFar( DEFAULT_PARAM_FAR );
    pCameraNode->SetNear( DEFAULT_PARAM_NEAR );
    pCameraNode->SetAspectRatio( DEFAULT_PARAM_ASPECT_RATIO );
    pCameraNode->SetFovy( DEFAULT_PARAM_FOVY_RAD );
    
    m_pDrawEnvNode->AddCamera( pCameraNode );

    FIELD_DEBUG_VALUE_SET( Debug::DebugTypes::s_CameraFar, DEFAULT_PARAM_FAR );
    FIELD_DEBUG_VALUE_SET( Debug::DebugTypes::s_CameraNear, DEFAULT_PARAM_NEAR );
  }

  // カメラ実体生成
  {
    m_pBaseCamera = GFL_NEW( rInitDesc.m_pSystemHeap ) poke_3d::model::BaseCamera;
    m_pBaseCamera->Create(
      rInitDesc.m_pDeviceAllocator,
      rInitDesc.m_pDeviceHeap,
      m_pDrawEnvNode,
      0
      );
  }

  // その他
  {
    m_IsUpdate = true;
  }
}

/**
* @brief 終了処理
*
* @return 無し
*/
void CameraUnit::Terminate( void )
{
#if PM_DEBUG
  GFL_ASSERT_MSG( m_IsUseSwap == false, "Terminate : SwapController戻し忘れ" );
#endif // PM_DEBUG

  GFL_SAFE_DELETE( m_pController );
  GFL_SAFE_DELETE( m_pBaseCamera );
  GFL_SAFE_DELETE( m_pDrawEnvNode );
  m_IsUpdate = false;
}

/**
* @brief 更新処理
*
* @return 無し
*/
void CameraUnit::Update( void )
{
  if( m_pController && m_IsUpdate )
  {
    m_pController->Update( m_pBaseCamera );
  }
}

/**
* @brief ワールド振動をカメラに適用
*/
void CameraUnit::ApplyGlobalShake(gfl2::math::SpringSimulation& rHorizonSpring, gfl2::math::SpringSimulation& rVerticalSpring)
{
  if (m_pController && m_IsUpdate)
  {
    gfl2::math::Vector3 fixCameraPosition(m_pBaseCamera->GetPosition());
    gfl2::math::Vector3 fixTargetPosition(m_pBaseCamera->GetTargetPosition());
    gfl2::math::Vector3 upvector = m_pBaseCamera->GetUpVector();

    fixCameraPosition.x += rHorizonSpring.GetPostion();
    fixTargetPosition.x += rHorizonSpring.GetPostion();
    fixCameraPosition.y += rVerticalSpring.GetPostion();
    fixTargetPosition.y += rVerticalSpring.GetPostion();

    m_pBaseCamera->SetupCameraLookAt(
      fixCameraPosition,
      fixTargetPosition,
      upvector
      );
  }
}

/**
* @brief 補間を打ち切り、最新の状態に更新する
*
* @return 無し
*/
void CameraUnit::Fix( void )
{
  if( m_pController && m_IsUpdate )
  {
    m_pController->Fix( m_pBaseCamera );
  }
}

/**
* @brief ターゲット座標、ポジションを取得
*
* @param  pTarget   ターゲット座標格納先
* @oaram  pPosition ポジション格納先
*
* @return 無し
*/
void CameraUnit::GetCameraAndTargetPosition( gfl2::math::Vector3* pTarget, gfl2::math::Vector3* pPosition ) const
{
  *pTarget = m_pBaseCamera->GetTargetPosition();
  *pPosition = m_pBaseCamera->GetPosition();
}


/**
* @brief 操作機能変更
*
* @param  pController 変更後の機能  (寿命を管理する)
*
* @return 無し
*/
void CameraUnit::ChangeController( Controller::ControllerBase* pController )
{
  GFL_SAFE_DELETE( m_pController );
  m_pController = pController;
  if( m_pController )
  {
    m_pController->SetUp( m_pBaseCamera );

    if( m_IsUpdate == false )
    {
      m_pController->Pause( m_pBaseCamera );
    }
  }
}

/**
* @brief 操作機能入れ替え
*
* @param pNextController 変更後の機能 (使用者が寿命の管理をしてください)
*
* @return 変更前の機能 (使用者が保持し、使用後に戻してください)
*
* @caution 使用者は使用後にこのメソッドを使用して変更前の機能を戻してください
*
* @return 無し
*/
Controller::ControllerBase* CameraUnit::SwapController( Controller::ControllerBase* pNextController )
{
  GFL_ASSERT_MSG( m_pController != NULL, "SwapController : m_pController is NULL" );
  GFL_ASSERT_MSG( pNextController != NULL, "SwapController : pNextController is NULL" );

  m_pController->Remove( m_pBaseCamera );
  Controller::ControllerBase* pOldController = m_pController;

  m_pController = pNextController;
  m_pController->SetUp( m_pBaseCamera );
  if( m_IsUpdate == false )
  {
    m_pController->Pause( m_pBaseCamera );
  }

#if PM_DEBUG
  m_IsUseSwap = !m_IsUseSwap;
#endif // PM_DEBUG

  return pOldController;
}

/**
* @brief カメラのY軸回転量を取得
*
* @return Y軸回転量( ラジアン、Z軸正方向==0.0f )
*/
f32 CameraUnit::GetRotationY( void ) const
{
  gfl2::math::Matrix34 rot;
  gfl2::math::Vector3 vec( 0.0f, 0.0f, 1.0f );

  vec = m_pBaseCamera->GetRotationQuat().Transform( vec );
  return atan2( vec.z, vec.x ) * -1.0f - PI * 0.5f;
}

/**
* @brief 更新フラグを設定
*
* @param  updateFlag  更新フラグ
*
* @return 無し
*/
void CameraUnit::SetUpdateFlag( b32 updateFlag )
{
  if( m_IsUpdate != updateFlag && m_pController )
  {
    if( updateFlag )
    {
      m_pController->Resume( m_pBaseCamera );
    }
    else
    {
      m_pController->Pause( m_pBaseCamera );
    }
  }

  m_IsUpdate = updateFlag;
}

/**
* @brief カメラの基本パラメータをコピーする
* @note  View行列、射影行列に関わるパラメータのみをコピーする。アニメーション関連、コントローラー関連の値はコピーされません
*
* @param  rSrc コピー元
*
* @return 無し
*/
void CameraUnit::CopyBaseParam( const CameraUnit& rSrc )
{
  // 射影
  m_pBaseCamera->SetFar( rSrc.m_pBaseCamera->GetFar() );
  m_pBaseCamera->SetNear( rSrc.m_pBaseCamera->GetNear() );
  m_pBaseCamera->SetFovy( rSrc.m_pBaseCamera->GetFovy() );
  m_pBaseCamera->SetAspectRatio( rSrc.m_pBaseCamera->GetAspectRatio() );
  
  // view
  m_pBaseCamera->SetPosition( rSrc.m_pBaseCamera->GetPosition() );
  m_pBaseCamera->SetTargetPosition( rSrc.m_pBaseCamera->GetTargetPosition() );
  m_pBaseCamera->SetRotationQuat( rSrc.m_pBaseCamera->GetRotationQuat() );
}


/**
* @brief 現在の設定値から生成されるプロジェクション行列を取得する
*
* @param  pOut 行列格納先
*
* @return 引数で渡したポインタ
*/
gfl2::math::Matrix44* CameraUnit::GetProjectionMatrix( gfl2::math::Matrix44* pOut ) const
{
  if( m_pBaseCamera )
  {
    if( m_pController && m_pController->GetProjectionMatrixOverride( pOut ) )
    {
      // コントローラーが設定した行列を返す

    }
    else
    {
      *pOut = m_pBaseCamera->GetProjectionMatrix();
    }
  }
  else
  {
    *pOut = gfl2::math::Matrix44::GetIdentity();
  }

  return pOut;
}

/**
* @brief 正射影か？
*
* @return true 正射影 false 射影
*/
b32 CameraUnit::IsOrtho( void ) const
{
  if( m_pBaseCamera && m_pController )
  {
    return m_pController->IsOrtho();
  }

  return false;
}

/**
* @brief 現在の設定値から生成されるビュー行列を取得する
*
* @param  pOut 行列格納先
*
* @return 引数で渡したポインタ
*/
gfl2::math::Matrix34* CameraUnit::GetViewMatrix( gfl2::math::Matrix34* pOut ) const
{
  if( m_pBaseCamera )
  {
    if( m_pController && m_pController->GetViewMatrixOverride( pOut ) )
    {
      // コントローラーが設定した行列を返す

    }
    else
    {
      *pOut = m_pBaseCamera->GetViewMatrix();
    }
  }
  else
  {
    *pOut = gfl2::math::Matrix34::GetIdentity();
  }
  
  return pOut;
}

/**
* @brief 指定位置を始点とし、指定フレームで最新の状態まで遷移する
*
* @param  rTargetPosition カメラターゲット座標( 始点 )
* @param  rCameraPosition カメラ座標( 始点 )
* @param  frame           遷移にかけるフレーム
*
* @return 成否( コントローラーが設定されていないとき、コントローラーの状態によっては失敗する )
*/
b32 CameraUnit::SetInterpolation( const gfl2::math::Vector3& rTargetPosition, const gfl2::math::Vector3& rCameraPosition, u32 frame, gfl2::math::Easing::EaseFunc easeFunc  )
{
  if( m_pController )
  {
    return m_pController->SetInterpolation( rTargetPosition, rCameraPosition, frame, easeFunc );
  }
  else
  {
    return false;
  }
}

#if PM_DEBUG
void CameraUnit::Dump( void )
{
  if( m_pBaseCamera == NULL )
  {
    return;
  }

  GFL_PRINT("[Camera]-- Camera Info Dump --\n" );
  GFL_PRINT("[Camera]Target %f %f %f\n", m_pBaseCamera->GetPosition().x, m_pBaseCamera->GetPosition().y, m_pBaseCamera->GetPosition().z );
  GFL_PRINT("[Camera]Positon %f %f %f\n", m_pBaseCamera->GetPosition().x, m_pBaseCamera->GetPosition().y, m_pBaseCamera->GetPosition().z );
  GFL_PRINT("[Camera]Far %f\n", m_pBaseCamera->GetFar() );
  GFL_PRINT("[Camera]Near %f\n", m_pBaseCamera->GetNear() );
  GFL_PRINT("[Camera]FovY %f(%f)\n", m_pBaseCamera->GetFovy(), gfl2::math::ConvRadToDeg( m_pBaseCamera->GetFovy() ) );
}
#endif

GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );
