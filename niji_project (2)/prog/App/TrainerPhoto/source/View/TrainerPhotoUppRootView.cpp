//======================================================================
/**
 * @file TrainerPhotoUppRootView.h
 * @date 2015/09/02 16:23:10
 * @author uchida_yuto
 * @brief 証明写真アプリの上画面ルートView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "App/TrainerPhoto/source/View/TrainerPhotoUppRootView.h"

//  レイアウトリソースのインクルード
#include "niji_conv_header/app/id_photo/res2d/idPhotoLyt.h"
#include "niji_conv_header/app/id_photo/res2d/idPhotoLyt_anim_list.h"
#include "niji_conv_header/app/id_photo/res2d/idPhotoLyt_pane.h"

#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(photo)

  const u32 TrainerPhotoUppRootView::ms_aAnimeId[ ANIME_MAX ]  =
  {
    LA_IDPHOTOLYT_ID_PHOTO_MIN_UPP_000_PHOTO_00,
    LA_IDPHOTOLYT_ID_PHOTO_MIN_UPP_000_SHUTTER_00,
  };

  TrainerPhotoUppRootView::TrainerPhotoUppRootView( INIT_PARAM& rInitParam ) :
  UIView( rInitParam.pHeap, rInitParam.pHeap->GetDeviceAllocator(), rInitParam.pHeap->GetDeviceAllocator() )
  {
    _Clear();

    GFL_ASSERT( rInitParam.pRenderManager );
    GFL_ASSERT( rInitParam.pLytBinData );
    GFL_ASSERT( rInitParam.pAppMsgData );
    GFL_ASSERT( rInitParam.pWordSet );

    m_initParam = rInitParam;
  }

  TrainerPhotoUppRootView::~TrainerPhotoUppRootView()
  {
    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //------------------------------------------------------------------
  /**
    * @brief   初期化
    */
  //------------------------------------------------------------------
  bool TrainerPhotoUppRootView::InitFunc()
  {
    if( m_initSeq == 0 )
    {
      _Initialize2D( m_initParam.pLytBinData, m_initParam.pAppMsgData, m_initParam.pWordSet );

      {
        gfl2::lyt::LytWk* pLayoutWork         = UIView::GetLayoutWork( LYT_WORK_NORMAL );

        m_pTrainerIconRootPane    = pLayoutWork->GetPane( PANENAME_ID_PHOTO_MIN_UPP_000_PANE_ROOT );
        GFL_ASSERT( m_pTrainerIconRootPane );

        m_pTrainerIconPicturePane = pLayoutWork->GetPicturePane( PANENAME_ID_PHOTO_MIN_UPP_000_PANE_PLAYERPHOTO );
        GFL_ASSERT( m_pTrainerIconPicturePane );
      }

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
  bool TrainerPhotoUppRootView::EndFunc()
  {
    return  ( UIView::IsDrawing() == false );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  更新処理
  *         子ウインドウについても自動的にコールする(子→親の順）
  */
  //--------------------------------------------------------------------------------------------
  void TrainerPhotoUppRootView::Update()
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
  void TrainerPhotoUppRootView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    if( ( displayNo == gfl2::gfx::CtrDisplayNo::LEFT )
     || ( displayNo == gfl2::gfx::CtrDisplayNo::RIGHT ) )
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
  void TrainerPhotoUppRootView::SetState( const State newStaet )
  {
    m_state = newStaet;

    if( m_state == STATE_IDLE )
    {
    }
  }

  //  アニメアニメ再生
  void TrainerPhotoUppRootView::PlayAnime( const Anime animeId )
  {
    app::util::G2DUtil* pG2DUtil        = UIView::GetG2DUtil();
    if( animeId == ANIME_ICON_IN )
    {
      Sound::PlaySE( SEQ_SE_SYS_NJ_018 );
    }

    pG2DUtil->StartAnime( LYT_WORK_NORMAL, ms_aAnimeId[ animeId ] );
  }

  //  アニメ再生中か
  bool TrainerPhotoUppRootView::IsPlayAnime( const Anime animeId )
  {
    app::util::G2DUtil* pG2DUtil        = UIView::GetG2DUtil();
    return  ( pG2DUtil->IsAnimeEnd( LYT_WORK_NORMAL, ms_aAnimeId[ animeId ] ) == false );
  }

  //------------------------------------------------------------------
  /**
    * @brief   2D画面の作成
    * @param   pAppLytBuff レイアウトデータのバイナリポインタ
    */
  //------------------------------------------------------------------
  void TrainerPhotoUppRootView::_Initialize2D( void* pAppLytBuff, gfl2::str::MsgData* pAppMsgData, print::WordSet* pWordSet )
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
        LYTRES_IDPHOTOLYT_ID_PHOTO_MIN_UPP_000_BFLYT,
        LA_IDPHOTOLYT_ID_PHOTO_MIN_UPP_000___NUM,
        pAppLytBuff,
        true,
        app::util::G2DUtil::SETUP_UPPER,
        true
      }
    };

    UIView::Create2D( m_initParam.pHeap, NULL, 8, aResTbl, GFL_NELEMS(aResTbl), aSetupData, GFL_NELEMS(aSetupData), pAppMsgData, pWordSet );
  }

GFL_NAMESPACE_END(photo)
GFL_NAMESPACE_END(app)
