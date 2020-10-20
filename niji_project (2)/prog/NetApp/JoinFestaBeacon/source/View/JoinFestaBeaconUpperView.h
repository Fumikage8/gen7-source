//=============================================================================
/**
 * @file    JoinFestaBeaconUpperView.h
 * @brief   ビーコン上画面
 * @author  kousaka_shin
 * @date    
 */
//=============================================================================
#if !defined( JOINFESTABEACON_UPPERVIEW_H_INCLUDED )
#define JOINFESTABEACON_UPPERVIEW_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <Fade/include/gfl2_FadeManager.h>
// niji
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"

#include <NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h>



// 前方宣言
namespace NetApp
{
  namespace JoinFestaBeacon
  {
    namespace System
    {
      class JoinFestaBeaconWork;
    }
  }
}


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)
GFL_NAMESPACE_BEGIN(View)


//=============================================================================
/**
 * @class JoinFestaBeaconUpperView
 * @brief 
 * @date  2015.07.15
 */
//=============================================================================
class JoinFestaBeaconUpperView
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
public:


private:


  enum
  {
    PLAYER_ICON_MAX = 3,
  };



  enum LytID
  {
    LYTID_MAIN = 0,

    LYTID_MAX
  };


  enum ButtonID
  {
    BUTTON_KEY_CANCEL = 0,

    BUTTON_MAX
  };


public:
  JoinFestaBeaconUpperView( NetApp::JoinFestaBeacon::System::JoinFestaBeaconWork* m_pJoinFestaBeaconWork , app::util::Heap * pHeap );
  virtual ~JoinFestaBeaconUpperView();

public:

  //-----------------------------------------------------------------------------
  /**
   * @brief   プレイヤーアイコンの表示
   */
  //-----------------------------------------------------------------------------
  void SetUpPlayerIcon( void );

private:

  //-----------------------------------------------------------------------------
  /**
   * @brief   プレイヤーアイコンにデータを渡す
   */
  //-----------------------------------------------------------------------------
  void CreatePlayerIcon( u32 index , NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData );

private:

  virtual void Update(void);
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 button_id );
  virtual void OnLayoutPaneTouchTriggerEvent( const u32 button_id );
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick );

private:

  void CreateLayout(void);
  void DeleteLayout(void);
  void CreateButton(void);
  void CreateKeyButton( app::ui::ButtonInfoLytWk * info );

private:
  app::util::Heap * m_heap;

  gfl2::str::MsgData * m_pMsgData;      //!< メッセージデータ
  print::WordSet * m_pWordSet;          //!< 単語セット

  NetApp::JoinFestaBeacon::System::JoinFestaBeaconWork* m_pJoinFestaBeaconWork;

  bool m_isPlayerIconSetup[PLAYER_ICON_MAX];
};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)
#endif  // JOINFESTABEACON_UPPERVIEW_H_INCLUDED
