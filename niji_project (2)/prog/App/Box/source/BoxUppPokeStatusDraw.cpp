//======================================================================
/**
 * @file BoxUppPokeStatusDraw.cpp
 * @date 2015/07/28 12:47:28
 * @author uchida_yuto
 * @brief Box上画面のポケモンステータス表示View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "App/Box/source/BoxUppPokeStatusDraw.h"

//  nijiのインクルード
#include "PokeTool/include/PokeModelSystem.h"
#include "model/include/gfl2_BaseCamera.h"
#include "App/RaderChart/include/app_tool_BoxRaderChart.h"
#include "AppLib/include/Util/AppRenderingManager.h"
#include "AppLib/include/Tool/AppToolPokeSimpleModelInFrame.h"
#include "AppLib/include/Tool/AppCommonGrpIconData.h"
#include "AppLib/include/Tool/AppToolItemIcon.h"
#include "AppLib/include/Util/app_util_Common.h"
#include "PokeTool/include/PokeTool.h"
#include "PokeTool/include/PokeToolPokePlace.h"
#include "Savedata/include/ZukanSave.h"
#include "Print/include/WordSetLoader.h"

//  素材のインクルード
#include "niji_conv_header/app/box/res2d/staupp.h"
#include "niji_conv_header/app/box/res2d/staupp_anim_list.h"
#include "niji_conv_header/app/box/res2d/staupp_pane.h"

//  テキストのインクルード
#include "niji_conv_header/message/msg_box.h"

//  リソースのインクルード
// 動的ヘッダシステムを利用するときは以下のヘッダをインクルードする
#include <debug/include/DynamicHeader/gfl2_DebugDynamicHeader.h>

//  アプリ全体で使う動的パラメータ
#define NIJI_DEBUG_BOX_UPP_POKE_STATUS_DRAW_HEADER_FILEPATH "niji_conv_header/app/box/dynamicHeader/BoxUppPokeStatusDrawDynamic.h"

// 動的ヘッダシステムで利用するヘッダは以下のマクロを通してincludeしてください
#include GFL_DYNAMIC_HEADER(NIJI_DEBUG_BOX_UPP_POKE_STATUS_DRAW_HEADER_FILEPATH)

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

GFL_NAMESPACE_BEGIN( StatusLocalData )
  
  struct TALENT_EVALUATION_TABLE
  {
    //  評価テキストid
    u32 textId;
    //  評価値の下限値
    u32 minNum;
    //  評価値の上限値
    u32 maxNum;
  };

  //  固体値の評価テーブルデータ
  static const TALENT_EVALUATION_TABLE  s_aPersonalEvaluationTableData[] =
  {
    { msg_judge_01_01, 0,  0 },
    { msg_judge_01_02, 1,  15 },
    { msg_judge_01_03, 16, 25 },
    { msg_judge_01_04, 26, 29 },
    { msg_judge_01_05, 30, 30 },
    { msg_judge_01_06, 31, 999 }
  };

  //  固体値総合の評価テーブルデータ
  static const TALENT_EVALUATION_TABLE  s_aAllNumPersonalEvaluationTableData[] =
  {
    { msg_judge_02_01, 0,   90 },
    { msg_judge_02_02, 91,  120 },
    { msg_judge_02_03, 121, 150 },
    { msg_judge_02_04, 151, 999 }
  };

GFL_NAMESPACE_END( StatusLocalData )

  BoxUppPokeStatusDraw::~BoxUppPokeStatusDraw()
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
  bool BoxUppPokeStatusDraw::InitFunc()
  {
    if( m_bInit == true )
    {
      return  true;
    }

    enum
    {
      SEQ_LOAD_START,
      SEQ_LOAD_WAIT,
      SEQ_INIT,
      SEQ_WAIT_LOAD
    };

    if( m_initSeq == SEQ_LOAD_START )
    {
      //  特性や性格などの明記があるので専用テキストを作成する
      u32 readBit = PRELOAD_ITEMNAME | PRELOAD_WAZANAME | PRELOAD_TOKUSEI | PRELOAD_SEIKAKU;
      m_pWordSetLoader  = GFL_NEW( m_initParam.pHeap->GetDeviceHeap() ) WordSetLoader( m_initParam.pHeap->GetDeviceHeap(), readBit );

      m_pWordSetLoader->StartLoad( m_initParam.pHeap->GetDeviceHeap()->GetLowerHandle() );

      ++m_initSeq;
    }

    if( m_initSeq == SEQ_LOAD_WAIT )
    {
      // ヘッダファイルのロード
      // ファイルパスと同時にテーブルのキャパシティ(最大定義数)も指定する必要がある
      GFL_LOAD_HEADER_HASH(NIJI_DEBUG_BOX_UPP_POKE_STATUS_DRAW_HEADER_FILEPATH, 64);

      //  ワードローダのロード終了チャック
      if( m_pWordSetLoader->WaitLoad() == true )
      {
        ++m_initSeq;
      }
    }

    if( m_initSeq == SEQ_INIT )
    {
      //  レイアウトに必要なWordSetを作成
      {
        m_pWordSet  = GFL_NEW( m_initParam.pHeap->GetDeviceHeap() ) print::WordSet( m_initParam.pHeap->GetDeviceHeap() );

        m_pWordSet->SetWordSetLoader( m_pWordSetLoader );
      }

      //  2D画面を生成
      {
        _Init2D();
      }

      //  モンスターボールアイコン画像のアイテムアイコン管理
      {
        app::tool::ItemIconToolEx::ItemIconToolParam  initParam;
        initParam.layout_id = LYT_WORK_NORMAL;
        initParam.pHeap     = m_initParam.pHeap;
        initParam.pUIView   = this;
        initParam.res_id    = 0;
        initParam.iconMax   = 1;
        initParam.sync      = false;

        //  ロードは非同期で使用
        m_pItemIconTool = GFL_NEW( m_initParam.pHeap->GetDeviceHeap() ) app::tool::ItemIconToolEx( initParam );
      }

      //  ポケモンモデル管理の初期化
      {
        //  ポケモンモデルシステム作成
        m_pPokeModelSys = GFL_NEW( m_initParam.pHeap->GetDeviceHeap() ) PokeTool::PokeModelSystem();
        GFL_ASSERT( m_pPokeModelSys );

        //  ポケモンモデルシステム初期化非同期
        m_pPokeModelSys->InitSystemAsync( 
          m_initParam.pHeap->GetDeviceHeap(),
          m_initParam.pHeap->GetDeviceHeap(),
          GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager(), 1 );
      }

      ++m_initSeq;
    }

    if( m_initSeq == SEQ_WAIT_LOAD )
    {
      bool  bNextSeq  = true;

      //  ポケモンモデル管理ロード待ち
      if( m_pPokeModelSys->IsFinishInitSystemAsync() == false )
      {
        bNextSeq  = false;
      }

      //  グラフィックフォントアイコンがデータが使えるかどうか
      if( m_initParam.pAppCommonGrapIconData->IsReady() == false )
      {
        bNextSeq  = false;
      }

      //  ジャッジ画面の準備ができているか
      if( m_judgeDraw.IsReady() == false )
      {
        bNextSeq  = false;
      }

      if( bNextSeq == true )
      {
        //  ポケモンモデル用のヒープ作成
        {
          m_pPokeModelSys->CreateSystem( false, NULL );

          //ポケモン用ヒープ作成
          PokeTool::PokeModelSystem::HeapOption opt;
          opt.animeType       = PokeTool::MODEL_ANIMETYPE_APP;
          opt.useIdModel      = false;
          opt.useShadow       = false;
          opt.useColorShader  = false;
          m_pPokeModelSys->CreatePokeModelHeap( m_initParam.pHeap->GetDeviceHeap(), &opt );
        }

        //  ポケモンモデル生成
        {
          //  ポケモンモデル作成
          {
            app::tool::PokeSimpleModelInFrame::INIT_PARAM  initParam;

            //  必要なパラメータを設定
            {
              PokeTool::PokeModel::SetupOption* pOption = &initParam.setupOpt;
              pOption->dataHeap = m_initParam.pHeap->GetDeviceHeap();
              pOption->workHeap = m_initParam.pHeap->GetDeviceHeap()->GetLowerHandle();
              // @fix GFNMCat[3562] ゼルネアス、ホワイトキュレム、ブラックキュレムの場合はモーションをかわいがりに
              pOption->animeType  = PokeTool::MODEL_ANIMETYPE_APP; //アプリ用ID
            }

            initParam.pPokeModelSys       = m_pPokeModelSys;
            initParam.pRenderingManager   = m_initParam.pRenderinManager;
            initParam.renderPlace         = app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY;
            initParam.layerNo             = m_initParam.pokeInFrameLayerNo;

            //  フレームのライトをレンダラーに追加
            {
              m_pPokeInFrameDrawEnvNode = app::util::AppRenderingManager::CreateDefaultDrawEnv( m_initParam.pHeap );
              GFL_ASSERT( m_pPokeInFrameDrawEnvNode );
              app::tool::PokeSimpleModelInFrame::SetLightParamForInFrame( m_pPokeInFrameDrawEnvNode->GetLightSet( 0 )->GetLightNode( 0 ), System::Camera::CModelViewerInFrame::TURN_DIRECTION_LEFT );
              m_initParam.pRenderinManager->AddDrawEnv( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_pPokeInFrameDrawEnvNode, initParam.layerNo );
            }

            m_pPokeSimpleModelInFrame  = GFL_NEW( m_initParam.pHeap->GetDeviceHeap() ) app::tool::PokeSimpleModelInFrame( initParam );
            m_pPokeSimpleModelInFrame->InitInFrame( System::Camera::CModelInFrame::SCREEN_UPPER, System::Camera::CModelViewerInFrame::TURN_DIRECTION_LEFT );

            //  フレーム枠の設定
            {
              m_pPokeSimpleModelInFrame->SetInFrameFrame( 
              GFL_VALUE_DEFINE_IN_HEADER( BOX_UPP_POKE_STATUS_DRAW_DYANMIC_IN_FRAME_LEFT ), 
              GFL_VALUE_DEFINE_IN_HEADER( BOX_UPP_POKE_STATUS_DRAW_DYANMIC_IN_FRAME_TOP ), 
              GFL_VALUE_DEFINE_IN_HEADER( BOX_UPP_POKE_STATUS_DRAW_DYANMIC_IN_FRAME_RIGHT ), 
              GFL_VALUE_DEFINE_IN_HEADER( BOX_UPP_POKE_STATUS_DRAW_DYANMIC_IN_FRAME_BOTTOM ) );

              gfl2::math::Vector3 pos( 0, 0, 0 );
              m_pPokeSimpleModelInFrame->SetPos( pos );
            }

            //  アニメ設定は待機用に
            m_pPokeSimpleModelInFrame->SetAnime( PokeTool::MODEL_ANIME_BT_WAIT_A, true );
            m_pPokeSimpleModelInFrame->SetVisible( false );
          }
        }

        m_initSeq   = 0;
        m_endSeq    = 0;
        m_bInit     = true;

        return  true;
      }
    }

    return  false;
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
  bool BoxUppPokeStatusDraw::EndFunc()
  {
    if( m_bInit == false )
    {
      return  true;
    }

    enum
    {
      SEQ_ICON_CLOSE  = 0,
      SEQ_POKE_MODE_DELETE,
      SEQ_POKE_MODE_DELETE_WAIT,
      SEQ_SYS_POKE_DELETE,
      SEQ_SYS_POKE_DELETE_WAIT,
      SEQ_END
    };

    m_pItemIconTool->Update();

    if( m_endSeq == SEQ_ICON_CLOSE )
    {
      if( m_pItemIconTool->EndFunc() == true )
      {
        ++m_endSeq;
      }
    }

    if( m_endSeq == SEQ_POKE_MODE_DELETE )
    {
      m_judgeDraw.Terminate();
      m_pPokeSimpleModelInFrame->Terminate();
      m_pWordSetLoader->Release();

      ++m_endSeq;
    }

    if( m_endSeq == SEQ_POKE_MODE_DELETE_WAIT )
    {
      bool  bNextSeq  = true;

      //  ポケモンモデルが終了できるかチェック
      if( m_pPokeSimpleModelInFrame->IsDelete() == false )
      {
        bNextSeq  = false;
      }

      if( m_judgeDraw.IsDelete() == false )
      {
        bNextSeq  = false;
      }

      if( bNextSeq == true )
      {
        ++m_endSeq;
      }
    }

    if( m_endSeq == SEQ_SYS_POKE_DELETE )
    {
      //  ポケモンモデルシステムの後始末
      {
        m_initParam.pRenderinManager->RemoveDrawEnv( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_pPokeInFrameDrawEnvNode, m_initParam.pokeInFrameLayerNo );
        GFL_SAFE_DELETE( m_pPokeInFrameDrawEnvNode );

        //ポケモン用ヒープ削除
        m_pPokeModelSys->DeletePokeModelHeap();
        m_pPokeModelSys->TermSystemAsync();
      }

      ++m_endSeq;
    }

    if( m_endSeq == SEQ_SYS_POKE_DELETE_WAIT )
    {
      if( m_pPokeModelSys->IsFinishTermSystemAsync() == true )
      {
        ++m_endSeq;
      }
    }

    if( m_endSeq == SEQ_END )
    {
      GFL_UNLOAD_HEADER_HASH(NIJI_DEBUG_BOX_UPP_POKE_STATUS_DRAW_HEADER_FILEPATH);

      UIView::Delete2D();

      GFL_SAFE_DELETE( m_pPokeSimpleModelInFrame );
      GFL_SAFE_DELETE( m_pPokeModelSys );
      GFL_SAFE_DELETE( m_pWordSet );
      GFL_SAFE_DELETE( m_pItemIconTool );
      GFL_SAFE_DELETE( m_pWordSetLoader );
      
      m_endSeq   = 0;
      m_initSeq  = 0;
      m_bInit    = false;

      return true;
    }

    return false;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  画面を開く
  */
  //--------------------------------------------------------------------------------------------
  void BoxUppPokeStatusDraw::Open()
  {
    GFL_ASSERT( m_bInit == true );

    UIView::SetVisible( true );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  画面を閉じる
  */
  //--------------------------------------------------------------------------------------------
  void BoxUppPokeStatusDraw::Close()
  {
    GFL_ASSERT( m_bInit == true );

    UIView::SetVisible( false );

    m_pPokeSimpleModelInFrame->SetVisible( UIView::IsVisible() );
    // @fix GFNMCat[1117] レーダーチャートを個別表示設定する。モデルと同じでViewで設定できない
    m_judgeDraw.SetVisibleReaderChart( UIView::IsVisible() );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      描画モード変更
  * @param[in]  mode: 変更した描画モードを設定
  */
  //--------------------------------------------------------------------------------------------
  void BoxUppPokeStatusDraw::SetDrawMode( const DrawMode mode )
  {
    GFL_ASSERT( m_bInit == true );

    m_normalDraw.SetVisible( false );
    m_judgeDraw.SetVisible( false );

    //  タマゴ表示モードであればステータスとジャッジの表示設定はしない
    //  これがないとタマゴパラメータなのにステータスやジャッジが表示されるので、それを防ぐために追加
    if( m_bEggDraw == false )
    {
      if( mode == DRAW_MODE_STATUS )
      {
        m_normalDraw.SetVisible( true );
      }
      else
      if( mode == DRAW_MODE_JUDDGE )
      {
        m_judgeDraw.SetVisible( true );
      }
    }

    m_nowDrawMode = mode;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      ステータスパラメータを指定
  * @param[in]  pPokeParam: 参照するポケモンパラメータを指定
  * @parma[in]  bLock:        ロック中
  * @param[in]  bRegistTeam:  チーム登録中
  * @note
  *   この関数が呼ばれた段階で表示するポケモンモデルのロードをする
  */
  //--------------------------------------------------------------------------------------------
  void BoxUppPokeStatusDraw::SetParam( const pml::pokepara::PokemonParam& rPokeParam, const bool bLock, const bool bRegistTeam )
  {
    GFL_ASSERT( m_bInit == true );

    app::util::G2DUtil* pG2DUtil            = UIView::GetG2DUtil();

    //  ロックアイコン表示設定
    if( bLock == true )
    {
      pG2DUtil->SetPaneVisible( m_pLockIconPane, true );
    }
    else
    {
      pG2DUtil->SetPaneVisible( m_pLockIconPane, false );
    }

    //  チーム登録アイコン表示設定
    if( bRegistTeam == true )
    {
      pG2DUtil->SetPaneVisible( m_pRegistTeamIconPane, true );
    }
    else
    {
      pG2DUtil->SetPaneVisible( m_pRegistTeamIconPane, false );
    }


    //  パラメータアクセスを高速モードに
    bool  bParamFastMode  = rPokeParam.StartFastMode();

    //  ポケモンモデルロード
    PokeTool::SimpleParam param;
    {
      param.formNo  = rPokeParam.GetFormNo();
      param.isEgg   = rPokeParam.IsEgg( pml::pokepara::CHECK_BOTH_EGG );
      param.monsNo  = rPokeParam.GetMonsNo();
      param.perRand = rPokeParam.GetPersonalRnd();
      param.sex     = rPokeParam.GetSex();
      param.isRare  = rPokeParam.IsRare();

      m_pPokeSimpleModelInFrame->StartLoad( param );
    }

    //  共通画面の更新
    {
      // @fix GFNMCat[2855] １フレーム内で何度も呼び出しても問題ないように画面切り替えアニメはすべて停止させる
      pG2DUtil->StopAnime( LYT_WORK_NORMAL, LA_STAUPP_BOX_STA_UPP_000_PATERN_EGG );
      if( param.isEgg == true )
      {
        //  卵の時は専用の表示をする
        pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_STAUPP_BOX_STA_UPP_000_PATERN_EGG );

        m_normalDraw.SetVisible( false );
        m_judgeDraw.SetVisible( false );

        //  タマゴ描画モードかフラグを立てる。
        //  画面表示設定でステータスとジャッジの表示をスキップするために必要
        m_bEggDraw  = true;
      }
      else
      {
        m_bEggDraw  = false;

        //  スタータス画面の更新
        m_normalDraw.SetParam( rPokeParam );

        //  ジャッジ画面の更新
        m_judgeDraw.SetParam( rPokeParam );

        //  各モードに合わせた表示をする
        SetDrawMode( m_nowDrawMode );
      }

      //  @fix GFNMCat[875] タマゴの時はモンスターボール表示処理が抜けていたのを入れた
      //  モンスターボールアイコン画像を切り替え
      {
        item::BALL_ID haveBallId  = (item::BALL_ID)rPokeParam.GetGetBall();        
        if( haveBallId == item::BALLID_NULL )
        {
          // @fix GFNMCat[4245] 捕獲モンスターボールがない場合はモンスターボールアイコンはデフォルト表示する
          haveBallId  = item::BALLID_MONSUTAABOORU; 
        }

        m_changeBallId = haveBallId;
        
        if( m_nowBallId == m_changeBallId )
        {
          //  同じボールはすでにロード済みなので切り替えはしない
          m_changeBallId  = item::BALLID_NULL;
        }
        else
        {
          m_changeBallSeq = 0;
        }
      }

      //  ニック名設定
      {
        // @fix GFNMCat[1169] タマゴの時はニックネームに名前が入っているので、そのまま使う
        pG2DUtil->SetRegisterPokeNickName( 0, &rPokeParam );
        pG2DUtil->SetTextBoxPaneStringExpand( m_pNickNameTextPane, PARAM_NAME );
      }

      //  レベル値設定
      {
        pG2DUtil->SetTextBoxPaneNumber( m_pLvTextPane, PARAM__VALUE_3_S, rPokeParam.GetLevel(), 3 );
      }

      //  性別設定
      {
        pG2DUtil->SetPaneVisible( m_pSexPane, true );
        // @fix GFNMCat[1583] アニメ切り替えをいったんキャンセル
        pG2DUtil->StopAnime( LYT_WORK_NORMAL, LA_STAUPP_BOX_STA_UPP_000__ICON_GENDER_00_ICON_MALE );
        pG2DUtil->StopAnime( LYT_WORK_NORMAL, LA_STAUPP_BOX_STA_UPP_000__ICON_GENDER_00_ICON_FEMALE );

        pml::Sex sex = PokeTool::GetDrawSex( &rPokeParam );
        if( sex == pml::SEX_MALE )
        {
          pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_STAUPP_BOX_STA_UPP_000__ICON_GENDER_00_ICON_MALE );
        }
        else
        if( sex == pml::SEX_FEMALE )
        {
          pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_STAUPP_BOX_STA_UPP_000__ICON_GENDER_00_ICON_FEMALE );
        }
        else
        {
          pG2DUtil->SetPaneVisible( m_pSexPane, false );
        }
      }

      //  ポケモン状態アイコン設定
      {
        pG2DUtil->SetPaneVisible( m_pSickPartsPane, true );
        if( rPokeParam.HaveSick() == true )
        {
          m_initParam.pAppCommonGrapIconData->ReplacePaneTextureByPokeSickIcon( rPokeParam.GetSick(), m_pSickIconPicturePane );
        }
        else
        if( rPokeParam.GetHp() <= 0 )
        {
          m_initParam.pAppCommonGrapIconData->ReplacePaneTextureByPokeHinshiIcon( m_pSickIconPicturePane );
        }
        else
        if( rPokeParam.HavePokerusJustNow() == true )
        {
          m_initParam.pAppCommonGrapIconData->ReplacePaneTextureByPokerusIcon( m_pSickIconPicturePane );
        }
        else
        {
          //  アイコンは非表示
          pG2DUtil->SetPaneVisible( m_pSickPartsPane, false );
        }
      }

      //  言語アイコン設定
      {
        u8 lang  = (u8)rPokeParam.GetLangId();
        if( lang == System::GetLang() )
        {
          //  ポケモンの言語と操作している機器の言語が一致しているので表示しない
          pG2DUtil->SetPaneVisible( m_pLangIconPartsPane, false );
        }
        else
        {
          //  言語アイコンを差し替え
          pG2DUtil->SetPaneVisible( m_pLangIconPartsPane, true );

          //  ポケモンを捕まえた国アイコンに差し替え
          if( m_initParam.pAppCommonGrapIconData->ReplacePaneTextureByLangIcon( lang, m_pLangIconPicturePane ) == false )
          {
            //  言語が無効な場合は表示しない
            //  ここにきたら論理バグ
            pG2DUtil->SetPaneVisible( m_pLangIconPartsPane, false );
          }
        }
      }

      //  ポケモン色違いレアマーク表示設定
      pG2DUtil->SetPaneVisible( m_pRareIconPane, rPokeParam.IsRare() );

      //  ポケモンポケルス抗体マーク表示設定
      pG2DUtil->SetPaneVisible( m_pPokerusAntibodyPane, rPokeParam.HavePokerusPast() );

      //  ポケモン産地マーク表示設定
      {
        PokeTool::PokePlace::PlaceMarkID markID = PokeTool::PokePlace::GetPlaceMarkID( rPokeParam.GetCassetteVersion() );

        const u32 aAnimeIdList[]  =
        {
          LA_STAUPP_BOX_STA_UPP_000__MARK_SANCHI_ICON_6TH,
          LA_STAUPP_BOX_STA_UPP_000__MARK_SANCHI_ICON_ALORA,
					LA_STAUPP_BOX_STA_UPP_000__MARK_SANCHI_ICON_NONE,
					LA_STAUPP_BOX_STA_UPP_000__MARK_SANCHI_ICON_VC,
					LA_STAUPP_BOX_STA_UPP_000__MARK_SANCHI_ICON_HOROHORO,
        };

        GFL_ASSERT( markID < GFL_NELEMS( aAnimeIdList ) );
        for( u32 i = 0; i < GFL_NELEMS( aAnimeIdList ); ++i )
        {
          pG2DUtil->StopAnime( LYT_WORK_NORMAL, aAnimeIdList[ i ] );
        }

        pG2DUtil->StartAnime( LYT_WORK_NORMAL, aAnimeIdList[ markID ] );
      }

      //  ボックスのマーキング
      {
        const u32 aBoxAnimeIdList[ pml::pokepara::BOX_MARK_NUM ] =
        {
          LA_STAUPP_BOX_STA_UPP_000__PLATE_MARK_ICON_00,
          LA_STAUPP_BOX_STA_UPP_000__PLATE_MARK_ICON_01,
          LA_STAUPP_BOX_STA_UPP_000__PLATE_MARK_ICON_02,
          LA_STAUPP_BOX_STA_UPP_000__PLATE_MARK_ICON_03,
          LA_STAUPP_BOX_STA_UPP_000__PLATE_MARK_ICON_04,
          LA_STAUPP_BOX_STA_UPP_000__PLATE_MARK_ICON_05
        };

        pml::pokepara::BoxMarkContainer boxMark;
        rPokeParam.GetAllBoxMark( &boxMark );

        u32 loopNum = GFL_NELEMS( boxMark.markColor );
        GFL_ASSERT( GFL_NELEMS( aBoxAnimeIdList ) == loopNum );

        gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );

        for( u32 i = 0; i < loopNum; ++i )
        {
          pLytWk->BindAnime(aBoxAnimeIdList[ i ]);
          pLytWk->SetAnimeFrame( aBoxAnimeIdList[ i ], boxMark.markColor[ i ] );
        }
      }
    }

    //  パラメータの高速アクセスモードを解除
    rPokeParam.EndFastMode( bParamFastMode );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  更新処理
  *         子ウインドウについても自動的にコールする(子→親の順）
  */
  //--------------------------------------------------------------------------------------------
  void BoxUppPokeStatusDraw::Update()
  {
    if( m_bInit == false )
    {
      return;
    }

    app::util::G2DUtil* pG2DUtil            = UIView::GetG2DUtil();

    if( m_pPokeSimpleModelInFrame->IsReady() == true )
    {
      m_pPokeSimpleModelInFrame->SetVisible( UIView::IsVisible() );
    }

    if( m_changeBallId != item::BALLID_NULL )
    {
      if( m_changeBallSeq == 0 )
      {
        pG2DUtil->SetPaneVisible( m_pMonsBallPicturePane, false );
        ++m_changeBallSeq;
      }
      else
      {
        //  ロード開始(同期読み)
        //  @fix GFNMCat[1747] ボールidからアイテムidに変換してなかった
        m_pItemIconTool->SetItemIcon( 0, item::ITEM_BallID2ItemID( m_changeBallId ), m_pMonsBallPicturePane );

        pG2DUtil->SetPaneVisible( m_pMonsBallPicturePane, true );

        m_nowBallId    = m_changeBallId;
        m_changeBallId = item::BALLID_NULL;
      }
    }
    else
    if( m_nowBallId != item::BALLID_NULL )
    {
      if( m_pItemIconTool->IsReplaced( 0 ) == true )
      {
        pG2DUtil->SetPaneVisible( m_pMonsBallPicturePane, true );
      }
    }
    else
    {
      pG2DUtil->SetPaneVisible( m_pMonsBallPicturePane, false );
    }

    m_judgeDraw.Update();
    m_pItemIconTool->Update();
    m_pPokeSimpleModelInFrame->Update();

    m_pPokeModelSys->Update();

    UIView::Update();
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  描画処理
  *         子ウインドウについても自動的にコールする(親→子の順）
  */
  //--------------------------------------------------------------------------------------------
  void BoxUppPokeStatusDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    if( m_bInit == false )
    {
      return;
    }

    if( ( displayNo == gfl2::gfx::CtrDisplayNo::LEFT ) ||
        ( displayNo == gfl2::gfx::CtrDisplayNo::RIGHT ) )
    {
      //  アニメ再生で画面切り替えを即時反映するために必要
      UIView::Update();

      app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
      for( s32 i = 0; i < LYT_WORK_MAX; ++i )
      {
        pG2DUtil->AddDrawableLytWkForOneFrame( m_initParam.pRenderinManager, displayNo, i );
      }
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  2D初期化処理
  * @note
  *     レイアウトデータから描画に必要な生成をしている
  */
  //--------------------------------------------------------------------------------------------
  void BoxUppPokeStatusDraw::_Init2D()
  {
    // アーカイブIDテーブル
    const ::app::util::G2DUtil::LytResData aResTbl[] =
    {
      {  m_initParam.pLytBinData, 8, ::app::util::G2DUtil::ATTACH_NONE },
    };

    //  作成するレイアウトワーク
    const ::app::util::G2DUtil::LytwkData aSetupData[ LYT_WORK_MAX ] =
    {
      {
        0,
        LYTRES_STAUPP_BOX_STA_UPP_000_BFLYT,
        LA_STAUPP_BOX_STA_UPP_000___NUM,
        m_initParam.pLytBinData,
        true,
        app::util::G2DUtil::SETUP_UPPER,
        true
      },
    };

    UIView::Create2D( m_initParam.pHeap, NULL, 32, aResTbl, GFL_NELEMS(aResTbl), aSetupData, GFL_NELEMS(aSetupData), m_initParam.pBoxMsgData, m_pWordSet );

    UIView::SetVisible( false );

    //  通常ステータス表示の初期化
    m_normalDraw.Initialize();

    //  ジャッジ表示の初期化
    m_judgeDraw.Initialize( m_initParam.pHeap, m_initParam.pAsyncFileManager );

    //  必要なペインを取得
    {
      app::util::G2DUtil* pG2DUtil            = UIView::GetG2DUtil(); 
      gfl2::lyt::LytWk* pLayoutWork           = UIView::GetLayoutWork( LYT_WORK_NORMAL );
      gfl2::lyt::LytMultiResID& rMulitiResId  = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );

      //  状態異常のアイコンルートペイン
      {
        m_pSickPartsPane        = pLayoutWork->GetPartsPane( PANENAME_BOX_STA_UPP_000_PANE_ICON_SICK_00 );
        GFL_ASSERT( m_pSickPartsPane );

        m_pSickIconPicturePane  = pLayoutWork->GetPicturePane( m_pSickPartsPane, PANENAME_COMMON_ICN_002_PANE_ICON_SICK, &rMulitiResId );
        GFL_ASSERT( m_pSickIconPicturePane );
      }

      //  色違いレアアイコンペイン
      m_pRareIconPane         = pLayoutWork->GetPane( PANENAME_BOX_STA_UPP_000_PANE_MARK_STAR );
      GFL_ASSERT( m_pRareIconPane );

      //  ポケルス抗体アイコンペイン
      m_pPokerusAntibodyPane  = pLayoutWork->GetPane( PANENAME_BOX_STA_UPP_000_PANE_MARK_POKERUS );
      GFL_ASSERT( m_pPokerusAntibodyPane );

      //  言語アイコンペイン
      {
        m_pLangIconPartsPane    = pLayoutWork->GetPartsPane( PANENAME_BOX_STA_UPP_000_PANE_ICON_LANG );
        GFL_ASSERT( m_pLangIconPartsPane );

        m_pLangIconPicturePane  = pLayoutWork->GetPicturePane( m_pLangIconPartsPane, PANENAME_COMMON_ICN_008_PANE_BG_00, &rMulitiResId );
        GFL_ASSERT( m_pLangIconPicturePane );
      }

      //  ニックネームテキストペイン
      m_pNickNameTextPane     = pLayoutWork->GetTextBoxPane( PANENAME_BOX_PLT_UPP_000_PANE_TEXTBOX_00 );
      GFL_ASSERT( m_pNickNameTextPane );

      //  モンスターボールアイコン画像ペイン
      {
        m_pMonsBallPicturePane  = pLayoutWork->GetPicturePane( PANENAME_BOX_STA_UPP_000_PANE_ICON_BALL );
        GFL_ASSERT( m_pMonsBallPicturePane );

        pG2DUtil->SetPaneVisible( m_pMonsBallPicturePane, false );
      }

      //  性別アイコンペイン
      m_pSexPane              = pLayoutWork->GetPartsPane( PANENAME_BOX_STA_UPP_000_PANE_ICON_GENDER_00 );
      GFL_ASSERT( m_pSexPane );

      //  レベル値のテキストペイン
      m_pLvTextPane           = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_02 );
      GFL_ASSERT( m_pLvTextPane );

      //  レベル表記
      {
        gfl2::lyt::LytTextBox*  pTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_01 );
        GFL_ASSERT( pTextPane );

        pG2DUtil->SetTextBoxPaneString( pTextPane, PARAM_LEVEL_00 );
      }

      //  ロックアイコン
      m_pLockIconPane = pLayoutWork->GetPane( PANENAME_BOX_STA_UPP_000_PANE_ICON_LOCK );
      GFL_ASSERT( m_pLockIconPane );

      //  チーム登録アイコン
      m_pRegistTeamIconPane = pLayoutWork->GetPane( PANENAME_BOX_STA_UPP_000_PANE_ICON_TEAMMARK );
      GFL_ASSERT( m_pRegistTeamIconPane );
    }
  }

//  ここから先は BoxUppPokeStatusDraw::NormalDrawクラスの定義
  //--------------------------------------------------------------------------------------------
  /**
  * @brief      初期化
  * @note
  *   必ずViewの2D作成後に実行する
  */
  //--------------------------------------------------------------------------------------------
  void BoxUppPokeStatusDraw::NormalDraw::Initialize()
  {
    GFL_ASSERT( m_pRootView );

    app::util::G2DUtil* pG2DUtil            = m_pRootView->GetG2DUtil(); 
    gfl2::lyt::LytWk* pLayoutWork           = m_pRootView->GetLayoutWork( LYT_WORK_NORMAL );
    gfl2::lyt::LytMultiResID& rMulitiResId  = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );

    //  項目名など表示が固定なのはここで設定
    {
      //  図鑑No項目
      {
        gfl2::lyt::LytTextBox*  pTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_03 );
        GFL_ASSERT( pTextPane );

        pG2DUtil->SetTextBoxPaneString( pTextPane, PARAM_ZUKAN_NO_00 );

        m_pZukanNoNumTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_07 );
        GFL_ASSERT( m_pZukanNoNumTextPane );
      }

      //  種族名項目
      {
        gfl2::lyt::LytTextBox*  pTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_04 );
        GFL_ASSERT( pTextPane );

        pG2DUtil->SetTextBoxPaneString( pTextPane, PARAM_NAME_00 );

        m_pPokeKindNameTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_08 );
        GFL_ASSERT( m_pPokeKindNameTextPane );
      }

      //  わざタイプ項目
      {
        gfl2::lyt::LytTextBox*  pTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_05 );
        GFL_ASSERT( pTextPane );

        pG2DUtil->SetTextBoxPaneString( pTextPane, PARAM_TYPE );

        //  わざタイプアイコン２つのペイン取得
        //  アイコン１の取得
        {
          gfl2::lyt::LytParts*  pIconPartsPane  =  pLayoutWork->GetPartsPane( PANENAME_BOX_STA_UPP_000_PANE_ICONTYPE_00 );
          GFL_ASSERT( pIconPartsPane );

          m_pPokeTypeIcon01Pane = pLayoutWork->GetPicturePane( pIconPartsPane, PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE, &rMulitiResId );
          GFL_ASSERT( m_pPokeTypeIcon01Pane );
        }

        //  アイコン２の取得
        {
          gfl2::lyt::LytParts*  pIconPartsPane  =  pLayoutWork->GetPartsPane( PANENAME_BOX_STA_UPP_000_PANE_ICONTYPE_01 );
          GFL_ASSERT( pIconPartsPane );

          m_pPokeTypeIcon02Pane = pLayoutWork->GetPicturePane( pIconPartsPane, PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE, &rMulitiResId );
          GFL_ASSERT( m_pPokeTypeIcon02Pane );
        }
      }

      //  所持プレイヤー名項目
      {
        gfl2::lyt::LytTextBox*  pTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_06 );
        GFL_ASSERT( pTextPane );

        pG2DUtil->SetTextBoxPaneString( pTextPane, PARAM_OYANAME_00 );

        m_pHavePlayerNameTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_09 );
        GFL_ASSERT( m_pHavePlayerNameTextPane );
      }

      //  ポケモンHP項目
      {
        // HP表示
        {
          gfl2::lyt::LytTextBox*  pTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_10 );
          GFL_ASSERT( pTextPane );

          pG2DUtil->SetTextBoxPaneString( pTextPane, PARAM_HP_00 );
        }

        // / 表示
        {
          gfl2::lyt::LytTextBox*  pTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_17 );
          GFL_ASSERT( pTextPane );

          pG2DUtil->SetTextBoxPaneString( pTextPane, PARAM_HP_01 );
        }

        m_pPokeHPNowNumTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_16 );
        GFL_ASSERT( m_pPokeHPNowNumTextPane );

        m_pPokeHPMaxNumTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_18 );
        GFL_ASSERT( m_pPokeHPMaxNumTextPane );
      }

      //  ポケモン攻撃項目
      {
        gfl2::lyt::LytTextBox*  pTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_11 );
        GFL_ASSERT( pTextPane );

        pG2DUtil->SetTextBoxPaneString( pTextPane, PARAM_ATK_00 );

        m_pPokeAtkNumTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_19 );
        GFL_ASSERT( m_pPokeAtkNumTextPane );
      }

      //  ポケモン防御項目
      {
        gfl2::lyt::LytTextBox*  pTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_12 );
        GFL_ASSERT( pTextPane );

        pG2DUtil->SetTextBoxPaneString( pTextPane, PARAM_DEF_00 );

        m_pPokeDefNumTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_20 );
        GFL_ASSERT( m_pPokeDefNumTextPane );
      }

      //  ポケモンとくこう項目
      {
        gfl2::lyt::LytTextBox*  pTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_13 );
        GFL_ASSERT( pTextPane );

        pG2DUtil->SetTextBoxPaneString( pTextPane, PARAM_SPATK_00 );

        m_pPokeTokukouNumTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_21 );
        GFL_ASSERT( m_pPokeTokukouNumTextPane );
      }

      //  ポケモンとくぼう項目
      {
        gfl2::lyt::LytTextBox*  pTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_14 );
        GFL_ASSERT( pTextPane );

        pG2DUtil->SetTextBoxPaneString( pTextPane, PARAM_SPDEF_00 );

        m_pPokeTokubouNumTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_22 );
        GFL_ASSERT( m_pPokeTokubouNumTextPane );
      }

      //  ポケモンすばやさ項目
      {
        gfl2::lyt::LytTextBox*  pTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_15 );
        GFL_ASSERT( pTextPane );

        pG2DUtil->SetTextBoxPaneString( pTextPane, PARAM_AGI_00 );

        m_pPokeSpeedNumTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_23 );
        GFL_ASSERT( m_pPokeSpeedNumTextPane );
      }

      //  ポケモンせいかく項目
      {
        gfl2::lyt::LytTextBox*  pTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_24 );
        GFL_ASSERT( pTextPane );

        pG2DUtil->SetTextBoxPaneString( pTextPane, PARAM_SEIKAKU_00 );

        m_pPokeSeikakuNameTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_27 );
        GFL_ASSERT( m_pPokeSeikakuNameTextPane );
      }

      //  ポケモンとくせい項目
      {
        gfl2::lyt::LytTextBox*  pTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_25 );
        GFL_ASSERT( pTextPane );

        pG2DUtil->SetTextBoxPaneString( pTextPane, PARAM_TOKUSEI_00 );

        m_pPokeTokuseiNameTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_28 );
        GFL_ASSERT( m_pPokeTokuseiNameTextPane );
      }

      //  ポケモンもちもの項目
      {
        gfl2::lyt::LytTextBox*  pTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_26 );
        GFL_ASSERT( pTextPane );

        pG2DUtil->SetTextBoxPaneString( pTextPane, PARAM_ITEM_00 );

        m_pPokeItemNameTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_29 );
        GFL_ASSERT( m_pPokeItemNameTextPane );
      }

      //  ポケモンの所持わざ項目
      {
        gfl2::lyt::LytTextBox*  pTextPane = pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTBOX_30 );
        GFL_ASSERT( pTextPane );

        pG2DUtil->SetTextBoxPaneString( pTextPane, PARAM_WAZA_00 );

        //  各技項目のデータ作成
        {
          struct SETUP_DATA
          {
            u32 itemPartsPaneIdx;
            u32 aColorAnimeId[ POKETYPE_MAX ];
          };

          const SETUP_DATA  aSetupData[ pml::MAX_WAZA_NUM ] =
          {
            //  項目１
            {
              PANENAME_BOX_STA_UPP_000_PANE_SKILL_00,
              {
                LA_STAUPP_BOX_STA_UPP_000__SKILL_00_COLOR_00,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_00_COLOR_01,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_00_COLOR_02,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_00_COLOR_03,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_00_COLOR_04,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_00_COLOR_05,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_00_COLOR_06,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_00_COLOR_07,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_00_COLOR_08,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_00_COLOR_09,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_00_COLOR_10,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_00_COLOR_11,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_00_COLOR_12,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_00_COLOR_13,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_00_COLOR_14,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_00_COLOR_15,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_00_COLOR_16,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_00_COLOR_17
              }
            },

            //  項目２
            {
              PANENAME_BOX_STA_UPP_000_PANE_SKILL_01,
              {
                LA_STAUPP_BOX_STA_UPP_000__SKILL_01_COLOR_00,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_01_COLOR_01,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_01_COLOR_02,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_01_COLOR_03,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_01_COLOR_04,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_01_COLOR_05,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_01_COLOR_06,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_01_COLOR_07,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_01_COLOR_08,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_01_COLOR_09,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_01_COLOR_10,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_01_COLOR_11,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_01_COLOR_12,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_01_COLOR_13,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_01_COLOR_14,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_01_COLOR_15,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_01_COLOR_16,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_01_COLOR_17
              }
            },

            //  項目３
            {
              PANENAME_BOX_STA_UPP_000_PANE_SKILL_02,
              {
                LA_STAUPP_BOX_STA_UPP_000__SKILL_02_COLOR_00,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_02_COLOR_01,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_02_COLOR_02,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_02_COLOR_03,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_02_COLOR_04,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_02_COLOR_05,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_02_COLOR_06,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_02_COLOR_07,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_02_COLOR_08,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_02_COLOR_09,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_02_COLOR_10,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_02_COLOR_11,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_02_COLOR_12,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_02_COLOR_13,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_02_COLOR_14,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_02_COLOR_15,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_02_COLOR_16,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_02_COLOR_17
              }
            },

            //  項目４
            {
              PANENAME_BOX_STA_UPP_000_PANE_SKILL_03,
              {
                LA_STAUPP_BOX_STA_UPP_000__SKILL_03_COLOR_00,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_03_COLOR_01,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_03_COLOR_02,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_03_COLOR_03,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_03_COLOR_04,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_03_COLOR_05,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_03_COLOR_06,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_03_COLOR_07,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_03_COLOR_08,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_03_COLOR_09,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_03_COLOR_10,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_03_COLOR_11,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_03_COLOR_12,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_03_COLOR_13,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_03_COLOR_14,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_03_COLOR_15,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_03_COLOR_16,
                LA_STAUPP_BOX_STA_UPP_000__SKILL_03_COLOR_17
              }
            }
          };

          SKILL_ITEM_DATA*  pSetSkillData = NULL;
          u32 loopNum = GFL_NELEMS( m_aSkillItemData );
          for( u32 i = 0; i < loopNum; ++i )
          {
            pSetSkillData = &m_aSkillItemData[ i ];
            pSetSkillData->pItemPartsPane   = pLayoutWork->GetPartsPane( aSetupData[ i ].itemPartsPaneIdx );
            GFL_ASSERT( pSetSkillData->pItemPartsPane );

            pSetSkillData->pTextPane        = pLayoutWork->GetTextBoxPane( pSetSkillData->pItemPartsPane, PANENAME_BOX_PLT_UPP_000_PANE_TEXTBOX_00, &rMulitiResId );
            GFL_ASSERT( pSetSkillData->pTextPane );

            //  アニメidを転送コピー
            GFL_ASSERT( sizeof( aSetupData[ i ].aColorAnimeId ) == sizeof( pSetSkillData->aWazaColorAnimeId ) );
            gfl2::std::MemCopy( aSetupData[ i ].aColorAnimeId, pSetSkillData->aWazaColorAnimeId, sizeof( aSetupData[ i ].aColorAnimeId ) );
          }
        }
      }
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      ステータスパラメータを指定
  * @param[in]  pPokeParam:           参照するポケモンパラメータを指定
  * @note
  *   この関数が呼ばれた段階で表示するポケモンモデルのロードをする
  */
  //--------------------------------------------------------------------------------------------
  void BoxUppPokeStatusDraw::NormalDraw::SetParam( const pml::pokepara::PokemonParam& rPokeParam )
  {
    app::util::G2DUtil* pG2DUtil  = m_pRootView->GetG2DUtil();

    const INIT_PARAM& rRootViewInitParam  = m_pRootView->GetInitParam();

    //  図鑑No設定
    {
      if( PokeTool::ExtendData::IsExistLocalZukanStatic( rPokeParam.GetMonsNo(), PokeTool::ExtendData::LOCAL_AREA_A ) == true )
      {
        s32 no = PokeTool::ExtendData::GetLocalNumberStatic( rPokeParam.GetMonsNo(), PokeTool::ExtendData::LOCAL_AREA_A );
        //  図鑑にあるのでNO表示
        m_bZukanNo  = true;
        pG2DUtil->SetTextBoxPaneNumber( m_pZukanNoNumTextPane, PARAM__VALUE_3, no, 3, 0, print::NUM_DISP_ZERO );
      }
      else
      {
        //  図鑑にない
        m_bZukanNo  = false;
        pG2DUtil->SetTextBoxPaneString( m_pZukanNoNumTextPane, PARAM_ZUKAN_NO_02 );
      }
    }

    //  ポケモン種族名設定
    {
      //  種族がタマゴの場合対応
      pG2DUtil->SetRegisterMonsNameOrTamago( 0, &rPokeParam );
      pG2DUtil->SetTextBoxPaneStringExpand( m_pPokeKindNameTextPane, PARAM_POKENAME );
    }

    //  ポケモンタイプ設定
    {
      pml::PokeType type1 = rPokeParam.GetType1();
      pml::PokeType type2 = rPokeParam.GetType2();

      //  アイコン画像差し替え
      rRootViewInitParam.pAppCommonGrapIconData->ReplacePaneTextureByWazaTypeIcon( (PokeType_tag)type1, m_pPokeTypeIcon01Pane );
      rRootViewInitParam.pAppCommonGrapIconData->ReplacePaneTextureByWazaTypeIcon( (PokeType_tag)type2, m_pPokeTypeIcon02Pane );

      if( type1 != type2 )
      {
        //  ２つが異なっていると２つのタイプアイコンを表示
        pG2DUtil->SetPaneVisible( m_pPokeTypeIcon02Pane, true );
      }
      else
      {
        pG2DUtil->SetPaneVisible( m_pPokeTypeIcon02Pane, false );
      }
    }

    //  所持プレイヤー名
    {
      pG2DUtil->GetWordSet()->RegisterPokeOyaName( 0, &rPokeParam );
      pG2DUtil->SetTextBoxPaneStringExpand( m_pHavePlayerNameTextPane, PARAM_OYANAME_01 );
    }

    //  ポケモンHP値設定
    {
      //  HP
      pG2DUtil->SetTextBoxPaneNumber( m_pPokeHPNowNumTextPane, PARAM__VALUE_3, rPokeParam.GetHp(), 3, 0 );
      //  MaxHP
      pG2DUtil->SetTextBoxPaneNumber( m_pPokeHPMaxNumTextPane, PARAM__VALUE_3, rPokeParam.GetMaxHp(), 3, 0 );
    }

    //  基本パラメータの更新
    {
      //  ポケモン攻撃値設定
      pG2DUtil->SetTextBoxPaneNumber( m_pPokeAtkNumTextPane, PARAM__VALUE_3, rPokeParam.GetPower( pml::pokepara::POWER_ATK ), 3, 0 );

      //  ポケモン防御値設定
      pG2DUtil->SetTextBoxPaneNumber( m_pPokeDefNumTextPane, PARAM__VALUE_3, rPokeParam.GetPower( pml::pokepara::POWER_DEF ), 3, 0 );

      //  ポケモンとくこう値設定
      pG2DUtil->SetTextBoxPaneNumber( m_pPokeTokukouNumTextPane, PARAM__VALUE_3, rPokeParam.GetPower( pml::pokepara::POWER_SPATK ), 3, 0 );

      //  ポケモンとくぼう値設定
      pG2DUtil->SetTextBoxPaneNumber( m_pPokeTokubouNumTextPane, PARAM__VALUE_3, rPokeParam.GetPower( pml::pokepara::POWER_SPDEF ), 3, 0 );

      //  ポケモンすばやさ値設定
      pG2DUtil->SetTextBoxPaneNumber( m_pPokeSpeedNumTextPane, PARAM__VALUE_3, rPokeParam.GetPower( pml::pokepara::POWER_AGI ), 3, 0 );
    }

    //  ポケモン性格名設定
    {
      pG2DUtil->SetRegisterSeikakuName( 0, rPokeParam.GetSeikaku() );
      pG2DUtil->SetTextBoxPaneStringExpand( m_pPokeSeikakuNameTextPane, PARAM_SEIKAKU_01 );
    }

    //  ポケモン特性名設定
    {
      pG2DUtil->SetRegisterTokuseiName( 0, rPokeParam.GetTokuseiNo() );
      pG2DUtil->SetTextBoxPaneStringExpand( m_pPokeTokuseiNameTextPane, PARAM_TOKUSEI_01 );
    }

    //  ポケモン持ち物名設定
    {
      pG2DUtil->SetRegisterItemName( 0, rPokeParam.GetItem(), 1 );
      pG2DUtil->SetTextBoxPaneStringExpand( m_pPokeItemNameTextPane, PARAM_ITEM_01 );
    }

    //  ポケモン所持技設定
    {
      SKILL_ITEM_DATA*  pItemData = NULL;
      u8  wazaHaveNum = rPokeParam.GetWazaCount();
      WazaNo  wazaId  = WAZANO_NULL;

      u8  loopNum     = GFL_NELEMS( m_aSkillItemData );
      for( u8 i = 0; i < loopNum; ++i )
      {
        pItemData = &m_aSkillItemData[ i ];
        // @fix GFNMCat[1583] アニメ切り替えをいったんキャンセル
        {
          for( u8 colorAnimeIdx = 0; colorAnimeIdx < GFL_NELEMS( pItemData->aWazaColorAnimeId ); ++colorAnimeIdx )
          {
            pG2DUtil->StopAnime( LYT_WORK_NORMAL, pItemData->aWazaColorAnimeId[ colorAnimeIdx ] );
          }
        }

        if( i < wazaHaveNum )
        {
          wazaId  = rPokeParam.GetWazaNo( i );

          pG2DUtil->SetPaneVisible( pItemData->pItemPartsPane, true );

          //  技名を設定
          pG2DUtil->SetRegisterWazaName( 0, wazaId );
          pG2DUtil->SetTextBoxPaneStringExpand( pItemData->pTextPane, PARAM_WAZA_01 );

          //  技項目色を設定
          pml::PokeType type  = pml::wazadata::GetType( wazaId );
          GFL_ASSERT( type < GFL_NELEMS( pItemData->aWazaColorAnimeId ) );
          pG2DUtil->StartAnime( LYT_WORK_NORMAL, pItemData->aWazaColorAnimeId[ type ] );
        }
        else
        {
          //  非表示
          pG2DUtil->SetPaneVisible( pItemData->pItemPartsPane, false );
        }
      }
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      表示指定
  * @param[in]  bFlg:           表示フラグ
  */
  //--------------------------------------------------------------------------------------------
  void BoxUppPokeStatusDraw::NormalDraw::SetVisible( const bool bFlg )
  {
    GFL_ASSERT( m_pRootView );
    app::util::G2DUtil* pG2DUtil  = m_pRootView->GetG2DUtil();

    // @fix NMCat[2858] 画面切り替えアニメは一度停止しないと残る
    pG2DUtil->StopAnime( LYT_WORK_NORMAL, LA_STAUPP_BOX_STA_UPP_000_PATERN_STATUS, false );
    pG2DUtil->StopAnime( LYT_WORK_NORMAL, LA_STAUPP_BOX_STA_UPP_000_PATERN_NONUMBER, false );

    if( bFlg == true )
    {
      //  図鑑番号がある
      if( m_bZukanNo == true )
      {
        pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_STAUPP_BOX_STA_UPP_000_PATERN_STATUS );
      }
      else
      {
        //  図鑑番号がない
        pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_STAUPP_BOX_STA_UPP_000_PATERN_NONUMBER );
      }
    }
  }

//  ここから先は BoxUppPokeStatusDraw::JudgeDrawクラスの定義
  //--------------------------------------------------------------------------------------------
  /**
  * @brief      デストラクタ
  * @note
  *             レーダーチャートのメモリ開放をここでしている
  */
  //--------------------------------------------------------------------------------------------
  BoxUppPokeStatusDraw::JudgeDraw::~JudgeDraw()
  {
    GFL_SAFE_DELETE( m_pRaderChart );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      初期化
  * @param[in]  pHeap: メモリ確保するアプリ用ヒープ
  * @note
  *   必ずViewの2D作成後に実行する
  */
  //--------------------------------------------------------------------------------------------
  void BoxUppPokeStatusDraw::JudgeDraw::Initialize( app::util::Heap* pUtilHeap, gfl2::fs::AsyncFileManager* pAsyncFileManager )
  {
    GFL_ASSERT( pUtilHeap );
    GFL_ASSERT( pAsyncFileManager );

    m_bReady  = false;

    m_rand.Initialize();

    app::util::G2DUtil* pG2DUtil            = m_pRootView->GetG2DUtil();
    GFL_ASSERT( pG2DUtil != NULL );

    gfl2::lyt::LytWk* pLayoutWork           = m_pRootView->GetLayoutWork( LYT_WORK_NORMAL );
    GFL_ASSERT( pLayoutWork != NULL );

    gfl2::lyt::LytMultiResID& rMultiResId   = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );

    //  レーダーチャート作成
    {
      m_pRaderChart = GFL_NEW( pUtilHeap->GetDeviceHeap() ) app::tool::BoxRaderChart();
      //  作成を非同期開始
      m_pRaderChart->StartAsyncCreate( pUtilHeap, pAsyncFileManager );

      // レーダーチャートの中心座標
      {
        gfl2::lyt::LytPane * pPane = pLayoutWork->GetPane( PANENAME_BOX_STA_UPP_000_PANE_PIVOT_CENTER );
        GFL_ASSERT( pPane );

        gfl2::math::Vector3 vec = gfl2::math::ConvertNwVec3ToGfVec3( pPane->GetTranslate() );
        m_raderChartPos.x = vec.x - 0.5f;  // @fix NMCat[832] グラフの数値が最大の時に外枠に達していない  // 横に-0.5ずらすと枠の絵に合うしきれいに描画される
        m_raderChartPos.y = vec.y;
      }

      // レーダーチャート半径
      {
        // @fix GFNMCat[1117] レーダーチャートの半径計算に必要な終点位置のペインを変更
        gfl2::lyt::LytPane * pPane = pLayoutWork->GetPane( PANENAME_BOX_STA_UPP_000_PANE_PIVOT_00 );
        GFL_ASSERT( pPane );

        gfl2::math::Vector3 vec = gfl2::math::ConvertNwVec3ToGfVec3( pPane->GetTranslate() );

        m_raderChartRadiusVal = gfl2::math::FAbs( vec.y - m_raderChartPos.y ) + 1.0f;  // @fix NMCat[832] グラフの数値が最大の時に外枠に達していない  // 横に-0.5ずらしたので半径は1.0増やしておく
      }
    }

    //  パラメータからのコメントメッセージテキストペイン取得
    {
      gfl2::lyt::LytParts*  pPartsPane  = pLayoutWork->GetPartsPane( PANENAME_BOX_STA_UPP_000_PANE_MESSAGE );
      GFL_ASSERT( pPartsPane );

      m_pStatusCommentMessageTextPane = pLayoutWork->GetTextBoxPane( pPartsPane, PANENAME_COMMON_MSG_UPP_001_PANE_TEXTBOX_00, &rMultiResId );
      GFL_ASSERT( m_pStatusCommentMessageTextPane );
    }

    //  パラメータ項目データリストを作成
    {
      const JUDGE_SETUP_PARAM_ITEM_DATA  c_aInitSetupParamItemDataList[ pml::pokepara::POWER_NUM ] =
      {
        //  HP
        {
          pml::personal::PARAM_ID_basic_hp,
          pml::pokepara::POWER_HP,

          pLayoutWork->GetPicturePane( PANENAME_BOX_STA_UPP_000_PANE_PARAMNAME_00 ),

          LA_STAUPP_BOX_STA_UPP_000_PARAM_NAME_00,

          //  パラメータ最大の星マークペイン
          pLayoutWork->GetPartsPane( PANENAME_BOX_STA_UPP_000_PANE_STAR_00 ),
          //  評価テキストペイン
          pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTVALUE_00 ),

          //  レーダーチャート頂点idx
          0,
        },

        //  ATK
        {
          pml::personal::PARAM_ID_basic_atk,
          pml::pokepara::POWER_ATK,

          pLayoutWork->GetPicturePane( PANENAME_BOX_STA_UPP_000_PANE_PARAMNAME_01 ),

          LA_STAUPP_BOX_STA_UPP_000_PARAM_NAME_01,

          //  パラメータ最大の星マークペイン
          pLayoutWork->GetPartsPane( PANENAME_BOX_STA_UPP_000_PANE_STAR_01 ),
          //  評価テキストペイン
          pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTVALUE_01 ),

          //  レーダーチャート頂点idx
          1,
        },

        //  DEF
        {
          pml::personal::PARAM_ID_basic_def,
          pml::pokepara::POWER_DEF,

          pLayoutWork->GetPicturePane( PANENAME_BOX_STA_UPP_000_PANE_PARAMNAME_02 ),

          LA_STAUPP_BOX_STA_UPP_000_PARAM_NAME_02,

          //  パラメータ最大の星マークペイン
          pLayoutWork->GetPartsPane( PANENAME_BOX_STA_UPP_000_PANE_STAR_02 ),
          //  評価テキストペイン
          pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTVALUE_02 ),

          //  レーダーチャート頂点idx
          2,
        },

        //  AGI
        {
          pml::personal::PARAM_ID_basic_agi,
          pml::pokepara::POWER_AGI,

          pLayoutWork->GetPicturePane( PANENAME_BOX_STA_UPP_000_PANE_PARAMNAME_05 ),

          LA_STAUPP_BOX_STA_UPP_000_PARAM_NAME_05,

          //  パラメータ最大の星マークペイン
          pLayoutWork->GetPartsPane( PANENAME_BOX_STA_UPP_000_PANE_STAR_05 ),
          //  評価テキストペイン
          pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTVALUE_05 ),

          //  レーダーチャート頂点idx
          3,
        },

        //  POWER_SPATK
        {
          pml::personal::PARAM_ID_basic_spatk,
          pml::pokepara::POWER_SPATK,

          pLayoutWork->GetPicturePane( PANENAME_BOX_STA_UPP_000_PANE_PARAMNAME_03 ),

          LA_STAUPP_BOX_STA_UPP_000_PARAM_NAME_03,

          //  パラメータ最大の星マークペイン
          pLayoutWork->GetPartsPane( PANENAME_BOX_STA_UPP_000_PANE_STAR_03 ),
          //  評価テキストペイン
          pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTVALUE_03 ),

          //  レーダーチャート頂点idx
          5,
        },

        //  POWER_SPDEF
        {
          pml::personal::PARAM_ID_basic_spdef,
          pml::pokepara::POWER_SPDEF,

          pLayoutWork->GetPicturePane( PANENAME_BOX_STA_UPP_000_PANE_PARAMNAME_04 ),

          LA_STAUPP_BOX_STA_UPP_000_PARAM_NAME_04,

          //  パラメータ最大の星マークペイン
          pLayoutWork->GetPartsPane( PANENAME_BOX_STA_UPP_000_PANE_STAR_04 ),
          //  評価テキストペイン
          pLayoutWork->GetTextBoxPane( PANENAME_BOX_STA_UPP_000_PANE_TEXTVALUE_04 ),

          //  レーダーチャート頂点idx
          4,
        }
      };

      GFL_ASSERT( sizeof( c_aInitSetupParamItemDataList ) == sizeof( m_aSetupParamItemDataList ) );
      gfl2::std::MemCopy( c_aInitSetupParamItemDataList, m_aSetupParamItemDataList, sizeof( m_aSetupParamItemDataList ) );

      //  項目名アイコンのテクスチャ差し替え
      {
        const INIT_PARAM& rRootViewInitParam  = m_pRootView->GetInitParam();

        u32 loopNum = GFL_NELEMS( m_aSetupParamItemDataList );
        for( u32 i = 0; i < loopNum; ++i )
        {
          GFL_ASSERT( m_aSetupParamItemDataList[ i ].pItenNameIconPane );
          rRootViewInitParam.pAppCommonGrapIconData->ReplacePaneTextureByPokeParamIcon( m_aSetupParamItemDataList[ i ].talentId, m_aSetupParamItemDataList[ i ].pItenNameIconPane );
        }
      }
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      終了処理
  */
  //--------------------------------------------------------------------------------------------
  void BoxUppPokeStatusDraw::JudgeDraw::Terminate()
  {
    if( m_pRaderChart != NULL )
    {
      //  レーダーチャート描画リストからはずす
      m_pRootView->GetInitParam().pRenderinManager->UnsetRaderChart( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY );
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      破棄可能か
  * @return     可能 true / 不可能 false
  */
  //--------------------------------------------------------------------------------------------
  bool BoxUppPokeStatusDraw::JudgeDraw::IsDelete()
  {
    bool  bRet  = true;

    if( m_pRaderChart != NULL )
    {
      //  レーダーチャートが破棄できるかチェック
      m_pRaderChart->Update();
      if( m_pRaderChart->CanDestroy() == false )
      {
        bRet  = false;
      }
    }

    return  bRet;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      使用準備できているか
  * @return     できている true / できていない false
  */
  //--------------------------------------------------------------------------------------------
  bool BoxUppPokeStatusDraw::JudgeDraw::IsReady()
  {
    if( m_bReady == true )

    GFL_ASSERT( m_pRaderChart != NULL );

    bool  bRet  = true;

    m_pRaderChart->Update();
    if( m_pRaderChart->IsCreated() == false )
    {
      bRet  = false;
    }
    else
    {
      m_pRaderChart->SetPos( m_raderChartPos );
      m_pRaderChart->SetBoxRadius( m_raderChartRadiusVal );

      // @fix 岩尾さん要望　固体値の色を変更
      gfl2::math::Vector4 outsideColor( 110.0f / 255.0f, 30.0f / 255.0f, 255.0f / 255.0f, 160.0f / 255.0f );
      m_pRaderChart->SetColor( app::util::g_raderChartPokeParamBaseColor, outsideColor );
    }

    m_bReady  = bRet;
    return  bRet;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      更新処理
  */
  //--------------------------------------------------------------------------------------------
  void BoxUppPokeStatusDraw::JudgeDraw::Update()
  {
    if( IsReady() == false )
    {
      return;
    }

    if( m_pRaderChart != NULL )
    {
      // @fix NMCat[2877] レーダーチャートの描画とUIViewの描画では表示タイミングがチャートの方が早いので
      //                  更新処理内で表示設定をすることで表示タイミングを合わせる
      //  大本のViewが有効になっていないとレーダーチャートは表示しない
      bool  bRootViewVisible  = m_pRootView->IsVisible();
      SetVisibleReaderChart( ( ( m_bVisible == true ) && ( bRootViewVisible == true ) ) );

      m_pRaderChart->Update();
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      ステータスパラメータを指定
  * @param[in]  pPokeParam:           参照するポケモンパラメータを指定
  */
  //--------------------------------------------------------------------------------------------
  void BoxUppPokeStatusDraw::JudgeDraw::SetParam( const pml::pokepara::PokemonParam& rPokeParam )
  {
    GFL_ASSERT( m_pRootView != NULL );

    app::util::G2DUtil* pG2DUtil  = m_pRootView->GetG2DUtil();
    GFL_ASSERT( pG2DUtil != NULL );

    //  パラメータロード
    pml::personal::LoadPersonalData( rPokeParam.GetMonsNo(), rPokeParam.GetFormNo() );

    //  ポケモンの得意・不得意パラメータを取得
    pml::pokepara::PowerID  goodPowerId;
    pml::pokepara::PowerID  weakPowerId;
    app::util::OutputSpecialPointPokemSeikaku( rPokeParam.GetSeikaku(), &goodPowerId, &weakPowerId );

    JUDGE_SETUP_PARAM_ITEM_DATA*  pSetupParamItemData = NULL;

    u32 talentAllNum  = 0;

    u32 loopNum = GFL_NELEMS( m_aSetupParamItemDataList );
    for( u32 i = 0; i < loopNum; ++i )
    {
      pSetupParamItemData = &m_aSetupParamItemDataList[ i ];

      u32 personalNum  = pml::personal::GetPersonalParam( pSetupParamItemData->personalId );
      u32 talentNum    = rPokeParam.GetTalentPower( pSetupParamItemData->talentId );

      talentAllNum  += talentNum;

      //  レーダーチャート設定
      {
        // @fix GFNMCat[1757] レーダーチャートの頂点指定をデータテーブル上で行えるようにした（ループのidx値でずれるから）
        m_pRaderChart->SetBoxVal( app::tool::RaderChart::FIGURE_INSIDE, pSetupParamItemData->readerChartPointIdx, personalNum );
        m_pRaderChart->SetBoxVal( app::tool::RaderChart::FIGURE_OUTSIDE, pSetupParamItemData->readerChartPointIdx, talentNum );
      }

      //  パラメータ評価テキスト設定
      {
        u32 textId  = 0;
        if( rPokeParam.IsTrainingDone( pSetupParamItemData->talentId ) == true )
        {
          //  すごい特訓済みなら専用テキスト表示
          textId  = msg_judge_03_01;
        }
        else
        {
          textId  = _ConverParamNumToEvaluationTextId( talentNum );
        }

        pG2DUtil->SetTextBoxPaneString( pSetupParamItemData->pEvaluationTextPane, textId );
      }

      //  パラメータ項目の色指定
      {
        u32 colorFrameNum = ANIME_FRAME_PARAM_ITEM_COLOR_NONE;
        // @fix NMCat[863] 得意な項目は赤に不得意な項目は青にする

        //  得意なパラメータは項目を赤に
        if( pSetupParamItemData->talentId == goodPowerId )
        {
          colorFrameNum = ANIME_FRAME_PARAM_ITEM_COLOR_RAD;
        }
        else
        //  不得意なパラメータは項目を青に
        if( pSetupParamItemData->talentId == weakPowerId )
        {
          colorFrameNum = ANIME_FRAME_PARAM_ITEM_COLOR_BLUE;
        }

        pG2DUtil->StartAnimeFrame( LYT_WORK_NORMAL, pSetupParamItemData->colorAnimeId, colorFrameNum, false, true );
      }
    }

    //  相貌固体値からコメントテキストidを取得
    {
      u32 textId  = _ConverParamTotalNumToEvaluationTextId( talentAllNum );
      pG2DUtil->SetTextBoxPaneString( m_pStatusCommentMessageTextPane, textId );
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      表示指定
  * @param[in]  bFlg:           表示フラグ
  */
  //--------------------------------------------------------------------------------------------
  void BoxUppPokeStatusDraw::JudgeDraw::SetVisible( const bool bFlg )
  {
    GFL_ASSERT( m_pRootView );
    app::util::G2DUtil* pG2DUtil  = m_pRootView->GetG2DUtil();

    // @fix NMCat[2858] 画面切り替えアニメは一度停止しないと残る
    pG2DUtil->StopAnime( LYT_WORK_NORMAL, LA_STAUPP_BOX_STA_UPP_000_PATERN_JUDGE, false );

    if( bFlg == true )
    {
      pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_STAUPP_BOX_STA_UPP_000_PATERN_JUDGE );
    }

    m_bVisible  = bFlg;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      レーダーチャート表示指定
  * @param[in]  bFlg:           表示フラグ
  */
  //--------------------------------------------------------------------------------------------
  void BoxUppPokeStatusDraw::JudgeDraw::SetVisibleReaderChart( const bool bFlg )
  {
    //  一旦レーダーチャート描画リストからはずす
    m_pRootView->GetInitParam().pRenderinManager->UnsetRaderChart( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY );

    // @fix GFNMCat[1117] ジャッジ表示中でないと引数で設定してもレーダーチャートは表示しない
    //                    ジャッジ表示モードでないのに表示関数が呼ばれることがあるから
    if( ( bFlg == true ) && ( m_bVisible == true ) ) 
    {
      m_pRootView->GetInitParam().pRenderinManager->SetRaderChart( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_pRaderChart );        
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      パラメータ値から評価テキストidを変換
  * @param[in]  paramNum: テキストidに変換したいパラメータ値
  * @return     paramNumの値から評価テキストidを返す
  */
  //--------------------------------------------------------------------------------------------
  u32 BoxUppPokeStatusDraw::JudgeDraw::_ConverParamNumToEvaluationTextId( const u32 paramNum )
  {
    u32 loopNum = GFL_NELEMS( StatusLocalData::s_aPersonalEvaluationTableData );
    for( u32 i = 0; i < loopNum; ++i )
    {
      //  上限内に値が収まっているかどうか
      if( StatusLocalData::s_aPersonalEvaluationTableData[ i ].minNum <= paramNum )
      {
        if( paramNum <= StatusLocalData::s_aPersonalEvaluationTableData[ i ].maxNum )
        {
          return  StatusLocalData::s_aPersonalEvaluationTableData[ i ].textId;
        }
      }
    }

    return  msg_judge_01_06;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      トータルパラメータ値から評価テキストidを変換
  * @param[in]  paramTotalNum: テキストidに変換したいパラメータ値
  * @return     paramTotalNumの値から評価テキストidを返す
  */
  //--------------------------------------------------------------------------------------------
  u32 BoxUppPokeStatusDraw::JudgeDraw::_ConverParamTotalNumToEvaluationTextId( const u32 paramTotalNum )
  {
    u32 loopNum = GFL_NELEMS( StatusLocalData::s_aAllNumPersonalEvaluationTableData );
    for( u32 i = 0; i < loopNum; ++i )
    {
      //  上限内に値が収まっているかどうか
      if( StatusLocalData::s_aAllNumPersonalEvaluationTableData[ i ].minNum <= paramTotalNum )
      {
        if( paramTotalNum <= StatusLocalData::s_aAllNumPersonalEvaluationTableData[ i ].maxNum )
        {
          return  StatusLocalData::s_aAllNumPersonalEvaluationTableData[ i ].textId;
        }
      }
    }

    return  msg_judge_02_01;
  }

GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )
