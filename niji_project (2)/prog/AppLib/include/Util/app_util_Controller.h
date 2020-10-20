//=============================================================================
/**
 * @brief UIを使用するクラス
 * @file util_Controller.h
 * @author obata_toshihiro
 * @date 2011.11.10
 */
//=============================================================================
#ifndef __APP_UTIL_CONTROLLER_H__
#define __APP_UTIL_CONTROLLER_H__

#include <macro/include/gfl2_macros.h>
#include <ui/include/gfl2_UI.h>

namespace app {
  namespace util {

    class Controller
    {
			GFL_FORBID_COPY_AND_ASSIGN(Controller);
      public: 

        //-------------------------------------------------------------------------
        // コンストラクタ・デストラクタ
        //-------------------------------------------------------------------------
        /**
         * @brief コンストラクタ
         */
        Controller( gfl2::ui::DeviceManager::BUTTON_ID button_id = gfl2::ui::DeviceManager::BUTTON_STANDARD );
        /**
         * @brief デストラクタ
         */
        virtual ~Controller();


        //-------------------------------------------------------------------------
        // UIオブジェクトの取得
        //-------------------------------------------------------------------------
        /**
         * @brief デバイスマネージャを取得する
         */
        const gfl2::ui::DeviceManager* GetDeviceManager( void ) const;

        /**
         * @brief ボタンオブジェクトを取得する
         */
        const gfl2::ui::Button* GetButton( void ) const;

        /**
         * @brief アナログスティックを取得する
         */
        const gfl2::ui::VectorDevice* GetStick( void ) const;

        /**
         * @brief タッチパネルオブジェクトを取得する
         */
        const gfl2::ui::TouchPanel* GetTouchPanel( void ) const;
      private:
        gfl2::ui::DeviceManager::BUTTON_ID  m_button_id;
    };


  }  // namespace util
} //namespace app


#endif  // __APP_UTIL_CONTROLLER_H__
