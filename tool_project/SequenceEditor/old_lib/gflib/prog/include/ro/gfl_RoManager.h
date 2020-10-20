//=============================================================================
/**
 * File:   gfl_RoManager.h
 * Author: Hisanao Suzuki
 *
 * Created on 2011/11/18
 * 動的モジュール管理クラス
 **/
//=============================================================================


#ifndef __GFL_RO_MANAGER_H__
#define __GFL_RO_MANAGER_H__

#pragma once

#include <nn.h>
#include <nn/ro.h>
#include <gfl_Macros.h>
#include <gfl_Heap.h>
#include <base/gfl_Singleton.h>


// =========================================================================
// RO有効フラグ(現状OFF)
// =========================================================================
#ifdef GFL_USE_RO
  #define GFL_ENABLE_RO 1

  // シンボルを明示的に公開
  #define GFL_DLL_EXPORT NN_DLL_EXPORT

  // シンボルへの参照を明示的に生成
  #define GFL_DLL_IMPORT NN_DLL_IMPORT
#else
  #define GFL_ENABLE_RO 0
  #define GFL_DLL_EXPORT
  #define GFL_DLL_IMPORT
#endif



#if GFL_ENABLE_RO

namespace gfl{

/**
* @namespace gfl::ro
* @brief DLL用ネームスペース
*/
namespace ro{



// =========================================================================
// DEFINE
// =========================================================================
#define DEFAULT_MODULE_MAX  32              // デフォルト登録可能モジュール数


// =========================================================================
//! @brief  動的モジュール管理クラス
// =========================================================================
class RoManager : public gfl::base::Singleton<RoManager>
{
  GFL_FORBID_COPY_AND_ASSIGN(RoManager);

private:
  void*                     m_crsBuffer;      //!< CRSファイル用バッファ
  void*                     m_crrBuffer;      //!< CRRファイル用バッファ
  nn::ro::RegistrationList* m_registList;     //!< 登録モジュール一覧
  nn::ro::Module**          m_modules;        //!< モジュールリスト
  bit8**                    m_bssBuffer;      //!< .data/.bssバッファ
  void**                    m_moduleBuffer;   //!< モジュール用バッファ
  gfl::heap::HeapBase*      m_heapForModules; //!< モジュール生成用ヒープ
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
  s32 GetModuleIndex( nn::ro::Module* targetModule );

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
  void* LoadFile( gfl::heap::HeapBase* heap, const char* fileName, size_t *size );

public:

  //---------------------------------------------------------------------
  /**
  * @brief  コンストラクタ
  *
  * @param[in]  modulesHeap DLLモジュール生成時に使用されるヒープエリア
  */
  // ---------------------------------------------------------------------
  RoManager( gfl::heap::HeapBase* modulesHeap );

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
  void Initialize( gfl::heap::HeapBase* heap, const char* crsFileName, const char* crrFileName, u32 moduleMax=DEFAULT_MODULE_MAX );

  //---------------------------------------------------------------------
  /**
  * @brief  動的モジュールのロード
  *
  * @param[in]  crsFileName CRSファイル名
  * @param[in]  crrFileName CRRファイル名
  * @param[in]  fixLevel    モジュール解放時のメモリの解放段階。
  *
  * @par   fixLevelについて
  *         - 詳しくは、以下のAPIリファレンスを参照してください。
  *         - [/CTR_SDK/documents/api/nn/ro/FixLevel.html]
  */
  // ---------------------------------------------------------------------
  nn::ro::Module* LoadModule( const char* croFileName, nn::ro::FixLevel fixLevel=nn::ro::FIX_LEVEL_1 );
  nn::ro::Module* LoadModuleLower( const char* croFileName, nn::ro::FixLevel fixLevel=nn::ro::FIX_LEVEL_1 );

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
  void StartModule( nn::ro::Module* targetModule, bool isLinkCheck=false );

  //---------------------------------------------------------------------
  /**
  * @brief  動的モジュールを解放する
  *
  * @param[in]  targetModule 解放対象となる動的モジュール
  */
  // ---------------------------------------------------------------------
  void DisposeModule( nn::ro::Module* targetModule );

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
  * @return nn::ro::Moduleのポインタの配列
  */
  // ---------------------------------------------------------------------
  nn::ro::Module**  DebugGetModules( u32 *registNum ){
    *registNum = m_registNum;
    return m_modules;
  };
#endif

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
extern nn::ro::Module* StartModuleFunc( const char* croFileName, bool is_lower = false );



} // namespace ro
} // namespace gfl


//---------------------------------------------------------------------
/**
* @brief  SINGLETONの前方参照宣言
*/
// ---------------------------------------------------------------------
SINGLETON_INSTANCE_DECLARATION( gfl::ro::RoManager );


#endif  // GFL_ENABLE_RO

#endif  // __GFL_RO_MANAGER_H__

