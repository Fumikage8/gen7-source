//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dModelMemoryManager.cpp
 *	@brief  H3D　モデルメモリ解放管理　マネージャ
 *	@author	tomoya takahashi
 *	@date		2012.08.30
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#if GFL_GRP_G3D_H3D_USE


#include <grp/g3d/gfl_G3dH3dModelMemoryManager.h>


namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  /**
   *	@brief  コンストラクタ
   */
  //-----------------------------------------------------------------------------
  H3dModelMemoryManager::H3dModelMemoryManager(void) : 
    m_pBuffer(NULL),
    m_BufferMax(0)
  {
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  デストラクタ
   */
  //-----------------------------------------------------------------------------
  H3dModelMemoryManager::~H3dModelMemoryManager(void)
  {
    this->Finalize();
  }
  
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  マネージャ初期化
   *
   *	@param  p_heap        ヒープ
   *	@param  h3dmodel_max  モデル最大数
   */
  //-----------------------------------------------------------------------------
  void H3dModelMemoryManager::Initialize( gfl::heap::HeapBase* p_heap, u32 h3dmodel_max )
  {
    if( m_pBuffer ){
      GFL_ASSERT(0);
      this->Finalize();
    }
    
    m_pBuffer = static_cast<Data*>(GflHeapAllocMemory( p_heap, sizeof(Data) * h3dmodel_max ));
    m_BufferMax = h3dmodel_max;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  マネージャ破棄　（登録されたメモリを一括で解放します）
   */
  //-----------------------------------------------------------------------------
  void H3dModelMemoryManager::Finalize( void )
  {
    if( m_pBuffer ){

      for( u32 i=0; i<m_BufferMax; ++i ){
        if( m_pBuffer[i].data_in ){
          FreeMemoryData( &m_pBuffer[i] );
        }
      }
      GflHeapFreeMemory( m_pBuffer );
      m_pBuffer = NULL;
    }
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  解放処理の更新    CalculateやSubmitViewのタイミングで呼ばれる想定である。
   *	                          １フレームに１回。
   */
  //-----------------------------------------------------------------------------
  void H3dModelMemoryManager::Update( void )
  {
    for( u32 i=0; i<m_BufferMax; ++i ){
      if( m_pBuffer[i].data_in ){
        if( m_pBuffer[i].count <= 0 ){
          FreeMemoryData(&m_pBuffer[i]);
        }else{
          --m_pBuffer[i].count;
        }
      }
    }
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  破棄するメモリを登録    １回だけ登録するように注意
   */
  //-----------------------------------------------------------------------------
#if 1
  void H3dModelMemoryManager::SetDeleteMemory( IMemortObject* pObject )
  {
    for( u32 i=0; i<m_BufferMax; ++i ){

      if( !m_pBuffer[i].data_in ){
        m_pBuffer[i].pMemortObject  = pObject;
        m_pBuffer[i].count          = 1;
        m_pBuffer[i].data_in        = true;
        return ;
      }
    }

    GFL_ASSERT_STOP(0);
  }
  
#else
  void H3dModelMemoryManager::SetDeleteMemory( void * p_state_heap, void * p_state_device )
  {
    for( u32 i=0; i<m_BufferMax; ++i ){

      if( !m_pBuffer[i].data_in ){
        m_pBuffer[i].p_state_heap   = p_state_heap;
        m_pBuffer[i].p_state_device = p_state_device;
        m_pBuffer[i].count          = 1;
        m_pBuffer[i].data_in        = true;
        return ;
      }
    }

    GFL_ASSERT_STOP(0);
  }
#endif
  //----------------------------------------------------------------------------
  /**
   *	@brief    データの有無をチェック
   */
  //-----------------------------------------------------------------------------
  bool H3dModelMemoryManager::IsHeapIdMemoryExists( void ) const
  {
    for( u32 i=0; i<m_BufferMax; ++i ){

      if( m_pBuffer[i].data_in ){
        return true;
      }
    }
    return false;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief    heapIDから作られたメモリが存在しているかチェック
   *
   *	@retval true  存在している
   *	@retval false 存在していない
   */
  //-----------------------------------------------------------------------------
  bool H3dModelMemoryManager::IsHeapIdMemoryExists( gfl::heap::HEAPID heapID ) const
  {
    return false;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  メモリの破棄
   */
  //-----------------------------------------------------------------------------
  void H3dModelMemoryManager::FreeMemoryData( Data* p_data )
  {
    if( p_data->data_in ){
#if 1
      GFL_SAFE_DELETE( p_data->pMemortObject );
#else
      if(p_data->p_state_heap){
        GflHeapFreeMemory( p_data->p_state_heap );
      }
      if(p_data->p_state_device){
        GflHeapFreeMemory( p_data->p_state_device );
      }
      p_data->p_state_heap    = NULL;
      p_data->p_state_device  = NULL;
#endif
      p_data->data_in = false;
    }
  }


}  // namespace g3d
}  // namespace grp
}  // namespace gfl


#endif  // GFL_GRP_G3D_H3D_USE

