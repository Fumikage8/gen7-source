//======================================================================
/**
 * @file BoxUppChatDraw.cpp
 * @date 2015/07/22 19:21:44
 * @author uchida_yuto
 * @brief Boxの上画面のチャット表示
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "App/Box/source/BoxUppChatDraw.h"

//  nijiのインクルード
#include "PokeTool/include/PokeModelSystem.h"
#include "AppLib/include/Util/AppRenderingManager.h"
#include "AppLib/include/Tool/AppToolPokeSimpleModelInFrame.h"
#include "PokeTool/include/PokeTool.h"

//  素材のインクルード
#include "niji_conv_header/app/box/res2d/trdupp.h"
#include "niji_conv_header/app/box/res2d/trdupp_anim_list.h"
#include "niji_conv_header/app/box/res2d/trdupp_pane.h"

//  テキストのインクルード
#include "niji_conv_header/message/msg_box.h"

//  リソースのインクルード
// 動的ヘッダシステムを利用するときは以下のヘッダをインクルードする
#include <debug/include/DynamicHeader/gfl2_DebugDynamicHeader.h>

//  アプリ全体で使う動的パラメータ
#define NIJI_DEBUG_BOX_UPP_CHAT_DRAW_HEADER_FILEPATH "niji_conv_header/app/box/dynamicHeader/BoxUppChatDrawDynamic.h"

// 動的ヘッダシステムで利用するヘッダは以下のマクロを通してincludeしてください
#include GFL_DYNAMIC_HEADER(NIJI_DEBUG_BOX_UPP_CHAT_DRAW_HEADER_FILEPATH)

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

  BoxUppChatDraw::~BoxUppChatDraw()
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
  bool BoxUppChatDraw::InitFunc()
  {
    if( m_bInit == true )
    {
      return  true;
    }

    enum
    {
      SEQ_INIT  = 0,
      SEQ_WAIT_LOAD
    };

    if( m_initSeq == SEQ_INIT )
    {
      // ヘッダファイルのロード
      // ファイルパスと同時にテーブルのキャパシティ(最大定義数)も指定する必要がある
      GFL_LOAD_HEADER_HASH(NIJI_DEBUG_BOX_UPP_CHAT_DRAW_HEADER_FILEPATH, 64);

      //  トレーナーアイコン表示を生成
      {
        m_pTrainerIconRendering = GFL_NEW( m_pUtilHeap->GetDeviceHeap() ) app::tool::AppToolTrainerIconRendering();
        //  表示アイコン最大数2
        m_pTrainerIconRendering->Initialize( m_pRenderinManager, m_pUtilHeap, GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager(), 2, app::util::G2DUtil::SETUP_UPPER );
      }

      //  レイアウトに必要なWordSetを作成
      {
        m_pWordSet  = GFL_NEW( m_pUtilHeap->GetDeviceHeap() ) print::WordSet( m_pUtilHeap->GetDeviceHeap() );
      }

      //  2D画面を生成
      {
        _Init2D();
      }

      //  ポケモンモデル関連の初期化
      {
        //  ポケモンモデルシステム作成
        m_pPokeModelSys = GFL_NEW( m_pUtilHeap->GetDeviceHeap() ) PokeTool::PokeModelSystem();
        GFL_ASSERT( m_pPokeModelSys );

        //  ポケモンモデルシステム初期化非同期
        m_pPokeModelSys->InitSystemAsync( 
          m_pUtilHeap->GetDeviceHeap(),
          m_pUtilHeap->GetDeviceHeap(),
          GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager(),
          GFL_NELEMS( m_pPlayerObjList ) );
      }

      ++m_initSeq;
    }

    if( m_initSeq == SEQ_WAIT_LOAD )
    {
      //  ポケモンモデル管理とトレーナーアイコン表示の準備待ち
      if( ( m_pPokeModelSys->IsFinishInitSystemAsync() == true ) && ( m_pTrainerIconRendering->IsReady() == true ) )
      {
        //  ポケモンモデルシステムのヒープ作成
        {
          m_pPokeModelSys->CreateSystem( false, NULL );

          //ポケモン用ヒープ作成
          PokeTool::PokeModelSystem::HeapOption opt;
          opt.animeType       = PokeTool::MODEL_ANIMETYPE_APP;
          opt.useIdModel      = false;
          opt.useShadow       = false;
          opt.useColorShader  = false;
          m_pPokeModelSys->CreatePokeModelHeap( m_pUtilHeap->GetDeviceHeap(), &opt );
        }

        gfl2::lyt::LytWk* pLayoutWork = UIView::GetLayoutWork( LYT_WORK_NORMAL );
        app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

        //  各プレイヤーのオブジェクトを作成
        {
          //  ２体分で再ロードするので必要
          //  自身
          {
            u32 layerNo = 0;
            PlayerObj*  pSelfPlayerObj  = GFL_NEW( m_pUtilHeap->GetDeviceHeap() ) PlayerObj( m_pUtilHeap, m_pPokeModelSys, m_pRenderinManager, layerNo );

            //  インフレームのライト環境設定
            {
              pSelfPlayerObj->m_pPokeInFrameDrawEnvNode = app::util::AppRenderingManager::CreateDefaultDrawEnv( m_pUtilHeap );
              GFL_ASSERT( pSelfPlayerObj->m_pPokeInFrameDrawEnvNode );
              app::tool::PokeSimpleModelInFrame::SetLightParamForInFrame( pSelfPlayerObj->m_pPokeInFrameDrawEnvNode->GetLightSet( 0 )->GetLightNode( 0 ), System::Camera::CModelViewerInFrame::TURN_DIRECTION_RIGHT );
              m_pRenderinManager->AddDrawEnv( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, pSelfPlayerObj->m_pPokeInFrameDrawEnvNode, layerNo );
            }

            pSelfPlayerObj->m_pPokeSimpleModelInFrame->InitInFrame( System::Camera::CModelInFrame::SCREEN_UPPER, System::Camera::CModelViewerInFrame::TURN_DIRECTION_RIGHT );

            //  インフレーム枠は動的ヘッダーで調整
            {
              pSelfPlayerObj->m_pPokeSimpleModelInFrame->SetInFrameFrame( 
              GFL_VALUE_DEFINE_IN_HEADER( BOX_UPP_CHAT_DRAW_DYANMIC_POKE_LEFT_IN_FRAME_LEFT ), 
              GFL_VALUE_DEFINE_IN_HEADER( BOX_UPP_CHAT_DRAW_DYANMIC_POKE_LEFT_IN_FRAME_TOP ), 
              GFL_VALUE_DEFINE_IN_HEADER( BOX_UPP_CHAT_DRAW_DYANMIC_POKE_LEFT_IN_FRAME_RIGHT ), 
              GFL_VALUE_DEFINE_IN_HEADER( BOX_UPP_CHAT_DRAW_DYANMIC_POKE_LEFT_IN_FRAME_BOTTOM ) );

              gfl2::math::Vector3 pos( 0, 0, 0 );
              pSelfPlayerObj->m_pPokeSimpleModelInFrame->SetPos( pos );
            }

            pSelfPlayerObj->m_pPokeSimpleModelInFrame->SetAnime( PokeTool::MODEL_ANIME_BT_WAIT_A, true );

            //  チャットテキストペイン取得
            pSelfPlayerObj->m_pChatMesssageTextPane   = pLayoutWork->GetTextBoxPane( PANENAME_BOX_TRD_UPP_000_PANE_TEXTCOMMENT_01 );
            GFL_ASSERT( pSelfPlayerObj->m_pChatMesssageTextPane );

            //  ポケモンレベル値テキストペイン取得
            pSelfPlayerObj->m_pPokeLvNumTextPane      = pLayoutWork->GetTextBoxPane( PANENAME_BOX_TRD_UPP_000_PANE_TEXT_999_01 );
            GFL_ASSERT( pSelfPlayerObj->m_pPokeLvNumTextPane );

            //  ポケモンレベル値のタイトル
            {
              pSelfPlayerObj->m_pPokeLvTitleTextPane      = pLayoutWork->GetTextBoxPane( PANENAME_BOX_TRD_UPP_000_PANE_TEXT_LV_01 );
              GFL_ASSERT( pSelfPlayerObj->m_pPokeLvTitleTextPane );
              pG2DUtil->SetTextBoxPaneString( pSelfPlayerObj->m_pPokeLvTitleTextPane, PARAM_LEVEL_00 );
            }

            //  ポケモン名のテキストペイン取得
            pSelfPlayerObj->m_pPokeNameTextPane       = pLayoutWork->GetTextBoxPane( PANENAME_BOX_TRD_UPP_000_PANE_TEXTNAME_01 );
            GFL_ASSERT( pSelfPlayerObj->m_pPokeNameTextPane );
            
            //  ポケモンの所持アイテムアイコンペイン取得
            pSelfPlayerObj->m_pHaveItemIconPane       = pLayoutWork->GetPane( PANENAME_BOX_TRD_UPP_000_PANE_ICONITEM_01 );
            GFL_ASSERT( pSelfPlayerObj->m_pHaveItemIconPane );

            //  ポケモンの所持アイテム名のテキストペイン取得
            pSelfPlayerObj->m_pHaveItemTextPane       = pLayoutWork->GetTextBoxPane( PANENAME_BOX_TRD_UPP_000_PANE_TEXTITEM_01 );
            GFL_ASSERT( pSelfPlayerObj->m_pHaveItemTextPane );

            //  ポケモンの性別アイコンアニメ・ペイン取得
            {
              pSelfPlayerObj->m_pSexIconPane  = pLayoutWork->GetPane( PANENAME_BOX_TRD_UPP_000_PANE_GENDER_01 );
              GFL_ASSERT( pSelfPlayerObj->m_pSexIconPane );

              pSelfPlayerObj->m_sexMaleIconAnimeId    = LA_TRDUPP_BOX_TRD_UPP_000__GENDER_01_ICON_MALE;
              pSelfPlayerObj->m_sexFemaleIconAnimeId  = LA_TRDUPP_BOX_TRD_UPP_000__GENDER_01_ICON_FEMALE;
            }

            //  トレーナーアイコン画像ペイン取得
            pSelfPlayerObj->m_pTrainerIconPane        = pLayoutWork->GetPicturePane( PANENAME_BOX_TRD_UPP_000_PANE_PLAYERPHOTO_00 );
            GFL_ASSERT( pSelfPlayerObj->m_pTrainerIconPane );
            pG2DUtil->SetPaneVisible( pSelfPlayerObj->m_pTrainerIconPane, false );

            pSelfPlayerObj->m_pTrainerIconFramePane   = pLayoutWork->GetPane( PANENAME_BOX_TRD_UPP_000_PANE_WINDOW_PLAYER_FRAME_00 );
            GFL_ASSERT( pSelfPlayerObj->m_pTrainerIconFramePane );

            m_pPlayerObjList[ PLYAER_TYPE_SELF ]  = pSelfPlayerObj;
          }

          //  相手
          {
            u32 layerNo = 1;
            PlayerObj*  pOtherPlayerObj  = GFL_NEW( m_pUtilHeap->GetDeviceHeap() ) PlayerObj( m_pUtilHeap, m_pPokeModelSys, m_pRenderinManager, layerNo );

            //  インフレームのライト環境設定
            {
              pOtherPlayerObj->m_pPokeInFrameDrawEnvNode = app::util::AppRenderingManager::CreateDefaultDrawEnv( m_pUtilHeap );
              GFL_ASSERT( pOtherPlayerObj->m_pPokeInFrameDrawEnvNode );
              app::tool::PokeSimpleModelInFrame::SetLightParamForInFrame( pOtherPlayerObj->m_pPokeInFrameDrawEnvNode->GetLightSet( 0 )->GetLightNode( 0 ), System::Camera::CModelViewerInFrame::TURN_DIRECTION_LEFT );
              // @fix GFNMCat[1407] ポケモンモデルのレイヤー層とライトに反映するレイヤー層がずれていた。
              m_pRenderinManager->AddDrawEnv( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, pOtherPlayerObj->m_pPokeInFrameDrawEnvNode, layerNo );
            }

            pOtherPlayerObj->m_pPokeSimpleModelInFrame->InitInFrame( System::Camera::CModelInFrame::SCREEN_UPPER, System::Camera::CModelViewerInFrame::TURN_DIRECTION_LEFT );
            pOtherPlayerObj->m_pPokeSimpleModelInFrame->SetAnime( PokeTool::MODEL_ANIME_BT_WAIT_A, true );

            //  インフレーム枠は動的ヘッダーで調整
            {
              pOtherPlayerObj->m_pPokeSimpleModelInFrame->SetInFrameFrame( 
              GFL_VALUE_DEFINE_IN_HEADER( BOX_UPP_CHAT_DRAW_DYANMIC_POKE_RIGHT_IN_FRAME_LEFT ), 
              GFL_VALUE_DEFINE_IN_HEADER( BOX_UPP_CHAT_DRAW_DYANMIC_POKE_RIGHT_IN_FRAME_TOP ), 
              GFL_VALUE_DEFINE_IN_HEADER( BOX_UPP_CHAT_DRAW_DYANMIC_POKE_RIGHT_IN_FRAME_RIGHT ), 
              GFL_VALUE_DEFINE_IN_HEADER( BOX_UPP_CHAT_DRAW_DYANMIC_POKE_RIGHT_IN_FRAME_BOTTOM ) );

              gfl2::math::Vector3 pos( 0, 0, 0 );
              pOtherPlayerObj->m_pPokeSimpleModelInFrame->SetPos( pos );
            }

            //  チャットテキストペイン取得
            pOtherPlayerObj->m_pChatMesssageTextPane   = pLayoutWork->GetTextBoxPane( PANENAME_BOX_TRD_UPP_000_PANE_TEXTCOMMENT_00 );
            GFL_ASSERT( pOtherPlayerObj->m_pChatMesssageTextPane );

            //  ポケモンレベル値テキストペイン取得
            pOtherPlayerObj->m_pPokeLvNumTextPane      = pLayoutWork->GetTextBoxPane( PANENAME_BOX_TRD_UPP_000_PANE_TEXT_999_00 );
            GFL_ASSERT( pOtherPlayerObj->m_pPokeLvNumTextPane );

            //  ポケモンレベル値のタイトル
            {
              pOtherPlayerObj->m_pPokeLvTitleTextPane      = pLayoutWork->GetTextBoxPane( PANENAME_BOX_TRD_UPP_000_PANE_TEXT_LV_00 );
              GFL_ASSERT( pOtherPlayerObj->m_pPokeLvTitleTextPane );
              pG2DUtil->SetTextBoxPaneString( pOtherPlayerObj->m_pPokeLvTitleTextPane, PARAM_LEVEL_00 );
            }

            //  ポケモン名のテキストペイン取得
            pOtherPlayerObj->m_pPokeNameTextPane       = pLayoutWork->GetTextBoxPane( PANENAME_BOX_TRD_UPP_000_PANE_TEXTNAME_00 );
            GFL_ASSERT( pOtherPlayerObj->m_pPokeNameTextPane );
            
            //  ポケモンの所持アイテムアイコンペイン取得
            pOtherPlayerObj->m_pHaveItemIconPane       = pLayoutWork->GetPane( PANENAME_BOX_TRD_UPP_000_PANE_ICONITEM_00 );
            GFL_ASSERT( pOtherPlayerObj->m_pHaveItemIconPane );

            //  ポケモンの所持アイテム名のテキストペイン取得
            pOtherPlayerObj->m_pHaveItemTextPane       = pLayoutWork->GetTextBoxPane( PANENAME_BOX_TRD_UPP_000_PANE_TEXTITEM_00 );
            GFL_ASSERT( pOtherPlayerObj->m_pHaveItemTextPane );

            //  ポケモンの性別アイコンアニメ・ペイン取得
            {
              pOtherPlayerObj->m_pSexIconPane  = pLayoutWork->GetPane( PANENAME_BOX_TRD_UPP_000_PANE_GENDER_00 );
              GFL_ASSERT( pOtherPlayerObj->m_pSexIconPane );

              pOtherPlayerObj->m_sexMaleIconAnimeId    = LA_TRDUPP_BOX_TRD_UPP_000__GENDER_00_ICON_MALE;
              pOtherPlayerObj->m_sexFemaleIconAnimeId  = LA_TRDUPP_BOX_TRD_UPP_000__GENDER_00_ICON_FEMALE;
            }

            //  トレーナーアイコン画像ペイン取得
            pOtherPlayerObj->m_pTrainerIconPane        = pLayoutWork->GetPicturePane( PANENAME_BOX_TRD_UPP_000_PANE_PLAYERPHOTO_01 );
            GFL_ASSERT( pOtherPlayerObj->m_pTrainerIconPane );
            pG2DUtil->SetPaneVisible( pOtherPlayerObj->m_pTrainerIconPane, false );

            pOtherPlayerObj->m_pTrainerIconFramePane   = pLayoutWork->GetPane( PANENAME_BOX_TRD_UPP_000_PANE_WINDOW_PLAYER_FRAME_01 );
            GFL_ASSERT( pOtherPlayerObj->m_pTrainerIconFramePane );

            m_pPlayerObjList[ PLYAER_TYPE_OTHER ]  = pOtherPlayerObj;
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
  bool BoxUppChatDraw::EndFunc()
  {
    if( m_bInit == false )
    {
      return  true;
    }

    enum
    {
      SEQ_POKE_MODE_DELETE  = 0,
      SEQ_POKE_MODE_DELETE_WAIT,
      SEQ_SYS_POKE_DELETE,
      SEQ_SYS_POKE_DELETE_WAIT,
      SEQ_TRAINER_ICON_DELETE,
      SEQ_TRAINER_ICON_DELETE_WAIT,
      SEQ_END
    };

    if( m_endSeq == SEQ_POKE_MODE_DELETE )
    {
      PlayerObj*  pPlayerObj  = NULL;
      u32 loopNum = GFL_NELEMS( m_pPlayerObjList );
      for( u32 i = 0; i < loopNum; ++i )
      {
        pPlayerObj  = m_pPlayerObjList[ i ];
        {
          m_pRenderinManager->RemoveDrawEnv( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, pPlayerObj->m_pPokeInFrameDrawEnvNode );
          GFL_SAFE_DELETE( pPlayerObj->m_pPokeInFrameDrawEnvNode );
        }

        pPlayerObj->m_pPokeSimpleModelInFrame->Terminate();
      }

      ++m_endSeq;
    }

    if( m_endSeq == SEQ_POKE_MODE_DELETE_WAIT )
    {
      bool  bNextSeq  = true;

      u32 loopNum = GFL_NELEMS( m_pPlayerObjList );
      for( u32 i = 0; i < loopNum; ++i )
      {
        if( m_pPlayerObjList[ i ]->m_pPokeSimpleModelInFrame->IsDelete() == false )
        {
          bNextSeq  = false;
        }
      }

      if( bNextSeq == true )
      {
        ++m_endSeq;
      }
    }

    if( m_endSeq == SEQ_SYS_POKE_DELETE )
    {
      //  ポケモンモデルシステムの後処理
      {
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

    if( m_endSeq == SEQ_TRAINER_ICON_DELETE )
    {
      m_pTrainerIconRendering->Terminate();
      ++m_endSeq;
    }

    if( m_endSeq == SEQ_TRAINER_ICON_DELETE_WAIT )
    {
      if( m_pTrainerIconRendering->IsDelete() == true )
      {
        ++m_endSeq;
      }
    }

    if( m_endSeq == SEQ_END )
    {
      GFL_UNLOAD_HEADER_HASH(NIJI_DEBUG_BOX_UPP_CHAT_DRAW_HEADER_FILEPATH);

      {
        u32 loopNum = GFL_NELEMS( m_pPlayerObjList );
        for( u32 i = 0; i < loopNum; ++i )
        {
          GFL_SAFE_DELETE( m_pPlayerObjList[ i ] );
        }
      }

      GFL_SAFE_DELETE( m_pTrainerIconRendering );
      GFL_SAFE_DELETE( m_pPokeModelSys );
      GFL_SAFE_DELETE( m_pWordSet );

      UIView::Delete2D();
      
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
  void BoxUppChatDraw::Open()
  {
    GFL_ASSERT( m_bInit == true );

    UIView::SetVisible( true );

    {
      bool  bModelVisible = true;
      u32 loopNum = GFL_NELEMS( m_pPlayerObjList );
      for( u32 i = 0; i < loopNum; ++i )
      {
        bModelVisible = true;

        //  プレイヤー側のデータで持っている有効フラグを参照してモデル表示設定をする
        if( m_pPlayerObjList[ i ]->m_bVisible == false )
        {
          bModelVisible = false;
        }

        m_pPlayerObjList[ i ]->m_pPokeSimpleModelInFrame->SetVisible( bModelVisible );
      }
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  画面を閉じる
  */
  //--------------------------------------------------------------------------------------------
  void BoxUppChatDraw::Close()
  {
    GFL_ASSERT( m_bInit == true );

    UIView::SetVisible( false );

    {
      u32 loopNum = GFL_NELEMS( m_pPlayerObjList );
      for( u32 i = 0; i < loopNum; ++i )
      {
        //  UIViewの表示フラグとポケモン表示フラグを同期させる
        m_pPlayerObjList[ i ]->m_pPokeSimpleModelInFrame->SetVisible( UIView::IsVisible() );
      }
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      指定プレイヤーが参照するポケモンパラメータを指定
  * @param[in]  type:       プレイヤーを指定(自身か相手か)
  * @param[in]  pPokeParam: 参照するポケモンパラメータを指定
  * @note
  *   この関数が呼ばれた段階で表示するポケモンモデルのロードをする
  */
  //--------------------------------------------------------------------------------------------
  void BoxUppChatDraw::SetPokeParam( const PlayerType type, const pml::pokepara::PokemonParam* pPokeParam )
  {
    GFL_ASSERT( m_bInit == true );
    GFL_ASSERT( type < GFL_NELEMS( m_pPlayerObjList ) );
    GFL_ASSERT( m_pPlayerObjList[ type ] );

    //  パラメータアクセスを高速モードに
    bool  bParamFastMode  = pPokeParam->StartFastMode();

    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    PlayerObj*  pAcitvePlayerObj  = m_pPlayerObjList[ type ];

    PokeTool::SimpleParam param;

    // @fix GFNMCat[820] モデル表示にポケモンパラメータ指定をしていなかったので指定するように修正。また卵の時の専用表示を追加
     
    //  ポケモンのモデルロード開始！
    {
      param.formNo  = pPokeParam->GetFormNo();
      param.isEgg   = pPokeParam->IsEgg( pml::pokepara::CHECK_BOTH_EGG );
      param.monsNo  = pPokeParam->GetMonsNo();
      param.perRand = pPokeParam->GetPersonalRnd();
      param.sex     = pPokeParam->GetSex();
      param.isRare  = pPokeParam->IsRare();

      pAcitvePlayerObj->m_pPokeSimpleModelInFrame->StartLoad( param );
    }

    //  ポケモン名の表示
    {
      // @fix GFNMCat[1169] タマゴの時はニックネームに名前が入っているので、そのまま使う
      pG2DUtil->SetRegisterPokeNickName( 0, pPokeParam );
      pG2DUtil->SetTextBoxPaneStringExpand( pAcitvePlayerObj->m_pPokeNameTextPane, PARAM_NAME );
    }

    if( param.isEgg == true )
    {
      //  卵の時の表示
      pG2DUtil->SetPaneVisible( pAcitvePlayerObj->m_pSexIconPane, false );
      pG2DUtil->SetPaneVisible( pAcitvePlayerObj->m_pPokeLvNumTextPane, false );
      pG2DUtil->SetPaneVisible( pAcitvePlayerObj->m_pHaveItemIconPane, false );
      pG2DUtil->SetPaneVisible( pAcitvePlayerObj->m_pHaveItemTextPane, false );
      pG2DUtil->SetPaneVisible( pAcitvePlayerObj->m_pPokeLvTitleTextPane, false );
    }
    else
    {
      //  性別アイコン設定
      {
        // @fix GFNMCat[1378] 性別表示時のパラメータ取得方法が間違っていました。
        pml::Sex sex = PokeTool::GetDrawSex( pPokeParam );

        pG2DUtil->StopAnime( LYT_WORK_NORMAL, pAcitvePlayerObj->m_sexMaleIconAnimeId );
        pG2DUtil->StopAnime( LYT_WORK_NORMAL, pAcitvePlayerObj->m_sexFemaleIconAnimeId );

        if( sex == pml::SEX_MALE )
        {
          //  男
          pG2DUtil->StartAnime( LYT_WORK_NORMAL, pAcitvePlayerObj->m_sexMaleIconAnimeId );
          pG2DUtil->SetPaneVisible( pAcitvePlayerObj->m_pSexIconPane, true );
        }
        else
        if( sex == pml::SEX_FEMALE )
        {
          //  女
          pG2DUtil->StartAnime( LYT_WORK_NORMAL, pAcitvePlayerObj->m_sexFemaleIconAnimeId );
          pG2DUtil->SetPaneVisible( pAcitvePlayerObj->m_pSexIconPane, true );
        }
        else
        {
          //  それ以外
          pG2DUtil->SetPaneVisible( pAcitvePlayerObj->m_pSexIconPane, false );
        }
      }

      //  ポケモンのレベル値の表示
      {
        pG2DUtil->SetPaneVisible( pAcitvePlayerObj->m_pPokeLvTitleTextPane, true );
        pG2DUtil->SetPaneVisible( pAcitvePlayerObj->m_pPokeLvNumTextPane, true );
        pG2DUtil->SetTextBoxPaneNumber( pAcitvePlayerObj->m_pPokeLvNumTextPane, PARAM__VALUE_3, pPokeParam->GetLevel(), 3 );
      }

      //  ポケモン所持アイテム表示
      {
        u16 haveItemNo  = pPokeParam->GetItem();
        if( haveItemNo != ITEM_DUMMY_DATA )
        {
          //  アイテム所持しているので表示
          pG2DUtil->SetPaneVisible( pAcitvePlayerObj->m_pHaveItemIconPane, true );
          pG2DUtil->SetPaneVisible( pAcitvePlayerObj->m_pHaveItemTextPane, true );

          //  アイテム名表示
          pG2DUtil->SetRegisterItemName( 0, haveItemNo );
          pG2DUtil->SetTextBoxPaneStringExpand( pAcitvePlayerObj->m_pHaveItemTextPane, PARAM_ITEM_01 );
        }
        else
        {
          //  アイテム所持していないので非表示
          pG2DUtil->SetPaneVisible( pAcitvePlayerObj->m_pHaveItemIconPane, false );
          pG2DUtil->SetPaneVisible( pAcitvePlayerObj->m_pHaveItemTextPane, false );
        }
      }
    }

    //  パラメータアクセスの高速モードを解除
    pPokeParam->EndFastMode( bParamFastMode );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      表示するトレーナーパラメータを指定
  * @param[in]  type:       プレイヤーを指定(自身か相手か)
  * @param[in]  rIconData   プレイヤーのアイコンパラメータ
  * @note
  *   この関数が呼ばれた段階で表示するデータのロードをする
  */
  //--------------------------------------------------------------------------------------------
  void BoxUppChatDraw::SetTrainerParam( const PlayerType type, const app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA& rIconData )
  {
    GFL_ASSERT( m_bInit == true );
    GFL_ASSERT( type < GFL_NELEMS( m_pPlayerObjList ) );
    GFL_ASSERT( m_pPlayerObjList[ type ] );

    app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA  iconData  = rIconData;
    iconData.pReplacePane = m_pPlayerObjList[ type ]->m_pTrainerIconPane;
    iconData.pInLoadVisibleChangePane = m_pPlayerObjList[ type ]->m_pTrainerIconFramePane;

    //  内部でパラメータをコピーしているの参照渡しでも平気
    m_pTrainerIconRendering->SetIconObjectData( (u32)type, iconData );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      チャットウィンドウに表示するメッセージを指定
  * @param[in]  type:     プレイヤーを指定(自身か相手か)
  * @param[in]  pStrBuf:  チャットウィンドウに表示するテキスト
  */
  //--------------------------------------------------------------------------------------------
  void BoxUppChatDraw::SetChatWindowMessageText( const PlayerType type, gfl2::str::StrBuf* pStrBuf )
  {
    GFL_ASSERT( m_bInit == true );
    GFL_ASSERT( type < GFL_NELEMS( m_pPlayerObjList ) );
    GFL_ASSERT( m_pPlayerObjList[ type ] );

    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    pG2DUtil->SetTextBoxPaneString( m_pPlayerObjList[ type ]->m_pChatMesssageTextPane, pStrBuf );

    //  新しくコメントを設定したので特定アニメを再生
    _PlayAnimeInComment( type );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      チャットウィンドウに表示するポケモンの名前を指定
  * @param[in]  wordsetno:  ワードセット番号
  * @param[in]  monsno:  モンスター番号
  */
  //--------------------------------------------------------------------------------------------
  void BoxUppChatDraw::ExpandPokemon(int wordsetno, int monsno )
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    pG2DUtil->SetRegisterMonsNameNo(wordsetno, (MonsNo)monsno);
  }
  //--------------------------------------------------------------------------------------------
  /**
  * @brief      チャットウィンドウに表示するポケモンの名前を指定
  * @param[in]  wordsetno:  ワードセット番号
  * @param[in]  pp:  モンスター構造体
  */
  //--------------------------------------------------------------------------------------------
  void BoxUppChatDraw::ExpandPokemon(int wordsetno,  const pml::pokepara::CoreParam * pp  )
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    //  ここはニックネームにしないとアウト
    pG2DUtil->SetRegisterPokeNickName(wordsetno, pp);
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      チャットウィンドウに表示するメッセージを指定
  * @param[in]  type:     プレイヤーを指定(自身か相手か)
  * @param[in]  strID:  チャットウィンドウに表示するテキスト
  */
  //--------------------------------------------------------------------------------------------
  void BoxUppChatDraw::SetChatWindowMessageID( const PlayerType type, u32 strID )
  {
    GFL_ASSERT( m_bInit == true );
    GFL_ASSERT( type < GFL_NELEMS( m_pPlayerObjList ) );
    GFL_ASSERT( m_pPlayerObjList[ type ] );

    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    pG2DUtil->SetTextBoxPaneStringExpand( m_pPlayerObjList[ type ]->m_pChatMesssageTextPane, strID );

    //  新しくコメントを設定したので特定アニメを再生
    _PlayAnimeInComment( type );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  更新処理
  *         子ウインドウについても自動的にコールする(子→親の順）
  */
  //--------------------------------------------------------------------------------------------
  void BoxUppChatDraw::Update()
  {
    if( m_bInit == false )
    {
      return;
    }

    m_pTrainerIconRendering->Update();

    {
      u32 loopNum = GFL_NELEMS( m_pPlayerObjList );
      for( u32 i = 0; i < loopNum; ++i )
      {
        m_pPlayerObjList[ i ]->m_pPokeSimpleModelInFrame->Update();
      }
    }

    m_pPokeModelSys->Update();

    UIView::Update();
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  描画処理
  *         子ウインドウについても自動的にコールする(親→子の順）
  */
  //--------------------------------------------------------------------------------------------
  void BoxUppChatDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    if( m_bInit == false )
    {
      return;
    }

    if( ( displayNo == gfl2::gfx::CtrDisplayNo::LEFT ) ||
        ( displayNo == gfl2::gfx::CtrDisplayNo::RIGHT ) )
    {
      app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
      for( s32 i = 0; i < LYT_WORK_MAX; ++i )
      {
        pG2DUtil->AddDrawableLytWkForOneFrame( m_pRenderinManager, displayNo, i );
      }
    }

    m_pTrainerIconRendering->Draw( displayNo );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  2D初期化処理
  * @note
  *     レイアウトデータから描画に必要な生成をしている
  */
  //--------------------------------------------------------------------------------------------
  void BoxUppChatDraw::_Init2D()
  {
    // アーカイブIDテーブル
    const ::app::util::G2DUtil::LytResData aResTbl[] =
    {
      {  m_pLytBinData, 32, ::app::util::G2DUtil::ATTACH_NONE },
    };

    //  作成するレイアウトワーク
    const ::app::util::G2DUtil::LytwkData aSetupData[ LYT_WORK_MAX ] =
    {
      {
        0,
        LYTRES_TRDUPP_BOX_TRD_UPP_000_BFLYT,
        LA_TRDUPP_BOX_TRD_UPP_000___NUM,
        m_pLytBinData,
        true,
        ::app::util::G2DUtil::SETUP_UPPER,
        true
      }
    };

    UIView::Create2D( m_pUtilHeap, NULL, 32, aResTbl, GFL_NELEMS(aResTbl), aSetupData, GFL_NELEMS(aSetupData), m_pBoxMsgData, m_pWordSet );

    UIView::SetVisible( false );
  }

  //--------------------------------------------------------------------------------------------
  /**
   * @brief      ポケモンステータスのONOFF
   * @param[in]  type:   プレイヤーを指定(自身か相手か)
   * @param[in]  strID:  チャットウィンドウに表示するテキスト
   */
  //--------------------------------------------------------------------------------------------
   void BoxUppChatDraw::SetPokemonStateOnOff( const PlayerType type,bool bvisible )
  {
    int pane[]={PANENAME_BOX_TRD_UPP_000_PANE_NULL_VISIBLE_01, PANENAME_BOX_TRD_UPP_000_PANE_NULL_VISIBLE_00};
    GetG2DUtil()->SetPaneVisible(LYT_WORK_NORMAL, pane[type], bvisible);

    if( UIView::IsVisible() == false )
    {
      //  UIView全体がvisible = false 状態ではモデル表示設定は向こうとなる
    }
    else
    {
      m_pPlayerObjList[ type ]->m_pPokeSimpleModelInFrame->SetVisible( bvisible );
    }

    //  有効状態を保存しておく、画面をOpen関数で開いた時にモデル表示・非表示をするのに必要
    m_pPlayerObjList[ type ]->m_bVisible  = bvisible;
  }

  //  コメントアニメ再生
  void BoxUppChatDraw::_PlayAnimeInComment( const PlayerType type )
  {
    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    if(type == PLYAER_TYPE_SELF)
    {
      pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_TRDUPP_BOX_TRD_UPP_000_COMMENT_IN_00 );
    }
    else
    {
      pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_TRDUPP_BOX_TRD_UPP_000_COMMENT_IN );
    }

  }

//  ここからは PlayerObjクラスの定義
  //--------------------------------------------------------------------------------------------
  /**
  * @brief      コンストラクタ
  * @param[in]  pUtilHeap:          基本となるHeap
  * @param[in]  pPokeModelSys:      モデル描画に必要な管理
  * @param[in]  pRenderingManager:  描画レンダリング
  * @param[in]  layerNo:            ポケモンモデルの描画レイヤー(0 or 1)
  * @note
  *   内部でポケモンモデルのロードに必要なローカルメモリを確保している。
  *   最低でも5MByteのサイズがヒープに必要なので注意
  */
  //--------------------------------------------------------------------------------------------
  BoxUppChatDraw::PlayerObj::PlayerObj( app::util::Heap* pUtilHeap, PokeTool::PokeModelSystem* pPokeModelSys, app::util::AppRenderingManager* pRenderingManager, u32 layerNo )
  {
    GFL_ASSERT( pUtilHeap );
    GFL_ASSERT( pPokeModelSys );
    GFL_ASSERT( pRenderingManager );

    _Clear();

    //  ポケモンモデル作成
    {
      app::tool::PokeSimpleModelInFrame::INIT_PARAM  initParam;

      //  必要なパラメータを設定
      {
        PokeTool::PokeModel::SetupOption* pOption = &initParam.setupOpt;
        pOption->dataHeap = pUtilHeap->GetDeviceHeap();
        pOption->workHeap = pUtilHeap->GetDeviceHeap()->GetLowerHandle();
        // @fix GFNMCat[3562] ゼルネアス、ホワイトキュレム、ブラックキュレムの場合はモーションをかわいがりに
        pOption->animeType  = PokeTool::MODEL_ANIMETYPE_APP; //アプリ用ID
      }

      initParam.pPokeModelSys       = pPokeModelSys;
      initParam.pRenderingManager   = pRenderingManager;
      initParam.renderPlace         = app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY;
      initParam.layerNo             = layerNo;

      m_pPokeSimpleModelInFrame  = GFL_NEW( pUtilHeap->GetDeviceHeap() ) app::tool::PokeSimpleModelInFrame( initParam );
      m_pPokeSimpleModelInFrame->InitInFrame( System::Camera::CModelInFrame::SCREEN_UPPER );
    }
  }

  BoxUppChatDraw::PlayerObj::~PlayerObj()
  {
    GFL_SAFE_DELETE( m_pPokeSimpleModelInFrame );
  }

GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )
