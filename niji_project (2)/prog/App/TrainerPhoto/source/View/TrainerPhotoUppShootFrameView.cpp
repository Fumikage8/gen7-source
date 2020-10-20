//======================================================================
/**
 * @file TrainerPhotoUppShootFrameView.h
 * @date 2015/09/02 16:23:10
 * @author uchida_yuto
 * @brief 証明写真アプリの上画面撮影フレームView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "App/TrainerPhoto/source/View/TrainerPhotoUppShootFrameView.h"

//  レイアウトリソースのインクルード
#include "niji_conv_header/app/id_photo/res2d/idPhotoLyt.h"
#include "niji_conv_header/app/id_photo/res2d/idPhotoLyt_anim_list.h"
#include "niji_conv_header/app/id_photo/res2d/idPhotoLyt_pane.h"

#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(photo)

  TrainerPhotoUppShootFrameView::TrainerPhotoUppShootFrameView( INIT_PARAM& rInitParam ) :
  UIView( rInitParam.pHeap, rInitParam.pHeap->GetDeviceAllocator(), rInitParam.pHeap->GetDeviceAllocator() )
  {
    _Clear();

    GFL_ASSERT( rInitParam.pRenderManager );
    m_initParam = rInitParam;
  }

  TrainerPhotoUppShootFrameView::~TrainerPhotoUppShootFrameView()
  {
    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //------------------------------------------------------------------
  /**
    * @brief   初期化
    */
  //------------------------------------------------------------------
  bool TrainerPhotoUppShootFrameView::InitFunc()
  {
    GFL_ASSERT( m_initParam.pLytBinData );
    GFL_ASSERT( m_initParam.pAppMsgData );
    GFL_ASSERT( m_initParam.pWordSet );

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
  bool TrainerPhotoUppShootFrameView::EndFunc()
  {
    return  ( UIView::IsDrawing() == false );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  更新処理
  *         子ウインドウについても自動的にコールする(子→親の順）
  */
  //--------------------------------------------------------------------------------------------
  void TrainerPhotoUppShootFrameView::Update()
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
  void TrainerPhotoUppShootFrameView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    if( ( displayNo == gfl2::gfx::CtrDisplayNo::LEFT )
     || ( displayNo == gfl2::gfx::CtrDisplayNo::RIGHT ) )
    {
      app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
      for( s32 i = 0; i < LYT_WORK_MAX; ++i )
      {
        pG2DUtil->AddDrawableLytWkForOneFrame( m_initParam.pRenderManager, displayNo, i, gfl2::lyt::ORDER_NORMAL, 1 );
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   シャッターイベント開始
    */
  //------------------------------------------------------------------
  void TrainerPhotoUppShootFrameView::DoEventShutter()
  {
    app::util::G2DUtil* pG2DUtil        = UIView::GetG2DUtil();
    pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_IDPHOTOLYT_ID_PHOTO_FRM_UPP_000_SHUTTER_00 );

    //  シャッター音
    Sound::PlaySE( SEQ_SE_IDPHOTO );
  }

  //------------------------------------------------------------------
  /**
    * @brief   シャッターイベント中か
    */
  //------------------------------------------------------------------
  bool TrainerPhotoUppShootFrameView::IsEventShutter()
  {
    app::util::G2DUtil* pG2DUtil        = UIView::GetG2DUtil();
    if( pG2DUtil->IsAnimeEnd( LYT_WORK_NORMAL, LA_IDPHOTOLYT_ID_PHOTO_FRM_UPP_000_SHUTTER_00 ) == true )
    {
      return  false;
    }

    return  true;
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    * @param[in] newStaet 設定する状態
    */
  //------------------------------------------------------------------
  void TrainerPhotoUppShootFrameView::SetState( const State newStaet )
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
  void TrainerPhotoUppShootFrameView::_Initialize2D( void* pAppLytBuff, gfl2::str::MsgData* pAppMsgData, print::WordSet* pWordSet )
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
        LYTRES_IDPHOTOLYT_ID_PHOTO_FRM_UPP_000_BFLYT,
        LA_IDPHOTOLYT_ID_PHOTO_FRM_UPP_000___NUM,
        pAppLytBuff,
        true,
        app::util::G2DUtil::SETUP_UPPER,
        true
      }
    };

    UIView::Create2D( m_initParam.pHeap, NULL, 16, aResTbl, GFL_NELEMS(aResTbl), aSetupData, GFL_NELEMS(aSetupData), pAppMsgData, pWordSet );
  }

GFL_NAMESPACE_END(photo)
GFL_NAMESPACE_END(app)
