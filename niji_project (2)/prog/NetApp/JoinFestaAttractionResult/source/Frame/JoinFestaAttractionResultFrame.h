// ============================================================================
/*
 * @file JoinFestaAttractionResultFrame.h
 * @brief アトラクション結果アプリの○○プロセスです。
 * @date 2015.11.02
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONRESULT_FRAME_H_INCLUDE )
#define JOINFESTAATTRACTIONRESULT_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <math/include/gfl2_math_random.h>
#include "NetApp/JoinFestaAttractionResult/source/Frame/JoinFestaAttractionResultFrameBase.h"
#include "NetApp/JoinFestaAttractionResult/source/System/JoinFestaAttractionResultWork.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListMessageMenuLowerViewListener.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRankingLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionResult)
GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaAttractionResultFrame : public NetApp::JoinFestaAttractionResult::Frame::JoinFestaAttractionResultFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaAttractionResultFrame );
public:
  JoinFestaAttractionResultFrame(
    NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultWork* pJoinFestaAttractionResultWork,
    NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultViewSystemAccessor* pJoinFestaAttractionResultViewSystemAccessor );
  virtual~JoinFestaAttractionResultFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

  u32 randomScore( u32 frameCount, u32 targetScore, u32 digitMax );

  u32 getDigit( u32 num );

  void checkPlayME();

private:

  NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener   m_JoinFestaPlayerListMessageMenuLowerViewListener;
  NetAppLib::JoinFestaUI::JoinFestaRankingLowerViewListener                 m_JoinFestaRankingLowerViewListener;
  u32                                                                       m_FrameCount;
  u32                                                                       m_TotalScore;
  u32                                                                       m_MyScore;
  u32                                                                       m_TotalScoreDigit;
  u32                                                                       m_MyScoreDigit;
  u32                                                                       m_DigitMax;
  u32                                                                       m_TotalStarDrawNum;
  u32                                                                       m_MyStarDrawNum;
  u32                                                                       m_StarDrawMax;
  u32                                                                       m_GetFesCoin;
  u32                                                                       m_GetFesCoinDigit;
  bool                                                                      m_bIsTotalScoreUpdate;
  bool                                                                      m_bIsMyScoreUpdate;
  gfl2::math::Random                                                        m_Random;
  gfl2::str::StrBuf                                                         m_MessageStrBuf;
  bool                                                                      m_bIsMePlaying;
  u32                                                                       m_PlayME;
  bool                                                                      m_bIsFinishRequest;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaAttractionResult)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAATTRACTIONRESULT_FRAME_H_INCLUDE
