#ifndef GFLIB2_SYSTEM_INPUTDEVICE_WINDX11_WINDX11KEY_BOARD_H_INCLUDED
#define GFLIB2_SYSTEM_INPUTDEVICE_WINDX11_WINDX11KEY_BOARD_H_INCLUDED

#include <stdio.h>
#include <types/include/gfl2_Typedef.h>
#include <util/include/gfl2_AutoComPtr.h>

#define	DIRECTINPUT_VERSION	0x800
#include <dinput.h>
#pragma comment( lib, "dxguid" )
#pragma comment( lib, "dinput8" )
# undef DrawText
#pragma comment( lib, "winmm" )

namespace gfl2 { namespace system { namespace InputDevice { namespace WinDX11 {

  class WinDX11KeyBoard
  {
  public:


  protected:

    WinDX11KeyBoard();
    inline virtual ~WinDX11KeyBoard();

    //! @brief パッドバイスの初期化
    b32 Initialize_( HWND hWnd );

    //! @brief パッドデバイスの解放
    void Terminate_( void );

    //! @brief パッドデバイスの更新
    void Update_( void );

    //! @brief キーが押されているか？
    b32 IsHold_( KeyBoardKey key );

    //! @brief キーが押されたか？
    b32 IsTrigger_( KeyBoardKey key );

    //! @brief	入力データを一時的に初期化する
    void ClearData_( void );

    //! @brief  フレームレート設定
    void SetFrameRate_( FrameRate rate );

  private:

    static const char KEY_MAP_DEFAULT[ KeyBoardKey::NumberOf ];

    static WinDX11KeyBoard						*s_Gp;

    AutoComPtr<IDirectInput8>					m_pInput8;
    AutoComPtr<IDirectInputDevice8>		m_pKeyBoard;

    FrameRate                         m_Rate;
    u8                                m_RateBit;

    typedef struct {
      BYTE	trgOn:1;
      BYTE	trgOff:1;
      BYTE	contOn:1;
      BYTE	trgOnDouble:1;
      BYTE	trgOnDelayWait:4;
    }KEYBUTTON_STATUS;
    void StatusSet(KEYBUTTON_STATUS* keybutton, BOOL trg);
    KEYBUTTON_STATUS								 m_KeyBoardStatusTable[ KeyBoardKey::NumberOf ];
  };

}}}}


#endif