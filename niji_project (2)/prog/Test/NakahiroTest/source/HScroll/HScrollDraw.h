#if PM_DEBUG

#if !defined( H_SCROLL_DRAW_H_INCLUDED )
#define H_SCROLL_DRAW_H_INCLUDED

#pragma once


// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// niji
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"
#include "AppLib/include/Tool/AppToolNewPaneList.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class ItemIcon;
class MenuCursor;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Nakahiro)

// 前方宣言
class ListSmall;
class ListMiddle;
class ListLarge;


class HScrollDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
public:
  class HScrollListener
  {
  public:
    enum ActionID
    {
      ACTION_ID_END = 0,
    };

  public:
    virtual void OnAction( ActionID id ) = 0;
  };


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap        ヒープ
   * @param   render_man  描画マネージャ
   */
  //-----------------------------------------------------------------------------
  HScrollDraw( app::util::Heap * heap, app::util::AppRenderingManager * render_man );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~HScrollDraw();

  //-----------------------------------------------------------------------------
  /**
   * @func    SetItemMakeListener
   * @brief   リスナーをセット
   * @date    2015.05.26
   *
   * @param   listener リスナー
   */
  //-----------------------------------------------------------------------------
  void SetDrawListener( HScrollListener * listener );

  //-----------------------------------------------------------------------------
  /**
   * @func    Init
   * @brief   初期化処理
   * @date    2015.05.26
   *
   * @param   res_buf レイアウトリソースバッファ
   */
  //-----------------------------------------------------------------------------
  void Init( void * res_buf );

  //-----------------------------------------------------------------------------
  /**
   * @func    End
   * @brief   終了処理
   * @date    2015.05.26
   *
   * @retval  true  = 処理完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool End(void);

  void InitSmallList( app::tool::ItemIcon * icon, app::tool::MenuCursor * cursor );
  bool ExitSmallList(void);

  void InitMiddleList( app::tool::ItemIcon * icon, app::tool::MenuCursor * cursor );
  bool ExitMiddleList(void);

  void InitLargeList( app::tool::ItemIcon * icon, app::tool::MenuCursor * cursor );
  bool ExitLargeList(void);

  void ChangeInput(void);


private:
  App::Tool::NewPaneList * GetActiveList(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update
   * @brief   更新
   * @date    2015.05.26
   */
  //-----------------------------------------------------------------------------
  virtual void Update(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Draw
   * @brief   描画
   * @date    2015.05.26
   */
  //-----------------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  //-----------------------------------------------------------------------------
  /**
   * @func    Initialize2D
   * @brief   2D関連初期化
   * @date    2015.05.26
   *
   * @param   res_buf   レイアウトリソースバッファ
   */
  //-----------------------------------------------------------------------------
  void Initialize2D( void * res_buf );

  //-----------------------------------------------------------------------------
  /**
   * @func    Terminate2D
   * @brief   2D関連削除
   * @date    2015.05.26
   */
  //-----------------------------------------------------------------------------
  void Terminate2D(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    OnKeyAction
   * @brief   キー入力の検知
   * @date    2015.03.05
   *
   * @param   button  ボタン
   * @param   key     十字キー
   * @param   stick   アナログスティック
   *
   * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
   * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
   * @retval  ENABLE_ACCESS         = それ以外
   *
   * @note  override UIInputListener
   */
  //-----------------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick );


private:
  app::util::Heap * m_heap;                             //!< ヒープ
  app::util::AppRenderingManager * m_renderingManager;  //!< 描画マネージャ

  HScrollListener * m_listener;   //!< リスナー

  ListSmall * m_pListSmall;
  ListMiddle * m_pListMiddle;
  ListLarge * m_pListLarge;

  u32 m_inputMode;
};


GFL_NAMESPACE_END(Nakahiro)
GFL_NAMESPACE_END(Test)


#endif  // H_SCROLL_DRAW_H_INCLUDED

#endif  // PM_DEBUG
