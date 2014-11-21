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
#include "SS.h"
#include "SSKnight.h"
#include "SSKing.h"
#include "FEnv.h"
#include "FPos.h"
#include "Move.h"
#include "ML.h"
#include "Hash.h"
#include "TBV.h"
#include "PEnv.h"
#include "Pos.h"

miType Pos::CountMovesNonEvasionFromSquarePawn(const sqrType frsqr) const
{
  miType count = 0;
  const tidType tid = sqrtotid[frsqr];

  // Frozen men are unmovable

  if (!frozentbv.TestTid(tid))
  {
    // Determine pinned status and restriction bidirection, if any

    const bool pinflag = pinnedtbv.TestTid(tid);
    const bdrType bdr =
      (pinflag ? cvsweepdirtobdr[Board::ExtractDirection(LocateGoodKing(), frsqr)] : bdrNil);

    SS ss(frsqr);
    sqrType tosqr;

    const rankType frrank = MapSqrToRank(frsqr);
    const bool r2flag = (frrank == r2rank[GetGood()]), r7flag = (frrank == r7rank[GetGood()]);

    // Pawn advancement, including promotion

    if (!pinflag || (bdr == bdrN))
    {
      ss.SetDirection(pawnadvdir[GetGood()]); tosqr = ss.Advance();
      if (IsSqrNotNil(tosqr) && IsOccupantVacant(tosqr))
      {
        if (r7flag) count += promLen;
        else
        {
          count++;
          if (r2flag)
          {
            tosqr = ss.Advance(); if (IsOccupantVacant(tosqr)) count++;
          };
        };
      };
    };

    // Pawn capturing

    for (flankType flank = 0; flank < flankLen; flank++)
    {
      const dirType capdir = pawncapdir[GetGood()][flank];

      if (!pinflag || (cvsweepdirtobdr[capdir] == bdr))
      {
        ss.SetDirection(capdir); tosqr = ss.Advance();
        if (IsSqrNotNil(tosqr))
        {
          if (IsOccupantEvil(tosqr))
          {
            if (r7flag) count += promLen; else count++;
          };

          // En passant captures

          if ((GetEpsq() == tosqr) && IsEnPassantLegal(frsqr, LocateGoodKing())) count++;
        };
      };
    };
  };
  return count;
}

miType Pos::CountMovesNonEvasionFromSquareKnight(const sqrType frsqr) const
{
  miType count = 0;

  // Frozen men are unmovable

  if (!frozentbv.TestTid(sqrtotid[frsqr]))
  {
    SSKnight ssknight(frsqr);
    sqrType tosqr;

    // Scan through the run square sequence for this knight

    while (IsSqrNotNil(tosqr = ssknight.Advance()))
      if (!IsOccupantGood(tosqr)) count++;
  };
  return count;
}

miType Pos::CountMovesNonEvasionFromSquareSweep(const sqrType frsqr) const
{
  miType count = 0;
  const tidType tid = sqrtotid[frsqr];

  // Frozen men are unmovable

  if (!frozentbv.TestTid(tid))
  {
    const bool pinflag = pinnedtbv.TestTid(tid);
    const bdrType bdr =
      (pinflag ? cvsweepdirtobdr[Board::ExtractDirection(LocateGoodKing(), frsqr)] : bdrNil);

    const manType frman = GetMan(frsqr);
    const dirType stopdir = mands1dir[frman];
    SS ss(frsqr);

    // Loop through each sweep direction for this sweeper

    for (dirType dir = mands0dir[frman]; dir <= stopdir; dir++)
    {
      if (!pinflag || (cvsweepdirtobdr[dir] == bdr))
      {
        sqrType tosqr;
        manType toman;

        ss.SetDirection(dir);
        while (IsSqrNotNil(tosqr = ss.Advance()) && IsManVacant(toman = GetMan(tosqr))) count++;
        if (IsSqrNotNil(tosqr) && IsManEvil(toman)) count++;
      };
    };
  };
  return count;
}

miType Pos::CountMovesNonEvasionFromSquareKing(const sqrType frsqr) const
{
  miType count = 0;
  SSKing ssking(frsqr);
  sqrType tosqr;

  // Castling

  if (IsSomeCastling())
    for (flankType flank = 0; flank < flankLen; flank++)
      if (IsCastlingLegal(cvcolorflanktocast[GetGood()][flank])) count++;

  // Non castling; scan through the run square sequence for this king

  while (IsSqrNotNil(tosqr = ssking.Advance()))
    if ((!IsOccupantGood(tosqr)) && !EvilAttacksSquare(tosqr)) count++;
  return count;
}

miType Pos::CountMovesNonEvasionFromSquare(const sqrType frsqr) const
{
  miType count;

  switch (GetPiece(frsqr))
  {
    case piecePawn:
      count = CountMovesNonEvasionFromSquarePawn(frsqr);
      break;

    case pieceKnight:
      count = CountMovesNonEvasionFromSquareKnight(frsqr);
      break;

    case pieceBishop:
    case pieceRook:
    case pieceQueen:
      count = CountMovesNonEvasionFromSquareSweep(frsqr);
      break;

    case pieceKing:
      count = CountMovesNonEvasionFromSquareKing(frsqr);
      break;

    default:
      count = 0; SwitchFault();
      break;
  };
  return count;
}

miType Pos::CountMovesNonEvasion(void) const
{
  TBV scantbv = tbvbc[GetGood()];
  tidType scantid;
  miType count = 0;

  while (IsTidNotNil(scantid = scantbv.NextTid()))
    count += CountMovesNonEvasionFromSquare(tidtosqr[scantid]);
  return count;
}

miType Pos::CountMovesEvasion(void) const
{
  const tidType goodkingtid = cvcolortokingtid[GetGood()];
  const sqrType goodkingsqr = tidtosqr[goodkingtid];
  const manType goodpawnman = GoodMan(piecePawn);
  const bool singlecheck = (checkercount == 1);
  const tidType checkertid = (singlecheck ? checkertbv.FirstTid() : tidNil);
  const sqrType checkersqr = (singlecheck ? tidtosqr[checkertid] : sqrNil);
  const manType checkerman = (singlecheck ? GetMan(checkersqr) : manNil);
  miType count = 0;

  // Capture single checker, but not with the king

  if (singlecheck)
  {
    TBV defendertbv;
    tidType defendertid;

    CalcColorAttacksToSquare(GetGood(), checkersqr, defendertbv);
    defendertbv.ResetTid(goodkingtid);
    while (IsTidNotNil(defendertid = defendertbv.NextTid()))
    {
      if (!pinnedtbv.TestTid(defendertid))
      {
        const sqrType defendersqr = tidtosqr[defendertid];
        const manType defenderman = GetMan(defendersqr);

        if ((defenderman == goodpawnman) && (MapSqrToRank(defendersqr) == r7rank[GetGood()]))
          count += promLen;
        else
          count++;
      };
    };
  };

  // Move king; includes captures

  {
    TBV attackertbv(checkertbv);
    sqrType tosqrs[dirSLen];

    // Move king/pass one: (simple destination validation)

    for (dirType dir = dirE; dir <= dirSE; dir++)
    {
      const sqrType tosqr = Board::FetchNextSquare(goodkingsqr, dir);

      if (IsSqrNil(tosqr)) tosqrs[dir] = sqrNil;
      else
        tosqrs[dir] = IsOccupantGood(tosqr) ? sqrNil : tosqr;
    };

    // Move king/pass two: (shadow square deletion)

    tidType attackertid;

    while (IsTidNotNil(attackertid = attackertbv.NextTid()))
    {
      const sqrType attackersqr = tidtosqr[attackertid];

      if (sweeperflag[GetMan(attackersqr)])
        tosqrs[Board::ExtractDirection(attackersqr, goodkingsqr)] = sqrNil;
    };

    // Move king/pass three: (hazard deletion)

    for (dirType dir = dirE; dir <= dirSE; dir++)
    {
      const sqrType tosqr = tosqrs[dir];

      if (IsSqrNotNil(tosqr) && EvilAttacksSquare(tosqr)) tosqrs[dir] = sqrNil;
    };

    // Move king/pass four: (move generation)

    for (dirType dir = dirE; dir <= dirSE; dir++)
      if (IsSqrNotNil(tosqrs[dir])) count++;
  };

  // Interpose against single checking sweeper

  if (singlecheck && sweeperflag[checkerman])
  {
    const dirType dir = Board::ExtractDirection(goodkingsqr, checkersqr);
    const sqrdeltaType del = sqrdels[dir];
    sqrType tosqr = goodkingsqr + del;

    while (tosqr != checkersqr)
    {
      // Non-pawn/non-king interpositions

      TBV interposertbv;
      tidType interposertid;

      CalcColorAttacksToSquare(GetGood(), tosqr, interposertbv);
      interposertbv.ResetTid(goodkingtid);
      while (IsTidNotNil(interposertid = interposertbv.NextTid()))
      {
        const sqrType interposersqr = tidtosqr[interposertid];
        const manType interposerman = GetMan(interposersqr);

        if ((interposerman != goodpawnman) && !pinnedtbv.TestTid(interposertid)) count++;
      };

      // Single square advance pawn interpositions

      const sqrType pawn1frsqr = Board::FetchNextSquare(tosqr, pawnadvdir[GetEvil()]);

      if (IsSqrNotNil(pawn1frsqr))
      {
        const manType pawn1man = GetMan(pawn1frsqr);

        if (pawn1man == goodpawnman)
        {
          const tidType pawn1tid = sqrtotid[pawn1frsqr];

          if (!pinnedtbv.TestTid(pawn1tid))
          {
            if (MapSqrToRank(tosqr) != r8rank[GetGood()]) count++; else count += promLen;
          };
        }
        else
        {
          // Double square advance pawn interpositions

          if (IsManVacant(pawn1man))
          {
            const sqrType pawn2frsqr = Board::FetchNextSquare(pawn1frsqr, pawnadvdir[GetEvil()]);

            if (IsSqrNotNil(pawn2frsqr) && (MapSqrToRank(pawn2frsqr) == r2rank[GetGood()]))
            {
              const manType pawn2man = GetMan(pawn2frsqr);

              if (pawn2man == goodpawnman)
              {
                if (!pinnedtbv.TestTid(sqrtotid[pawn2frsqr])) count++;
              };
            };
          };
        };
      };
      tosqr += del;
    };
  };

  // En passant capture

  if (IsEpsqNotNil())
  {
    for (flankType flank = 0; flank < flankLen; flank++)
    {
      const sqrType frsqr = Board::FetchNextSquare(GetEpsq(), pawncapdir[GetEvil()][flank]);

      if (IsSqrNotNil(frsqr) && (GetMan(frsqr) == goodpawnman) &&
        IsEnPassantLegal(frsqr, goodkingsqr))
        count++;
    };
  };

  return count;
}

miType Pos::CountMoves(void) const
{
  return (InCheck() ? CountMovesEvasion() : CountMovesNonEvasion());
}
