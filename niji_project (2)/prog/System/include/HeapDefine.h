#ifndef __H_HEAP_DEFINE_H__
#define __H_HEAP_DEFINE_H__
/**
* @file    HeapDefine.h
* @brief   nijiアプリケーション ヒープID定義
* @author  Miyuki Iwasawa 
* @date    2015.01.31
*/

#include <heap/include/gfl2_Heap.h>

//HeapDefine.cdatのマクロ展開用
#define HEAP_DEFINE(str) str,

//HEAPID定義
enum
{
  HEAPID_START = gfl2::heap::HEAPID_USER_START-1,
  //マクロ展開を使っていたので上のような書き方をしていた。sangoのmain.h参照

  //マクロを使って展開するので、実際の定義は下記のファイル内に増やしてください。
#include "HeapDefine.cdat"

  HEAPID_CHILD_MAX //終端
};

//niji全体で使用さえるローカルヒープの数 @note 256で0x800byteメモリを使用
static const int HEAPID_LOCAL_MAX = 256;


#endif //__H_HEAP_DEFINE_H__

