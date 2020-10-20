// ============================================================================
/*
 * @file JoinFestaAttractionResultUpperView.h
 * @brief アトラクション結果アプリの上画面を表示するクラスです。
 * @date 2015.11.02
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONRESULTUPPERVIEW_H_INCLUDE )
#define JOINFESTAATTRACTIONRESULTUPPERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetApp/JoinFestaAttractionResult/source/System/JoinFestaAttractionResultWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionResult)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaAttractionResultUpperView :
  public NetAppLib::System::NetApplicationViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaAttractionResultUpperView );
public:
  JoinFestaAttractionResultUpperView( NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultWork* pJoinFestaAttractionResultWork );
  virtual~JoinFestaAttractionResultUpperView();

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  void SetTotalScore( u32 score, u32 digit );
  void SetMyScore( u32 score, u32 digit );
  void SetGetFesCoin( u32 fesCoin, u32 digit );

  void SetVisibleScore( bool bVisible );
  void SetVisibleGetFesCoin( bool bVisible );
  void SetVisibleTotalStarIcon( bool bVisible, int index = -1 );
  void SetVisibleMyStarIcon( bool bVisible, int index= -1 );
  void SetVisibleTotalScoreUpdateMessage( bool bVisible );
  void SetVisibleMyScoreUpdateMessage( bool bVisible );

  void SetupAttractionResultInfo();

  void StartTotalStarIconAnimation( u32 starNum );
  void StartMyStarIconAnimation( u32 starNum );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_JoinFestaAttractionResult,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_UPPER,
  };

private:

  NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultWork*                         m_pJoinFestaAttractionResultWork;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaAttractionResult)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAATTRACTIONRESULTUPPERVIEW_H_INCLUDE
