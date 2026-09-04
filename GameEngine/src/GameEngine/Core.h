#pragma once

#ifdef GAMEENGINE_PLATFORM_WINDOWS
	#ifdef GAMEENGINE_BUILD_DLL
		#define GAMEENGINE_API __declspec(dllexport)
	#else
		#define GAMEENGINE_API __declspec(dllimport)
	#endif
#endif

#define BIT(x) (1 << x)