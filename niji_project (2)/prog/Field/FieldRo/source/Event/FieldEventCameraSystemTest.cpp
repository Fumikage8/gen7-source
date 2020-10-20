#if PM_DEBUG
#if ENABLE_DEBUG_CAMERA_SYSTEM_TEST

/**
* @brief  カメラシステム実装確認用イベント
* @file   FieldEventCameraSystemTest.cpp
* @author ikuta_junya
* @data   2015.05.23
*/

// gfl2
#include <Debug/include/gfl2_DebugPrint.h>

// niji
#include <System/include/HeapDefine.h>
#include <GameSys/include/GameManager.h>

// field
#include "Field/FieldRo/include/Event/FieldEventCameraSystemTest.h"
#include "Field/FieldRo/include/FieldMap.h"

// field camera
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeEventScript.h"

#include "Field/FieldRo/include/Camera/FieldCameraSystemAnimationResource.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Event );


/**
* @brief コンストラクタ
*
* @param  pSystemHeap GameEventに渡すヒープ
*
* @return 無し
*/ 
EventCameraSystemTest::EventCameraSystemTest( gfl2::heap::HeapBase* pSystemHeap ) :

  GameSys::GameEvent( pSystemHeap )

  // 内部システム
  // 外部オブジェクト
  ,m_pPlayerCharacter( NULL )
  ,m_pCameraController( NULL )
  // 外部システム
  ,m_pFieldMap( NULL )
  ,m_pSystemHeap( NULL )

{

}

/**
* @brief デストラクタ
*
* @return 無し
*/ 
EventCameraSystemTest::~EventCameraSystemTest()
{

}

/**
* @brief 初期化処理(インスタンス生成直後に実行する)
*
* @param  rDesc   初期化設定
*
* @return 無し
*/ 
void EventCameraSystemTest::Initialize( const InitializeDescription& rDesc )
{
  m_pSystemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  // m_pSystemHeap = rDesc.m_pHeap;
}

//--------------------------------------------------------------
/**
* @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
* @retval  true  起動してよい
* @retval  false 起動しない
*/
//--------------------------------------------------------------
bool EventCameraSystemTest::BootChk(GameSys::GameManager* gmgr)
{
  return true;
}

//--------------------------------------------------------------
/**
* @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
*/
//--------------------------------------------------------------
void EventCameraSystemTest::InitFunc(GameSys::GameManager* gmgr)
{
  m_pFieldMap = gmgr->GetFieldmap();

  // 自機を取得
  m_pPlayerCharacter = m_pFieldMap->GetPlayerCharacter();
}

//--------------------------------------------------------------
/**
* @brief   呼び出される関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
* @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
*/
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventCameraSystemTest::MainFunc(GameSys::GameManager* gmgr)
{
  // シーケンス制御値
  struct Seq
  {
    enum Value
    {
      INIT = 0,
      MAIN,
      CAMERA_MOVE_WAIT,    // カメラ移動中
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
        
        // FieldUnitの基本パラメーターをEventUnitにコピー
        // イベントスクリプト用コントローラー内部で値を使用する可能性があるので先に行う
        pCameraManager->GetCameraByUnitNo( Camera::UnitNo::EVENT )->CopyBaseParam(
          *pCameraManager->GetCameraByUnitNo( Camera::UnitNo::FIELD )
          );
        
        // 使用カメラをField->Eventに変更( Fieldはスリープ状態になる )
        pCameraManager->ChangeMainCameraByUnitNo( Camera::UnitNo::EVENT );
        
        // イベントスクリプト用コントローラーを生成
        // EventUnitはポケファインダーなどで使いまわすので、毎回使用開始時点で作ります
        // downcastせずアクセスする為に、作成したコントローラーはScriptWorkなどで保持してください
        {
          Camera::Controller::ControllerTypeEventScript::InitializeDescription initDesc;
          initDesc.dummy = 0; // 引数はまだ仮です
          m_pCameraController = GFL_NEW( m_pSystemHeap )Camera::Controller::ControllerTypeEventScript( initDesc );
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
      
      }

      this->SetSeqNo( Seq::MAIN );
      break;
    } // case Seq::INIT:

  case Seq::MAIN:
    {
      // 移動命令を出す
      // 操作
      {
        GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
        gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
        gfl2::ui::VectorDevice* pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
        gfl2::ui::Button* pButton = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

        gfl2::math::Vector3 pos( 0.0f, 0.0f, 0.0f );
        gfl2::math::Vector3 pos_r( 0.0f, 0.0f, 0.0f );
//        bool pad_u = false;
//        bool pad_d = false;
//        bool pad_l = false;
//        bool pad_r = false;

        pos_r.x = pVecDevice->GetX();
        pos_r.y = pVecDevice->GetY();
//        pad_u = pButton->IsHold( gfl2::ui::BUTTON_UP );
//        pad_d = pButton->IsHold( gfl2::ui::BUTTON_DOWN );
//        pad_l = pButton->IsHold( gfl2::ui::BUTTON_LEFT );
//        pad_r = pButton->IsHold( gfl2::ui::BUTTON_RIGHT );

#if 1
        if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
        {
          Camera::CameraManager* pCameraManager = m_pFieldMap->GetCameraManager();

#if 0
          m_pCameraController->GetShakeController()->Begin( 
            pCameraManager->GetSystemAnimationResource()->GetResource( 0 ),
            true,
            1.0f,
            1.0f
            );
#endif
          gfl2::math::Vector3 pos = m_pPlayerCharacter->GetCharaDrawInstance()->GetPosition();
          pos.y += 133.0f;
          gfl2::math::Vector3 offest( 350.0f, 350.0f, 0.0f );

          m_pCameraController->Move(
            pos + offest,
            pos,
            50,
            gfl2::math::Easing::LINEAR,
            40
            );

          this->SetSeqNo( Seq::CAMERA_MOVE_WAIT );

          break;
        }

#elif 1
        // 自機の位置にカメラ移動、スクリプトコマンドでは以下の命令をラップしてください
        if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
        {
          // @note イージング関数の定義はスクリプト向けに別途定義し、値を結びつけてください
          // 各関数の動きは下記サイトが参考になります
          // http://easings.net/ja

          gfl2::math::Vector3 pos = m_pPlayerCharacter->GetCharaDrawInstance()->GetPosition();
          pos.y += 133.0f;
          gfl2::math::Vector3 offest( 0.0f, 350.0f, 350.0f );

          m_pCameraController->Move(
            pos + offest,
            pos,
            50,
            gfl2::math::Easing::LINEAR,
            20
            );
          
          this->SetSeqNo( Seq::CAMERA_MOVE_WAIT );
          break;
        }
#else
        if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
        {
          // @note イージング関数の定義はスクリプト向けに別途定義し、値を結びつけてください
          // 各関数の動きは下記サイトが参考になります
          // http://easings.net/ja

          gfl2::math::Vector3 pos = m_pPlayerCharacter->GetCharaDrawInstance()->GetPosition();
          pos.y += 133.0f;
          gfl2::math::Vector3 offest( 0.0f, 350.0f, 350.0f );

          m_pCameraController->Move(
            pos,
            gfl2::math::Vector3( -30.0f, 20.0f, 90.0f ),
            500.0f,
            50,
            gfl2::math::Easing::LINEAR
            );
          
          this->SetSeqNo( Seq::CAMERA_MOVE_WAIT );
          break;
        }
#endif

#if 0
        if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) )
        {
          Camera::CameraManager* pCameraManager = m_pFieldMap->GetCameraManager();
          m_pCameraController->GetShakeController()->End( 120 ); 
          break;
        }
#elif 1
        if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) )
        {
          // @note イージング関数の定義はスクリプト向けに別途定義し、値を結びつけてください
          // 各関数の動きは下記サイトが参考になります
          // http://easings.net/ja

          gfl2::math::Vector3 pos = m_pPlayerCharacter->GetCharaDrawInstance()->GetPosition();
          pos.y += 133.0f;
          gfl2::math::Vector3 offest( 350.0f, 350.0f, 0.0f );

          m_pCameraController->Move(
            pos + offest,
            pos,
            50,
            gfl2::math::Easing::LINEAR,
            0
            );
          
          this->SetSeqNo( Seq::CAMERA_MOVE_WAIT );
          break;
        }
#else
        if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) )
        {
          // @note イージング関数の定義はスクリプト向けに別途定義し、値を結びつけてください
          // 各関数の動きは下記サイトが参考になります
          // http://easings.net/ja

          gfl2::math::Vector3 pos = m_pPlayerCharacter->GetCharaDrawInstance()->GetPosition();
          pos.y += 133.0f;
          gfl2::math::Vector3 offest( 0.0f, 350.0f, 350.0f );

          m_pCameraController->Move(
            pos,
            gfl2::math::Vector3( -30.0f, 20.0f, 45.0f ),
            500.0f,
            50,
            gfl2::math::Easing::LINEAR
            );
          
          this->SetSeqNo( Seq::CAMERA_MOVE_WAIT );
          break;
        }
#endif
        
        
        // カメラ移動、通常カメラの位置に戻す
        if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) )
        {
          // Fieldカメラの状態を取得
          // @note FieldUnitは必ず存在します
          Camera::CameraManager* pCameraManager = m_pFieldMap->GetCameraManager();
          const Camera::CameraUnit* pCameraUnitField = pCameraManager->GetCameraByUnitNo( Camera::UnitNo::FIELD );
          
          // @note BaseCameraはCameraUnit初期化後であれば必ず存在します
          m_pCameraController->Move(
            pCameraUnitField->GetBaseCamera()->GetPosition(),
            pCameraUnitField->GetBaseCamera()->GetTargetPosition(),
            50,
            gfl2::math::Easing::LINEAR
            );

          this->SetSeqNo( Seq::CAMERA_MOVE_WAIT );
          break;
        }

        // 終了
        if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
        {
          this->SetSeqNo( Seq::TERM );
          break;
        }

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
        pCameraManager->ChangeMainCameraByUnitNo( Camera::UnitNo::FIELD );
      }

      this->SetSeqNo( Seq::TERM_WAIT );
      break;

    } // case Seq::TERM

   case Seq::TERM_WAIT:
     {
       this->SetSeqNo( Seq::END );
       break;
     } // case Seq::TERM_WAIT


   case Seq::CAMERA_MOVE_WAIT:
     {
       // カメラ動作完了を待つ( アニメーション再生中もtrueを返す予定 )
       if( m_pCameraController->IsMoving() == false )
       {
         this->SetSeqNo( Seq::MAIN );
         break;
       }

       break;
     } // case Seq::CAMERA_MOVE_WAIT:

   case Seq::END:
     {
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
void EventCameraSystemTest::EndFunc(GameSys::GameManager* gmgr)
{

}

GFL_NAMESPACE_END( Event );
GFL_NAMESPACE_END( Field );

#endif
#endif