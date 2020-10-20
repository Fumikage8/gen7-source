#ifndef __D_FUKUSHIMA_H__
#define __D_FUKUSHIMA_H__
#pragma once
//=============================================================================
/**
 * @file    d_fukushima.h
 * @brief   ランチャーリスト処理
 * @author  fukushima_yuya
 * @date    2015.03.02
 */
//=============================================================================
#if PM_DEBUG

#include <Debug/Launcher/include/LauncherProc.h>
#include <Debug/Launcher/include/LauncherListData.h>

#include <App/PokeList/include/PokeListAppParam.h>
#include <App/PokeList/include/PokeListSetupParam.h>
#include <App/Status/include/StatusAppParam.h>
#include <App/Bag/include/BagAppParam.h>
#include <App/StrInput/include/StrInputAppParam.h>
#include <App/WazaOshie/include/WazaOshieAppParam.h>
#include <App/TitleMenu/include/TitleMenuAppParam.h>
#include <App/LangSelect/include/LangSelectAppParam.h>
#include <App/BoxSearch/include/BoxSearchAppParam.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)
class TimeLimit;
GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)


GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(startmenu)

//-----------------------------------------------------------------------------
/**
 * @class   DebugPartsLauncher
 * @brief   パーツのデバッグをするランチャークラス
 */
//-----------------------------------------------------------------------------
class LauncherFukushima : public debug::launcher::ListData
{
private:
  static const u32 ItemList[];  //!< 項目文字列データテーブル

public:
  // 破棄
  static void Delete( void );

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief    コンストラクタ・デストラクタ
   */
  //-----------------------------------------------------------------------------
  LauncherFukushima( void );
  virtual ~LauncherFukushima( void );

  //-----------------------------------------------------------------------------
  /**
   * @func    SelectListener
   * @brief   決定時のリスナー
   * @date    2015.02.18
   *
   * @param   pos       カーソル位置 ( キャンセル時はCURSOR_POS_CANCEL )
   * @param   launcher  呼び出し元のランチャークラス
   *
   * @return  ランチャーへのリクエストID
   */
  //-----------------------------------------------------------------------------
  virtual debug::launcher::ListData::RequestID SelectListener( u32 pos, debug::launcher::LauncherProc * launcher );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetArcDataID
   * @brief   アークデータID取得
   * @date    2015.02.18
   *
   * @return  アークデータID
   */
  //-----------------------------------------------------------------------------
  virtual gfl2::fs::ArcFile::ARCDATID GetArcDataID(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetListTable
   * @brief   項目文字列データテーブル取得
   * @date    2015.02.18
   *
   * @return  文字列データテーブル
   */
  //-----------------------------------------------------------------------------
  virtual const u32 * GetListTable(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetListTable
   * @brief   項目文字列データテーブルの要素数取得
   * @date    2015.02.18
   *
   * @return  文字列データテーブルの要素数
   */
  //-----------------------------------------------------------------------------
  virtual u32 GetListTableSize(void);

private:
  //-----------------------------------------------------------------------------
  /*
   * @brief   フェード処理
   */
  //-----------------------------------------------------------------------------
  void FadeOut( void );

  //-----------------------------------------------------------------------------
  /*
   * @brief   ポケモンリスト
   * @param   pos   選択した項目インデックス
   */
  //-----------------------------------------------------------------------------
  launcher::ListData::RequestID CallPokeProc( u32 pos );

  //-----------------------------------------------------------------------------
  /*
   * @brief   技教え・技忘れ
   * @param   pos   選択した項目インデックス
   */
  //-----------------------------------------------------------------------------
  launcher::ListData::RequestID CallWazaOshieProc( u32 pos );

  //-----------------------------------------------------------------------------
  /*
   * @brief   タイトルメニュー
   * @param   pos   選択した項目インデックス
   */
  //-----------------------------------------------------------------------------
  launcher::ListData::RequestID CallTitleMenuProc( u32 pos );

  //-----------------------------------------------------------------------------
  /*
   * @brief   言語選択
   * @param   pos   選択した項目インデックス
   */
  //-----------------------------------------------------------------------------
  launcher::ListData::RequestID CallLangSelectProc( u32 pos );

  //-----------------------------------------------------------------------------
  /*
   * @brief   ボックス検索
   * @param   pos   選択した項目インデックス
   */
  //-----------------------------------------------------------------------------
  launcher::ListData::RequestID CallBoxSearchProc( u32 pos );

  //-----------------------------------------------------------------------------
  /*
   * @brief   イベントリスト
   * @param   pos   選択した項目インデックス
   */
  //-----------------------------------------------------------------------------
  launcher::ListData::RequestID CallEventListProc( u32 pos );

  //-----------------------------------------------------------------------------
  /*
   * @brief   容姿・性別選択
   * @param   pos   選択した項目インデックス
   */
  //-----------------------------------------------------------------------------
  launcher::ListData::RequestID CallHeroSelectProc( u32 pos );

  //-----------------------------------------------------------------------------
  /*
   * @brief   ファインダーメニュー
   * @param   pos   選択した項目インデックス
   */
  //-----------------------------------------------------------------------------
  launcher::ListData::RequestID CallFinderMenuProc( u32 pos );

  //-----------------------------------------------------------------------------
  /*
   * @brief   文字入力
   * @param   pos   選択した項目インデックス
   */
  //-----------------------------------------------------------------------------
  launcher::ListData::RequestID CallStrInputProc( u32 pos );

  //-----------------------------------------------------------------------------
  /*
   * @brief   テスト環境
   * @param   pos   選択した項目インデックス
   */
  //-----------------------------------------------------------------------------
  launcher::ListData::RequestID CallTestProc( u32 pos );

  //-----------------------------------------------------------------------------
  /*
  * @brief   バトルフォント表示テスト
  * @param   pos   選択した項目インデックス
  */
  //-----------------------------------------------------------------------------
  launcher::ListData::RequestID CallBattleFontTestProc( u32 pos );

  //-----------------------------------------------------------------------------
  /*
  * @brief   ImageDBテスト
  * @param   pos   選択した項目インデックス
  */
  //-----------------------------------------------------------------------------
  launcher::ListData::RequestID CallImageDBTestProc( u32 pos );

  //-----------------------------------------------------------------------------
  /*
  * @brief   ファインダースタジオデコテスト
  * @param   pos   選択した項目インデックス
  */
  //-----------------------------------------------------------------------------
  launcher::ListData::RequestID CallFSDecoTestProc( u32 pos );

  //-----------------------------------------------------------------------------
  /*
  * @brief   ファインダースタジオ：写真撮影
  * @param   pos   選択した項目インデックス
  */
  //-----------------------------------------------------------------------------
  launcher::ListData::RequestID CallFinderStudioCaptureProc( u32 pos );

  //-----------------------------------------------------------------------------
  /*
  * @brief   ファインダースタジオ：アルバム
  * @param   pos   選択した項目インデックス
  */
  //-----------------------------------------------------------------------------
  launcher::ListData::RequestID CallFinderStudioAlbumProc( u32 pos );

  //-----------------------------------------------------------------------------
  /*
   * @brief   ポケモンリストの外部設定パラメータの設定
   */
  //-----------------------------------------------------------------------------
  void SetPokeListAppParam( u32 mode );

private:
  //-----------------------------------------------------------------------------
  /*
   * @brief   パーティの初期化
   */
  //-----------------------------------------------------------------------------
  void InitParty( void );

  //-----------------------------------------------------------------------------
  /**
   * @brief   ポケモンの生成
   *
   * @param   party   パーティデータ
   * @param   index   パーティ内のポケモンインデックス
   */
  //-----------------------------------------------------------------------------
  void CreatePokeParam( pml::PokeParty* party, u32 index );

private:
  GameSys::GameManager*           m_pGameManager;

  gfl2::Fade::FadeManager*        m_pFadeManager;

  App::PokeList::CONTEXT_PARAM    m_PokeListContext;

  App::StrInput::APP_PARAM        m_StrInputAppParam;

  App::WazaOshie::APP_PARAM       m_WazaOshieAppParam;

  App::TitleMenu::APP_PARAM       m_TitleMenuAppParam;

  App::LangSelect::APP_PARAM      m_LangSelectAppParam;

  App::BoxSearch::APP_PARAM       m_BoxSearchAppParam;

private:
  static pml::PokeParty*          m_spParty_Myself;

  static pml::PokeParty*          m_spParty_Friend;

  static pml::PokeParty*          m_spParty_Enemy_0;

  static pml::PokeParty*          m_spParty_Enemy_1;

  static App::Tool::TimeLimit*    m_spTimeLimit;
};

GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)

#endif  // PM_DEBUG
#endif  // #ifndef __D_FUKUSHIMA_H__
