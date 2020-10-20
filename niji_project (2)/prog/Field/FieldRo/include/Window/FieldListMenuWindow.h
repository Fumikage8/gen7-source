//==============================================================================
/**
 * @file        FieldListMenuWindow.h
 * @brief       要約説明
  
    このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/05/08(金) 21:22:26
 */
//==============================================================================

#if !defined(__FIELDLISTMENUWINDOW_H__) // 重複定義防止
#define __FIELDLISTMENUWINDOW_H__ // 重複定義防止マクロ
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>

#include <pml/include/pmlib.h>

#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "AppLib/include/Tool/ListMenu.h"   //
#include "Field/FieldRo/include/Window/FieldListMenuWindow.h"

#include "Print/include/PrintSystem.h"
#include "niji_conv_header/message/script/msg_common_scr.h"
#include "arc_index/script_message.gaix"


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Tool )
class ListMenu;
GFL_NAMESPACE_END( Tool )
GFL_NAMESPACE_END( App )


/// @brief フィールド
GFL_NAMESPACE_BEGIN( Field )
/// @brief スクリプト
GFL_NAMESPACE_BEGIN( FieldWindow )

class FieldWindowManager;

//=============================================================================
/**
 * @class FieldListMenuWindow
 * @brief バッグ画面表示クラス
 * @date  2015.03.04
 */
//=============================================================================
class FieldListMenuWindow
{
  enum{ TEMP_STRBUF_LENGTH = (18*3+1) };    //!< 汎用文字列バッファの文字数
  
  struct StItem{
    gfl2::str::StrBuf *m_pTempStrBuf;
    s32               m_iIndex;
  };
  
  s32                             m_iX;
  s32                             m_iY;
  gfl2::fs::ArcFile::ARCID        m_uArcId;
  gfl2::fs::ArcFile::ARCDATID     m_uDatId;
  s32                             m_iStartIdx;
  u32                             m_uCancelType;
  bool                            m_bIsRight;
  
  bool                            m_bDecide;

  gfl2::heap::HeapBase            *m_pHeap;
  App::Tool::ListMenu             *m_pListMenu;
  gfl2::str::MsgData              *m_pMsgData;
  bool                            m_bExtMsgData;
  app::util::Heap                 m_appUtilHeap;
  
  Field::FieldWindow::FieldWindowManager  *m_pFieldWindowManager;

  StItem  m_stItem[ App::Tool::ListMenu::ITEMLIST_MAX ];
  s32     m_iIteMax;

  print::WordSet  *m_pWordSet;
public:

  void  AddItem( s32 _iMsg1, s32 _iIndex = 0 );
#if PM_DEBUG
  void  DebugAddItem( const char* pText, s32 _iIndex = 0 );
#endif
  void  Start();

  void  DeleteMenu(){ m_pListMenu->DeleteMenu();  }
  bool IsInstanceDelete(){ return m_pListMenu->IsInstanceDelete();  }
  void  SetVisible( bool flag ){ m_pListMenu->SetVisible( flag );  }
  
  /// ------- getters ---------
  print::WordSet* GetWordSet() const {  return m_pWordSet;  }
  bool  GetDecide() const;
  s32  GetSelectId() const;
  
  /// ------- setters ---------
  void  SetWordSet( print::WordSet* _pWordSet ){  m_pWordSet = _pWordSet; }
  void  SetDecide( const bool _bDecide ){ m_bDecide = _bDecide; }
  
  ///        コンストラクタ.
  FieldListMenuWindow( Field::FieldWindow::FieldWindowManager *_pFieldWindowManager,
    s32 _iX = 0,
    s32 _iY = 0,
    gfl2::fs::ArcFile::ARCID _uArcId = print::GetMessageArcId_Script(),
    gfl2::fs::ArcFile::ARCDATID _uDatId = GARC_script_message_common_scr_DAT,
    s32 _iStartIdx = 0,
    u32 _uCancelType = App::Tool::ListMenu::B_BUTTON_MODE_CANCEL,
    bool _bIsRight = false,
    gfl2::str::MsgData *_pMshData = NULL
  );
  ///        デストラクタ.
  virtual ~FieldListMenuWindow();
};

GFL_NAMESPACE_END( FieldWindow )
GFL_NAMESPACE_END( Field )

#endif // __FIELDLISTMENUWINDOW_H__ 重複定義防止
