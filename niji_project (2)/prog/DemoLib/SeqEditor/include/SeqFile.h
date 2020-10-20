//======================================================================
/**
 * @file	seqfile.h
 * @brief	SequenceEditorスクリプトファイル
 * @author	ariizumi
 * @data	11/8/1
 */
//======================================================================

#ifndef __SEQ_FILE_H__
#define __SEQ_FILE_H__

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_Vector3.h>
#include <math/include/gfl2_Vec.h>
#include <util/include/gfl2_List.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>

GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(SeqEditor)

class SeqSystem;
class SeqFile;

typedef struct
{
  int commandNo;
  int startFrame;
  int endFrame;
  int groupNo;
  int groupOpt;
  bool isDebug;
  bool isSkipMode;
}CommandParam;

class SeqFileCallback
{
  GFL_FORBID_COPY_AND_ASSIGN(SeqFileCallback); // コピーコンストラクタ＋代入禁止
public:
  SeqFileCallback(){};
  virtual ~SeqFileCallback(){};
  
  //コマンド受け取りコールバック。
  virtual void CommnadCallback( SeqFile *seqFile , const CommandParam &param ) = 0;
  
};

class SeqFile
{
public:

  SeqFile( SeqSystem *system , gfl2::heap::HeapBase *heapMem , gfl2::fs::AsyncFileManager *fileMng, const gfl2::fs::ArcFile::ARCID arcId , const gfl2::fs::ArcFile::ARCDATID datId , SeqFileCallback *cbWork );
  SeqFile( SeqSystem *system , gfl2::heap::HeapBase *heapMem , gfl2::fs::AsyncFileManager *fileMng, char *path , SeqFileCallback *cbWork );
  SeqFile( SeqSystem *system , void *buf , SeqFileCallback *cbWork );
  
  ~SeqFile();
  
  void UpdateFrame();

  //データ取得系
  void LoadDataInt( int *data , bool isStep = true );
  void LoadDataU16( u16 *data , bool isStep = true );
  void LoadDataF32( f32 *data , bool isStep = true );
  void LoadDataVec3( gfl2::math::Vector3 *data , bool isStep = true );
  void LoadDataString( char* buf , int length , bool isStep = true );
  void LoadDataBool( bool *data , bool isStep = true );
  
  bool IsFinish(void){return mIsFinish;}
  
  //フレームスキップ 指定フレームまでスキップします。
  //-1指定で最後まで行きます。comTblと組み合わせてコマンドのチェックもできます。
  //次にUpdateをよぶと指定フレームからの処理ができます。
  //SeqSystemの初期化でコマンドサイズテーブルを渡している必要があります。
  //comTblを渡すと各コマンド時に呼ばれます。サイズテーブルで読み込み位置をずらしているので、
  //不要なコマンドはNULLを入れて問題ありません。
  //isResetがtrueのとき、ResetSeqを呼びます。
  void SkipFrame( int frame, bool isReset = true );
  
  //上で最後まで見た後、最初にするために。
  void ResetSeq(void);

  //loadPosにはGetLoadPosした値をいれ、保持してもらう。
  //終了時に次回の開始位置が入っているので、そのまま続きがチェックできる。
  //内部のLoadPosはコマンドヘッダを読んだ後の状態なので、LoadDataが使える。
  //-1でもうない状態
  int GetCommandFrame( int comNo , char** loadPos );
  char* GetLoadPos(void){return mLoadPos;}
  
  int GetFrame(void){return mFrame;}
  int GetMaxFrame(void){return mMaxFrame;}
  void SetMaxFrame(int frame){mMaxFrame = frame;}
  
private:

  void InitLoadData(void);
  void LoadHeader(void);
  
  void LoadCommandParam(CommandParam *param);

  void *mSeqData;
  char *mLoadPos;
  bool mAllocData;

  int mFrame;
  int mMaxFrame;
  bool mIsFinish;
  bool mIsFinishCommand;
  
  int mMainVersion;
  
  SeqSystem *mSystem;
  SeqFileCallback *mCallbackWork;
};

typedef gfl2::util::List<SeqFile*> SeqFileList;
typedef SeqFileList::Iterator SeqFileIt;
  
GFL_NAMESPACE_END(SeqEditor)
GFL_NAMESPACE_END(DemoLib)

#endif //__SEQ_FILE_H__

