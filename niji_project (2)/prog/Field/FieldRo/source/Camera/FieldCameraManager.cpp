/**
* @brief  フィールドカメラを管理するクラス
* @file   FieldCameraManager.cpp
* @author ikuta_junya
* @data   2015.05.21
*/

// field
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldRo/include/Camera/Target/FieldCameraTargetBase.h"
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeArea.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaDataManager.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaCollisionManager.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaScrollStopManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraSystemAnimationResource.h"
#include "Field/FieldRo/include/Camera/FieldCameraAnimationLoader.h"
#include "Field/FieldRo/include/Camera/FieldCameraHitObject.h"

// コリジョンシステム
#include "Field/FieldRo/include/Collision/BaseActor.h"
#include "Field/FieldRo/include/Collision/StaticActor.h"
#include "Field/FieldRo/include/Collision/DynamicActor.h"
#include "Field/FieldRo/include/Collision/BaseCollisionScene.h"
#include "Field/FieldRo/include/Collision/CollisionSceneDefault.h"
#include "Field/FieldRo/include/Collision/CollisionSceneNone.h"
#include "Field/FieldRo/include/Collision/BaseShape.h"


#if PM_DEBUG

// debug
#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#include "Field/FieldRo/include/Debug/FieldDebugMenu.h"
#include "Debug/DebugWin/include/DebugWinSystem.h"

#endif

#if PM_DEBUG
#include "Field/FieldRo/include/Debug/FieldDebugCollisionRenderManager.h"
#endif  //#if PM_DEBUG

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );

struct CollisonSetting
{
  f32 r;
};

struct CameraSetting
{
  CollisonSetting collison;
};


// カメラ設定、最終的にはconst化
CameraSetting cameraSetting =
{
  {
    50.0f,
  } // CollisonSetting
};


// カメラエリアデータ内部の構成
struct CameraAreaDataIndex
{
  enum Value
  {
    DATA = 0,
    COLLISION_PRIMITIVE,
    COLLISION_MESH,
    SCROLL_STOP,
    MAX
  };
};


/**
* @brief コンストラクタ
*
* @return 無し
*/ 
CameraManager::CameraManager( void ) :
  // 制御
  m_MainCameraUnitNo( 0 )
  ,m_ViewCameraUnitNo( -1 )

  // 内部オブジェクト
  ,m_pCameraUnitInstanceHeap( NULL )
  ,m_pCameraAreaDataManager( NULL )
  ,m_pCameraAreaCollisionManager( NULL )
  ,m_pAreaController( NULL )
  ,m_pScrollStopManager( NULL )
  ,m_pEventCameraAnimationLoader( NULL )
  ,m_pInstanceHeapForEvCamera( NULL )

  // 外部オブジェクト
  ,m_pTarget( NULL )
  ,m_pFieldMap( NULL )
  ,m_pPlacementDataManager( NULL )
  ,m_pCameraEyeCollsionScene( NULL )
#if PM_DEBUG
  ,m_pDebugWinGroup( NULL )
#endif
{
  for( u32 i = 0; i < UnitNo::MAX; i++ )
  {
    m_pCameraUnitTable[ i ] = NULL;
  }
}

/**
* @brief デストラクタ
*
* @return 無し
*/
CameraManager::~CameraManager( void )
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
void CameraManager::Initialize( const InitializeDescription& rInitDesc )
{
  m_pTarget = rInitDesc.m_pTarget;
  m_pFieldMap = rInitDesc.m_pFieldMap;
  m_pPlacementDataManager = rInitDesc.m_pPlacementDataManager;
  m_pCameraUnitInstanceHeap = GFL_CREATE_LOCAL_HEAP_NAME( rInitDesc.m_pSystemHeap, 1024*5, gfl2::heap::HEAP_TYPE_EXP, false, "FieldCameraAreaController" ); ;
  m_pCameraEyeCollsionScene = rInitDesc.m_pCameraEyeCollsionScene;

  // イベントカメラ用ヒープ生成
  {

    // @note コントローラー生成248バイト　160216
    // @note 確保直後のTotalFreeSizeは496になっている模様、CTRライブラリの仕様？
    size_t size = gfl2::heap::Manager::CalcCreateHeapSize( 512, gfl2::heap::HEAP_TYPE_EXP );
    m_pInstanceHeapForEvCamera = GFL_CREATE_LOCAL_HEAP_NAME( rInitDesc.m_pSystemHeap, size, gfl2::heap::HEAP_TYPE_EXP, false, "FieldCameraScriptEventController"); ;
  }

  // フィールド通常カメラ生成
  {
    CameraUnit::InitializeDescription desc;
    desc.m_pDeviceAllocator = rInitDesc.m_pDeviceAllocator;
    desc.m_pDeviceHeap = rInitDesc.m_pDeviceHeap;
    desc.m_pSystemHeap = rInitDesc.m_pSystemHeap;

    m_pCameraUnitTable[ UnitNo::FIELD ] = GFL_NEW( rInitDesc.m_pSystemHeap ) CameraUnit;
    m_pCameraUnitTable[ UnitNo::FIELD ]->Initialize( desc );
    m_pCameraUnitTable[ UnitNo::FIELD ]->SetUpdateFlag( true );
  }

  // スクリプト/ポケファインダー用 
  {
    CameraUnit::InitializeDescription desc;
    desc.m_pDeviceAllocator = rInitDesc.m_pDeviceAllocator;
    desc.m_pDeviceHeap = rInitDesc.m_pDeviceHeap;
    desc.m_pSystemHeap = rInitDesc.m_pSystemHeap;

    m_pCameraUnitTable[ UnitNo::EVENT ] = GFL_NEW( rInitDesc.m_pSystemHeap ) CameraUnit;
    m_pCameraUnitTable[ UnitNo::EVENT ]->Initialize( desc );
    m_pCameraUnitTable[ UnitNo::EVENT ]->SetUpdateFlag( false );
  }

  // デバッグ用
  {
    CameraUnit::InitializeDescription desc;
    desc.m_pDeviceAllocator = rInitDesc.m_pDeviceAllocator;
    desc.m_pDeviceHeap = rInitDesc.m_pDeviceHeap;
    desc.m_pSystemHeap = rInitDesc.m_pSystemHeap;

    m_pCameraUnitTable[ UnitNo::DEBUG ] = GFL_NEW( rInitDesc.m_pSystemHeap ) CameraUnit;
    m_pCameraUnitTable[ UnitNo::DEBUG ]->Initialize( desc );
    m_pCameraUnitTable[ UnitNo::DEBUG ]->SetUpdateFlag( false );
  }

  // 使用カメラ設定
  {
    m_MainCameraUnitNo = UnitNo::FIELD;
    m_ViewCameraUnitNo = -1;
  }


  // カメラエリアデータ
  {
    Area::AreaDataManager::InitializeDescription desc;

    // データが0個の場合もヘッダーデータが存在する
    desc.m_pAreaData = rInitDesc.m_pCameraAreaDataAccessor->GetData( CameraAreaDataIndex::DATA );
    desc.m_pZoneDefaultData = rInitDesc.m_pCameraZoneDefaultData;
    desc.m_pZoneDefaultSupportData = rInitDesc.m_pCameraSupportZoneDefaultData;
    desc.m_pZoneData = rInitDesc.m_pZoneData;
    desc.m_pHeap = rInitDesc.m_pSystemHeap;
    m_pCameraAreaDataManager = GFL_NEW( rInitDesc.m_pSystemHeap ) Area::AreaDataManager( desc );
  }

  // カメラエリアコリジョンデータ
  {
    Area::AreaCollisionManager::InitializeDescription desc;
    desc.m_pSystemHeap = rInitDesc.m_pSystemHeap;
    desc.m_pDeviceAllocator = rInitDesc.m_pDeviceAllocator;
    
    // 基本形状はデータが0個の場合もヘッダーデータが存在する
    desc.m_pPrimitiveData = rInitDesc.m_pCameraAreaDataAccessor->GetData( CameraAreaDataIndex::COLLISION_PRIMITIVE );

    // 自由形状(gfcol)はデータが存在しない場合は0バイトデータになっている
    if( rInitDesc.m_pCameraAreaDataAccessor->GetDataSize( CameraAreaDataIndex::COLLISION_MESH ) > 0 )
    {
      desc.m_pModelData = rInitDesc.m_pCameraAreaDataAccessor->GetData( CameraAreaDataIndex::COLLISION_MESH );
    }
    else
    {
      desc.m_pModelData = NULL;
    }

    m_pCameraAreaCollisionManager = GFL_NEW( rInitDesc.m_pSystemHeap ) Area::AreaCollisionManager( desc );
  }

  // スクロールストップ
  {
    Area::ScrollStopManager::InitializeDescription desc;
    desc.m_pHeap = rInitDesc.m_pSystemHeap;
    desc.m_pData = NULL;

    if( rInitDesc.m_pCameraAreaDataAccessor->GetDataMax() > CameraAreaDataIndex::SCROLL_STOP ) // このチェックはフォーマット移行が終われば不要
    {
      if( rInitDesc.m_pCameraAreaDataAccessor->GetDataSize( CameraAreaDataIndex::SCROLL_STOP ) > 0 )
      {
        desc.m_pData = rInitDesc.m_pCameraAreaDataAccessor->GetData( CameraAreaDataIndex::SCROLL_STOP );
      }
      else
      {
        desc.m_pData = NULL;
      }
    }

    m_pScrollStopManager = GFL_NEW( rInitDesc.m_pSystemHeap ) Area::ScrollStopManager( desc );

  }

  // システムアニメーション
  {
    CameraSystemAnimationResource::InitializeDescription desc;
    desc.m_pDeviceAllocator = rInitDesc.m_pNijiDeviceAllocator;
    desc.m_pDeviceHeap = rInitDesc.m_pDeviceHeap;
    desc.m_pSystemHeap = rInitDesc.m_pSystemHeap;
    desc.m_pData = rInitDesc.m_pSystemAnimationData;

    m_pSystemAnimationResource = GFL_NEW( rInitDesc.m_pSystemHeap ) CameraSystemAnimationResource();
    m_pSystemAnimationResource->Initialize( desc );
  }
 
  // イベント用アニメ管理
  {
    CameraAnimationLoader::InitializeDescription desc;
    m_pEventCameraAnimationLoader = GFL_NEW( rInitDesc.m_pSystemHeap ) CameraAnimationLoader();
    m_pEventCameraAnimationLoader->Initialize( desc );
  }

  // コリジョン用アクター生成＆シーン登録
  {
    m_pDynamicActorForCameraEye = m_pCameraEyeCollsionScene->CreateDynamicActor( rInitDesc.m_pNijiDeviceAllocator, rInitDesc.m_pSystemHeap );

    m_pDynamicActorForCameraEye->CreateSphere( gfl2::math::Vector3( 0.0f, 0.0f, 0.0f ), 0.0f );
    m_pDynamicActorForCameraEye->SetActive( false );
    m_pDynamicActorForCameraEye->DisableAdjustVector();

    m_pCameraEyeCollsionScene->RegistDynamicActor( m_pDynamicActorForCameraEye );
  }

  m_GlobalHorizonSpring.Init();
  m_GlobalVerticalSpring.Init();

}

/**
* @brief 終了処理
*
* @return 無し
*/
void CameraManager::Terminate( void )
{
#if PM_DEBUG
  // デバッグメニューへの登録を解除
  UnResistDebugMenu();
#endif


  if( m_pDynamicActorForCameraEye )
  {
    m_pCameraEyeCollsionScene->UnRegistDynamicActor( m_pDynamicActorForCameraEye );
    GFL_SAFE_DELETE( m_pDynamicActorForCameraEye );
  }

  GFL_SAFE_DELETE( m_pEventCameraAnimationLoader );
  GFL_SAFE_DELETE( m_pSystemAnimationResource );
  GFL_SAFE_DELETE( m_pScrollStopManager );
  GFL_SAFE_DELETE( m_pCameraAreaCollisionManager );
  GFL_SAFE_DELETE( m_pCameraAreaDataManager );

  for( u32 i = 0; i < UnitNo::MAX; i++ )
  {
    GFL_SAFE_DELETE( m_pCameraUnitTable[i] );
  }
  m_pCameraEyeCollsionScene = NULL;
  m_pAreaController = NULL;
  m_pTarget = NULL;
  m_pFieldMap = NULL;
  m_pPlacementDataManager = NULL;
  GFL_DELETE_HEAP( m_pInstanceHeapForEvCamera );
  GFL_DELETE_HEAP( m_pCameraUnitInstanceHeap );
}

/**
* @brief 更新処理
*
* @return 無し
*/
void CameraManager::Update( void )
{
  // 注視座標更新
  m_pTarget->Update();

  // カメラ揺れ
  m_GlobalHorizonSpring.Update();
  m_GlobalVerticalSpring.Update();

  for( u32 i = 0; i < UnitNo::MAX; i++ )
  {
    if( m_pCameraUnitTable[ i ] )
    {
#if PM_DEBUG
      if( !FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::CAMERA_STOP ) )
#endif
      {
        m_pCameraUnitTable[ i ]->Update();

        // 振動反映
        if (m_GlobalHorizonSpring.IsValid() || m_GlobalVerticalSpring.IsValid() ) 
        {
          m_pCameraUnitTable[i]->ApplyGlobalShake( m_GlobalHorizonSpring, m_GlobalVerticalSpring );
        }
      }
    }
  }

  m_pEventCameraAnimationLoader->Update();
}

/**
* @brief カメラコリジョンの更新処理
*
* @return 無し
*/
void CameraManager::UpdateCameraEyeCollision( void )
{
  // カメラ反映
  SphereShape* pShape = static_cast<SphereShape*>( m_pDynamicActorForCameraEye->GetShape() );
  CameraUnit* pMainCamera = this->GetMainGamePlayCamera();

  // 形状更新
  m_pDynamicActorForCameraEye->SetPosition( pMainCamera->GetBaseCamera()->GetPosition() );
  pShape->m_pos = gfl2::math::Vector3::GetZero();
  pShape->m_r = cameraSetting.collison.r;

  // コリジョン実行
  m_pDynamicActorForCameraEye->SetActive( true );
  m_pDynamicActorForCameraEye->InitCollisionResult();
  m_pCameraEyeCollsionScene->Update();

  // HitしたメッシュをOFF
  for( u32 i = 0; i < BaseActor::MAX_COLLISION_RESULT_CNT; i++ )
  {
    BaseActor* pCollisionActor = m_pDynamicActorForCameraEye->GetCollisionResult( i )->pTargetActor;
    if( pCollisionActor == NULL)
    {
      continue;
    }

    // メッシュを取得
    void* pUserData = pCollisionActor->GetUserData();

    // Hitしました
    Field::Camera::CameraHitObject* pHitObject = reinterpret_cast<Field::Camera::CameraHitObject*>( pUserData );
    pHitObject->Hit();
  }
}


/**
* @brief フィールド操作開始時に実行する処理
*
* @return 無し
*/
void CameraManager::Ready( void )
{
  // @note この時点では自機にコリジョン押し返しが適用されていないので、注意。またReady～1stUpdateまでにイベントによる座標変更が発生する事がある 例) ドア接続

  if( m_pAreaController )
  {
    m_pAreaController->Ready();
  }

#if PM_DEBUG
  // デバッグメニューへの登録
  ResistDebugMenu();
#endif

}

/**
* @brief カメラターゲット変更処理
*
* @param  pTarger ターゲット (寿命の管理は行いません)
*
* @return 無し
*/
void CameraManager::ChangeCameraTarget( Target::TargetBase* pTarger )
{
  m_pTarget = pTarger;
}

/**
* @brief 通常マップ用のカメラセットアップ
* @note  カメラエリア用コントローラーをフィールドカメラに設定する
*
* @return 無し
*/
void CameraManager::SetUpForNormalMap( void )
{
  {
    Controller::ControllerTypeArea::InitializeDescription desc;
    desc.m_pAreaDataManager = m_pCameraAreaDataManager;
    desc.m_pAreaCollisionManager = m_pCameraAreaCollisionManager;
    desc.m_pTarget = m_pTarget;
    desc.m_pSystemHeap = m_pCameraUnitInstanceHeap;
    desc.m_pFieldMap = m_pFieldMap;
    desc.m_pPlacementDataManager = m_pPlacementDataManager;
    desc.m_pScrollStopManager = m_pScrollStopManager;

    m_pAreaController = GFL_NEW( m_pCameraUnitInstanceHeap ) Controller::ControllerTypeArea(
      desc
      );
    m_pCameraUnitTable[ UnitNo::FIELD ]->ChangeController( m_pAreaController );
  }
}

/**
* @brief 自機操作、カリングなどに使用するカメラを取得する
*
* @return カメラ
*/
const CameraUnit* CameraManager::GetMainGamePlayCamera( void ) const
{
  return m_pCameraUnitTable[ m_MainCameraUnitNo ];
}

/**
* @brief 描画に使用するカメラを取得する
* @note  基本的にGamePlayCameraと同じだが、デバッグView時などは違うカメラが返る事がある
*
* @return カメラ
*/
const CameraUnit* CameraManager::GetMainViewCamera( void ) const
{
  if( m_ViewCameraUnitNo != -1 )
  {
    return m_pCameraUnitTable[ m_ViewCameraUnitNo ];
  }
  else
  {
    return this->GetMainGamePlayCamera();
  }
}

/**
* @brief 自機操作、カリングなどに使用するカメラを取得する
*
* @return カメラ
*/
CameraUnit* CameraManager::GetMainGamePlayCamera( void )
{
  return m_pCameraUnitTable[ m_MainCameraUnitNo ];
}

/**
* @brief 描画に使用するカメラを取得する
* @note  基本的にGamePlayCameraと同じだが、デバッグView時などは違うカメラが返る事がある
*
* @return カメラ
*/
CameraUnit* CameraManager::GetMainViewCamera( void )
{
  if( m_ViewCameraUnitNo != -1 )
  {
    return m_pCameraUnitTable[ m_ViewCameraUnitNo ];
  }
  else
  {
    return this->GetMainGamePlayCamera();
  }
}

/**
* @brief 自機操作、カリングなどに使用するカメラのUnitNoを取得
*
* @return UnitNo
*/
u32 CameraManager::GetUnitNoForMainGamePlayCamera( void ) const
{
  return m_MainCameraUnitNo;
}

/**
* @brief 描画に使用するカメラのUnitNoを取得
* @note  基本的にGamePlayCameraと同じだが、デバッグView時などは違うカメラが返る事がある
*
* @return UnitNo
*/
u32 CameraManager::GetUnitNoForGetMainViewCamera( void ) const
{ 
  if( m_ViewCameraUnitNo != -1 )
  {
    return m_ViewCameraUnitNo;
  }
  else
  {
    return this->GetUnitNoForMainGamePlayCamera();
  }
}

/**
* @brief カメラを取得する
*
* @param  unitNo  ユニット番号 ( 今はUnitNo )
*
* @return カメラ
*/
const CameraUnit* CameraManager::GetCameraByUnitNo( u32 unitNo ) const
{
  if( unitNo >= UnitNo::MAX )
  {
    return NULL;
  }

  return m_pCameraUnitTable[ unitNo ];
}

/**
* @brief カメラを取得する
*
* @param  unitNo  ユニット番号 ( 今はUnitNo )
*
* @return カメラ
*/
CameraUnit* CameraManager::GetCameraByUnitNo( u32 unitNo )
{
  if( unitNo >= UnitNo::MAX )
  {
    return NULL;
  }

  return m_pCameraUnitTable[ unitNo ];
}

/**
* @brief 描画に使用するカメラを設定する
*
* @param  unitNo  ユニット番号 ( 今はUnitNo )
* @param  prevSleep trueなら現在使用しているカメラをSleepして変更
*
* @return 成否
*/
b32 CameraManager::ChangeMainCameraByUnitNo( u32 unitNo, b32 prevSleep )
{
  if( unitNo >= UnitNo::MAX || m_pCameraUnitTable[ unitNo ] == NULL )
  {
    return false;
  }

  // 現在使用しているカメラをSleep
  if( m_pCameraUnitTable[ m_MainCameraUnitNo ] && prevSleep && m_MainCameraUnitNo != m_ViewCameraUnitNo )
  {
    m_pCameraUnitTable[ m_MainCameraUnitNo ]->SetUpdateFlag( false );
  }

  m_MainCameraUnitNo = unitNo;
  m_pCameraUnitTable[ m_MainCameraUnitNo ]->SetUpdateFlag( true );

  return true;
}

/**
* @brief 描画で使用するカメラを上書き設定する
*
* @param  unitNo  ユニット番号 ( 今はUnitNo )
* @param  prevSleep trueなら現在使用しているカメラをSleepして変更( Mainで使用されている場合はSleepしない )
*
* @return 成否
*/
b32 CameraManager::OverrideViewCameraByUnitNo( u32 unitNo, b32 prevSleep )
{
  if( unitNo >= UnitNo::MAX || m_pCameraUnitTable[ unitNo ] == NULL )
  {
    return false;
  }

  // 現在使用しているカメラをSleep
  if( prevSleep && this->IsOverrideViewCamera() && m_ViewCameraUnitNo != m_MainCameraUnitNo )
  {
    m_pCameraUnitTable[ m_ViewCameraUnitNo ]->SetUpdateFlag( false );
  }

  m_ViewCameraUnitNo = unitNo;
  m_pCameraUnitTable[ m_ViewCameraUnitNo ]->SetUpdateFlag( true );
  return true;
}

/**
* @brief 描画で使用するカメラを上書き設定を解除
*
* @param  prevSleep trueなら現在使用しているカメラをSleepして変更( Mainで使用されている場合はSleepしない )
*
* @return 成否
*/
b32 CameraManager::ClearOverrideViewCamera( b32 prevSleep )
{
  if( m_ViewCameraUnitNo == -1 )
  {
    return false;
  }

  // 現在使用しているカメラをSleep
  if( m_pCameraUnitTable[ m_ViewCameraUnitNo ] && prevSleep && m_ViewCameraUnitNo != m_MainCameraUnitNo )
  {
    m_pCameraUnitTable[ m_ViewCameraUnitNo ]->SetUpdateFlag( false );
  }

  m_ViewCameraUnitNo = -1;
  return true;
}

/**
* @brief 描画で使用するカメラを上書き設定しているか？
* @note  結果的に同じカメラでも、Overrideしている扱いになる
*
* @param  pUnitNo 設定しているUnitNo格納先  NULL指定可、戻り値trueのときのみ有効
*
* @return true している   false していない
*/
b32 CameraManager::IsOverrideViewCamera( u32* pUnitNo ) const
{
  if( m_ViewCameraUnitNo != -1 )
  {
    if( pUnitNo )
    {
      *pUnitNo = m_ViewCameraUnitNo;
    }

    return true;
  }

  return false;
}

/**
* @brief カメラの更新フラグを設定
*
* @param  unitNo  ユニット番号 ( 今はUnitNo )
* @param  isActive  trueなら更新
*
* @return 成否
*/
b32 CameraManager::SetActiveFlagByUnitNo( u32 unitNo, b32 isActive )
{
  if( unitNo >= UnitNo::MAX || m_pCameraUnitTable[ unitNo ] == NULL )
  {
    return false;
  }

  m_pCameraUnitTable[ unitNo ]->SetUpdateFlag( isActive );
  return true;
}

/**
* @brief カメラサポート挙動の制御アクションの実行
*
* @param  value     アクション種別
*
* @return 成否
*/
b32 CameraManager::SetSupportAction( SupportAction::Value value )
{
  if( m_pAreaController )
  {
    return m_pAreaController->SetSupportAction( value );
  }
  else
  {
    return false;
  }
}

/**
* @brief 通常フィールドコントローラーを差し替える
*        差し替え後の復帰処理は現状では提供していません
*
* @param  pControllerBase 差し替え後のコントローラー
*
* @return 無し
*/
void CameraManager::ChangeFieldController( Controller::ControllerBase* pControllerBase )
{
  CameraUnit* pCameraUnit = GetCameraByUnitNo( UnitNo::FIELD );

  m_pAreaController = NULL;
  pCameraUnit->ChangeController( pControllerBase );
}

/**
* @brief フィールドカメラフォーカスモジュールを取得
*
* @return フォーカスモジュール( 初期化前はNULLの可能性があります )
*/
const Area::Focus* CameraManager::GetFocus( void ) const
{
  if( m_pAreaController == NULL )
  {
    return NULL;
  }

  return m_pAreaController->GetFocus();
}
Area::Focus* CameraManager::GetFocus( void )
{
  if( m_pAreaController == NULL )
  {
    return NULL;
  }

  return m_pAreaController->GetFocus();
}

/**
* @brief フィールドカメラサポートモジュールを取得
*        JFなど通常フィールドカメラを使わない場所では存在しない事があります
*
* @return サポートモジュール( NULLの可能性があります )
*/
const Area::Support* CameraManager::GetSupport( void ) const
{
  if( m_pAreaController == NULL )
  {
    return NULL;
  }

  return m_pAreaController->GetSupport();
}

#if PM_DEBUG

/**
* @brief 現在HitしているカメラエリアのアトリビュートNoを取得
*
* @return　アトリビュートNo( 無効な場合は-1 )
*/
s32 CameraManager::GetHitAttr( void ) const
{
  // カメラエリアが存在しない
  if( m_pAreaController == NULL )
  {
    return -1;
  }

  // areaNo -> attr 変換
  u32 areaNo = m_pAreaController->GetNowAreaDataNo();
  if( areaNo == Area::AreaDataNo::DEFAULT || areaNo == Area::AreaDataNo::INVALID )
  {
    return -1;
  }

  return static_cast<s32>( areaNo );
}

/**
* @brief デバッグ表示物登録処理
*/
void CameraManager::DebugRenderResistCameraTarget( b32 drawActiveOnlyFlag )
{
  Debug::DebugCollisionRenderManager* pManager = GFL_SINGLETON_INSTANCE( Debug::DebugCollisionRenderManager );
  if( pManager != NULL && m_pTarget != NULL )
  {
    f32 drawSize = 10.0f;

    gfl2::math::Vector3 pos;
    gfl2::math::Vector3 controlPos;
    m_pTarget->GetCameraTargetPosition( &pos );
    m_pTarget->GetTargetRowPosition( &controlPos );

    pos.y -= drawSize * 0.5f;         // シリンダーの座標は足元座標指定なので、中心になるまでずらす
    controlPos.y -= drawSize * 0.5f;

    pManager->RegistRenderCollisionCylinder( pos, drawSize * 0.5f, drawSize, gfl2::math::Vector4( 0.0f, 1.0f, 0.0f, 1.0f ), false );
    pManager->RegistRenderCollisionCylinder( controlPos, drawSize * 0.5f, drawSize, gfl2::math::Vector4( 1.0f, 0.0f, 0.0f, 1.0f ), false );
  }
}
void CameraManager::DebugRenderResistCameraTargetPosition( b32 drawActiveOnlyFlag )
{
  Debug::DebugCollisionRenderManager* pManager = GFL_SINGLETON_INSTANCE( Debug::DebugCollisionRenderManager );
  if( pManager != NULL && m_pTarget != NULL )
  {
    f32 drawSize = 7.5f;

    gfl2::math::Vector3 cameraPos;
    gfl2::math::Vector3 cameraTargetPos;

    this->GetMainGamePlayCamera()->GetCameraAndTargetPosition( &cameraTargetPos, &cameraPos );

    cameraTargetPos.y -= drawSize * 0.5f;         // シリンダーの座標は足元座標指定なので、中心になるまでずらす
    cameraTargetPos.y -= drawSize * 0.5f;
   
    pManager->RegistRenderCollisionCylinder( cameraTargetPos, drawSize * 0.5f, drawSize, gfl2::math::Vector4( 255.0f / 255.0f, 79.0f / 255.0f, 25.0f / 255.0f, 1.0f ), true );
  }
}

#endif  //#if PM_DEBUG

// private:

#if PM_DEBUG
// デバッグメニューへの登録
void CameraManager::ResistDebugMenu( void )
{
  // @fix GFNMCat[1208]:デバッグメニュー：本来選択できない項目が登録され、選択すると停止する。対処。 グループ名の変更に追従するため、「ハードコーディングの名前指定によるグループ取得」→「FieldDebugMenuから直接グループを取得」に変更。
  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::debug::DebugWinGroup* pGroup = pGameManager->GetFieldmap()->GetFieldDebugMenu()->GetCameraRootGroup();
  if( pGroup )
  {
    gfl2::heap::HeapBase *pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
    gfl2::debug::DebugWinGroup* pCollisionGroup = gfl2::debug::DebugWin_AddGroup( pHeap, L"Collision", pGroup );

    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    gfl2::str::MsgData* pMsgData = pGameManager->GetFieldmap()->GetFieldDebugMenu()->GetMessageData();
    
    // コリジョン関連
    gfl2::debug::DebugWin_AddItemValueFloat( pCollisionGroup, pHeap, L"r", &cameraSetting.collison.r, 0.1f, 1000.0f, 0.1f );

    Field::Camera::CameraHitObject::Setting* pSetting = Field::Camera::CameraHitObject::GetSettingPtr();
    
#if 0
        s32 downFrame;
    s32 downEaseFunc;
    s32 upFrame;
    s32 upEaseFunc;
#endif
    gfl2::debug::DebugWin_AddItemValueInt( pCollisionGroup, pHeap, L"downFrame", &pSetting->downFrame, 0, 30 * 60, 1 );
    Field::Debug::FieldDebugMenu::DebugWin_AddItemValueEaseFunc(  pCollisionGroup, pHeap, L"downEaseFunc", &pSetting->downEaseFunc );

    gfl2::debug::DebugWin_AddItemValueInt( pCollisionGroup, pHeap, L"upFrame", &pSetting->upFrame, 0, 30 * 60, 1 );
    Field::Debug::FieldDebugMenu::DebugWin_AddItemValueEaseFunc(  pCollisionGroup, pHeap, L"upEaseFunc", &pSetting->upEaseFunc );

    // 記憶
    m_pDebugWinGroup = pCollisionGroup;
  }
}
// デバッグメニューへの登録を解除
void CameraManager::UnResistDebugMenu( void )
{
  // @fix GFNMCat[1208]:デバッグメニュー：本来選択できない項目が登録され、選択すると停止する。対処。 グループ名の変更に追従するため、「ハードコーディングの名前指定によるグループ削除」→「記憶したグループクラスを指定して削除」に変更。
  if( m_pDebugWinGroup )
  {
    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    if( pGameManager->GetFieldmap()->GetFieldDebugMenu() && pGameManager->GetFieldmap()->GetFieldDebugMenu()->GetCameraRootGroup() )
    {
      pGameManager->GetFieldmap()->GetFieldDebugMenu()->GetCameraRootGroup()->RemoveItem( m_pDebugWinGroup );
      m_pDebugWinGroup = NULL;
    }
    else
    {
      // @note 無いなら親ごと削除されている
      m_pDebugWinGroup = NULL;
    }
  }
}
#endif


GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );