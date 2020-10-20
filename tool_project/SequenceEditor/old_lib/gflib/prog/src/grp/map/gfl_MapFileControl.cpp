//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_MapFileControl.cpp
 *	@brief  �}�b�v�Ǘ��@�t�@�C���R���g���[��
 *	@author	tomoya takahashi
 *	@date		2011.01.28
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#include <gflib.h>
#include <grp/map/gfl_MapFileControl.h>

namespace gfl{
namespace grp{
namespace map{


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					����
*/
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief    �R���X�g���N�^
 */
//-----------------------------------------------------------------------------
FileControlArray::FileControlArray( void ) :
  m_ppFileControl(NULL),
  m_FileControlNum(0)
{
}

//----------------------------------------------------------------------------
/**
 *	@brief    �f�X�g���N�^
 */
//-----------------------------------------------------------------------------
FileControlArray::~FileControlArray()
{
  Finalize();
}


//-----------------------------------------------------------------------------
/**
 *  @brief  ������
 *
 *  @param  arrayNum    �z��
 *  @param  p_heap      �q�[�v
 */
//-----------------------------------------------------------------------------
void FileControlArray::Initialize( u32 arrayNum, gfl::heap::HeapBase* p_heap )
{
  if( m_ppFileControl ){
    GFL_ASSERT( m_ppFileControl == NULL );//@check
    Finalize();
  }
  
  m_ppFileControl   = GFL_NEW_ARRAY(p_heap) FileControl*[arrayNum];
  m_FileControlNum  = arrayNum;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �j������
 */
//-----------------------------------------------------------------------------
void FileControlArray::Finalize( void )
{
  if( m_ppFileControl ){
    GFL_DELETE_ARRAY m_ppFileControl;
    m_ppFileControl   = NULL;
    m_FileControlNum  = 0;
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief �t�@�C���R���g���[���̓o�^
 *  
 *  @param  p_control   �R���g���[�� 
 *
 *  @retval true  �o�^����
 *  @retval false �o�^���s�i�o�^���I�[�o�[�j
 */
//-----------------------------------------------------------------------------
b32 FileControlArray::SetFileControl( FileControl* p_control )
{
  u32 index = GetClearIndex();

  if( index >= m_FileControlNum ){
    return false;
  }

  // �|�C���^�̓o�^
  m_ppFileControl[index] = p_control;

  return true;
}


//-----------------------------------------------------------------------------
/**
 *  @brief �V�O�l�`���̃t�@�C���R���g���[���擾
 *
 *  @param  signature   �V�O�l�`���[ 
 *
 *  @retval �V�O�l�`���̃t�@�C���R���g���[��
 *  @retval NULL  �݂���Ȃ��B
 */
//-----------------------------------------------------------------------------
FileControl* FileControlArray::GetFileControl( SignatureType signature ) const
{
  // �V�O�l�`���̃t�@�C���R���g���[�����擾
  for( u32 i=0; i<m_FileControlNum; i++ ){

    if( m_ppFileControl[i] ){

      if( m_ppFileControl[i]->GetSignature() == signature ){
        return m_ppFileControl[i];
      }
    }
  }

  // ������Ȃ������B
  GFL_ASSERT( 0 );
  return NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �����Ă���o�b�t�@�C���f�b�N�X���擾
 *
 *	@return �����Ă���o�b�t�@�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
u32 FileControlArray::GetClearIndex( void ) const
{
  for( u32 i=0; i<m_FileControlNum; i++ ){
    if( m_ppFileControl[i] == NULL )
    {
      return i;
    }
  }

  // �v�f��������Ȃ�
  GFL_ASSERT( 0 );
  return 0;
}





//-----------------------------------------------------------------------------
/**
 *      FileContorl
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief  �u���b�N����ݒ�  �u���b�N�̃t�@�C���ǂݍ��݂����������
 *	�@�@�@�@���̊֐����R�[�������B�@���̓����ŁA�`����𐮂���K�v������B
 *
 *	@param  p_map   �}�b�v�u���b�N���
 *
 *	@retval true  ����
 *	@retval false �r��
 */
//-----------------------------------------------------------------------------
b32 FileControl::SetUp( gfl::grp::map::System* p_map )
{
  GFL_UNUSED(p_map);

  return true;
}

//----------------------------------------------------------------------------
/**
 *	@brief    �A�g���r���[�g�����擾
 *
 *	@param	cp_map      �}�b�v�u���b�N���
 *	@param  p_attrInfo  �A�g���r���[�g���i�[��
 *	@param  cp_pos      �擾����RD���W
 *	@param  cp_way      �n�ʕ����iDefault�@(0,-1,0)
 *	@param  map_width   �}�b�v�u���b�N�̃T�C�Y
 */
//-----------------------------------------------------------------------------
void FileControl::GetAttr( const gfl::grp::map::System* cp_map, AttrInfo* p_attrInfo, const void* cp_pos, const gfl::math::VEC3* cp_way, f32 map_width )
{
  GFL_UNUSED(cp_map);
  GFL_UNUSED(p_attrInfo);
  GFL_UNUSED(cp_pos);
  GFL_UNUSED(cp_way);
  GFL_UNUSED(map_width);

  // �\���̂��N���A
  ClearAttrInfo( p_attrInfo );
}



//----------------------------------------------------------------------------
/**
 *	@brief    �A�g���r���[�g���̏���������
 *
 *	@param  p_attrInfo  �A�g���r���[�g���i�[��
 */
//-----------------------------------------------------------------------------
void FileControl::ClearAttrInfo( AttrInfo* p_attrInfo )
{
  gfl::std::MemClear( p_attrInfo, sizeof(AttrInfo) );
}


} // namespace map
} // namespace grp
} // namespace gfl

