#ifndef __GFL_HEAPMANAGER_H__
#define __GFL_HEAPMANAGER_H__
//======================================================================
/**
 * @file	gfl_HeapManager.h
 * @brief	ヒープ管理システム
 * @author	ariizumi
 * @data	10/10/05
 */
//======================================================================
#pragma once

#include <gfl_Macros.h>

#if GFL_DEBUG
#define GFL_HEAP_DEBUG 0
#else
#define GFL_HEAP_DEBUG 0
#endif

namespace gfl
{
namespace heap
{


//GflHeapBaseを継承して、プラットフォームごとの処理を追加するための対応。
//アプリではHeapBaseクラスとして共通で使うことが可能

//プラットフォーム別処理
class CtrHeapBase;
typedef CtrHeapBase HeapBase;
//プラットフォーム別処理 ここまで


//HEAPID型の定義
typedef int HEAPID;

//不正なID
static const HEAPID GFL_HEAP_INVALID_ID = -1;

//ヒープの種類(プラットフォーム別にするか？)
typedef enum
{
  HEAP_TYPE_EXP,  //拡張ヒープ
  HEAP_TYPE_UNIT, //ユニットヒープ
  
  HEAP_TYPE_MAX
}HeapType;

//HEAPID用定義
enum
{
  HEAPID_SYSTEM,
  
  HEAPID_MAX,
  //アプリごとのHEAPID開始位置
  HEAPID_USER_START = HEAPID_MAX
};

typedef struct _HEAP_MNG_DATA HEAP_MNG_DATA;

#if GFL_HEAP_DEBUG
#define	GFL_CREATE_HEAP(parentHeap,childId,size,heapType,isBack)	( gfl::heap::Manager::CreateHeap(parentHeap,childId,size,heapType,isBack,__FILE__,__LINE__) )
#define	GFL_CREATE_HEAP_BUFFER(buffer,childId,size,heapType)	( gfl::heap::Manager::CreateHeapBuffer(buffer,childId,size,heapType,__FILE__,__LINE__) )
#define	GFL_CREATE_HEAP_BUFFER_EX(buffer,childId,size,heapType,param)	( gfl::heap::Manager::CreateHeapBufferEx(buffer,childId,size,heapType,param,__FILE__,__LINE__) )
#else
#define	GFL_CREATE_HEAP(parentHeap,childId,size,heapType,isBack)	( gfl::heap::Manager::CreateHeap(parentHeap,childId,size,heapType,isBack) )
#define	GFL_CREATE_HEAP_BUFFER(buffer,childId,size,heapType)	( gfl::heap::Manager::CreateHeapBuffer(buffer,childId,size,heapType) )
#define	GFL_CREATE_HEAP_BUFFER_EX(buffer,childId,size,heapType,param)	( gfl::heap::Manager::CreateHeapBufferEx(buffer,childId,size,heapType,param) )
#endif

//#pragma mark [> gfl::heap::Manager
class Manager
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(Manager);
public:
  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープ管理初期化
   *
   *  @param  void *buf 初期バッファ
   *                    管理領域を取った後HEAPID_SYSTEMとして確保されます
   *  @param  const int size 初期バッファのサイズ
   *  @param  const int HeappNum 定義してあるHEAPIDの最大数です
   */
  //-----------------------------------------------------------------------------
  static void Initialize( void *buf , const int size , const int HeapNum );

  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープ管理開放
   *
   *  @param  bool isCheck 解放忘れチェックを行う
   *                       ソフトリセットなど特殊な状況以外でフラグを落とさないこと！
   */
  //-----------------------------------------------------------------------------
  static void Finalize( bool isCheck = true );

  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープ管理リセット(ソフトリセット用)
   *          すべてのヒープの中身をすべて解放し、ヒープ自体も解放します。
   *          例外対象を引数で指定します。
   *
   *  @param  const HEAPID clearIdArr 中身は消すが、ヒープ自体は残しておくHEAPIDの配列
   *  @param  const int clearIdNum    ↑の個数
   *  @param  const HEAPID keepIdArr  処理をしないHEAPIDの配列
   *  @param  const int keepIdNum     ↑の個数
   */
  //-----------------------------------------------------------------------------
  static void Reset( const HEAPID *clearIdArr , const int clearIdNum ,  const HEAPID *keepIdArr , const int keepIdNum );
  

  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープ作成時の実消費サイズ計算 確保するサイズ+管理領域
   *          Createの時は管理領域は自動で追加されます
   *
   *  @param  const int size 確保したいメモリのサイズ
   *  @param  const HeapType heapType 確保したいヒープの種類
   */
  //-----------------------------------------------------------------------------
  static int CalcCreateHeapSize( const int size , const HeapType heapType );

  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープ作成 親HeapBase指定版
   *          指定サイズ＋管理領域分が親ヒープから確保されます
   *
   *  @param  HeapBase *parentHeap 親となるHeapBaseのポインタ
   *  @param  const HEAPID childId 新しく作るヒープのHEAPID
   *  @param  const int size 確保するメモリのサイズ
   *  @param  const HeapType heapType 確保するヒープの種類
	 *  @param  const bool isBack 後方確保
   */
  //-----------------------------------------------------------------------------
#if GFL_HEAP_DEBUG
  static HeapBase* CreateHeap( HeapBase *parentHeap , const HEAPID childId , const int size , const HeapType heapType , const bool isBack = false, const char *filename=__FILE__, const int linenum=__LINE__ );
#else
  static HeapBase* CreateHeap( HeapBase *parentHeap , const HEAPID childId , const int size , const HeapType heapType , const bool isBack = false );
#endif

  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープ作成 バッファから作成版
   *          指定サイズから管理領域分が引かれ、残りサイズが実際のヒープサイズになります
   *
   *  @param  void *buffer ヒープを作成するバッファ
   *  @param  const HEAPID childId 新しく作るヒープのHEAPID
   *  @param  const int size 確保するメモリのサイズ
   *  @param  const HeapType heapType 確保するヒープの種類
   */
  //-----------------------------------------------------------------------------
#if GFL_HEAP_DEBUG
  static HeapBase* CreateHeapBuffer( void *buffer , const HEAPID childId , const int size , const HeapType heapType, const char *filename=__FILE__, const int linenum=__LINE__ );
#else
  static HeapBase* CreateHeapBuffer( void *buffer , const HEAPID childId , const int size , const HeapType heapType );
#endif

  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープ作成 バッファから作成版
   *          指定サイズから管理領域分が引かれ、残りサイズが実際のヒープサイズになります
   *          特殊なヒープを作る際にパラメータを渡せる
   *
   *  @param  void *buffer ヒープを作成するバッファ
   *  @param  const HEAPID childId 新しく作るヒープのHEAPID
   *  @param  const int size 確保するメモリのサイズ
   *  @param  const HeapType heapType 確保するヒープの種類
   *  @param  void *param ヒープ用パラメータ
   */
  //-----------------------------------------------------------------------------
#if GFL_HEAP_DEBUG
  static HeapBase* CreateHeapBufferEx( void *buffer , const HEAPID childId , const int size , const HeapType heapType , void *param, const char *filename=__FILE__, const int linenum=__LINE__ );
#else
  static HeapBase* CreateHeapBufferEx( void *buffer , const HEAPID childId , const int size , const HeapType heapType , void *param );
#endif

  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープ削除
   *
   *  @param  HeapBase *heap 削除するHeapBase
   */
  //-----------------------------------------------------------------------------
  static void DeleteHeap( HeapBase *heap );

  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープ削除(チェック処理なし)
   *          子ヒープと解放残しもすべて解放します。
   *          ただしCreateHeapBufferで作られたHeapは解放できません。
   *
   *  @param  HeapBase *heap 削除するHeapBase
   */
  //-----------------------------------------------------------------------------
  static void DeleteHeapNoCheck( HeapBase *heap );

  //----------------------------------------------------------------------------
  /**
   *  @brief  HEAPIDからHeapBaseクラスを取得
   *
   *  @param  const HEAPID heapId 取得したいヒープのHEAPID
   *  @return HeapBase* 対応するHeapBaseクラス 
   */
  //-----------------------------------------------------------------------------
  static HeapBase* GetHeapByHeapId( const HEAPID heapId );

  //======================================================================
  //  utility関数
  //======================================================================
  //----------------------------------------------------------------------------
  /**
   *  @brief  HEAPIDに対応するHeapBaseがあるかチェック
   *
   *  @param  const HEAPID heapId 調べたいヒープのHEAPID
   *  @return bool ture:存在する false:存在しない
   */
  //-----------------------------------------------------------------------------
  static bool IsExistHeapId( const HEAPID heapId );

  //----------------------------------------------------------------------------
  /**
   *  @brief  確保された領域のサイズを取得(HeapBaseにもあります)
   *
   *  @param  const void mem 調べるメモリ
   *  @return int メモリサイズ(-1は失敗)
   */
  //-----------------------------------------------------------------------------
  static int GetMemorySize( const void *mem );

  //----------------------------------------------------------------------------
  /**
   *  @brief  渡されたメモリが正当かどうかをチェックします。
   *
   *  @param  const void mem 調べるメモリ
   *  @return bool チェック失敗でfalse
   */
  //-----------------------------------------------------------------------------
  static bool CheckMemoryBlock( const void *mem );

  //======================================================================
  //  デバッグ機能
  //======================================================================
  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープの取得状況の出力
   */
  //-----------------------------------------------------------------------------
  static void Dump(void);
  static void DumpTree( HeapBase *heap );

private:
  //ヒープ作成コア部分
  static HeapBase* CreateHeapCore( void *buffer , const HEAPID childId , const int size , const HeapType heapType , const bool isBuffer , void *param );
  //ヒープ削除コア(チェックフラグの有無のみ
  static void DeleteHeapCore( HeapBase *heap , bool isCheck = true );

  //管理データ
  static HEAP_MNG_DATA *m_heapMngData;
  //最大個数
  static int m_heapNum;

};

} //namespace heap
} //namespace gfl

#endif // __GFL_HEAPMANAGER_H__
