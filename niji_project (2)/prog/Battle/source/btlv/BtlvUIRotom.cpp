//======================================================================
/**
 * @file BtlvUIRotom.cpp
 * @date 2016/12/01 12:19:01
 * @author ariizumi_nobuhiko
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>

#include "DemoLib/Object/include/Object_Layout.h"

#include "btlv_core.h"
#include "BtlvUIRotom.h"
#include "btlv_seq_com_define.h"
#include "btlv_ui.h"

#include "niji_conv_header/app/battle/upper/rotom_target_pane.h"
#include "niji_conv_header/message/msg_btl_app.h"

#include <arc_index/battleUpperLyt.gaix>

GFL_NAMESPACE_BEGIN(btl)
GFL_NAMESPACE_BEGIN(app)

BtlvUiRotom::BtlvUiRotom(BattleViewSystem *core)
: mBtlvCore(core)
, mIsLoad(false)
, mIsCreate(false)
{

}

void BtlvUiRotom::StartLoad(void)
{
  if (mIsLoad == true)
  {
    return;
  }

  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId = ARCID_BATTLE_UPPER_LYT;
    openReq.heapForFile = mBtlvCore->GetTempHeap()->GetLowerHandle();
    openReq.heapForReq = mBtlvCore->GetTempHeap()->GetLowerHandle();
    mBtlvCore->GetAsyncFileManager()->AddArcFileOpenReq(openReq);
  }

  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
    loadReq.arcId = ARCID_BATTLE_UPPER_LYT;
    loadReq.datId = GARC_battleUpperLyt_rotom_target_applyt_COMP; //エラー対応のため無効化
    loadReq.align = 128;
    loadReq.heapForBuf = mBtlvCore->GetTempHeap();
    loadReq.heapForReq = mBtlvCore->GetTempHeap()->GetLowerHandle();
    loadReq.heapForCompressed = mBtlvCore->GetTempHeap()->GetLowerHandle();
    loadReq.ppBuf = &mDataBuf;
    mBtlvCore->GetAsyncFileManager()->AddArcFileLoadDataReq(loadReq);
  }

  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
    closeReq.arcId = ARCID_BATTLE_UPPER_LYT;
    closeReq.heapForReq = mBtlvCore->GetTempHeap()->GetLowerHandle();
    mBtlvCore->GetAsyncFileManager()->AddArcFileCloseReq(closeReq);
  }

  mIsLoad = true;
}

bool BtlvUiRotom::IsFinishLoad(void)
{
  if ( mIsLoad == false )
  {
    return false;
  }
  //ARCを初期化・開放する
  if (mBtlvCore->GetAsyncFileManager()->IsArcFileOpenFinished(ARCID_BATTLE_UPPER_LYT) == false)
  {
    return false;
  }
  if (mBtlvCore->GetAsyncFileManager()->IsArcFileLoadDataFinished(&mDataBuf) == false)
  {
    return false;
  }
  if (mBtlvCore->GetAsyncFileManager()->IsArcFileCloseFinished(ARCID_BATTLE_UPPER_LYT) == false)
  {
    return false;
  }

  return true;
}

void BtlvUiRotom::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  if (mIsCreate == false)
  {
    return;
  }
  if (displayNo == gfl2::gfx::CtrDisplayNo::LEFT)
  {
    //mLyt->UpdateAnimation(1.0f);  //勝手にフレーム数を固定してしまうので呼ばない
    mLyt->UpdateTree();             //代わりにこっちを呼ぶ
    mLyt->DrawLayout(displayNo);
  }
}

void BtlvUiRotom::Create(void)
{
  if (mIsCreate == true)
  {
    return;
  }

  gfl2::heap::HeapBase *heap = mBtlvCore->GetTempHeap();

  mAppHeap = GFL_NEW(heap)::app::util::Heap();
  mAppHeap->LocalHeapCreate(heap, heap, 0x10000, 0x30000);

  mLyt = GFL_NEW(heap)DemoLib::Obj::Object_Layout(mAppHeap, mBtlvCore->GetRenderSys()->GetAppRenderingManager(), 0, 3);
  mLyt->LoadLayout(mDataBuf);
  mLyt->AddRenderPath();

  mIsCreate = true;
}

void BtlvUiRotom::RemoveLayout(void)
{
  if (mIsCreate == false)
  {
    return;
  }

  mLyt->RemoveRenderPath();
}

void BtlvUiRotom::Delete(void)
{
  if (mIsCreate == false)
  {
    return;
  }

  mLyt->Delete();
  GFL_SAFE_DELETE(mLyt);
  GFL_SAFE_DELETE(mAppHeap);
  GflHeapSafeFreeMemory(mDataBuf);

  mIsCreate = false;
  mIsLoad = false;
}

void BtlvUiRotom::StartAnimation(int type)
{
  if (mIsLoad == false)
  {
    GFL_ASSERT_MSG(0,"Rotom is not load!\n"); //@check 開発中に気づく
    return;
  }

  if (mIsCreate == false)
  {
    Create();
  }

  switch (type)
  {
  case ROTOM_EFFECT_WILD:
    mLyt->SetAnimation(0);
    break;
  case ROTOM_EFFECT_LOOP:
    mLyt->SetAnimation(1);
    break;
  case ROTOM_EFFECT_CLOSE:
    mLyt->SetAnimation(2);
    break;
  }
}

void BtlvUiRotom::SetData(const BTL_POKEPARAM *bpp)
{
#if 0 //文字は出なくなった
  gfl2::str::MsgData *msgData = mBtlvCore->GetUiSys()->GetAppMessageData();
  gfl2::str::StrBuf *workStr = mBtlvCore->GetWorkStr(0);

  //番号
  int localZukanNo = PokeTool::ExtendData::GetLocalNumberStatic(bpp->GetMonsNo(), PokeTool::ExtendData::LOCAL_AREA_A);
  if (localZukanNo == PokeTool::ExtendData::LOCAL_NUMBER_NONE)
  {
    //地方図鑑番号外
    msgData->GetString(STR_ROTOM_INFO_NUM_UNKNOWN, *workStr);
    mLyt->GetG2DUtil()->SetTextBoxPaneString(0, PANENAME_BATTLE_ROTOM_TARGET_000_PANE_TEXTBOX_00, workStr);
  }
  else
  {
    msgData->GetString(STR_ROTOM_INFO_NUM, *workStr);
    mLyt->GetG2DUtil()->SetTextBoxPaneNumber(0, PANENAME_BATTLE_ROTOM_TARGET_000_PANE_TEXTBOX_00, workStr, localZukanNo, 3, 0, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT);
  }

  //ポケモン名
  pml::personal::GetMonsName(workStr, static_cast<MonsNo>(bpp->GetMonsNo()));
  mLyt->GetG2DUtil()->SetTextBoxPaneString(0, PANENAME_BATTLE_ROTOM_TARGET_000_PANE_TEXTBOX_01, workStr);
#endif
}


GFL_NAMESPACE_END(app)
GFL_NAMESPACE_END(btl)
