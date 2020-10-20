#if !defined(GFL2_FSDEBUG_H_INCLUDED)
#define GFL2_FSDEBUG_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_FsDebug.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   ファイルシステムのデバッグ機能
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>



//==============================================================================
//
// 処理の流れをプリントするときは、これでプリントして下さい。
//
//==============================================================================
// 処理の流れをプリント
//#define  GFL_FSDEBUG_PRINT_SEQ(...)  GFL_PRINT(__VA_ARGS__)
#define  GFL_FSDEBUG_PRINT_SEQ(...)  ((void)0)

// 処理の流れで関数名をプリント
//#define  GFL_FSDEBUG_PRINT_SEQ_FUNC()  GFL_PRINT("%s\n",__FUNCTION__)
#define  GFL_FSDEBUG_PRINT_SEQ_FUNC()  ((void)0)



//==============================================================================
//
// 失敗に気付かせるためにASSERTで止める場合、これを使って下さい。
//
//==============================================================================
// AsyncFileManagerでの失敗に気付かせるために止める。気付かせるためのものなので、抜けても大丈夫なようにケアしておいて下さい。
#define  GFL_FSDEBUG_ASYNC_FILE_MANAGER_ATTENTION  GFL_ASSERT



#endif  // GFL2_FSDEBUG_H_INCLUDED

