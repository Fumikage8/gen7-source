//=============================================================================
/**
 * @file   gfl2_UI_WIN32_TouchPanel.cpp
 * @brief  3DSのタッチパネル入力を検出するクラス
 * @author toshihiro_obata
 * @date   2010.10.21
 */
//=============================================================================
#ifdef  GF_PLATFORM_WIN32


#include "ui/include/gfl2_UI_TouchPanel.h"
#include "./gfl2_UI_WIN32_TouchPanel.h"


#include <system/include/InputDevice/gfl2_Mouse.h>
#include <debug/include/gfl2_DebugPrint.h>

namespace gfl2 {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     *
     * @param p_reader  入力の読み取りを実行するインスタンス
     * @param p_status  読み取った入力を取得するバッファ
     */
    //-------------------------------------------------------------------------
    WIN32_TouchPanel::WIN32_TouchPanel() : 
      DeviceImplementer()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    WIN32_TouchPanel::~WIN32_TouchPanel()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief メイン処理
     */
    //-------------------------------------------------------------------------
    void WIN32_TouchPanel::Main( void )
    {
      // 何もしない
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 入力データを取得する
     *
     * @param p_buffer  取得したデータの格納先 
     */
    //-------------------------------------------------------------------------
    void WIN32_TouchPanel::GetDetectiveData( void* p_buffer ) const
    {
      TouchPanel::DetectiveData* data = reinterpret_cast<TouchPanel::DetectiveData*>(p_buffer);

      data->x = gfl2::system::InputDevice::Mouse::GetLeftUpOriginPos().x;
      data->y = gfl2::system::InputDevice::Mouse::GetLeftUpOriginPos().y;
      data->touch = gfl2::system::InputDevice::Mouse::IsPress( gfl2::system::InputDevice::MouseButton::Left ) != 0;

      //座標がウィンドウ領域に合わさるように調整（ホントは外側からパラメータを渡すようにした方が良い）
      POINT point;
      GetCursorPos(&point);
      HWND hWnd = WindowFromPoint(point);

      RECT rect;
      if( GetWindowRect( hWnd , &rect ) )
      {
        const u16 WIN_X_OFFSET = 40;
        const u16 WIN_Y_OFFSET = 240;
        const u16 POINT_X_MAX = 320;
        const u16 POINT_Y_MAX = 240;

        data->x -= WIN_X_OFFSET;
        data->y -= WIN_Y_OFFSET;

        if( data->x >= 0 && data->x <= POINT_X_MAX )
        {
          if( data->y >= 0 && data->y <= POINT_Y_MAX )
          {//範囲内

            //自プログラムのウィンドウ以外だったら無視する
            DWORD dwProcID = GetCurrentProcessId();
            HWND temphWnd = GetTopWindow(GetDesktopWindow());
            while(temphWnd)
            {
              DWORD dwWndProcID = 0;
              GetWindowThreadProcessId(temphWnd, &dwWndProcID);
              if(dwWndProcID == dwProcID)
              {
                if( temphWnd == hWnd )
                {
                  return;
                }
              }
              temphWnd = GetNextWindow(temphWnd, GW_HWNDNEXT);
            }
          }
        }
      }

      //範囲外
      data->x = 0;
      data->y = 0;
      data->touch = false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 入力データのサイズを取得する
     */
    //-------------------------------------------------------------------------
    u32 WIN32_TouchPanel::GetDetectiveDataSize( void ) const
    {
      return sizeof(TouchPanel::DetectiveData);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 実効データのサイズを取得する
     */
    //-------------------------------------------------------------------------
    u32 WIN32_TouchPanel::GetEffectiveDataSize( void ) const
    {
      return sizeof( TouchPanel::EffectiveData );
    }


  } //namespace ui
} //namespace gfl2 
#endif