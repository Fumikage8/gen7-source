/**
* @brief  デバッグ用カメラ位置調整イベント
* @file   FieldEventDebugCameraAdjustPosition.cpp
* @author ikuta_junya
* @data   2015.05.23
*/

#if PM_DEBUG

// gfl2
#include <Debug/include/gfl2_DebugPrint.h>

// niji
#include "System/include/HeapDefine.h"
#include "GameSys/include/GameManager.h"
#include "Sound/include/Sound.h"

// field
#include "Field/FieldRo/include/Event/FieldEventDebugCameraAdjustPosition.h"
#include "Field/FieldRo/include/FieldMap.h"

// field camera
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeDebugAdjustPosition.h"
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeEventScript.h"

// field debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#include <Debug/DebugWin/include/DebugWinSystem.h>
#include <Debug/include/gfl2_DebugWinLocal.h>
// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Event );


/**
* @brief コンストラクタ
*
* @param  pSystemHeap GameEventに渡すヒープ
*
* @return 無し
*/ 
EventDebugCameraAdjustPosition::EventDebugCameraAdjustPosition( gfl2::heap::HeapBase* pSystemHeap ) :

  GameSys::GameEvent( pSystemHeap )

  // 内部制御
  ,m_PrevUnitNo( 0 )
  ,m_IsDebugDraw( true )

  // 内部システム
  // 外部オブジェクト
  ,m_pPlayerCharacter( NULL )
  ,m_pCameraController( NULL )
  // 外部システム
  ,m_pFieldMap( NULL )
  ,m_pSystemHeap( NULL )
  ,m_pMsgData( NULL )

{

}

/**
* @brief デストラクタ
*
* @return 無し
*/ 
EventDebugCameraAdjustPosition::~EventDebugCameraAdjustPosition()
{

}

/**
* @brief 初期化処理(インスタンス生成直後に実行する)
*
* @param  rDesc   初期化設定
*
* @return 無し
*/ 
void EventDebugCameraAdjustPosition::Initialize( const InitializeDescription& rDesc )
{
  m_pSystemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  // m_pSystemHeap = rDesc.m_pHeap;
  ::Debug::DebugWin::DebugWinWork* devWork = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  if( devWork ) m_pMsgData = devWork->GetMessageData();
}

//--------------------------------------------------------------
/**
* @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
* @retval  true  起動してよい
* @retval  false 起動しない
*/
//--------------------------------------------------------------
bool EventDebugCameraAdjustPosition::BootChk(GameSys::GameManager* gmgr)
{
  return true;
}

//--------------------------------------------------------------
/**
* @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
*/
//--------------------------------------------------------------
void EventDebugCameraAdjustPosition::InitFunc(GameSys::GameManager* gmgr)
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
GameSys::GMEVENT_RESULT EventDebugCameraAdjustPosition::MainFunc(GameSys::GameManager* gmgr)
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
          m_pCameraController = GFL_NEW( m_pSystemHeap )Camera::Controller::ControllerTypeAdjustPosition( pCameraManager->GetTarget() );
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
      gfl2::debug::DebugWin* devWin = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin);

      if( m_IsDebugDraw && Debug::DebugTypes::s_pDrawUtilTextUpper )
      {
#if defined(GF_PLATFORM_WIN32)
        gfl2::str::StrBuf* strBuf = GFL_NEW( devWin->GetHeap() ) gfl2::str::StrBuf( gfl2::debug::DEBUGWIN_NAME_LEN, devWin->GetHeap() );
        Debug::DebugTypes::s_pDrawUtilTextUpper->SetTextColor( gfl2::math::Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
        Debug::DebugTypes::s_pDrawUtilTextUpper->SetTextScale( 0.65f, 0.65f );
        m_pMsgData->GetString( msg_dmenu_camera_event_dbg_on_ctrl, *strBuf ); // "デバッグカメラ操作中"
        Debug::DebugTypes::s_pDrawUtilTextUpper->DrawTextW( 200, 5, strBuf->GetPtr() );
        m_pMsgData->GetString( msg_dmenu_camera_event_dbg_finish, *strBuf ); // "Bボタンで終了"
        Debug::DebugTypes::s_pDrawUtilTextUpper->DrawTextW( 200, 20, strBuf->GetPtr() );
        m_pMsgData->GetString( msg_dmenu_camera_event_dbg_console_off, *strBuf ); // "L + STARTボタンで非表示"
        Debug::DebugTypes::s_pDrawUtilTextUpper->DrawTextW( 200, 35, strBuf->GetPtr() );
        GFL_SAFE_DELETE( strBuf );
#endif
      }

      // 操作
      {
        GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
        gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
        // gfl2::ui::VectorDevice* pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
        gfl2::ui::Button* pButton = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

        // 終了
        if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
        {
          this->SetSeqNo( Seq::TERM );
          Sound::PlaySE( SEQ_SE_CANCEL1 ); // キャンセル音
          break;
        }

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
void EventDebugCameraAdjustPosition::EndFunc(GameSys::GameManager* gmgr)
{

}

GFL_NAMESPACE_END( Event );
GFL_NAMESPACE_END( Field );

#endif //PM_DEBUG
