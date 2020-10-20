#if !defined( __XY_EXTSAVEDATA_H__ )
#define __XY_EXTSAVEDATA_H__
//==============================================================================
/**
 * @file    NijiExtSaveData.h
 * @brief   拡張セーブデータの定義
 * @author  tamada
 * @date    2012.09.25
 */
//==============================================================================
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#if defined(GF_PLATFORM_WIN32)
namespace nn
{
  namespace fs
  {
    typedef int ExtSaveDataId;
  }
}

#endif

GFL_NAMESPACE_BEGIN(ExtSavedata)


//-----------------------------------------------------------------------------
/**
 * @brief 拡張セーブデータ定義
 */
//-----------------------------------------------------------------------------
struct ExtSaveDataFormat
{
  const wchar_t * directory_name;
  int             max_file_num;
  size_t          max_file_size;
};

//-----------------------------------------------------------------------------
/**
 * @brief 拡張セーブデータ・ディレクトリ指定ID
 */
//-----------------------------------------------------------------------------
enum NIJI_EXTSAVE_ID
{
  EXTID_BATTLEVIDEO,
  EXTID_FINDERSTUDIO,
  EXTID_REGULATION,
  EXTID_PSSICON_FRIEND,
  EXTID_TRIALHOUSE,

  EXTID_NUM,
};

//-----------------------------------------------------------------------------
/**
 * @class NijiExtSaveData
 */
//-----------------------------------------------------------------------------
class NijiExtSaveData
{
  GFL_FORBID_COPY_AND_ASSIGN(NijiExtSaveData); //コピーコンストラクタ＋代入禁止

public:


  static nn::fs::ExtSaveDataId GetExtSaveDataId( void );
  //----------------------------------------------------------
  /// 生成時に指定するディレクトリ数
  //----------------------------------------------------------
  static int GetMaxDirectoryCount(void);

  //----------------------------------------------------------
  /// 生成時に指定するファイル数
  //----------------------------------------------------------
  static int GetMaxFileCount(void);

  //----------------------------------------------------------
  /// xyで必要な企画分のディレクトリ数
  //----------------------------------------------------------
  static int GetSaveDataFormatCount( void );

  static const ExtSaveDataFormat * GetSaveDataFormat( int id );

  //----------------------------------------------------------
  /**
   * @brief       各企画毎の最大データサイズを取得する
   * @param[in]   id
   */
  //----------------------------------------------------------
  static size_t GetSaveFileSize( int id );

  //----------------------------------------------------------
  /**
   * @brief     各企画毎の最大ファイル数を取得する
   * @param[in]   id
   */
  //----------------------------------------------------------
  static int GetSaveFileMaxNum( int id );
private:
  


};  //NijiExtSaveData

GFL_NAMESPACE_END(extsavedata)



#endif  // __XY_EXTSAVEDATA_H__




