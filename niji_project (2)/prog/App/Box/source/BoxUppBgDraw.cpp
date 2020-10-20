//======================================================================
/**
 * @file    BoxUppBgDraw.cpp
 * @date    2015/10/30
 * @author  k.ohno
 * @brief   BOX上画面背景
 */
//======================================================================
#include "App/Box/source/BoxUppBgDraw.h"

//  nijiのインクルード
#include "AppLib/include/Util/AppRenderingManager.h"

//  素材のインクルード
#include "niji_conv_header/app/box/res2d/bgrupp.h"
#include "niji_conv_header/app/box/res2d/bgrupp_pane.h"
#include "niji_conv_header/app/box/res2d/bgrupp_anim_list.h"


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

BoxUppBgDraw::~BoxUppBgDraw()
{
}

//--------------------------------------------------------------------------------------------
/**
 * @brief  初期化処理
 * @return true 処理終了 / false 処理が終了していないので再度呼び出す
 * @note
 *         返り値が true になるまで呼び出す
 */
//--------------------------------------------------------------------------------------------
bool BoxUppBgDraw::InitFunc()
{
  {
    //  2D画面を生成
    {
      _Init2D();
    }
    GetG2DUtil()->StartAnime( LYT_WORK_NORMAL, LA_BGRUPP_BOX_BGR_UPP_000_KEEP );

  }
  return  true;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief  終了処理
 * @return true 処理終了 / false 処理が終了していないので再度呼び出す
 * @note
 *         返り値が true になるまで
 *         この関数を呼び出すとInitFunc()で初期化しないと使えないので注意
 *         使うケースとしては画面をもう使わないときの後処理に使う
 */
//--------------------------------------------------------------------------------------------
bool BoxUppBgDraw::EndFunc()
{
  UIView::Delete2D();
  return true;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief  更新処理
 *         子ウインドウについても自動的にコールする(子→親の順）
 */
//--------------------------------------------------------------------------------------------
void BoxUppBgDraw::Update()
{
  UIView::Update();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief  描画処理
 *         子ウインドウについても自動的にコールする(親→子の順）
 */
//--------------------------------------------------------------------------------------------
void BoxUppBgDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  pG2DUtil->AddDrawableLytWkForOneFrame( m_pRenderinManager, displayNo, LYT_WORK_NORMAL );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief  2D初期化処理
 * @note
 *     レイアウトデータから描画に必要な生成をしている
 */
//--------------------------------------------------------------------------------------------
void BoxUppBgDraw::_Init2D()
{
  // アーカイブIDテーブル
  const ::app::util::G2DUtil::LytResData aResTbl[] =
  {
    {  m_pLytBinData, 1, ::app::util::G2DUtil::ATTACH_ROOT },
  };

  //  作成するレイアウトワーク
  const ::app::util::G2DUtil::LytwkData aSetupData[ LYT_WORK_MAX ] =
  {
    {
      0,
      LYTRES_BGRUPP_BOX_BGR_UPP_000_BFLYT,
      LA_BGRUPP_BOX_BGR_UPP_000___NUM,
      m_pLytBinData,
      true,
      ::app::util::G2DUtil::SETUP_UPPER,
      true
      }
  };

  Create2D( m_pUtilHeap, NULL, 32, aResTbl, GFL_NELEMS(aResTbl), aSetupData, GFL_NELEMS(aSetupData), app::ui::UIView::UNUSE_MESSAGE_DATA );
}



GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )
