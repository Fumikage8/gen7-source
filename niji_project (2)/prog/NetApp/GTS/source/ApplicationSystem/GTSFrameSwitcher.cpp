// ============================================================================
/*
 * @file GTSFrameSwitcher.cpp
 * @brief GTSアプリのフレームを切り替えるクラスです。
 * @date 2015.03.26
 */
// ============================================================================
#include "NetApp/GTS/source/ApplicationSystem/GTSFrameSwitcher.h"
#include "NetApp/GTS/source/GTSFrameID.h"
#include "NetApp/GTS/source/GTSFrameResult.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(ApplicationSystem)

NetAppLib::System::FrameID GTSFrameSwitcher::GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult )
{
  NetAppLib::System::FrameID nextFrameID = NetAppLib::System::FRAME_ID_INVALID;

  switch( executeFrameID )
  {
    // ---------------------------------------------------------------------------------
    // GTSアプリを起動した時にGTSの状態を確認するプロセス
    case NetApp::GTS::GTS_FRAME_ID_STATE_CONFIRM:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // ポケモンをアップロードしていない
        case NetApp::GTS::STATE_CONFIRM_RESULT_POKEMON_NOT_UPLOAD:
          nextFrameID = NetApp::GTS::GTS_FRAME_ID_TOP_MENU;
        break;

        // -----------------------------------------------------------------------------
        // ポケモンがアップロードされている(GTSに預けられている状態)
        case NetApp::GTS::STATE_CONFIRM_RESULT_POKEMON_UPLOAD:
          nextFrameID = NetApp::GTS::GTS_FRAME_ID_TOP_MENU;
        break;

        // -----------------------------------------------------------------------------
        // ポケモンの交換が成功した
        case NetApp::GTS::STATE_CONFIRM_RESULT_POKEMON_TRADE_SUCCESS:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;

        // -----------------------------------------------------------------------------
        // ポケモンが戻ってきた
        case NetApp::GTS::STATE_CONFIRM_RESULT_POKEMON_RETURN:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;

        // -----------------------------------------------------------------------------
        // エラーが発生
        case NetApp::GTS::STATE_CONFIRM_RESULT_ERROR:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;
      }
    }
    break;

    // ---------------------------------------------------------------------------------
    // GTSのトップメニューのプロセス
    case NetApp::GTS::GTS_FRAME_ID_TOP_MENU:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 戻る
        case NetApp::GTS::TOP_MENU_RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;

        // -----------------------------------------------------------------------------
        // ポケモンを預ける
        case NetApp::GTS::TOP_MENU_RESULT_UPLOAD_POKEMON:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;

        // -----------------------------------------------------------------------------
        // ポケモンを探す
        case NetApp::GTS::TOP_MENU_RESULT_SEARCH_POKEMON:
          nextFrameID = NetApp::GTS::GTS_FRAME_ID_SEARCH_POKEMON;
        break;

        // -----------------------------------------------------------------------------
        // (預けた)ポケモンを引き取る
        case NetApp::GTS::TOP_MENU_RESULT_DOWNLOAD_MY_POKEMON:
          nextFrameID = NetApp::GTS::GTS_FRAME_ID_DOWNLOAD_MY_POKEMON;
        break;
      }
    }
    break;

    // ---------------------------------------------------------------------------------
    // 探したいポケモンを選択するプロセス
    case NetApp::GTS::GTS_FRAME_ID_SEARCH_POKEMON:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 戻る
        case NetApp::GTS::SEARCH_POKEMON_RESULT_BACK:
          nextFrameID = NetApp::GTS::GTS_FRAME_ID_TOP_MENU;
        break;

        // -----------------------------------------------------------------------------
        // ポケモンの検索に成功
        case NetApp::GTS::SEARCH_POKEMON_RESULT_SUCCESS:
          nextFrameID = NetApp::GTS::GTS_FRAME_ID_TRADE_PERSON_SELECT;
        break;

        // -----------------------------------------------------------------------------
        // エラーが発生
        case NetApp::GTS::SEARCH_POKEMON_RESULT_ERROR:
          nextFrameID = NetApp::GTS::GTS_FRAME_ID_TOP_MENU;
        break;

        // -----------------------------------------------------------------------------
        // ポケモンの名前入力ボタンを押した
        case NetApp::GTS::SEARCH_POKEMON_RESULT_NAME_INPUT:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;
      }
    }
    break;

    // ---------------------------------------------------------------------------------
    // ポケモンを預けるプロセス
    case NetApp::GTS::GTS_FRAME_ID_UPLOAD_POKEMON:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 戻る
        case NetApp::GTS::UPLOAD_POKEMON_RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;

        // -----------------------------------------------------------------------------
        // ポケモンのアップロードに成功
        case NetApp::GTS::UPLOAD_POKEMON_RESULT_SUCCESS:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;

        // -----------------------------------------------------------------------------
        // エラーが発生
        case NetApp::GTS::UPLOAD_POKEMON_RESULT_ERROR:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;

        // -----------------------------------------------------------------------------
        // ポケモンの名前入力ボタンを押した
        case NetApp::GTS::UPLOAD_POKEMON_RESULT_NAME_INPUT:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;
      }
    }
    break;

    // ---------------------------------------------------------------------------------
    // ポケモンを引き取るプロセス
    case NetApp::GTS::GTS_FRAME_ID_DOWNLOAD_MY_POKEMON:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 戻る
        case NetApp::GTS::DOWNLOAD_MY_POKEMON_RESULT_BACK:
          nextFrameID = NetApp::GTS::GTS_FRAME_ID_TOP_MENU;
        break;

        // -----------------------------------------------------------------------------
        // ポケモンの交換が成功した
        case NetApp::GTS::DOWNLOAD_MY_POKEMON_RESULT_POKEMON_TRADE_SUCCESS:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;

        // -----------------------------------------------------------------------------
        // ポケモンが戻ってきた
        case NetApp::GTS::DOWNLOAD_MY_POKEMON_RESULT_POKEMON_RETURN:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;

        // -----------------------------------------------------------------------------
        // エラーが発生
        case NetApp::GTS::DOWNLOAD_MY_POKEMON_RESULT_ERROR:
          nextFrameID = NetApp::GTS::GTS_FRAME_ID_TOP_MENU;
        break;
      }
    }
    break;

    // ---------------------------------------------------------------------------------
    // 交換相手を選択するプロセス
    case NetApp::GTS::GTS_FRAME_ID_TRADE_PERSON_SELECT:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 戻る
        case NetApp::GTS::TRADE_PERSON_SELECT_RESULT_BACK:
          nextFrameID = NetApp::GTS::GTS_FRAME_ID_SEARCH_POKEMON;
        break;

        // -----------------------------------------------------------------------------
        // 交換相手を選択した
        case NetApp::GTS::TRADE_PERSON_SELECT_RESULT_SELECT:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;

        // -----------------------------------------------------------------------------
        // 絞り込み検索ボタンを押した
        case NetApp::GTS::TRADE_PERSON_SELECT_RESULT_REFINE_SEARCH:
          nextFrameID = NetApp::GTS::GTS_FRAME_ID_TRADE_PERSON_REFINE_SEARCH;
        break;

        // -----------------------------------------------------------------------------
        // エラーが発生
        case NetApp::GTS::TRADE_PERSON_SELECT_RESULT_ERROR:
          nextFrameID = NetApp::GTS::GTS_FRAME_ID_SEARCH_POKEMON;
        break;
      }
    }
    break;

    // ---------------------------------------------------------------------------------
    // 交換相手を絞り込むプロセス
    case NetApp::GTS::GTS_FRAME_ID_TRADE_PERSON_REFINE_SEARCH:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 戻る
        case NetApp::GTS::TRADE_PERSON_REFINE_SEARCH_RESULT_BACK:
          nextFrameID = NetApp::GTS::GTS_FRAME_ID_TRADE_PERSON_SELECT;
        break;
      }
    }
    break;

    // ---------------------------------------------------------------------------------
    // ポケモンを交換するプロセス
    case NetApp::GTS::GTS_FRAME_ID_TRADE_POKEMON:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 既にポケモンが他の人と交換されていた
        case NetApp::GTS::TRADE_POKEMON_RESULT_ALREADY_TRADE_POKEMON:
          nextFrameID = NetApp::GTS::GTS_FRAME_ID_SEARCH_POKEMON;
        break;

        // -----------------------------------------------------------------------------
        // ポケモンの交換に成功した
        case NetApp::GTS::TRADE_POKEMON_RESULT_SUCCESS:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;

        // -----------------------------------------------------------------------------
        // エラーが発生した
        case NetApp::GTS::TRADE_POKEMON_RESULT_ERROR:
          nextFrameID = NetApp::GTS::GTS_FRAME_ID_TRADE_PERSON_SELECT;
        break;
      }
    }
    break;

    // ---------------------------------------------------------------------------------
    // ポケモンの名前入力
    case NetApp::GTS::GTS_FRAME_ID_NAME_INPUT:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::GTS::NAME_INPUT_RESULT_TO_SEARCH_POKEMON:
          nextFrameID = NetApp::GTS::GTS_FRAME_ID_SEARCH_POKEMON;
        break;

        // -----------------------------------------------------------------------------
        // 
        case NetApp::GTS::NAME_INPUT_RESULT_TO_UPLOAD_POKEMON:
          nextFrameID = NetApp::GTS::GTS_FRAME_ID_UPLOAD_POKEMON;
        break;
      }
    }
    break;
  }

  return nextFrameID;
}


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
