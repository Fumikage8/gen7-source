#ifndef GFLIB2_SYSTEM_INPUTDEVICE_CTR_CTRPAD_H_INCLUDED
#define GFLIB2_SYSTEM_INPUTDEVICE_CTR_CTRPAD_H_INCLUDED

#include <stdio.h>
#include <types/include/gfl2_Typedef.h>
#include <util/include/gfl2_AutoComPtr.h>

#include <nn.h>
#include <nn/os.h>
#include <nn/hid.h>
#include <nn/hid/CTR/hid_ExtraPadReader.h>
#include <nn/hid/CTR/hid_ExtraPadStatus.h>
#include <nn/extrapad.h> //拡張スライドパッド補正アプレットの呼び出しに使用します

namespace gfl2 { namespace system { namespace InputDevice { namespace CTR {

class CTRPad
{
protected:

	CTRPad();
	inline virtual ~CTRPad();

	//! @brief パッドバイスの初期化
	b32 Initialize_( void );

	//! @brief パッドデバイスの解放
	void Terminate_( void );

	//! @brief パッドデバイスの更新
	void Update_( void );

	//! @brief パッドが接続されているか？
	b32 IsConnecting_( u32 pno );

	//! @brief ボタンが押されているか？
	b32 IsPress_( u32 pno, Button button );

	//! @brief ボタンが押されたか？
	b32 IsPush_( u32 pno, Button button );

	//! @brief	入力データを一時的に初期化する
	void ClearData_( u32 pno );

	//! @brief スティックの傾斜状態を取得
	const StickPos& GetStick_( u32 pno, Stick stick );

  //! @brief  フレームレート設定
  void SetFrameRate_( FrameRate rate ){}

private:

	static const s32 PADBIT[ Button::NumberOf ];

	//u8															m_ExtraPadWorkingMemory[4096] NN_ATTRIBUTE_ALIGN(4096);
	StickPos												m_PadStick;

	nn::hid::PadReader							padReader;
	nn::hid::PadStatus							padStatus;
	nn::hid::PadStatus							padStatusArray[32];
	s32															padReadLen;

};

}}}}


#endif