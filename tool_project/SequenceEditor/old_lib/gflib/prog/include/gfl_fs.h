#ifndef __GFL_FS_H__
#define __GFL_FS_H__
//===========================================================================
/**
 * @file  gf_fs.h
 * @brief GAMEFREAK Library FileIO System
 * @author  GAMEFREAK inc.
 * @date  2010.09.24
 */
//===========================================================================
#pragma once
#ifdef  __cplusplus

#if GFL_DEBUG
#define GFL_FILE_DEBUG 1
#else
#define GFL_FILE_DEBUG 0
#endif

#include <fs/gfl_HioSwitch.h>

#include <fs/gfl_FileApi.h>
#include <fs/gfl_ReadFile.h>
#include <fs/gfl_ArcFile.h>
#include <fs/gfl_FileReadManager.h>
#include <fs/gfl_FileBase.h>
#include <fs/gfl_RomFile.h>
#include <fs/gfl_BackupFile.h>
#include <fs/gfl_BackupData.h>
#include <fs/gfl_BackupSystem.h>


#if GFL_FILE_DEBUG
#include <fs/gfl_SdmcFile.h>
#endif

#if GFL_HIO_ENABLE
#include <fs/gfl_HioFile.h>
#include <fs/gfl_ReadPCFile.h>
#include <fs/gfl_ReadPCFileManager.h>
#endif

//    #include <gflib/fs/gf_File.h>
//    #include <gfl/fs/gf_fs_FileWriter.h>
//    #include <gfl/fs/gf_fs_File.h>


#endif // __cplusplus
#endif // __GFL_FS_H__
