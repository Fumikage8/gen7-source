//==============================================================================
/**
 * @file        GameOverProcParam.h
 * @brief       要約説明
  
    このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/07/23(木) 22:10:26
 */
//==============================================================================

#if !defined(__GAMEOVERPROCPARAM_H__) // 重複定義防止
#define __GAMEOVERPROCPARAM_H__ // 重複定義防止マクロ
#pragma once


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN(Savedata)
  class MyStatus;
GFL_NAMESPACE_END(Savedata)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(GameOver)


//==============================================================================
/**
 @class     GameOverProcParam
 @brief     図鑑のProcに渡す引数
 */
//==============================================================================
class GameOverProcParam
{
public:
  s32                 m_iMsgId;
  Savedata::MyStatus  *m_pStatus;

public:
  GameOverProcParam(void)
    : m_iMsgId(0)
    , m_pStatus( NULL ) ///< Coverity.10743
  {}
};


GFL_NAMESPACE_END(GameOver)
GFL_NAMESPACE_END(app)

#endif // __GAMEOVERPROCPARAM_H__ 重複定義防止
