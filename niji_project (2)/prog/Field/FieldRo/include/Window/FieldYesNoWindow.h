//==============================================================================
/**
 * @file        FieldYesNoWindow.h
 * @brief       要約説明
  
    このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/04/21(火) 13:03:31
 */
//==============================================================================

#if !defined(__FIELDYESNOWINDOW_H__)  // 重複定義防止
#define __FIELDYESNOWINDOW_H__  // 重複定義防止マクロ
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
 * @class FieldYesNoWindow
 * @brief バッグ画面表示クラス
 * @date  2015.03.04
 */
//=============================================================================
class FieldYesNoWindow
{
  bool                            m_bDecide;

  gfl2::heap::HeapBase            *m_pHeap;
  app::util::Heap                 m_appUtilHeap;

  App::Tool::ListMenu             *m_pListMenu;
  
  Field::FieldWindow::FieldWindowManager  *m_pFieldWindowManager;
public:

  void  DeleteMenu(){ m_pListMenu->DeleteMenu();  }
  bool IsInstanceDelete(){ return m_pListMenu->IsInstanceDelete();  }
  void  SetVisible( bool flag ){ m_pListMenu->SetVisible( flag );  }
  
  /// ------- getters ---------
  bool  GetDecide() const;
  s32  GetSelectId() const;
  
  /// ------- setters ---------
  void  SetDecide( const bool _bDecide ){ m_bDecide = _bDecide; }
  
  ///        コンストラクタ.
  FieldYesNoWindow( Field::FieldWindow::FieldWindowManager *_pFieldWindowManager,
    s32 _iX = 0,
    s32 _iY = 0,
    gfl2::fs::ArcFile::ARCID _uArcId = print::GetMessageArcId_Script(),
    gfl2::fs::ArcFile::ARCDATID _uDatId = GARC_script_message_common_scr_DAT,
    s32 _iMsg1 = msg_common_scr_yes,
    s32 _iMsg2 = msg_common_scr_no,
    s32 _iStartIdx = 0,
    u32 _uCancelType = App::Tool::ListMenu::B_BUTTON_MODE_CANCEL,
    void *_pMshData = NULL
  );
  ///        デストラクタ.
  virtual ~FieldYesNoWindow();
};

GFL_NAMESPACE_END( FieldWindow )
GFL_NAMESPACE_END( Field )

#endif // __FIELDYESNOWINDOW_H__ 重複定義防止
