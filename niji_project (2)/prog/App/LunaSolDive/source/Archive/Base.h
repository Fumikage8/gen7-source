//======================================================================
/**
 * @file Base.h
 * @date 2016/11/25 13:08:14
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__ARCHIVE__BASE_H_INCLUDED__
#define __LUNA_SOL_DIVE__ARCHIVE__BASE_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <types/include/gfl2_Typedef.h>

// forward declaration
namespace gfl2{
namespace fs{
class AsyncFileManager;
}// namespace fs
}// namespace gfl2

namespace LunaSolDive{
namespace Archive{
class Base
{
  GFL_FORBID_COPY_AND_ASSIGN(Base);
public:
  // destructor
  virtual ~Base() = 0;
  // Open this archive 
  void Open() const;
  // Whether this archive has been opened or not
  bool IsOpened() const;
  // Close this archive
  void Close() const;
  // Whether this archive has been closed or not
  bool IsClosed() const;
  // Load the data from archive file
  void Load(
              const u32& index) const;
  // Whether the data has been loaded or not
  bool IsLoaded(
              const u32& index) const;
  // Load all data from archive file
  void LoadAll() const;
  // Whether all data have been loaded or not
  bool IsAllLoaded() const;
  // Data
  void* Data(
              const u32& index) const;
  // The Number of Data
  const u32& DataNumber() const;
protected:
  // constructor
  Base();
  // Initialize
  void Initialize(
              gfl2::heap::HeapBase* pHeap,
              gfl2::fs::AsyncFileManager* pFileManager,
              const u32& archiveID,
              const u32& dataNumber,
              const bool& isCompressed);
  // Terminate
  void Terminate();
private:
  // Heap pointer
  gfl2::heap::HeapBase* m_pHeap;
  // AsyncFileManager pointer
  gfl2::fs::AsyncFileManager* m_pFileManager;
  // Archive ID
  u32 m_ArchiveID;
  // The number of data
  u32 m_DataNumber;
  // DataList
  void** m_DataList;
  // Whether the data is compressed or not
  bool m_IsCompressed;
};
}// namespace Archive
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__ARCHIVE__BASE_H_INCLUDED__