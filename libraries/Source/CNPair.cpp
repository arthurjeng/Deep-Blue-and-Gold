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
#include "CNPair.h"

CNPair::CNPair(const FEnv& fenv) {good = fenv.GetGood(); fmvn = fenv.GetFmvn();}

void CNPair::Increment(void) {good = OtherColor(good); if (IsColorWhite(good)) fmvn++;}

void CNPair::Print(void) const
{
  PrintUi16(fmvn); if (IsColorBlack(good)) PrintFS(fsMsEllipsis);
}
