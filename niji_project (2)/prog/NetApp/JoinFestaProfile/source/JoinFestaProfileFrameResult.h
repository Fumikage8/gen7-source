// ============================================================================
/*
 * @file JoinFestaProfileFrameResult.h
 * @brief プロフィールアプリのリザルトコードを定義します。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPROFILE_FRAMERESULT_H_INCLUDE )
#define JOINFESTAPROFILE_FRAMERESULT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)


enum SelectEditMenuFrameResultCode
{
  SELECT_EDIT_MENU_FRAME_RESULT_BACK,
  SELECT_EDIT_MENU_FRAME_RESULT_TO_DESCRIPTION,
  SELECT_EDIT_MENU_FRAME_RESULT_TO_NAME_INPUT
};


enum DescriptionFrameResultCode
{
  DESCRIPTION_FRAME_RESULT_BACK
};


enum NameInputFrameResultCode
{
  NAME_INPUT_FRAME_RESULT_BACK
};


GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPROFILE_FRAMERESULT_H_INCLUDE
