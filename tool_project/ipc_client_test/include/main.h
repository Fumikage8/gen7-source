#ifndef __MAIN_H__
#define __MAIN_H__
//======================================================================
/**
 * @file	main.h
 * @brief	
 * @author	ariizumi
 * @data	10/10/20
 */
//======================================================================
#pragma once

#define HEAP_DEFINE(str) str,

namespace heapdefine{
//HEAPID�̒�`
enum
{
  HEAPID_START = gfl2::heap::HEAPID_USER_START-1,
  #include "heap_define.h"

  HEAPID_CHILD_MAX //�I�[
};
};//heapdefine

#include "heap_size.h"


#endif// __MAIN_H__
