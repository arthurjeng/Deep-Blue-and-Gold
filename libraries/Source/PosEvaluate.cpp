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
#include "FEnv.h"
#include "FPos.h"
#include "Move.h"
#include "ML.h"
#include "Hash.h"
#include "TBV.h"
#include "PEnv.h"
#include "Pos.h"

svType Pos::EvaluatePawn(void) const
{
  svType pscore = svEven;

  // Basic placement

  pscore += (svType) ReadFlashSi8(pawnplacement + (thiscolor << 6) + thissqr);

  // Multiple penalty

  if (filepawns[thiscolor][MapSqrToFile(thissqr)] > 1) pscore += -(svCP * 15);

  return pscore;
}

svType Pos::EvaluateKnight(void) const
{
  const sdbType sdb = FetchSDB(thissqr, LocateKing(thatcolor));
  svType pscore = svEven;

  // Rim penalty

  if (IsRimSqr(thissqr)) pscore += -(svCP * 12);

  // Center tropism

  pscore += ReadFlashSi8(knightcentertropism + thissqr) / 2;

  // Other color king tropism

  pscore += (MapSdbToSumFR(sdb) - 7) * -(svCP * 4);

  // Mobility

  if (pinnedtbv.TestTid(thistid))
    pscore += -(svCP * 20);
  else
    pscore += (CountAttacksFromSquare(thissqr) - 5) * (svCP * 4);

  return pscore;
}

svType Pos::EvaluateBishop(void) const
{
  svType pscore = svEven;

  // Rim penalty

  if (IsRimSqr(thissqr)) pscore += -(svCP * 10);

  // Mobility

  if (pinnedtbv.TestTid(thistid))
    pscore += -(svCP * 20);
  else
    pscore += (CountAttacksFromSquare(thissqr) - 6) * (svCP * 3);

  return pscore;
}

svType Pos::EvaluateRook(void) const
{
  svType pscore = svEven;

  // Mobility

  if (pinnedtbv.TestTid(thistid))
    pscore += -(svCP * 25);
  else
    pscore += (CountAttacksFromSquare(thissqr) - 7) * (svCP * 2);

  // Open/semi-open file bonus

  const fileType file = MapSqrToFile(thissqr);
  const ui pc0 = filepawns[thiscolor][file], pc1 = filepawns[thatcolor][file];

  if ((pc0 + pc1) == 0) pscore += (svCP * 20);
  else
    if (pc0 == 0) pscore += (svCP * 15);

  return pscore;
}

svType Pos::EvaluateQueen(void) const
{
  svType pscore = svEven;

  // Rim penalty

  if (IsRimSqr(thissqr)) pscore += -(svCP * 8);

  // Mobility

  if (pinnedtbv.TestTid(thistid))
    pscore += -(svCP * 30);
  else
    pscore += (CountAttacksFromSquare(thissqr) - 11) * svCP;

  return pscore;
}

svType Pos::EvaluateKing(void) const
{
  svType pscore = svEven;

  // Center tropism based on other color material

  if (msbc[thatcolor] > (svInitial / 3))
    pscore += (FetchCenterTropismDistance(thissqr) - 128) / 2;
  else
    pscore += (128 - FetchCenterTropismDistance(thissqr)) / 2;

  return pscore;
}

svType Pos::Evaluate(void)
{
  svType pscores[colorRLen];

  // Generate file/color pawn counts

  for (ui colorindex = 0; colorindex < colorRLen; colorindex++)
  {
    for (ui fileindex = 0; fileindex < fileLen; fileindex++)
      filepawns[colorindex][fileindex] = 0;

    TBV pawntbv = tbvbcp[colorindex][piecePawn];
    tidType pawntid;

    while (IsTidNotNil(pawntid = pawntbv.NextTid()))
      filepawns[colorindex][MapSqrToFile(tidtosqr[pawntid])]++;
  };

  // Calculate positional score components for each color

  for (ui colorindex = 0; colorindex < colorRLen; colorindex++)
  {
    // Assign evaluation colors

    thiscolor = (colorType) colorindex; thatcolor = OtherColor(thiscolor);
    pscores[thiscolor] = svEven;

    // Calculate positional score components for one color

    TBV scantbv = tbvbc[thiscolor];

    // Scan through each target ID of the current color

    while (IsTidNotNil(thistid = scantbv.NextTid()))
    {
      svType pscore;

      // Set the evaluation square

      thissqr = tidtosqr[thistid];

      // Process evaluation according to piece kind

      switch (GetPiece(thissqr))
      {
        case piecePawn:   pscore = EvaluatePawn();   break;
        case pieceKnight: pscore = EvaluateKnight(); break;
        case pieceBishop: pscore = EvaluateBishop(); break;
        case pieceRook:   pscore = EvaluateRook();   break;
        case pieceQueen:  pscore = EvaluateQueen();  break;
        case pieceKing:   pscore = EvaluateKing();   break;
        default: pscore = 0; SwitchFault(); break;
      };

      // Accumulate the positional score component for the current color

      pscores[thiscolor] += pscore;
    };
  };

  // Apply the on-the-move bonus

  pscores[GetGood()] += svPawn / 10;

  // Combine the material and the positional score components

  const svType goodscore = msbc[GetGood()] + pscores[GetGood()];
  const svType evilscore = msbc[GetEvil()] + pscores[GetEvil()];

  // Return the evaluation from the good player's point of view

  return goodscore - evilscore;
}
