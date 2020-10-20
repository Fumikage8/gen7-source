//======================================================================
/**
 * @file  seqeditor.cpp
 * @brief SequenceEditorファイル読み込み
 * @author  ariizumi
 * @data  11/8/1
 */
//======================================================================

#include "DemoLib/SeqEditor/include/SeqSystem.h"

GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(SeqEditor)


SeqSystem::SeqSystem( const InitParam &param )
:mHeapMem( param.heapMem )
,mCommandSizeTeble(param.comSizeTable)
,mFileList(param.heapMem,8)
{
  
}

SeqSystem::~SeqSystem()
{
  UnLoadAll();
}

SeqFile* SeqSystem::LoadFile( gfl2::fs::AsyncFileManager *fileMng, const gfl2::fs::ArcFile::ARCID arcId , const gfl2::fs::ArcFile::ARCDATID datId , SeqFileCallback *cbWork )
{
  SeqFile *file = GFL_NEW(mHeapMem) SeqFile(this,mHeapMem,fileMng,arcId,datId,cbWork);
  
  mFileList.PushBack( file );
  
  return file;
}

SeqFile* SeqSystem::LoadPcFile( gfl2::fs::AsyncFileManager *fileMng, char *path , SeqFileCallback *cbWork )
{
  SeqFile *file = GFL_NEW(mHeapMem) SeqFile(this,mHeapMem,fileMng,path,cbWork);

  mFileList.PushBack( file );
  
  return file;
}

SeqFile* SeqSystem::SetBuffer( void *buf , SeqFileCallback *cbWork )
{
  SeqFile *file = GFL_NEW(mHeapMem) SeqFile(this,buf,cbWork);

  mFileList.PushBack( file );
  
  return file;
}

void SeqSystem::UnLoad( SeqFile* file )
{
  mFileList.Remove( file );
  
  GFL_DELETE file;
}

void SeqSystem::UnLoadAll(void)
{
  for (SeqFileIt it = mFileList.Begin(); it != mFileList.End();)
  {
    SeqFileIt currIt = ++it;
    SeqFile *node = (*currIt);
    
    UnLoad(node);
  }
}

void SeqSystem::UpdateFrame( const int step )
{
  for( int i=0;i<step;i++ )
  {
    for (SeqFileList::Iterator it = mFileList.Begin(); it != mFileList.End();)
    {
      SeqFileList::Iterator currIt = it;
      SeqFile *node = (*currIt);
      ++it;
      node->UpdateFrame();
    }
  }
}

GFL_NAMESPACE_END(SeqEditor)
GFL_NAMESPACE_END(DemoLib)
