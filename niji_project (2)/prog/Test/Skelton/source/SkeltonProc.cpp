// ============================================================================
/*
* @file SkeltonProc.cpp
* @brief SkeltonのメインProcです。
* @date 2015.10.02
* @author Junya Ikuta
*/
// ============================================================================
#if defined(GF_PLATFORM_WIN32)

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ParserUtil.h>
#include "Test/Skelton/include/SkeltonProc.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "System/include/HeapDefine.h"
#include "System/include/GflUse.h"



// RenderingEngine
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfEnvData.h>

#include <system/include/InputDevice/gfl2_Pad.h>
#include <system/include/InputDevice/gfl2_KeyBoard.h>


// skelton
#include "Test/Skelton/include/SkeltonRscManager.h"
#include "Test/Skelton/include/Field/SkeltonFieldMapBlock.h"
#include "Test/Skelton/include/SkeltonDebugInfo.h"
#include "Test/Skelton/include/Field/SkeltonModelManager.h"
#include "Test/Skelton/include/Field/SkeltonPlayerControl.h"
#include "Test/Skelton/include/Field/Camera/SkeltonCamera.h"
#include "Test/Skelton/include/Field/Camera/SkeltonCameraDataManager.h"
#include "Test/Skelton/include/Field/MapJump/SkeltonJumpCollisionManager.h"
#include "Test/Skelton/include/Field/MapJump/SkeltonJumpDataManager.h"
#include "Test/Skelton/include/Field/SkeltonModel.h"
#include "Test/Skelton/include/Field/MapJump/SkeltonEventMapJump.h"

GFL_NAMESPACE_BEGIN(Test)
  GFL_NAMESPACE_BEGIN(Skelton)

  // @todo フルパスを消す
  // output
  static const char* const FILE_PATH_CAMERA_INFO("R:/home/momiji/exec/data_win32/skelton/resource/config/camera_info.txt"); ///< カメラ情報の出力先
static const char* const FILE_PATH_PLAYER_INFO("R:/home/momiji/exec/data_win32/skelton/resource/config/player_info.txt");    ///< 自機情報の出力先

// load
static const char* const FILE_PATH_FIELD_SETTIG("R:/home/momiji/exec/data_win32/skelton/resource/config/field.txt");              ///< フィールド設定の出力・読み込み先
static const char* const FILE_PATH_FIELD_START_SETTIG("R:/home/momiji/exec/data_win32/skelton/resource/config/field_start.txt");  ///< フィールド設定の出力・読み込み先(開始時のみ)
static const char* const FILE_PATH_DEFAULT_ENV("R:/home/momiji/exec/data_win32/skelton/resource/map/default_env.gfenv");          ///< 環境設定ファイル、マップ別に指定されてなければこれを使用する

static const char* const PLAYER_SETTIG_LABEL_NAME_TABLE[ PlayerControl::PLAYER_MOVE_MAX ] = {
  "WALK_SLOW",
  "WALK", 
  "WALK_FAST", 
  "RUN", 
  "DEBUG_RUN" 
};


SkeltonProc::SkeltonProc() :
  mRenderPathCnt( 0 )

  // 環境関連
  ,m_pEnvNode( NULL )
  ,m_pDrawEnvNode( NULL )
  ,m_pEnvAnimationNode( NULL )
  ,m_isInitDrawEnvAnimation( false )

  ,mHeap( NULL )
  ,mAllocator( NULL )
  ,mModelRenderPath( NULL )
{
}


SkeltonProc::~SkeltonProc()
{
}


gfl2::proc::Result SkeltonProc::InitFunc( gfl2::proc::Manager* pManager )
{
  gfl2::proc::Result procResult = gfl2::proc::RES_CONTINUE;
  GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();

  switch( GetSubSeq() )
  {
  case 0:
    {
      // メモリ生成
      gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
      mHeap = GFL_CREATE_LOCAL_HEAP_NAME(devHeap,0x1000000,gfl2::heap::HEAP_TYPE_EXP,false,"Skelton");
      mAllocator = GFL_NEW(mHeap)System::nijiAllocator(mHeap);
      m_pNwAllocator = GFL_NEW(mHeap)gfl2::heap::NwAllocator(mHeap);

      // 描画パス生成
      {
        mModelRenderPath = GFL_NEW(mHeap)System::ModelRenderPath(mAllocator,32);

        gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::InitDescription desc;
        desc.m_DrawManagerDesc.m_NodeStackSize	  = 64;
        desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 16;

        mModelRenderPath->Initialize( mAllocator, desc );

        m_UtilTextRenderPath = GFL_NEW(mHeap) gfl2::util::UtilTextRenderPath( mAllocator );
        m_DebugDrawUtilText =  GFL_NEW(mHeap) gfl2::util::DrawUtilText( m_pNwAllocator, m_pNwAllocator );
        m_DebugDrawUtilText->Initialize( m_pNwAllocator, m_pNwAllocator, m_UtilTextRenderPath );
        m_DebugDrawUtilText->SetFont( System::GflUse::GetSystemFont()->GetResFont() );

      }

      // カメラ行列(仮)
      {
        u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );
        u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );

        mProjMtx[0] = gfl2::math::Matrix44::GetPerspective(
          gfl2::math::ConvDegToRad(35.0f),		// 視野角35°
          (f32)displayWidth / (f32)displayHeight,
          1.0f,							// 前方投影面までの距離
          12000.0f);							// 後方投影面までの距離


        mViewMtx[0] = gfl2::math::Matrix34::GetLookAt(
          gfl2::math::Vector3( 460.0f, 0.0f, 3800.0f ) + gfl2::math::Vector3( 0.0f, 1000.0f, 1000.0f ),
          gfl2::math::Vector3( 460.0f, 0.0f, 3800.0f ),
          gfl2::math::Vector3( 0.0f, 1.0f, 0.0f )
          );
      }


      // オブジェクト生成
      {
        m_pCamera = GFL_NEW( mHeap ) SkeltonCamera();
        m_pCameraDataManager = GFL_NEW( mHeap ) SkeltonCameraDataManager();

        m_pSkeltonRscManager = GFL_NEW( mHeap ) SkeltonRscManager( mHeap, mAllocator );
        m_pSkeltonFieldMapBlockManager = GFL_NEW( mHeap ) SkeltonFieldMapBlockManager( mHeap, mAllocator );

        m_pSkeltonFieldMapBlockManager->Initialize( NULL, NULL, m_pSkeltonRscManager, mModelRenderPath );
        m_pSkeltonFieldMapBlockManager->SetupSampleMaps();

        m_pSkeltonModelManager = GFL_NEW(mHeap) SkeltonModelManager(mHeap, mAllocator);
        m_pSkeltonModelManager->Initialize( NULL, NULL, m_pSkeltonRscManager, mModelRenderPath );

        m_player = m_pSkeltonModelManager->CreateSkeltonModel( SMDL_PLAYER, chara::RSCID_PLAYER_MALE );
        m_player->GetTransform()->SetTranslation( SkeltonDebugInfo::m_PlayerStartPosition );
        m_player->GetTransform()->SetAngle( SkeltonDebugInfo::m_PlayerStartAngle );

        m_playerControl = GFL_NEW( mHeap ) PlayerControl();
        m_playerControl->activate( m_player, m_pCamera );

        m_pMapJumpDataManager = GFL_NEW( mHeap ) JumpDataManager();
        m_pMapJumpCollisionDataManager = GFL_NEW( mHeap ) CollisionDataManager();
        SkeltonDebugInfo::m_pMapJumpCollisionDataManager = m_pMapJumpCollisionDataManager;


        m_pCamera->Initialize( m_player );
        SIZE dispSize;
        dispSize.cx = 400.0f;
        dispSize.cy = 240.0f;
        SkeltonDebugInfo::m_AngleOfView = 35.0f;
        float aspect = (float)dispSize.cx / (float)dispSize.cy;
        SkeltonCamera::ProjectionSetting projectionSetting = 
        {
          1.0f,					  // near
          100000.0f,			// far
          aspect,					// aspect
          SkeltonDebugInfo::m_AngleOfView, // aov
        };
        m_pCamera->SetProjectionSetting( projectionSetting );

        // カメラデータをロード (　LoadSettingで行う )
#if 0
        c16 pathWork[1024];
        wcscpy_s( pathWork, 1024, SkeltonDebugInfo::m_MapRootPath );
        wcscat_s( pathWork, 1024, L"\\camera_data.txt" );

        m_pCameraDataManager->Initialize();
        m_pCameraDataManager->LoadData( pathWork );
#endif
      }


      //
      // 起動時のみの設定更新
      //

      m_pDebugSkeltonNameMangling = GFL_NEW( mHeap ) SkeltonNameMangling::GroupNode( "field" );

      // シンボルを登録
      {
        // 地形
        m_pDebugSkeltonNameMangling->AddSkeltonNameMangling( &SkeltonDebugInfo::m_MapScale, "map_scale" );


        // エッジ表示
        m_pDebugSkeltonNameMangling->AddSkeltonNameMangling( &SkeltonDebugInfo::m_EdgeScale, "edge_scale" );

        SkeltonNameMangling::GroupNode* pMapGroup = m_pDebugSkeltonNameMangling->AddGroup( "MAP" );
        if( pMapGroup )
        {
          SkeltonNameMangling::GroupNode* pResourceGroup = pMapGroup->AddGroup( "resource" );
          if( pResourceGroup )
          {
            pResourceGroup->AddSkeltonNameMangling( "folder_path", &SkeltonDebugInfo::m_MapRootPath[0], GFL_NELEMS( SkeltonDebugInfo::m_MapRootPath ) );
          }
        }

        // 自機
        SkeltonNameMangling::GroupNode* pPlayerGroup = m_pDebugSkeltonNameMangling->AddGroup( "PLAYER" );
        if( pPlayerGroup )
        {
          // 初期座標設定
          SkeltonNameMangling::GroupNode* pPositionGroup = pPlayerGroup->AddGroup( "position" );
          if( pPositionGroup )
          {
            // @todo Vector型はサポートしたい
            pPositionGroup->AddSkeltonNameMangling( &SkeltonDebugInfo::m_PlayerStartPosition.x, "x" );
            pPositionGroup->AddSkeltonNameMangling( &SkeltonDebugInfo::m_PlayerStartPosition.y, "y" );
            pPositionGroup->AddSkeltonNameMangling( &SkeltonDebugInfo::m_PlayerStartPosition.z, "z" );
          }
          pPlayerGroup->AddSkeltonNameMangling(  &SkeltonDebugInfo::m_PlayerStartAngle, "angle" );

          // Bボタン割り当て
          SkeltonNameMangling::SkeltonNameManglingS32Node* pWorkNode = pPlayerGroup->AddSkeltonNameMangling( &SkeltonDebugInfo::m_PlayerMoveByDush, "b_mode" );
          for( u32 i = 0; i < PlayerControl::PLAYER_MOVE_MAX; i++ )
          {
            // @todo インスタンスごとに持つので、無駄使い
            pWorkNode->AddHash( PLAYER_SETTIG_LABEL_NAME_TABLE[i], i );
          }

          // 移動速度
          SkeltonNameMangling::GroupNode* pMoveModeGroup = pPlayerGroup->AddGroup( "MOVE_MODE" );
          if( pMoveModeGroup )
          {
            for( u32 i = 0; i < PlayerControl::PLAYER_MOVE_MAX; i++ )
            {
              SkeltonNameMangling::GroupNode* pMoveModeChildGroup = pMoveModeGroup->AddGroup( PLAYER_SETTIG_LABEL_NAME_TABLE[i] );

              SkeltonDebugInfo::PlayerMoveSetting* pMoveSetting = &SkeltonDebugInfo::m_PlayerMoveSettingTable[ i ];

              pMoveModeChildGroup->AddSkeltonNameMangling( &pMoveSetting->moveSpeed, "move_speed" );
              pMoveModeChildGroup->AddSkeltonNameMangling( &pMoveSetting->motionSpeed, "motion_speed" );
              pMoveModeChildGroup->AddSkeltonNameMangling( &pMoveSetting->motionBaseMoveSpeed, "motion_move_speed" );
              pMoveModeChildGroup->AddSkeltonNameMangling( &pMoveSetting->motionSpeedAutomaticFlag, "motion_speed_auto" );
              pMoveModeChildGroup->AddSkeltonNameMangling( &pMoveSetting->stickLevel, "stick_value" );
              pMoveModeChildGroup->AddSkeltonNameMangling( "name", &pMoveSetting->name[0], GFL_NELEMS( pMoveSetting->name ) );

              SkeltonNameMangling::SkeltonNameManglingS32Node* pWorkNode = pPlayerGroup->AddSkeltonNameMangling( &pMoveSetting->motionId, "motion_id" );
              pWorkNode->AddHash( "MOTION_ID_WALK", chara::MOTION_ID_WALK );
              pWorkNode->AddHash( "MOTION_ID_WALK_FAST", chara::MOTION_ID_WALK_FAST );
              pWorkNode->AddHash( "MOTION_ID_RUN", chara::MOTION_ID_RUN );

            }
          } // if( pMoveModeGroup )
        }

#if 0
        // カメラ
        SkeltonNameMangling::GroupNode* pCameraGroup = m_pDebugSkeltonNameMangling->AddGroup( "CAMERA" );
        if( pCameraGroup )
        {
          pCameraGroup->AddSkeltonNameMangling( &m_camera->m_distance, "distance" );

          SkeltonNameMangling::GroupNode* pRotationGroup = pCameraGroup->AddGroup( "rotation" );
          if( pRotationGroup )
          {
            // @todo Vector型はサポートしたい
            pCameraGroup->AddSkeltonNameMangling( &m_camera->m_cameraRotation.x, "X" );
            pCameraGroup->AddSkeltonNameMangling( &m_camera->m_cameraRotation.y, "y" );
            pCameraGroup->AddSkeltonNameMangling( &m_camera->m_cameraRotation.z, "z" );
          }

          SkeltonNameMangling::GroupNode* pTargetOffsetGroup = pCameraGroup->AddGroup( "target_offset" );
          if( pTargetOffsetGroup )
          {
            pTargetOffsetGroup->AddSkeltonNameMangling( &SkeltonDebugInfo::m_CameraTargetOffset.x, "X" );
            pTargetOffsetGroup->AddSkeltonNameMangling( &SkeltonDebugInfo::m_CameraTargetOffset.y, "y" );
            pTargetOffsetGroup->AddSkeltonNameMangling( &SkeltonDebugInfo::m_CameraTargetOffset.z, "z" );
          }
        }
#endif 

      } // シンボルを登録

      procResult = gfl2::proc::RES_FINISH;
      break;
    }
  }

  return procResult;
}


gfl2::proc::Result SkeltonProc::EndFunc( gfl2::proc::Manager* pManager )
{
  gfl2::proc::Result procResult = gfl2::proc::RES_CONTINUE;
  GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();

  switch( GetSubSeq() )
  {
  case 0:
    {
      GFL_SAFE_DELETE( m_pDebugSkeltonNameMangling );

      // 環境
      DeleteEnv();
      m_pSkeltonModelManager->Terminate();
      m_pSkeltonFieldMapBlockManager->Terminate();

      m_pMapJumpDataManager->Terminate();
      m_pMapJumpCollisionDataManager->Terminate();


      AddSubSeq();
      break;
    }

  case 1:
    {
      GFL_SAFE_DELETE( m_pMapJumpDataManager );
      GFL_SAFE_DELETE( m_pMapJumpCollisionDataManager );

      if( m_playerControl )
      {
        m_playerControl->activate( NULL, NULL );
      }

      GFL_SAFE_DELETE( m_pCameraDataManager );
      GFL_SAFE_DELETE( m_pCamera );

      GFL_SAFE_DELETE( m_playerControl );
      GFL_SAFE_DELETE( m_pSkeltonModelManager );

      // オブジェクト
      GFL_SAFE_DELETE( m_pSkeltonFieldMapBlockManager );
      GFL_SAFE_DELETE( m_pSkeltonRscManager );

      // 描画パス破棄
      GFL_SAFE_DELETE( m_DebugDrawUtilText );
      GFL_SAFE_DELETE(m_UtilTextRenderPath);
      GFL_SAFE_DELETE(mModelRenderPath);

      // メモリ破棄
      GFL_SAFE_DELETE(m_pNwAllocator);
      GFL_SAFE_DELETE(mAllocator);
      GFL_DELETE_HEAP(mHeap);

      procResult = gfl2::proc::RES_FINISH;
      break;
    }
  }

  return procResult;
}

gfl2::proc::Result SkeltonProc::UpdateFunc( gfl2::proc::Manager* pManager )
{
  gfl2::proc::Result procResult = gfl2::proc::RES_CONTINUE;
  GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();

  m_DebugDrawUtilText->ClearDrawText();

  switch( GetSubSeq() )
  {
  case 0:
    {
      if( LoadSetting() )
      {
        SetSubSeq( 2 );
        // 現在の設定を初期値扱いにする
        // m_camera->SaveForDefaultSetting();
        SkeltonDebugInfo::m_CameraTargetDefaultOffset = SkeltonDebugInfo::m_CameraTargetOffset;
        break;
      }
      else
      {
        SetSubSeq( 1 );
        // 現在の設定を初期値扱いにする
        // m_camera->SaveForDefaultSetting();
        SkeltonDebugInfo::m_CameraTargetDefaultOffset = SkeltonDebugInfo::m_CameraTargetOffset;
        break;
      }

      break;
    }

  case 1:
    {
      //
      // 更新
      // 

      if( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameEventManager()->IsExists() == false )
      {
        // 設定再読込み
        if( gfl2::system::InputDevice::KeyBoard::IsTrigger( gfl2::system::InputDevice::KeyBoardKey::KBKEY_U ) )
        {
          if( LoadSetting() )
          {
            SetSubSeq( 2 );
            break;
          }
        }

        // 地形リソースを読み込み直す
        if( gfl2::system::InputDevice::KeyBoard::IsTrigger( gfl2::system::InputDevice::KeyBoardKey::KBKEY_9 ) )
        {
          m_pSkeltonFieldMapBlockManager->ReloadReq();
          CreateEnv( SkeltonDebugInfo::m_MapRootPath );
          SetSubSeq( 2 );
          break;
        }

        // 操作
        controlPC();
      }

      if( m_playerControl )
      {
        m_playerControl->Update();
      }

      if( m_pSkeltonFieldMapBlockManager )
      {
        m_pSkeltonFieldMapBlockManager->UpdateAllModels( 0 );
      }

      // キャラ更新
      m_pSkeltonModelManager->UpdateAllModels( 1 );

      // 環境アニメを更新
      if( m_isInitDrawEnvAnimation )
      {
        m_DrawEnvAnimationController.Update();
      }

      // カメラコリジョン更新
#if 0
      if( SkeltonDebugInfo::m_DebugInfoDebugMode == SkeltonDebugInfo::DEBUG_MODE_CAMERA )
      {
        if( m_pCamera->IsDebug() == false )
        {
          // m_pCamera->ChangeDebugCamera();
        }
        m_pCamera->SetDebug( true );
      }
      else
#endif
      {
        if( m_pCamera->IsDebug() )
        {
          m_pCamera->SetDebug( false );
          UpdateCameraCollision( true );
        }
        else
        {
          UpdateCameraCollision();
        }
      }

      // マップジャンプコリジョン更新
      UpdateMapJumpCollision();

      // カメラ更新
      m_pCamera->Update();

      // 自機情報
      SkeltonDebugInfo::m_DebugInfoPlayerPos = m_player->GetTransform()->GetTranslation();
      SkeltonDebugInfo::m_DebugInfoPlayerDir = m_player->GetTransform()->GetAngle();

      //モデル計算
      gfl2::renderingengine::renderer::RenderingPipeLine::Update();
      gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast();

      break;
    }

    // 地形リロード
  case 2:
    {
      if( m_pSkeltonFieldMapBlockManager->IsReloadOk() )
      {
        m_pSkeltonFieldMapBlockManager->ReloadSampleMaps();
        SetSubSeq( 1 );
      }

      //モデル計算
      gfl2::renderingengine::renderer::RenderingPipeLine::Update();
      gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast();

      break;
    }
  }

  SkeltonDebugInfo::OnDrawFont( m_DebugDrawUtilText );
  return procResult;
}

void SkeltonProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  // @todo 立体視の場合はPreDrawFuncでコマンドをセーブし、DrawFuncで実行する形にする 150218 ikuta_junya
  switch( displayNo )
  {
    // 左画面描画
  case gfl2::gfx::CtrDisplayNo::LEFT:
    {
      {
        gfl2::math::Matrix tmp;
        m_pCamera->GetViewMatrix( &tmp );
        tmp = tmp.Transpose();
        gfl2::renderingengine::renderer::RenderingPipeLine::SetViewMatrix( *(gfl2::math::Matrix34*)&tmp );	
        m_pCamera->GetProjectionMatrix( &tmp );
        tmp = tmp.Transpose();
        gfl2::renderingengine::renderer::RenderingPipeLine::SetProjectionMatrix( *(gfl2::math::Matrix44*)&tmp );

      }

      gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ displayNo ] );
      gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
      u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ displayNo ] );
      u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ displayNo ] );

      gfl2::renderingengine::renderer::RenderingPipeLine::SetRenderTarget( pBackBuffer );
      gfl2::renderingengine::renderer::RenderingPipeLine::SetDepthStencil( pDepthBuffer );
      gfl2::renderingengine::renderer::RenderingPipeLine::SetDisplaySize( displayWidth, displayHeight );

      // gfl2::renderingengine::renderer::RenderingPipeLine::SetProjectionMatrix( mProjMtx[0] );
      // gfl2::renderingengine::renderer::RenderingPipeLine::SetViewMatrix( mViewMtx[0] );	

      gfl2::renderingengine::renderer::RenderingPipeLine::Execute();
      break;
    } // case gfl2::gfx::CtrDisplayNo::LEFT

    // 右画面描画(立体視時のみ)
  case gfl2::gfx::CtrDisplayNo::RIGHT:
    break;
    // 下画面描画
  case gfl2::gfx::CtrDisplayNo::DOWN:
    break;
  } // switch( displayNo )
}

//============================================================================
//RenderingPipeLine
//============================================================================
//----------------------------------------------------------------------------
/**
*  @brief  最初のパスから描画できる状態にする
*/
//-----------------------------------------------------------------------------
b32 SkeltonProc::StartRenderPath(void)
{
  mRenderPathCnt = 0;
  return true;
}

//----------------------------------------------------------------------------
/**
*  @brief  現状のパスにあったシーンを返す
*  @return シーン
*/
//-----------------------------------------------------------------------------
gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* SkeltonProc::GetRenderPath(void)
{
  //メインの描画
  switch( mRenderPathCnt )
  {
  case 0:
    return mModelRenderPath;
  case 1:
    return m_UtilTextRenderPath;
  }
  return NULL;
}

//----------------------------------------------------------------------------
/**
*  @brief  次のパスへ遷移
*  @return trueなら次のパス、falseなら描画終了
*/
//-----------------------------------------------------------------------------
b32 SkeltonProc::NextRenderPath(void)
{
  mRenderPathCnt++;
  if( mRenderPathCnt >= 2 )
  {
    return false;
  }
  return true;
}


void SkeltonProc::controlPC( void )
{
  enum {MAINTIMER_ID = 128 };

  enum {
    PlayerPadNo = 0,
    NpcPadNo = 1,
  };

  //
  // 共通処理
  //

  SkeltonDebugInfo::m_CameraTargetDraw = false;
  // SkeltonDebugInfo::m_DebugInfoCameraStopFlag = false;

  // スティック入力値情報更新
#if 0
  const gfl2::system::InputDevice::StickPos& pos
    = gfl2::system::InputDevice::Pad::GetStick( PlayerPadNo, gfl2::system::InputDevice::Stick::Left );
  f32 inputValue = gfl2::math::Vector4( pos.x, 0.0f, pos.y, 0.0f ).Length();
  if( inputValue > 1.0f )
  {
    inputValue = 1.0f;
  }
  SkeltonDebugInfo::m_DebugInfoStickLevel = inputValue;
#endif
#if 1

#endif


  //
  // 自機操作
  //

  // イベントが発生していない時だけ、自機操作をできるようにする
  //if( m_gameManager->IsEventExists() == false )
  // デバッグ速度を適用
  m_playerControl->SetRunSpeed( SkeltonDebugInfo::m_DebugInfoPlayerRunSpeed );
  m_playerControl->SetWalkSpeed( SkeltonDebugInfo::m_DebugInfoPlayerWalkSpeed );

  SkeltonModel::Command r_cmd;

  const gfl2::system::InputDevice::StickPos& pos
    = gfl2::system::InputDevice::Pad::GetStick( PlayerPadNo, gfl2::system::InputDevice::Stick::Left );
  bool pc_run = gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::A );
  bool debug_run = gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::L1 );
  SkeltonModel::Command pc_cmd = m_playerControl->CreatePCCommand( 
    pos.x, pos.y, 
    pc_run, debug_run,
    m_pSkeltonFieldMapBlockManager->GetPrimary()->GetCollision() );
  m_player->SetCommand( pc_cmd );

  // ガッツポーズアニメ再生
  if( gfl2::system::InputDevice::Pad::IsPush( PlayerPadNo, gfl2::system::InputDevice::Button::B ) )
  {
    m_pSkeltonModelManager->GetSkeltonModel( SMDL_PLAYER )->ChangeAnimation( chara::MOTION_ID_SP, false, 0.0f );

#if 0
    gfl2::math::Vector4 pos( m_pSkeltonModelManager->GetSkeltonModel( SMDL_PLAYER )->GetTransform()->GetTranslation() );
    f32 angle( m_pSkeltonModelManager->GetSkeltonModel( SMDL_PLAYER )->GetTransform()->GetAngle() );
    GFL_PRINT("player pos x : %f y : %f z : %f angle : %f \n", pos.x, pos.y, pos.z, angle );
#endif
  }
  r_cmd = m_playerControl->CreateNetworkCommand( pos.x, pos.y, pc_run );


  // モード切替
#if 0
  if( gfl2::system::InputDevice::Pad::IsPush( PlayerPadNo, gfl2::system::InputDevice::Button::Y ) )
  {
    if( m_IsSnapMode )
    {
      m_IsSnapMode = false;
      UpdateCameraCollision( true );
      m_IsFPS15Mode = false;

      // 移動操作で向きが変更されないようにする、解除
      m_pSkeltonModelManager->GetSkeltonModel( SMDL_PLAYER )->SetAngleLock( false );
    }
    else
    {
      m_IsSnapMode = true;
      m_pCamera->ChangeSnapCamera();

      // 移動操作で向きが変更されないようにする
      m_pSkeltonModelManager->GetSkeltonModel( SMDL_PLAYER )->SetAngleLock( true );
    }
  }

  // スナップモードのみ、描画FPSを15FPS風にする機能
  if( m_IsSnapMode && gfl2::system::InputDevice::Pad::IsPush( PlayerPadNo, gfl2::system::InputDevice::Button::X ) )
  {
    m_IsFPS15Mode = !m_IsFPS15Mode;
  }
#endif

  // 設定出力
  if( gfl2::system::InputDevice::KeyBoard::IsTrigger( gfl2::system::InputDevice::KeyBoardKey::KBKEY_X ) )
  {
    OutPutCameraData();
    OutPutPlayerData();
  }

  //
  // デバッグモード切替
  //
#if 1
  if( gfl2::system::InputDevice::KeyBoard::IsTrigger( gfl2::system::InputDevice::KeyBoardKey::KBKEY_ARROW_L ) || gfl2::system::InputDevice::Pad::IsPush( PlayerPadNo, gfl2::system::InputDevice::Button::Left ) )
  {
    if( SkeltonDebugInfo::m_DebugInfoDebugMode == SkeltonDebugInfo::DEBUG_MODE_START )
    {
      SkeltonDebugInfo::m_DebugInfoDebugMode = SkeltonDebugInfo::DEBUG_MODE_END;
    }
    else
    {
      SkeltonDebugInfo::m_DebugInfoDebugMode = static_cast<SkeltonDebugInfo::DebugMode>( SkeltonDebugInfo::m_DebugInfoDebugMode - 1 );
    }
  }
  else if( gfl2::system::InputDevice::KeyBoard::IsTrigger( gfl2::system::InputDevice::KeyBoardKey::KBKEY_ARROW_R ) || gfl2::system::InputDevice::Pad::IsPush( PlayerPadNo, gfl2::system::InputDevice::Button::Right ) )
  {
    if( SkeltonDebugInfo::m_DebugInfoDebugMode == SkeltonDebugInfo::DEBUG_MODE_END )
    {
      SkeltonDebugInfo::m_DebugInfoDebugMode = SkeltonDebugInfo::DEBUG_MODE_START;
    }
    else
    {
      SkeltonDebugInfo::m_DebugInfoDebugMode = static_cast<SkeltonDebugInfo::DebugMode>( SkeltonDebugInfo::m_DebugInfoDebugMode + 1 );
    }
  }
#endif

  // コリジョンモデル表示切替
  if( gfl2::system::InputDevice::KeyBoard::IsTrigger( gfl2::system::InputDevice::KeyBoardKey::KBKEY_Z ) )
  {
    m_pSkeltonFieldMapBlockManager->GetPrimary()->VisibleModeCountUp();
  }

  // 表示情報更新
  SkeltonDebugInfo::m_InfoMapVisibleMode = m_pSkeltonFieldMapBlockManager->GetPrimary()->GetVisibleMode();

#if 0
  if( pUiSys->checkKeyboardTrgOn( KBKEY_G ) )
  {
    gfl2::heap::ClearDefaultDeleteCount();
    u64 before = gfl2::system::Timer::PerformanceCounter::GetMicroSecond();
    m_pSkeltonModelManager->CreateSkeltonModel(SMDL_POKE4, chara::RSCID_TEST_TEST );
    u64 now = gfl2::system::Timer::PerformanceCounter::GetMicroSecond();

    GFL_PRINT("create movemodel time/delete count %d / %d", now - before, gfl2::heap::GetDefaultDeleteCount() );
    FIELD_LOG_PRINT( 2, "delete count %d", gfl2::heap::GetDefaultDeleteCount() );
  }
#endif

#if 0
  // 
  // 各モード別処理
  // 
  switch( SkeltonDebugInfo::m_DebugInfoDebugMode )
  {
  case SkeltonDebugInfo::DEBUG_MODE_PLAYER:
    {
#if 0
      // 主人公の姿を変更
      if( gfl2::system::InputDevice::Pad::IsPush( PlayerPadNo, gfl2::system::InputDevice::Button::R1 ) )
      {
        togglePlayer( m_pSkeltonModelManager, SMDL_PLAYER);
      }
#endif


      b32 triggerFlag = false;
      f32 changeSpeed = 0.01f;

      if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::X ) )
      {
        triggerFlag = true;
      }

      if( triggerFlag )
      {
        if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::A ) )
        {
          // 走りの速さをスピードアップ
          if( gfl2::system::InputDevice::Pad::IsPush( PlayerPadNo, gfl2::system::InputDevice::Button::R2 ) )
          {
            SkeltonDebugInfo::m_DebugInfoPlayerRunSpeed += changeSpeed;
          }
          if( gfl2::system::InputDevice::Pad::IsPush( PlayerPadNo, gfl2::system::InputDevice::Button::L2 ) )
          {
            SkeltonDebugInfo::m_DebugInfoPlayerRunSpeed -= changeSpeed;
          }
        }
        else
        {
          // 歩きの速さをスピードアップ
          if( gfl2::system::InputDevice::Pad::IsPush( PlayerPadNo, gfl2::system::InputDevice::Button::R2 ) )
          {
            SkeltonDebugInfo::m_DebugInfoPlayerWalkSpeed += changeSpeed;
          }
          if( gfl2::system::InputDevice::Pad::IsPush( PlayerPadNo, gfl2::system::InputDevice::Button::L2 ) )
          {
            SkeltonDebugInfo::m_DebugInfoPlayerWalkSpeed -= changeSpeed;
          }
        }
      }
      else
      {
        if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::A ) )
        {
          // 走りの速さをスピードアップ
          if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::R2 ) )
          {
            SkeltonDebugInfo::m_DebugInfoPlayerRunSpeed += changeSpeed;
          }
          if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::L2 ) )
          {
            SkeltonDebugInfo::m_DebugInfoPlayerRunSpeed -= changeSpeed;
          }
        }
        else
        {

          // 歩きの速さをスピードアップ
          if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::R2 ) )
          {
            SkeltonDebugInfo::m_DebugInfoPlayerWalkSpeed += changeSpeed;
          }
          if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::L2 ) )
          {
            SkeltonDebugInfo::m_DebugInfoPlayerWalkSpeed -= changeSpeed;
          }
        }
      }

      // 設定を初期状態に戻す
      if( pUiSys->checkKeyboardTrgOn( KBKEY_0 ) )
      {
      }

      // 影のON/OFF
      if( pUiSys->checkKeyboardTrgOn( KBKEY_Z ) )
      {
        SkeltonDebugInfo::m_ShadowDrawFlag = !SkeltonDebugInfo::m_ShadowDrawFlag;
      }

#if defined(WIN32)
      // 自機情報をファイル出力
      if( pUiSys->checkKeyboardTrgOn( KBKEY_X ) )
      {
        OutPutPlayerData();
      }
#endif

#if 0
      if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::Up ) )
      {
        SkeltonDebugInfo::m_DebugInfoPlayerMotionStepFrame += 0.1f;
      }
      if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::Down ) )
      {
        SkeltonDebugInfo::m_DebugInfoPlayerMotionStepFrame -= 0.1f;
      }
#endif
      break;
    }
  case SkeltonDebugInfo::DEBUG_MODE_CAMERA:
    {
      f32 aovChange = 0.01f * SkeltonDebugInfo::m_CameraSpeed;

      // L1ボタン押されているなら加速
      if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::L1 ) )
      {
        aovChange *= 10.0f;
      }

      // カメラを停止する
      if( gfl2::system::InputDevice::Pad::IsPush( PlayerPadNo, gfl2::system::InputDevice::Button::X ) )
      {
        // @todo 
        // SkeltonDebugInfo::m_DebugInfoCameraStopFlag = !SkeltonDebugInfo::m_DebugInfoCameraStopFlag;
      }

      // カメラの画角を変更する
      if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::X ) )
      {
        SkeltonCamera::ProjectionSetting projectionSetting;
        m_pCamera->GetProjectionSetting( &projectionSetting );
        projectionSetting.aov -= aovChange;
        m_pCamera->SetProjectionSetting( projectionSetting );
      }
      if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::Y ) )
      {
        SkeltonCamera::ProjectionSetting projectionSetting;
        m_pCamera->GetProjectionSetting( &projectionSetting );
        projectionSetting.aov += aovChange;
        m_pCamera->SetProjectionSetting( projectionSetting );
      }

      // カメラを初期状態に戻す
      if( pUiSys->checkKeyboardTrgOn( KBKEY_0 ) )
      {
        UpdateCameraCollision( true );
      }

      // カメラ情報をログ出力
      if( pUiSys->checkKeyboardTrgOn(KBKEY_Z) )
      {
        m_camera->Dump();
      }

#if defined(WIN32)
      // カメラ設定をファイル出力
      if( pUiSys->checkKeyboardTrgOn(KBKEY_X) )
      {
        OutPutCameraData();
      }
#endif

      // カメラ操作用コリジョン判定タイプの変更
      if( pUiSys->checkKeyboardTrgOn(KBKEY_1) )
      {
        s32 type = m_pCameraControlCollision->GetCollisionType();
        type++;
        if( type >= SkeltonCameraControlCollision::CollisionType::MAX)
        {
          type -= SkeltonCameraControlCollision::CollisionType::MAX;
        }
        m_pCameraControlCollision->Change(
          static_cast<SkeltonCameraControlCollision::CollisionType>( type), 
          m_pSkeltonFieldMapBlockManager->GetPrimary()->GetCameraControlCollision( type)
          );
      }

      //
      // 表示情報更新
      //

      // 画角
      SkeltonCamera::ProjectionSetting projectionSetting;
      m_pCamera->GetProjectionSetting( &projectionSetting );
      SkeltonDebugInfo::m_AngleOfView = projectionSetting.aov;

      // カメラデータ名
      SkeltonDebugInfo::SetCameraCollisionMeshName( m_pCamera->GetDataName() );
      break;
    }

  case SkeltonDebugInfo::DEBUG_MODE_MAP:
    {
      // 地形の拡大率を変更
      f32 scaleChangeSpeed = 0.01f;
      f32 prevScale = m_pSkeltonFieldMapBlockManager->GetPrimary()->GetTransform()->GetScale().x;
      if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::R2 ) )
      {
        SkeltonDebugInfo::m_MapScale += scaleChangeSpeed;
        m_pSkeltonFieldMapBlockManager->GetPrimary()->GetTransform()->SetScale(
          gfl2::math::Vector4( SkeltonDebugInfo::m_MapScale, SkeltonDebugInfo::m_MapScale, SkeltonDebugInfo::m_MapScale )
          );
      }
      if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::L2 ) )
      {
        SkeltonDebugInfo::m_MapScale -= scaleChangeSpeed;
        if( SkeltonDebugInfo::m_MapScale < 0.1f )
        {
          SkeltonDebugInfo::m_MapScale = 0.1f;
        }
        m_pSkeltonFieldMapBlockManager->GetPrimary()->GetTransform()->SetScale(
          gfl2::math::Vector4( SkeltonDebugInfo::m_MapScale, SkeltonDebugInfo::m_MapScale, SkeltonDebugInfo::m_MapScale )
          );
      }
      if( pUiSys->checkKeyboardTrgOn(KBKEY_0) )
      {
        SkeltonDebugInfo::m_MapScale = 1.0f;
        m_pSkeltonFieldMapBlockManager->GetPrimary()->GetTransform()->SetScale(
          gfl2::math::Vector4( SkeltonDebugInfo::m_MapScale, SkeltonDebugInfo::m_MapScale, SkeltonDebugInfo::m_MapScale )
          );
      }
      f32 nowScale = m_pSkeltonFieldMapBlockManager->GetPrimary()->GetTransform()->GetScale().x;

      // 人を地形の拡大縮小に合わせて移動させてあげる
      if( nowScale != prevScale )
      {
        for( u32 i = 0; i < SMDL_MAX; i++ )
        {
          SkeltonModel* pSkeltonModel = m_pSkeltonModelManager->GetSkeltonModelByDataIndex( i );
          // 未使用
          if( pSkeltonModel->GetExists() == false )
          {
            continue;
          }

          SkeltonFieldTransform* pTransform = pSkeltonModel->GetTransform();
          pTransform->SetTranslation(
            pTransform->GetTranslation() / prevScale * nowScale
            );
        }

      }

      if( pUiSys->checkKeyboardTrgOn(KBKEY_9) )
      {
        // 地形リソースを読み込み直す
        m_pSkeltonFieldMapBlockManager->ReloadSampleMaps();
        CreateEnv( SkeltonDebugInfo::m_MapRootPath );
      }


      // ジャンプ
      if( pUiSys->checkKeyboardTrgOn(KBKEY_1) )
      {
        // 初期位置(看板)
        gfl2::math::Vector4 newPos( SkeltonDebugInfo::m_PlayerStartPosition );
        newPos *= SkeltonDebugInfo::m_MapScale;
        m_pSkeltonModelManager->GetSkeltonModel( SMDL_PLAYER )->GetTransform()->SetTranslation( newPos );
      }

      if( pUiSys->checkKeyboardTrgOn(KBKEY_2) )
      {
        // NPC前(中央エリア)
        gfl2::math::Vector4 newPos( 3060.0f, 0.0f, 14700.0f );
        newPos *= SkeltonDebugInfo::m_MapScale;
        m_pSkeltonModelManager->GetSkeltonModel( SMDL_PLAYER )->GetTransform()->SetTranslation( newPos );
      }
      if( pUiSys->checkKeyboardTrgOn(KBKEY_3) )
      {
        // 港(南エリア)
        gfl2::math::Vector4 newPos( -5300.0f, 0.0f, 15000.0f );
        newPos *= SkeltonDebugInfo::m_MapScale;
        m_pSkeltonModelManager->GetSkeltonModel( SMDL_PLAYER )->GetTransform()->SetTranslation( newPos );
      }
      if( pUiSys->checkKeyboardTrgOn(KBKEY_4) )
      {
        // 砂浜(東南エリア)
        gfl2::math::Vector4 newPos( 13800.0f, 0.0f, 18100.0f );
        newPos *= SkeltonDebugInfo::m_MapScale;
        m_pSkeltonModelManager->GetSkeltonModel( SMDL_PLAYER )->GetTransform()->SetTranslation( newPos );
      }

      if( pUiSys->checkKeyboardTrgOn(KBKEY_5) )
      {
        // 北端
        gfl2::math::Vector4 newPos( -6600.0f, 0.0f, -16300.0f );
        newPos *= SkeltonDebugInfo::m_MapScale;
        m_pSkeltonModelManager->GetSkeltonModel( SMDL_PLAYER )->GetTransform()->SetTranslation( newPos );
      }

      if( pUiSys->checkKeyboardTrgOn(KBKEY_6) )
      {
        // 西端
        gfl2::math::Vector4 newPos( -26200.0f, 0.0f, -5300.0f );
        newPos *= SkeltonDebugInfo::m_MapScale;
        m_pSkeltonModelManager->GetSkeltonModel( SMDL_PLAYER )->GetTransform()->SetTranslation( newPos );
      }

      if( pUiSys->checkKeyboardTrgOn(KBKEY_7) )
      {
        // ビル前(南西エリア)
        gfl2::math::Vector4 newPos( -12700.0f, 0.0f, 10000.0f );
        newPos *= SkeltonDebugInfo::m_MapScale;
        m_pSkeltonModelManager->GetSkeltonModel( SMDL_PLAYER )->GetTransform()->SetTranslation( newPos );
      }


      // コリジョンモデル表示切替
      if( pUiSys->checkKeyboardTrgOn(KBKEY_Z) )
      {
        m_pSkeltonFieldMapBlockManager->GetPrimary()->VisibleModeCountUp();
      }

      // 表示情報更新
      SkeltonDebugInfo::m_InfoMapVisibleMode = m_pSkeltonFieldMapBlockManager->GetPrimary()->GetVisibleMode();
      break;
    }

  case SkeltonDebugInfo::DEBUG_MODE_ONLY_FPS:
    {
      break;
    }

  case SkeltonDebugInfo::DEBUG_MODE_NONE:
    {
      break;
    }
  }
#endif

}


void SkeltonProc::CreateEnv( const c16* pMapRootPath )
{
  // 一旦削除
  if( m_pDrawEnvNode )
  {
    DeleteEnv();
  }

  // 文字コード変換 @todo c8に統一する
  c8 mapRootPathWork[512];
  strcpy_s( mapRootPathWork, 512, gfl2::renderingengine::scenegraph::resource::ParserUtil::UTF16toShitJIS( pMapRootPath ).c_str() );

  // マップフォルダにあるgfenvファイルを読み込む
  c8 pathWork[512];
  sprintf_s( pathWork, 512, "%s/env.gfenv", mapRootPathWork );
  gfl2::gfx::GLMemory::SetInstanceAllocator( mAllocator);
  gfl2::renderingengine::scenegraph::resource::GfEnvData	resourceData;
  gfl2::gfx::GLMemory::ClearInstanceAllocator();
  if( resourceData.ReadFile( pathWork ) == false )
  {
    // 無いこともある、無い場合はデフォルト設定を読み込む
    if( resourceData.ReadFile( FILE_PATH_DEFAULT_ENV ) == false )
    {
      // デフォルト設定が見つからない場合は起動不可
      GFL_ASSERT_STOP_MSG(0, "起動に必要なファイルが見つかりませんでした。\nfilePath=%s", FILE_PATH_DEFAULT_ENV );
    }
  }

  // env設定をシーンに追加する
  m_pEnvNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( mAllocator, &resourceData );
  m_pDrawEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode( mAllocator, m_pEnvNode );

  // 環境ノードアニメーション
  {
    sprintf_s( pathWork, 512, "%s/env.gfmot", mapRootPathWork );
    gfl2::gfx::GLMemory::SetInstanceAllocator( mAllocator);
    gfl2::renderingengine::scenegraph::resource::GfMotData resourceData;
    gfl2::gfx::GLMemory::ClearInstanceAllocator();
    if( resourceData.ReadFile( pathWork ) )
    {
      m_pEnvAnimationNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( mAllocator, &resourceData );

      m_DrawEnvAnimationController.Initialize( mHeap, m_pDrawEnvNode);

      // ライトアニメーション
      gfl2::animation::LightAnimationSlot* pLightSlot = m_DrawEnvAnimationController.GetLightSlot();
      if( pLightSlot )
      {
        pLightSlot->SetAnimationResource(m_pEnvAnimationNode);
        pLightSlot->SetLoop(true);
      }
#if 0
      // カメラアニメーション 
      gfl2::animation::CameraAnimationSlot* pCameraSlot = m_DrawEnvAnimationController.GetCameraSlot();
      pCameraSlot->SetAnimationResource(pAnimationResourceRootNode);
      pCameraSlot->SetLoop(true);
#endif
      m_isInitDrawEnvAnimation = true;
    }
  }

  // 範囲を強制的に上書き
#if 0
  gfl2::math::AABB				envSpace( gfl2::math::Vector4(-50000.0f, -50000.0f, -50000.0f), gfl2::math::Vector4(50000.0f, 50000.0f, 50000.0f) );
  m_pDrawEnvNode->SetAABB( envSpace );
#endif

  mModelRenderPath->AddDrawEnv( m_pDrawEnvNode );
}

void SkeltonProc::DeleteEnv( void )
{
  if( m_pEnvAnimationNode )
  {
    m_DrawEnvAnimationController.Finalize();
    GFL_SAFE_DELETE( m_pEnvAnimationNode);
  }

  if( m_pDrawEnvNode )
  {
    mModelRenderPath->RemoveDrawEnv( m_pDrawEnvNode );
    GFL_SAFE_DELETE( m_pDrawEnvNode );
  }
}

void SkeltonProc::LoadSettingFirst( void )
{
#if 0
  SkeltonNameMangling::GroupNode groupNode( "field" );
  SkeltonNameMangling::GroupNode* pMapGroup = groupNode.AddGroup( "MAP" );

  if( pMapGroup )
  {
    SkeltonNameMangling::GroupNode* pResourceGroup = pMapGroup->AddGroup( "resource" );
    if( pResourceGroup )
    {
      pResourceGroup->AddSkeltonNameMangling( "folder_path", &SkeltonDebugInfo::m_MapRootPath[0], GFL_NELEMS( SkeltonDebugInfo::m_MapRootPath ) );
    }
  }

  if( groupNode.LoadAndUpdate( FILE_PATH_FIELD_SETTIG ) )
  {
    // GFL_PRINT("Load Success %s \n", FILE_PATH_FIELD_SETTIG );
    // FIELD_LOG_PRINT( 2, "Load Success \n%s", FILE_PATH_FIELD_SETTIG );
  }
  else
  {
    // GFL_PRINT("Load Error %s \n", FILE_PATH_FIELD_SETTIG );
    // FIELD_LOG_PRINT( 2, "Load Error \n%s", FILE_PATH_FIELD_SETTIG );
  }
#endif
}

b32 SkeltonProc::LoadSetting( void )
{
  gfl2::math::Vector4 prevPlayerStartPosition( SkeltonDebugInfo::m_PlayerStartPosition );

  class GroupNodeCallBack_ChangeCheck : public SkeltonNameMangling::IGroupNodeCallbackable
  {
  public:
    GroupNodeCallBack_ChangeCheck( void ) : changeFlag( false )
    {
    }

    virtual ~GroupNodeCallBack_ChangeCheck( void )
    {
    }

    virtual void Callback( SkeltonNameMangling::GroupNode* pGroupNode, b32 isChange )
    {
      // @todo ここから親ノードを辿れると楽かも

      if( isChange )
      {
        changeFlag = true;
      }
    }

  public:
    b32 changeFlag;
  };


  //
  // コールバックを登録
  //
  GroupNodeCallBack_ChangeCheck mapResourceCallBackObject;
  SkeltonNameMangling::GroupNode* pMapResourceNode = m_pDebugSkeltonNameMangling->GetGroupNode("MAP/resource");
  if( pMapResourceNode )
  {
    pMapResourceNode->SetCallBack( &mapResourceCallBackObject );
  }

#if 0
  m_camera->Reset();
  CameraSetting setting;
  SkeltonDebugInfo::m_AngleOfView = m_camera->GetSetting( &setting )->aov;
#endif 

  if( m_pDebugSkeltonNameMangling->LoadAndUpdate( FILE_PATH_FIELD_SETTIG ) )
  {
    GFL_PRINT("Load Success %s \n", FILE_PATH_FIELD_SETTIG );
    FIELD_LOG_PRINT( 2, "Load Success \n%s", FILE_PATH_FIELD_SETTIG );
  }
  else
  {
    GFL_PRINT("Load Error %s \n", FILE_PATH_FIELD_SETTIG );
    FIELD_LOG_PRINT( 2, "Load Error \n%s", FILE_PATH_FIELD_SETTIG );
  }

  // pGSys->GetGfglUse()->SetProjectionMatrix( m_camera->GetProjMatrix() );


  // pGSys->GetGfglUse()->GetRenderPipeLine()->ChangeEdgeScale( SkeltonDebugInfo::m_EdgeScale );

  //
  // コールバックを解除
  //
  if( pMapResourceNode )
  {
    pMapResourceNode->SetCallBack( NULL );
  }

  // 地形読み直し
  if( mapResourceCallBackObject.changeFlag )
  {
    // 地形リソースを読み込み直す
    m_pSkeltonFieldMapBlockManager->ReloadReq();
    CreateEnv( SkeltonDebugInfo::m_MapRootPath );
  }

  // カメラデータ読み直し
  m_pCameraDataManager->Terminate();
  m_pCameraDataManager->Initialize();
  c16 pathWork[1024];
  wcscpy_s( pathWork, 1024, SkeltonDebugInfo::m_MapRootPath );
  wcscat_s( pathWork, 1024, L"\\camera_data.txt" );
  m_pCameraDataManager->LoadData( pathWork );
  
  wcscpy_s( pathWork, 1024, SkeltonDebugInfo::m_MapRootPath );
  wcscat_s( pathWork, 1024, L"\\camera_data.xml" );
  m_pCameraDataManager->CreateXml( pathWork );

#if 1
  // マップジャンプデータリロード
  m_pMapJumpDataManager->Terminate();
  m_pMapJumpDataManager->Initialize();
  wmemset( pathWork, L'', 1024);
  wcscpy_s( pathWork, 1024, SkeltonDebugInfo::m_MapRootPath );
  wcscat_s( pathWork, 1024, L"\\jump_data.txt" );
  m_pMapJumpDataManager->LoadData( pathWork);
  wmemset( pathWork, L'', 1024);
  wcscpy_s( pathWork, 1024, SkeltonDebugInfo::m_MapRootPath );
  wcscat_s( pathWork, 1024, L"\\jump_data.xml" );
  m_pMapJumpDataManager->CreateXml( pathWork);

  // マップジャンプコリジョンデータリロード
  m_pMapJumpCollisionDataManager->Terminate();
  m_pMapJumpCollisionDataManager->Initialize();
  wmemset( pathWork, L'', 1024);
  wcscpy_s( pathWork, 1024, SkeltonDebugInfo::m_MapRootPath );
  wcscat_s( pathWork, 1024, L"\\jump_collision_data.txt" );
  m_pMapJumpCollisionDataManager->LoadData( pathWork );
#endif 

  // 自機座標再セット
  if( prevPlayerStartPosition != SkeltonDebugInfo::m_PlayerStartPosition )
  {
    m_player->GetTransform()->SetTranslation( SkeltonDebugInfo::m_PlayerStartPosition );
    m_player->GetTransform()->SetAngle( SkeltonDebugInfo::m_PlayerStartAngle );
  }

  UpdateCameraCollision( true );


  return mapResourceCallBackObject.changeFlag;
}

void SkeltonProc::UpdateCameraCollision( b32 forceChangeFlag )
{
  static const u32 MAX_HIT_COUNT = 10;

  // コリジョンライブラリ、コールバックインターフェイス
  class CameraCallbackable : public gfl2::collision::ICollisionModelCallbackable
  {
  public:

    struct HITDATA
    {
      f32   height;
      const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle;
    };

    u32 m_HitCount;
    f32 m_HitHeightTable[ MAX_HIT_COUNT ];
    HITDATA m_HitDataTable[ MAX_HIT_COUNT ];

    CameraCallbackable() : m_HitCount( 0 )
    {
      for( u32 i = 0; i < MAX_HIT_COUNT; i++ )
      {
        m_HitHeightTable[ i ] = 0.0f;
        m_HitDataTable[ i ].height = 0.0f;
        m_HitDataTable[ i ].pTriangle = NULL;
      }
    }

    virtual ~CameraCallbackable()
    {
    }

    // 当たり判定コールバック関数
    virtual bool CollisionModelHeightCallback( const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle, f32 height )
    {
      // 高さを記憶
      m_HitHeightTable[ m_HitCount ] = height;
      m_HitDataTable[ m_HitCount ].height = height;
      m_HitDataTable[ m_HitCount ].pTriangle = pTriangle;
      m_HitCount++;

      if( m_HitCount >= MAX_HIT_COUNT )
      {
        // 衝突検出終了
        return false;
      }
      else
      {
        // 衝突検出続行
        return true;
      }
    }


    // 指定の高さに一番近いカメラデータを取得する
    const CAMERA_DATA* GetNearCameraData( f32 height, const SkeltonCameraDataManager& rSkeltonCameraDataManager )
    {
      if( m_HitCount == 0 )
      {
        return NULL;
      }

#if 1
#if NOT_ENABLE_COLLISION
      return NULL;
#else
      s32 resultDataIndex = m_HitDataTable[0].pTriangle->m_GroundAttribute;
      f32 resultDiff( gfl2::math::Abs( m_HitDataTable[0].height - height ) );
      u32 hitIndex = 0;
      for( u32 i = 1; i < m_HitCount; i++ )
      {
        f32 diff( gfl2::math::Abs( m_HitDataTable[i].height - height ));
        if( diff < resultDiff )
        {
          resultDataIndex = m_HitDataTable[i].pTriangle->m_GroundAttribute;
          resultDiff = diff;
          hitIndex = i;
        }
      }

      const CAMERA_DATA* pResultCameraData = rSkeltonCameraDataManager.GetCameraDataByIndex( resultDataIndex );

      for( u32 i = 0; i < m_HitCount; i++ )
      {
        if( i == hitIndex )
        {
          continue;
        }

        if( gfl2::math::Abs( m_HitDataTable[ i ].height - m_HitDataTable[ hitIndex ].height ) > 10.0f )
        {
          continue;
        }

        const CAMERA_DATA* pCameraDataWork = rSkeltonCameraDataManager.GetCameraDataByIndex( m_HitDataTable[i].pTriangle->m_GroundAttribute );
        if( pResultCameraData )
        {
          if( pCameraDataWork )
          {
            if( pResultCameraData->priority > pCameraDataWork->priority )
            {
              pResultCameraData = pCameraDataWork;
            }
          }
        }
        else
        {
          pResultCameraData = pCameraDataWork;
        }

      }

      return pResultCameraData;
#endif
#endif
    }

    // 再取得可能な状態にする
    void Clear( void )
    {
      m_HitCount = 0;
    }

  };

  b32 hitFlag( false );
  if( m_pSkeltonFieldMapBlockManager->GetPrimary()->GetCameraCollision() )
  {
    CameraCallbackable cameraCallBack;
    m_pSkeltonFieldMapBlockManager->GetPrimary()->GetCameraCollision()->GetHeight(
      m_player->GetTransform()->GetTranslation(),
      NULL,
      &cameraCallBack
      );



    const CAMERA_DATA* pCameraData = cameraCallBack.GetNearCameraData( m_player->GetTransform()->GetTranslation().y, *m_pCameraDataManager );
    if( pCameraData )
    {
      m_pCamera->Change( pCameraData, forceChangeFlag );

      wcscpy_s( SkeltonDebugInfo::m_InfoCameraCollisionMeshName, 1024, gfl2::renderingengine::scenegraph::resource::ParserUtil::ShitJIStoUTF16( pCameraData->name ).c_str() );
      hitFlag = true;
    }
  }

  // デフォルトカメラに切り替える
  if( hitFlag == false )
  {
    wcscpy_s( SkeltonDebugInfo::m_InfoCameraCollisionMeshName, 1024, L"Default" );
    m_pCamera->Change( m_pCameraDataManager->GetDefaultCameraData(), forceChangeFlag );
  }

}

// マップジャンプ衝突判定
void SkeltonProc::UpdateMapJumpCollision()
{
  s32 hitData = this->GetHitMapJumpDataName(
    m_player->GetTransform()->GetTranslation(),
    m_pSkeltonFieldMapBlockManager->GetPrimary()->GetJumpCollision(),
    m_pMapJumpCollisionDataManager,
    m_pMapJumpDataManager
    );

  if( hitData )
  {
    const JUMP_DATA* pJumpData = m_pMapJumpDataManager->GetDataByIndex( hitData );
    if( pJumpData)
    {
      // ジャンプ先にジャンプエリアがあり、連続でジャンプしてしまうのを防ぐため、
      // データ取得ができないようロックする
      m_pMapJumpDataManager->Lock();
      Test::Skelton::SkeltonEventMapJump::CallMapJumpEvent( 
        GFL_SINGLETON_INSTANCE( GameSys::GameManager ), pJumpData, m_player, *m_player->GetTransform(), m_pSkeltonFieldMapBlockManager->GetPrimary()->GetCollision());
    }
  }
  else
  {
    // 当たっていなかったらアンロック
    m_pMapJumpDataManager->Unlock();
  }
}

// 衝突したジャンプデータ名を取得
s32 SkeltonProc::GetHitMapJumpDataName( gfl2::math::Vector4& pos, gfl2::collision::CollisionModel* pCollisionModel, CollisionDataManager* pJumpCollisionDataManager, JumpDataManager* pJumpDataManager)
{
  // @todo ひとまずモデル判定を優先
  s32 result = -1;
  result = this->GetHitMapJumpDataName_Model(
    m_player->GetTransform()->GetTranslation(),
    pCollisionModel,
    pJumpDataManager
    );

  if( result != -1 )
  {
    return result;
  }

  result = this->GetHitMapJumpDataName_Data(
    m_player->GetTransform()->GetTranslation(),
    pJumpCollisionDataManager,
    pJumpDataManager
    );

  if( result != -1 )
  {
    return result;
  }

  return NULL;
}

// 衝突したジャンプデータ名をコリジョンモデルから取得
s32 SkeltonProc::GetHitMapJumpDataName_Model( gfl2::math::Vector4& pos, gfl2::collision::CollisionModel* pCollisionModel, JumpDataManager* pJumpDataManager)
{
  static   const u32 MAX_HIT_COUNT = 10;  // 衝突情報を格納する最大数

  // コリジョンライブラリ、コールバックインターフェイス
  class MapJumpCallbackable : public gfl2::collision::ICollisionModelCallbackable
  {
  public:

    // 衝突情報
    struct HITDATA
    {
      f32   height;
      const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle;
    };

    u32     m_HitCount;                         // 当たった数
    HITDATA m_HitDataTable[ MAX_HIT_COUNT ];    // 衝突情報バッファ

    // コンストラクタ
    MapJumpCallbackable() :
      m_HitCount( 0 )
    {
      for( u32 i = 0; i < MAX_HIT_COUNT; i++)
      {
        m_HitDataTable[i].height = 0.0f;
        m_HitDataTable[i].pTriangle = NULL;
      }
    }

    // デストラクタ
    virtual ~MapJumpCallbackable()
    {
    }

    // 当たり判定コールバック関数
    virtual bool CollisionModelHeightCallback( const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle, f32 height)
    {
      // 高さを記憶
      m_HitDataTable[m_HitCount].height = height;
      m_HitDataTable[m_HitCount].pTriangle = pTriangle;
      m_HitCount++;

      if( m_HitCount >= MAX_HIT_COUNT)
      {
        // 衝突検出終了
        return false;
      }
      else
      {
        // 衝突検出続行
        return true;
      }
    }

    // 当たったかどうか
    bool IsHit()
    {
      return ( m_HitCount != 0 );
    }

    // 指定の高さに一番近いコリジョンメッシュの名前を取得する
    const s32 GetNearHitName( JumpDataManager* pJumpDataManager, f32 height)
    {
      // 衝突していなかったらNULLを返す
      if( m_HitCount == 0)
      {
        return -1;
      }

#if NOT_ENABLE_COLLISION
      return -1;
#else
      // 一番近い衝突メッシュを探す
      s32 resultIndex = m_HitDataTable[0].pTriangle->m_GroundAttribute;
      f32 resultDiff = gfl2::math::Abs( m_HitDataTable[0].height - height);
      u32 priority = UINT_MAX;
      for( u32 i = 1; i < m_HitCount; i++)
      {
        f32 diff( gfl2::math::Abs( m_HitDataTable[i].height - height ));
        if( diff < resultDiff)
        {
          const JUMP_DATA* pJumpData = pJumpDataManager->GetDataByIndex( m_HitDataTable[i].pTriangle->m_GroundAttribute, true);
          if( pJumpData)
          {
            if( pJumpData->priority < priority)
            {
              resultIndex = m_HitDataTable[i].pTriangle->m_GroundAttribute;
              resultDiff = diff;
              priority = pJumpData->priority;
            }
          }
        }
      }

      return resultIndex;
#endif
    }

    // 再取得可能な状態にする
    void Clear( void)
    {
      m_HitCount = 0;
    }
  };

  // コリジョンメッシュとの当たり判定
  if( pCollisionModel)
  {
    MapJumpCallbackable mapJumpCallBack;
    mapJumpCallBack.Clear();
    pCollisionModel->GetHeight(
      pos,
      NULL,
      &mapJumpCallBack
      );

    // 一番近い衝突メッシュ名を取得
    return mapJumpCallBack.GetNearHitName( pJumpDataManager, pos.y);
  }
  return -1;
}

// 衝突したジャンプデータ名をコリジョンデータから取得
s32 SkeltonProc::GetHitMapJumpDataName_Data( gfl2::math::Vector4& pos, CollisionDataManager* pJumpCollisionDataManager, JumpDataManager* pJumpDataManager)
{
  const COLLISION_DATA* pCollisionDataTable = pJumpCollisionDataManager->GetData();
  u32 dataNum = pJumpCollisionDataManager->GetDataNum();

  const c8* retName = NULL;
  s32 retDataIndex = -1;
  u32 priority = UINT_MAX;
  for( s32 i = 0; i < dataNum; ++i)
  {
    if( pCollisionDataTable[i].aabb.IsContain( pos))
    {
      const JUMP_DATA* pJumpData = pJumpDataManager->GetData( pCollisionDataTable[i].name, true);
      if( pJumpData)
      {
        if( pJumpData->priority < priority)
        {
          retDataIndex = pCollisionDataTable->id;
          retName = pCollisionDataTable[i].name;
          priority = pJumpData->priority;
        }
      }
    }
  }

  return retDataIndex;
}

void SkeltonProc::OutPutPlayerData( void )
{
  std::ofstream ofs( FILE_PATH_PLAYER_INFO );

  ofs << "<!--自機の現在座標-->" << std::endl;
  ofs << "<position>" << std::endl;
  ofs << "  <x>" << m_player->GetTransform()->GetTranslation().x << "</x>" << std::endl;
  ofs << "  <y>" << m_player->GetTransform()->GetTranslation().y << "</y>" << std::endl;
  ofs << "  <z>" << m_player->GetTransform()->GetTranslation().z << "</z>" << std::endl;
  ofs << "</position>" << std::endl;
  ofs << "<angle>" << m_player->GetTransform()->GetRotation().y << "</angle>" << std::endl;
  ofs << "<angle type=\"degree\">" << gfl2::math::ConvRadToDeg( m_player->GetTransform()->GetRotation().y) << "</angle>" << std::endl;

  GFL_PRINT("Output Success %s \n", FILE_PATH_PLAYER_INFO );
  FIELD_LOG_PRINT( 2, "Output Success \n%s", FILE_PATH_PLAYER_INFO );
}

void SkeltonProc::OutPutCameraData( void )
{
  std::ofstream ofs( FILE_PATH_CAMERA_INFO );

  m_pCamera->OutPutData( &ofs );
  GFL_PRINT("Output Success %s \n", FILE_PATH_CAMERA_INFO );
  FIELD_LOG_PRINT( 2, "Output Success \n%s", FILE_PATH_CAMERA_INFO );
}


GFL_NAMESPACE_END(Skelton)
  GFL_NAMESPACE_END(Test)

#endif // GF_PLATFORM_WIN32