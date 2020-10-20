#ifndef GFL2_PROC_BASE_H__
#define GFL2_PROC_BASE_H__
//=============================================================================
/**
 * @file	 gfl2_ProcBase.h
 * @brief	 プロセス基底クラス
 * @author k.ohno
 * @date	 2014.1.16
 */
//=============================================================================
#pragma once

#include <ro/include/gfl2_RoManager.h>
#include "macro/include/gfl2_Macros.h"
#include "heap/include/gfl2_Heap.h"

// gfl2
#include <gfx/include/gfl2_types.h>   // for displayNo

namespace gfl2 {
namespace proc {

class Manager;


//------------------------------------------------------------------
/**
 * @brief	プロセス動作関数の結果
 */
//------------------------------------------------------------------
typedef enum
{
  RES_CONTINUE,		///<動作継続中
  RES_FINISH,				///<動作終了

  ENUM_FORCE_DWORD(Result)  ///< 大きさをそろえる  ????_FORCE_DWORD
}Result;


typedef enum
{
  RES_FF,		///<動作継続中
  ENUM_FORCE_DWORD(Result2)  ///< 大きさをそろえる
}Result2;



class BaseProcess
{
  GFL_FORBID_COPY_AND_ASSIGN(BaseProcess);

protected:
  int m_subseq;					      ///< プロセス関数の動作シーケンス
  bool mbEnd;  //Endフラグ

private:
  BaseProcess* parent;			///< 上位プロセスへのポインタ

  #if GFL_RO_ENABLE
  gfl2::ro::Module*      m_pModule;         //!< 動的モジュール(自身）
  gfl2::ro::Module**     m_pChildModule;    //!< 子の動的モジュール
  u32                  m_pChildModuleNum; //!< 子の動的モジュール数
  #endif//GFL_RO_ENABLE

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  BaseProcess(void) : m_subseq(0), mbEnd(false), parent(NULL)
  #if GFL_RO_ENABLE
  , m_pModule(NULL), m_pChildModule(NULL), m_pChildModuleNum(0)
  #endif//GFL_RO_ENABLE
  {};

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~BaseProcess()
  {
    #if GFL_RO_ENABLE
    if( m_pChildModule != NULL )
    {
      GflHeapFreeMemory(m_pChildModule);
      m_pChildModule = NULL;
    }
    #endif//GFL_RO_ENABLE
  };

  //------------------------------------------------------------------
  /**
   * @brief	  プロセスの過程（現在の状態）を返す
   * @return	ProcSeqENUM
   */
  //------------------------------------------------------------------
  inline int GetSubSeq(void) const { return m_subseq; };
  //------------------------------------------------------------------
  /**
   * @brief	  プロセスの過程（現在の状態）を更新する
   * @return	ProcSeqENUM
   */
  //------------------------------------------------------------------
  inline void SetSubSeq(int seq){  m_subseq = seq; };
  //------------------------------------------------------------------
  /**
   * @brief	  プロセスの過程（現在の状態）をリセットする
   * @return	ProcSeqENUM
   */
  //------------------------------------------------------------------
  inline void ResetSubSeq(void){  m_subseq = 0; };
  //------------------------------------------------------------------
  /**
   * @brief	  プロセスの過程（現在の状態）を１つ先に進める
   * @return	ProcSeqENUM
   */
  //------------------------------------------------------------------
  inline void AddSubSeq(void){  m_subseq++; };


public:


  //------------------------------------------------------------------
  /**
   * @brief	  PROCの初期化を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  virtual Result InitFunc(Manager* pManager) = 0;
  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン更新を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  virtual Result UpdateFunc(Manager* pManager) = 0;

  //------------------------------------------------------------------
  /**
  * @brief	PROCの描画処理(下準備)
  * @param pManager   プロセス管理マネージャー
  * 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
  * 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
  */
  //------------------------------------------------------------------
  virtual void   PreDrawFunc(Manager* pManager ){GFL_UNUSED(pManager);};

  //------------------------------------------------------------------
  /**
  * @brief	PROCの描画処理
  * @param pManager   プロセス管理マネージャー
  * @param displayNo  描画対象画面
  * 登録したPROCによる、フレームバッファへの描画を実行します。
  * ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
  */
  //------------------------------------------------------------------
  virtual void   DrawFunc(Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo ){GFL_UNUSED(pManager);};

  //------------------------------------------------------------------
  /**
   * @brief	  PROCの終了を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  virtual Result EndFunc(Manager* pManager) = 0;
  //---------------------------------------------------------------------------------
  /**
   * @brief   このProcに制御があったら終了するように指示
   */
  //---------------------------------------------------------------------------------
  void SetProcEnd(void){ mbEnd = true; };


  #if GFL_RO_ENABLE
public:
  //------------------------------------------------------------------
  /**
   * @brief   動的モジュールポインタの登録
   */
  //------------------------------------------------------------------
  void SetModulePtr( gfl2::ro::Module* pModule ){ m_pModule = pModule; }

  //------------------------------------------------------------------
  /**
   * @brief   登録されている動的モジュールポインタの取得
   */
  //------------------------------------------------------------------
  gfl2::ro::Module* GetModulePtr( void ){ return m_pModule; }

public:

  //------------------------------------------------------------------
  /**
   * @brief   子となる動的モジュールのバッファサイズを取得
   */
  //------------------------------------------------------------------
  u32 GetChildModulePtrBufferNum( void ){ return m_pChildModuleNum; }

  //------------------------------------------------------------------
  /**
   * @brief   子となる動的モジュールを格納するためのバッファを確保
   */
  //------------------------------------------------------------------
  void CreateChildModulePtrBuffer( gfl2::heap::HeapBase* heap, u32 num )
  {
    if( m_pChildModule == NULL )
    {
      m_pChildModule = (gfl2::ro::Module**)GflHeapAllocMemory(heap, sizeof(gfl2::ro::Module*)*num );
      m_pChildModuleNum = num;
    }
  }

  //------------------------------------------------------------------
  /**
   * @brief   子となる動的モジュールポインタの登録
   */
  //------------------------------------------------------------------
  void SetChildModulePtr( u32 index, gfl2::ro::Module* pModule )
  {
    //後からDLLを追加する際の対応のためindexを自動で調べるようにしました。 160105 Ariizumi

    GFL_ASSERT_STOP( index < m_pChildModuleNum );  //@fix
    for( int i=0;i<m_pChildModuleNum;i++ )
    {
      if( m_pChildModule[i] == NULL )
      {
        m_pChildModule[i] = pModule;
        return;
      }
    }
    GFL_ASSERT_STOP( 0 );  //@fix バッファオーバーフォロー不可
  }

  //------------------------------------------------------------------
  /**
   * @brief   子して登録されている動的モジュールポインタの取得
   */
  //------------------------------------------------------------------
  gfl2::ro::Module* GetChildModulePtr( u32 index ){ return m_pChildModule[index]; }

#else//GFL_RO_ENABLE
  //ダミー
  void SetModulePtr( gfl2::ro::Module* pModule ){}
  gfl2::ro::Module* GetModulePtr( void ){ return NULL; }
  u32 GetChildModulePtrBufferNum( void ){ return 0; }
  void CreateChildModulePtrBuffer( gfl2::heap::HeapBase* heap, u32 num ){}
  void SetChildModulePtr( u32 index, gfl2::ro::Module* pModule ){}
  gfl2::ro::Module* GetChildModulePtr( u32 index ){ return NULL; }
#endif//GFL_RO_ENABLE

};

}  //proc
} //gfl2

#endif // __GFL_PROCBASEPROCESS_H__
