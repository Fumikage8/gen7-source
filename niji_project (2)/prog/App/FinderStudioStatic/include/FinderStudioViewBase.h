#if !defined( __FINDER_STUDIO_VIEW_BASE_H_INCLUDED__ )
#define __FINDER_STUDIO_VIEW_BASE_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioViewBase.h
 * @date    2016/11/18 11:18:16
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ：Viewベース
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <AppLib/include/UI/UIView.h>
#include <AppLib/include/UI/UIResponder.h>
#include <Sound/include/Sound.h>

#include <str/include/gfl2_Str.h>
#include <print/include/WordSet.h>


//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )
class MenuCursor;
class AppCommonGrpIconData;
GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_BEGIN( util )
class KeyRepeatController;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )

//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
class ViewUtil;
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
///
/// @brief    ViewBaseクラス
///
//////////////////////////////////////////////////////////////////////////
class ViewBase
  : public app::ui::UIView
{
public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  ViewBase( app::util::Heap* heap, app::util::AppRenderingManager* renderMan, void* cursorBuf = NULL );
  virtual ~ViewBase( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   初期化・終了
  //-------------------------------------------------------
  virtual void Initialize( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );
  virtual bool Terminate( void ) { return true; }

  //-------------------------------------------------------
  // @brief   更新・描画
  //-------------------------------------------------------
  virtual void Update( void );
  virtual void Draw( gfl2::gfx::CtrDisplayNo no );

  //-------------------------------------------------------
  // @brief   表示・非表示
  //-------------------------------------------------------
  virtual void Show( void );
  virtual void Hide( void );

protected:
  //-------------------------------------------------------
  // @brief   初期化
  //-------------------------------------------------------
  virtual void Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet ) { ; }
  virtual void InitPane( void ) { ; }
  virtual void InitText( void ) { ; }
  virtual void InitButton( void ) { ; }
  virtual void InitAnim( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   リザルト 設定・取得
  //-------------------------------------------------------
  void SetResult( u32 result ) { m_Result = result; }
  u32  GetResult( void ) const { return m_Result; }

  //-------------------------------------------------------
  // @brief   メニューカーソル
  //-------------------------------------------------------
  void CreateMenuCursor( void );
  void DeleteMenuCursor( void );
  void PutCursor( gfl2::lyt::LytPane* pane, bool visible = true );
  void VisibleCursor( void );
  void InvisibleCursor( void );

  //-------------------------------------------------------
  // @brief   共通アイコンクラス 設定
  //-------------------------------------------------------
  void SetCommonIcon( app::tool::AppCommonGrpIconData* icon ) { m_pCommonIcon = icon; }

protected:
  //-------------------------------------------------------
  // @brief   レイアウトシステム 初期化・終了
  //-------------------------------------------------------
  void InitializeSystem( void );
  void TerminateSystem( void );

  //-------------------------------------------------------
  // @brief   ViewUtilクラス 生成・破棄
  //-------------------------------------------------------
  void CreateViewUtil( u32 lytID = 0 );
  void DeleteViewUtil( void );

  //-------------------------------------------------------
  // @brief   キーリピートクラス 生成・破棄
  //-------------------------------------------------------
  void CreateKeyRepeatController( u32 posMaxY, u32 posMaxX );
  void DeleteKeyRepeatController( void );

  //-------------------------------------------------------
  // @brief   更新・描画
  //-------------------------------------------------------
  void _Update( void );
  void _Draw( gfl2::gfx::CtrDisplayNo no );

  //-------------------------------------------------------
  // @brief   表示・非表示
  //-------------------------------------------------------
  void _Show( void );
  void _Hide( void );

  //-------------------------------------------------------
  // @brief   View設定
  //-------------------------------------------------------
  void SetUIInputExclusiveKeyByButtonAnimation( void );
  void ResetUIInputExclusiveKeyByButtonAnimation( void );

protected:
  app::util::Heap*                  m_pHeap;
  app::util::AppRenderingManager*   m_pAppRenderingManager;
  app::tool::MenuCursor*            m_pMenuCursor;
  app::tool::AppCommonGrpIconData*  m_pCommonIcon;
  app::util::KeyRepeatController*   m_pKeyController;

  ViewUtil*   m_pUtil;
  void*       m_pCursorBuf;

  u32   m_Result;
  u8    m_CursorPos;

  bool  m_bDrawEnable;
};




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ButtonInfoクラス
///
//////////////////////////////////////////////////////////////////////////
class ButtonInfo
{
public:
  static void SetupBtnInfo( app::ui::ButtonInfoEx* info, u32 id, u32 touchAnim );

public:
  // ボタンアニメ
  enum {
    BTN_ANIM_TOUCH,
    BTN_ANIM_RELEASE,
    BTN_ANIM_CANCEL,
    BTN_ANIM_PASSIVE,
    BTN_ANIM_ACTIVE,
    BTN_ANIM_TOUCH_RELEASE,
    BTN_ANIM_NUM,
  };

private:
  static app::ui::ButtonInfoEx  m_DefBtnInfo;
};

GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_VIEW_BASE_H_INCLUDED__
