// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#include "Definitions.h"
#include "Constants.h"
#include "Utilities.h"

#if (IsDevHost)
#include <cassert>
#endif

#include "TBV.h"
#include "PEnv.h"

void PEnv::Reset(void)
{
  pinnedtbv.Reset(); frozentbv.Reset(); checkertbv.Reset();
  checkercount = 0; ischecked = false;
}
