#ifndef __APP_FRAME_CELL_SUPER_H__
#define __APP_FRAME_CELL_SUPER_H__
//=============================================================================
/**
 * @file	 AppFrameCellSuper.h
 * @brief	 プロセス基底クラス
 * @author k.ohno
 * @date	 2014.1.16
 */
//=============================================================================
#pragma once

#include "macro/include/gfl2_Macros.h"
#include "heap/include/gfl2_Heap.h"
#include "gfx/include/gfl2_types.h"

// AppFrameCellを継承したクラスを作成　　一個       
// HEAPID_APP HEAPID_APPDEV メモリーから必要な領域をCREATEする事  == Heap::Create
// 


GFL_NAMESPACE_BEGIN( applib )
GFL_NAMESPACE_BEGIN( frame )

class Manager;

//------------------------------------------------------------------
/**
 * @brief	プロセス動作関数の結果
 */
//------------------------------------------------------------------
typedef enum
{
  RES_CONTINUE,		///<動作継続中
  RES_NEXT,				 ///<動作終了 フレームをあけずに続きの処理を行う
  RES_FINISH,				///<動作終了
}Result;



class CellSuper
{
  GFL_FORBID_COPY_AND_ASSIGN(CellSuper);

protected:
  int m_subseq;					      ///< プロセス関数の動作シーケンス（デバッグで表示するので使用お願いします）
  bool mbEnd;  //Endフラグ

private:
  CellSuper* pParent;	   		///< 上位プロセスへのポインタ
  CellSuper* pParallel;			///< 並列なもののプロセス
  Manager* mpManager;    ///< このCELLを登録したManager  MENU_FRAME_CALL_MODALやMENU_FRAME_CALL_MODELESSの呼び出しだけに使用してください

protected:
  Manager* GetManager() { return mpManager; }

public:
  //------------------------------------------------------------------
  /**
  
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  CellSuper(void) : m_subseq(0), mbEnd(false)
  {};

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~CellSuper()
  {
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
  virtual Result InitFunc(void) = 0;
  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン更新を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  virtual Result UpdateFunc(void) = 0;
  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  virtual void   DrawFunc(gfl2::gfx::CtrDisplayNo no){};
  //------------------------------------------------------------------
  /**
   * @brief	  PROCの終了を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  virtual Result EndFunc(void) = 0;
  //------------------------------------------------------------------
  /**
   * @brief	   描画できる状態になった事を返すための関数
   * @return	 描画できる状態になった場合trueを返す
   */
  //------------------------------------------------------------------
  virtual bool IsDrawEnable(void){ return false; }

  //---------------------------------------------------------------------------------
  /**
   * @brief   このProcに制御があったら終了するように指示
   */
  //---------------------------------------------------------------------------------
  void SetProcEnd(void){ mbEnd = true; };
  //---------------------------------------------------------------------------------
  /**
   * @brief   このCELLを登録したManagerの設定
   */
  //---------------------------------------------------------------------------------
  void SetMyManager(Manager* pManager){ mpManager = pManager; };

  //---------------------------------------------------------------------------------
  /**
   * @brief   このCELLを登録したManagerの設定
   */
  //---------------------------------------------------------------------------------
  Manager* GetMyManager(void){ return mpManager; };



  
};

GFL_NAMESPACE_END( frame )
GFL_NAMESPACE_END( applib )

#endif // __APP_FRAME_CELL_SUPER_H__




