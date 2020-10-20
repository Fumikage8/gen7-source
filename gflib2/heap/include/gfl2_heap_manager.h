#ifndef __GFL_HEAPMANAGER_H__
#define __GFL_HEAPMANAGER_H__
//======================================================================
/**
 * @file	gfl2_HeapManager.h
 * @brief	ヒープ管理システム
 * @author	ariizumi
 * @data	10/10/05
 */
//======================================================================
#pragma once

#include <stdio.h>  //NULL
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_math_control.h>
//#include <heap/gfl2_CtrExpHeap.h>
//#include <heap/gfl2_CtrUnitHeap.h>

#if GFL_DEBUG
#define GFL_HEAP_DEBUG 1
#else
#define GFL_HEAP_DEBUG 0
#endif

namespace gfl2
{
namespace heap
{


//GflHeapBaseを継承して、プラットフォームごとの処理を追加するための対応。
//アプリではHeapBaseクラスとして共通で使うことが可能

//プラットフォーム別処理
#if defined(GF_PLATFORM_CTR)
class CtrHeapBase;
typedef CtrHeapBase HeapBase;
#elif defined(GF_PLATFORM_WIN32)
class WinHeap;
typedef WinHeap HeapBase;
#endif  //プラットフォーム分岐
//プラットフォーム別処理 ここまで


//HEAPID型の定義
typedef int HEAPID;

//不正なID
static const HEAPID GFL_HEAP_INVALID_ID = -1;
//ローカルヒープ用ID
static const HEAPID GFL_HEAP_LOCAL_ID = -2;
static const HEAPID GFL_HEAP_LOCAL_ID_BASE = -10; //この値からマイナス方向にIDをつける

//namespaceをつけるため、namespaceの外に移動(一番下)
//#define GFL_HEAP_INDEX_TO_LOCAL_HEAP_ID(idx) (GFL_HEAP_LOCAL_ID_BASE-idx)
//#define GFL_HEAP_LOCAL_HEAP_ID_TO_INDEX(id) (gfl2::math::Abs(id-GFL_HEAP_LOCAL_ID_BASE))
//#define GFL_HEAP_HEAP_ID_TO_INDEX(heapId) (heapId<=GFL_HEAP_LOCAL_ID_BASE?GFL_HEAP_LOCAL_HEAP_ID_TO_INDEX(heapId):heapId)

//ヒープの種類(プラットフォーム別にするか？)
typedef enum
{
  HEAP_TYPE_EXP,  //拡張ヒープ
  
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
//name無し
#define	GFL_CREATE_HEAP(parentHeap,childId,size,heapType,isBack)	( gfl2::heap::Manager::CreateHeap(parentHeap,childId,size,heapType,isBack,NULL,__FILE__,__LINE__) )
#define	GFL_CREATE_HEAP_BUFFER(buffer,childId,size,heapType)	( gfl2::heap::Manager::CreateHeapBuffer(buffer,childId,size,heapType,NULL,__FILE__,__LINE__) )
#define	GFL_CREATE_HEAP_BUFFER_EX(buffer,childId,size,heapType,param)	( gfl2::heap::Manager::CreateHeapBufferEx(buffer,childId,size,heapType,param,NULL,__FILE__,__LINE__) )
#define	GFL_CREATE_LOCAL_HEAP(parentHeap,size,heapType,isBack)	( gfl2::heap::Manager::CreateHeap(parentHeap,gfl2::heap::GFL_HEAP_LOCAL_ID,size,heapType,isBack,NULL,__FILE__,__LINE__) )
//name有
#define	GFL_CREATE_HEAP_NAME(parentHeap,childId,size,heapType,isBack,name)	( gfl2::heap::Manager::CreateHeap(parentHeap,childId,size,heapType,isBack,name,__FILE__,__LINE__) )
#define	GFL_CREATE_LOCAL_HEAP_NAME(parentHeap,size,heapType,isBack,name)	( gfl2::heap::Manager::CreateHeap(parentHeap,gfl2::heap::GFL_HEAP_LOCAL_ID,size,heapType,isBack,name,__FILE__,__LINE__) )

#else
//name無し
#define	GFL_CREATE_HEAP(parentHeap,childId,size,heapType,isBack)	( gfl2::heap::Manager::CreateHeap(parentHeap,childId,size,heapType,isBack) )
#define	GFL_CREATE_HEAP_BUFFER(buffer,childId,size,heapType)	( gfl2::heap::Manager::CreateHeapBuffer(buffer,childId,size,heapType) )
#define	GFL_CREATE_HEAP_BUFFER_EX(buffer,childId,size,heapType,param)	( gfl2::heap::Manager::CreateHeapBufferEx(buffer,childId,size,heapType,param) )
#define	GFL_CREATE_LOCAL_HEAP(parentHeap,size,heapType,isBack)	( gfl2::heap::Manager::CreateHeap(parentHeap,gfl2::heap::GFL_HEAP_LOCAL_ID,size,heapType,isBack) )
//全部いり
#define	GFL_CREATE_HEAP_NAME(parentHeap,childId,size,heapType,isBack,name)	( gfl2::heap::Manager::CreateHeap(parentHeap,childId,size,heapType,isBack,name) )
#define	GFL_CREATE_LOCAL_HEAP_NAME(parentHeap,size,heapType,isBack,name)	( gfl2::heap::Manager::CreateHeap(parentHeap,gfl2::heap::GFL_HEAP_LOCAL_ID,size,heapType,isBack,name) )
#endif

#define	GFL_DELETE_HEAP(heapBase)	( gfl2::heap::Manager::DeleteHeap(heapBase) )


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
  static void Initialize( void *buf , const int size , const int HeapNum , const int localHeapNum = 0 );

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
  static HeapBase* CreateHeap( HeapBase *parentHeap , const HEAPID childId , const int size , const HeapType heapType , const bool isBack = false, const char *heapName=NULL, const char *filename=__FILE__, const int linenum=__LINE__ );
#else
  static HeapBase* CreateHeap( HeapBase *parentHeap , const HEAPID childId , const int size , const HeapType heapType , const bool isBack = false, const char *heapName=NULL );
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
  static HeapBase* CreateHeapBuffer( void *buffer , const HEAPID childId , const int size , const HeapType heapType, const char *heapName=NULL, const char *filename=__FILE__, const int linenum=__LINE__ );
#else
  static HeapBase* CreateHeapBuffer( void *buffer , const HEAPID childId , const int size , const HeapType heapType, const char *heapName=NULL );
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
  static HeapBase* CreateHeapBufferEx( void *buffer , const HEAPID childId , const int size , const HeapType heapType , void *param, const char *heapName=NULL, const char *filename=__FILE__, const int linenum=__LINE__ );
#else
  static HeapBase* CreateHeapBufferEx( void *buffer , const HEAPID childId , const int size , const HeapType heapType , void *param, const char *heapName=NULL);
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
   *  @brief  ローカルヒープを加味し、HEAP_MNG_DATAを取得
   *
   *  @param  const HEAPID heapId 調べたいヒープのHEAPID
   *  @return HEAP_MNG_DATA
   */
  //-----------------------------------------------------------------------------
  static HEAP_MNG_DATA* IsGetHeapManageData( const HEAPID heapId );

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

  /**
  *  @brief  渡されたメモリが管理しているメモリかチェックする
  *  @note   CheckMemoryBlockでは偶然の一致を検出できない
  *  @note   検索するため、重い
  *  @note   Win32版のみ動作
  *
  *  @param  pMemory 調べるメモリ
  *
  *  @return 管理しているメモリならtrue、管理外ならfalseを返す
  */
  static b32 IsManaged( const void *pMemory );

  /**
  *  @brief  渡されたメモリが管理しているメモリかチェックする
  *  @note   初期確保サイズの範囲チェックのみを行う
  *  @note   Win32版のみ動作
  *
  *  @param  pMemory 調べるメモリ
  *
  *  @return 管理しているメモリならtrue、管理外ならfalseを返す
  */
  static b32 IsManagedFast( const void *pMemory );

  /**
  *  @brief  管理外メモリをdeleteする際に使用する関数ポインタ型
  *
  */
  typedef void (*DeleteFunc)(void*);
  /**
  *  @brief  管理外メモリをdeleteする際に使用する関数を設定する
  *
  *  @param[in]  func  関数ポインタ。NULLで外す。
  *                    関数の例  void t_operatorDelete( void* aPtr );
  */
  static void SetUnmanagedDeleteFunc(DeleteFunc func)
  {
    m_unmanagedDeleteFunc = func;
  }
  /**
  *  @brief  管理外メモリをdeleteする際に使用する関数を取得する
  *
  *  @return  関数ポインタ。NULLのときは設定されていない。
  */
  static DeleteFunc GetUnmanagedDeleteFunc(void)
  {
    return m_unmanagedDeleteFunc;
  }

  /**
  *  @brief  通常newが使用するヒープを設定する
  *  @note   CTR版のみ動作、WIN32の通常newではヒープシステム外でメモリ確保が行われているため。
  *  @note   この関数は通常new使用箇所の削除に目処が立ち次第、削除する可能性があります
  *
  *  @param[in]  pHeap  ヒープ。NULLで外す。(NULLの場合はHEAPID_SYSTEMが使用される)
  */
  static void SetDefaultNewUseHeap( HeapBase* pHeap )
  {
    m_defaultNewUseHeap = pHeap;
  }

  /**
  *  @brief  通常newが使用するヒープを取得する
  *
  *  @return ヒープ
  */
  static HeapBase* GetDefaultNewUseHeap( void )
  {
    if( m_defaultNewUseHeap )
    {
      return m_defaultNewUseHeap;
    }
    else
    {
      return GetHeapByHeapId( HEAPID_SYSTEM );
    }
  }


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
#if GFL_HEAP_DEBUG
  static void SetHioDumpDefault( bool isOn );
  static bool IsHioDumpDefault( void ) { return m_hioDumpDefault; }
#endif //GFL_HEAP_DEBUG

private:
  //ヒープ作成コア部分
  static HeapBase* CreateHeapCore( void *buffer , const HEAPID childId , const int size , const HeapType heapType , const bool isBuffer , void *param);
  //ヒープ削除コア(チェックフラグの有無のみ
  static void DeleteHeapCore( HeapBase *heap , bool isCheck = true );

  //管理データ
  static HEAP_MNG_DATA *m_heapMngData;
  //最大個数
  static int m_heapNum;

  //ローカルヒープ管理データ
  static HEAP_MNG_DATA *m_localHeapMngData;
  //ローカルヒープ最大個数
  static int m_localHeapNum;

  //管理外メモリをdeleteする際に使用する関数
  static DeleteFunc m_unmanagedDeleteFunc;

  //通常newが使用するヒープ(設定されていない場合はNULL)
  static HeapBase* m_defaultNewUseHeap;

#if GFL_HEAP_DEBUG
  static bool m_hioDumpDefault;
#endif //GFL_HEAP_DEBUG
};

}; //namespace heap
}; //namespace gfl2

#define GFL_HEAP_INDEX_TO_LOCAL_HEAP_ID(idx) (gfl2::heap::GFL_HEAP_LOCAL_ID_BASE-idx)
#define GFL_HEAP_LOCAL_HEAP_ID_TO_INDEX(id) (gfl2::math::Abs(id-gfl2::heap::GFL_HEAP_LOCAL_ID_BASE))

#define GFL_HEAP_HEAP_ID_TO_INDEX(heapId) (heapId<=gfl2::heap::GFL_HEAP_LOCAL_ID_BASE?GFL_HEAP_LOCAL_HEAP_ID_TO_INDEX(heapId):heapId)


#endif // __GFL_HEAPMANAGER_H__
