//============================================================================================
/**
 * @file NetAppPackFile.h
 * @brief パックファイルを扱うクラスです
 * @date 2016.02.03
 * @author endo_akira
 */
//============================================================================================
#if !defined( NETAPP_PACK_FILE_H_INCLUDE )
#define NETAPP_PACK_FILE_H_INCLUDE
#pragma once


#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(Util)


class PackFile
{
  GFL_FORBID_COPY_AND_ASSIGN(PackFile);

public:

  PackFile();
  PackFile( const u8* pBuffer );
  virtual~PackFile();

  void LoadFromMemory( const u8* pBuffer );

  //! @brief  レギュレーションパックファイルが破損しているかチェックを行う
  //! @param  fileCountMin ファイル数最小値(0指定でチェックしない)
  //! @param  fileCountMax ファイル数最大値(0指定でチェックしない)
  //! @param  packFileSize パックファイルのサイズ
  //! @retval true 壊れている(不正ファイル)
  //! @retval false 壊れていない(正常なファイル)
  bool IsBrokenRegulationPackFile( const u32 fileCountMin, const u32 fileCountMax, const u32 packFileSize );

  u32 GetFileCount();

  u32 GetFileSize( u32 index );

  const void* GetFileBuffer( u32 index );

private:

  //! @retval true 壊れている(不正ファイル)
  //! @retval false 壊れていない(正常なファイル)
  bool checkOffsetTableAndPackFileSize( const u32 packFileSize );
  u32 calcPaddingSize( const u32 address, const u32 alignSize );

private:

  typedef struct
  {
    u32     version;
    u32     fileCount;
    u8      reserve[8];
  } Header;

  typedef struct
  {
    u32     offset;
    u32     size;
  } Offset;

  const u8*         m_pTopAddress;
  const Header*     m_pHeader;
  const Offset*     m_pOffsetTable;

};


GFL_NAMESPACE_END(Util)
GFL_NAMESPACE_END(NetAppLib)



#endif // NETAPP_PACK_FILE_H_INCLUDE

