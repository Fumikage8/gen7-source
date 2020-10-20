/**
* @brief  フィールドデバッグメニュー
* @file   FieldDebugMenu.cpp
* @author ikuta_junya
* @data   2015.03.08
*/

// @note このファイルはsangoでは最終的に15000行近くになっていた。多くなってきたらファイルを分けたい。 150308 ikuta_junya


#if PM_DEBUG

// gfl2
#include <debug/include/gfl2_DebugWinItemTypes.h>
#include <RenderingEngine/include/SceneGraph/gfl2_Traverser.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

// niji
#include "debug/DebugWin/include/DebugWinSystem.h"
#include "GameSys/include/GameManager.h"
#include "System/include/HeapDefine.h"
#include "Sound/include/Sound.h"

// FieldTest
#include "test/FieldTest/include/debug/FieldDebugMenu.h"
#include "test/FieldTest/include/debug/FieldDebugTypes.h"
#include "test/FieldTest/include/FieldTestUtility.h"

// デバッグ関数内部で必要になったもの
#include "test/FieldTest/include/FieldSystem.h"
#include "test/FieldTest/include/movemodel_manager.h"
#include "test/FieldTest/include/movemodel.h"
#include "test/FieldTest/include/field_transform.h"
#include "test/FieldTest/include/Camera/FieldCameraUtility.h"
#include "test/FieldTest/include/Camera/FieldCamera.h"
#include "test/FieldTest/include/debug/FieldDebugInstanceMemoryCollection.h"
#include "test/FieldTest/include/debug/FieldDebugModelInstanceInfo.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( debug )

// デバッグメニューで項目を選択中に実行される関数
GFL_NAMESPACE_BEGIN( Item )

// DrawInfoグループ
GFL_NAMESPACE_BEGIN( DrawInfo )

GFL_NAMESPACE_END( DrawInfo )


// DrawSwitchグループ
GFL_NAMESPACE_BEGIN( DrawSwitch )

// DrawSwitchの値を全て初期値にする
static void Reset( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager	= pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice*	pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button*	pButton	= pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  for( u32 i = 0; i < FieldDebugTypes::DRAW_SWITCH_MAX; i++ )
  {
    FieldDebugTypes::s_DrawSwitchTable[ i ] = FieldDebugTypes::GetDrawSwitchInitialValue( static_cast< FieldDebugTypes::DrawSwitch >( i ) );
  }

  // @todo 決定音
  Sound::PlaySE( SEQ_SE_DECIDE1 );
}

// DrawSwitchの値を全て0にする
static void AllZero( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager	= pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice*	pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button*	pButton	= pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  for( u32 i = 0; i < FieldDebugTypes::DRAW_SWITCH_MAX; i++ )
  {
    FieldDebugTypes::s_DrawSwitchTable[ i ] = 0;
  }

  // @todo 決定音
  Sound::PlaySE( SEQ_SE_DECIDE1 );
}



GFL_NAMESPACE_END( DrawSwitch )

// フィールド関連のDumpを行うグループ
GFL_NAMESPACE_BEGIN( Dump )

// プレイヤー情報をDump
static void PlayerInfo( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager	= pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice*	pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button*	pButton	= pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  fld::mmodel::MoveModelManager* pMoveModelManager = pGameManager->GetFieldSystem()->GetMoveModelManager();
  if( pMoveModelManager )
  {
    fld::mmodel::MoveModel* pMoveModelPlayer = pMoveModelManager->GetMoveModel( fld::mmodel::MMDL_PLAYER );
    if( pMoveModelPlayer )
    {
      FieldTransform* pTransform = pMoveModelPlayer->GetTransform();

      GFL_RELEASE_PRINT("-- PlayerInfo --\n");
      GFL_RELEASE_PRINT("pos x=%f y= %f z=%f \n", pTransform->GetTranslation().x, pTransform->GetTranslation().y, pTransform->GetTranslation().z );
    }
  }
  
  // @todo 決定音
  Sound::PlaySE( SEQ_SE_DECIDE1 );
}

// カメラ情報をDump
static void CameraInfo( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager	= pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice*	pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button*	pButton	= pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  Field::Camera::FieldCamera* pCamera = pGameManager->GetFieldSystem()->GetPlayerCamera();
  if( pCamera )
  {
    gfl2::heap::HeapBase* pSystemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
    Field::Camera::CAMERA_DATA* pCameraData = Field::Camera::CreateDefaultCameraData( pSystemHeap );

    GFL_RELEASE_PRINT("-- CameraInfo --\n");
    pCamera->Dump();
  }
  
  // @todo 決定音
  Sound::PlaySE( SEQ_SE_DECIDE1 );
}

// シーングラフのノード数をDump
static void NodeCount( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager	= pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice*	pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button*	pButton	= pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  gfl2::renderingengine::scenegraph::Traverser::Visitor<gfl2::renderingengine::scenegraph::DagNode, Test::FieldTest::NodeCountVisitor> visitor;
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseFast<gfl2::renderingengine::scenegraph::DagNode, Test::FieldTest::NodeCountVisitor>( &visitor );

  GFL_RELEASE_PRINT("-- NodeCountDump --\n");
  GFL_RELEASE_PRINT("NodeCount[%d]\n", visitor.GetNodeCount() );
  GFL_RELEASE_PRINT("NodeName[実数][継承数]  (NodeNameに対しTraverseを実行した際の対象総数は[実数]+[継承数])\n\n" );
    
  GFL_RELEASE_PRINT("UnknownNode[%d][%d]\n", visitor.GetUnknownNodeCount(), visitor.GetUnknownNodeCountOfInheritance() );

  for( u32 i = 0; i < visitor.GetNodeCountDataNum(); i++ )
  {
    Test::FieldTest::NodeCountVisitor::NODE_COUNT_DATA data;
    visitor.GetNodeCountData( i, &data );

    GFL_RELEASE_PRINT("%s[%d][%d]\n", data.pNodeName, data.count, data.countOfInheritance );
  }

  // @todo 決定音
  Sound::PlaySE( SEQ_SE_DECIDE1 );
}

// メモリ情報
static void MemoryCollection( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager	= pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice*	pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button*	pButton	= pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_DumpAll();
}

// モデル情報
static void ModelInfo( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager	= pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice*	pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button*	pButton	= pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  // トラバースして、モデルの情報を表示
  Test::FieldTest::DebugTraverseModelInstanceInfoDraw();
}

GFL_NAMESPACE_END( Dump )

// 計測関連グループ
GFL_NAMESPACE_BEGIN( Performance )

// フィールドの状態を計測状態にする
static void SetUp( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager	= pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice*	pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button*	pButton	= pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  // 自機座標をリセット
  fld::mmodel::MoveModelManager* pMoveModelManager = pGameManager->GetFieldSystem()->GetMoveModelManager();
  if( pMoveModelManager )
  {
    fld::mmodel::MoveModel* pMoveModelPlayer = pMoveModelManager->GetMoveModel( fld::mmodel::MMDL_PLAYER );
    if( pMoveModelPlayer )
    {
      FieldTransform* pTransform = pMoveModelPlayer->GetTransform();
      pTransform->SetTranslation( gfl2::math::Vector( 0.0f, -50.0f, 0.0f ) );
      pTransform->Update();
    }
  }

  // カメラをリセット
  Field::Camera::FieldCamera* pCamera = pGameManager->GetFieldSystem()->GetPlayerCamera();
  if( pCamera )
  {
    gfl2::heap::HeapBase* pSystemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
    Field::Camera::CAMERA_DATA* pCameraData = Field::Camera::CreateDefaultCameraData( pSystemHeap );

    pCamera->Change( pCameraData, true );

    GFL_SAFE_DELETE( pCameraData->pData );
    GFL_SAFE_DELETE( pCameraData );
  }

  // BGM停止
  Sound::StopAllBGM();

  // 全消し+IDエッジ
  for( u32 i = 0; i < FieldDebugTypes::DRAW_SWITCH_MAX; i++ )
  {
    FieldDebugTypes::s_DrawSwitchTable[ i ] = 0;
  }

  FieldDebugTypes::s_DrawSwitchTable[ FieldDebugTypes::DRAW_SWITCH_EDGE_MODE ] = 1;
  FieldDebugTypes::s_DrawSwitchTable[ FieldDebugTypes::DRAW_SWITCH_EDGE_TEST_00 ] = 1;


  // @todo 決定音
  Sound::PlaySE( SEQ_SE_DECIDE1 );

}

// フィールドの状態を計測状態にする
static void SetUp2( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager	= pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice*	pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button*	pButton	= pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  // 自機座標をリセット
  fld::mmodel::MoveModelManager* pMoveModelManager = pGameManager->GetFieldSystem()->GetMoveModelManager();
  if( pMoveModelManager )
  {
    fld::mmodel::MoveModel* pMoveModelPlayer = pMoveModelManager->GetMoveModel( fld::mmodel::MMDL_PLAYER );
    if( pMoveModelPlayer )
    {
      FieldTransform* pTransform = pMoveModelPlayer->GetTransform();
      pTransform->SetTranslation( gfl2::math::Vector( 0.0f, -50.0f, -95.0f ) );
      pTransform->Update();
    }
  }

  // カメラをリセット
  Field::Camera::FieldCamera* pCamera = pGameManager->GetFieldSystem()->GetPlayerCamera();
  if( pCamera )
  {
    gfl2::heap::HeapBase* pSystemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
    Field::Camera::CAMERA_DATA* pCameraData = Field::Camera::CreateDefaultCameraData( pSystemHeap );

    Field::Camera::CAMERA_DATA_RELATIVE_ROTATE* pData = static_cast< Field::Camera::CAMERA_DATA_RELATIVE_ROTATE* >( pCameraData->pData );

    // バストアップ
    pData->distance = 219.0f;
    pData->rotate.x = gfl2::math::ConvDegToRad( -19.0f );

    pCamera->Change( pCameraData, true );

    GFL_SAFE_DELETE( pCameraData->pData );
    GFL_SAFE_DELETE( pCameraData );
  }

  // BGM停止
  Sound::StopAllBGM();

  // 全消し+IDエッジ
  for( u32 i = 0; i < FieldDebugTypes::DRAW_SWITCH_MAX; i++ )
  {
    FieldDebugTypes::s_DrawSwitchTable[ i ] = 0;
  }

  FieldDebugTypes::s_DrawSwitchTable[ FieldDebugTypes::DRAW_SWITCH_EDGE_MODE ] = 1;
  FieldDebugTypes::s_DrawSwitchTable[ FieldDebugTypes::DRAW_SWITCH_EDGE_TEST_00 ] = 1;

  // @todo 決定音
}

static void SetUp3( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager	= pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice*	pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button*	pButton	= pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  // 自機座標をリセット
  fld::mmodel::MoveModelManager* pMoveModelManager = pGameManager->GetFieldSystem()->GetMoveModelManager();
  if( pMoveModelManager )
  {
    fld::mmodel::MoveModel* pMoveModelPlayer = pMoveModelManager->GetMoveModel( fld::mmodel::MMDL_PLAYER );
    if( pMoveModelPlayer )
    {
      FieldTransform* pTransform = pMoveModelPlayer->GetTransform();
      pTransform->SetTranslation( gfl2::math::Vector( 622.746338f, 350.0f, 1429.354004f ) );

      gfl2::math::Quaternion qua;
      qua.RadianYToQuaternion( gfl2::math::ConvDegToRad( 45.0f ) );
      pTransform->SetQuaternion( qua );
      pTransform->Update();
    }
  }

  // カメラをリセット
  Field::Camera::FieldCamera* pCamera = pGameManager->GetFieldSystem()->GetPlayerCamera();
  if( pCamera )
  {
    gfl2::heap::HeapBase* pSystemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
    Field::Camera::CAMERA_DATA* pCameraData = Field::Camera::CreateDefaultCameraData( pSystemHeap );

    Field::Camera::CAMERA_DATA_RELATIVE_ROTATE* pData = static_cast< Field::Camera::CAMERA_DATA_RELATIVE_ROTATE* >( pCameraData->pData );

    pData->distance = 480.0f;
    pData->rotate.x = gfl2::math::ConvDegToRad( -2.115830f );
    pData->rotate.y = gfl2::math::ConvDegToRad( 50.693432f );

    pCamera->Change( pCameraData, true );

    GFL_SAFE_DELETE( pCameraData->pData );
    GFL_SAFE_DELETE( pCameraData );
  }


}

static void SetUpTerrain1( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager	= pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice*	pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button*	pButton	= pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  // 自機座標をリセット
  fld::mmodel::MoveModelManager* pMoveModelManager = pGameManager->GetFieldSystem()->GetMoveModelManager();
  if( pMoveModelManager )
  {
    fld::mmodel::MoveModel* pMoveModelPlayer = pMoveModelManager->GetMoveModel( fld::mmodel::MMDL_PLAYER );
    if( pMoveModelPlayer )
    {
      FieldTransform* pTransform = pMoveModelPlayer->GetTransform();
      pTransform->SetTranslation( gfl2::math::Vector( 622.746338f, 350.0f, 1429.354004f ) );

      gfl2::math::Quaternion qua;
      qua.RadianYToQuaternion( gfl2::math::ConvDegToRad( 45.0f ) );
      pTransform->SetQuaternion( qua );
      pTransform->Update();
    }
  }

  // カメラをリセット
  Field::Camera::FieldCamera* pCamera = pGameManager->GetFieldSystem()->GetPlayerCamera();
  if( pCamera )
  {
    gfl2::heap::HeapBase* pSystemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
    Field::Camera::CAMERA_DATA* pCameraData = Field::Camera::CreateDefaultCameraData( pSystemHeap );

    Field::Camera::CAMERA_DATA_RELATIVE_ROTATE* pData = static_cast< Field::Camera::CAMERA_DATA_RELATIVE_ROTATE* >( pCameraData->pData );

    pData->distance = 480.0f;
    pData->rotate.x = gfl2::math::ConvDegToRad( -2.115830f );
    pData->rotate.y = gfl2::math::ConvDegToRad( 50.693432f );

    pCamera->Change( pCameraData, true );

    GFL_SAFE_DELETE( pCameraData->pData );
    GFL_SAFE_DELETE( pCameraData );
  }


  FieldDebugTypes::s_DrawSwitchTable[ FieldDebugTypes::DRAW_SWITCH_ENCOUNT_MIDDLE ] = 1;

}

static void SetUpTerrain2( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager	= pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice*	pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button*	pButton	= pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  // 自機座標をリセット
  fld::mmodel::MoveModelManager* pMoveModelManager = pGameManager->GetFieldSystem()->GetMoveModelManager();
  if( pMoveModelManager )
  {
    fld::mmodel::MoveModel* pMoveModelPlayer = pMoveModelManager->GetMoveModel( fld::mmodel::MMDL_PLAYER );
    if( pMoveModelPlayer )
    {
      FieldTransform* pTransform = pMoveModelPlayer->GetTransform();
      pTransform->SetTranslation( gfl2::math::Vector( -816.0f, 351.000000f, 2120.0f ) );

      gfl2::math::Quaternion qua;
      qua.RadianYToQuaternion( gfl2::math::ConvDegToRad( 45.0f ) );
      pTransform->SetQuaternion( qua );
      pTransform->Update();
    }
  }

  // カメラをリセット
  Field::Camera::FieldCamera* pCamera = pGameManager->GetFieldSystem()->GetPlayerCamera();
  if( pCamera )
  {
    gfl2::heap::HeapBase* pSystemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
    Field::Camera::CAMERA_DATA* pCameraData = Field::Camera::CreateDefaultCameraData( pSystemHeap );

    Field::Camera::CAMERA_DATA_RELATIVE_ROTATE* pData = static_cast< Field::Camera::CAMERA_DATA_RELATIVE_ROTATE* >( pCameraData->pData );

    pData->distance = 620.000000f;
    pData->rotate.x = gfl2::math::ConvDegToRad( -7.246861f );
    pData->rotate.y = gfl2::math::ConvDegToRad( 4.680757f );

    pCamera->Change( pCameraData, true );

    GFL_SAFE_DELETE( pCameraData->pData );
    GFL_SAFE_DELETE( pCameraData );
  }


  FieldDebugTypes::s_DrawSwitchTable[ FieldDebugTypes::DRAW_SWITCH_ENCOUNT_MIDDLE ] = 1;

}



static void SetUpTerrain3( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager	= pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice*	pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button*	pButton	= pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  // 自機座標をリセット
  fld::mmodel::MoveModelManager* pMoveModelManager = pGameManager->GetFieldSystem()->GetMoveModelManager();
  if( pMoveModelManager )
  {
    fld::mmodel::MoveModel* pMoveModelPlayer = pMoveModelManager->GetMoveModel( fld::mmodel::MMDL_PLAYER );
    if( pMoveModelPlayer )
    {
      FieldTransform* pTransform = pMoveModelPlayer->GetTransform();
      pTransform->SetTranslation( gfl2::math::Vector( -1479.0f, 351.000000f, 2357.0f ) );

      gfl2::math::Quaternion qua;
      qua.RadianYToQuaternion( gfl2::math::ConvDegToRad( 45.0f ) );
      pTransform->SetQuaternion( qua );
      pTransform->Update();
    }
  }

  // カメラをリセット
  Field::Camera::FieldCamera* pCamera = pGameManager->GetFieldSystem()->GetPlayerCamera();
  if( pCamera )
  {
    gfl2::heap::HeapBase* pSystemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
    Field::Camera::CAMERA_DATA* pCameraData = Field::Camera::CreateDefaultCameraData( pSystemHeap );

    Field::Camera::CAMERA_DATA_RELATIVE_ROTATE* pData = static_cast< Field::Camera::CAMERA_DATA_RELATIVE_ROTATE* >( pCameraData->pData );

    pData->distance = 700.000000f;
    pData->rotate.x = gfl2::math::ConvDegToRad( -11.732604f );
    pData->rotate.y = gfl2::math::ConvDegToRad( 1.829615f );

    pCamera->Change( pCameraData, true );

    GFL_SAFE_DELETE( pCameraData->pData );
    GFL_SAFE_DELETE( pCameraData );
  }


  FieldDebugTypes::s_DrawSwitchTable[ FieldDebugTypes::DRAW_SWITCH_ENCOUNT_MIDDLE ] = 1;

}

static void SetUpEnc1( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager	= pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice*	pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button*	pButton	= pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  // 自機座標をリセット
  fld::mmodel::MoveModelManager* pMoveModelManager = pGameManager->GetFieldSystem()->GetMoveModelManager();
  if( pMoveModelManager )
  {
    fld::mmodel::MoveModel* pMoveModelPlayer = pMoveModelManager->GetMoveModel( fld::mmodel::MMDL_PLAYER );
    if( pMoveModelPlayer )
    {
      FieldTransform* pTransform = pMoveModelPlayer->GetTransform();

      if( pButton->IsHold( gfl2::ui::BUTTON_R ) == false )
      {
        // 草に埋まるver
        pTransform->SetTranslation( gfl2::math::Vector( 973.101746f, -439.000000f, 5110.853516f ) );
      }
      else
      {
        // 草に埋まらないver
        pTransform->SetTranslation( gfl2::math::Vector( 973.101746f, -368.000000f, 5110.853516f ) );
      }

      gfl2::math::Quaternion qua;
      qua.RadianYToQuaternion( gfl2::math::ConvDegToRad( 45.0f ) );
      pTransform->SetQuaternion( qua );
      pTransform->Update();
    }
  }

  // カメラをリセット
  Field::Camera::FieldCamera* pCamera = pGameManager->GetFieldSystem()->GetPlayerCamera();
  if( pCamera )
  {
    gfl2::heap::HeapBase* pSystemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
    Field::Camera::CAMERA_DATA* pCameraData = Field::Camera::CreateDefaultCameraData( pSystemHeap );

    Field::Camera::CAMERA_DATA_RELATIVE_ROTATE* pData = static_cast< Field::Camera::CAMERA_DATA_RELATIVE_ROTATE* >( pCameraData->pData );

    pData->distance = 694.000000f;
    pData->rotate.x = gfl2::math::ConvDegToRad( -19.866873f );
    pData->rotate.y = gfl2::math::ConvDegToRad( 0.628691f );

    pCamera->Change( pCameraData, true );

    GFL_SAFE_DELETE( pCameraData->pData );
    GFL_SAFE_DELETE( pCameraData );
  }


  FieldDebugTypes::s_DrawSwitchTable[ FieldDebugTypes::DRAW_SWITCH_ENCOUNT_MIDDLE ] = 1;

}

static void SetUpEnc2( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager	= pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice*	pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button*	pButton	= pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  // 自機座標をリセット
  fld::mmodel::MoveModelManager* pMoveModelManager = pGameManager->GetFieldSystem()->GetMoveModelManager();
  if( pMoveModelManager )
  {
    fld::mmodel::MoveModel* pMoveModelPlayer = pMoveModelManager->GetMoveModel( fld::mmodel::MMDL_PLAYER );
    if( pMoveModelPlayer )
    {
      FieldTransform* pTransform = pMoveModelPlayer->GetTransform();

      if( pButton->IsHold( gfl2::ui::BUTTON_R ) == false )
      {
        // 草に埋まるver
        pTransform->SetTranslation( gfl2::math::Vector( 973.101746f, -439.000000f, 5110.853516f ) );
      }
      else
      {
        // 草に埋まらないver
        pTransform->SetTranslation( gfl2::math::Vector( 973.101746f, -368.000000f, 5110.853516f ) );
      }

      gfl2::math::Quaternion qua;
      qua.RadianYToQuaternion( gfl2::math::ConvDegToRad( 45.0f ) );
      pTransform->SetQuaternion( qua );
      pTransform->Update();
    }
  }

  // カメラをリセット
  Field::Camera::FieldCamera* pCamera = pGameManager->GetFieldSystem()->GetPlayerCamera();
  if( pCamera )
  {
    gfl2::heap::HeapBase* pSystemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
    Field::Camera::CAMERA_DATA* pCameraData = Field::Camera::CreateDefaultCameraData( pSystemHeap );

    Field::Camera::CAMERA_DATA_RELATIVE_ROTATE* pData = static_cast< Field::Camera::CAMERA_DATA_RELATIVE_ROTATE* >( pCameraData->pData );

    pData->distance = 1654.000000f;
    pData->rotate.x = gfl2::math::ConvDegToRad( -19.866873f );
    pData->rotate.y = gfl2::math::ConvDegToRad( 0.628691f );

    pCamera->Change( pCameraData, true );

    GFL_SAFE_DELETE( pCameraData->pData );
    GFL_SAFE_DELETE( pCameraData );
  }


  FieldDebugTypes::s_DrawSwitchTable[ FieldDebugTypes::DRAW_SWITCH_ENCOUNT_MIDDLE ] = 1;

}

static void SetUpEnc3( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager	= pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice*	pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button*	pButton	= pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  // 自機座標をリセット
  fld::mmodel::MoveModelManager* pMoveModelManager = pGameManager->GetFieldSystem()->GetMoveModelManager();
  if( pMoveModelManager )
  {
    fld::mmodel::MoveModel* pMoveModelPlayer = pMoveModelManager->GetMoveModel( fld::mmodel::MMDL_PLAYER );
    if( pMoveModelPlayer )
    {
      FieldTransform* pTransform = pMoveModelPlayer->GetTransform();

      if( pButton->IsHold( gfl2::ui::BUTTON_R ) == false )
      {
        // 草に埋まるver
        pTransform->SetTranslation( gfl2::math::Vector( 973.101746f, -439.000000f, 5110.853516f ) );
      }
      else
      {
        // 草に埋まらないver
        pTransform->SetTranslation( gfl2::math::Vector( 973.101746f, -368.000000f, 5110.853516f ) );
      }

      gfl2::math::Quaternion qua;
      qua.RadianYToQuaternion( gfl2::math::ConvDegToRad( 45.0f ) );
      pTransform->SetQuaternion( qua );
      pTransform->Update();
    }
  }

  // カメラをリセット
  Field::Camera::FieldCamera* pCamera = pGameManager->GetFieldSystem()->GetPlayerCamera();
  if( pCamera )
  {
    gfl2::heap::HeapBase* pSystemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
    Field::Camera::CAMERA_DATA* pCameraData = Field::Camera::CreateDefaultCameraData( pSystemHeap );

    Field::Camera::CAMERA_DATA_RELATIVE_ROTATE* pData = static_cast< Field::Camera::CAMERA_DATA_RELATIVE_ROTATE* >( pCameraData->pData );

    pData->distance = 1654.000000f;
    pData->rotate.x = gfl2::math::ConvDegToRad( -6.313796f );
    pData->rotate.y = gfl2::math::ConvDegToRad( 0.628691f );

    pCamera->Change( pCameraData, true );

    GFL_SAFE_DELETE( pCameraData->pData );
    GFL_SAFE_DELETE( pCameraData );
  }


  FieldDebugTypes::s_DrawSwitchTable[ FieldDebugTypes::DRAW_SWITCH_ENCOUNT_MIDDLE ] = 1;

}


// フィールドの状態をミップマップ計測状態にする
static void SetUpMipMap( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager	= pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice*	pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button*	pButton	= pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  // 自機座標をリセット
  fld::mmodel::MoveModelManager* pMoveModelManager = pGameManager->GetFieldSystem()->GetMoveModelManager();
  if( pMoveModelManager )
  {
    fld::mmodel::MoveModel* pMoveModelPlayer = pMoveModelManager->GetMoveModel( fld::mmodel::MMDL_PLAYER );
    if( pMoveModelPlayer )
    {
      FieldTransform* pTransform = pMoveModelPlayer->GetTransform();
      pTransform->SetTranslation( gfl2::math::Vector( -690.0f, 350.0f, -720.0f ) );
      pTransform->Update();
    }
  }

  // カメラをリセット
  Field::Camera::FieldCamera* pCamera = pGameManager->GetFieldSystem()->GetPlayerCamera();
  if( pCamera )
  {
    gfl2::heap::HeapBase* pSystemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
    Field::Camera::CAMERA_DATA* pCameraData = Field::Camera::CreateDefaultCameraData( pSystemHeap );

    Field::Camera::CAMERA_DATA_RELATIVE_ROTATE* pData = static_cast< Field::Camera::CAMERA_DATA_RELATIVE_ROTATE* >( pCameraData->pData );

    pData->distance = 579.0f;
    pData->rotate.x = gfl2::math::ConvDegToRad( -0.5f );
    pData->rotate.y = gfl2::math::ConvDegToRad( 57.0f );

    pCamera->Change( pCameraData, true );

    GFL_SAFE_DELETE( pCameraData->pData );
    GFL_SAFE_DELETE( pCameraData );
  }

  // BGM停止
  Sound::StopAllBGM();

  // 全消し+IDエッジ
  for( u32 i = 0; i < FieldDebugTypes::DRAW_SWITCH_MAX; i++ )
  {
    FieldDebugTypes::s_DrawSwitchTable[ i ] = 0;
  }

  FieldDebugTypes::s_DrawSwitchTable[ FieldDebugTypes::DRAW_SWITCH_EDGE_MODE ] = 1;
  FieldDebugTypes::s_DrawSwitchTable[ FieldDebugTypes::DRAW_SWITCH_TERRAIN_DISTANT_VIEW ] = 1;
  FieldDebugTypes::s_DrawSwitchTable[ FieldDebugTypes::DRAW_SWITCH_TERRAIN_HIGH ] = 1;
  FieldDebugTypes::s_DrawSwitchTable[ FieldDebugTypes::DRAW_SWITCH_TERRAIN_MIDDLE ] = 1;

  // @todo 決定音
}

// フィールドの状態をミップマップ計測状態にする
static void TestTraverseFast( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager	= pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice*	pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button*	pButton	= pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

#if defined(GF_PLATFORM_CTR)
  gfl2::renderingengine::scenegraph::Traverser::Visitor<gfl2::renderingengine::scenegraph::DagNode, Test::FieldTest::VisitCountVisitor> visitor;
  nn::os::Tick start = nn::os::Tick::GetSystemCurrent();
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseFast<gfl2::renderingengine::scenegraph::DagNode, Test::FieldTest::VisitCountVisitor>( &visitor );
  nn::os::Tick result = nn::os::Tick::GetSystemCurrent() - start;
  GFL_RELEASE_PRINT("visit[%4d] us[%6d]", visitor.GetVisitCount() , result.ToTimeSpan().GetMicroSeconds() );
#endif
}


GFL_NAMESPACE_END( Performance )

// サウンド関連の処理
GFL_NAMESPACE_BEGIN( SoundDebug )

// フィールドBGMを停止
static void BGMStop( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager	= pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice*	pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button*	pButton	= pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  // BGM停止
  Sound::StopAllBGM();

  // @todo 決定音
}
// フィールドBGMを再開
static void BGMStart( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager	= pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice*	pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button*	pButton	= pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  // BGM停止
  Sound::StartBGM(STRM_BGM_T_01);

  // @todo 決定音
  Sound::PlaySE( SEQ_SE_DECIDE1 );
}

GFL_NAMESPACE_END( SoundDebug )

// フィールド関連のカメラ操作を行うグループを追加
GFL_NAMESPACE_BEGIN( Camera )

static void SetTop( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager	= pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice*	pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button*	pButton	= pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  Field::Camera::FieldCamera* pCamera = pGameManager->GetFieldSystem()->GetPlayerCamera();
  if( pCamera )
  {
    pCamera->ChangeTopCamera();
  }

  // 決定音
  Sound::PlaySE( SEQ_SE_DECIDE1 );

}

static void EndDebug( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager	= pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice*	pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button*	pButton	= pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  Field::Camera::FieldCamera* pCamera = pGameManager->GetFieldSystem()->GetPlayerCamera();
  if( pCamera )
  {
    pCamera->EndDebugCamera();
  }

  // 決定音
  Sound::PlaySE( SEQ_SE_DECIDE1 );
}

static void SetDefault( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager	= pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice*	pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button*	pButton	= pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  // カメラをリセット
  Field::Camera::FieldCamera* pCamera = pGameManager->GetFieldSystem()->GetPlayerCamera();
  if( pCamera )
  {
    gfl2::heap::HeapBase* pSystemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
    Field::Camera::CAMERA_DATA* pCameraData = Field::Camera::CreateDefaultCameraData( pSystemHeap );

    pCamera->Change( pCameraData, true );
    pCamera->EndDebugCamera();

    GFL_SAFE_DELETE( pCameraData->pData );
    GFL_SAFE_DELETE( pCameraData );
  }

  // 決定音
  Sound::PlaySE( SEQ_SE_DECIDE1 );

}

GFL_NAMESPACE_END( Camera )

// Tick計測関連グループ
GFL_NAMESPACE_BEGIN( Tick )

// Tick計測関連グループが開かれたときの処理
void OpenGroupFunc( void *work )
{
#if FIELD_DEBUG_TICK_ENABLE
  FIELD_DEBUG_TICK_SET_FLAG( true );
  FIELD_DEBUG_TICK_RESET();
#endif // #if GFL_DEBUGWIN_USE
}

static wchar_t* DrawLabelName( void* userWork, gfl2::debug::DebugWinItem* item )
{
  // ラベル名のみ表示
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr , gfl2::debug::DEBUGWIN_NAME_LEN,
    L"%ls", item->GetName()
    );

  return workStr;
}

static wchar_t* DrawTickValue( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if 0
  // チック値のみ表示
  u32 index = reinterpret_cast<u32>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr , gfl2::debug::DEBUGWIN_NAME_LEN,
    L" Now[%6d] Ave[%6d] Max[%6d]",
    FIELD_DEBUG_TICK_US_GET(index),
    FIELD_DEBUG_TICK_US_AVE_GET(index),
    FIELD_DEBUG_TICK_US_MAX_GET(index)
    );
#endif
  // チック値のみ表示
  u32 index = reinterpret_cast<u32>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr , gfl2::debug::DEBUGWIN_NAME_LEN,
    L"%ls now[%6d] ave[%6d]",
    item->GetName(),
    FIELD_DEBUG_TICK_US_GET(index),
    FIELD_DEBUG_TICK_US_AVE_GET(index)
    );

  return workStr;
}

static void Reset( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED(item);
  GFL_UNUSED(userWork);

  GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager	= pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	pButton	= pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    FIELD_DEBUG_TICK_RESET();
  }
}

GFL_NAMESPACE_END( Tick )

GFL_NAMESPACE_END( Item )

/**
* @brief コンストラクタ
* フィールドデバッグメニューの構築を実行します
*
* @param  pHeap         メニュー構築に使用するヒープ
* @param  pGameManager  ゲーム管理
*
* @return 無し
*/ 
FieldDebugMenu::FieldDebugMenu( gfl2::heap::HeapBase* pHeap, GameSys::GameManager* pGameManager ) :
m_pHeap( pHeap ),
m_pGameManager( pGameManager ),
m_pFieldRootGroup( NULL )
{
  // FieldグループをTopに追加
  m_pFieldRootGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"Field", NULL );


  // DrawInfoグループをFieldに追加
  {
    gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"DrawInfo", m_pFieldRootGroup ); 

    // @todo 値を表示したいだけだが、値操作関数を使いまわしている、独自表示関数に置き換える
    gfl2::debug::DebugWin_AddItemValueU32( 
      pGropWork, 
      m_pHeap,
      L"drawbleNodeNum",
      &FieldDebugTypes::s_DrawInfo.drawbleNodeNum,
      0,        // 最小値
      999999,   // 最大値
      0         // 変更量
      );

    gfl2::debug::DebugWin_AddItemValueU32( 
      pGropWork, 
      m_pHeap,
      L"visibleNodeNum",
      &FieldDebugTypes::s_DrawInfo.visibleNodeNum,
      0,        // 最小値
      999999,   // 最大値
      0         // 変更量
      );

    gfl2::debug::DebugWin_AddItemValueU32( 
      pGropWork, 
      m_pHeap,
      L"visibleTagNum",
      &FieldDebugTypes::s_DrawInfo.visibleTagNum,
      0,        // 最小値
      999999,   // 最大値
      0         // 変更量
      );
    
    gfl2::debug::DebugWin_AddItemUpdateFunc( 
      pGropWork,
      m_pHeap,
      L"transformNodeTraverse",
      NULL, 
      NULL,
      NULL
      );

    gfl2::debug::DebugWin_AddItemValueInt( 
      pGropWork, 
      m_pHeap,
      L"-SkipCheck",
      &Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::MODEL_TRAVERSE_MODE ],
      0,        // 最小値
      1,        // 最大値
      1         // 変更量
      );

    gfl2::debug::DebugWin_AddItemValueU32( 
      pGropWork, 
      m_pHeap,
      L"-TraverseCount",
      &FieldDebugTypes::s_DrawInfo.transformNodeTraverseCount,
      0,        // 最小値
      999999,   // 最大値
      0         // 変更量
      );

    gfl2::debug::DebugWin_AddItemValueU32( 
      pGropWork, 
      m_pHeap,
      L"-WorldMatrixUpdateCount",
      &FieldDebugTypes::s_DrawInfo.transformNodeWorldMatrixUpdateCount,
      0,        // 最小値
      999999,   // 最大値
      0         // 変更量
      );

    gfl2::debug::DebugWin_AddItemValueU32( 
      pGropWork, 
      m_pHeap,
      L"-WorldMatrixUpdateSkipCount",
      &FieldDebugTypes::s_DrawInfo.transformNodeWorldMatrixUpdateSkipCount,
      0,        // 最小値
      999999,   // 最大値
      0         // 変更量
      );

  }

  // DrawSwitchグループをFieldに追加
  {
    // FieldDebugTypes::s_DrawSwitchTableを0..1の間で変更できるように追加する
    gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"DrawSwitch", m_pFieldRootGroup, FieldDebugTypes::DRAW_SWITCH_MAX ); 

    for( u32 i = 0; i < FieldDebugTypes::DRAW_SWITCH_MAX; i++ )
    {
      gfl2::debug::DebugWin_AddItemValueInt( 
        pGropWork, 
        m_pHeap,
        FieldDebugTypes::GetDrawSwitchLabel( static_cast< FieldDebugTypes::DrawSwitch >( i ) ),
        &FieldDebugTypes::s_DrawSwitchTable[ i ],
        0,  // 最小値
        FieldDebugTypes::GetDrawSwitchCountMax( static_cast< FieldDebugTypes::DrawSwitch >( i ) ),  // 最大値
        1   // 変更量
        );
    }

    //
    // FieldDebugTypes::s_DrawSwitchTable以外でDrawSwitchに追加したい物がある場合はここに追加、Reset関数にも忘れずに、グループ生成時の項目数追加も忘れずに
    //

    pGropWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"DrawSwitchEx", m_pFieldRootGroup );

    // DrawSwitchの項目の値を全て初期値にする
    gfl2::debug::DebugWin_AddItemUpdateFunc( 
      pGropWork,
      m_pHeap,
      L"Reset",
      NULL, 
      Item::DrawSwitch::Reset,
      NULL
      );
    
    // DrawSwitchの値を全て0にする
    gfl2::debug::DebugWin_AddItemUpdateFunc( 
      pGropWork,
      m_pHeap,
      L"AllZero",
      NULL, 
      Item::DrawSwitch::AllZero,
      NULL
      );
  }

  // フィールド関連のDumpを行うグループを追加
  {
    gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"Dump", m_pFieldRootGroup );
  
    // プレイヤー情報をDump
    gfl2::debug::DebugWin_AddItemUpdateFunc( 
      pGropWork,
      m_pHeap,
      L"PlayerInfo",
      NULL, 
      Item::Dump::PlayerInfo,
      NULL
      );

    // カメラ情報をDump
    gfl2::debug::DebugWin_AddItemUpdateFunc( 
      pGropWork,
      m_pHeap,
      L"CameraInfo",
      NULL, 
      Item::Dump::CameraInfo,
      NULL
      );

    // シーングラフにぶら下がっているノード数をDump
    gfl2::debug::DebugWin_AddItemUpdateFunc( 
      pGropWork,
      m_pHeap,
      L"NodeCount",
      NULL, 
      Item::Dump::NodeCount,
      NULL
      );

    gfl2::debug::DebugWin_AddItemUpdateFunc( 
      pGropWork,
      m_pHeap,
      L"MemoryCollection",
      NULL, 
      Item::Dump::MemoryCollection,
      NULL
      );

    gfl2::debug::DebugWin_AddItemUpdateFunc( 
      pGropWork,
      m_pHeap,
      L"ModelInfo",
      NULL, 
      Item::Dump::ModelInfo,
      NULL
      );
  } 

  // 2015/03用、処理負荷計測用機能を追加
  {
    gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"Performance_20150323", m_pFieldRootGroup );
  
    // フィールドの状態を計測状態にする
    gfl2::debug::DebugWin_AddItemUpdateFunc( 
      pGropWork,
      m_pHeap,
      L"SetUp",
      NULL, 
      Item::Performance::SetUp,
      NULL
      );

    // フィールドの状態を計測状態にする
    gfl2::debug::DebugWin_AddItemUpdateFunc( 
      pGropWork,
      m_pHeap,
      L"SetUp2",
      NULL, 
      Item::Performance::SetUp2,
      NULL
      );

    gfl2::debug::DebugWin_AddItemUpdateFunc( 
      pGropWork,
      m_pHeap,
      L"SetUp3",
      NULL, 
      Item::Performance::SetUp3,
      NULL
      );

    // 地形計測用
    {
      gfl2::debug::DebugWin_AddItemUpdateFunc( 
        pGropWork,
        m_pHeap,
        L"SetUpTerrain1",
        NULL, 
        Item::Performance::SetUpTerrain1,
        NULL
        );

      gfl2::debug::DebugWin_AddItemUpdateFunc( 
        pGropWork,
        m_pHeap,
        L"SetUpTerrain2",
        NULL, 
        Item::Performance::SetUpTerrain2,
        NULL
        );

      gfl2::debug::DebugWin_AddItemUpdateFunc( 
        pGropWork,
        m_pHeap,
        L"SetUpTerrain3",
        NULL, 
        Item::Performance::SetUpTerrain3,
        NULL
        );
    }

    // Enc草計測用
    {
      gfl2::debug::DebugWin_AddItemUpdateFunc( 
        pGropWork,
        m_pHeap,
        L"SetUpEnc1",
        NULL, 
        Item::Performance::SetUpEnc1,
        NULL
        );

      gfl2::debug::DebugWin_AddItemUpdateFunc( 
        pGropWork,
        m_pHeap,
        L"SetUpEnc2",
        NULL, 
        Item::Performance::SetUpEnc2,
        NULL
        );

      gfl2::debug::DebugWin_AddItemUpdateFunc( 
        pGropWork,
        m_pHeap,
        L"SetUpEnc3",
        NULL, 
        Item::Performance::SetUpEnc3,
        NULL
        );
    }
    // フィールドの状態をミップマップ計測状態にする
    gfl2::debug::DebugWin_AddItemUpdateFunc( 
      pGropWork,
      m_pHeap,
      L"SetUpMipMap",
      NULL, 
      Item::Performance::SetUpMipMap,
      NULL
      );
    

  }
  // フィールド関連のサウンドセットを行うグループを追加
  {
    gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"Sound", m_pFieldRootGroup );
  
    // BGM停止
    gfl2::debug::DebugWin_AddItemUpdateFunc( 
      pGropWork,
      m_pHeap,
      L"BGMStop",
      NULL, 
      Item::SoundDebug::BGMStop,
      NULL
      );

    // BGM再開
    gfl2::debug::DebugWin_AddItemUpdateFunc( 
      pGropWork,
      m_pHeap,
      L"BGMStart",
      NULL, 
      Item::SoundDebug::BGMStart,
      NULL
      );
  }
  // フィールド関連のカメラ操作を行うグループを追加
  {
    gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"Camera", m_pFieldRootGroup );
  
    // 正射影(上)にする
    gfl2::debug::DebugWin_AddItemUpdateFunc( 
      pGropWork,
      m_pHeap,
      L"SetTop",
      NULL, 
      Item::Camera::SetTop,
      NULL
      );

    // デバッグカメラ終了
    gfl2::debug::DebugWin_AddItemUpdateFunc( 
      pGropWork,
      m_pHeap,
      L"EndDebug",
      NULL, 
      Item::Camera::EndDebug,
      NULL
      );
    
    // 通常カメラに戻す
    gfl2::debug::DebugWin_AddItemUpdateFunc( 
      pGropWork,
      m_pHeap,
      L"SetDefault",
      NULL, 
      Item::Camera::SetDefault,
      NULL
      );
  }
  // フィールドで計測したTickの表示を行うグループを追加
  {
    gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"Tick View", m_pFieldRootGroup );
    pGropWork->SetOpenGroupFunc( Item::Tick::OpenGroupFunc );

    for( u32 i = 0; i < FieldDebugTypes::TickDefine::MAX; ++i)
    {
      const u32 WORK_STR_LENGTH = 64;
      wchar_t workStr[WORK_STR_LENGTH];
#if defined(GF_PLATFORM_CTR)
      swprintf( workStr , WORK_STR_LENGTH, L"%s" , FieldDebugTypes::GetTickLabel( static_cast<FieldDebugTypes::TickDefine::Index>( i)));
#elif defined(GF_PLATFORM_WIN32)
      size_t ret;
      mbstowcs_s( &ret, workStr, WORK_STR_LENGTH, FieldDebugTypes::GetTickLabel( static_cast<FieldDebugTypes::TickDefine::Index>( i)), _TRUNCATE);
      swprintf( workStr , WORK_STR_LENGTH, L"%s" , workStr);
#endif

#if 0
      // ラベル名表示
      gfl2::debug::DebugWin_AddItemUpdateFunc( 
        pGropWork,
        m_pHeap,
        workStr,
        NULL,
        NULL,
        Item::Tick::DrawLabelName
        );
#endif
      // チック表示
      gfl2::debug::DebugWin_AddItemUpdateFunc( 
        pGropWork,
        m_pHeap,
        workStr,
        reinterpret_cast<void*>( i), 
        NULL,
        Item::Tick::DrawTickValue
        );
    }

    // チックリセット
    gfl2::debug::DebugWin_AddItemUpdateFunc( 
      pGropWork,
      m_pHeap,
      L"Reset",
      NULL,
      Item::Tick::Reset,
      NULL
      );
  }

}

/**
* @brief デストラクタ
*
* @return 無し
*/
FieldDebugMenu::~FieldDebugMenu( void )
{
  // Fieldグループ以下を削除する
  if( m_pFieldRootGroup )
  {
    gfl2::debug::DebugWin_RemoveGroup( m_pFieldRootGroup );
    m_pFieldRootGroup = NULL;
  }
}


GFL_NAMESPACE_END( debug )
GFL_NAMESPACE_END( Field )


#endif // #if PM_DEBUG