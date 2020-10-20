/**
 * @file	gfl_OsHeader.h
 * @brief	
 * @author	kimura_shin
 * @data	2011/04/06, 16:40
 */

#ifndef GFL_OSHEADER_H
#define	GFL_OSHEADER_H
#pragma once

// =============================================================================
// �v���b�g�t�H�[���ˑ��w�b�_
// =============================================================================

#if defined(GFL_PLATFORM_XENON)

#include <xtl.h> // Hardware dependant header

#elif defined(GFL_PLATFORM_WII)

#include <revolution.h> // Hardware dependant header

#elif defined(GFL_PLATFORM_3DS)

#include <nn.h> // Hardware dependant header

#elif defined(GFL_PLATFORM_WINDOWS)

// �f�o�b�O���������ݒ�
#include <stdio.h>
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DbgNew  ::new(_NORMAL_BLOCK, __FILE__, __LINE__) // rapidxml �ŃG���[���o��̂Ŏg���Ȃ��c
#else
// �����[�X����DOS����\�������Ȃ�
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#define DbgNew  new
#endif // _DEBUG

#include <windows.h> // Hardware dependant header

#elif defined(GFL_PLATFORM_UNIX)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#endif

#endif	/* GFL_OSHEADER_H */

