#include <gfx/include/gfl2_GLMemory.h>

namespace gfl2 { namespace gfx {

IGLAllocator				*GLMemory::s_pUserAllocator = NULL;
IGLAllocator				*GLMemory::s_pInstanceAllocator = NULL;
int                  GLMemory::s_InstanceAllocatorCount = 0;
IGLAllocator				*GLMemory::s_pNewParameterAllocator = NULL;

}}
