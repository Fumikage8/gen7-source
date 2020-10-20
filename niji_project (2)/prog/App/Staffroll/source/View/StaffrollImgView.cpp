// ============================================================================
/*
 * @file StaffrollImgView.h
 * @brief スタッフロールアプリの１枚絵View
 * @date 2016.01.08
 * @author uchida_yuto
 */
// ============================================================================
#include "App/Staffroll/source/View/StaffrollImgView.h"
#include "App/Staffroll/source/StaffrollResourceID.h"
#include "App/Staffroll/source/System/StaffrollWork.h"

#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

//  nijiのインクルード
//#include "AppLib/include/util/app_util_GetPaneHelper.h"
//#include "AppLib/include/Tool/IconObject.h"
#include "System/include/PokemonVersion.h"

//  リソースのインクルード
#include <arc_index/message.gaix>
#include <niji_conv_header/app/staffroll/res2d/StaffrollLyt.h>
#include <niji_conv_header/app/staffroll/res2d/StaffrollLyt_anim_list.h>
#include <niji_conv_header/app/staffroll/res2d/StaffrollLyt_pane.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Staffroll)
GFL_NAMESPACE_BEGIN(View)

  const u32 StaffrollImgView::ms_aAnimeId[StaffrollImgView::LAYOUT_WORK_ID_MAX][ StaffrollImgView::ANIME_MAX ] =
  {
    {
      LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__STAFFROLL_SET_FADE_IN,
      LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__STAFFROLL_SET_BG_CHANGE_00,
      LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__STAFFROLL_SET_BG_CHANGE_01,
      LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__STAFFROLL_SET_BG_CHANGE_00,
      LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__STAFFROLL_SET_BG_CHANGE_01,
      LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__STAFFROLL_SET_BG_CHANGE_00,
      LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__STAFFROLL_SET_BG_CHANGE_01,
      LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__STAFFROLL_SET_BG_CHANGE_00,
    },
    {
      LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__BG_FADE_IN,
      LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__BG_BG_CHANGE_00,
      LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__BG_BG_CHANGE_01,
      LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__BG_BG_CHANGE_00,
      LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__BG_BG_CHANGE_01,
      LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__BG_BG_CHANGE_00,
      LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__BG_BG_CHANGE_01,
      LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__BG_BG_CHANGE_00,
    },
  };

  const StaffrollImgView::CHARA_ANIME StaffrollImgView::CharaAnimeTable[StaffrollImgView::LAYOUT_WORK_ID_MAX][StaffrollImgView::CHARA_MAX] =
  {
    { // 上画面
      {
        LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_00_CHARA_IN,
        LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_00_CHARA_OUT,
        {
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_00_CHARA_LOOP_00,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_00_CHARA_LOOP_01,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_00_CHARA_LOOP_02,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_00_CHARA_LOOP_03,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_00_CHARA_LOOP_04,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_00_CHARA_LOOP_05,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_00_CHARA_LOOP_06,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_00_CHARA_LOOP_07,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_00_CHARA_LOOP_08,
        },
      },
      {
        LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_01_CHARA_IN,
        LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_01_CHARA_OUT,
        {
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_01_CHARA_LOOP_00,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_01_CHARA_LOOP_01,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_01_CHARA_LOOP_02,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_01_CHARA_LOOP_03,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_01_CHARA_LOOP_04,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_01_CHARA_LOOP_05,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_01_CHARA_LOOP_06,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_01_CHARA_LOOP_07,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_01_CHARA_LOOP_08,
        },
      },
      {
        LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_02_CHARA_IN,
        LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_02_CHARA_OUT,
        {
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_02_CHARA_LOOP_00,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_02_CHARA_LOOP_01,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_02_CHARA_LOOP_02,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_02_CHARA_LOOP_03,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_02_CHARA_LOOP_04,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_02_CHARA_LOOP_05,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_02_CHARA_LOOP_06,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_02_CHARA_LOOP_07,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_02_CHARA_LOOP_08,
        },
      },
      {
        LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_03_CHARA_IN,
        LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_03_CHARA_OUT,
        {
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_03_CHARA_LOOP_00,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_03_CHARA_LOOP_01,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_03_CHARA_LOOP_02,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_03_CHARA_LOOP_03,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_03_CHARA_LOOP_04,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_03_CHARA_LOOP_05,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_03_CHARA_LOOP_06,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_03_CHARA_LOOP_07,
          LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__CHARA_03_CHARA_LOOP_08,
        },
      },
    },

    { // 下画面
      {
        LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_00_CHARA_IN,
        LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_00_CHARA_OUT,
        {
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_00_CHARA_LOOP_00,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_00_CHARA_LOOP_01,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_00_CHARA_LOOP_02,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_00_CHARA_LOOP_03,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_00_CHARA_LOOP_04,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_00_CHARA_LOOP_05,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_00_CHARA_LOOP_06,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_00_CHARA_LOOP_07,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_00_CHARA_LOOP_08,
        },
      },
      {
        LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_01_CHARA_IN,
        LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_01_CHARA_OUT,
        {
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_01_CHARA_LOOP_00,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_01_CHARA_LOOP_01,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_01_CHARA_LOOP_02,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_01_CHARA_LOOP_03,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_01_CHARA_LOOP_04,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_01_CHARA_LOOP_05,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_01_CHARA_LOOP_06,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_01_CHARA_LOOP_07,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_01_CHARA_LOOP_08,
        },
      },
      {
        LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_02_CHARA_IN,
        LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_02_CHARA_OUT,
        {
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_02_CHARA_LOOP_00,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_02_CHARA_LOOP_01,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_02_CHARA_LOOP_02,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_02_CHARA_LOOP_03,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_02_CHARA_LOOP_04,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_02_CHARA_LOOP_05,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_02_CHARA_LOOP_06,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_02_CHARA_LOOP_07,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_02_CHARA_LOOP_08,
        },
      },
      {
        LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_03_CHARA_IN,
        LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_03_CHARA_OUT,
        {
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_03_CHARA_LOOP_00,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_03_CHARA_LOOP_01,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_03_CHARA_LOOP_02,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_03_CHARA_LOOP_03,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_03_CHARA_LOOP_04,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_03_CHARA_LOOP_05,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_03_CHARA_LOOP_06,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_03_CHARA_LOOP_07,
          LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__CHARA_03_CHARA_LOOP_08,
        },
      },
    },
  };



  StaffrollImgView::StaffrollImgView( 
  App::Staffroll::System::StaffrollWork* pWork, 
  const App::Staffroll::StaffrollResourceID resId
  )
    : NetAppLib::System::NetApplicationViewBase( pWork->GetAppHeap() )
  {
    Initialize2D( pWork, resId );

    _Clear();

    GFL_ASSERT( pWork );
    m_pWork = pWork;

    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

    gfl2::lyt::LytWk * lytwk[LAYOUT_WORK_ID_MAX];
    for( u32 i=0; i<LAYOUT_WORK_ID_MAX; i++ )
    {
      lytwk[i] = pG2DUtil->GetLayoutWork( i );
      GFL_ASSERT( lytwk[i] );
    }


/*
    { //  画像ペイン
      static const gfl2::lyt::LytPaneIndex tbl[LAYOUT_WORK_ID_MAX][2] =
      {
        { PANENAME_STAFFROLL_BG_UPP_000_PANE_BG_BACK, PANENAME_STAFFROLL_BG_UPP_000_PANE_BG_FRONT },
        { PANENAME_STAFFROLL_BG_LOW_000_PANE_BG_BACK, PANENAME_STAFFROLL_BG_LOW_000_PANE_BG_FRONT },
      };
      for( u32 i=0; i<LAYOUT_WORK_ID_MAX; i++ )
      {
        m_pBackImgPicturePane[i] = lytwk[i]->GetPicturePane( tbl[i][0] );
        GFL_ASSERT( m_pBackImgPicturePane[i] );

        m_pFrontImgPicturePane[i] = lytwk[i]->GetPicturePane( tbl[i][1] );
        GFL_ASSERT( m_pFrontImgPicturePane[i] );
      }
    }

    //  画像ロードオブジェクト用意
    {
      app::tool::IconObject::INIT_PARAM param;
      param.arcId       = pWork->GetImgTexArcId();
      param.bCompFile   = pWork->IsImgTexFileComp();
      param.sizeHeight  = m_pBackImgPicturePane[LAYOUT_WORK_ID_UPPER]->GetSize().height;
      param.sizeWidth   = m_pBackImgPicturePane[LAYOUT_WORK_ID_UPPER]->GetSize().width;

      m_pImgObject = GFL_NEW( m_pWork->GetDeviceHeap() ) app::tool::IconObject( param, m_pWork->GetDeviceHeap() );
      //  すぐにペインを貼り付けない
      m_pImgObject->SetEnableReplaceTex( false );

      //  転送用のテクスチャーバッファを作成
      {
        u32 loopNum = GFL_NELEMS( m_pLytTexBufData );
        for( u32 i = 0; i < loopNum; ++i )
        {
          m_pLytTexBufData[ i ] = GFL_NEW( m_pWork->GetDeviceHeap() ) app::tool::IconObject( param, m_pWork->GetDeviceHeap() );
          m_pLytTexBufData[ i ]->SetEnableReplaceTex( false );
          m_pLytTexBufData[ i ]->GetTexDataCtrl()->AllocTextureBuffer();
        }        
      }
    }
*/
  }

  StaffrollImgView::~StaffrollImgView()
  {
//    GFL_SAFE_DELETE( m_pImgObject );
/*
    {
      u32 loopNum = GFL_NELEMS( m_pLytTexBufData );
      for( u32 i = 0; i < loopNum; ++i )
      {
        GFL_SAFE_DELETE( m_pLytTexBufData[ i ] );
      }        
    }
*/

    Terminate2D();
  }

  //  初期化
/*
  bool StaffrollImgView::InitFunc()
  {
    return  m_pImgObject->InitFunc();
  }
*/

  //  終了
/*
  bool StaffrollImgView::EndFunc()
  {
    //  途中で終了した時に読む
    return  m_pImgObject->EndFunc();
  }
*/

/*
  //  フェードアニメ開始
  void StaffrollImgView::BeginFadeSkip( const u32 startLabelIdx )
  {
    m_fadeAnimeLabelIdx = startLabelIdx;
  }
*/

  //  フェードアニメスキップ終了
/*
  bool StaffrollImgView::IsEndFadeSkip()
  {
    if( m_fadeAnimeLabelIdx < ANIME_MAX )
    {
      return  false;
    }
    return  true;
  }
*/

  //  次のフェードアニメ
/*
  void StaffrollImgView::NextFadeSkip()
  {
    if( ANIME_MAX <= m_fadeAnimeLabelIdx )
    {
      GFL_ASSERT( 0 );
      return;
    }
    ++m_fadeAnimeLabelIdx;
  }
*/

  //  フェードアニメ開始
/*
  void StaffrollImgView::StartFadeAnime()
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    for( u32 i=0; i<LAYOUT_WORK_ID_MAX; i++ )
    {
      pG2DUtil->StartAnime( i, ms_aAnimeId[i][m_fadeAnimeLabelIdx] ); 
      m_pBackImgPicturePane[i]->SetVisible( true ); 
      m_pFrontImgPicturePane[i]->SetVisible( true );
    }
  }
*/

  //  フェードアニメ中か
/*
  bool StaffrollImgView::IsFadeAnime()
  {
    if( ANIME_MAX <= m_fadeAnimeLabelIdx )
    {
      GFL_ASSERT( 0 );
      return  true;
    }
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
//    return  pG2DUtil->IsAnimeEnd( LAYOUT_WORK_ID_UPPER, ms_aAnimeId[LAYOUT_WORK_ID_UPPER][m_fadeAnimeLabelIdx] );
    bool result = false;
    for( u32 i=0; i<LAYOUT_WORK_ID_MAX; i++ )
    {
      if( pG2DUtil->IsAnimeEnd( i, ms_aAnimeId[i][m_fadeAnimeLabelIdx] ) != false )
      {
        result = true;
        break;
      }
    }
    return result;
  }
*/

  //  アニメ再生
/*
  void StaffrollImgView::PlayAnime( const Anime animeId )
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    for( u32 i=0; i<LAYOUT_WORK_ID_MAX; i++ )
    {
      pG2DUtil->StartAnime( i, ms_aAnimeId[i][animeId] );
    }
  }
*/

  //  アニメ再生中か
/*
  bool StaffrollImgView::IsPlayAnime( const Anime animeId )
  {
    app::util::G2DUtil* pG2DUtil        = UIView::GetG2DUtil();
    bool result = false;
//    return ( pG2DUtil->IsAnimeEnd( LAYOUT_WORK_ID_UPPER, ms_aAnimeId[ animeId ] ) == false );
    for( u32 i=0; i<LAYOUT_WORK_ID_MAX; i++ )
    {
      if( pG2DUtil->IsAnimeEnd( i, ms_aAnimeId[i][animeId] ) == false )
      {
        result = true;
        break;
      }
    }
    return result;
  }
*/

  //  画像ロード設定
/*
  void StaffrollImgView::StartTexLoadAsync( const u32 id )
  {
    bool flg = true;

    for( u32 i=0; i<LAYOUT_WORK_ID_MAX; i++ )
    {
      if( m_pBackImgPicturePane[i] == NULL )
      {
        flg = false;
      }
      else
      {
        m_pImgObject->ReplaceTex( m_pBackImgPicturePane[i], id );
        m_pBackImgPicturePane[i]->SetVisible( true );
      }
    }

    if( flg != false )
    {
      m_bFlipImg  = true;
    }
  }
*/
/*
  bool StaffrollImgView::IsTexLoaded()
  {
    return  m_pImgObject->IsReplaceTex();
  }
*/

  //  2枚のペインに画像反映してフリップ
/*
  void StaffrollImgView::FlipTexTranslate()
  {
    if( IsTexLoaded() == false )
    {
      GFL_ASSERT( 0 );
      return;
    }
    
    if( m_bFlipImg == false )
    {
      return;
    }

    //  front
    app::tool::LytTexData*  pFrontLytTexBufData = m_pLytTexBufData[ m_currentTexBufIdx ]->GetTexDataCtrl();
    if( 0 < pFrontLytTexBufData->GetTexSize() )
    {
      for( u32 i=0; i<LAYOUT_WORK_ID_MAX; i++ )
      {
        app::tool::LytTexReplaceSystem::ReplaceTextureStatic( &pFrontLytTexBufData->GetTexInfo(), m_pFrontImgPicturePane[i], 0, 0 );
      }
    }

    u32 nextTexBufIdx  = ( m_currentTexBufIdx + 1 ) % 2;
    m_currentTexBufIdx  = nextTexBufIdx;

    //  元テクスチャーを転送
    app::tool::LytTexData*  pCurrentLytTexBufData = m_pLytTexBufData[ m_currentTexBufIdx ]->GetTexDataCtrl();
    
    pCurrentLytTexBufData->CopyTextureBuffer( m_pImgObject->GetTexDataCtrl() );
    //  paneで扱えるようにテクスチャーを生成
    pCurrentLytTexBufData->LoadTexture();

    UpdateTex();

    m_bFlipImg  = false;
  }
*/

  //  テクスチャー更新
/*
  void StaffrollImgView::UpdateTex()
  {
    //  ペインに貼り付け
    //  back
    app::tool::LytTexData*  pCurrentLytTexBufData = m_pLytTexBufData[ m_currentTexBufIdx ]->GetTexDataCtrl();
    for( u32 i=0; i<LAYOUT_WORK_ID_MAX; i++ )
    {
      app::tool::LytTexReplaceSystem::ReplaceTextureStatic( &pCurrentLytTexBufData->GetTexInfo(), m_pBackImgPicturePane[i], 0, 0 );
    }
  }
*/

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  更新処理
  *         子ウインドウについても自動的にコールする(子→親の順）
  */
  //--------------------------------------------------------------------------------------------
  void StaffrollImgView::Update(void)
  {
//    m_pImgObject->UpdateFunc();
    NetAppLib::System::NetApplicationViewBase::Update();
  }

  void StaffrollImgView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    app::util::AppRenderingManager* pAppRenderingManager = m_pWork->GetAppRenderingManager();
    for( u32 i=0; i<LAYOUT_WORK_ID_MAX; i++ )
    {
      pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, i );
    }
  }



  void StaffrollImgView::Initialize2D(
        App::Staffroll::System::StaffrollWork * work,
        const App::Staffroll::StaffrollResourceID res_id )
  {
    NetAppLib::System::ResourceManager * man = work->GetResourceManager();
    void * buf = man->GetResourceBuffer( res_id );

    // リソースデータ
    const app::util::G2DUtil::LytResData res_tbl[] =
    {
      { buf, 0, app::util::G2DUtil::ATTACH_ROOT },
    };
    // レイアウトデータ
	  const app::util::G2DUtil::LytwkData data_tbl[] =
    {
      {
        0,
        LYTRES_STAFFROLLLYT_STAFFROLL_BG_UPP_000_BFLYT,
        LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000___NUM,
        buf,
        true,
        app::util::G2DUtil::SETUP_UPPER,
        true
      },
      {
        0,
        LYTRES_STAFFROLLLYT_STAFFROLL_BG_LOW_000_BFLYT,
        LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000___NUM,
        buf,
        true,
        app::util::G2DUtil::SETUP_LOWER,
        true
      },
    };

//  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
//  CreateGraphicsSystem( pAppHeap, false );

    NetAppLib::Message::MessageUtility * msg = work->GetMessageUtility();

    Create2D(
      work->GetAppHeap(),
      NULL,
      LAYOUT_WORK_ID_MAX,
      res_tbl,
      GFL_NELEMS(res_tbl),
      data_tbl,
      GFL_NELEMS(data_tbl),
      msg->GetData(GARC_message_staff_list_DAT),
      msg->GetWordSet() );
  }

  void StaffrollImgView::Terminate2D(void)
  {
    Delete2D();
//    DeleteGraphicsSystem();
  }





  u32 StaffrollImgView::GetAnimeNo(void)
  {
    return m_animeNo;
  }

  void StaffrollImgView::SetNextAnimeNo(void)
  {
    m_animeNo++;
    if( m_animeNo >= ANIME_MAX )
    {
      m_animeNo = 0;
    }
  }

  void StaffrollImgView::SetBgPatern(void)
  {
    static const u32 tbl[2][LAYOUT_WORK_ID_MAX] =
    {
      { LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__STAFFROLL_SET_BG_BACK_PTN,  LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__BG_BG_BACK_PTN },
      { LA_STAFFROLLLYT_STAFFROLL_BG_UPP_000__STAFFROLL_SET_BG_FRONT_PTN, LA_STAFFROLLLYT_STAFFROLL_BG_LOW_000__BG_BG_FRONT_PTN },
    };

    const u32 * anm = tbl[m_animeNo&1];

    app::util::G2DUtil * g2d = GetG2DUtil();

    for( u32 i=0; i<LAYOUT_WORK_ID_MAX; i++ )
    {
      g2d->StartAnimeFrame( i, anm[i], static_cast<f32>(m_animeNo), false );
    }
  }

  void StaffrollImgView::StartBgAnime(void)
  {
    app::util::G2DUtil * g2d = GetG2DUtil();
    for( u32 i=0; i<LAYOUT_WORK_ID_MAX; i++ )
    {
      g2d->StartAnime( i, ms_aAnimeId[i][m_animeNo] ); 
    }
  }

  bool StaffrollImgView::IsBgAnime(void)
  {
    app::util::G2DUtil * g2d = GetG2DUtil();

    for( u32 i=0; i<LAYOUT_WORK_ID_MAX; i++ )
    {
      if( g2d->IsAnimeEnd( i, ms_aAnimeId[i][m_animeNo] ) == false )
      {
        return false;
      }
    }
    return true;
  }


  void StaffrollImgView::StartCharaAnimeLoop(void)
  {
    app::util::G2DUtil * g2d = GetG2DUtil();
    u32 anm = m_animeNo;

    if( anm == 0 )
    {
      if( ::System::GetVersion() == VERSION_MOON2 )
      {
        anm = CHARA_ANIME_LOOP_MAX - 1;
      }
    }

    for( u32 i=0; i<LAYOUT_WORK_ID_MAX; i++ )
    {
      for( u32 j=0; j<CHARA_MAX; j++ )
      {
        for( u32 k=0; k<CHARA_ANIME_LOOP_MAX; k++ )
        {
          if( k == anm )
          {
            g2d->StartAnime( i, CharaAnimeTable[i][j].loop[k] );
          }
          else
          {
            g2d->StopAnime( i, CharaAnimeTable[i][j].loop[k] );
          }
        }
      }
    }
  }

  void StaffrollImgView::StartCharaAnimeOut(void)
  {
    app::util::G2DUtil * g2d = GetG2DUtil();

    for( u32 i=0; i<LAYOUT_WORK_ID_MAX; i++ )
    {
      for( u32 j=0; j<CHARA_MAX; j++ )
      {
        g2d->StartAnime( i, CharaAnimeTable[i][j].out );
      }
    }
  }

  bool StaffrollImgView::IsCharaAnimeOut(void)
  {
    app::util::G2DUtil * g2d = GetG2DUtil();

    for( u32 i=0; i<LAYOUT_WORK_ID_MAX; i++ )
    {
      for( u32 j=0; j<CHARA_MAX; j++ )
      {
        if( g2d->IsAnimeEnd( i, CharaAnimeTable[i][j].out ) == false )
        {
          return false;
        }
      }
    }
    return true;
  }

  void StaffrollImgView::StartCharaAnimeIn(void)
  {
    app::util::G2DUtil * g2d = GetG2DUtil();

    for( u32 i=0; i<LAYOUT_WORK_ID_MAX; i++ )
    {
      for( u32 j=0; j<CHARA_MAX; j++ )
      {
        g2d->StartAnime( i, CharaAnimeTable[i][j].in );
      }
    }
  }

  bool StaffrollImgView::IsCharaAnimeIn(void)
  {
    app::util::G2DUtil * g2d = GetG2DUtil();

    for( u32 i=0; i<LAYOUT_WORK_ID_MAX; i++ )
    {
      for( u32 j=0; j<CHARA_MAX; j++ )
      {
        if( g2d->IsAnimeEnd( i, CharaAnimeTable[i][j].in ) == false )
        {
          return false;
        }
      }
    }
    return true;
  }

  



GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(Staffroll)
GFL_NAMESPACE_END(App)
