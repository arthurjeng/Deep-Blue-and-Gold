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

#include "Board.h"
#include "CNPair.h"
#include "Move.h"
#include "SAN.h"
#include "ML.h"
#include "FEnv.h"
#include "FPos.h"
#include "Hash.h"
#include "TBV.h"
#include "PEnv.h"
#include "Pos.h"

svType Move::MaxGain(void) const
{
  return cvmantosv[GetToMan()] + cvmsctosv[GetMsc()];
}

svType Move::CenterTropismGain(void) const
{
  return
    (si16) Board::FetchCenterTropismDistance(GetFrSqr()) -
    (si16) Board::FetchCenterTropismDistance(GetToSqr());
}

sqrType Move::CalcEPVictimSqr(void) const
{
  sqrType sqr;

  if (tosqr > sqrH4) sqr = tosqr - fileLen; else sqr = tosqr + fileLen;
  return sqr;
}

svType Move::LikelyGain(const Pos& pos) const
{
  // Estimate the likely material gain for the move

  svType likelygain = MaxGain();

  if (pos.EvilAttacksSquare(GetToSqr())) likelygain -= cvmantosv[GetFrMan()];
  return likelygain;
}

void Move::Flip(void)
{
  // Flip the men and the squares of the move

  const Move move(*this);

  PutFrSqr(OtherSqr(move.GetFrSqr()));
  PutToSqr(OtherSqr(move.GetToSqr()));
  PutFrMan(otherman[move.GetFrMan()]);
  PutToMan(otherman[move.GetToMan()]);
}

void Move::Mark(Pos& pos, const ML& priorml)
{
  // Add notation flag markings to a single move

  const bool disam =
    ((manflags[GetFrMan()] & manfmKingOrPawn) == 0) &&
    (pos.GetManCountByMan(GetFrMan()) > 1);

  ML ml(priorml);

  // Phase 1: Process generation according to check status

  if (pos.InCheck()) pos.GenEvasion(ml);
  else
  {
    // Handle non-evasion generation

    if (disam)
    {
      // Might need disambiguation

      TBV tbv = pos.FetchTBVBM(GetFrMan());
      tidType tid;

      while (IsTidNotNil(tid = tbv.NextTid()))
        pos.GenAddNonEvasionFromSquare(genmAll, pos.GetTidToSqr(tid), ml);
    }
    else
    {
      // Won't need disambiguation

      pos.GenAddNonEvasionFromSquare(genmAll, GetFrSqr(), ml);
    };
  };

  // Phase 2: Process file/rank disambiguation as needed

  if (disam)
  {
    ml.FilterByToSqr(GetToSqr()); ml.FilterByFrMan(GetFrMan());
    if (ml.GetCount() > 1) ml.MarkDisambiguation(pos);
  };

  // Phase 3: Process check/checkmate marking

  if (ml.GetCount() > 1) ml.FilterByMove(*this);
  ml.MarkChecksAndCheckmates(pos);

  // Phase 4: Find this move in the generation list and calculate any new marking flags

  const mfmType addflags = ml.FetchMove(ml.LocateNoFail(*this)).GetFlags() & mfmNotation;

  // Phase 5: Apply any flags to this move

  if (addflags != 0) PutFlags(GetFlags() | addflags);
}

void Move::EncodeSAN(char **bptrptr) const
{
  // Encode the SAN representation for the move

  SAN san;

  san.LoadFromMove(*this); san.Encode(bptrptr);
}

void Move::Print(void) const
{
  // Print the SAN representation for the move

  SAN san;

  san.LoadFromMove(*this); san.Print();
}

void Move::Print(const FEnv& fenv) const
{
  // Print the move number and SAN representation for the move

  const CNPair cnpair(fenv);
  SAN san;

  cnpair.Print(); PrintSpace(); san.LoadFromMove(*this); san.Print();
}

void Move::PrintVTMS(const Move *mptr)
{
  // Print a void move terminated move sequence

  while (mptr->IsNotVoid()) {mptr->Print(); mptr++; if (mptr->IsNotVoid()) PrintSpace();};
}

void Move::PrintVTMS(const Move *mptr, const FEnv& startfenv)
{
  // Print a void move terminated move sequence including move numbers

  if (mptr->IsNotVoid())
  {
    CNPair cnpair(startfenv);

    // Special treatment if the first move is by Black

    if (IsColorBlack(cnpair.GetGood()))
    {
      cnpair.Print(); PrintSpace(); mptr->Print(); cnpair.Increment(); mptr++;
      if (mptr->IsNotVoid()) PrintSpace();
    };

    // Handle the remaining moves in the sequence

    while (mptr->IsNotVoid())
    {
      // Only White's moves get a move number prefix

      if (IsColorWhite(cnpair.GetGood())) {cnpair.Print(); PrintSpace();};
      mptr->Print(); cnpair.Increment(); mptr++;

      // Handle possible leading space for the next move

      if (mptr->IsNotVoid()) PrintSpace();
    };
  };
}

bool Move::SameMove(const Move& move0, const Move& move1)
{
  return
    (move0.tosqr == move1.tosqr) && (move0.frsqr == move1.frsqr) &&
    (move0.frman == move1.frman) && (move0.toman == move1.toman) &&
    (move0.msc == move1.msc);
}
