//==============================================================================
/**
 * @file	gflib.h
 * @brief	
 * @author	kimura_shin
 * @data	2011/04/06, 16:05
 */
// =============================================================================

#ifndef GFLIB_H
#define	GFLIB_H

#pragma once

// �v���b�g�t�H�[���ʂ̒�`�����߂Ă���܂�
#include <core/gfl_Config.h>

#ifdef GFL_PLATFORM_PC // ======================================================

#include <gfl_Pc.h>

#elif defined(GFL_PLATFORM_3DS) // =============================================

#include <gflib_3ds.h> // ���߂�Ȃ����A���X�� gflib.h �ł��B

#endif // ======================================================================

#include <gfl_Framework.h>

#endif	/* GFLIB_H */
