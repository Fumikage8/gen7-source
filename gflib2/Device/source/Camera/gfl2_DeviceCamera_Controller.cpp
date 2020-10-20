#if GF_PLATFORM_CTR
/////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   gfl2_DeviceCamera_Controller.cpp
 * @brief  カメラコントローラ
 * @author obata_toshihiro
 * @date   2010.11.19
 */
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <nn.h>
#include "gfl2_DeviceCamera_BufferManager.h"
#include <Device/include/Camera/gfl2_DeviceCamera_System.h>
#include <Device/include/Camera/gfl2_DeviceCamera_Controller.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(device)
GFL_NAMESPACE_BEGIN(camera)


      //---------------------------------------------------------------------------------
      /**
       * @brief カメラを起動する
       */
      //---------------------------------------------------------------------------------
      void Controller::Activate( void )
      {
        m_pCameraSystem->ActivateCamera( m_cameraID );
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief すべてのカメラを停止する
       */
      //---------------------------------------------------------------------------------
      void Controller::DeactivateAllCamera( void )
      {
        m_pCameraSystem->DeactivateAllCamera();
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief キャプチャを開始する
       */
      //---------------------------------------------------------------------------------
      void Controller::StartCapture( void )
      {
        m_pCameraSystem->StartCapture( m_cameraID );
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief キャプチャを停止する
       */
      //---------------------------------------------------------------------------------
      void Controller::StopCapture( void )
      {
        m_pCameraSystem->StopCapture( m_cameraID );
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief キャプチャ中かどうかを調べる
       */
      //---------------------------------------------------------------------------------
      bool Controller::IsCapturing( void ) const
      {
        return m_pCameraSystem->IsCapturing( m_cameraID );
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief キャプチャした画像データ( RGBフォーマット )を取得する
       */
      //---------------------------------------------------------------------------------
      const void* Controller::GetImage( void ) const
      {
        const BufferManager* p_buffer = m_pCameraSystem->GetBufferManager( m_cameraID );
        const void* p_rgb_data = p_buffer->GetRgbBuffer_Latest();
        return p_rgb_data;
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief キャプチャ画像( RGBフォーマット )のサイズを取得する
       */
      //---------------------------------------------------------------------------------
      u32 Controller::GetImageSize( void ) const
      {
        const BufferManager* p_buffer = m_pCameraSystem->GetBufferManager( m_cameraID );
        return p_buffer->GetRgbBufferSize();
      }



      //---------------------------------------------------------------------------------
      /**
       * @brief キャプチャした画像データ( YUVフォーマット )を取得する
       */
      //---------------------------------------------------------------------------------
      const void* Controller::GetYuvImage( void ) const
      {
        const BufferManager* p_buffer = m_pCameraSystem->GetBufferManager( m_cameraID );
        const void* p_yuv_data = p_buffer->GetYuvBuffer_Latest();
        return p_yuv_data;
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief キャプチャ画像( YUVフォーマット )のサイズを取得する
       */
      //---------------------------------------------------------------------------------
      u32 Controller::GetYuvImageSize( void ) const
      {
        const BufferManager* p_buffer = m_pCameraSystem->GetBufferManager( m_cameraID );
        return p_buffer->GetYuvBufferSize();
      }


      //-------------------------------------------------------------------------------
      /**
       * @brief 露光を設定します。
       * @param[in] exposure  露出量を指定します。指定可能な値の範囲は [-5, +5] です。
       */
      //-------------------------------------------------------------------------------
      void Controller::SetExposure(s8 exposure)
      {
        m_pCameraSystem->SetExposure( m_cameraID, exposure );
      }



      //---------------------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       */
      //---------------------------------------------------------------------------------
      Controller::Controller( const SetupParam& param ) : 
        m_cameraID( param.cameraID ),
        m_pCameraSystem( param.pCameraSystem )
      {
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief デストラクタ
       */
      //---------------------------------------------------------------------------------
      Controller::~Controller()
      {
      }


GFL_NAMESPACE_END(camera)
GFL_NAMESPACE_END(device)
GFL_NAMESPACE_END(gfl2)
#endif // GF_PLATFORM_CTR
