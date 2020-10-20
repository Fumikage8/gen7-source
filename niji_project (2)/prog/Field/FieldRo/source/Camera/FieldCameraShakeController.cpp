/**
* @brief  フィールドカメラの操作、イベントスクリプト用
* @file   FieldCameraShakeController.cpp
* @author ikuta_junya
* @data   2015.05.21
*/

// field
#include "Field/FieldRo/include/Camera/FieldCameraShakeController.h"


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );


ShakeController::ShakeController() :
  m_IsShake( false )
  ,m_Scale( 1.0f )
  ,m_pCamera( NULL )
{
  m_EndFadeValue.Clear();
}

ShakeController::~ShakeController()
{

}

// システム公開関数
void ShakeController::Set( poke_3d::model::BaseCamera* pCamera )
{
  m_pCamera = pCamera;
  m_IsShake = false;
}
void ShakeController::Clear( void )
{
  m_pCamera = NULL;
  m_IsShake = false;
}
void ShakeController::Update( void )
{
  if( m_pCamera == NULL || m_IsShake == false )
  {
    return;
  }

  //
  // 更新
  //

  // 終了フェード更新、ローカルアニメの反映率を落としていく
  if( m_EndFadeValue.IsValid() )
  {
    m_EndFadeValue.Update();
    m_pCamera->SetLocalAnimationScale( m_EndFadeValue.GetValue() );
  }

  // アニメ更新
  m_pCamera->UpdateAnimation();

  //
  // 終了チェック
  //

  // 再生完了による終了
  if( m_pCamera->IsAnimation() == false )
  {
    EndImpl();
    return;
  }

  // フェード完了による終了
  if( m_EndFadeValue.IsValid() && m_EndFadeValue.IsEnd() )
  {
    EndImpl();
    return;
  }
}

// 使用者公開関数
void ShakeController::Begin( gfl2::renderingengine::scenegraph::resource::ResourceNode* pAnime, b32 loopFlag, f32 stepFrame, f32 scale )
{
  GFL_ASSERT( m_pCamera->IsAnimation() == false );    // 複数アニメーションの同時再生は許容していない
  
  // @note 非ループの場合は再生後に内部でクリアされる
  m_pCamera->ChangeAnimationLocalByResourceNode( pAnime );
  m_pCamera->SetAnimationLoop( loopFlag );
  m_pCamera->SetAnimationStepFrame( stepFrame );
  m_pCamera->SetLocalAnimationScale( scale );
  m_Scale = scale;

  m_IsShake = true;

}

void ShakeController::End( u32 fadeFrame )
{
  if( fadeFrame == 0 )
  {
    // 即終了
    EndImpl();
  }
  else
  {
    // フェードして終了
    m_EndFadeValue.Move( m_Scale, 0.0f, fadeFrame, gfl2::math::Easing::LINEAR );
  }
}

b32 ShakeController::IsShake( void ) const
{
  if( m_pCamera == NULL )
  {
    return false;
  }

  return m_IsShake;
}

void ShakeController::EndImpl( void )
{
  m_pCamera->SetLocalAnimationScale( 1.0f );
  m_pCamera->EndAnimation();
  m_IsShake = false;
  m_Scale = 1.0f;
  m_EndFadeValue.Clear();
}


GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );