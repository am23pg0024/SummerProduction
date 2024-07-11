#pragma once
/**
 *	@file
 *	@brief	デバッグ関連
 */

#include "UnlimitedLib.h"


#if defined(_DEBUG)

#define	TRACE(_format_, ...)						\
		{											\
			char temp[MAX_PATH];					\
			sprintf_s(temp, _format_, __VA_ARGS__);	\
			OutputDebugString(temp);				\
		}

#define	ASSERT(...)	assert(__VA_ARGS__);

#define USE_IMGUI	(1)

#else

#define	TRACE(...)
#define	ASSERT(...)

#define USE_IMGUI	(0)

#endif
