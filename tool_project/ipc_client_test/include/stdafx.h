// stdafx.h : �W���̃V�X�e�� �C���N���[�h �t�@�C���̃C���N���[�h �t�@�C���A�܂���
// �Q�Ɖ񐔂������A�����܂�ύX����Ȃ��A�v���W�F�N�g��p�̃C���N���[�h �t�@�C��
// ���L�q���܂��B
//

#pragma once

// C�p�}�N��min/max(winmindef.h���j���L���ɂȂ邱�Ƃ�h�����߂̒�` for windows.h
#define	NOMINMAX

#define STRICT					      // �^�`�F�b�N�������ɍs�Ȃ�
#define WIN32_LEAN_AND_MEAN		// �w�b�_�[���炠�܂�g���Ȃ��֐����Ȃ�
#define WINVER        0x0600	// Windows Vista�ȍ~�Ή��A�v�����w��(�Ȃ��Ă��悢)
#define _WIN32_WINNT  0x0600	// ����

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Windows �w�b�_�[����g�p����Ă��Ȃ����������O���܂��B
// Windows �w�b�_�[ �t�@�C��:
#include <windows.h>

#define	_CRT_RAND_S
// C �����^�C�� �w�b�_�[ �t�@�C��
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <mmsystem.h>


// TODO: �v���O�����ɕK�v�Ȓǉ��w�b�_�[�������ŎQ�Ƃ��Ă��������B
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include <heap/include/gfl2_Heap.h>
