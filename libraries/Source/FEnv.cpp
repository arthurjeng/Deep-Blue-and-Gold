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
#include "Hash.h"

void FEnv::ResetAux(void)
{
  good = colorWhite; evil = colorBlack; epsq = sqrNil; hmvc = 0; fmvn = 1;
}

void FEnv::Reset(void) {ResetAux(); csab = csabNone;}

void FEnv::SetInitialArray(void) {ResetAux(); csab = csabFull;}

void FEnv::CalcPosHash(Hash& hash) const
{
  hash.Reset();
  for (castType cast = 0; cast < castLen; cast++)
    if (TestCast(cast)) hash.FoldCast(cast);
  if (IsSqrNotNil(epsq)) hash.FoldEpSqr(epsq);
}

void FEnv::CalcPosHashRev(Hash& hash) const
{
  hash.Reset();
  for (castType cast = 0; cast < castLen; cast++)
    if (TestCast(cast)) hash.FoldCast(othercast[cast]);
  if (IsSqrNotNil(epsq)) hash.FoldEpSqr(OtherSqr(epsq));
}

void FEnv::Flip(void)
{
  const FEnv fenv(*this);

  good = fenv.evil; evil = fenv.good;
  csab = 0;
  for (castType cast = 0; cast < castLen; cast++)
    if (fenv.TestCast(cast)) csab |= BX(othercast[cast]);
  if (IsSqrNil(fenv.epsq)) epsq = sqrNil; else epsq = OtherSqr(fenv.epsq);
}

bool FEnv::IsValid(void) const
{
  bool valid = true;

  if (IsSqrNotNil(epsq))
  {
    if ((hmvc != 0) || (MapSqrToRank(epsq) != r6rank[GetGood()])) valid = false;
  };
  if (valid && (fmvn < 1)) valid = false;
  return valid;
}

void FEnv::Print(void) const
{
  PrintColor(GetGood());
  PrintSpace(); PrintCsab(csab); PrintSpace(); PrintSqrMeta(epsq);
  PrintSpace(); PrintUi16(hmvc); PrintSpace(); PrintUi16(fmvn);
}
