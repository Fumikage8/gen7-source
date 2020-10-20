//======================================================================
/**
 * @file	seqfile.cpp
 * @brief	SequenceEditorスクリプトファイル
 * @author	ariizumi
 * @data	11/8/1
 */
//======================================================================

#include <Debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_std_string.h>

#include <fs/include/gfl2_FsPcUtil.h>

#include "DemoLib/SeqEditor/include/SeqFile.h"
#include "DemoLib/SeqEditor/include/SeqSystem.h"

GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(SeqEditor)

SeqFile::SeqFile( SeqSystem *system , gfl2::heap::HeapBase *heapMem , gfl2::fs::AsyncFileManager *fileMng, const gfl2::fs::ArcFile::ARCID arcId , const gfl2::fs::ArcFile::ARCDATID datId , SeqFileCallback *cbWork )
:mSystem(system)
,mCallbackWork(cbWork)
{

  size_t size;
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
  req.arcId = arcId;
  req.datId = datId;
  req.heapForBuf = heapMem;
  req.heapForReq = heapMem->GetLowerHandle();
  req.ppBuf = &mSeqData;
  req.pBufSize = &size;

  fileMng->SyncArcFileLoadData(req);
#if SEQ_EDITOR_DEBUG 
  ARIIZUMI_PRINT("SeqEditor LoadFile size[%x]\n",size);
#endif
  
  mAllocData = true;
  
  InitLoadData();
  LoadHeader();

}

//デバッグ用PCロード
SeqFile::SeqFile( SeqSystem *system , gfl2::heap::HeapBase *heapMem , gfl2::fs::AsyncFileManager *fileMng, char *path , SeqFileCallback *cbWork )
:mSystem(system)
,mCallbackWork(cbWork)
{
  size_t size;

  gfl2::fs::PcUtil::ReadFile(&mSeqData,&size,NULL,path,heapMem,8);

#if SEQ_EDITOR_DEBUG 
  ARIIZUMI_PRINT("SeqEditor LoadFile size[%x]\n",size);
#endif
  
  mAllocData = true;
  
  InitLoadData();
  LoadHeader();
}

SeqFile::SeqFile( SeqSystem *system , void *buf , SeqFileCallback *cbWork)
:mSystem(system)
,mCallbackWork(cbWork)
{
 
  mSeqData = buf;
  mAllocData = false;
  InitLoadData();
  LoadHeader();

}

SeqFile::~SeqFile()
{
  if( mSeqData && mAllocData )
  {
    GflHeapFreeMemory( mSeqData );
  }
}

void SeqFile::InitLoadData(void)
{
  mLoadPos = (char*)mSeqData;

  mFrame = 0;
  mIsFinish = false;
  mIsFinishCommand = false;
}

void SeqFile::LoadHeader(void)
{
  char fileHeader[5] = {0};
  int subVer;
  
  gfl2::std::MemCopy( mLoadPos , fileHeader , 4 );
  mLoadPos += 4;
  LoadDataInt( &mMainVersion );
  LoadDataInt( &subVer );

  LoadDataInt( &mMaxFrame );

  //Skipの時にヘッダー部分を飛ばす処理を入れるので、ここに読み込み増やしたら対応

#if SEQ_EDITOR_DEBUG 
  ARIIZUMI_PRINT("SeqEditor Header[%4s] Version[%d.%d] MaxFrame[%d]\n",fileHeader,mMainVersion,subVer,mMaxFrame);
#endif
  
}

//コマンド部分のロード処理
void SeqFile::LoadCommandParam(CommandParam *param)
{
  s32 sFrame,eFrame;
  u16 comNo,grpNo;
  u16 grpOpt = 0;
  u16 isDebug = 0;
  LoadDataInt( &sFrame );
  LoadDataInt( &eFrame );
  LoadDataU16( &grpNo );
  if( mMainVersion >= 2 )
  {
    LoadDataU16( &grpOpt );
  }
  if( mMainVersion >= 3 )
  {
    LoadDataU16( &isDebug );
  }
  LoadDataU16( &comNo );

  param->commandNo  = comNo;
  param->startFrame = sFrame;
  param->endFrame   = eFrame;
  param->groupNo    = grpNo;
  param->groupOpt   = grpOpt;
  if( isDebug > 0 )
  {
    param->isDebug   = true;
  }
  else
  {
    param->isDebug   = false;
  }
  
}

void SeqFile::UpdateFrame()
{
  if( !mIsFinish )
  {
    int frame;
    
    if( mIsFinishCommand == false )
    {
      do
      {
        LoadDataInt( &frame , false );
        //GFL_PRINT("![%d]\n",frame);
        if( frame == mFrame )
        {
          CommandParam param;
          LoadCommandParam(&param);
          param.isSkipMode = false;

          char *checkLoadPos = mLoadPos;		  
          mCallbackWork->CommnadCallback( this,param );
          
          //テーブルを見て位置を補正
          const int *sizeTable = mSystem->GetSizeTable();
          if( sizeTable )
          {
            checkLoadPos += sizeTable[param.commandNo];
            if( checkLoadPos != mLoadPos )
            {
       ///とりあえず       GFL_PRINT("ロード位置がずれました。[%x][%x]\n",checkLoadPos,mLoadPos);
              if( mLoadPos < checkLoadPos )
              {
                //足らない時だけ補正する
                mLoadPos = checkLoadPos;
              }
            }
          }

        }
        if( frame == -1 )
        {
#if SEQ_EDITOR_DEBUG 
          ARIIZUMI_PRINT("CommandEnd\n");
#endif
          mIsFinishCommand = true;
          break;
        }
      }while( frame == mFrame &&
              mIsFinishCommand == false );
    }
    if( mFrame >= mMaxFrame )
    {
#if SEQ_EDITOR_DEBUG 
      ARIIZUMI_PRINT("FrameEnd\n");
#endif
      mIsFinish = true;
    }
    
    mFrame++;
    
  }
}

void SeqFile::LoadDataInt( int *data , bool isStep )
{
  gfl2::std::MemCopy( mLoadPos , data , 4 );
  if(isStep)
  {
    mLoadPos += 4;
  }
}

void SeqFile::LoadDataU16( u16 *data , bool isStep )
{
  gfl2::std::MemCopy( mLoadPos , data , 2 );
  if(isStep)
  {
    mLoadPos += 2;
  }
}

void SeqFile::LoadDataF32( f32 *data , bool isStep )
{
  gfl2::std::MemCopy( mLoadPos , data , 4 );
  if(isStep)
  {
    mLoadPos += 4;
  }
}

void SeqFile::LoadDataVec3( gfl2::math::Vector3 *data , bool isStep )
{
  gfl2::std::MemCopy( mLoadPos   , &data->x , 4 );
  gfl2::std::MemCopy( mLoadPos+4 , &data->y , 4 );
  gfl2::std::MemCopy( mLoadPos+8 , &data->z , 4 );
  if(isStep)
  {
    mLoadPos += 12;
  }
}
void SeqFile::LoadDataString( char* buf , int length , bool isStep  )
{
  gfl2::std::MemCopy( mLoadPos   , buf , length );
  if(isStep)
  {
    mLoadPos += length;
  }
}

//数値的にはInt.ただ、使い分けとWarnning対応のため
void SeqFile::LoadDataBool( bool *data , bool isStep )
{
  int temp;
  gfl2::std::MemCopy( mLoadPos , &temp , 4 );
  *data = (temp!=0);

  if(isStep)
  {
    mLoadPos += 4;
  }
}

  //フレームスキップ 指定フレームまでスキップします。
  //-1指定で最後まで行きます。comTblと組み合わせてコマンドのチェックもできます。
  //次にUpdateをよぶと指定フレームからの処理ができます。
  //SeqSystemの初期化でコマンドサイズテーブルを渡している必要があります。
  //comTblを渡すと各コマンド時に呼ばれます。サイズテーブルで読み込み位置をずらしているので、
  //不要なコマンドはNULLを入れて問題ありません。
  //isResetがtrueのとき、ResetSeqを呼びます。
void SeqFile::SkipFrame( int frame, bool isReset )
{
  const int *sizeTable = mSystem->GetSizeTable();

  GFL_ASSERT_MSG( sizeTable , "SizeTableが指定されていないため、SkipFrameを使えません！\n" );//@fix 作成時に気づく。処理なしで戻るように修正
  if( sizeTable == NULL )
  {
    return;
  }

  bool isLoop = true;
  if(isReset) ResetSeq();

  if( frame > 0 )
  {
    mFrame = frame-1;
  }
  else
  if( frame == 0 )
  {
    return;
  }
  else
  {
    mFrame = mMaxFrame+1;
  }
  
  while( isLoop )
  {
    int checkFrame;
    LoadDataInt( &checkFrame , false );
    if( checkFrame > mFrame ||
        checkFrame == -1 )
    {
      isLoop = false;
    }
    else
    {
      CommandParam param;
      LoadCommandParam(&param);
      param.isSkipMode = true;

      char *tempLoadPos = mLoadPos;
      mCallbackWork->CommnadCallback( this,param );
      mLoadPos = tempLoadPos + sizeTable[param.commandNo];
    }
  }
}

void SeqFile::ResetSeq(void)
{
  InitLoadData();
  mLoadPos = (char*)mSeqData;
  mLoadPos += 16;  //LoadHeaderより
}

//指定コマンドのフレームを見る。ResetSeqしなければ続きから調べられる
  //-1でもうない状態
int SeqFile::GetCommandFrame( int comNo , char** loadPos )
{
  const int *sizeTable = mSystem->GetSizeTable();

  GFL_ASSERT_MSG( sizeTable , "SizeTableが指定されていないため、GetCommandFrameを使えません！\n" ); //@fix 作成時に気づく。処理なしで戻るように修正
  if( sizeTable == NULL )
  {
    return -1;
  }

  bool isLoop = true;
  mLoadPos = *loadPos;
  while( isLoop )
  {
    int checkFrame;
    LoadDataInt( &checkFrame , false );
    if( checkFrame == -1 )
    {
      isLoop = false;
    }
    else
    {
      CommandParam param;
      LoadCommandParam(&param);
      param.isSkipMode = true;

      *loadPos = mLoadPos + sizeTable[param.commandNo];
      
      if( param.commandNo == comNo )
      {
        return param.startFrame;
      }
      mLoadPos = mLoadPos + sizeTable[param.commandNo];
    }
  }
  
  return -1;
}

GFL_NAMESPACE_END(SeqEditor)
GFL_NAMESPACE_END(DemoLib)

