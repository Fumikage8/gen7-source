//=============================================================================
/**
 * @brief UIを使用するクラス
 * @file app_util_Controller.cpp
 * @author obata_toshihiro
 * @date 2011.11.10
 */
//=============================================================================
#include <AppLib/include/Util/app_util_Controller.h>
#include <base/include/gfl2_Singleton.h>
#include <prog/GameSys/include/GameManager.h>
#include <stdio.h>

namespace app {
  namespace util {


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     */
    //-------------------------------------------------------------------------
     Controller::Controller( gfl2::ui::DeviceManager::BUTTON_ID button_id )
    {
      m_button_id = button_id;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    Controller::~Controller()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デバイスマネージャを取得する
     */
    //-------------------------------------------------------------------------
    const gfl2::ui::DeviceManager* Controller::GetDeviceManager( void ) const
    {
      const GameSys::GameManager* game_manager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
      return game_manager->GetUiDeviceManager(); 
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ボタンオブジェクトを取得する
     */
    //-------------------------------------------------------------------------
    const gfl2::ui::Button* Controller::GetButton( void ) const
    {
      const gfl2::ui::DeviceManager* device_manager = this->GetDeviceManager();
      return device_manager->GetButton( m_button_id );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief アナログスティックを取得する
     */
    //-------------------------------------------------------------------------
    const gfl2::ui::VectorDevice* Controller::GetStick( void ) const
    {
      const gfl2::ui::DeviceManager* device_manager = this->GetDeviceManager();
      return device_manager->GetStick( gfl2::ui::DeviceManager::STICK_STANDARD );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief タッチパネルオブジェクトを取得する
     */
    //-------------------------------------------------------------------------
    const gfl2::ui::TouchPanel* Controller::GetTouchPanel( void ) const
    {
      const gfl2::ui::DeviceManager* device_manager = this->GetDeviceManager();
      return device_manager->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );
    }


  }  // namespace util
} //namespace app
