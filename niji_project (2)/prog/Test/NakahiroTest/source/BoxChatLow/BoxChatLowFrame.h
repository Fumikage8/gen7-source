#if PM_DEBUG

#if !defined( TEST_BOX_CHAT_LOW_FRAME_H_INCLUDED )
#define TEST_BOX_CHAT_LOW_FRAME_H_INCLUDED

#pragma once


// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <str/include/gfl2_Str.h>
// niji
#include "AppLib/include/Frame/AppFrameCellSuper.h"
// module
#include "App/Box/source/BoxLowChtDraw.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class Heap;
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Nakahiro)

class BoxChatLowFrame
  : public applib::frame::CellSuper
  , public App::Box::BoxLowChtDrawListener
{
  GFL_FORBID_COPY_AND_ASSIGN(BoxChatLowFrame);   // コピーコンストラクタ＋代入禁止

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //-----------------------------------------------------------------------------
  BoxChatLowFrame(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~BoxChatLowFrame();

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

  void LoadMenuCursorResource(void);
  void UnloadMenuCursorResource(void);

  void CreateMsgData(void);
  void DeleteMsgData(void);

  virtual void OnButtonAction( int button_no );


private:
  app::util::Heap * m_heap;                               //!< ヒープ
  app::util::AppRenderingManager * m_renderingManager;    //!< 描画マネージャ

  void * m_pLytResBuff;
  void * m_pMenuCursorResBuff;
  gfl2::str::MsgData * m_pMsgData;

  App::Box::BoxLowChtDraw * m_pDraw;
};


GFL_NAMESPACE_END(Nakahiro)
GFL_NAMESPACE_END(Test)


#endif  // TEST_BOX_CHAT_LOW_FRAME_H_INCLUDED

#endif  // PM_DEBUG
