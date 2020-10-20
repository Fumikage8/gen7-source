// ============================================================================
/*
 * @file EndRollTheEndView.h
 * @brief エンドロールアプリのTheEnd表示
 * @date 2016.01.06
 * @author uchida_yuto
 */
// ============================================================================
#include "App/EndRoll/source/View/EndRollTheEndView.h"
#include "App/EndRoll/source/EndrollResourceID.h"
#include "App/EndRoll/source/System/EndrollWork.h"

#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

//  nijiのインクルード
#include "AppLib/include/util/app_util_GetPaneHelper.h"

//  リソースのインクルード
#include <arc_index/message.gaix>
#include <arc_index/EndrollLyt.gaix>
#include <niji_conv_header/app/endroll/res2d/EndrollLyt.h>
#include <niji_conv_header/app/endroll/res2d/EndrollLyt_anim_list.h>
#include <niji_conv_header/app/endroll/res2d/EndrollLyt_pane.h>
#include <niji_conv_header/app/common/graphic_font/endroll_grpfont/res2d/EndrollGrpFont.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(EndRoll)
GFL_NAMESPACE_BEGIN(View)

  const u32 EndRollTheEndView::ms_aAnimeId[ EndRollTheEndView::ANIME_MAX ] =
  {
    LA_ENDROLLLYT_ENDROLL_THE_END_UPP_000_IN,
    LA_ENDROLLLYT_ENDROLL_THE_END_UPP_000_LOOP,
    LA_ENDROLLLYT_ENDROLL_THE_END_UPP_000_IMG,
    LA_ENDROLLLYT_ENDROLL_THE_END_UPP_000_OUT,
  };

  EndRollTheEndView::EndRollTheEndView( 
  App::EndRoll::System::EndRollWork* pWork, 
  const App::EndRoll::EndRollResourceID resId 
  )
  : NetAppLib::System::NetApplicationViewBase( pWork->GetAppHeap() )
  {
    _Clear();

    //  手動でレイアウトを作成
    _Create2DOneLayout(
      pWork,
      resId,
      LYTRES_ENDROLLLYT_ENDROLL_THE_END_UPP_000_BFLYT,
      LA_ENDROLLLYT_ENDROLL_THE_END_UPP_000___NUM,
      app::util::G2DUtil::SETUP_UPPER,
      pWork->GetMessageUtility(),
      gfl2::fs::ArcFile::ARCDATID_NULL
    );

    GFL_ASSERT( pWork );
    m_pWork = pWork;

    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

    gfl2::lyt::LytWk* pLayoutWork = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID );
    GFL_ASSERT( pLayoutWork );
  }

  EndRollTheEndView::~EndRollTheEndView()
  {
  }

  //  一枚絵表示設定
  void EndRollTheEndView::SetImg( poke_3d::model::DressUpParam dressUpParam )
  {
    struct BODY_CATEGORY_DATA
    {
      u16 sex;
      u16 skin;
      u32 animeIdx;
    };

    // ボディカテゴリ
    const BODY_CATEGORY_DATA c_aBodyCategory[] =
    {
      { // 男１(ホワイト)
        poke_3d::model::DressUpParam::SEX_MALE,
        DRESSUP_COLOR_ID_SKIN_SWH,
        0,
      },

      { // 男２（イエロー）
        poke_3d::model::DressUpParam::SEX_MALE,
        DRESSUP_COLOR_ID_SKIN_SYE,
        1,
      },

      { // 男３（ブラウン）
        poke_3d::model::DressUpParam::SEX_MALE,
        DRESSUP_COLOR_ID_SKIN_SBR,
        2,
      },

      { // 男４（ダークブラウン）
        poke_3d::model::DressUpParam::SEX_MALE,
        DRESSUP_COLOR_ID_SKIN_SDB,
        3,
      },

      { // 女１(ホワイト)
        poke_3d::model::DressUpParam::SEX_FEMALE,
        DRESSUP_COLOR_ID_SKIN_SWH,
        4,
      },

      { // 女２（イエロー）
        poke_3d::model::DressUpParam::SEX_FEMALE,
        DRESSUP_COLOR_ID_SKIN_SYE,
        5,
      },

      { // 女３（ブラウン）
        poke_3d::model::DressUpParam::SEX_FEMALE,
        DRESSUP_COLOR_ID_SKIN_SBR,
        6,
      },

      { // 女４（ダークブラウン）
        poke_3d::model::DressUpParam::SEX_FEMALE,
        DRESSUP_COLOR_ID_SKIN_SDB,
        7,
       },
    };

    u32 loopNum = GFL_NELEMS( c_aBodyCategory );
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( c_aBodyCategory[ i ].skin == dressUpParam.bodyParams[ poke_3d::model::DressUpParam::BODY_CATEGORY_BODY_COLOR ] )
      {
        if( c_aBodyCategory[ i ].sex == dressUpParam.bodyParams[ poke_3d::model::DressUpParam::BODY_CATEGORY_SEX ] )
        {
          app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
          pG2DUtil->StartAnimeFrame( LAYOUT_WORK_ID, LA_ENDROLLLYT_ENDROLL_THE_END_UPP_000_IMG, c_aBodyCategory[ i ].animeIdx );

          //  アニメで切り替えて停止
          pG2DUtil->PauseAnime( LAYOUT_WORK_ID, LA_ENDROLLLYT_ENDROLL_THE_END_UPP_000_IMG, false );

          //  即時反映のために更新を呼ぶ
          UIView::Update();

          break;
        }
      }       
    }
  }

  //------------------------------------------------------------------
  /**
    *  @func     Create2DOneLayout
    *  @brief    ひとつのLayoutリソースに特化するかわりに必要なパラメータを減らしたCreate2D簡易版
    *  @note     グラフィックフォントを使っているので専用でレイアウト作成が必要
    */
  //------------------------------------------------------------------
  void  EndRollTheEndView::_Create2DOneLayout(
    NetAppLib::System::ApplicationWorkBase* pWork,
    NetAppLib::System::ResourceID           layoutResourceID,
    gfl2::lyt::LytArcIndex                  layoutArcIndex,
    u32                                     animationMax,
    app::util::G2DUtil::SETUP_DISPLAY_TYPE  setupDisplayType,
    NetAppLib::Message::MessageUtility*     pMessageUtility,
    gfl2::fs::ArcFile::ARCDATID             messageArcDatID
  )
  {
    NetAppLib::System::ResourceManager* pResourceManager = pWork->GetResourceManager();
    void* pLayoutBuffer   = pResourceManager->GetResourceBuffer( layoutResourceID );
    void* pGrpFontBuffer  = pResourceManager->GetResourceBuffer( APPENDROLL_RESOURCE_ID_GRPFONT_LAYOUT );
    app::util::Heap* pAppHeap = pWork->GetAppHeap();

    SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
    CreateGraphicsSystem( pAppHeap, false );

    const app::util::G2DUtil::LytResData resTbl[] =
    {
      // res_0
      {
        pLayoutBuffer,
        LYTRES_ENDROLLGRPFONT_END,
        app::util::G2DUtil::ATTACH_ROOT
      },

      // res_1
      {
        pGrpFontBuffer,
        0,
        app::util::G2DUtil::ATTACH_RESOURCE
      },
    };
  
    m_LayoutWorkData.res_index            = 0;
    m_LayoutWorkData.lytres_id            = layoutArcIndex;
    m_LayoutWorkData.anm_max              = animationMax;
    m_LayoutWorkData.buf                  = pLayoutBuffer;
    m_LayoutWorkData.calculateMtxFlag     = true;
    m_LayoutWorkData.setupDisp            = setupDisplayType;
    m_LayoutWorkData.enable               = true;

    if( messageArcDatID != gfl2::fs::ArcFile::ARCDATID_NULL )
    {
      Create2D( pAppHeap, NULL, 1,
        resTbl, GFL_NELEMS( resTbl ),
        &m_LayoutWorkData, 1,
        pMessageUtility->GetData( messageArcDatID ), pMessageUtility->GetWordSet() );
    }
    else
    {
      Create2D( pAppHeap, NULL, 1,
        resTbl, GFL_NELEMS( resTbl ),
        &m_LayoutWorkData, 1,
        UNUSE_MESSAGE_DATA );
    }
  }

  //  アニメ再生
  void EndRollTheEndView::PlayAnime( const Anime animeId )
  {
    app::util::G2DUtil* pG2DUtil        = UIView::GetG2DUtil();
    pG2DUtil->StartAnime( LAYOUT_WORK_ID, ms_aAnimeId[ animeId ] );
  }

  //  アニメ再生フレーム最大数を取得
  u32 EndRollTheEndView::GetPlayAnimeLastFrame( const Anime animeId )
  {
    app::util::G2DUtil* pG2DUtil        = UIView::GetG2DUtil();
    return  pG2DUtil->GetAnimeMaxFrame( LAYOUT_WORK_ID, ms_aAnimeId[ animeId ] );
  }

  //  アニメ再生中か
  bool EndRollTheEndView::IsPlayAnime( const Anime animeId )
  {
    app::util::G2DUtil* pG2DUtil        = UIView::GetG2DUtil();
    return  ( pG2DUtil->IsAnimeEnd( LAYOUT_WORK_ID, ms_aAnimeId[ animeId ] ) == false );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  更新処理
  *         子ウインドウについても自動的にコールする(子→親の順）
  */
  //--------------------------------------------------------------------------------------------
  void EndRollTheEndView::Update(void)
  {
    NetAppLib::System::NetApplicationViewBase::Update();
  }

  void EndRollTheEndView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    app::util::AppRenderingManager* pAppRenderingManager = m_pWork->GetAppRenderingManager();
    pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID );
  }

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(EndRoll)
GFL_NAMESPACE_END(App)

