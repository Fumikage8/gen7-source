// ============================================================================
/*
 * @file JoinFestaPlayerListShowSelectFrame.h
 * @brief 閲覧アプリのSDカード読み込みプロセスです。
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSHOWSELECT_FRAME_H_INCLUDE )
#define JOINFESTAPLAYERLISTSHOWSELECT_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/JoinFestaPlayerListShow/source/Frame/JoinFestaPlayerListShowFrameBase.h"
#include "NetApp/JoinFestaPlayerListShow/source/System/JoinFestaPlayerListShowWork.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListSelectLowerViewListener.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListMessageMenuLowerViewListener.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)
GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaPlayerListShowSelectFrame :
  public NetApp::JoinFestaPlayerListShow::Frame::JoinFestaPlayerListShowFrameBase,
  public NetAppLib::JoinFesta::JoinFestaPersonalListener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListShowSelectFrame );
public:
  JoinFestaPlayerListShowSelectFrame(
    NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowWork* pJoinFestaPlayerListShowWork,
    NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowViewSystemAccessor* pJoinFestaPlayerListShowViewSystemAccessor );
  virtual~JoinFestaPlayerListShowSelectFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();
  virtual void OnWLANSwitchDisable();

  // JoinFestaPersonalListener
  virtual void OnAddJoinFestaPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person );
  virtual void OnUpdateJoinFesatPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person, bool isUpdate );
  virtual void OnDeleteJoinFestaPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person );

  void setupList();

  void setVisibleUpdateButton( bool bVisible );

  void setActivePlayerData( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pActivePlayerData );
  const NetAppLib::JoinFestaUI::JoinFestaPlayerData*  getActivePlayerData(void) const;

  /*  photo */
  void callPhotoView(void);
  void callPhotoSlideShow(void);
  void applyPhotoNewFlag(void);


private:
  /**
    @fix    MMcat[342]   【フェスサークル】「しゃしんをみる」「スライドショー」のボタンアイコンタッチ直後にタッチ操作を受け付けている
            nijiにあったボタン類はその場でFrameが終了するか、ModalViewが上に追加されるため問題はなかったが、写真系はFadeをはさんで遷移し、且つ戻ってくるため明示的に入力の排他が必要
    @brief  入力排他
  */
  void  LockLowerListView(const bool bLock);

  NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewListener        m_JoinFestaPlayerListSelectLowerViewListener;
  NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener   m_JoinFestaPlayerListMessageMenuLowerViewListener;
  bool                                                                      m_bUpdateButtonVisible;
  bool                                                                      m_bWaitForPhotoViewEvent;
  bool                                                                      m_bNeedToRemoveLowerView;
  bool                                                                      m_bRequestedSlideShow;
};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSHOWSELECT_FRAME_H_INCLUDE
