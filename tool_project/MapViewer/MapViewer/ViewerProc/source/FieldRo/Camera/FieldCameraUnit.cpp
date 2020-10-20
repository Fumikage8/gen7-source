/**
* @brief  フィールドカメラ、1制御単位クラス
* @file   FieldCameraUnit.cpp
* @author ikuta_junya
* @data   2015.05.21
*/


// gfl2
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>

// field
#include "ViewerProc/include/FieldRo/Camera/FieldCameraUnit.h"


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
  // カメラノード作成 @todo 今は必ず作るが、アニメーションを使用しない場合は不要になる
  {
    m_pDrawEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode();
    gfl2::renderingengine::scenegraph::instance::CameraNode* pCameraNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateCameraNode();

    pCameraNode->SetFar( DEFAULT_PARAM_FAR );
    pCameraNode->SetNear( DEFAULT_PARAM_NEAR );
    pCameraNode->SetAspectRatio( DEFAULT_PARAM_ASPECT_RATIO );
    pCameraNode->SetFovy( DEFAULT_PARAM_FOVY_RAD );
    
    m_pDrawEnvNode->AddCamera( pCameraNode );
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

  // 射影設定 @todo 仮
  {
    // EnvNodeを渡している場合、CameraNodeの値がCreate時に設定されている
#if 0
    m_pBaseCamera->SetNear( 60000.0f );
    m_pBaseCamera->SetFar( 60000.0f );
    m_pBaseCamera->SetAspectRatio( 
      gfl2::gfx::GFGLUtil::GetDisplayWidthByDisplayMode( gfl2::gfx::DisplayMode::HD720P ) 
      / (f32)gfl2::gfx::GFGLUtil::GetDisplayHeightByDisplayMode( gfl2::gfx::DisplayMode::HD720P )
      );

    m_pBaseCamera->SetFovy( gfl2::math::ConvDegToRad( 35.0f ) );
#endif
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
* @brief 操作機能変更
*
* @param  pController 変更後の機能  (寿命を管理する)
*
* @return 無し
*/
void CameraUnit::ChangeController( Controller::ControllerBase* pController )
{
  // @todo　ここで補間を行う


  GFL_SAFE_DELETE( m_pController );
  m_pController = pController;
  if( m_pController )
  {
    m_pController->SetUp( m_pBaseCamera );
  }
}

/**
* @brief カメラのY軸回転量を取得 @todo この関数は消すかも
*
* @return Y軸回転量( ラジアン、Z軸正方向==0.0f )
*/
f32 CameraUnit::GetRotationY( void ) const
{
  gfl2::math::Matrix34 rot;
  gfl2::math::Vector3 vec( 0.0f, 0.0f, 1.0f );

  m_pBaseCamera->GetRotationQuat().QuaternionToMatrix( rot );
  vec = rot.TransformCoord( vec );
  ///return atan2( vec.z, vec.x ) * -1.0f - PI * 0.5f;
	return atan2( vec.z, vec.x ) * -1.0f + PI * 0.5f;

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


GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );
