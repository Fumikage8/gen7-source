//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dResource.cpp
 *	@brief  H3D���\�[�X
 *	@author	Koji Kawada
 *	@date		2012.04.06
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>
#include <nw/h3d.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>
#include <gfl_fs.h>

// gflib grp
#include <grp/gfl_GraphicsType.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>



#if GFL_GRP_G3D_H3D_USE

// NW4C
#include <nw/h3d.h>
// gflib grp g3dh3d
#include <grp/g3d/gfl_G3dH3dResource.h>



namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X����
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	H3D���\�[�X�N���X
//=====================================
H3dResource::H3dResource(void)
  : m_mem_loc(),
    m_address_map(),
    m_srcRawAddress(0),
    m_buff(NULL),
    m_resbin(NULL),
    m_bit_flag(BIT_FLAG_NONE),
    m_result_desc(RESULT_OK)
{
  // �l���ς���Ă��܂��Ă��Ȃ����`�F�b�N����
  STATIC_ASSERT(RESULT_OK                         == nw::h3d::Result::RESULT_OK                        );
  STATIC_ASSERT(RESULT_BUFFER_TOO_SHORT           == nw::h3d::Result::RESULT_BUFFER_TOO_SHORT          );
  STATIC_ASSERT(RESULT_RESOURCE_NOT_FOUND_TEXTURE == nw::h3d::Result::RESULT_RESOURCE_NOT_FOUND_TEXTURE);
  STATIC_ASSERT(RESULT_RESOURCE_NOT_FOUND_SHADER  == nw::h3d::Result::RESULT_RESOURCE_NOT_FOUND_SHADER );
  STATIC_ASSERT(RESULT_RESOURCE_NOT_FOUND_LUT     == nw::h3d::Result::RESULT_RESOURCE_NOT_FOUND_LUT    );
}

H3dResource::~H3dResource()
{
  DetachBuffer();
}

b32 H3dResource::Load(gfl::heap::HeapBase* device_memory, gfl::heap::HeapBase* /*heap_memory_temp*/, const char* path, b32 is_compress)
{
  GFL_ASSERT_MSG(!is_compress, "���߂�Ȃ����B�܂����k�ɑΉ����Ă��Ȃ��ł��B\n");  //@check

  // �t�@�C�����[�h
  gfl::fs::ReadFile read_file;
  bool open_ret = read_file.Open(path);
  if( !open_ret ) return false;
  u32 size       = read_file.GetSize();
  u32 size_align = read_file.GetSizeAlign();  // �e�N�X�`�������[�h����ɂ�128Byte�A���C�����g���s���K�v������̂ŁA
  size_align     = 128;                       // �������[�h���邩�ŃA���C�����g�𕪂���K�v�����邩��
  void* buffer = GflHeapAllocMemoryAlign( device_memory, size, size_align );
  s32 read_ret = read_file.Read( buffer, size );  // size��size_align�ɍ��킹���A���C�����g�ɂ��Ă����K�v�����邩��
  read_file.Close();
  if( read_ret < 0 )
  {
    GflHeapFreeMemory(buffer);
    return false;
  }
  else
  {
    AttachBuffer(buffer);
    SetOwnBufferFlag(true);
    return true;
  }
}
b32 H3dResource::Load(gfl::heap::HeapBase* device_memory, gfl::heap::HeapBase* heap_memory_temp, gfl::fs::ArcFile* arc_file, u32 arc_data_id, b32 is_compress)
{
  // �t�@�C�����[�h
  u32 size;
  u32 size_align = 128;  // �e�N�X�`�������[�h����ɂ�128Byte�A���C�����g���s���K�v������̂ŁA
                            // �������[�h���邩�ŃA���C�����g�𕪂���K�v�����邩��
  void*  buffer = NULL;

  if( is_compress )
  {
    // ���k�f�[�^
    buffer = arc_file->LoadCompressedDataAlloc( arc_data_id, heap_memory_temp, device_memory, size_align, &size );
    if( !buffer )
    {
      return false;
    }
    else
    {
      AttachBuffer(buffer);
      SetOwnBufferFlag(true);
      return true; 
    }
  }
  else
  {
    // ���k���Ă��Ȃ��f�[�^
    size = arc_file->GetDataSize( arc_data_id );
    buffer = GflHeapAllocMemoryAlign( device_memory, size, size_align );
    s32 read_ret = arc_file->LoadData( arc_data_id, buffer );
    if( read_ret < 0 )
    {
      GflHeapFreeMemory(buffer);
      return false;
    }
    else
    {
      AttachBuffer(buffer);
      SetOwnBufferFlag(true);
      return true;
    }
  }
}
b32 H3dResource::Load(gfl::heap::HeapBase* device_memory, gfl::heap::HeapBase* heap_memory_temp, u32 arc_id, u32 arc_data_id, b32 is_compress)
{
  gfl::fs::ArcFile* arc_file = GFL_NEW(heap_memory_temp) gfl::fs::ArcFile( heap_memory_temp, arc_id, gfl::fs::ArcFile::OPEN );
  b32 ret = Load(device_memory, heap_memory_temp, arc_file, arc_data_id, is_compress);
  GFL_DELETE arc_file;
  return ret;
}

b32 H3dResource::AttachBuffer(void* buff)
{
  b32 ret = false;
  if( !m_buff )
  {
    m_buff = buff;
    ret = true;
  }
  else
  {
    GFL_ASSERT_MSG(0, "���̃o�b�t�@�����ɃA�^�b�`����Ă��܂��B\n");
  }
  return ret;
}
void H3dResource::DetachBuffer( b32 fUnInit )
{
  if( m_resbin )
  {
    if( m_mem_loc.IsRawSectionVram() )
    {
      if( m_resbin->GetSectionAddress(nw::h3d::FileSectionType::RAW) ) 
        gfl::grp::GraphicsSystem::FreeMemory(
            m_mem_loc.raw_section,
            m_resbin->GetSectionAddress(nw::h3d::FileSectionType::RAW)
        );
    }

    if( m_mem_loc.IsCommandSectionVram() )  // @note �R�}���h��VRAM�ɓ]������l�����ɂ����p�q�[�v��������n���Ă��炤�ƁA���H���������ɂȂ�Ȃ������B1/2 �� Map�֐���vram_copy_heap_allocator��ǉ����܂����B
    {
      if(m_address_map.srcUCmdAddress)
      {
        GflHeapFreeMemory(reinterpret_cast<void*>(m_address_map.srcUCmdAddress));
        m_address_map.srcUCmdAddress = 0;
      }
    }

    if( m_mem_loc.IsCommandSectionVram() )
    {
      if( m_resbin->GetSectionAddress(nw::h3d::FileSectionType::UNINIT_COMMAND) ) 
        gfl::grp::GraphicsSystem::FreeMemory(
            m_mem_loc.command_section,
            m_resbin->GetSectionAddress(nw::h3d::FileSectionType::UNINIT_COMMAND)
        );
 
      if( m_resbin->GetSectionAddress(nw::h3d::FileSectionType::COMMAND) ) 
        gfl::grp::GraphicsSystem::FreeMemory(
            m_mem_loc.command_section,
            m_resbin->GetSectionAddress(nw::h3d::FileSectionType::COMMAND)
        );
    }
    else 
    {
      if( m_resbin->GetSectionAddress(nw::h3d::FileSectionType::UNINIT_COMMAND) ) 
        GflHeapFreeMemory(
            m_resbin->GetSectionAddress(nw::h3d::FileSectionType::UNINIT_COMMAND));
    }

    // �o���邾���ȑO�̗���Ɠ����ɂ��邽�߂ɁA
    // �|�C���^��ێ����Ă����AUninit��j�����s���B
    // �iUninit�̒���AdressMap�̓N���A����Ȃ��̂ŕK�v�Ȃ���������̂��߁j
    void * p_uninit_data = m_resbin->GetSectionAddress(nw::h3d::FileSectionType::UNINIT_DATA);

    if ( fUnInit )  // UNINIT_DATA���������O�ɏ����l�ɖ߂��悤�ɏC��
      m_resbin->Uninit();

    if( p_uninit_data ) 
      GflHeapFreeMemory(
          p_uninit_data);
      
    GFL_DELETE m_resbin;  // m_buff��m_resbin�͊o���Ă��邪�Am_resbin�ɂ̓f�X�g���N�^���Ȃ��̂ŁA������m_buff����������邱�Ƃ͂Ȃ��B
  }

  // �A�^�b�`�����o�b�t�@�̊Ǘ������̃C���X�^���X�ōs���ꍇ
  if( GetOwnBufferFlag() )
  {
    if( m_buff )
    {
      GflHeapFreeMemory(m_buff);
    }
  }

  // �R���X�g���N�^�Ɠ���������
  m_mem_loc = MemoryLocation();
  m_address_map = nw::h3d::res::ResourceBinary::AddressMap();
  m_srcRawAddress = 0;
  m_buff = NULL;
  m_resbin = NULL;
  m_bit_flag = BIT_FLAG_NONE;
  m_result_desc = RESULT_OK;
}

void H3dResource::Map(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    const MemoryLocation* mem_loc,
    gfl::heap::NwAllocator* vram_copy_heap_allocator
)
{
  // NintendoWare/CTR/demos/h3d/Demos/sources/scene.cpp
  // #if RAW_DATA_AND_COMMAND_ON_VRAM
  // nw::h3d::res::ResourceBinary*
  // ContentsFactory::CreateResourceBinary(void* buffer)

  // NintendoWare/CTR/sources/libraries/gfx/res/gfx_ResTexture.cpp
  // static void
  // TexImage2D( u32 texID, ResPixelBasedImage resImage, u32 loadFlag )

  if( !(m_bit_flag & BIT_FLAG_IS_INIT) )
  {
    if( m_buff )
    {
      ////////////////////////////////////////////////////////
      if(mem_loc) m_mem_loc = *mem_loc;
      
      ////////////////////////////////////////////////////////
      nw::h3d::res::ResourceBinary* binary = GFL_NEW(heap_allocator->GetHeapBase()) nw::h3d::res::ResourceBinary;
      
      nw::h3d::res::ResourceBinary::Header* header = reinterpret_cast<nw::h3d::res::ResourceBinary::Header*>(m_buff);
      m_address_map = nw::h3d::res::ResourceBinary::AddressMap();
      m_address_map.Map(header);
      m_srcRawAddress = 0;

      // RAW�Z�N�V����(FileSectionType::RAW)
      if( m_mem_loc.IsRawSectionVram() )
      {
        // RAW
        m_srcRawAddress = m_address_map.sectionAddress[nw::h3d::FileSectionType::RAW];
 
        // VRAM  RAW
        if( header->sectionSize[nw::h3d::FileSectionType::RAW] )
        {
          m_address_map.sectionAddress[nw::h3d::FileSectionType::RAW] = reinterpret_cast<u32>(
            gfl::grp::GraphicsSystem::AllocMemory(
                m_mem_loc.raw_section,
                gfl::grp::MEM_TEXTURE,  // @note type�̓A���C�������g�̌���Ɏg����̂�NN_GX_MEM_VERTEXBUFFER���傫���e�N�X�`���ɂ��Ă����B
                                        // ���ׂĂȂ����Ǒ����e�N�X�`���̂ق����A���C�������g���傫���͂��B
                                        // ���Ȃ݂Ɏg���Ă��Ȃ����ǒ��_�C���f�b�N�X�X�g���[���ƒ��_�X�g���[���͂ǂ����NN_GX_MEM_VERTEXBUFFER�ł����̂��ȁB
                nw::ut::RoundUp(header->sectionSize[nw::h3d::FileSectionType::RAW], nw::h3d::Alignment::RAW_SECTION)  // @note �T�C�Y�����łȂ��擪�A�h���X��nw::h3d::Alignment::RAW_SECTION��MathRoundup�����ق������������������MemoryControl::Alloc��MemoryUseType�����Ă���Ă���Ă���Asize������Ă���Ă��邩���B
            )
          );
        }
        else
        {
          m_address_map.sectionAddress[nw::h3d::FileSectionType::RAW] = 0;
        }
      }
 
      // �R�}���h�Z�N�V����(FileSectionType::COMMAND��FileSectionType::UNINIT_COMMAND)
      if( m_mem_loc.IsCommandSectionVram() )
      {
        // COMMAND
        m_address_map.srcCmdAddress = m_address_map.sectionAddress[nw::h3d::FileSectionType::COMMAND];
 
        // UINIT_COMMAND
        if( header->sectionSize[nw::h3d::FileSectionType::UNINIT_COMMAND] )  // �T�C�Y0�̂Ƃ����L�蓾��B���̂Ƃ��̓A�h���XNULL�ŁB
        {
          if(!vram_copy_heap_allocator) vram_copy_heap_allocator = heap_allocator;
          m_address_map.srcUCmdAddress = reinterpret_cast<u32>(
              GflHeapAllocMemoryAlign( vram_copy_heap_allocator->GetHeapBase(),
                  header->sectionSize[nw::h3d::FileSectionType::UNINIT_COMMAND],
                  nw::h3d::Alignment::DMA_SOURCE )
          );
        }
        else
        {
          m_address_map.srcUCmdAddress = 0;
        }
 
        // VRAM  COMMAND
        if( header->sectionSize[nw::h3d::FileSectionType::COMMAND] )
        {
          m_address_map.sectionAddress[nw::h3d::FileSectionType::COMMAND] = reinterpret_cast<u32>(
            gfl::grp::GraphicsSystem::AllocMemory(
                m_mem_loc.command_section,
                gfl::grp::MEM_COMMANDBUFFER,
                nw::ut::RoundUp(header->sectionSize[nw::h3d::FileSectionType::COMMAND], nw::h3d::Alignment::COMMAND_SECTION)  // @note �T�C�Y�����łȂ��擪�A�h���X��nw::h3d::Alignment::COMMAND_SECTION��MathRoundup�����ق������������������MemoryControl::Alloc��MemoryUseType�����Ă���Ă���Ă���Asize������Ă���Ă��邩���B
            )
          );
        }
        else
        {
          m_address_map.sectionAddress[nw::h3d::FileSectionType::COMMAND] = 0;
        }
 
        // VRAM  UNINIT_COMMAND
        if( header->sectionSize[nw::h3d::FileSectionType::UNINIT_COMMAND] )
        {
          m_address_map.sectionAddress[nw::h3d::FileSectionType::UNINIT_COMMAND] = reinterpret_cast<u32>(
            gfl::grp::GraphicsSystem::AllocMemory(
                m_mem_loc.command_section,
                gfl::grp::MEM_COMMANDBUFFER,
                nw::ut::RoundUp(header->sectionSize[nw::h3d::FileSectionType::UNINIT_COMMAND], nw::h3d::Alignment::UNINIT_COMMAND_SECTION)  // @note �T�C�Y�����łȂ��擪�A�h���X��nw::h3d::Alignment::UNINIT_COMMAND_SECTION��MathRoundup�����ق����������� > AllocMemory�֐��̒��Ŏg�p�p�r�^�C�v���ƂɃA���C�������g����Ă���̂��m�F
            )
          );
        }
        else
        {
          m_address_map.sectionAddress[nw::h3d::FileSectionType::UNINIT_COMMAND] = 0;
        }
      }
      else
      {
        if( header->sectionSize[nw::h3d::FileSectionType::UNINIT_COMMAND] )  // �T�C�Y0�̂Ƃ����L�蓾��B���̂Ƃ��̓A�h���XNULL�ŁB
        {
          m_address_map.sectionAddress[nw::h3d::FileSectionType::UNINIT_COMMAND] = reinterpret_cast<u32>(
              GflHeapAllocMemoryAlign( device_allocator->GetHeapBase(),
                  header->sectionSize[nw::h3d::FileSectionType::UNINIT_COMMAND],
                  nw::h3d::Alignment::UNINIT_COMMAND_SECTION )
          );
        }
        else
        {
          m_address_map.sectionAddress[nw::h3d::FileSectionType::UNINIT_COMMAND] = 0;
        }
      }
 
      // ���������̃f�[�^�Z�N�V����
      {
        // UINIT_DATA
        if( header->sectionSize[nw::h3d::FileSectionType::UNINIT_DATA] )  // �T�C�Y0�̂Ƃ����L�蓾��B���̂Ƃ��̓A�h���XNULL�ŁB
        {
          m_address_map.sectionAddress[nw::h3d::FileSectionType::UNINIT_DATA] = reinterpret_cast<u32>(
                GflHeapAllocMemoryAlign( heap_allocator->GetHeapBase(),
                    header->sectionSize[nw::h3d::FileSectionType::UNINIT_DATA],
                    nw::h3d::Alignment::UNINIT_DATA_SECTION )
          );
        }
        else
        {
          m_address_map.sectionAddress[nw::h3d::FileSectionType::UNINIT_DATA] = 0;
        }
      }
 
      ////////////////////////////////////////////////////////
      // ������
      binary->Init(m_buff, m_address_map);

      m_resbin = binary;
      m_bit_flag |= BIT_FLAG_IS_INIT;
    }
  }

  ////////////////////////////////////////////////////////
  // linkOwn
  linkOwn();

  ////////////////////////////////////////////////////////
  // VRAM�ɓ]��
  if( m_bit_flag & BIT_FLAG_IS_LINK_OWN)
  {
    if ( m_buff )
    {//KW12�C��
      nw::h3d::res::ResourceBinary::Header* header = reinterpret_cast<nw::h3d::res::ResourceBinary::Header*>(m_buff);

      // RAW�Z�N�V����(FileSectionType::RAW)
      if( m_mem_loc.IsRawSectionVram() )
      {
        if( header->sectionSize[nw::h3d::FileSectionType::RAW] )
        {
            nngxAddVramDmaCommandNoCacheFlush(
                reinterpret_cast<const GLvoid*>(m_srcRawAddress),
                reinterpret_cast<GLvoid*>(m_address_map.sectionAddress[nw::h3d::FileSectionType::RAW]),
                header->sectionSize[nw::h3d::FileSectionType::RAW]);
        }
      }

      // �R�}���h�Z�N�V����(FileSectionType::COMMAND��FileSectionType::UNINIT_COMMAND)
      if( m_mem_loc.IsCommandSectionVram() )
      {
        if (header->sectionSize[nw::h3d::FileSectionType::COMMAND])
        {
            nngxAddVramDmaCommandNoCacheFlush(
                reinterpret_cast<const GLvoid*>(m_address_map.srcCmdAddress),
                reinterpret_cast<GLvoid*>(m_address_map.sectionAddress[nw::h3d::FileSectionType::COMMAND]),
                header->sectionSize[nw::h3d::FileSectionType::COMMAND]);
        }

        if (header->sectionSize[nw::h3d::FileSectionType::UNINIT_COMMAND])
        {
            nngxAddVramDmaCommandNoCacheFlush(
                reinterpret_cast<const GLvoid*>(m_address_map.srcUCmdAddress),
                reinterpret_cast<GLvoid*>(m_address_map.sectionAddress[nw::h3d::FileSectionType::UNINIT_COMMAND]),
                header->sectionSize[nw::h3d::FileSectionType::UNINIT_COMMAND]);
        }
      }
    }
  }

}



void H3dResource::DeleteRelocationTableSection( gfl::heap::NwAllocator* allocator )
{
  if( IsSetup() )
  {
    nw::h3d::res::ResourceBinary::Header* header = reinterpret_cast<nw::h3d::res::ResourceBinary::Header*>(m_buff);
    size_t requestSize = m_address_map.sectionAddress[nw::h3d::FileSectionType::RELOCATABLE_TABLE] - nw::ut::GetIntPtr(m_buff);
    size_t curr_size = allocator->GetHeapBase()->GetMemorySize( const_cast<void*>(m_buff) );
    GFL_ASSERT_MSG(curr_size>=requestSize, "���̃T�C�Y���傫�ȃT�C�Y�Ƀ��T�C�Y���悤�Ƃ��Ă��܂��Bcurr%d, req=%d\n", curr_size, requestSize);  //@check ���T�C�Y�Ɏ��s���邾���B
    allocator->GetHeapBase()->ResizeMemory( const_cast<void*>(m_buff), requestSize );
  }
  else
  {
    GFL_ASSERT_MSG(0, "�Z�b�g�A�b�v���������Ă��܂���BVRAM�ւ̃R�s�[�R�}���h�������C��t���āB\n");
  }
}

void H3dResource::DeleteRawSection( gfl::heap::NwAllocator* allocator )
{
  DeleteRelocationTableSection( allocator );
  
  if( IsSetup() )
  {
    if( m_mem_loc.IsRawSectionVram() )
    {
      if( m_srcRawAddress )
      {
        nw::h3d::res::ResourceBinary::Header* header = reinterpret_cast<nw::h3d::res::ResourceBinary::Header*>(m_buff);
        size_t requestSize = m_srcRawAddress - nw::ut::GetIntPtr(m_buff);
        size_t curr_size = allocator->GetHeapBase()->GetMemorySize( const_cast<void*>(m_buff) );
        GFL_ASSERT_MSG(curr_size>=requestSize, "���̃T�C�Y���傫�ȃT�C�Y�Ƀ��T�C�Y���悤�Ƃ��Ă��܂��Bcurr%d, req=%d\n", curr_size, requestSize);  //@check ���T�C�Y�Ɏ��s���邾���B
        allocator->GetHeapBase()->ResizeMemory( const_cast<void*>(m_buff), requestSize );
      }
    }
  }
  else
  {
    GFL_ASSERT_MSG(0, "�Z�b�g�A�b�v���������Ă��܂���BVRAM�ւ̃R�s�[�R�}���h�������C��t���āB\n");
  }
}

void H3dResource::DeleteCommandSection( gfl::heap::NwAllocator* allocator )
{
   DeleteRawSection(allocator);

  if( IsSetup() )
  {
    if( m_mem_loc.IsCommandSectionVram() )  // @note �R�}���h��VRAM�ɓ]������l�����ɂ����p�q�[�v��������n���Ă��炤�ƁA���H���������ɂȂ�Ȃ������B2/2 �� Map�֐���vram_copy_heap_allocator��ǉ����܂����B
    {
      if(m_address_map.srcUCmdAddress)
      {
        GflHeapFreeMemory(reinterpret_cast<void*>(m_address_map.srcUCmdAddress));
        m_address_map.srcUCmdAddress = 0;
      }
    }

    if( m_mem_loc.IsCommandSectionVram() && m_mem_loc.IsRawSectionVram() )
    {
      nw::h3d::res::ResourceBinary::Header* header = reinterpret_cast<nw::h3d::res::ResourceBinary::Header*>(m_buff);
      size_t requestSize = nw::ut::RoundUp(
          m_address_map.sectionAddress[nw::h3d::FileSectionType::STRING] +
          header->sectionSize[nw::h3d::FileSectionType::STRING] -
          nw::ut::GetIntPtr(m_buff),
          4
      );
      size_t curr_size = allocator->GetHeapBase()->GetMemorySize( const_cast<void*>(m_buff) );
      GFL_ASSERT_MSG(curr_size>=requestSize, "���̃T�C�Y���傫�ȃT�C�Y�Ƀ��T�C�Y���悤�Ƃ��Ă��܂��Bcurr%d, req=%d\n", curr_size, requestSize);  //@check ���T�C�Y�Ɏ��s���邾���B
      allocator->GetHeapBase()->ResizeMemory( const_cast<void*>(m_buff), requestSize );
    }
  }
  else
  {
    GFL_ASSERT_MSG(0, "�Z�b�g�A�b�v���������Ă��܂���BVRAM�ւ̃R�s�[�R�}���h�������C��t���āB\n");
  }
}

void H3dResource::DeleteSomeSection( gfl::heap::NwAllocator* allocator )
{
  DeleteCommandSection(allocator);
}



//----------------------------------------------------------------------------
/**
 *	@brief  �q�����f�[�^�̐擪�A�h���X���擾
 */
//-----------------------------------------------------------------------------
uptr H3dResource::GetSrcRawAddress( void ) const
{
  if( IsSetup() )
  {
    if( m_mem_loc.IsRawSectionVram() )
    {
      if( m_srcRawAddress )
      {
        return m_srcRawAddress;
      }
    }
  }
  else
  {
    GFL_ASSERT_MSG(0, "�Z�b�g�A�b�v���������Ă��܂���BVRAM�ւ̃R�s�[�R�}���h�������C��t���āB\n");
  }
  return 0;
}





b32 H3dResource::Setup(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, H3dResource* other_resource, u32 common_resource_flag)
{
  b32 ret = false;
  init(heap_allocator, device_allocator);
  linkOwn();
  linkOther(other_resource);

  {
    if( common_resource_flag & COMMON_RESOURCE_H3D_DEFAULT_SHADER_BIT )
    {
      H3dResource* common_resource = GFL_SINGLETON_INSTANCE(System)->GetCommonDataH3dDefaultShaderResource();
      linkOther(common_resource);
    }
    //if( common_resource_flag & COMMON_RESOURCE_??? )
    //{
    //  H3dResource* common_resource = GFL_SINGLETON_INSTANCE(System)->Get???Rsource();
    //  linkOther(common_resource);
    //}
  }

  if(m_bit_flag & BIT_FLAG_IS_SETUP)
  {
    ret = true;
  }
  return ret;
}

b32 H3dResource::SetupUsingSystemTexture(void)
{
//NintendoWare/CTR/sources/libraries/h3d/h3d_ResourceImpl.h
//Result
//ModelContent::Link(const char* name, TextureContent* textureContent, u32 srcCmdAddress, u32 cmdAddress)

  H3dDynamicTexture* tex = GFL_SINGLETON_INSTANCE(System)->GetCommonDataH3dSmallTexture();

  if( tex )
  {
    if( !(m_bit_flag & BIT_FLAG_IS_SETUP) )
    {
      if(m_bit_flag & BIT_FLAG_IS_LINK_OWN)
      {
        b32 is_success_all = true;
        for(u32 i=0; i<m_resbin->GetModelCount(); ++i)
        {
          nw::h3d::res::ModelContent* model = m_resbin->GetModel(i);
          if(model)
          {
            nw::h3d::fnd::DataArray<nw::h3d::res::MaterialData>::iterator end = model->materials.end();
            for(nw::h3d::fnd::DataArray<nw::h3d::res::MaterialData>::iterator iter = model->materials.begin(); iter != end; ++iter)
            {
              // �e�N�X�`���̎Q�Ɖ���
              nw::h3d::res::TextureContent** textures[nw::h3d::Constants::PIXELBASED_TEXTURE_UNIT_QUANTITY] =
              {
                &iter->texture0, &iter->texture1, &iter->texture2
              };

              for(int tIdx = 0; tIdx < nw::h3d::Constants::PIXELBASED_TEXTURE_UNIT_QUANTITY; ++tIdx)
              {
                nw::h3d::res::TextureContent*& content = *textures[tIdx];
                if( content == NULL && iter->textureNames[tIdx])
                {
                  nw::h3d::Result result(nw::h3d::Result::RESULT_OK);
                 
                  u32 srcCmdAddress = 0;
                  u32 cmdAddress = 0;
                  if( m_mem_loc.IsCommandSectionVram() )
                  {
                    srcCmdAddress = m_address_map.srcCmdAddress;
                  }
                  if( m_mem_loc.IsCommandSectionVram() )
                  {
                    cmdAddress = m_address_map.sectionAddress[nw::h3d::FileSectionType::COMMAND];
                  }
                  result = model->Link(  // ��x�Q�Ɖ������ꂽ�v�f�́A��� Link ���s���Ă��㏑������邱�Ƃ͂���܂���B
                      iter->textureNames[tIdx],
                      tex->GetNwTextureContent(), 
                      srcCmdAddress,
                      cmdAddress
                  );  // @note NintendoWare��ModelContent::Link�e�N�X�`���ł�result�̂�����̓��[�v�񂵂Ă���Ӗ����Ȃ��̂ŊԈ���Ă���C������B1/2
                 
                  if( !(result.IsSuccess()) )
                  {
                    is_success_all = false;
                  }
                  m_result_desc = result.GetDescription();
                }
              }
            }
          }
        }
        
        // �e�N�X�`���̎Q�Ɖ����Ȃ̂Ń��f�������ł悭�A���C�g�͂��K�v���Ȃ�
        
        if( is_success_all )
        {
          m_bit_flag |= BIT_FLAG_IS_SETUP;
        }
        
        // @note NintendoWare��ModelContent::Link�e�N�X�`���ł�result�̂�����̓��[�v�񂵂Ă���Ӗ����Ȃ��̂ŊԈ���Ă���C������B1/2
        // �Ǝ��̏I�����������
        if( isAllReferenceResolved() )
        {
          m_bit_flag |= BIT_FLAG_IS_SETUP;
        }
      }
    }
  }

  if(m_bit_flag & BIT_FLAG_IS_SETUP)
  {
    return true;
  }
  return false;
}

#if GFL_DEBUG
void printModelName(b32* print_flag, const char* print_name)
{
  if( !(*print_flag) )
  {
    if( print_name )
    {
      GFL_PRINT("Unresolve Model Name [%s]\n", print_name);
    }
    else
    {
      GFL_PRINT("Unresolve Model Name NONE\n");
    }
    *print_flag = true;
  }
}
void printMaterialName(b32* print_flag, const char* print_name)
{
  if( !(*print_flag) )
  {
    if( print_name )
    {
      GFL_PRINT("  Material Name [%s]\n", print_name);
    }
    else
    {
      GFL_PRINT("  Material Name NONE\n");
    }
    *print_flag = true;
  }
}
#endif

void H3dResource::PrintUnresolvedReference(void) const
{
// NintendoWare/CTR/sources/libraries/h3d/
// h3d_ResourceImpl.h
//
// Result
// ModelContent::Link(ResourceBinary& binary, u32 srcCmdAddress, u32 cmdAddress)

#if GFL_DEBUG

  for(u32 i=0; i<m_resbin->GetModelCount(); ++i)
  {
    nw::h3d::res::ModelContent* model = m_resbin->GetModel(i);
    if(model)
    {
      b32 model_name_print = false;

      nw::h3d::fnd::DataArray<nw::h3d::res::MaterialData>::iterator end = model->materials.end();
      for(nw::h3d::fnd::DataArray<nw::h3d::res::MaterialData>::iterator iter = model->materials.begin(); iter != end; ++iter)
      {
        b32 material_name_print = false;

        nw::h3d::res::MaterialContent& material = *iter->content;

        // �V�F�[�_�̎Q�Ɖ����̖�����
        if(material.shaderContent == NULL)
        {
          printModelName(&model_name_print, model->name);
          printMaterialName(&material_name_print, material.name);

          if(material.shaderName)
          {
            GFL_PRINT("    Shader Name [%s]\n", material.shaderName);
          }
          else
          {
            GFL_PRINT("    Shader Name NONE\n");
          }
        }

        // ���b�N�A�b�v�e�[�u���̎Q�Ɖ����̖�����
        for(int lIdx = 0; lIdx < nw::h3d::MaterialLutType::QUANTITY; ++lIdx)
        {
          if(material.lutData[lIdx] == NULL && material.lutPaths[lIdx] && material.lutNames[lIdx])
          {
            printModelName(&model_name_print, model->name);
            printMaterialName(&material_name_print, material.name);

            GFL_PRINT("    Lut %d Name [%s]\n", lIdx, material.lutPaths[lIdx]);
          }
        }

        // �e�N�X�`���̎Q�Ɖ����̖�����
        nw::h3d::res::TextureContent** textures[nw::h3d::Constants::PIXELBASED_TEXTURE_UNIT_QUANTITY] =
        {
          &iter->texture0, &iter->texture1, &iter->texture2
        };

        for(int tIdx = 0; tIdx < nw::h3d::Constants::PIXELBASED_TEXTURE_UNIT_QUANTITY; ++tIdx)
        {
          nw::h3d::res::TextureContent*& content = *textures[tIdx];
          if(content == NULL && iter->textureNames[tIdx])
          {
            printModelName(&model_name_print, model->name);
            printMaterialName(&material_name_print, material.name);

            GFL_PRINT("    Texture %d Name [%s]\n", tIdx, iter->textureNames[tIdx]);
          }
        }
      }

    }
  }
  
  // @note ���f�������łȂ����C�g�����K�v�����遨�f�o�b�O�p�r�����狖���āB

#endif
}

b32 H3dResource::isAllReferenceResolved(void) const
{
// NintendoWare/CTR/sources/libraries/h3d/
// h3d_ResourceImpl.h
//
// Result
// ModelContent::Link(ResourceBinary& binary, u32 srcCmdAddress, u32 cmdAddress)

  b32 is_resolved = true;

  for(u32 i=0; i<m_resbin->GetModelCount(); ++i)
  {
    nw::h3d::res::ModelContent* model = m_resbin->GetModel(i);
    if(model)
    {
      nw::h3d::fnd::DataArray<nw::h3d::res::MaterialData>::iterator end = model->materials.end();
      for(nw::h3d::fnd::DataArray<nw::h3d::res::MaterialData>::iterator iter = model->materials.begin(); iter != end; ++iter)
      {
        nw::h3d::res::MaterialContent& material = *iter->content;

        // �V�F�[�_�̎Q�Ɖ����̖�����
        if(material.shaderContent == NULL)
        {
          is_resolved = false;
          break;
        }

        // ���b�N�A�b�v�e�[�u���̎Q�Ɖ����̖�����
        for(int lIdx = 0; lIdx < nw::h3d::MaterialLutType::QUANTITY; ++lIdx)
        {
          if(material.lutData[lIdx] == NULL && material.lutPaths[lIdx] && material.lutNames[lIdx])
          {
            is_resolved = false;
            break;
          }
        }
        if( !is_resolved ) break;

        // �e�N�X�`���̎Q�Ɖ����̖�����
        nw::h3d::res::TextureContent** textures[nw::h3d::Constants::PIXELBASED_TEXTURE_UNIT_QUANTITY] =
        {
          &iter->texture0, &iter->texture1, &iter->texture2
        };

        for(int tIdx = 0; tIdx < nw::h3d::Constants::PIXELBASED_TEXTURE_UNIT_QUANTITY; ++tIdx)
        {
          nw::h3d::res::TextureContent*& content = *textures[tIdx];
          if(content == NULL && iter->textureNames[tIdx])
          {
            is_resolved = false;
            break;
          }
        }
        if( !is_resolved ) break;
      }

    }

    if( !is_resolved ) break;
  }
  
  // @note ���f�������łȂ����C�g�����K�v�����遨h3d�̃��C�g��gflib_cpp�ł͈����Ă��Ȃ��B
  
  return is_resolved;
}

b32 H3dResource::AttachBufferAndSetup(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, void* buff, H3dResource* other_resource, u32 common_resource_flag)
{
  b32 ret = AttachBuffer(buff);
  if(ret)
  {
    ret = Setup(heap_allocator, device_allocator, other_resource, common_resource_flag);
  }
  return ret;
}

s32 H3dResource::GetModelContentIndex(const char* name) const
{
  if( m_resbin )
  {
    u16 idx = m_resbin->GetModelIndex(name);
    if( idx != nw::h3d::Constants::NOT_FOUND_INDEX )  //if( idx != nw::h3d::fnd::PatriciaDic::NOT_FOUND_INDEX )
    {
      return idx;
    }
  }
  return INDEX_NONE;
}
s32 H3dResource::GetSkeletalAnimContentIndex(const char* name) const
{
  if( m_resbin )
  {
    u16 idx = m_resbin->GetSkeletalAnimIndex(name);
    if(idx != nw::h3d::Constants::NOT_FOUND_INDEX)
    {
      return idx;
    }
  }
  return INDEX_NONE;
}
s32 H3dResource::GetMaterialAnimContentIndex(const char* name) const
{
  if( m_resbin )
  {
    u16 idx = m_resbin->GetMaterialAnimIndex(name);
    if(idx != nw::h3d::Constants::NOT_FOUND_INDEX)
    {
      return idx;
    }
  }
  return INDEX_NONE;
}
s32 H3dResource::GetVisibilityAnimContentIndex(const char* name) const
{
  if( m_resbin )
  {
    u16 idx = m_resbin->GetVisibilityAnimIndex(name);
    if(idx != nw::h3d::Constants::NOT_FOUND_INDEX)
    {
      return idx;
    }
  }
  return INDEX_NONE;
}
s32 H3dResource::GetTextureContentIndex(const char* name) const
{
  if( m_resbin )
  {
    u16 idx = m_resbin->GetTextureIndex(name);
    if( idx != nw::h3d::Constants::NOT_FOUND_INDEX )
    {
      return idx;
    }
  }
  return INDEX_NONE;
}

#ifdef BUGFIX_GFBTS1721_20130801
s32 H3dResource::GetMaterialCount( s32 index_in_resource ) const
{
  if( m_resbin )
  {
    const nw::h3d::res::ModelContent*     pCont;
    pCont = GetNwModelContent(index_in_resource);
    if ( pCont )
    {
      return pCont->materials.size();
    }
  }
  return -1;
}
#endif

s32 H3dResource::GetShaderContentIndex(const char* name) const
{
  if( m_resbin )
  {
    u16 idx = m_resbin->GetShaderIndex(name);
    if( idx != nw::h3d::Constants::NOT_FOUND_INDEX )
    {
      return idx;
    }
  }
  return INDEX_NONE;
}

const char* H3dResource::GetShaderContentName(s32 index) const
{
  if( m_resbin )
  {
    const nw::h3d::res::ShaderContent* cont = m_resbin->GetShader(index);
    if(cont)
    {
      return cont->name;
    }
  }
  return NULL;
}

u32 H3dResource::GetModelContentCount(void) const
{
  if( m_resbin )
  {
    return m_resbin->GetModelCount();
  }
  return 0;
}
u32 H3dResource::GetSkeletalAnimContentCount(void) const
{
  if( m_resbin )
  {
    return m_resbin->GetSkeletalAnimCount();
  }
  return 0;
}
u32 H3dResource::GetMaterialAnimContentCount(void) const
{
  if( m_resbin )
  {
    return m_resbin->GetMaterialAnimCount();
  }
  return 0;
}
u32 H3dResource::GetVisibilityAnimContentCount(void) const
{
  if( m_resbin )
  {
    return m_resbin->GetVisibilityAnimCount();
  }
  return 0;
}
u32 H3dResource::GetTextureContentCount(void) const
{
  if( m_resbin )
  {
    return m_resbin->GetTextureCount();
  }
  return 0;
}
u32 H3dResource::GetShaderContentCount(void) const
{
  if( m_resbin )
  {
    return m_resbin->GetShaderCount();
  }
  return 0;
}

u32 H3dResource::GetSkeletalAnimElementNum(s32 index_in_resource) const
{
  nw::h3d::res::SkeletalAnimContent* skel = GetNwSkeletalAnimContent(index_in_resource);
  if(skel)
  {
    return skel->GetAnimElementCount();
  }
  return 0;
}
u32 H3dResource::GetMaterialAnimElementNum(s32 index_in_resource) const
{
  nw::h3d::res::MaterialAnimContent* mate = GetNwMaterialAnimContent(index_in_resource);
  if(mate)
  {
    return mate->GetAnimElementCount();
  }
  return 0;
}
u32 H3dResource::GetVisibilityAnimElementNum(s32 index_in_resource) const
{
  nw::h3d::res::VisibilityAnimContent* visi = GetNwVisibilityAnimContent(index_in_resource);
  if(visi)
  {
    return visi->GetAnimElementCount();
  }
  return 0;
}

nw::h3d::res::ModelContent* H3dResource::GetNwModelContent(const char* name) const
{
  return GetNwModelContent( GetModelContentIndex(name) );
}
nw::h3d::res::ModelContent* H3dResource::GetNwModelContent(const s32 index) const
{
  if( index >= 0 )
  {
    if( m_resbin )
    {
      return m_resbin->GetModel(index);
    }
  }
  return NULL;
}

nw::h3d::res::SkeletalAnimContent* H3dResource::GetNwSkeletalAnimContent(const char* name) const
{
  return GetNwSkeletalAnimContent( GetSkeletalAnimContentIndex(name) );
}
nw::h3d::res::SkeletalAnimContent* H3dResource::GetNwSkeletalAnimContent(const s32 index) const
{
  if( index >= 0 )
  {
    if( m_resbin )
    {
      return m_resbin->GetSkeletalAnim(index);
    }
  }
  return NULL;
}

nw::h3d::res::MaterialAnimContent* H3dResource::GetNwMaterialAnimContent(const char* name) const
{
  return GetNwMaterialAnimContent( GetMaterialAnimContentIndex(name) );
}
nw::h3d::res::MaterialAnimContent* H3dResource::GetNwMaterialAnimContent(const s32 index) const
{
  if( index >= 0 )
  {
    if( m_resbin )
    {
      return m_resbin->GetMaterialAnim(index);
    }
  }
  return NULL;
}

nw::h3d::res::VisibilityAnimContent* H3dResource::GetNwVisibilityAnimContent(const char* name) const
{
  return GetNwVisibilityAnimContent( GetVisibilityAnimContentIndex(name) );
}
nw::h3d::res::VisibilityAnimContent* H3dResource::GetNwVisibilityAnimContent(const s32 index) const
{
  if( index >= 0 )
  {
    if( m_resbin )
    {
      return m_resbin->GetVisibilityAnim(index);
    }
  }
  return NULL;
}

nw::h3d::res::TextureContent* H3dResource::GetNwTextureContent(const char* name) const
{
  return GetNwTextureContent( GetTextureContentIndex(name) );
}
nw::h3d::res::TextureContent* H3dResource::GetNwTextureContent(const s32 index) const
{
  if( index >= 0 )
  {
    if( m_resbin )
    {
      return m_resbin->GetTexture(index);
    }
  }
  return NULL;
}
nw::h3d::res::ShaderContent* H3dResource::GetNwShaderContent(const char* name) const
{
  return GetNwShaderContent( GetShaderContentIndex(name) );
}
nw::h3d::res::ShaderContent* H3dResource::GetNwShaderContent(const s32 index) const
{
  if( index >= 0 )
  {
    if( m_resbin )
    {
      return m_resbin->GetShader(index);
    }
  }
  return NULL;
}

void H3dResource::init(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator)
{
  if( !(m_bit_flag & BIT_FLAG_IS_INIT) )
  {
    if( m_buff )
    {
      nw::h3d::res::ResourceBinary* binary = GFL_NEW(heap_allocator->GetHeapBase()) nw::h3d::res::ResourceBinary;
      
      nw::h3d::res::ResourceBinary::Header* header = reinterpret_cast<nw::h3d::res::ResourceBinary::Header*>(m_buff);
      m_address_map = nw::h3d::res::ResourceBinary::AddressMap();
      m_address_map.Map(header);
      m_srcRawAddress = 0;
 
      if( header->sectionSize[nw::h3d::FileSectionType::UNINIT_DATA] )  // �T�C�Y0�̂Ƃ����L�蓾��B���̂Ƃ��̓A�h���XNULL�ŁB
      {
        m_address_map.sectionAddress[nw::h3d::FileSectionType::UNINIT_DATA] = reinterpret_cast<u32>(
            GflHeapAllocMemoryAlign(heap_allocator->GetHeapBase(), header->sectionSize[nw::h3d::FileSectionType::UNINIT_DATA], nw::h3d::Alignment::UNINIT_DATA_SECTION));
      }
      else
      {
        m_address_map.sectionAddress[nw::h3d::FileSectionType::UNINIT_DATA] = 0;
      }
      if( header->sectionSize[nw::h3d::FileSectionType::UNINIT_COMMAND] )  // �T�C�Y0�̂Ƃ����L�蓾��B���̂Ƃ��̓A�h���XNULL�ŁB
      {
        m_address_map.sectionAddress[nw::h3d::FileSectionType::UNINIT_COMMAND] = reinterpret_cast<u32>(
            GflHeapAllocMemoryAlign(device_allocator->GetHeapBase(), header->sectionSize[nw::h3d::FileSectionType::UNINIT_COMMAND], nw::h3d::Alignment::UNINIT_COMMAND_SECTION));
      }
      else
      {
        m_address_map.sectionAddress[nw::h3d::FileSectionType::UNINIT_COMMAND] = 0;
      }
 
      // ������
      binary->Init(m_buff, m_address_map);

      m_resbin = binary;
      m_bit_flag |= BIT_FLAG_IS_INIT;
    }
  }
}
void H3dResource::linkOwn(void)
{
  // linkOwn
  if( !(m_bit_flag & BIT_FLAG_IS_LINK_OWN) )
  {
    if(m_bit_flag & BIT_FLAG_IS_INIT)
    {
      b32 is_success_all = true;
      for(u32 i=0; i<m_resbin->GetModelCount(); ++i)
      {
        nw::h3d::res::ModelContent* model = m_resbin->GetModel(i);
        if(model)
        {
          nw::h3d::Result result(nw::h3d::Result::RESULT_OK);

          u32 srcCmdAddress = 0;
          u32 cmdAddress = 0;
          if( m_mem_loc.IsCommandSectionVram() )
          {
            srcCmdAddress = m_address_map.srcCmdAddress;
          }
          if( m_mem_loc.IsCommandSectionVram() )
          {
            cmdAddress = m_address_map.sectionAddress[nw::h3d::FileSectionType::COMMAND];
          }
          result = model->Link(  // ��x�Q�Ɖ������ꂽ�v�f�́A��� Link ���s���Ă��㏑������邱�Ƃ͂���܂���B
              *m_resbin,
              srcCmdAddress,
              cmdAddress
          );

          if( !(result.IsSuccess()) )
          {
            is_success_all = false;
          }
          m_result_desc = result.GetDescription();
        }
        else
        {
          GFL_ASSERT(model);  //@check
        }
      }
      
      // @note ���f�������łȂ����C�g�����K�v�����遨h3d�̃��C�g��gflib_cpp�ł͈����Ă��Ȃ��B

      m_bit_flag |= BIT_FLAG_IS_LINK_OWN;
      if( is_success_all )
      {
        m_bit_flag |= BIT_FLAG_IS_SETUP;
      }
    }
  }
}

void H3dResource::linkOther(H3dResource* other_resource)
{
  if( other_resource )
  {
    if( !(m_bit_flag & BIT_FLAG_IS_SETUP) )
    {
      if(m_bit_flag & BIT_FLAG_IS_LINK_OWN)
      {
        b32 is_success_all = true;
        for(u32 i=0; i<m_resbin->GetModelCount(); ++i)
        {
          nw::h3d::res::ModelContent* model = m_resbin->GetModel(i);
          if(model)
          {
            nw::h3d::Result result(nw::h3d::Result::RESULT_OK);

            u32 srcCmdAddress = 0;
            u32 cmdAddress = 0;
            if( m_mem_loc.IsCommandSectionVram() )
            {
              srcCmdAddress = m_address_map.srcCmdAddress;
            }
            if( m_mem_loc.IsCommandSectionVram() )
            {
              cmdAddress = m_address_map.sectionAddress[nw::h3d::FileSectionType::COMMAND];
            }
            result = model->Link(  // ��x�Q�Ɖ������ꂽ�v�f�́A��� Link ���s���Ă��㏑������邱�Ƃ͂���܂���B
                *(other_resource->GetNwResourceBinary()),
                srcCmdAddress,
                cmdAddress
            );
 
            if( !(result.IsSuccess()) )
            {
              is_success_all = false;
            }
            m_result_desc = result.GetDescription();
          }
        }
        
        // @note ���f�������łȂ����C�g�����K�v�����遨h3d�̃��C�g��gflib_cpp�ł͈����Ă��Ȃ��B
        
        if( is_success_all )
        {
          m_bit_flag |= BIT_FLAG_IS_SETUP;
        }
      }
    }
  }
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE

