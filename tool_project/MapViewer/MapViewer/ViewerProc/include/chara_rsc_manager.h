#pragma once
//=================================================================================================
/**
 *	@file	model_rsc_manager.h
 *	@brief
 *	@author	tamada@gamefreak.co.jp
 *	@date	2014.04.24
 */
//=================================================================================================
#include "chara_define.h"
#include "../../System/include/HeapDefine.h"
#include <fs/include/gfl2_FsAsyncFileManager.h>

namespace gfl2{ namespace renderingengine { namespace scenegraph {
	namespace resource { class ResourceNode;		}
	namespace instance { class ModelInstanceNode;	}
}}}


//----------------------------------------------------------------------------------------
//! @brief ���\�[�X�Ǘ��I�u�W�F�N�g�̃}�l�[�W���[
//----------------------------------------------------------------------------------------
class RscManager
{
public:

	//-----------------------------------------------------------------------------
	/*
	*          �����N���X�錾(���\�[�X�A���P�[�^)
	*          @class GLAllocator
	*/
	//-----------------------------------------------------------------------------
	class GLResourceAllocator : public gfl2::gfx::IGLAllocator
	{
	public:
		
		//----------------------------------------------------------------------------
		/**
		*  @brief  �R���X�g���N�^
		*/
		//-----------------------------------------------------------------------------
		GLResourceAllocator():IGLAllocator(),m_pHeap(NULL) {}

		//----------------------------------------------------------------------------
		/**
		*  @brief  �f�X�g���N�^
		*/
		//-----------------------------------------------------------------------------
		virtual			~GLResourceAllocator(){}
		
    //----------------------------------------------------------------------------
    /**
    *  @brief  �R���X�g���N�^
    */
    //-----------------------------------------------------------------------------
    void  init(gfl2::heap::HeapBase* pHeap){ m_pHeap = pHeap; }

		//----------------------------------------------------------------------------
		/**
		*  @brief  ���C���������̎擾
		*  @param  �擾�������������T�C�Y
		*  @param  �A���C�����g
		*  @return ������
		*/
		//-----------------------------------------------------------------------------
		virtual void*	SystemMemoryMalloc(u32 size, u32 alignment){ return GflHeapAllocMemoryAlign( m_pHeap , size , alignment ); }

		//----------------------------------------------------------------------------
		/**
		*  @brief  ���C���������̉��
		*  @param  ����������������̃|�C���^
		*/
		//-----------------------------------------------------------------------------
		virtual void	SystemMemoryFree(void* const address){ return GflHeapFreeMemory( address ); }

		//----------------------------------------------------------------------------
		/**
		*  @brief  �r�f�I�������̎擾
		*  @param  �擾�������������T�C�Y
		*  @param  �A���C�����g
		*  @return ������
		*/
		//-----------------------------------------------------------------------------
		virtual void*	LocalMemoryMalloc(u32 size, u32 alignment){ return NULL; }

		//----------------------------------------------------------------------------
		/**
		*  @brief  �r�f�I�������̉��
		*  @param  ����������������̃|�C���^
		*/
		//-----------------------------------------------------------------------------
		virtual void	LocalMemoryFree(void* const address){ }
		
	private:
    
    gfl2::heap::HeapBase *m_pHeap;

	};

	//----------------------------------------------------------------------------
	/**
	*	@brief  �V���O���g���C���X�^���X�̎擾
	*    
	*	@return �C���X�^���X
	*/
	//-----------------------------------------------------------------------------
	static RscManager* RscManager::getInstance() 
	{
		static RscManager instance;
		return &instance;
	}

	//-----------------------------------------------------------------------------
	/**
	*  @brief	�R���X�g���N�^
	*/
	//-----------------------------------------------------------------------------
	RscManager(){}

	//-----------------------------------------------------------------------------
	/**
	*  @brief	�f�X�g���N�^
	*/
	//-----------------------------------------------------------------------------
	~RscManager(){}

	//-----------------------------------------------------------------------------
	/**
	*  @brief	Setup�ETerminate
	*/
	//-----------------------------------------------------------------------------
  void Setup( gfl2::heap::HeapBase * pDevice, gfl2::fs::AsyncFileManager* pAsyncReadMan );
  void Terminate(void);

	//----------------------------------------------------------------------------
	/**
	*	@brief  HIGH���f���̎擾�֐�
	*
	*	@param	id		���\�[�XID
	*	@return ���f�����\�[�X�m�[�h�̃C���X�^���X
	*/
	//-----------------------------------------------------------------------------
  gfl2::renderingengine::scenegraph::resource::ResourceNode* GetHighModelResource(chara::RSC_ID id);

  //----------------------------------------------------------------------------
	/**
	*	@brief  LOW���f���̎擾�֐�
	*
	*	@param	id		���\�[�XID
	*	@return ���f�����\�[�X�m�[�h�̃C���X�^���X
	*/
	//-----------------------------------------------------------------------------
  gfl2::renderingengine::scenegraph::resource::ResourceNode* GetLowModelResource (chara::RSC_ID id);

	//----------------------------------------------------------------------------
	/**
	*	@brief  ���[�V�����̎擾�֐�
	*
	*	@param	id		���\�[�XID
	*	@return ���[�V�������\�[�X�m�[�h�̃C���X�^���X
	*/
	//-----------------------------------------------------------------------------
	gfl2::renderingengine::scenegraph::resource::ResourceNode* GetMotionResource(chara::RSC_ID id,u32 motionId);

  //----------------------------------------------------------------------------
	/**
	*	@brief  �����䃂�[�V�����̎擾�֐�
	*
	*	@param	id		���\�[�XID
	*	@return �����䃂�[�V�������\�[�X�m�[�h�̃C���X�^���X
	*/
	//-----------------------------------------------------------------------------
	gfl2::renderingengine::scenegraph::resource::ResourceNode* GetMouthMotionResource( chara::RSC_ID id );

  //----------------------------------------------------------------------------
	/**
	*	@brief  �ڃp�`���䃂�[�V�����̎擾�֐�
	*
	*	@param	id		���\�[�XID
	*	@return �����䃂�[�V�������\�[�X�m�[�h�̃C���X�^���X
	*/
	//-----------------------------------------------------------------------------
	gfl2::renderingengine::scenegraph::resource::ResourceNode* GetEyeMotionResource( chara::RSC_ID id );

  //----------------------------------------------------------------------------
	/**
	*	@brief  ���[�V�����R���t�B�O�̎擾�֐�
	*
	*	@param	id		���\�[�XID
	*	@return ���[�V�����R���t�B�O���\�[�X�m�[�h�̃C���X�^���X
	*/
	//-----------------------------------------------------------------------------
	gfl2::renderingengine::scenegraph::resource::ResourceNode* GetMotionConfigResource(chara::RSC_ID id,u32 motionId);

	//----------------------------------------------------------------------------
	/**
	*	@brief  �Փ˂����R���W�����̎擾�֐�
	*
	*	@param	id		���\�[�XID
	*	@return �R���W�������\�[�X�m�[�h�̃C���X�^���X
	*/
	//-----------------------------------------------------------------------------
	gfl2::renderingengine::scenegraph::resource::ResourceNode* GetStaticCollisionResource(chara::RSC_ID id);


  GLResourceAllocator* GetAllocator( void ){ return &m_ResourceAllocator; }

  // ���f�����\�[�X�m�[�h���擾
  inline gfl2::renderingengine::scenegraph::resource::ResourceNode** GetHighModelResourceNodeTable() { return m_ppModelResourceNodeTable; }

private:

	//----------------------------------------------------------------------------
	/**
	*	@brief  �t�@�C���ǂݍ���
	*	@param  ���p����A���P�[�^
	*	@param  �t�@�C���p�X
	*	@return Raw�f�[�^
	*/
	//-----------------------------------------------------------------------------
	void *ReadData( const char* pFilePath );

	GLResourceAllocator m_ResourceAllocator;

  gfl2::heap::HeapBase* m_pDevice;
  gfl2::fs::AsyncFileManager* m_pAsyncReadMan;


  // ���f�����\�[�X
  void**                                                      m_ppModelBinaryTable;   
  gfl2::renderingengine::scenegraph::resource::ResourceNode** m_ppModelResourceNodeTable;
  u32                                                         m_ModelDataCount;

  // ���[�V�������\�[�X
  void**                                                      m_ppMotionBinaryTable;   
  gfl2::renderingengine::scenegraph::resource::ResourceNode** m_ppMotionResourceNodeTable;
  u32                                                         m_MotionDataCount;

  static const s32                                            BIN_DATA_CNT = 256;
  c8*                                                         m_pBinaryDataArray[BIN_DATA_CNT];
  s32                                                         m_binaryDataIndex;

  static const s32                                            RESOURCE_NODE_CNT = 256;
  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pResourceNodeArray[RESOURCE_NODE_CNT];
  s32                                                         m_resourceNodeIndex;
};
