#if 0 // 未実装
//=============================================================================
/**
 * @file    NewPaneListPartsV.h
 * @brief   ペインリスト項目関連処理（縦リスト）
 * @author  Hiroyuki Nakamura
 * @date    2015.07.22
 */
//=============================================================================
#if !defined( NEW_PANE_LIST_PARTS_V_H_INCUDED )
#define NEW_PANE_LIST_PARTS_V_H_INCUDED

#pragma once

#include "NewPaneListPartsBase.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

//=============================================================================
/**
 * @class NewPaneListPartsV
 * @brief ペインリスト項目処理クラス（縦リスト）
 * @date  2015.07.22
 */
//=============================================================================
class NewPaneListPartsV
  : public NewPaneListPartsBase
{
  GFL_FORBID_COPY_AND_ASSIGN( NewPaneListPartsV );

public:
  NewPaneListPartsV(
    const NewPaneList::SETUP_PARAM * param,
    NewPaneList::ListListener * listener,
    app::tool::ButtonManager * button_manager,
    gfl2::lyt::LytPane * insert_pane,
    u8 * low_tbl,
    f32 scroll_max,
    NewPaneList::EffectMode effect );
  virtual ~NewPaneListPartsV();

  virtual void InitListPut(void);
  virtual f32 InitListPutBase( s32 mv );
  virtual f32 MoveList( s32 offs );

  virtual u32 GetPosPaneIndex( int pos );
  virtual s32 GetPosPaneLow( u32 index );

  virtual void ChangeButtonEnable(void);

  virtual NewPaneListPartsBase::InputID CheckKeyInput(void);
  virtual NewPaneListPartsBase::InputID CheckKeyInput( gfl2::ui::VectorDevice * key );
  virtual NewPaneListPartsBase::InputID CheckKeyInputSkip( gfl2::ui::VectorDevice * key );
  virtual bool IsKeyHold( NewPaneListPartsBase::InputID id );

  virtual void StartItemChangeScroll( s32 mv );
  virtual void MoveChangeList( s32 offs );
  virtual void AdjustChangeList(void);
  virtual void EndItemChangeScroll(void);
  virtual void InitListPutItemChangeLoop(void);
  virtual void InitListPutItemChangeSkip(void);


private:
  virtual u32 CalcEffectParam( u32 index );
};

GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

#endif  // NEW_PANE_LIST_PARTS_V_H_INCUDED

#endif
