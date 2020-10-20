/**
* @brief  入力情報をカメラを考慮した形に変換する機能
* @file   FieldCameraUiDeviceTranslator.cpp
* @author ikuta_junya
* @data   2015.11.20
*/

// niji
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

// field
#include "Field/FieldRo/include/Camera/FieldCameraUiDeviceTranslator.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"

// Camera
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"

// Debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );


u32 UI_TRANS_DIFF_CHECK_FRAME( 5 );             // このフレームの間、入力値を記憶し差分をチェックする
f32 UI_TRANS_DIFF_CHECK_ANGLE( 3 );             // 入力値がこの角(度)に以上に差分があった場合、カメラを更新する
f32 UI_TRANS_INPUT_EFFECTIVE_LENGHT( 0.001f );  // この値以上の長さなら入力が有効(この値及び、自機操作のチェックがある、こちらは0除算防止用のケア)


/**
* @brief コンストラクタ
*
* @return 無し
*/ 
UiDeviceTranslator::UiDeviceTranslator( void ) : 
  m_Mode( Mode::NORMAL ),
  m_X( 0.0f ),
  m_Y( 0.0f ), 
  m_ViewRotationY( 0.0f ),
  m_ReadyFlag( false ),
  m_pStickLog( NULL ),
  m_pCameraManager( NULL ),
  m_pFieldmap( NULL )
{
}

/**
* @brief デストラクタ
*
* @return 無し
*/
UiDeviceTranslator::~UiDeviceTranslator( void )
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
void UiDeviceTranslator::Initialize( const InitializeDescription& rInitDesc )
{
  m_X = 0.0f;
  m_Y = 0.0f;
  m_ViewRotationY = 0.0f;

  m_pStickLog = GFL_NEW( rInitDesc.m_pSystemHeap ) gfl2::util::List<gfl2::math::Vector2>( rInitDesc.m_pSystemHeap, UI_TRANS_DIFF_CHECK_FRAME );

  m_pCameraManager = rInitDesc.m_pCameraManager;
  m_pFieldmap = rInitDesc.m_pFieldmap;
}

/**
* @brief 終了処理
*
* @return 無し
*/
void UiDeviceTranslator::Terminate( void )
{
  GFL_SAFE_DELETE( m_pStickLog );
}

/**
* @brief 更新処理
*
* @return 無し
*/
void UiDeviceTranslator::Update( void )
{
  if( m_ReadyFlag == false )
  {
    return;
  }

  // コントローラーの入力を取得
  GameSys::GameManager*     game_manager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager*  device_manager = game_manager->GetUiDeviceManager();
  gfl2::ui::VectorDevice*   pVecDevice     = device_manager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::math::Vector2       oriStick( pVecDevice->GetX(), pVecDevice->GetY() );

  // 
  // 入力に適用するカメラ角を更新する処理
  // 
  b32 updateCameraAngle( false );
  updateCameraAngle = CameraAngleCheckUpdate( oriStick );

  //
  // 入力に適用するカメラ角を更新
  //
  if( updateCameraAngle )
  {
    m_ViewRotationY = gfl2::math::NormalizeRadianAngle( m_pCameraManager->GetMainViewCamera()->GetRotationY() );
  }

  //
  // 入力値にカメラ角を適用
  //
  gfl2::math::Vector3 vec( oriStick.x, oriStick.y, 0.0f );
  vec = gfl2::math::Matrix34::GetRotationZ( m_ViewRotationY + PI ).TransformCoord( vec );
  m_X = vec.x;
  m_Y = vec.y;
}

/**
* @brief フィールドの準備が完了した際に実行される関数
*
* @return 無し
*/
void UiDeviceTranslator::Ready( void )
{
  m_ReadyFlag = true;

  // 入力に適用するカメラ角を更新
  m_ViewRotationY = gfl2::math::NormalizeRadianAngle( m_pCameraManager->GetMainViewCamera()->GetRotationY() );

  // スティック入力の蓄積をクリア
  m_pStickLog->Erase();

}

/**
* @brief 型式を変更
*
* @param  mode  型式
*
* @return 無し
*/
void UiDeviceTranslator::SetMode( Mode::Value mode )
{
  m_Mode = mode;

  // スティック入力の蓄積をクリア
  m_pStickLog->Erase();
}

/**
* @brief X入力量を取得
*
* @return 入力量( -1.0f ～ 1.0f )
*/
f32 UiDeviceTranslator::GetX( void ) const
{
  return m_X;
}

/**
* @brief Y入力量を取得
*
* @return 入力量( -1.0f ～ 1.0f )
*/
f32 UiDeviceTranslator::GetY( void ) const
{
  return m_Y;
}

/**
* @brief 入力変換に使うカメラ角を更新するかチェック
*  
* @param  rOriStick スティック入力値(無加工)
*
* @return true 更新する false 更新しない
*/
b32 UiDeviceTranslator::CameraAngleCheckUpdate( const gfl2::math::Vector2& rOriStick )
{
  // 即時反映モード
  if( m_Mode == Mode::DIRECT )
  {
    return true;
  }
  

  // 通常フィールドの処理、入力開始時のカメラ角を記憶
  f32                       stickLength    = rOriStick.Length();
  gfl2::math::Vector2       stickUnitVec( rOriStick.x, rOriStick.y );


  b32 updateCameraAngle( false );

  // 
  // カメラ角を継続する条件
  // ・自機が操作可能状態
  // ・入力値が自機操作扱い
  // ・過去数フレームからの入力方向の差が一定以下
  // 
  if( m_pFieldmap->GetPlayerCharacter()->IsEnableInput() && m_pFieldmap->GetPlayerCharacter()->IsInputStick() && stickLength > UI_TRANS_INPUT_EFFECTIVE_LENGHT )
  {
    stickUnitVec.NormalizeRet();
    
    // スティック入力が急激に変わったら即時反映
    gfl2::util::ListIterator< gfl2::math::Vector2 > it( m_pStickLog->Begin() );
    gfl2::util::ListIterator< gfl2::math::Vector2 > end( m_pStickLog->End() );
    for(; it!= end; it++)
    {
      f32 cos = stickUnitVec.Dot( *it );
      cos = gfl2::math::Clamp( cos, -1.0f, 1.0f );    // 計算誤差で1.0fを微量にオーバーする事がある。オーバーするとmath系のWarningが大量に出る

      // 入力差が一定以上なら、使用するカメラ角を更新する
      if( gfl2::math::FAbs( gfl2::math::AcosDeg( cos ) ) >= UI_TRANS_DIFF_CHECK_ANGLE )
      {
        updateCameraAngle = true;
        break;
      }
    }
  }
  else
  {
    // 即時反映
    updateCameraAngle = true;
  }

  // スティック入力の蓄積をクリア
  if( updateCameraAngle )
  {
    m_pStickLog->Erase();
  }
  else
  {
    // @note stickUnitVecがNormalizeされている事を担保

    // スティック入力を蓄積( 入力角の差分チェックを高速化するため、単位ベクトルで保持 )
    if( m_pStickLog->Size() == m_pStickLog->MaxSize() )
    {
      m_pStickLog->PopFront();
    }
    m_pStickLog->PushBack( stickUnitVec );
  }

  return updateCameraAngle;
}



GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );