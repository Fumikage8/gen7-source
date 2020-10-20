#if PM_DEBUG
//=============================================================================
/**
 * @file    d_uchida.h
 * @brief   内田用ランチャーリスト処理
 * @author  Yuto Uchida
 * @date    2015.02.20
 */
//=============================================================================
#include <Debug/Launcher/include/LauncherProc.h>
#include <Debug/Launcher/include/LauncherListData.h>

#include <Battle/include/battle_SetupParam.h>
#include "NetStatic/NetEvent/include/TeamSelectEvent.h"
#include "NetApp/TeamSelect/include/TeamSelectEventAppParam.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(ExtSavedata)
  class BattleRecorderSaveData;
GFL_NAMESPACE_END(ExtSavedata)

GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(startmenu)

//-----------------------------------------------------------------------------
/**
 * @class   LauncherUchida
 * @brief   内田用ランチャークラス
 * @date    2015.02.20
 */
//-----------------------------------------------------------------------------
class LauncherUchida
  : public launcher::ListData
{
private:
  static const u32 ItemList[];  //!< 項目文字列データテーブル

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //-----------------------------------------------------------------------------
  LauncherUchida(void);
  virtual ~LauncherUchida();

  //-----------------------------------------------------------------------------
  /**
   * @func    SelectListener
   * @brief   決定時のリスナー
   * @date    2015.02.20
   *
   * @param   pos       カーソル位置 ( キャンセル時はCURSOR_POS_CANCEL )
   * @param   launcher  呼び出し元のランチャークラス
   *
   * @return  ランチャーへのリクエストID
   */
  //-----------------------------------------------------------------------------
  virtual launcher::ListData::RequestID SelectListener( u32 pos, launcher::LauncherProc * launcher );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetArcDataID
   * @brief   アークデータID取得
   * @date    2015.02.20
   *
   * @return  アークデータID
   */
  //-----------------------------------------------------------------------------
  virtual gfl2::fs::ArcFile::ARCDATID GetArcDataID(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetListTable
   * @brief   項目文字列データテーブル取得
   * @date    2015.02.20
   *
   * @return  文字列データテーブル
   */
  //-----------------------------------------------------------------------------
  virtual const u32 * GetListTable(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetListTable
   * @brief   項目文字列データテーブルの要素数取得
   * @date    2015.02.20
   *
   * @return  文字列データテーブルの要素数
   */
  //-----------------------------------------------------------------------------
  virtual u32 GetListTableSize(void);

private:
  BTL_FIELD_SITUATION m_sit;

  pml::PokeParty* m_pEnemyParty;
  pml::pokepara::PokemonParam* m_pEnemyPokeParam[ 6 ];

  ExtSavedata::BattleRecorderSaveData*  m_pRecorderSaveData;
  Regulation*                           m_pBtlRegulation;

  BATTLE_SETUP_PARAM                    m_btlSetupParam;

  NetApp::TeamSelect::EVENT_APP_PARAM   m_teamSelectAppParam;
};

GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)

#endif  // PM_DEBUG
