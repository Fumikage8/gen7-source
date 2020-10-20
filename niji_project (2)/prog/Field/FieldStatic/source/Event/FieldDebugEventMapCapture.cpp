/**
* @brief  デバッグ用カメラ位置調整イベント
* @fix CONNMCat[582]:攻略本用マップ撮影機能
* @file   FieldEventMapCapture.cpp
* @author ikuta_junya
* @data   2015.05.23
*/

#if PM_DEBUG

// gfl2
#include <Debug/include/gfl2_DebugPrint.h>
#include <gfx/include/gfl2_DrawUtil.h>

// niji
#include "System/include/HeapDefine.h"
#include "GameSys/include/GameManager.h"
#include "Sound/include/Sound.h"

// field
#include "Field/FieldStatic/include/Event/FieldDebugEventMapCapture.h"
#include "Field/FieldRo/include/FieldMap.h"

// field camera
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeDebugAdjustPosition.h"
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeEventScript.h"

// field terrain
#include "Field/FieldRo/include/Terrain/FieldTerrainManager.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainBlock.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainLayoutData.h"

// field static model
#include "Field/FieldRo/include/StaticModel/StaticModelManager.h"

// field debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#include <Debug/DebugWin/include/DebugWinSystem.h>
#include <Debug/include/gfl2_DebugWinLocal.h>
// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Debug );
GFL_NAMESPACE_BEGIN( Event );


/**
* @brief フィールドカメラの操作、デバッグ用カメラ位置調整機能
*/
class ControllerTypeMapCapture: public Field::Camera::Controller::ControllerBase
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( ControllerTypeMapCapture );

public:

  /**
  * @brief コンストラクタ
  *
  * @param pTarget  注視対象
  *
  * @return 無し
  */ 
  ControllerTypeMapCapture( const Field::Camera::Target::TargetBase* pTarget ) :
    // 内部制御
    m_IsOrtho( true )
    ,m_TargetPositon( 0.0f, 0.0f, 0.0f )
    ,m_CameraPositon( 0.0f, 0.0f, 0.0f )
    ,m_Distance( 0.0f )   
    ,m_Rot( 0.0f, 0.0f, 0.0f )
    ,m_RotDeg( 0.0f, 0.0f, 0.0f )
    ,m_fovy( 0.0f)
    ,m_bank( 0.0f)
    ,m_StepMoveValue( 0.0f )
    ,m_OrthoDrawRange( 1000.0f )
    ,m_StartTargetPositon( 0.0f, 0.0f, 0.0f )
    ,m_StartDistance( 0.0f )   
    ,m_StartRot( 0.0f, 0.0f, 0.0f )

    // 外部オブジェクト
    ,m_pTarget( NULL )
  {
    m_pTarget = pTarget;
  }

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~ControllerTypeMapCapture( void )
  {

  }
  
  /**
  * @brief 初期化処理
  * @note  ControllerがCameraUnitの設定された直後に実行される
  *
  * @param  pCamera 操作するカメラの実体
  *
  * @return 無し
  */
  void SetUp( poke_3d::model::BaseCamera* pCamera )
  {
    gfl2::math::Vector3 rotRad;
    Field::Camera::Area::Utility::GetEventScriptCameraParameter( pCamera, &m_TargetPositon, &m_CameraPositon, &rotRad, &m_Distance );

    // @fix Z成分はバンクとして別で保持し、オイラーからは削除
    // Camera::Area::Utility::GetEventScriptCameraParameterはカメラのクォータニオンからオイラー角を求めm_Rotに代入するが、
    // このクラスではz成分はバンクとして別で保持しているため、バンクを使用していたときに、SetUpを走らせた後とUpdate中のm_Rotに差異が出ていた

    // このクラスでは度で回転を制御する
    m_RotDeg.x = gfl2::math::ConvRadToDeg( rotRad.x );
    m_RotDeg.y = gfl2::math::ConvRadToDeg( rotRad.y );
    m_RotDeg.z = gfl2::math::ConvRadToDeg( rotRad.z );
    
    m_bank = m_Rot.z;
    m_Rot.z = 0.0f;


    // 開始時点の情報を記憶
    m_StartDistance = m_Distance;
    m_StartRot = m_Rot;
    m_StartTargetPositon = m_TargetPositon;
    m_fovy = pCamera->GetFovy();

    // 距離を固定化
    m_CameraPositon = m_TargetPositon + ( m_CameraPositon - m_TargetPositon ).Normalize() * 2000.0f;
  }

  /**
  * @brief 更新処理
  *
  * @param  pCamera 操作するカメラの実体
  *
  * @return 無し
  */
  void Update( poke_3d::model::BaseCamera* pCamera )
  {
    UpdateTargetPosition();

    pCamera->SetupCameraLookAt(
      m_CameraPositon,
      m_TargetPositon,
      gfl2::math::Vector3( 0.0f, 1.0f, 0.0f ) 
      );
  }

  /**
  * @brief 更新処理
  *
  * @param  pCamera 操作するカメラの実体
  *
  * @return 無し
  */
  void UpdateControl( poke_3d::model::BaseCamera* pCamera )
  {
    GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
    gfl2::ui::VectorDevice* pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
    gfl2::ui::Button* pButton = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

    f32 stickValueX = pVecDevice->GetX();
    f32 stickValueY = pVecDevice->GetY();


    if( GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->IsOpen() == false )
    {
      // ステップ移動
      {

        gfl2::math::Vector3 oldPos = m_CameraPositon;

        gfl2::math::Vector3 localMove( 0.0f, 0.0f, 0.0f );
        b32 isDepthMove = false;

        if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
        {
          isDepthMove = true;
        }

        if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
        {
          localMove.x -= m_StepMoveValue;
        }
        if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
        {
          localMove.x += m_StepMoveValue;
        }

#if 0
        if( pButton->IsRepeat( gfl2::ui::BUTTON_UP ) )
        {
          localMove.z -= m_StepMoveValue;
        }
        if( pButton->IsRepeat( gfl2::ui::BUTTON_DOWN ) )
        {
          localMove.z += m_StepMoveValue;
        }
#endif
        if( isDepthMove )
        {
          // 奥行き移動
          if( pButton->IsRepeat( gfl2::ui::BUTTON_UP ) )
          {
            localMove.z -= m_StepMoveValue;
          }
          if( pButton->IsRepeat( gfl2::ui::BUTTON_DOWN ) )
          {
            localMove.z += m_StepMoveValue;
          }
        }
        else
        {
          if( pButton->IsRepeat( gfl2::ui::BUTTON_UP ) )
          {
            localMove.y += m_StepMoveValue;
          }
          if( pButton->IsRepeat( gfl2::ui::BUTTON_DOWN ) )
          {
            localMove.y -= m_StepMoveValue;
          }
        }



        // ローカル移動量をワールド移動量に変換し、現在座標に足しこむ
        if( localMove.Length() > 0.0f )
        {
          gfl2::math::Matrix34 rotMat( gfl2::math::Matrix34::GetRotationZYX( m_Rot.z, m_Rot.y, m_Rot.x ) );
          // gfl2::math::Matrix34 rotMat( gfl2::math::Matrix34::GetRotationZYX( 0.0f, m_Rot.y, 0.0f ) );
          gfl2::math::Vector3 move = rotMat.TransformCoord( localMove );
          m_CameraPositon += move;

          // 通常移動時は高さを維持する
          if( !isDepthMove )
          {
            gfl2::math::Vector3 cameraVec( 0.0f, 0.0f, 1.0f );
            cameraVec = rotMat.TransformCoord( cameraVec );
            m_CameraPositon = m_CameraPositon + cameraVec * ( ( oldPos.y - m_CameraPositon.y ) / cameraVec.y );
          }
        }
      }
    }

    // ターゲット座標更新
    UpdateTargetPosition();
  }


  /**
  * @brief 現在の設定値から生成されるプロジェクション行列を取得する
  * @note  この関数がfalseを返す場合は、BaseCameraから行列を取得する
  * @note  この関数によりvirtualが毎回呼ばれる+インターフェイスじゃなくなるので削除する可能性がある
  *
  * @param  pOut 行列格納先( 戻り値 true 時のみ値が入る)
  *
  * @return 成否
  */
  virtual b32 GetProjectionMatrixOverride( gfl2::math::Matrix44* pOut ) const
  {
    if( !m_IsOrtho )
    {
      return false;
    }

    f32 aspect = 400.0f / 240.0f;

    f32 drawRangeH = m_OrthoDrawRange * aspect * 0.5f;
    f32 drawRangeV = m_OrthoDrawRange * 0.5f;
   

    f32 orthoL =-drawRangeH;     // ニアクリッピング面の視錐台左辺のX座標
    f32 orthoR = drawRangeH;     // ニアクリッピング面の視錐台右辺のX座標
    f32 orthoB =-drawRangeV;     // ニアクリッピング面の視錐台下辺のY座標
    f32 orthoT = drawRangeV;     // ニアクリッピング面の視錐台上辺のY座標
    gfl2::math::Matrix44 projectionForOrtho;
    nw::math::MTX44 proj = gfl2::math::ConvertGfMtx44ToNwMtx44( projectionForOrtho );
    gfl2::math::MTX44OrthoPivot( 
      &proj,
      orthoL,
      orthoR,
      orthoB,
      orthoT,
      30.0f,
      30000.0f,
      gfl2::math::PIVOT_UPSIDE_TO_TOP );

    *pOut = gfl2::math::ConvertNwMtx44ToGfMtx44( proj );
    return true;
  }

  /**
  * @brief 正射影か？
  *
  * @return true 正射影 false 射影
  */
  virtual b32 IsOrtho( void ) const { return m_IsOrtho; }

  /**
  * @brief ステップ移動モード、移動量設定
  *
  * @param  moveValue 1回の移動量
  */
  void SetStepMoveValue( f32 moveValue )
  {
    m_StepMoveValue = moveValue;
  }

  /**
  * @brief  正投影幅(縦)設定
  *
  * @param  range 幅
  */
  void SetOrthoDrawRange( f32 range )
  {
    m_OrthoDrawRange = range;
  }

  /**
  * @brief  カメラ座標を設定
  *
  * @param  rPos 設定座標
  */
  void SetCameraPosition( const gfl2::math::Vector3& rPos )
  {
    m_CameraPositon = rPos;
  }

  /**
  * @brief  カメラ座標を取得
  *
  * @param  pPos 格納先
  */
  void GetCameraPosition( gfl2::math::Vector3* pPos )
  {
    *pPos = m_CameraPositon;
  }

  /**
  * @brief  カメラターゲット座標を取得
  *
  * @param  pPos 格納先
  */
  void GetCameraTargetPosition( gfl2::math::Vector3* pPos )
  {
    *pPos = m_TargetPositon;
  }

  /**
  * @brief  カメラ回転を設定
  *
  * @param  rRot 回転(度)
  */
  void SetCameraRotation( const gfl2::math::Vector3& rRot )
  {
    m_RotDeg = rRot;
  }

  /**
  * @brief  カメラ回転を取得
  *
  * @param  pRot 格納先(度)
  */
  void GetCameraRotation( gfl2::math::Vector3* pRot )
  {
    *pRot = m_RotDeg;
  }

  /**
  * @brief  カメラ回転を取得
  *
  * @param  pRot 格納先(ラジアン)
  */
  void GetCameraRotationRad( gfl2::math::Vector3* pRot )
  {
    *pRot = m_Rot;
  }

  /**
  * @brief  正射影設定
  *
  * @param  flag true 正射影  false 投影
  */
  void SetOrthoFlag( b32 flag )
  {
    m_IsOrtho = flag;
  }

private:

  /**
  * @brief  ターゲット座標更新
  */
  void UpdateTargetPosition( void )
  {
    //
    // ターゲット座標更新
    //

    // 度→ラジアン
    m_Rot.x = gfl2::math::ConvDegToRad( m_RotDeg.x );
    m_Rot.y = gfl2::math::ConvDegToRad( m_RotDeg.y );
    m_Rot.z = gfl2::math::ConvDegToRad( m_RotDeg.z );

    // 回転量を正規化
    m_Rot.x = gfl2::math::NormalizeRadianAngle( m_Rot.x );
    m_Rot.y = gfl2::math::NormalizeRadianAngle( m_Rot.y );
    m_Rot.z = gfl2::math::NormalizeRadianAngle( m_Rot.z );

    gfl2::math::Vector3 targetPosition;
    gfl2::math::Vector3 cameraPosition;
    cameraPosition = m_CameraPositon;
    targetPosition = m_TargetPositon;

    gfl2::math::Vector3 cameraOffset( 0.0f, 0.0f, -m_Distance );
    gfl2::math::Matrix34 rotMat( gfl2::math::Matrix34::GetRotationZYX( m_Rot.z, m_Rot.y, m_Rot.x ) );
    cameraOffset = rotMat.TransformCoord( cameraOffset );

    targetPosition.x = cameraPosition.x + cameraOffset.x;
    targetPosition.y = cameraPosition.y + cameraOffset.y;
    targetPosition.z = cameraPosition.z + cameraOffset.z;


    m_TargetPositon = targetPosition;
  }


  //
  // 内部制御
  //
  b32                       m_IsOrtho;        // trueなら正射影
  gfl2::math::Vector3       m_TargetPositon;  // ターゲット座標
  gfl2::math::Vector3       m_CameraPositon;  // カメラ座標
  f32                       m_Distance;   
  gfl2::math::Vector3       m_Rot;
  gfl2::math::Vector3       m_RotDeg;
  f32                       m_fovy;
  f32                       m_bank;

  f32                       m_StepMoveValue;
  f32                       m_OrthoDrawRange; // 正投影幅(縦)


  // 開始時点情報
  gfl2::math::Vector3       m_StartTargetPositon;
  f32                       m_StartDistance;   
  gfl2::math::Vector3       m_StartRot;


  //
  // 外部オブジェクト
  //
  const Field::Camera::Target::TargetBase* m_pTarget;  // 注視対象

};  // class FieldControllerTypeAdjustPosition




// 各種設定。デバッグでメニューで操作した結果を覚えておくため、static
EventMapCapture::SETTING EventMapCapture::s_Setting = 
{
  false,    // b32 is_event_call;       // trueならイベントコール済み
  false,    // b32 is_end_req;          // trueならイベント終了
  true,     // b32 is_ortho;            // trueなら正射影描画
  200.0f,   // f32 step_move_value;          // 固定移動量
  200.0f,   // f32 step_move_value_for_control;        // 固定移動量
  1000.0f,  // f32 ortho_draw_range;    // 正投影幅(縦)
  0.0f,     // f32 animation_stop_frame;// アニメーション停止させた際のフレーム
  gfl2::math::Vector3( 0.0f, 0.0f, 0.0f ), // gfl2::math::Vector3 camera_position;    // カメラ座標
  gfl2::math::Vector3( 0.0f, 0.0f, 0.0f ), // gfl2::math::Vector3 camera_rot_deg;     // カメラ回転
  false,                                   // b32 is_draw_control_positon;            // 地形制御座標
  gfl2::math::Vector3( 0.0f, 0.0f, 0.0f ), // gfl2::math::Vector3 control_position;   // 地形制御座標
};


/**
* @brief コンストラクタ
*
* @param  pSystemHeap GameEventに渡すヒープ
*
* @return 無し
*/ 
EventMapCapture::EventMapCapture( gfl2::heap::HeapBase* pSystemHeap ) :

  GameSys::GameEvent( pSystemHeap )

  // 内部制御
  ,m_PrevUnitNo( 0 )
  ,m_pCameraController( NULL )

#if 0
  ,m_IsDebugDraw( true )

  // 内部システム
  // 外部オブジェクト
  ,m_pPlayerCharacter( NULL )
  ,m_pCameraController( NULL )
#endif
  // 外部システム
  ,m_pFieldMap( NULL )
  ,m_pSystemHeap( NULL )
#if 0
  ,m_pMsgData( NULL )
#endif
{

}

/**
* @brief デストラクタ
*
* @return 無し
*/ 
EventMapCapture::~EventMapCapture()
{

}

/**
* @brief 初期化処理(インスタンス生成直後に実行する)
*
* @param  rDesc   初期化設定
*
* @return 無し
*/ 
void EventMapCapture::Initialize( const InitializeDescription& rDesc )
{
  m_pSystemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
#if 0
  // m_pSystemHeap = rDesc.m_pHeap;
  ::Debug::DebugWin::DebugWinWork* devWork = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  if( devWork ) m_pMsgData = devWork->GetMessageData();
#endif
}

//--------------------------------------------------------------
/**
* @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
* @retval  true  起動してよい
* @retval  false 起動しない
*/
//--------------------------------------------------------------
bool EventMapCapture::BootChk(GameSys::GameManager* gmgr)
{
  return true;
}

//--------------------------------------------------------------
/**
* @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
*/
//--------------------------------------------------------------
void EventMapCapture::InitFunc(GameSys::GameManager* gmgr)
{
  m_pFieldMap = gmgr->GetFieldmap();

#if 0
  // 自機を取得
  m_pPlayerCharacter = m_pFieldMap->GetPlayerCharacter();
#endif
}

//--------------------------------------------------------------
/**
* @brief   呼び出される関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
* @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
*/
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventMapCapture::MainFunc(GameSys::GameManager* gmgr)
{
  // シーケンス制御値
  struct Seq
  {
    enum Value
    {
      INIT = 0,
      MAIN,
      TERM,
      TERM_WAIT,
      END,
    };
  };

  switch( this->GetSeqNo() )
  {
  case Seq::INIT:
    {
      // フィールドカメラからスクリプトカメラへ切り替え
      {
        Camera::CameraManager* pCameraManager = m_pFieldMap->GetCameraManager();


        m_PrevUnitNo = pCameraManager->GetUnitNoForMainGamePlayCamera();

        // FieldUnitの基本パラメーターをEventUnitにコピー
        // コントローラー内部で値を使用する可能性があるので先に行う
        pCameraManager->GetCameraByUnitNo( Camera::UnitNo::DEBUG )->CopyBaseParam(
          *pCameraManager->GetMainGamePlayCamera()
          );

        // 使用カメラをField->Eventに変更( Fieldはスリープ状態になる )
        pCameraManager->ChangeMainCameraByUnitNo( Camera::UnitNo::DEBUG );
        
        // コントローラーを生成
        // EventUnitはポケファインダーなどで使いまわすので、毎回使用開始時点で作ります
        // downcastせずアクセスする為に、作成したコントローラーはScriptWorkなどで保持してください
        {
          m_pCameraController = GFL_NEW( m_pSystemHeap ) ControllerTypeMapCapture( pCameraManager->GetTarget() );
        }

        // 操作コントローラーをイベントスクリプト用コントローラに変更
        {
          // ↓ChangeMainCameraByUnitNo()で指定したカメラが返ります
          Camera::CameraUnit* pCameraUnit = pCameraManager->GetMainGamePlayCamera();

          // ↓描画に使うUnitを上書き設定している場合は、違うUnitが返ります、用途はデバッグ用で基本はGetMainGamePlayCameraと同じ
          // Camera::CameraUnit* pCameraUnit = pCameraManager->GetMainViewCamera();
          
          // この時点でフィールドシステムが更新してくれるようになります
          pCameraUnit->ChangeController( m_pCameraController );
        }
        
        m_pCameraController->GetCameraPosition( &s_Setting.camera_position );
        m_pCameraController->GetCameraRotation( &s_Setting.camera_rot_deg );

        gfl2::math::Vector3 playerPosition;
        playerPosition = m_pFieldMap->GetPlayerCharacter()->GetCharaDrawInstance()->GetPosition();
        s_Setting.control_position = playerPosition;
        s_Setting.is_event_call = true;
      }


       FIELD_DEBUG_SWITCH_SET( DebugTypes::Switch::VISIBLE_PLAYER, 0 );

      this->SetSeqNo( Seq::MAIN );
      break;
    } // case Seq::INIT:

  case Seq::MAIN:
    {
      b32 isPlayerDraw = false;

      // 設定を反映
      ApplySetting();

      // 終了
      if( s_Setting.is_end_req )
      {
        s_Setting.is_end_req = false;
        this->SetSeqNo( Seq::TERM );
        Sound::PlaySE( SEQ_SE_CANCEL1 ); // キャンセル音
        break;
      }

      // 操作
      if( GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->IsOpen() == false )
      {
        GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
        gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
        // gfl2::ui::VectorDevice* pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
        gfl2::ui::Button* pButton = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);



        if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
        {
          //
          // 読み込み基準点操作
          //

          gfl2::math::Vector3 localMove( 0.0f, 0.0f, 0.0f );

          if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
          {
            localMove.x -= s_Setting.step_move_value_for_control;
          }
          if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
          {
            localMove.x += s_Setting.step_move_value_for_control;
          }

          if( pButton->IsRepeat( gfl2::ui::BUTTON_UP ) )
          {
            localMove.z -= s_Setting.step_move_value_for_control;
          }
          if( pButton->IsRepeat( gfl2::ui::BUTTON_DOWN ) )
          {
            localMove.z += s_Setting.step_move_value_for_control;
          }

          gfl2::math::Vector3 rot;
          m_pCameraController->GetCameraRotationRad( &rot );
          gfl2::math::Matrix34 rotMat( gfl2::math::Matrix34::GetRotationZYX( 0.0f, rot.y, 0.0f ) );

          // gfl2::math::Matrix34 rotMat( gfl2::math::Matrix34::GetRotationZYX( 0.0f, m_Rot.y, 0.0f ) );
          gfl2::math::Vector3 move = rotMat.TransformCoord( localMove );
          s_Setting.control_position += move;


          isPlayerDraw = true;
        }
        else
        {
          //
          // カメラ操作
          //

          m_pCameraController->UpdateControl( NULL );
          m_pCameraController->GetCameraPosition( &s_Setting.camera_position );
          m_pCameraController->GetCameraRotation( &s_Setting.camera_rot_deg );
        }

        // 制御座標をカメラの中心に移動
        if( pButton->IsHold( gfl2::ui::BUTTON_A ) )
        {
          SetCotrolPointByRaycast();
          isPlayerDraw = true;
        }


        // 制御座標をカメラの真下に移動
        if( pButton->IsHold( gfl2::ui::BUTTON_X ) )
        {
          s_Setting.control_position = s_Setting.camera_position;
          isPlayerDraw = true;
        }


        FIELD_DEBUG_SWITCH_SET( DebugTypes::Switch::VISIBLE_PLAYER, isPlayerDraw );

#if 0
        // 操作開始時の状態にリセット
        if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) )
        {
          m_pCameraController->Reset();
          Sound::PlaySE( SEQ_SE_DECIDE1 ); // 決定音
        }

        // Dump
        if( pButton->IsTrigger( gfl2::ui::BUTTON_START ) )
        {
          m_pCameraController->DumpForScript();
          Sound::PlaySE( SEQ_SE_DECIDE1 ); // 決定音
        }

        // デバッグ表示切替
        if( pButton->IsHold( gfl2::ui::BUTTON_L ) && pButton->IsTrigger( gfl2::ui::BUTTON_START ) )
        {
          m_IsDebugDraw = !m_IsDebugDraw;
        }
#endif

      }

      break;
    } // case Seq::MAIN:

   case Seq::TERM:
    {
      // カメラを元に戻す
      {
        Camera::CameraManager* pCameraManager = m_pFieldMap->GetCameraManager();

        // コントローラーを解除
        Camera::CameraUnit* pCameraUnit = pCameraManager->GetMainGamePlayCamera();
        pCameraUnit->ChangeController( NULL );  // 内部でメモリ破棄してくれる　@note 微妙かも
        m_pCameraController = NULL;
        
        // 使用カメラをEvent->Fieldに変更( Eventはスリープ状態になる )
        pCameraManager->ChangeMainCameraByUnitNo( m_PrevUnitNo );
      }

      // 地形基準点を元に戻す
      {
        Field::Terrain::TerrainManager* pTerrainManager = m_pFieldMap->GetTerrainManager();
        pTerrainManager->ClearExternalBlockControlPoint();


        // ローカルアニメ再開
        pTerrainManager->DebugResumeLocalAnimation();
        m_pFieldMap->GetStaticModelManager()->DebugResumeLocalAnimation();
      }

      this->SetSeqNo( Seq::TERM_WAIT );
      break;

    } // case Seq::TERM

   case Seq::TERM_WAIT:
     {
       this->SetSeqNo( Seq::END );
       break;
     } // case Seq::TERM_WAIT

   case Seq::END:
     {
       s_Setting.is_event_call = false;
       FIELD_DEBUG_SWITCH_SET( DebugTypes::Switch::VISIBLE_PLAYER, 1 );
       return GameSys::GMEVENT_RES_FINISH;
     } // case Seq::END:

  } // switch( this->GetSeqNo() )

  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
* @brief   呼び出される関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
*/
//--------------------------------------------------------------
void EventMapCapture::EndFunc(GameSys::GameManager* gmgr)
{

}

/**
* @brief   設定を各モジュールに反映
*
* @return  無し
*/
void EventMapCapture::ApplySetting( void )
{
  m_pCameraController->SetStepMoveValue( s_Setting.step_move_value );
  m_pCameraController->SetOrthoDrawRange( s_Setting.ortho_draw_range );
  m_pCameraController->SetCameraPosition( s_Setting.camera_position );
  m_pCameraController->SetCameraRotation( s_Setting.camera_rot_deg );
  m_pCameraController->SetOrthoFlag( s_Setting.is_ortho );
  
  // 地形アニメーションのフレームを指定
  Field::Terrain::TerrainManager* pTerrainManager = m_pFieldMap->GetTerrainManager();
  pTerrainManager->DebugSuspendLocalAnimation( s_Setting.animation_stop_frame );

  // 配置モデルアニメーションのフレームを指定
  m_pFieldMap->GetStaticModelManager()->DebugSuspendLocalAnimation( s_Setting.animation_stop_frame );

  // 自機座標更新
  {
    gfl2::math::Vector3 controlWork( s_Setting.control_position );

    gfl2::math::Vector3 playerPosition;
    playerPosition = m_pFieldMap->GetPlayerCharacter()->GetCharaDrawInstance()->GetPosition();
    controlWork.y = playerPosition.y;

    m_pFieldMap->GetPlayerCharacter()->GetCharaDrawInstance()->SetPosition( controlWork );
    Field::Terrain::TerrainManager* pTerrainManager = m_pFieldMap->GetTerrainManager();
    pTerrainManager->SetExternalBlockControlPointByPosition( controlWork );
  }
}

/**
* @brief   レイキャストの結果の位置に制御座標を移動する
*
* @return  無し
*/
void EventMapCapture::SetCotrolPointByRaycast( void )
{
  if( m_pFieldMap->GetPlayerCharacter()->GetCollisionSceneContainerForGround() )
  {
    f32 startEndOffset = 1000000.0f;

    gfl2::math::Vector3 cameraPos;
    gfl2::math::Vector3 cameraTargetPos;

    m_pCameraController->GetCameraPosition( &cameraPos );
    m_pCameraController->GetCameraTargetPosition( &cameraTargetPos );

    cameraTargetPos = cameraPos + ( cameraTargetPos - cameraPos ).Normalize() * startEndOffset;

    // 地面の高さを取得
    RaycastCustomCallback::HIT_DATA hitData;
    gfl2::math::Vector4 start( cameraPos.x, cameraPos.y, cameraPos.z, 0.0f);
    gfl2::math::Vector4   end( cameraTargetPos.x, cameraTargetPos.y, cameraTargetPos.z, 0.0f);
    if( m_pFieldMap->GetPlayerCharacter()->GetCollisionSceneContainerForGround()->RaycastFromStaticActorsMesh(start,end,&hitData) )
    {
      s_Setting.control_position = gfl2::math::Vector3( hitData.intersection.x, hitData.intersection.y, hitData.intersection.z );
    }
  }
}

GFL_NAMESPACE_END( Event );
GFL_NAMESPACE_END( Debug );
GFL_NAMESPACE_END( Field );

#endif //PM_DEBUG
