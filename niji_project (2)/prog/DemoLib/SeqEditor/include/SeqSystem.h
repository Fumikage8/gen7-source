//======================================================================
/**
 * @file	seqeditor.h
 * @brief	SequenceEditorファイル読み込み
 * @author	ariizumi
 * @data	11/8/1
 */
//======================================================================

#ifndef __SEQ_SYSTEM_H__
#define __SEQ_SYSTEM_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>

#include "DemoLib/SeqEditor/include/SeqFile.h"


#if defined(DEBUG_ONLY_FOR_ariizumi_nobuhiko)
#define SEQ_EDITOR_DEBUG 1
#else
#define SEQ_EDITOR_DEBUG 0
#endif

GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(SeqEditor)


//includeの順番上諸々の定義はSeqFileで。  


typedef struct
{
  gfl2::heap::HeapBase *heapMem;
  const int *comSizeTable;  //NULL可

}InitParam;

class SeqSystem
{
public:
  SeqSystem( const InitParam &param );
  virtual ~SeqSystem();
  
  SeqFile* LoadFile( gfl2::fs::AsyncFileManager *fileMng, const gfl2::fs::ArcFile::ARCID arcId , const gfl2::fs::ArcFile::ARCDATID datId , SeqFileCallback *cbWork );
  SeqFile* LoadPcFile( gfl2::fs::AsyncFileManager *fileMng, char *path , SeqFileCallback *cbWork );
  SeqFile* SetBuffer( void *buf , SeqFileCallback *cbWork );

  void UnLoad( SeqFile* file );
  void UnLoadAll(void);
  
  void UpdateFrame( const int step = 1 );
  
  //コマンドテーブルと個数
  const int* GetSizeTable(void){return mCommandSizeTeble;}
  
  u32 GetActiveScriptNum(void){return mFileList.Size();}
private:
  gfl2::heap::HeapBase *mHeapMem;
  
  //コマンドテーブルと個数
  const int *mCommandSizeTeble;
  
  SeqFileList mFileList;

};

 
GFL_NAMESPACE_END(SeqEditor)
GFL_NAMESPACE_END(DemoLib)

#endif //__SEQ_SYSTEM_H__

