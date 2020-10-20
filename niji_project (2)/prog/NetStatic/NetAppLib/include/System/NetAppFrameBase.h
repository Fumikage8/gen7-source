// ============================================================================
/*
 * @file NetAppFrameBase.h
 * @brief 通信アプリ用フレーム基底クラスです。
 * @date 2015.06.17
 */
// ============================================================================
#if !defined( NETAPPFRAMEBASE_H_INCLUDE )
#define NETAPPFRAMEBASE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "AppLib/include/Frame/AppFrameCellSuper.h"
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(System)


class NetAppFrameBase : public applib::frame::CellSuper
{
  GFL_FORBID_COPY_AND_ASSIGN( NetAppFrameBase );

public:

  NetAppFrameBase( NetAppLib::System::ApplicationWorkBase* pWork );
  virtual~NetAppFrameBase();

  // CellSuper
  virtual applib::frame::Result InitFunc( void );
  virtual applib::frame::Result EndFunc( void );
  virtual applib::frame::Result UpdateFunc( void );
  virtual void DrawFunc( gfl2::gfx::CtrDisplayNo no );

  void FadeInRequest(){ m_bFadeInRequest = true; }
  void FadeOutRequest(){ m_bFadeOutRequest = true; }
  void FadeOutExecuted(){ m_bFadeOutExecuted = true; }

  //! @brief システム側で一時停止呼び出し時にコールされる
  virtual void OnSuspend(){}
  //! @brief システム側で再起動呼び出し時にコールされる
  virtual void OnResume(){}

  //セーブ機能
  bool UpdateSave( void );

protected:

  //! フレームを終了します(自身の処理を終わらせます)
  virtual void exitFrame( NetAppLib::System::FrameResult result );

private:
  
  //! このフレームの起動処理を記述します。true で終了
  virtual bool startup(){ return true; }
  //! このフレームが終了してもよいかの判定を記述します。true で終了
  virtual bool cleanup(){ return true; }

  //! このフレームで実装するリスナーの登録処理を記述します。
  virtual void setListener(){}
  //! このフレームで実装するリスナーの解除処理を記述します。
  virtual void removeListener(){}

  //! このフレームのシーケンス制御処理を記述します。
  virtual void updateSequence(){}


  //! 無線スイッチがOnになった時に1回だけ呼ばれる
  virtual void OnWLANSwitchEnable(){}
  //! 無線スイッチがOffになった時に1回だけ呼ばれる
  virtual void OnWLANSwitchDisable(){}

  enum InitSequence
  {
    INIT_SEQUENCE_INITIALIZE_SERVER_CLIENT_REQUEST_MANAGER,
    INIT_SEQUENCE_STARTUP,
    INIT_SEQUENCE_FADE_IN_START,
    INIT_SEQUENCE_FADE_IN_WAIT
  };

  enum EndSequence
  {
    END_SEQUENCE_FADE_OUT,
    END_SEQUENCE_SAVE_WAIT,
    END_SEQUENCE_REQUEST_CANCEL_CHECK,
    END_SEQUENCE_REQUEST_CANCEL_WAIT,
    END_SEQUENCE_CLEANUP,
    END_SEQUENCE_FINALIZE_SERVER_CLIENT_REQUEST_MANAGER
  };

  enum WLANSwitchState
  {
    WLAN_SWITCH_STATE_ON,
    WLAN_SWITCH_STATE_OFF
  };

private:

  NetAppLib::System::ApplicationWorkBase*     m_pWork;
  bool                                        m_bFadeInRequest;
  bool                                        m_bFadeOutRequest;
  bool                                        m_isSaveRequest;
  WLANSwitchState                             m_eWLANSwitchState;

  // @fix GFNMCat[5228]：フェードアウト後にフレームのInitでフェードインが呼ばれてしまうタイミングを解消
  bool                                        m_bFadeOutExecuted;
};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(NetAppLib)

#endif // NETAPPFRAMEBASE_H_INCLUDE
