#ifndef __H_MAIN_H__
#define __H_MAIN_H__
//===================================================================
/**
 * @file    Main.h
 * @brief   niji�v���W�F�N�g �A�v���P�[�V�������C��
 * @author  GAME FREAK Inc.
 * @date    15.01.17
*/
//===================================================================
#pragma once


namespace Main{

//--------------------------------------------
/*
  HEAPID_RESIDENT, HEAPID_RESIDENT_DEVICE�̏�����
  �\�t�g���Z�b�g�ŏ������߂ɁASystem�Ƃ͏������^�C�~���O�����炷

  GflUse���W���[���������̃^�C�~���O�ŌĂяo��
*/
//--------------------------------------------
extern void ResidentHeapCreate(void);
extern void ResidentHeapFree(void);

} //namespace Main

#endif //__H_MAIN_H__
