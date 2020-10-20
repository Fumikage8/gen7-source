/**
* @brief  フィールドカメラを管理するクラス
* @file   FieldCameraManager.cpp
* @author ikuta_junya
* @data   2015.05.21
*/

// gfl2
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryCollisionMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>

// field
#include "ViewerProc/include/FieldRo/Camera/FieldCameraManager.h"
#include "ViewerProc/include/FieldRo/Camera/FieldCameraUnit.h"
#include "ViewerProc/include/FieldRo/Camera/Target/FieldCameraTargetBase.h"
#include "ViewerProc/include/FieldRo/Camera/Controller/FieldCameraControllerTypeTest.h"
#include "ViewerProc/include/FieldRo/Camera/Controller/FieldCameraControllerTypeArea.h"
#include "ViewerProc/include/FieldRo/Camera/Area/FieldCameraAreaDataManager.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );

// カメラエリアデータ内部の構成
struct CameraAreaDataIndex
{
  enum Value
  {
    DATA = 0,
    COLLISION_MESH,
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
  ,m_pCameraAreaClollisionModel( NULL )
  ,m_pCameraAreaClollisionModelResource( NULL )
  
  // 外部オブジェクト
  ,m_pTarget( NULL )
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
  m_pCameraUnitInstanceHeap = GFL_CREATE_LOCAL_HEAP( rInitDesc.m_pSystemHeap, 1024*5, gfl2::heap::HEAP_TYPE_EXP, false ); ;

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

    // 開発中は無い事もある
    if( rInitDesc.m_pCameraAreaDataAccessor && 
      rInitDesc.m_pCameraAreaDataAccessor->GetDataMax() > CameraAreaDataIndex::DATA && 
      rInitDesc.m_pCameraAreaDataAccessor->GetDataSize( CameraAreaDataIndex::DATA ) > 0 )
    {
      desc.m_pData = rInitDesc.m_pCameraAreaDataAccessor->GetData( CameraAreaDataIndex::DATA );
    }
    else
    {
      desc.m_pData = NULL;
    }


    m_pCameraAreaDataManager = GFL_NEW( rInitDesc.m_pSystemHeap ) Area::AreaDataManager( desc );
  }

  // カメラコリジョンデータ(メッシュ)
  {
    // 無い事もある
    if( rInitDesc.m_pCameraAreaDataAccessor && 
      rInitDesc.m_pCameraAreaDataAccessor->GetDataMax() > CameraAreaDataIndex::COLLISION_MESH && 
      rInitDesc.m_pCameraAreaDataAccessor->GetDataSize( CameraAreaDataIndex::COLLISION_MESH ) > 0 )
    {
      s32 size = 0; // @todo
      gfl2::renderingengine::scenegraph::resource::GfBinaryCollisionMdlData resourceData;
      resourceData.SetModelData( static_cast<c8*>( rInitDesc.m_pCameraAreaDataAccessor->GetData( CameraAreaDataIndex::COLLISION_MESH )), size );
      m_pCameraAreaClollisionModelResource = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNodeTest( rInitDesc.m_pDeviceAllocator, &resourceData );

      m_pCameraAreaClollisionModel = GFL_NEW( rInitDesc.m_pSystemHeap ) gfl2::collision::CollisionModel();
      m_pCameraAreaClollisionModel->Initialize(m_pCameraAreaClollisionModelResource );
    }
  }


}

/**
* @brief 終了処理
*
* @return 無し
*/
void CameraManager::Terminate( void )
{
  GFL_SAFE_DELETE( m_pCameraAreaClollisionModel );
  GFL_SAFE_DELETE( m_pCameraAreaClollisionModelResource );
  GFL_SAFE_DELETE( m_pCameraAreaDataManager );

  for( u32 i = 0; i < UnitNo::MAX; i++ )
  {
    GFL_SAFE_DELETE( m_pCameraUnitTable[i] );
  }
  m_pTarget = NULL;
  GFL_DELETE_HEAP( m_pCameraUnitInstanceHeap );
}

/**
* @brief 更新処理
*
* @return 無し
*/
void CameraManager::Update( void )
{
  for( u32 i = 0; i < UnitNo::MAX; i++ )
  {
    if( m_pCameraUnitTable[ i ] )
    {
      m_pCameraUnitTable[ i ]->Update();
    }
  }
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
* @brief 開発要SetUp
*
* @return 無し
*/
void CameraManager::SetUpTest( void )
{
  // 初期操作設定 @todo 仮
#if 0
  {
    Controller::ControllerTypeTest* pController = GFL_NEW( m_pCameraUnitInstanceHeap ) Controller::ControllerTypeTest(
      m_pTarget
      );
    m_pCameraUnitTable[ UnitNo::FIELD ]->ChangeController( pController );
  }
#endif
  {
    Controller::ControllerTypeArea::InitializeDescription desc;
    desc.m_pAreaDataManager = m_pCameraAreaDataManager;
    desc.m_pTarget = m_pTarget;
    desc.m_pSystemHeap = m_pCameraUnitInstanceHeap; // @todo
    desc.m_pCameraAreaClollisionModel = m_pCameraAreaClollisionModel;

    Controller::ControllerTypeArea* pController = GFL_NEW( m_pCameraUnitInstanceHeap ) Controller::ControllerTypeArea(
      desc
      );
    m_pCameraUnitTable[ UnitNo::FIELD ]->ChangeController( pController );
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

GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );