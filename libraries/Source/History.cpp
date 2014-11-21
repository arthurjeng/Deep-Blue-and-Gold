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

#include "Hash.h"
#include "History.h"

void History::Reset(void)
{
  count = 0;
  for (miType index = 0; index < HistoryStackLen; index++) hashstack[index].Reset();
}

bool History::ALOneRep(const Hash& hash, const miType backcount) const
{
  bool onerep = false;
  miType index = count, checked = 0;

  while (!onerep && (index > 0) && (checked < backcount))
  {
    if ((checked & 1) && (Hash::SameHash(hash, hashstack[index - 1]))) onerep = true;
    index--; checked++;
  };
  return onerep;
}

bool History::ALTwoRep(const Hash& hash, const miType backcount) const
{
  bool tworep = false;
  miType index = count, checked = 0, reps = 0;

  while (!tworep && (index > 0) && (checked < backcount))
  {
    if ((checked & 1) && (Hash::SameHash(hash, hashstack[index - 1])))
    {
      reps++; if (reps == 2) tworep = true;
    };
    index--; checked++;
  };
  return tworep;
}
