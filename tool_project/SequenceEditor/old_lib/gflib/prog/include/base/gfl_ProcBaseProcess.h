#ifndef __GFL_PROCBASEPROCESS_H__
#define __GFL_PROCBASEPROCESS_H__
//=============================================================================
/**
 * @file	 gfl_ProcBaseProcess.h
 * @brief	 プロセス基底クラス
 * @author  k.ohno
 * @date	 2010.10.18
 */
//=============================================================================
#pragma once

#include <nn.h>
#include <ro/gfl_RoManager.h>
#include <gfl_Macros.h>
#include <gfl_Heap.h>

namespace gfl {
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

  #if GFL_ENABLE_RO
  nn::ro::Module*      m_pModule;         //!< 動的モジュール(自身）
  nn::ro::Module**     m_pChildModule;    //!< 子の動的モジュール
  u32                  m_pChildModuleNum; //!< 子の動的モジュール数
  #endif

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  BaseProcess(void) : m_subseq(0), mbEnd(false), parent(NULL)
  #if GFL_ENABLE_RO
  , m_pModule(NULL), m_pChildModule(NULL), m_pChildModuleNum(0)
  #endif
  {};

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~BaseProcess()
  {
    #if GFL_ENABLE_RO
    if( m_pChildModule != NULL )
    {
      GflHeapFreeMemory(m_pChildModule);
      m_pChildModule = NULL;
    }
    #endif
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
   * @brief	  PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  virtual void   DrawFunc(Manager* pManager){GFL_UNUSED(pManager);};
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


  #if GFL_ENABLE_RO
public:
  //------------------------------------------------------------------
  /**
   * @brief   動的モジュールポインタの登録
   */
  //------------------------------------------------------------------
  void SetModulePtr( nn::ro::Module* pModule ){ m_pModule = pModule; }

  //------------------------------------------------------------------
  /**
   * @brief   登録されている動的モジュールポインタの取得
   */
  //------------------------------------------------------------------
  nn::ro::Module* GetModulePtr( void ){ return m_pModule; }

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
  void CreateChildModulePtrBuffer( gfl::heap::HeapBase* heap, u32 num )
  {
    if( m_pChildModule == NULL )
    {
      m_pChildModule = (nn::ro::Module**)GflHeapAllocMemory(heap, sizeof(nn::ro::Module*)*num );
      m_pChildModuleNum = num;
    }
  }

  //------------------------------------------------------------------
  /**
   * @brief   子となる動的モジュールポインタの登録
   */
  //------------------------------------------------------------------
  void SetChildModulePtr( u32 index, nn::ro::Module* pModule )
  {
    GFL_ASSERT_STOP( index < m_pChildModuleNum );  //@fix
    m_pChildModule[index] = pModule;
  }

  //------------------------------------------------------------------
  /**
   * @brief   子して登録されている動的モジュールポインタの取得
   */
  //------------------------------------------------------------------
  nn::ro::Module* GetChildModulePtr( u32 index ){ return m_pChildModule[index]; }



  #endif

};

}  //proc
} //gfl

#endif // __GFL_PROCBASEPROCESS_H__
