//======================================================================
/**
 * @file  BtlvUiMsgWin.h
 * @brief バトル描画 メッセージ
 * @author  ariizumi  -> k.ohno
 * @data  10/11/15      2015.05.02
 */
//======================================================================
#ifndef __BTLV_UI_MSGWIN_H__
#define __BTLV_UI_MSGWIN_H__
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <print/include/MessageWindow.h>


#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/app_util_2d.h>
#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "AppLib/include/Util/AppRenderingManager.h"
#include <AppLib/include/ui/UIView.h>
//#include <AppLib/include/ui/UIInputListener.h>

#include "SaveData/include/ConfigSave.h"
#include <math/include/gfl2_Vector3.h>

#include "../btl_client.h"  //コールバック系のため


GFL_NAMESPACE_BEGIN(btl)
GFL_NAMESPACE_BEGIN(app)


class BtlvUiMsgWin : public ::app::ui::UIView, public ::app::ui::UIInputListener ,public ::print::MsgCallback
{
  GFL_FORBID_COPY_AND_ASSIGN(BtlvUiMsgWin);

public:
  BtlvUiMsgWin(BattleViewSystem* core,::app::util::Heap *pheap);
  virtual ~BtlvUiMsgWin();


  bool Setup(gfl2::fs::AsyncFileManager* pAsyncFileMan);

  //初期化
  virtual void Create(void);
  //解放
  virtual void Delete(void);
  //更新
  virtual void Update(void);
  // 描画　メッセージカーソルのため
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );


  //メッセージ処理系
  // アッパーだと 後ろ3つの引数はなくせるはず。メッセージ変えればいいだけなので。
  void DispMessage( const gfl2::str::StrBuf& str , bool isOneLine, bool isKeyWait, const int autoHide, bool isFinishNone );
  void HideMessage(void);

  void SetMessageCallBack( BTL_CLIENT* callbackTarget ){mCallBackTarget = callbackTarget;}

  //メッセージ処理速度を設定
  void SetMessageSpeed( Savedata::ConfigSave::MSG_SPEED  speed );
  //今表示しているメッセージだけに有効なメッセージ速度変更
  //トレーナーの繰り出しメッセージの時に、遅いの時だけ普通にする。
  void SetMessageSpeedForce_Normal(void);

  bool IsUpdateMessage();
  virtual bool IsVisible(void)const{return mIsVisible;}
  bool IsOneLine(void)const{return mIsOneLine;};

  void SetAutoStepMessage(bool flg );

  // メッセージカーソルの表示・非表示 trueで表示
  void SetVisibleMsgCursor( bool isVisible );

  bool CheckLineConnectable( const gfl2::str::StrBuf &str );
  u32 ConnectLine ( const gfl2::str::StrBuf &src_str, gfl2::str::StrBuf &dest_str );  // @fix NMCat[4015]: EOMを含まない文字数を返すようにした
  bool IsExistTagCode (const STRCODE *str_ptr, STRCODE tag_code);


#if 1

  void TestChainaMessage(void);
#endif
  
private:

  // メッセージカーソル系
  void CreateMsgCursor( void );
  bool DeleteMsgCursor( void );
  void UpdateMsgCursor( void );
  void DrawMsgCursor( void );
  bool CheckLoadMsgCursor( void );

  void fileReadStructSetup(::app::util::FileAccessor::RequestData* pfrs, int arcid, int dataid, ::app::util::Heap* pAppHeap, void** pReadBuffAddr);
  
  // 汎用
  gfl2::math::Vector3 GetPanePos( ::gfl2::lyt::LytPaneIndex index );

  // 自動キー送りをするかどうか
  inline bool IsMustAutoKey( void ) const{
    return ( m_pBtlvCore->IsCommMode() || m_pBtlvCore->IsRecordPlayMode() );
  };

  // メッセージウィンドウの挙動を設定する
  bool SetMsgWinBehavior( bool isKeyWait, bool isFinishNone );

  bool CheckInput( void );


public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief  キー入力の検知
   *         ※タッチパネルの入力が優先される
   *
   * @param  pButton  ボタン ( A,B,X,Y,L,R... )
   * @param  pKey     十字キー
   * @param  pStick   アナログスティック
   *
   * @return 同フレーム内での他入力イベントを許可するならtrue、
   *         他のイベントを排除するならfalseを返却すること。
   */
  //--------------------------------------------------------------------------------------------
  virtual ::app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );
  
  
private:

  ::app::util::G2DUtil*    m_g2dUtil;                 //!< 2D関連

	::app::util::Heap* m_pAppHeap;

  void* m_pArcReadBuff;  //レイアウトデータ

  BattleViewSystem* m_pBtlvCore;

  ::app::util::AppRenderingManager* m_renderingManager;  //!< 描画マネージャ

  ::app::util::FileAccessor::RequestData m_frs;
  
  
 // print::MessageWindow         *mStrWin;
 // ::app::tool::MsgCursor  *mMsgCursor;	// メッセージカーソル

  int m_FileMode;
//  BtlvLayout mLytWin;
  bool mIsUpdateMessage;
  bool mIsVisible;
  bool mIsOneLine;
  bool mIsVisibleMsgCursor;// メッセージカーソル表示・非表示フラグ
  bool mIsKeyWait;          ///< キー送り待ち手動で行うフラグ
  int  mAutoSkipWait;     // メッセージ自動送りの場合の待ちフレーム
  int  mAutoHideWait;       ///< メッセージが自動で消えるフレーム
  Savedata::ConfigSave::MSG_SPEED  mMsgSpeed;

  BTL_CLIENT *mCallBackTarget;
  BTL_CLIENT::PrintCallbackArg mCallBackArg;

  // メッセージカーソル設定

  // メッセージ自動送りの場合の▼で待つフレーム数
  static const int AUTO_SKIP_WAIT_PAUSE = 24;

public:
  //print::MsgCallback継承
  /*
   *  タグのコールバックが呼ばれると呼ばれます。
   *  戻り値がtureの限りメッセージの更新を止めます。
   *
   *  @param[in]   arg    メッセージで指定された引数が入ります。
   *
   *  @retval   bool      trueの間メッセージの更新は止まります。
   */
  virtual bool UpdateMsgCallback( u16 arg );

};


GFL_NAMESPACE_END(app)
GFL_NAMESPACE_END(btl)





#endif // __BTLV_UI_MSGWIN_H__
