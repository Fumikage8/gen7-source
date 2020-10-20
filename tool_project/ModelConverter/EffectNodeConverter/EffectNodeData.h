#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_GfEFFECTNODEDATA_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_GfEFFECTNODEDATA_H_INCLUDED

//���Windows�A�v���p�g�����C�u������p�ӂ��āA������Œ񋟂������܂��B
#if defined(WIN32)

#include <stdio.h>
#include <types/include/gfl2_Typedef.h>

#include <math/include/gfl2_Vector.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_IResourceData.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

//-----------------------------------------------------------------------------
/**
 * @class	GfLocData
 * @brief	GfLocData�N���X
 */
class GfEffectNodeData : public IResourceData
{
public:

	GfEffectNodeData( c8* pData );
	virtual ~GfEffectNodeData();

	//! @brief �S�f�[�^�̐��̎擾
	virtual s32 GetSize( void );

	// Write Binary File:
	virtual bool WriteFile(std::string filePath);

	//! @brief �f�[�^�̎擾
	virtual const void* GetData( u32 index );

	//! @brief �X�̃f�[�^�̃T�C�Y�擾
	virtual u32 GetDataSize( u32 index );

	//! @brief �^�C�v�f�[�^�̎擾
	virtual Type GetType( u32 index );

	//! @brief �t�@�C���^�C�v�̎擾
	virtual Type GetFileType( void )
	{
		return IResourceData::Type("gfloc");
	}

	//! @brief �f�[�^��ێ����Ă��邩�H
  virtual bool HasData(void)
  {
    return (this->GetData(0) != NULL);
  }

  //! @brief �o�[�W����ID�̎擾
  virtual u32 GetVersionId( void )
  {
    return 0;
  }

private:
	GfEffectNodeData():IResourceData(){}

	class Impl;
	Impl								*m_pImpl;
};


}}}}

#endif

#endif


