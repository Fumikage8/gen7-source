#if GFL_HIO_ENABLE


#include "debug/gfl_Host.h"
//==============================================================================
/**
 * @file	gfl_Host.cpp
 * @brief	
 * @author	kimura_shin
 * @data	2011/06/24, 15:13
 */
// =============================================================================

//s32 gfl::hio::Host::s_Initialized = false;
GFL_SINGLETON_SUBSTANCE(gfl::hio::Host )

//==============================================================================
//==============================================================================
gfl::hio::Host::Host(gfl::heap::HeapBase* heap, gfl::heap::HeapBase* dev_heap)
{
	//	GFL_ASSERT_MSG(IsInitialized(),"Not Initialized HIO.\n Please call gfl::hio::Host::InitializeHio() first.");
#if USE_GFL_NEW
	gfl::hio::HostRead* hrp = GFL_NEW( heap ) gfl::hio::HostRead( heap, dev_heap );
#    else // 
	gfl::hio::HostRead* hrp = new gfl::hio::HostRead( heap );
#    endif
	m_pHostRead = hrp;
#if USE_GFL_NEW
	gfl::hio::HostWrite* hwp = GFL_NEW( heap ) gfl::hio::HostWrite( heap, dev_heap );
#    else // 
	gfl::hio::HostWrite* hwp = new gfl::hio::HostWrite( heap );
#    endif
	m_pHostWrite = hwp;

//	hrp->Start( READ_THREAD_PRIORITY );
//	hwp->Start( WRITE_THREAD_PRIORITY );

	hwp->SetHostRead( hrp );
	hrp->SetHostWrite( hwp );
}

//==============================================================================
//==============================================================================
void gfl::hio::Host::Kill( void )
{
	HostWrite* hw = m_pHostWrite;
	HostRead* hr = m_pHostRead;

//	gfl::core::Debug::PrintConsole( "term!\n");
	if( hw ){
//		while(!hw->SendTerminationToClient( )){}

		hw->SetTerminated( );
		hw->Signal( );
		if( hw->IsConnected( ) ){
			hw->Disconnect( );
			//			hw->FinalizeChannel();
			//		}else{
		}
		//}
	}
	if( hr ){
		hr->SetTerminated( );
		hr->Signal( );
		if( hr->IsConnected( ) ){
			hr->Disconnect( );
			//			while(!hr->IsTerminated()){}
			//			hr->FinalizeChannel();
			//		}else{
		}
	}
}

//==============================================================================
//==============================================================================
bool gfl::hio::Host::IsTerminated()
{
	HostWrite* hw = m_pHostWrite;
	HostRead* hr = m_pHostRead;
	if(hw){
		if(hw->IsAlive()){
			return false;
		}
	}
	if(hr){
		if(hr->IsAlive()){
			return false;
		}
	}
	return true;
}

//==============================================================================
//==============================================================================
gfl::hio::Host::~Host( void )
{
#if USE_GFL_NEW
	GFL_DELETE m_pHostRead;
	GFL_DELETE m_pHostWrite;
#    else // 
	delete m_pHostRead;
	delete m_pHostWrite;
#    endif
	m_pHostRead = 0;
	m_pHostWrite = 0;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
gfl::hio::Host* gfl::hio::Host::Update()
{
	gfl::hio::Host* hio = gfl::hio::Host::GetInstance();
	if( !hio ){
		GFL_ASSERT_MSG(0, "HIO Singleton Class ¶¬‚ÉŽ¸”s‚µ‚Ä‚¢‚Ü‚· (DEBUG_HEAP ‚ª‘¶Ý‚µ‚Ä‚¢‚È‚¢‰Â”\«‚ª‚ ‚è‚Ü‚·)\n");
	}
	hio->UpdateReadWrite();
	return hio;
}

#endif
