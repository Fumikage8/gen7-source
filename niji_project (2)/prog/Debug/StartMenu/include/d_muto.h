
#if !defined(D_MUTO_H_INCLUDED)
#define D_MUTO_H_INCLUDED
#pragma once
#if PM_DEBUG
//=============================================================================
/**
 * @file    d_muto.h
 * @brief   ランチャーリスト処理
 * @author  Yu Muto 
 * @date    2015.05.08
 */
//=============================================================================

#include <Debug/Launcher/include/LauncherProc.h>
#include <Debug/Launcher/include/LauncherListData.h>


/*
  debug::startmenu
*/
namespace debug {
namespace startmenu {
//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
  * @class   LauncherMuto
  * @brief   武藤用debugLauncher
  */
//-----------------------------------------------------------------------------
class LauncherMuto : public debug::launcher::ListData
{
private:
  static const u32    ItemList[];   /**<  項目文字列データテーブル  */

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief    コンストラクタ・デストラクタ
   */
  //-----------------------------------------------------------------------------
  LauncherMuto( void );
  virtual ~LauncherMuto();


  /*
    ListData class I/F
  */

  //-----------------------------------------------------------------------------
  /**
   * @func    SelectListener
   * @brief   決定時のリスナー
   * @date    2015.05.08
   *
   * @param   pos       カーソル位置 ( キャンセル時はCURSOR_POS_CANCEL )
   * @param   launcher  呼び出し元のランチャークラス
   *
   * @return  ランチャーへのリクエストID
   */
  //-----------------------------------------------------------------------------
  virtual debug::launcher::ListData::RequestID  SelectListener( u32 pos, debug::launcher::LauncherProc * launcher );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetArcDataID
   * @brief   アークデータID取得
   * @date    2015.05.08
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
  virtual const u32*  GetListTable(void);

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
  class Inner;
  Inner*                  m_pInner;
  gfl2::heap::HeapBase*   m_pLocalHeapBase;

};




} /*  namespace debug       */
} /*  namespace startmenu   */


#endif  /*  PM_DEBUG  */
#endif  /*  #if !defined(D_MUTO_H_INCLUDED) */
