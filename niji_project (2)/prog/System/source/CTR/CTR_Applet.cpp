#if defined(GF_PLATFORM_CTR)
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_Applet.h
 *	@brief  アプレット　管理
 *	@author tomoya takahashi
 *	@date		2013.02.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include "System/include/CTR/CTR_Applet.h"
#include <nn.h>
#include <debug/include/gfl2_assert.h>
#include <Layout/include/gfl2_GraphicsType.h>
#include <system/include/Date/gfl2_Date.h>
GFL_NAMESPACE_BEGIN( System )
GFL_NAMESPACE_BEGIN( CTR )

  //----------------------------------------------------------------------------
  /**
   *  @brief  アプレットコール前処理
   *
   *  @note 内部で、AppletInterface->SaveApplication()をコール
   */
  //-----------------------------------------------------------------------------
  void Applet::AppletCallBefore( void )
  {
    //------------------------
    //SDK4.0からコマンドリストを完了させることが必須になりました！
    //------------------------
    //実行中のコマンドを終わらせる。
    nngxWaitCmdlistDone();
    nngxWaitVSync(NN_GX_DISPLAY_BOTH);
    //------------------------
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  アプレットコール後処理　（アプリケーションに復帰するときのみ使用する）
   *
   *  @note 内部で、AppletInterface->RecoverApplication()をコール
   */
  //-----------------------------------------------------------------------------
  void Applet::AppletCallAfter( void )
  {
    nngxWaitCmdlistDone();  ///< 実行を完了させる。
    GFL_GRPGL_ERROR_CHECK();

    /* gx ライブラリを使用している場合 */
    nngxUpdateState(NN_GX_STATE_ALL);
    /* GD ライブラリを使用している場合 */
    nn::gd::System::ForceDirty(nn::gd::System::MODULE_ALL);

    //------------------------

    //------------------------
    //時間復帰
    //------------------------
    gfl2::system::Date::Update(true);
  }



GFL_NAMESPACE_END( CTR )
GFL_NAMESPACE_END( System )


#endif
