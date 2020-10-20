#if PM_DEBUG

#if !defined( H_SCROLL_FRAME_H_INCLUDED )
#define H_SCROLL_FRAME_H_INCLUDED

#pragma once


// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <str/include/gfl2_Str.h>
// niji
#include "AppLib/include/Frame/AppFrameCellSuper.h"
// module
#include "HScrollDraw.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(app)

GFL_NAMESPACE_BEGIN(util)
class Heap;
class AppRenderingManager;
GFL_NAMESPACE_END(util)

GFL_NAMESPACE_BEGIN(tool)
class ItemIcon;
class MenuCursor;
GFL_NAMESPACE_END(tool)

GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Nakahiro)

class HScrollFrame
  : public applib::frame::CellSuper
  , public HScrollDraw::HScrollListener
{
  GFL_FORBID_COPY_AND_ASSIGN(HScrollFrame);   // コピーコンストラクタ＋代入禁止

private:
  static const u32 ITEM_ICON_MAX = 10+4+3;

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //-----------------------------------------------------------------------------
  HScrollFrame(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~HScrollFrame();

  //-----------------------------------------------------------------------------
  /**
   * @func    Setup
   * @brief   パラメータセット
   * @date    2015.05.26
   *
   * @parma   heap        ヒープ
   * @param   render_man  描画マネージャー
   */
  //-----------------------------------------------------------------------------
  void Setup( app::util::Heap * heap, app::util::AppRenderingManager * render_man );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitFunc
   * @brief   フレーム初期化
   * @date    2015.05.26
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result InitFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    EndFunc
   * @brief   フレーム終了
   * @date    2015.05.26
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result EndFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateFunc
   * @brief   フレーム更新
   * @date    2015.05.26
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result UpdateFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DrawFunc
   * @brief   フレーム描画
   * @date    2015.05.26
   *
   * @param   no  描画先のディスプレイ番号
   */
  //-----------------------------------------------------------------------------
  virtual void DrawFunc( gfl2::gfx::CtrDisplayNo no );


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    CreateDraw
   * @brief   描画クラス生成
   * @date    2015.05.26
   */
  //-----------------------------------------------------------------------------
  void CreateDraw(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteDraw
   * @brief   描画クラス削除
   * @date    2015.05.26
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteDraw(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadLayoutResource
   * @brief   レイアウトリソース読み込み
   * @date    2015.05.26
   */
  //-----------------------------------------------------------------------------
  void LoadLayoutResource(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UnloadLayourResource
   * @brief   レイアウトリソース削除
   * @date    2015.05.26
   */
  //-----------------------------------------------------------------------------
  void UnloadLayoutResource(void);

  void CreateItemIcon(void);
  bool DeleteItemIcon(void);

  void CreateMenuCursor(void);
  void DeleteMenuCursor(void);


  virtual void OnAction( HScrollDraw::HScrollListener::ActionID id );


private:
  app::util::Heap * m_heap;                               //!< ヒープ
  app::util::AppRenderingManager * m_renderingManager;    //!< 描画マネージャ

  void * m_pLytResBuff;
  app::tool::ItemIcon * m_pItemIcon;
  app::tool::MenuCursor * m_pMenuCursor[3];
  void * m_pMenuCursorResBuff;

  HScrollDraw * m_pDraw;
};


GFL_NAMESPACE_END(Nakahiro)
GFL_NAMESPACE_END(Test)


#endif  // H_SCROLL_FRAME_H_INCLUDED

#endif  // PM_DEBUG
