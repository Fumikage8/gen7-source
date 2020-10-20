#ifndef __GFL_ERRADAPTER_H__
#define __GFL_ERRADAPTER_H__
//=============================================================================
/**
 * @file   gfl_ErrAdapter.h
 * @brief  �G���[EULA�A�v���b�g��K�؂ɏ�������N���X
 * @author k.ohno
 * @date   2012.10.23
 */
//=============================================================================
#pragma once

#include <gflib.h>

#include <nn/Result.h>

#include <ro/gfl_RoManager.h>
#include <nn/erreula.h>


GFL_NAMESPACE_BEGIN(gfl)
GFL_NAMESPACE_BEGIN(err)



class ErrAdapter
{
  GFL_FORBID_COPY_AND_ASSIGN(ErrAdapter);
private:
  static nn::erreula::Parameter mEreParam;  //�n���p�����[�^ 1KB�ȏ゠��̂Œ�`
  static nn::erreula::CTR::ReturnCode _ErrorDispDirectMain(nn::erreula::Parameter* ere_param);

public:


  //------------------------------------------------------------------
  /**
   * @brief   �A�v���b�g���w�肳�ꂽ�ԍ��̃G���[���o��
   * @param   errorCode  �G���[�R�[�h
   * @param   errorType  �G���[�^�C�v
   * @return  nn::erreula::CTR::ReturnCode �C�V���A�v���b�g�̃��^�[���R�[�h
   */
  //------------------------------------------------------------------
  static nn::erreula::CTR::ReturnCode ErrorDisp(u32 errorCode , nn::erreula::ErrorType errorType);
  //------------------------------------------------------------------
  /**
   * @brief   ���ڃG���[���͂��o��
   * @param   pBuf StrBuf�̃|�C���^
   * @return  nn::erreula::CTR::ReturnCode �C�V���A�v���b�g�̃��^�[���R�[�h
   */
  //------------------------------------------------------------------
  static nn::erreula::CTR::ReturnCode ErrorDisp(gfl::str::StrBuf* pBuf);
#if GFL_DEBUG
  //------------------------------------------------------------------
  /**
   * @brief   ���ڃG���[���b�Z�[�W���o�� �f�o�b�O�p�r
   * @param   ���C�h�L�����^������
   * @return  nn::erreula::CTR::ReturnCode �C�V���A�v���b�g�̃��^�[���R�[�h
   */
  //------------------------------------------------------------------
  static nn::erreula::CTR::ReturnCode ErrorDisp(const wchar_t* pMessage);
  //------------------------------------------------------------------
  /**
   * @brief   ���ڃG���[���b�Z�[�W���o�� �f�o�b�O�p�r
   *          mbstowcs�ɂ͌��E������A�������\������Ȃ���������܂��B wchar_t�Ȃ�o�܂�
   * @param   ������
   * @return  nn::erreula::CTR::ReturnCode �C�V���A�v���b�g�̃��^�[���R�[�h
   */
  //------------------------------------------------------------------
  static nn::erreula::CTR::ReturnCode ErrorDisp(const char* pMessage);
  static void ErrorDispChar(const char* pMessage);

#endif //GFL_DEBUG

};

/*
�G���[����
�T�v

CTR �ł̓G���[��傫���ȉ��̂悤�ɕ��ނ��Ă��܂��B
�p���s�\�Ȓv���I�G���[

�u�v���I�ȃG���[�v�Ƃ́A�n�[�h�E�F�A�s���{�����i�Ŕ��������Ȃ��G���[�Ȃ�
�V�X�e�����������Ă����A�ł����A���n���h�����O�s�\�ȃG���[�̂��Ƃ��w���܂��B
���̂悤�ȃG���[�����������ꍇ�́A�����A�v���P�[�V�����ɃG���[���n����邱�Ƃ͂Ȃ��A
�V�X�e������~����FATAL�G���[��ʂ��\������܂��B
���̃G���[�\���ɑ����������[�U�[�̓��b�Z�[�W�ɏ]���ă}�j���A�����m�F���A
�������Ȃ��ꍇ�͍ŏI�I�Ƀ��[�U�T�|�[�g�ɘA�����邱�ƂɂȂ�܂��B
�V�X�e����G���[�\�����s�\�ȏꍇ�͂��̏�Œ�~���܂��B

�����\�ȃG���[

�u�����\�ȃG���[�v�Ƃ͈ȉ��̂悤�Ȃ��̂��w���܂��B

    �ݒ�̊ԈႢ
    �ʐM�G���[
    �t�@�C���̗e�ʃI�[�o�[�A�󂫗e�ʕs��
    �Z�[�u�f�[�^�̔j��
    �\�t�g�E�F�A�I�ȕs�����i�ď��������ŕ��A�\�Ȃ��́j
    �v���I�ł͂��邪�A���ʂ̃V�X�e�����s�p��������ł͂Ȃ����́i�J�����̏ᓙ�j

�����̃G���[�ɑ��������ꍇ�́A
�A�v���P�[�V�����͓K�؂Ƀn���h�����O���s���ăG���[��Ԃ���̕��������݂���A
�K�v�ɉ����ăG���[�\���A�v���b�g��p���ă��[�U�[�ɖ��̔�����`���Ȃ���΂Ȃ�܂���B
�A�v���P�[�V�����̏����͊�{�I�Ɍp�����܂��B
�G���[�n���h�����O

SDK �̑����̃C���^�t�F�[�X�̓G���[��Ԃ��\��������ꍇ�́A
nn::Result �N���X�̎��̂�Ԃ��܂��B
�ڍׂ́AResult �N���X���Q�Ƃ��Ă��������B
�e���C�u�����ł̃G���[�n���h�����O���@�̏ڍׂɂ��ẮA
���ꂼ��̃��C�u�����̃��t�@�����X���Q�Ƃ��Ă��������B

�t�@�C������Ɋ֘A����G���[�����ɂ���
�t�@�C���V�X�e�����Ԃ��G���[�́A�A�N�Z�X���郁�f�B�A�A
�Ώۂ̃t�@�C���ɑ΂��錠���Ȃǂɂ�葽�l�ȃG���[���������܂��B
���ɁA�Z�[�u�f�[�^���j�������ꍇ�ȂǃA�v���P�[�V������
�V�[�P���X�܂Ŋ܂߂��Ή����K�v�ȏꍇ������܂��B
���̂��߁A�t�@�C���V�X�e�����Ԃ��G���[�ɂ��ẮA
���̃��C�u�����Ƃ͈قȂ����������j�������Ă��܂��B
�ڂ����̓t�@�C���V�X�e���̃h�L�������g���Q�Ƃ��Ă��������B

 */


GFL_NAMESPACE_END(err)
GFL_NAMESPACE_END(gfl)

#endif // __GFL_ERRADAPTER_H__
