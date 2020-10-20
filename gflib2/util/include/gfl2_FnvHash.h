#ifndef GFLIB2_UTIL_FNVHASH_H_INCLUDED
#define GFLIB2_UTIL_FNVHASH_H_INCLUDED

#include <types/include/gfl2_Typedef.h>

namespace gfl2 { namespace util {

	// RenderKeyに登録するマテリアルのハッシュ値を作るための構造体
  // 値は24bitのハッシュ値となる.
	struct FnvHash24 {

    static const unsigned long OFFSET_BASIS = 2166136261;
		static const unsigned long FNV_1_PRIME_32 = 0x01000193;

		unsigned long		value;
		inline FnvHash24() { value = 0; };
		explicit inline FnvHash24(unsigned long val) : value(val) {};

		inline bool operator==(const FnvHash24 &val) const
		{
			return val.value == value;
		}
		
		inline FnvHash24(
      void* buf,
      unsigned long length,
      unsigned long startVal = OFFSET_BASIS)
		{
			value = startVal;
      unsigned char* bp = (unsigned char*)buf;
      unsigned char* be = bp + length;

			while (bp < be)
			{
				value *= FNV_1_PRIME_32;
				value ^= (unsigned long)*bp++;
			}
      // 32bitのハッシュになっているので, 24bitに変換を掛ける.
      // 参考: http://www.isthe.com/chongo/tech/comp/fnv/
      value = (value >> 24) ^ (value & 0x00FFFFFF);
		}
	};

	inline bool operator<( const FnvHash24 & lval, const FnvHash24 & rval )
	{
		return (lval.value < rval.value);
	}

	struct FnvHash {

		static const u32 FNV_1_PRIME_32 = 0x01000193;

		u32		value;
		inline FnvHash() { value = 0; };
		explicit inline FnvHash(u32 val) : value(val) {};

		inline bool operator==(const FnvHash &val) const
		{
			return val.value == value;
		}

		explicit inline FnvHash(const u8 *pData, u32 size, u32 startVal = FNV_1_PRIME_32)
		{
			value = startVal;
			while (size--)
			{
				value *= FNV_1_PRIME_32;
				value^= (u32)*pData++;
			}
		}
	
		explicit inline FnvHash(const u8 *str)
		{
			u32			startVal = FNV_1_PRIME_32;

			value = startVal;
			while (*str)
			{
				value *= FNV_1_PRIME_32;
				value^= (u32)*str++;
			}
		}
	};

	inline bool operator<( const FnvHash & lval, const FnvHash & rval )
	{
		return (lval.value < rval.value);
	}

	//-------------------------------------------------------------------
	//とりあえず最大７０文字足りなくなったら増やす
	template<const c8 C0, const c8 C1='\0', const c8 C2='\0', const c8 C3='\0',const c8 C4='\0', const c8 C5='\0', const c8 C6='\0',const c8 C7='\0', const c8 C8='\0', const c8 C9='\0',
		   const c8 C10='\0', const c8 C11='\0', const c8 C12='\0',const c8 C13='\0', const c8 C14='\0', const c8 C15='\0',const c8 C16='\0', const c8 C17='\0', const c8 C18='\0',const c8 C19='\0',
			 const c8 C20='\0', const c8 C21='\0', const c8 C22='\0',const c8 C23='\0', const c8 C24='\0', const c8 C25='\0',const c8 C26='\0', const c8 C27='\0', const c8 C28='\0',const c8 C29='\0',
			 const c8 C30='\0', const c8 C31='\0', const c8 C32='\0',const c8 C33='\0', const c8 C34='\0', const c8 C35='\0',const c8 C36='\0', const c8 C37='\0', const c8 C38='\0',const c8 C39='\0',
			 const c8 C40='\0', const c8 C41='\0', const c8 C42='\0',const c8 C43='\0', const c8 C44='\0', const c8 C45='\0',const c8 C46='\0', const c8 C47='\0', const c8 C48='\0',const c8 C49='\0',
			 const c8 C50='\0', const c8 C51='\0', const c8 C52='\0',const c8 C53='\0', const c8 C54='\0', const c8 C55='\0',const c8 C56='\0', const c8 C57='\0', const c8 C58='\0',const c8 C59='\0',
			 const c8 C60='\0', const c8 C61='\0', const c8 C62='\0',const c8 C63='\0', const c8 C64='\0', const c8 C65='\0',const c8 C66='\0', const c8 C67='\0', const c8 C68='\0',const c8 C69='\0'
	>
	struct ConstHash {

		template< c8 c, u32 last_value>
		struct MakeHash	{
			static const u32 value = (c == 0) ? last_value : (((u64)last_value * FnvHash::FNV_1_PRIME_32)&0xFFFFFFFF) ^c;
		};

		static const u32 value = 
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
																																																																							MakeHash<C0, FnvHash::FNV_1_PRIME_32>
																																																																							//MakeHash<C0,0>
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
	};


}}




#endif
