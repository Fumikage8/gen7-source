#if PM_DEBUG

#include "Test/UiUnitTest/include/UIBoxChatViewTestProc.h"

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameProcManager.h>

#include <System/include/HeapDefine.h>
#include <Print/include/SystemFont.h>

#include <arc_index/message.gaix>
#include <niji_conv_header/message/msg_btl_app.h>

#include "model/include/gfl2_BaseCamera.h"
#include "GameSys/include/GameData.h"

// 着せ替えのインデックス
#include <niji_conv_header/chara_test/dressup_item_id.h>
#include <niji_conv_header/chara_test/dressup_color_id.h>

// モーションのインデックス
#include <niji_conv_header/battle/chara/p2_base.h>

// arcファイル一覧
#include <arc_def_index/arc_def.h>
#include <arc_index/boxLyt.gaix>

GFL_NAMESPACE_BEGIN(Test)

//------------------------------------------------------------------
/**
  * @brief   PROCの初期化を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
  */
//------------------------------------------------------------------
gfl2::proc::Result UIBoxChatViewTestProc::InitFunc(gfl2::proc::Manager* pManager)
{
  s32 seq = BaseProcess::GetSubSeq();

  gfl2::proc::Result  result  = gfl2::proc::RES_CONTINUE;

  if( UIUnitTestBaseProc::InitFunc( pManager ) == gfl2::proc::RES_CONTINUE )
  {
    return  gfl2::proc::RES_CONTINUE;
  }

  if( seq == 0 )
  {
    _Claer();

    //  上画面の3D描画の下準備
    {
      //  カメラ生成
      app::util::AppRenderingManager::CreateDefaultCamera( mpAppHeap, &m_pPokeModelDrawEnvNode, &m_pPokeModelCamera );
      //  カメラ設定
      GFL_ASSERT( mpAppRenderingManager->IsCreated() == true );
      mpAppRenderingManager->SetCamera( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_pPokeModelCamera );
    }

    //  数字フォントは常駐化(サイズが少ないので問題なし)
    print::SystemFont_CreateNumFont( mpAppHeap->GetSystemHeap(), mpAppHeap->GetDeviceHeap() );
    print::SystemFont_SetLytSystemNumFont();

    m_pFileLoadState  = GFL_NEW( mpAppHeap->GetDeviceHeap() ) app::util::AppUtilFileControlState( ARCID_BOX, GARC_boxLyt_trdupp_applyt_COMP, mpAppHeap->GetDeviceHeap(), true, 128 );
    m_pFileLoadState->Start();

    //  UI画面で使う共通メッセージ
    m_pAppMsgData = GFL_NEW( mpAppHeap->GetDeviceHeap() ) gfl2::str::MsgData( print::GetMessageArcId( System::MSGLANGID_MAX ), GARC_message_box_DAT, mpAppHeap->GetDeviceHeap(), gfl2::str::MsgData::LOAD_FULL );
    GFL_ASSERT( m_pAppMsgData );

    ++seq;
  }
  
  if( seq == 1 )
  {
    m_pFileLoadState->Update();

    if( m_pFileLoadState->IsEnd() == true )
    {
      m_pView = GFL_NEW( mpAppHeap->GetDeviceHeap() ) App::Box::BoxUppChatDraw( m_pFileLoadState->GetLoadDataBuffer(), mpAppRenderingManager, mpAppHeap, m_pAppMsgData );
      ++seq;
    }
  }

  if( seq == 2 )
  {
    if( m_pView->InitFunc() == true )
    {
      m_pView->Open();

      //  自身
      {
        pml::pokepara::PokemonParam param( mpAppHeap->GetDeviceHeap()->GetLowerHandle(), (MonsNo)25, 50, 0 );
        param.SetItem( ITEM_PIKATYUUBIIZU_1 );

        m_pView->SetPokeParam( App::Box::BoxUppChatDraw::PLYAER_TYPE_SELF, &param );

        gfl2::str::StrBuf*  pTmpStrBuf  = GFL_NEW_LOW( mpAppHeap->GetDeviceHeap() ) gfl2::str::StrBuf( L"self", mpAppHeap->GetDeviceHeap()->GetLowerHandle() );
        m_pView->SetChatWindowMessageText( App::Box::BoxUppChatDraw::PLYAER_TYPE_SELF, pTmpStrBuf );

        GFL_SAFE_DELETE( pTmpStrBuf );

        {
          app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA  iconData;
          //  ステータスからアイコン表示に必要なデータをコンバート
          {
            Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
            iconData.ConvFromMyStatus( *pMyStatus );
          }

          m_pView->SetTrainerParam( App::Box::BoxUppChatDraw::PLYAER_TYPE_SELF, iconData );
        }
      }

      //  相手
      {
        pml::pokepara::PokemonParam param( mpAppHeap->GetDeviceHeap()->GetLowerHandle(), (MonsNo)30, 100, 0 );
        param.SetItem( ITEM_PIKATYUUBIIZU_1 );

        m_pView->SetPokeParam( App::Box::BoxUppChatDraw::PLYAER_TYPE_OTHER, &param );

        gfl2::str::StrBuf*  pTmpStrBuf  = GFL_NEW_LOW( mpAppHeap->GetDeviceHeap() ) gfl2::str::StrBuf( L"other", mpAppHeap->GetDeviceHeap()->GetLowerHandle() );
        m_pView->SetChatWindowMessageText( App::Box::BoxUppChatDraw::PLYAER_TYPE_OTHER, pTmpStrBuf );

        GFL_SAFE_DELETE( pTmpStrBuf );

        {
          app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA  iconData;
          //  ステータスからアイコン表示に必要なデータをコンバート
          {
            Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
            iconData.ConvFromMyStatus( *pMyStatus );
          }

          m_pView->SetTrainerParam( App::Box::BoxUppChatDraw::PLYAER_TYPE_OTHER, iconData );
        }
      }
      
      result  = gfl2::proc::RES_FINISH;
    }
  }

  BaseProcess::SetSubSeq( seq );

  return result;
}

//  テスト用のフレーム作成
applib::frame::CellSuper* UIBoxChatViewTestProc::OnCreateFrame( applib::frame::Manager* pFrameManager, GameSys::GameManager* pGameManager )
{
  return NULL;
}

//------------------------------------------------------------------
/**
  * @brief	  PROCのメイン更新を行う関数
  * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
  */
//------------------------------------------------------------------
gfl2::proc::Result UIBoxChatViewTestProc::UpdateFunc(gfl2::proc::Manager* pManager)
{
  UIUnitTestBaseProc::UpdateFunc( pManager );

  GFL_UNUSED( pManager );

  m_pView->UpdateTree();

  gfl2::ui::DeviceManager*                pDeviceManager  = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  gfl2::ui::Button*                       pButton         = pDeviceManager->GetButton(0);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    return  gfl2::proc::RES_FINISH;
  }
  else
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    //  ロード連続チェック

    static u32 monsNo = 25;
    ++monsNo;
    if( MONSNO_MAX <= monsNo )
    {
      monsNo  = 1;
    }

    //  自身
    {
      pml::pokepara::PokemonParam param( mpAppHeap->GetDeviceHeap()->GetLowerHandle(), (MonsNo)monsNo, 100, 0 );
      param.SetItem( ITEM_PIKATYUUBIIZU_1 );

      m_pView->SetPokeParam( App::Box::BoxUppChatDraw::PLYAER_TYPE_SELF, &param );
    }

    //  相手
    {
      pml::pokepara::PokemonParam param( mpAppHeap->GetDeviceHeap()->GetLowerHandle(), (MonsNo)monsNo, 100, 0 );
      param.SetItem( ITEM_PIKATYUUBIIZU_1 );

      m_pView->SetPokeParam( App::Box::BoxUppChatDraw::PLYAER_TYPE_OTHER, &param );
    }
  }
  else
  if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) )
  {
    m_pView->Close();
  }

  return gfl2::proc::RES_CONTINUE;
}

/**
* @brief	PROCの描画処理
* @param pManager   プロセス管理マネージャー
* @param displayNo  描画対象画面
* 登録したPROCによる、フレームバッファへの描画を実行します。
* ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
*/
void UIBoxChatViewTestProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  GFL_ASSERT( m_pView );

  m_pView->DrawTree( displayNo );

  UIUnitTestBaseProc::DrawFunc( pManager, displayNo );
}

//------------------------------------------------------------------
/**
  * @brief   PROCの終了を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
  */
//------------------------------------------------------------------
gfl2::proc::Result UIBoxChatViewTestProc::EndFunc(gfl2::proc::Manager* pManager)
{
  if( m_pView != NULL )
  {
    if( ( m_pView->EndFunc() == true ) && ( m_pView->IsDrawing() == false ) )
    {
      GFL_SAFE_DELETE( m_pView );
      GFL_SAFE_DELETE( m_pFileLoadState );
      GFL_SAFE_DELETE( m_pAppMsgData );

      GFL_SAFE_DELETE( m_pPokeModelDrawEnvNode );

      //  カメラを解除
      mpAppRenderingManager->UnsetCamera( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY );

      //  model/include/gfl2_BaseCamera.h ファイルがインクルードしていないと,
      //  クラス型が不定扱いになりデストラクタが派生元のデストラクタが呼ばれずにメモリ開放処理が走らない
      GFL_SAFE_DELETE( m_pPokeModelCamera );
    }

    return  gfl2::proc::RES_CONTINUE;
  }

  if( UIUnitTestBaseProc::EndFunc( pManager ) == gfl2::proc::RES_CONTINUE )
  {
    return  gfl2::proc::RES_CONTINUE;
  }

  //  ロードしたフォントを開放
  print::SystemFont_ResetDefaultFontInfo();
  print::SystemFont_DeleteNumFont();

  return gfl2::proc::RES_FINISH;
}

GFL_NAMESPACE_END(Test)

#endif // PM_DEBUG
