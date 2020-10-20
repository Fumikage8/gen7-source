//=============================================================================
/**
 * @file    d_obata.h
 * @brief   小幡用ランチャーリスト処理
 * @author  obata_toshihiro
 * @date    2015.02.20
 */
//=============================================================================
#if PM_DEBUG
#ifndef D_OBATA_H_INCLUDED
#define D_OBATA_H_INCLUDED

#include <str/include/gfl2_Str.h>
#include <pml/include/pml_PokePara.h>
#include <Debug/Launcher/include/LauncherProc.h>
#include <Debug/Launcher/include/LauncherListData.h>
#include <Debug/DebugPokeMake/include/DebugPokeMake.h>

GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(startmenu)

//-----------------------------------------------------------------------------
/**
 * @class   LauncherObata
 * @brief   小幡用ランチャークラス
 */
//-----------------------------------------------------------------------------
class LauncherObata
  : public launcher::ListData
{
private:
  static const u32 LIST_MAX = 9;        //!< 項目数
  static const u32 ItemList[LIST_MAX];  //!< 項目文字列データテーブル

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //-----------------------------------------------------------------------------
  LauncherObata(void);
  virtual ~LauncherObata();

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
  void CallDebugKeyboardProc();
  void CallDebugPokeMakeProc();
  void ExecExpTest();
  template <typename TestProcClassName> void CallTestProc( void );


private:
  gfl2::str::StrBuf* m_strBuf;
  debug::PokeMake::APP_PARAM m_debugPokeMakeParam;
};

GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)

#endif // D_OBATA_H_INCLUDED
#endif // PM_DEBUG
