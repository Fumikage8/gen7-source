//======================================================================
/**
 * @file    BoxUppBgDraw.h
 * @date    2015/10/30
 * @author  k.ohno
 * @brief   Boxの上画面背景
 */
//======================================================================

#if !defined __BOX_UPP_BG_DRAW_H_INCLUDED__
#define __BOX_UPP_BG_DRAW_H_INCLUDED__
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>

//  nijiのインクルード
#include "AppLib/include/ui/UIView.h"
#include "AppLib/include/Tool/AppToolTrainerIconRendering.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(PokeTool)

  class PokeModelSystem;

GFL_NAMESPACE_END(PokeTool)

GFL_NAMESPACE_BEGIN(poke_3d)
GFL_NAMESPACE_BEGIN(model)

    class BaseCamera;

GFL_NAMESPACE_END(model)
GFL_NAMESPACE_END(poke_3d)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

  class AppRenderingManager;
  class Heap;

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  class PokeSimpleModelInFrame;

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

  /**
   * @class BoxUppBgDraw
   * @brief 上画面背景
   */
  class BoxUppBgDraw : public app::ui::UIView
  {
      GFL_FORBID_COPY_AND_ASSIGN(BoxUppBgDraw);
  public:

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      初期設定
    * @param[in]  pLytBinData       : レイアウトデータ
    * @param[in]  pRenderingManager : 描画管理
    * @param[in]  pUtilHeap         : ヒープ
    * @param[in]  pBoxMsgData       : Box用のメッセージデータ
    * @note
    *     1:  各引数のポインタはクラスの変数に設定して使われるのでポインタ先のメモリ開放するとハングするので注意
    *     2:  pBoxMsgDataの引数に渡すメッセージファイルは「box.mstxt」でないといけない
    *     3:  上画面にモデルを描画するので引数 pRenderinManager には上画面のポケモンモデル描画設定したのを渡さないといけない
    */
    //--------------------------------------------------------------------------------------------
    BoxUppBgDraw( void* pLytBinData, app::util::AppRenderingManager* pRenderinManager, app::util::Heap* pHeap ) : app::ui::UIView( pHeap )
    {
      GFL_ASSERT( pLytBinData );
      GFL_ASSERT( pRenderinManager );
      GFL_ASSERT( pHeap );

//      _Clear();

      m_pLytBinData       = pLytBinData;
      m_pRenderinManager  = pRenderinManager;
      m_pUtilHeap         = pHeap;
    }

    virtual ~BoxUppBgDraw();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      初期化処理
    * @param[in]  pLytBinData       : レイアウトデータ
    * @param[in]  pRenderingManager : 描画管理
    * @param[in]  pUtilHeap         : ヒープ
    * @return     true 処理終了 / false 処理が終了していないので再度呼び出す
    */
    //--------------------------------------------------------------------------------------------
    bool InitFunc();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  終了処理
    * @return true 処理終了 / false 処理が終了していないので再度呼び出す
    */
    //--------------------------------------------------------------------------------------------
    bool EndFunc();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  更新処理
    *         子ウインドウについても自動的にコールする(子→親の順）
    */
    //--------------------------------------------------------------------------------------------
    virtual void Update();

  protected:
    //--------------------------------------------------------------------------------------------
    /**
    * @brief  描画処理
    *         子ウインドウについても自動的にコールする(親→子の順）
    */
    //--------------------------------------------------------------------------------------------
    virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  private:
    enum
    {
      LYT_WORK_NORMAL = 0,
      LYT_WORK_MAX,
    };


    //--------------------------------------------------------------------------------------------
    /**
    * @brief  2D初期化処理
    * @note
    *     レイアウトデータから描画に必要な生成をしている
    */
    //--------------------------------------------------------------------------------------------
    void _Init2D();


    void* m_pLytBinData;

    app::util::AppRenderingManager*                           m_pRenderinManager;
    app::util::Heap*                                          m_pUtilHeap;


  };

GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )

#endif // __BOX_UPP_BG_DRAW_H_INCLUDED__
