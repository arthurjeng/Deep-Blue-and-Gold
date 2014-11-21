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

#include "FEnv.h"
#include "TBV.h"
#include "MGS.h"
#include "PEnv.h"
#include "PIR.h"

void PIR::Reset(void) {mgs.Reset(); penv.Reset();}
