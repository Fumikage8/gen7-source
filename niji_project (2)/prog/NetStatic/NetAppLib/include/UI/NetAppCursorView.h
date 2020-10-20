
#if !defined(NETAPPCURSORVIEW_H_INCLUDED)
#define NETAPPCURSORVIEW_H_INCLUDED
#pragma once
//==============================================================================
/**
  *  @file   NetAppCursorView.h
  *  @date   2015.06.16 Tue.
  *  @author Yu Muto
  */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "AppLib/source/Tool/GeneralView/Module/CursorController.h"
#include  "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"

namespace NetAppLib {
  namespace System  {
    class ApplicationWorkBase;
  }

//  namespace UI  {
//    class NetAppMenuCursor;
//  }
}



namespace NetAppLib {
namespace UI  {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  NetAppCursorView
  *  @brief  UIをカーソルで選択して動作させるための基底View
  *          物理ボタンにUIを割り当てる簡易ショートカット機能も提供する
  *          CreateCursorをコールしない場合、ショートカット機能のみの使用が可能
  *  @note   カーソル移動は上下のみの対応
  *  @note   Listener系メソッド（OnKeyActionなど）が派生側でも必要な場合は、Overrideの上明示的にsuperを呼び出す
  *  @note   カーソルの描画が必要な場合は派生側で本クラスのDrawを明示的に呼び出す
  *  @date   2015.06.16 Tue.
  */
//==============================================================================
class NetAppCursorView
  : public    NetAppLib::System::NetApplicationViewBase
  , public    app::ui::UIInputListener
  , protected app::tool::CursorController::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( NetAppCursorView );
public:
  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  NetAppCursorView(NetAppLib::System::ApplicationWorkBase* pWorkBase);

  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual ~NetAppCursorView();


  //------------------------------------------------------------------
  /**
    *  @brief   CursorControllerを指定する
    *  @note    CursorControllerはCreateCursorでSetupされるため、未SetupのものをCreateCursor前に限り設定可能
    */
  //------------------------------------------------------------------
  void  SetCursorController(app::tool::CursorController* pCursorController = NULL);


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief   カーソル(NetAppMenuCursor)を生成する
    *  @param   menuCursorResourceID    [in]  カーソルのResourceID
    *  @param   layoutResourceIndex     [in]  「カーソルを乗せるためのボタンが含まれるLayout」のResourceを取得するためのIndex
    *  @param   layoutWorkID            [in]  「カーソルを乗せるためのボタンが含まれるLayoutWork」のID
    *  @note    本関数と対にReleaseCursorを明示的にコールする必要がある
    */
  //------------------------------------------------------------------
  void  CreateCursor(
          NetAppLib::System::ResourceID               menuCursorResourceID,
          u32                                         layoutResourceIndex,
          u32                                         layoutWorkID
        );


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief   カーソル(NetAppMenuCursor)を生成する
    *  @param   menuCursorResourceID    [in]  カーソルのResourceID
    *  @param   layoutResourceIndex     [in]  「カーソルを乗せるためのボタンが含まれるLayout」のResourceを取得するためのIndex
    *  @param   layoutWorkID            [in]  「カーソルを乗せるためのボタンが含まれるLayoutWork」のID
    *  @param   pButtonPartsPaneIDTable [in]  カーソル位置に対応するボタン部品のID配列
    *  @param   pCursorPaneIDTable      [in]  ボタン部品内カーソル位置定義NULLPaneのID配列、pButtonPartsPaneIDTableと一対一で対応すること
    *  @param   pMappedUIButtonIDTable  [in]  選択位置に対応するUIボタンのID配列、pButtonPartsPaneIDTableと一対一で対応すること
    *  @param   cursorPositionMax       [in]  カーソル位置の数（pButtonPartsPaneIDTable及びpCursorPaneIDTable、pMappedUIButtonIDTableの要素数）
    *  @note    本関数と対にReleaseCursorを明示的にコールする必要がある
    */
  //------------------------------------------------------------------
  void  CreateCursor(
          NetAppLib::System::ResourceID               menuCursorResourceID,
          u32                                         layoutResourceIndex,
          u32                                         layoutWorkID,
          /*  cursorPos  */
          const u32*                                  pButtonPartsPaneIDTable,
          const u32*                                  pCursorPaneIDTable,
          const app::tool::ButtonManager::ButtonId*   pMappedUIButtonIDTable,
          s32                                         cursorPositionMax
        );

  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief   カーソル(NetAppMenuCursor)を生成する
    *  @param   menuCursorResourceID    [in]  カーソルのResourceID
    *  @param   layoutResourceIndex     [in]  「カーソルを乗せるためのボタンが含まれるLayout」のResourceを取得するためのIndex
    *  @param   layoutWorkID            [in]  「カーソルを乗せるためのボタンが含まれるLayoutWork」のID
    *  @param   pCursorPosPanes         [in]  カーソル位置を定義するPaneの配列
    *  @param   pMappedUIButtonIDTable  [in]  選択位置に対応するUIボタンのID配列、pCursorPosPanesと一対一で対応すること
    *  @param   cursorPositionMax       [in]  カーソル位置の数（pCursorPosPanes及びpMappedUIButtonIDTableの要素数）
    *  @note    本関数と対にReleaseCursorを明示的にコールする必要がある
    */
  //------------------------------------------------------------------
  void  CreateCursor(
          NetAppLib::System::ResourceID               menuCursorResourceID,
          u32                                         layoutResourceIndex,
          u32                                         layoutWorkID,
          /*  cursorPos  */
          gfl2::lyt::LytPane*                         pCursorPosPanes[],
          const app::tool::ButtonManager::ButtonId*   pMappedUIButtonIDTable,
          s32                                         cursorPositionMax
        );

  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    CreateCursorで作成したカーソルの破棄
    *            処理順の都合上dtorからはコールされず、CreateCursorと対に明示的にコールする必要がある
    *  @note     たいていは派生側で作成したG2DUtilにインスタンスを乗せることになるので、
    *            Delete2D()/DeleteGraphicsSystem()の前に本メソッドを呼ぶ必要があることに注意
    */
  //------------------------------------------------------------------
  void  ReleaseCursor(void);


  //------------------------------------------------------------------
  /**
    *  @brief    カーソル位置テーブルを設定する
    */
  //------------------------------------------------------------------
  void  SetCusorPosPanes(
          const u32*                                pButtonPartsPaneIDTable,
          const u32*                                pCursorPaneIDTable,
          const app::tool::ButtonManager::ButtonId* pMappedUIButtonIDTable,
          const s32                                 cursorPositionMax
        );
  void  SetCusorPosPanes(
          gfl2::lyt::LytPane*                       pCursorPosPanes[],
          const app::tool::ButtonManager::ButtonId* pMappedUIButtonIDTable,
          const s32                                 cursorPositionMax
        );


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    カーソルを指定ボタンの位置に移動する
    *  @note     ボタンがクリックされたときや、画面の初期状態のつじつまあわせに
    */
  //------------------------------------------------------------------
  void  CursorMoveToButton(const app::tool::ButtonManager::ButtonId buttonID);
  void  CursorMoveToIndex(const u32 mappedUIButtonIDTableIndex);


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    ショートカット登録
    */
  //------------------------------------------------------------------
//  void  RegisterShortcut_A(const app::tool::ButtonManager::ButtonId buttonID = app::tool::ButtonManager::BUTTON_ID_NULL);
  void  RegisterShortcut_B(const app::tool::ButtonManager::ButtonId buttonID = app::tool::ButtonManager::BUTTON_ID_NULL)  {m_ShortcutButton_B = buttonID;};
  void  RegisterShortcut_X(const app::tool::ButtonManager::ButtonId buttonID = app::tool::ButtonManager::BUTTON_ID_NULL)  {m_ShortcutButton_X = buttonID;};
  void  RegisterShortcut_Y(const app::tool::ButtonManager::ButtonId buttonID = app::tool::ButtonManager::BUTTON_ID_NULL)  {m_ShortcutButton_Y = buttonID;};
  void  RegisterShortcut_L(const app::tool::ButtonManager::ButtonId buttonID = app::tool::ButtonManager::BUTTON_ID_NULL)  {m_ShortcutButton_L = buttonID;};
  void  RegisterShortcut_R(const app::tool::ButtonManager::ButtonId buttonID = app::tool::ButtonManager::BUTTON_ID_NULL)  {m_ShortcutButton_R = buttonID;};
  void  RegisterShortcut_Right(const app::tool::ButtonManager::ButtonId buttonID = app::tool::ButtonManager::BUTTON_ID_NULL)  {m_ShortcutButton_RIGHT = buttonID;};
  void  RegisterShortcut_Left(const app::tool::ButtonManager::ButtonId buttonID = app::tool::ButtonManager::BUTTON_ID_NULL)  {m_ShortcutButton_LEFT = buttonID;};




  /*  implement UIView I/F  */
  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
  virtual bool  IsDrawing(void) const;
   /*  endimplement UIView I/F  */


  /*  implement UIInputListener I/F  */
  virtual void                                      OnLayoutPaneTouchTriggerEvent( const u32 buttonID );
  virtual app::ui::UIInputListener::ListenerResult  OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );
  /*  end implement UIInputListener I/F  */


  /**
   * @brief カーソル表示・非表示設定
   * @param bVisible  trueで表示
   */
  void SetCursorVisible( bool bVisible );

  /**
   * @brief   カーソルが表示されているか
   * @retval  true  = 表示されている
   * @retval  false = それ以外
   */
  bool IsCursorVisible() const;

protected:
  void  RegisterCursorLocations(const u32* pButtonPartsPaneIDTable, const u32* pCursorPaneIDTable, const s32 cursorPositionMax);
  void  RegisterCursorLocations(gfl2::lyt::LytPane*  pCursorPosPanes[], const s32 cursorPositionMax);

  void  SetMappedUIButtonIDTable(const app::tool::ButtonManager::ButtonId* pMappedUIButtonIDTable, const s32 cursorPositionMax);

  /*  implement app::tool::CursorController  */
  virtual void  CursorController_OnEvent(app::tool::CursorController* pSender, const app::tool::CursorController::IEventHandler::EventCode eventCode);




  NetAppLib::System::ApplicationWorkBase* m_pWorkBase;
  /*  cursor  */
  app::tool::CursorController             m_DefaultCursorController;
  app::tool::CursorController*            m_pCursorController;
  u32                                     m_CursorPosLayoutWorkID;
  gfl2::lyt::LytMultiResID                m_CursorPosLytMultiResID;
  app::tool::ButtonManager::ButtonId*     m_pMappedUIButtonIDTable;
  u32                                     m_MappedUIButtonIDTableElems; 
  bool                                    m_bEnableCursor;

  /*  shortcut  */
//  app::tool::ButtonManager::ButtonId  m_ShortcutButton_A;
  app::tool::ButtonManager::ButtonId  m_ShortcutButton_B;
  app::tool::ButtonManager::ButtonId  m_ShortcutButton_X;
  app::tool::ButtonManager::ButtonId  m_ShortcutButton_Y;
  app::tool::ButtonManager::ButtonId  m_ShortcutButton_L;
  app::tool::ButtonManager::ButtonId  m_ShortcutButton_R;
  app::tool::ButtonManager::ButtonId  m_ShortcutButton_RIGHT;
  app::tool::ButtonManager::ButtonId  m_ShortcutButton_LEFT;
};














} /*  namespace UI        */
} /*  namespace NetAppLib */
#endif  /*  #if !defined(NETAPPCURSORVIEW_H_INCLUDED)  */
