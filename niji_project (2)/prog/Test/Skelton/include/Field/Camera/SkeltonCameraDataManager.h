#if defined(GF_PLATFORM_WIN32)


/**
* @file    SkeltonCameraDataManager.h
* @brief   フィールド上のカメラ
* @author  ikuta_junya
* @date    2014.09.26
*/

#if !defined( FIELDCAMERADATAMANAGER_H_INCLUDED )
#define FIELDCAMERADATAMANAGER_H_INCLUDED 
#pragma once

#include "Test/Skelton/include/Field/Camera/SkeltonCameraType.h"

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Skelton)

class SkeltonCameraDataManager
{
public:

  SkeltonCameraDataManager( void );
  virtual ~SkeltonCameraDataManager( void );

  /**
  * @brief   初期化処理
  *
  * @return  無し 
  */
  void Initialize( void );

  /**
  * @brief   終了処理
  *
  * @return  無し 
  */
  void Terminate( void );


  const CAMERA_DATA* GetDefaultCameraData( void ) const;
  const CAMERA_DATA* GetCameraData( const c8* pName ) const;
  const CAMERA_DATA* GetCameraDataByIndex( s32 index ) const;
  b32 LoadData( const c8* pFilePath );
  b32 LoadData( const c16* pFilePath );
  c8* ReadFile( const char* pFileName, s32* pSize, s32 align = 16 );
  c8* ReadFile( const c16* pFileName, s32* pSize, s32 align = 16 );
  void CreateXml( const c16* pFilePath );

  static b32 GetCameraType( const c8* pText, Type* pOut );
  static b32 GetCoordinateSystemType( const c8* pText, CoordinateSystemType* pOut );

private:

  b32 UpdateImpl( c8* pData );
  void DeleteCameraData( CAMERA_DATA* pData ) const;
  

  CAMERA_DATA*  m_pDefaultCameraData;
  CAMERA_DATA*  m_pCameraDataTable;
  u32           m_CameraDataUseSize;

};

GFL_NAMESPACE_END(Skelton)
GFL_NAMESPACE_END(Test)

#endif
#endif // GF_PLATFORM_WIN32
