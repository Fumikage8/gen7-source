//==============================================================================
/**
 @file    ZukanFrameSceneBase.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.27
 @brief   図鑑のシーンのフレームの基底
 */
//==============================================================================


// 図鑑のインクルード
#include "ZukanFrameSceneBase.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


ZukanFrameSceneBaseCell::ZukanFrameSceneBaseCell(void)
  : applib::frame::CellSuper(),
    m_param(),
    m_seqCount(0)
{}
ZukanFrameSceneBaseCell::~ZukanFrameSceneBaseCell()
{}

applib::frame::Result ZukanFrameSceneBaseCell::InitFunc(void)
{
  return applib::frame::RES_FINISH;
}
applib::frame::Result ZukanFrameSceneBaseCell::UpdateFunc(void)
{
  return applib::frame::RES_FINISH;
}
void                  ZukanFrameSceneBaseCell::DrawFunc(gfl2::gfx::CtrDisplayNo /*no*/)
{}
applib::frame::Result ZukanFrameSceneBaseCell::EndFunc(void)
{
  return applib::frame::RES_FINISH;
}

void ZukanFrameSceneBaseCell::Setup(const ZukanSceneParam& param)
{
  m_param = param;
}

void ZukanFrameSceneBaseCell::Req(s32 /*reqId*/)
{}

void ZukanFrameSceneBaseCell::changeSeq(int seq)
{
  this->SetSubSeq(seq);  // 親であるapplib::frame::CellSuperのメンバ
  m_seqCount = 0; 
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

