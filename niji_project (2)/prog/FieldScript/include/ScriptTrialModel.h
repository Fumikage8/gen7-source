//==============================================================================
/**
 * @file        ScriptTrialModel.h
 * @brief       要約説明
  
    このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/09/09(水) 17:10:06
 */
//==============================================================================

#if !defined(__SCRIPTTRIALMODEL_H__)  // 重複定義防止
#define __SCRIPTTRIALMODEL_H__  // 重複定義防止マクロ
#pragma once

#include <fs/include/gfl2_Fs.h>
#include <GameSys/include/GameManager.h>

// TrialModel
#include "Field/FieldRo/include/TrialModel/FieldTrialModelFactory.h"
#include "Field/FieldRo/include/TrialModel/FieldTrialModel.h"


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( FieldScript )

//------------------------------------------------------------------------------
/**
 * @class       ScriptTrialModel
 * @brief       
 * @author      N.Takeda
 * @date        2015/09/09(水) 17:10:43
 */
//------------------------------------------------------------------------------
class ScriptTrialModel
{
private:
  enum STATE
  {
    STATE_DONE,
    STATE_LOAD,
    STATE_INITWAIT,
    STATE_OPEN,
    STATE_OPENWAIT,
    STATE_LOADWAIT,
    STATE_CREATEWAIT,
    
    STATE_UNLOAD_FREE,
    STATE_UNLOAD_FREEWAIT,
    STATE_UNLOAD_DEL,
    STATE_UNLOAD_DELWAIT,
    STATE_MAX,
  };
  struct StResource
  {
    s32                                 m_iDatId;
    s32                                 m_iMonsNo;
    s32                                 m_iFormNo;
    void                                *m_pBuff;
    Field::TrialModel::FieldTrialModel  *m_pModel;
    gfl2::gfx::IGLAllocator             *m_pAllocator;
  };
  enum{ RESOURCE_MAX = 2 };
  
  GameSys::GameManager                  *m_pGameMan;
  gfl2::heap::HeapBase                  *m_pHeap;
  gfl2::heap::HeapBase                  *m_pSysHeap;
  
  Field::TrialModel::TrialModelFactory  *m_pFactory;
  STATE                                 m_eState;
  bool                                  m_bInitialized;
  
  StResource                            m_stResource[ RESOURCE_MAX ];
  s32                                   m_iResourceNum;
  
public:
  /// 読み込み関数.
  s32 Load( const s32 _iId, const s32 _iDatId, const s32 _iMonsNo,const s32 _iFormNo );
  void  UnLoad();
  s32 Del( const s32 _iId );
  
  bool  IsDone() const { return (m_eState == STATE_DONE); }
  
  ///        アニメーション切り替え.
  void  PlayAnim( const s32 _iId, const s32 _iAnim );
  bool  PlayIsEnd( const s32 _iId );
  /// 表示切替.
  void  Disp( const s32 _iId, const bool _bDisp );
  /// 影の表示,非表示.
  void  SetShadowVisible( const s32 _iId, const bool _bDisp );
  
  /// 更新.
  void  Update();
  /// 破棄.
  void  Tarminate();
  
  /// コンストラクタ.
  ScriptTrialModel( gfl2::heap::HeapBase * p_heap, GameSys::GameManager *pGameMan );
  virtual ~ScriptTrialModel();
  Field::TrialModel::FieldTrialModel* GetTrialMode( s32 _iId) { return m_stResource[_iId].m_pModel; }
};

GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )

#endif // __SCRIPTTRIALMODEL_H__ 重複定義防止
