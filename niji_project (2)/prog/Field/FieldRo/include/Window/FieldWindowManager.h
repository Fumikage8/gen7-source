//==============================================================================
/**
 * @file        FieldWindowManager.h
 * @brief       要約説明
  
    このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/04/21(火) 13:03:16
 */
//==============================================================================

#if !defined(__FIELDWINDOWMANAGER_H__)  // 重複定義防止
#define __FIELDWINDOWMANAGER_H__  // 重複定義防止マクロ
#pragma once

#include "AppLib/include/Ui/UIView.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)


/// @brief フィールド
GFL_NAMESPACE_BEGIN( Field )
/// @brief スクリプト
GFL_NAMESPACE_BEGIN( FieldWindow )


//------------------------------------------------------------------------------
/**
 * @class       FieldWindowView
 * @brief       
 * @author      N.Takeda
 * @date        2015/04/21(火) 19:54:13
 */
//------------------------------------------------------------------------------
class FieldWindowView : public app::ui::UIView
{
  
public:
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    
  }
  
  FieldWindowView( app::util::Heap * heap, gfl2::heap::NwAllocator *pLytAlloc=NULL, gfl2::heap::NwAllocator* pLytDevAlloc=NULL )
    : app::ui::UIView( heap, pLytAlloc, pLytDevAlloc )
  {}
};


//------------------------------------------------------------------------------
/**
 * @class       FieldWindowManager
 * @brief       ウィンドウクラスのマネージャー
 * @author      N.Takeda
 * @date        2015/04/21(火) 15:24:35
 */
//------------------------------------------------------------------------------
class FieldWindowManager
{
  
  FieldWindowView                 *m_pUIView;
  
  app::util::Heap                 *m_pAppHeap;
  gfl2::heap::HeapBase            *m_pManagerHeap;
  app::util::AppRenderingManager  *m_pRenderingManager;
  gfl2::fs::AsyncFileManager      *m_pFileManager;
  
  s32                             m_iLastResultYesNo;
  s32                             m_iLastResultListMenu;
  
public:
  enum{ LASTRESULT_NULL = -2 }; ///< SCR_LISTMENU_CANCEL が 0xffff なので一応 -2 に.
  
  /// ------- setters ---------
  void  SetLastResultListMenu( const s32 _iLastResultListMenu ){  m_iLastResultListMenu = _iLastResultListMenu; }
  void  SetLastResultYesNo( const s32 _iLastResultYesNo ){  m_iLastResultYesNo = _iLastResultYesNo; }
  void  ResetLastResultListMenu(){  m_iLastResultListMenu = LASTRESULT_NULL; }
  void  ResetLastResultYesNo(){  m_iLastResultYesNo = LASTRESULT_NULL; }
  
  /// ------- getters ---------
  s32 GetLastResultListMenu() const { GFL_ASSERT( m_iLastResultListMenu != LASTRESULT_NULL ); return m_iLastResultListMenu; }
  s32 GetLastResultYesNo() const {  GFL_ASSERT( m_iLastResultYesNo != LASTRESULT_NULL ); return m_iLastResultYesNo;  }
  gfl2::fs::AsyncFileManager* GetFileManager() const {  return m_pFileManager;  }
  app::util::Heap *GetAppHeap(){  return m_pAppHeap; }
  app::util::AppRenderingManager* GetRenderingManager() const { return m_pRenderingManager; }
  FieldWindowView*  GetUIView() const { return m_pUIView; }
  
  void  Update();
  void  Draw( gfl2::gfx::CtrDisplayNo displayNo );
  
  
  ///        コンストラクタ.
  FieldWindowManager( gfl2::heap::HeapBase* pHeap, app::util::Heap *_AppHeap, app::util::AppRenderingManager *_AppRenderManager, gfl2::fs::AsyncFileManager *pFileManager = NULL );
  ///        デストラクタ.
  virtual ~FieldWindowManager();
};

GFL_NAMESPACE_END( FieldWindow )
GFL_NAMESPACE_END( Field )

#endif // __FIELDWINDOWMANAGER_H__ 重複定義防止
