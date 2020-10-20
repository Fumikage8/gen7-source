
#if !defined(APPDEMOUTILITY_H_INCLUDED)
#define APPDEMOUTILITY_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   AppDemoUtility.h
 * @date   2015/08/27 Thu. 11:55:00
 * @author muto_yu
 * @brief  
 * @note   本utilityをStaticLib内に置くとSequenceViewSystem（.cro内クラス）への参照がLinkErrorとなる
 *         （安定して回避する方法が見つけられなかった）
 *         本クラスはAppDemo.cro内に配置するので、同様の理由で本Utilityは.cro内からアクセスするようにする
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_Macros.h>
#include  <types/include/gfl2_Typedef.h>
#include  <heap/include/gfl2_Heap.h>
#include  <ro/include/gfl2_RoManager.h>

#include  "DemoLib/SequencePlayer/include/SequencePlayerCore.h"

// =============================================================================
/**/
//==============================================================================


namespace App       {
namespace AppDemo   {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief  
  *  @date   2015/08/26 Wed. 14:55:01
  */
//==============================================================================
class AppDemoUtility
{
  GFL_FORBID_COPY_AND_ASSIGN(AppDemoUtility);
public:
  static const u32    INVALID_DEMO_GARCID = UINT_MAX;

  enum  HeapModel
  {
    HEAPMODEL_Full,
    HEAPMODEL_75_Percent,
    HEAPMODEL_Half,
    HEAPMODEL_Dendou, //殿堂入りは 6体ポケモン出すために Full + 2Mバイト必要 by pete さん
    
    HEAPMODEL_MAX
  };

  AppDemoUtility(gfl2::heap::HeapBase* pParentHeapBase, const HeapModel heapModel = HEAPMODEL_Full);
  virtual ~AppDemoUtility();



  /*
  */
  class IEventHandler
  {
  public:
    virtual ~IEventHandler(){}

    //------------------------------------------------------------------
    /**
      *   @brief    再生開始確認
      *             必要に応じpViewSystemに各種設定を行う（ポケモンIDなど）
      *   @return   再生するデモのgarcID、INVALID_DEMO_GARCIDを返すことで何も再生しない
      */
    //------------------------------------------------------------------
    virtual u32   AppDemoUtility_StartQuery(DemoLib::Sequence::SequenceViewSystem* pViewSystem) = 0;

    //------------------------------------------------------------------
    /**
      *  @brief    再生が終了した（解体はされていない）
      */
    //------------------------------------------------------------------
    virtual void  AppDemoUtility_OnSequenceEnd(DemoLib::Sequence::SequenceViewSystem* pViewSystem) = 0;
  };
  void  SetEventHandler(IEventHandler* pHandler)  {m_pHandler = pHandler;}



  /*
  */
  enum  Command
  {
    COMMAND_SystemSync,           /**<  待機ループの解除：        キー入力待ちなど    */
    COMMAND_SetSystemCancel,      /**<  デモ分岐フラグのセット：  進化キャンセルなど  */
    COMMAND_ResetSystemCancel,    /**<  デモ分岐フラグの解除：    進化キャンセルなど  */
    
    COMMAND_
  };
  void  SendCommand(const Command command);




  void  Update(void);
  void  PreDraw(void);
  void  Draw(const gfl2::gfx::CtrDisplayNo displayNo);
  void  ExitRequest(void)   {m_IsExitRequest  = true;}    /**<  終了リクエスト：再生が終了し次第システム終了へ遷移する。IsStable()で完了を検知  */
  bool  IsStable(void) const;                             /**<  終了処理の完了を確認する                                                        */

  void  SetCallBack(DemoLib::Sequence::SequenceCallBack* pCallback)   {m_pSequenceViewSystem->SetSystemCallBack(pCallback);}
  void  RemoveCallBack(void)                                          {m_pSequenceViewSystem->RemoveSystemCallBack();}

  
  int   GetCurrentFrame(void)         {return m_pSequenceViewSystem->GetCurrentFrame();}
  bool  IsAcceptedCancelCommand(void) {return m_pSequenceViewSystem->IsSystemCancel();}


  void  DumpHeap(void)
  {
#if PM_DEBUG
    if(m_pLocalHeapBase)      m_pLocalHeapBase->Dump();
    if(m_pSequenceViewSystem) m_pSequenceViewSystem->GetDevHeap()->Dump();
#endif
  }

protected:
  void  OnInitFirst(void);
  void  OnEndLast(void);
  bool  IsValidGARCID(const u32 garcID) const  {return  (garcID != INVALID_DEMO_GARCID);}

  HeapModel                                 m_HeapModel;
  gfl2::heap::HeapBase*                     m_pParentHeapBase;
  gfl2::heap::HeapBase*                     m_pLocalHeapBase;
  gfl2::ro::Module**                        m_ppROModules;
  DemoLib::Sequence::SequenceViewSystem*    m_pSequenceViewSystem;
  IEventHandler*                            m_pHandler;
  u32                                       m_Subseq;
  bool                                      m_IsExitRequest;
};




} /*  namespace AppDemo   */
} /*  namespace App       */
#endif  /*  #if !defined(APPDEMOUTILITY_H_INCLUDED)  */
