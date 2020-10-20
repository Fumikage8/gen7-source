#ifndef GFL_MAYA_COMMON_FNVHASH_H_
#define GFL_MAYA_COMMON_FNVHASH_H_

namespace gfl { namespace maya { namespace Common {

	#define			ENABLE_HASH_CMP

	struct FnvHash {

		#define FNV_1_PRIME_32 ((unsigned int)0x01000193)

		unsigned int		value;
		inline FnvHash() { value = 0; };
		explicit inline FnvHash(unsigned int val) : value(val) {};

		inline bool operator==(const FnvHash &val) const
		{
			return val.value == value;
		}
		
		explicit inline FnvHash(unsigned char *str,unsigned int startVal = FNV_1_PRIME_32)
		{
			//ConstHash‚ÆˆÙ‚È‚é’l‚É‚È‚Á‚Ä‚¢‚é‚ªC³‹Ö~I‚à‚¤—ÊY‚ªn‚Ü‚Á‚Ä‚µ‚Ü‚Á‚Ä‚¢‚éB2012/03/20
		#ifdef ENABLE_HASH_CMP
			value = startVal;
			while (*str)
			{
				value *= FNV_1_PRIME_32;
				value^= (unsigned int)*str++;
			}
		#endif
		}

		/*explicit inline FnvHash( const char *str, const unsigned int startVal = 0)
		{//Às‘¬“xd‹‚ÌŠÈˆÕHashFunc
		#ifdef ENABLE_HASH_CMP
			value = startVal;
			while (*str)
			{
				value += (unsigned int)*str++;
			}
		#else
			(void*)str;
			(void)startVal;
		#endif
		}*/
	};

	inline bool operator<( const FnvHash & lval, const FnvHash & rval )
	{
		return (lval.value < rval.value);
	}

	//-------------------------------------------------------------------
	//‚Æ‚è‚ ‚¦‚¸Å‘å‚V‚O•¶š‘«‚è‚È‚­‚È‚Á‚½‚ç‘‚â‚·
	template<const char C0, const char C1='\0', const char C2='\0', const char C3='\0',const char C4='\0', const char C5='\0', const char C6='\0',const char C7='\0', const char C8='\0', const char C9='\0',
		     const char C10='\0', const char C11='\0', const char C12='\0',const char C13='\0', const char C14='\0', const char C15='\0'/*,const char C16='\0', const char C17='\0', const char C18='\0',const char C19='\0',
			 const char C20='\0', const char C21='\0', const char C22='\0',const char C23='\0', const char C24='\0', const char C25='\0',const char C26='\0', const char C27='\0', const char C28='\0',const char C29='\0',
			 const char C30='\0', const char C31='\0', const char C32='\0',const char C33='\0', const char C34='\0', const char C35='\0',const char C36='\0', const char C37='\0', const char C38='\0',const char C39='\0',
			 const char C40='\0', const char C41='\0', const char C42='\0',const char C43='\0', const char C44='\0', const char C45='\0',const char C46='\0', const char C47='\0', const char C48='\0',const char C49='\0',
			 const char C50='\0', const char C51='\0', const char C52='\0',const char C53='\0', const char C54='\0', const char C55='\0',const char C56='\0', const char C57='\0', const char C58='\0',const char C59='\0',
			 const char C60='\0', const char C61='\0', const char C62='\0',const char C63='\0', const char C64='\0', const char C65='\0',const char C66='\0', const char C67='\0', const char C68='\0',const char C69='\0'*/
	>
	struct ConstHash {

		template< char c, unsigned int last_value>
		struct MakeHash	{
			//FnvHash‚ÆˆÙ‚È‚é’l‚É‚È‚Á‚Ä‚¢‚é‚ªC³‹Ö~I‚à‚¤—ÊY‚ªn‚Ü‚Á‚Ä‚µ‚Ü‚Á‚Ä‚¢‚éB2012/03/20
			//static const unsigned int value = (c == 0) ? last_value : (((UInt64)last_value * FNV_1_PRIME_32)&0xFFFFFFFF) ^c;
			static const unsigned int value = (c == 0) ? last_value : (last_value + (unsigned int)c);
		};

		static const unsigned int value = 
#if 0 // heavy for netbeans ...
		MakeHash<C69,
			MakeHash<C68,
				MakeHash<C67,
					MakeHash<C66,
						MakeHash<C65,
							MakeHash<C64,
								MakeHash<C63,
									MakeHash<C62,
										MakeHash<C61,
											MakeHash<C60,
												MakeHash<C59,
													MakeHash<C58,
														MakeHash<C57,
															MakeHash<C56,
																MakeHash<C55,
																	MakeHash<C54,
																		MakeHash<C53,
																			MakeHash<C52,
																				MakeHash<C51,
																					MakeHash<C50,
																						MakeHash<C49,
																							MakeHash<C48,
																								MakeHash<C47,
																									MakeHash<C46,
																										MakeHash<C45,
																											MakeHash<C44,
																												MakeHash<C43,
																													MakeHash<C42,
																														MakeHash<C41,
																															MakeHash<C40,																			
																																MakeHash<C39,
																																	MakeHash<C38,
																																		MakeHash<C37,
																																			MakeHash<C36,
																																				MakeHash<C35,
																																					MakeHash<C34,
																																						MakeHash<C33,
																																							MakeHash<C32,
																																								MakeHash<C31,
																																									MakeHash<C30,
																																										MakeHash<C29,
																																											MakeHash<C28,
																																												MakeHash<C27,
																																													MakeHash<C26,
																																														MakeHash<C25,
																																															MakeHash<C24,
																																																MakeHash<C23,
																																																	MakeHash<C22,
																																																		MakeHash<C21,
																																																			MakeHash<C20,
																																																				MakeHash<C19, 
																																																					MakeHash<C18, 
																																																						MakeHash<C17,
																																																							MakeHash<C16, 
																																																								MakeHash<C15, 
																																																									MakeHash<C14, 
																																																										MakeHash<C13,
																																																											MakeHash<C12, 
																																																												MakeHash<C11, 
																																																													MakeHash<C10, 
																																																														MakeHash<C9,
																																																															MakeHash<C8, 
																																																																MakeHash<C7, 
																																																																	MakeHash<C6, 
																																																																		MakeHash<C5,
																																																																			MakeHash<C4, 
																																																																				MakeHash<C3, 
																																																																					MakeHash<C2, 
																																																																						MakeHash<C1,
																																																																							//MakeHash<C0,FNV_1_PRIME_32>
																																																																							MakeHash<C0,0>
																																																																							::value>//C0
																																																																						::value>//C1
																																																																					::value>//C2
																																																																				::value>//C3
																																																																			::value>//C4
																																																																		::value>//C5
																																																																	::value>//C6
																																																																::value>//C7
																																																															::value>//C8
																																																														::value>//C9
																																																													::value>//C10
																																																												::value>//C11
																																																											::value>//C12
																																																										::value>//C13
																																																									::value>//C14
																																																								::value>//C15
																																																							::value>//C16
																																																						::value>//C17
																																																					::value>//C18
																																																				::value>//C19
																																																			::value>//C20
																																																		::value>//C21
																																																	::value>//C22
																																																::value>//C23
																																															::value>//C24
																																														::value>//C25
																																													::value>//C26
																																												::value>//C27
																																											::value>//C28
																																										::value>//C29
																																									::value>//C30
																																								::value>//C31
																																							::value>//C32
																																						::value>//C33
																																					::value>//C34
																																				::value>//C35
																																			::value>//C36
																																		::value>//C37
																																	::value>//C38
																																::value>//C39
																															::value>//C40
																														::value>//C41
																													::value>//C42
																												::value>//C43
																											::value>//C44
																										::value>//C45
																									::value>//C46
																								::value>//C47
																							::value>//C48
																						::value>//C49
																					::value>//C50
																				::value>//C51
																			::value>//C52
																		::value>//C53
																	::value>//C54
																::value>//C55
															::value>//C56
														::value>//C57
													::value>//C58
												::value>//C59
											::value>//C60
										::value>//C61
									::value>//C62
								::value>//C63
							::value>//C64
						::value>//C65
					::value>//C66
				::value>//C67
			::value>//C68
		::value;//C69
#else
		MakeHash<C15, 
		MakeHash<C14, 
		MakeHash<C13,
		MakeHash<C12, 
		MakeHash<C11, 
		MakeHash<C10, 
		MakeHash<C9,
		MakeHash<C8, 
		MakeHash<C7, 
		MakeHash<C6, 
		MakeHash<C5,
		MakeHash<C4, 
		MakeHash<C3, 
		MakeHash<C2, 
		MakeHash<C1,
		MakeHash<C0,0>
			::value>//C0
			::value>//C1
			::value>//C2
			::value>//C3
			::value>//C4
			::value>//C5
			::value>//C6
			::value>//C7
			::value>//C8
			::value>//C9
			::value>//C10
			::value>//C11
			::value>//C12
			::value>//C13
			::value>//C14
			::value;//C15
#endif
	};

}}} // end of namespace ...




#endif
