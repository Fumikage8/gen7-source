//======================================================================
/**
 * @file BattleVideoPlayerLowDataPlayerView.h
 * @date 2015/09/24 16:23:10
 * @author uchida_yuto
 * @brief アプリの下画面データ再生View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "NetApp/BattleVideoPlayer/source/View/BattleVideoPlayerLowDataPlayerView.h"

//  nijiのインクルード
#include "AppLib/include/util/app_util_GetPaneHelper.h"

//  レイアウトリソースのインクルード
#include "niji_conv_header/app/battle_video_player/BattleVideoPlayerLytData.h"
#include "niji_conv_header/app/battle_video_player/BattleVideoPlayerLytData_anim_list.h"
#include "niji_conv_header/app/battle_video_player/BattleVideoPlayerLytData_pane.h"

#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideoPlayer)

  LowDataPlayerView::LowDataPlayerView( INIT_PARAM& rInitParam ) :
  UIView( rInitParam.pHeap, rInitParam.pHeap->GetDeviceAllocator(), rInitParam.pHeap->GetDeviceAllocator() )
  {
    _Clear();

    GFL_ASSERT( rInitParam.pRenderManager );
    GFL_ASSERT( rInitParam.pLytBinData );
    GFL_ASSERT( rInitParam.pAppMsgData );
    GFL_ASSERT( rInitParam.pWordSet );

    m_initParam = rInitParam;
  }

  LowDataPlayerView::~LowDataPlayerView()
  {
    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //------------------------------------------------------------------
  /**
    * @brief   初期化
    */
  //------------------------------------------------------------------
  bool LowDataPlayerView::InitFunc()
  {
    if( m_initSeq == 0 )
    {
      _Initialize2D( m_initParam.pLytBinData, m_initParam.pAppMsgData, m_initParam.pWordSet );

      ++m_initSeq;
    }

    if( m_initSeq == 1 )
    {
      return  true;
    }

    return  false;
  }

  //------------------------------------------------------------------
  /**
    * @brief   終了処理
    */
  //------------------------------------------------------------------
  bool LowDataPlayerView::EndFunc()
  {
    return  ( UIView::IsDrawing() == false );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  更新処理
  *         子ウインドウについても自動的にコールする(子→親の順）
  */
  //--------------------------------------------------------------------------------------------
  void LowDataPlayerView::Update()
  {
    if( m_state == STATE_IDLE )
    {
    }

    UIView::Update();
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  描画処理
  *         子ウインドウについても自動的にコールする(親→子の順）
  */
  //--------------------------------------------------------------------------------------------
  void LowDataPlayerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    if( displayNo == gfl2::gfx::CtrDisplayNo::DOWN )
    {
      app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
      for( s32 i = 0; i < LYT_WORK_MAX; ++i )
      {
        pG2DUtil->AddDrawableLytWkForOneFrame( m_initParam.pRenderManager, displayNo, i );
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    * @param[in] newStaet 設定する状態
    */
  //------------------------------------------------------------------
  void LowDataPlayerView::SetState( const State newStaet )
  {
    m_state = newStaet;

    if( m_state == STATE_IDLE )
    {
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   2D画面の作成
    * @param   pAppLytBuff レイアウトデータのバイナリポインタ
    */
  //------------------------------------------------------------------
  void LowDataPlayerView::_Initialize2D( void* pAppLytBuff, gfl2::str::MsgData* pAppMsgData, print::WordSet* pWordSet )
  {
    // アーカイブIDテーブル
    const ::app::util::G2DUtil::LytResData aResTbl[] =
    {
      {  pAppLytBuff, 16, app::util::G2DUtil::ATTACH_NONE },
    };

    //  作成するレイアウトワーク
    const ::app::util::G2DUtil::LytwkData aSetupData[ LYT_WORK_MAX ] =
    {
      {
        0,
        LYTRES_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_001_BFLYT,
        LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_LOW_001___NUM,
        pAppLytBuff,
        true,
        app::util::G2DUtil::SETUP_LOWER,
        true
      }
    };

    UIView::Create2D( m_initParam.pHeap, NULL, 8, aResTbl, GFL_NELEMS(aResTbl), aSetupData, GFL_NELEMS(aSetupData), pAppMsgData, pWordSet );
  }

GFL_NAMESPACE_END(BattleVideoPlayer)
GFL_NAMESPACE_END(NetApp)
