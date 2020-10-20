//=============================================================================
/**
 * File:   gfl2_RoManager.h
 * Author: Hisanao Suzuki
 *
 * Created on 2011/11/18
 * 動的モジュール管理クラス
 *  150227 niji対応 ariizumi
 **/
//=============================================================================
#ifndef __GFL2_RO_MANAGER_H__
#define __GFL2_RO_MANAGER_H__

#pragma once

#ifdef GF_PLATFORM_CTR
#include <nn.h>
#include <nn/ro.h>
#endif //GF_PLATFORM_CTR
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <base/include/gfl2_Singleton.h>

//前方宣言

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)
class AsyncFileManager;
GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(ro)


#if GFL_RO_ENABLE
typedef nn::ro::Module Module;

// シンボルを明示的に公開
#define GFL_DLL_EXPORT NN_DLL_EXPORT
// シンボルへの参照を明示的に生成
#define GFL_DLL_IMPORT NN_DLL_IMPORT

typedef enum
{
  FIX_LEVEL_0 = nn::ro::FIX_LEVEL_0,    //!< FIX_LEVEL_NONE の別名です。
  FIX_LEVEL_1 = nn::ro::FIX_LEVEL_1,    //!< FIX_LEVEL_INTERNAL の別名です。
  FIX_LEVEL_2 = nn::ro::FIX_LEVEL_2,    //!< FIX_LEVEL_INTERNAL_IMPORT の別名です。
  FIX_LEVEL_3 = nn::ro::FIX_LEVEL_3,    //!< FIX_LEVEL_INTERNAL_IMPORT_EXPORT の別名です。
}FixLevel;
#else
typedef void Module;
#define GFL_DLL_EXPORT
#define GFL_DLL_IMPORT
typedef enum
{
  FIX_LEVEL_0,
  FIX_LEVEL_1,
  FIX_LEVEL_2,
  FIX_LEVEL_3,
}FixLevel;
#endif

// =========================================================================
// DEFINE
// =========================================================================
#define DEFAULT_MODULE_MAX  32              // デフォルト登録可能モジュール数



// =========================================================================
//! @brief  動的モジュール管理クラス
// =========================================================================
class RoManager : public gfl2::base::Singleton<RoManager>
{
  GFL_FORBID_COPY_AND_ASSIGN(RoManager);

#if GFL_RO_ENABLE

private:
  void*                     m_crsBuffer;      //!< CRSファイル用バッファ
  void*                     m_crrBuffer;      //!< CRRファイル用バッファ
  nn::ro::RegistrationList* m_registList;     //!< 登録モジュール一覧
  Module**          m_modules;        //!< モジュールリスト
  bit8**                    m_bssBuffer;      //!< .data/.bssバッファ
  void**                    m_moduleBuffer;   //!< モジュール用バッファ
  gfl2::heap::HeapBase*     m_heapForModules; //!< モジュール生成用ヒープ
  u32                       m_moduleMax;      //!< 登録可能なモジュール数
  u32                       m_registNum;      //!< 登録モジュール数
  bool                      m_isInit;         //!< 初期フラグ

private:
  //---------------------------------------------------------------------
  /**
  * @brief          動的モジュールの解放
  *
  * @param[in,out]  targetModule 解放対象となる動的モジュール
  *
  * @return 登録リストのインデックス値。0より小さい値の場合は未検出。
  */
  // ---------------------------------------------------------------------
  s32 GetModuleIndex( Module* targetModule );

  //---------------------------------------------------------------------
  /**
  * @brief  ファイル読み込み処理
  *
  * @param[in]  heap        ヒープ管理クラスへのポインタ
  * @param[in]  fileName    読み込みたいファイル名
  * @param[in]  size        ファイルサイズを格納する先のポインタ
  *
  * @return 読み込まれたファイル内容を格納したバッファへのポインタ
  */
  //---------------------------------------------------------------------
  void* LoadFile( gfl2::heap::HeapBase* heap, gfl2::fs::AsyncFileManager *fileMng , const char* fileName, size_t *size );

public:

  //---------------------------------------------------------------------
  /**
  * @brief  コンストラクタ
  *
  * @param[in]  modulesHeap DLLモジュール生成時に使用されるヒープエリア
  */
  // ---------------------------------------------------------------------
  RoManager( gfl2::heap::HeapBase* modulesHeap );

  //---------------------------------------------------------------------
  /**
  * @brief  デストラクタ
  */
  // ---------------------------------------------------------------------
  virtual ~RoManager( void );

public:
  //---------------------------------------------------------------------
  /**
  * @brief  初期化処理
  *
  * @param[in]  heap        ヒープ管理クラスへのポインタ
  * @param[in]  crsFileName CRSファイル名
  * @param[in]  crrFileName CRRファイル名
  * @param[in]  moduleMax   登録可能モジュール数
  */
  // ---------------------------------------------------------------------
  void Initialize( gfl2::heap::HeapBase* heap, gfl2::fs::AsyncFileManager *fileMng , const char* crsFileName, const char* crrFileName, u32 moduleMax=DEFAULT_MODULE_MAX );

  //---------------------------------------------------------------------
  /**
  * @brief  動的モジュールのロード
  *
  * @param[in]  crsFileName CRSファイル名
  * @param[in]  crrFileName CRRファイル名
  * @param[in]  fixLevel    モジュール解放時のメモリの解放段階。
  * @param[in]  loadHeap    読み込みに使うheap(NULLでInitializeで渡したHeapをつかいます。)
  *                         ※Deviceメモリは使えません！
  * @param[in]  fixLevel    モジュール解放時のメモリの解放段階。
  *
  * @par   fixLevelについて
  *         - 詳しくは、以下のAPIリファレンスを参照してください。
  *         - [/CTR_SDK/documents/api/nn/ro/FixLevel.html]
  */
  // ---------------------------------------------------------------------
  Module* LoadModule( gfl2::fs::AsyncFileManager *fileMng , const char* croFileName,gfl2::heap::HeapBase* loadHeap = NULL, FixLevel fixLevel=FIX_LEVEL_1 );
  Module* LoadModuleLower( gfl2::fs::AsyncFileManager *fileMng , const char* croFileName,gfl2::heap::HeapBase* loadHeap = NULL, FixLevel fixLevel=FIX_LEVEL_1 );

  //---------------------------------------------------------------------
  /**
  * @brief  動的モジュールのロード(バッファ渡し)
  * @param  croDataPtr  モジュールのデータ
  * @param  croFileSize モジュールのデータサイズ
  * @param  loadHeap    DLLリンク用ヒープ(NULLでデフォルト)
  * @param  fixLevel    モジュール解放時のメモリの解放段階。
  * @return 無し
  */
  // ---------------------------------------------------------------------
  Module* LoadModuleSetBuffer( void* croDataPtr , size_t croFileSize,gfl2::heap::HeapBase* loadHeap = NULL, FixLevel fixLevel=FIX_LEVEL_1 );

  //---------------------------------------------------------------------
  /**
  * @brief  動的モジュールを開始する
  *
  * @param[in]  targetModule 開始対象となる動的モジュール
  * @param[in]  isLinkCheck  シンボルリンクが解決されているかを厳密にチェックするならtrueを指定
  *
  * @attention 実行に失敗する条件である場合はASSERTで停止させる
  */
  // ---------------------------------------------------------------------
  void StartModule( Module* targetModule, bool isLinkCheck=false );

  //---------------------------------------------------------------------
  /**
  * @brief  動的モジュールを解放する
  *
  * @param[in]  targetModule 解放対象となる動的モジュール
  */
  // ---------------------------------------------------------------------
  void DisposeModule( Module* targetModule );

  //---------------------------------------------------------------------
  /**
  * @brief  登録されている全ての動的モジュールの解放
  */
  // ---------------------------------------------------------------------
  void DisposeAllModule( void );

  //---------------------------------------------------------------------
  /**
  * @brief  終了処理
  */
  // ---------------------------------------------------------------------
  void Finalize( void );



#if GFL_DEBUG
  //---------------------------------------------------------------------
  /**
  * @brief  デバッグ処理：現在読み込み中のDLLリストを取得する
  *
  * @param[out]  registNum  登録数を格納して返す
  *
  * @return Moduleのポインタの配列
  */
  // ---------------------------------------------------------------------
  Module**  DebugGetModules( u32 *registNum ){
    *registNum = m_registNum;
    return m_modules;
  };
#endif

#else //GFL_RO_ENABLE

  //ダミー
public:
  RoManager( gfl2::heap::HeapBase* modulesHeap ){};
  virtual ~RoManager( void ){};
  void Initialize( gfl2::heap::HeapBase* heap, gfl2::fs::AsyncFileManager *fileMng , const char* crsFileName, const char* crrFileName, u32 moduleMax=DEFAULT_MODULE_MAX ){};
  Module* LoadModule( gfl2::fs::AsyncFileManager *fileMng , const char* croFileName, gfl2::heap::HeapBase* loadHeap = NULL, FixLevel fixLevel=FIX_LEVEL_1 ){return NULL;};
  Module* LoadModuleLower( gfl2::fs::AsyncFileManager *fileMng , const char* croFileName, gfl2::heap::HeapBase* loadHeap = NULL, FixLevel fixLevel=FIX_LEVEL_1 ){return NULL;};
  Module* LoadModuleSetBuffer( void* croDataPtr , size_t croFileSize,gfl2::heap::HeapBase* loadHeap = NULL, FixLevel fixLevel=FIX_LEVEL_1 ){return NULL;};
  void StartModule( Module* targetModule, bool isLinkCheck=false ){};
  void DisposeModule( Module* targetModule ){};
  void DisposeAllModule( void ){};
  void Finalize( void ){};



#if GFL_DEBUG
  Module**  DebugGetModules( u32 *registNum ){}
#endif
#endif //GFL_RO_ENABLE


};

//---------------------------------------------------------------------
/**
* @brief  動的モジュールのロード（関数版）
*
* @param[in]  croFileName CROファイル名
* @param[in]  is_lower    ヒープの下からとる
*
* @return 動的モジュールへのポインタ
*/
// ---------------------------------------------------------------------
extern Module* StartModuleFunc( gfl2::fs::AsyncFileManager *fileMng , const char* croFileName, bool is_lower = false );

GFL_NAMESPACE_END(ro)
GFL_NAMESPACE_END(gfl2)

#if GFL_RO_ENABLE
//---------------------------------------------------------------------
/**
* @brief  SINGLETONの前方参照宣言
*/
// ---------------------------------------------------------------------
SINGLETON_INSTANCE_DECLARATION( gfl2::ro::RoManager );
#endif //GFL_RO_ENABLE


#endif // __GFL2_RO_MANAGER_H__
