#pragma once

#include <Windows.h>

namespace Utility {
	/*
		Retrieve the path to a todokanai resource
		Should work fine if the game is stored on other drives, but it's worth noting that
		the max length for the buffer is MAX_PATH as defined in the mdn docs...

		With that said, I'm not super sure how this is gonna perform under different
		environments, so this might be a hotspot for errors on user systems.

		This can be retooled to use GetFullPathNameW if that proves necessary.
	*/
	LPCSTR getResourcePath(LPCSTR filename);
}