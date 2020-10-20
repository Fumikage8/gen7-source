#if PM_DEBUG
//=============================================================================
/**
 * @file    d_endo.h
 * @brief   遠藤用ランチャーリスト処理
 * @author  Akira Endo
 * @date    2015.03.02
 */
//=============================================================================
#include <Debug/Launcher/include/LauncherProc.h>
#include <Debug/Launcher/include/LauncherListData.h>
#include "NetStatic/NetAppLib/include/UI/NetAppRegulationDrawInfo.h"


GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(startmenu)

//-----------------------------------------------------------------------------
/**
 * @class   LauncherEndo
 * @brief   遠藤用ランチャークラス
 */
//-----------------------------------------------------------------------------
class LauncherEndo
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
  LauncherEndo(void);
  virtual ~LauncherEndo();

  //-----------------------------------------------------------------------------
  /**
   * @func    SelectListener
   * @brief   決定時のリスナー
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
   *
   * @return  アークデータID
   */
  //-----------------------------------------------------------------------------
  virtual gfl2::fs::ArcFile::ARCDATID GetArcDataID(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetListTable
   * @brief   項目文字列データテーブル取得
   *
   * @return  文字列データテーブル
   */
  //-----------------------------------------------------------------------------
  virtual const u32 * GetListTable(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetListTable
   * @brief   項目文字列データテーブルの要素数取得
   *
   * @return  文字列データテーブルの要素数
   */
  //-----------------------------------------------------------------------------
  virtual u32 GetListTableSize(void);

private:
  bool initializeWifiNetworkLibrary();
  bool initializeLocalNetworkLibrary();

  NetAppLib::UI::RegulationDrawInfo     m_RegulationDrawInfo;

};

GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)

#endif  // PM_DEBUG
