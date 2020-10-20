//=============================================================================
/**
 * @file    QuickMatchUpperView.h
 * @brief   クイックマッチ画面
 * @author  kousaka_shin
 * @date    
 */
//=============================================================================
#if !defined( QUICKMATCH_UPPERVIEW_H_INCLUDED )
#define QUICKMATCH_UPPERVIEW_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <Fade/include/gfl2_FadeManager.h>
// niji
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"





// 前方宣言
namespace NetApp
{
  namespace QuickMatch
  {
    namespace System
    {
      class QuickMatchWork;
    }
  }
}


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QuickMatch)
GFL_NAMESPACE_BEGIN(View)


//=============================================================================
/**
 * @class QuickMatchUpperView
 * @brief 
 * @date  2015.07.15
 */
//=============================================================================
class QuickMatchUpperView
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
public:


private:

  enum
  {
    MAINSEQ_MAIN = 0,
  };

  enum
  {
    BUTTON_PANE_MAX = 1,//ボタンの数
    ANIME_MAX = 4,
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
  QuickMatchUpperView( NetApp::QuickMatch::System::QuickMatchWork* m_pQuickMatchWork , app::util::Heap * pHeap );
  virtual ~QuickMatchUpperView();

  void SetCursorIndex( u32 index );
  void ShowClientMessage( void );

  void HideTextBox( void );
  void HideAllLayout( void );

private:

  virtual void Update(void);
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 button_id );
  virtual void OnLayoutPaneTouchTriggerEvent( const u32 button_id );
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick );



private:

  void UpdateMain( void );

  void CreateLayout(void);
  void DeleteLayout(void);


  void CreateButton(void);
  void CreateKeyButton( app::ui::ButtonInfoLytWk * info );

  



private:
  app::util::Heap * m_heap;

  gfl2::str::MsgData * m_pMsgData;      //!< メッセージデータ
  print::WordSet * m_pWordSet;          //!< 単語セット


  bool m_isDrawEnable;

  NetApp::QuickMatch::System::QuickMatchWork* m_pQuickMatchWork;
};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetApp)
#endif  // QUICKMATCH_UPPERVIEW_H_INCLUDED
