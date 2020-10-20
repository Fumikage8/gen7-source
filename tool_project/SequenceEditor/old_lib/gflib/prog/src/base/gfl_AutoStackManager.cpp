//=============================================================================
/**
 * @file   gfl_AutoStackManager.cpp
 * @brief  StackMemoryAutoStackManager�����N���X
 * @author suzuki_hisanao
 * @date   2012.6.18
 */
//=============================================================================
#include <base/gfl_AutoStackManager.h>

namespace gfl {
namespace base {



/***************************************************************************
* @brief �R���X�g���N�^
*
* @param heap �X�^�b�N�̈�Ƃ��Ďg�p�����q�[�v�G���A�B<br>
*             Nex�Ŏg�p���邱�Ƃ��l�����A�f�o�C�X�q�[�v���w�肵�Ă͂Ȃ�Ȃ��B
*             �܂��A�{�������𑼂̃��W���[�����g�p���Ă͂Ȃ�Ȃ��B
*
* @return none
***************************************************************************/
AutoStackManager::AutoStackManager( gfl::heap::HeapBase* heap ) : nn::os::StackMemoryAutoStackManager()
{
  m_allocator = GFL_NEW( heap ) gfl::heap::FndAllocator( heap );
  GFL_ASSERT_STOP( (m_allocator!=NULL) );

  // ��������Bottom����擾����悤�ɐݒ�
  m_allocator->SetIsAllocLow( true );

  Initialize( m_allocator, m_stackForDestruct );
}




/***************************************************************************
* @brief �f�X�g���N�^
*
* @return none
***************************************************************************/
AutoStackManager::~AutoStackManager( void )
{
  Finalize();

  if( m_allocator != NULL )
  {
    GFL_DELETE( m_allocator );
    m_allocator = NULL;
  }
}


/***************************************************************************
 * @brief �X�^�b�N�p�̃��������m�ۂ���
 *
 * @param stackSize �m�ۂ��ׂ��X�^�b�N�̃T�C�Y
 *                  �T�C�Y��4096�̔{���łȂ��ꍇ�A�����I��4096�̔{�����m�ۂ����
 *
 * @return �m�ۂ����X�^�b�N�p�������̍Ō���ւ̃|�C���^
 ***************************************************************************/
void* AutoStackManager::Construct( size_t stackSize )
{
  // 4096�A���C�����g
  stackSize = nn::math::RoundUp( stackSize, 4096 );

  return nn::os::StackMemoryAutoStackManager::Construct( stackSize );
}


/***************************************************************************
* @brief �X�^�b�N�p�̃��������������
*
* @param pStackBottom ������ׂ��X�^�b�N�B
* @param isError �G���[�ɔ���������ۂ���\��
*
* @return none
***************************************************************************/
void AutoStackManager::Destruct( void * pStackBottom, bool isError )
{
  nn::os::StackMemoryAutoStackManager::Destruct( pStackBottom, isError );
}




} // namespace base
} // namespace gfl
