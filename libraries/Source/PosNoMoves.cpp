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

bool Pos::NoMovesNonEvasionFromSquarePawn(const sqrType frsqr) const
{
  bool nomoves = true;
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

    // Pawn advancement, including promotion

    if (!pinflag || (bdr == bdrN))
    {
      ss.SetDirection(pawnadvdir[GetGood()]); tosqr = ss.Advance();
      if (IsSqrNotNil(tosqr) && IsOccupantVacant(tosqr)) nomoves = false;
    };

    // Pawn capturing

    for (flankType flank = 0; nomoves && (flank < flankLen); flank++)
    {
      const dirType capdir = pawncapdir[GetGood()][flank];

      if (!pinflag || (cvsweepdirtobdr[capdir] == bdr))
      {
        ss.SetDirection(capdir); tosqr = ss.Advance();
        if (IsSqrNotNil(tosqr))
        {
          if (IsOccupantEvil(tosqr)) nomoves = false;

          // En passant captures

          if ((GetEpsq() == tosqr) && IsEnPassantLegal(frsqr, LocateGoodKing())) nomoves = false;
        };
      };
    };
  };
  return nomoves;
}

bool Pos::NoMovesNonEvasionFromSquareKnight(const sqrType frsqr) const
{
  bool nomoves = true;

  // Frozen men are unmovable

  if (!frozentbv.TestTid(sqrtotid[frsqr]))
  {
    SSKnight ssknight(frsqr);
    sqrType tosqr;

    // Scan through the run square sequence for this knight

    while (nomoves && IsSqrNotNil(tosqr = ssknight.Advance()))
      if (!IsOccupantGood(tosqr)) nomoves = false;
  };
  return nomoves;
}

bool Pos::NoMovesNonEvasionFromSquareSweep(const sqrType frsqr) const
{
  bool nomoves = true;
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

    for (dirType dir = mands0dir[frman]; nomoves && (dir <= stopdir); dir++)
    {
      if (!pinflag || (cvsweepdirtobdr[dir] == bdr))
      {
        sqrType tosqr;
        manType toman;

        ss.SetDirection(dir);
        while (nomoves && IsSqrNotNil(tosqr = ss.Advance()) && IsManVacant(toman = GetMan(tosqr)))
          nomoves = false;
        if (nomoves && IsSqrNotNil(tosqr) && IsManEvil(toman)) nomoves = false;
      };
    };
  };
  return nomoves;
}

bool Pos::NoMovesNonEvasionFromSquareKing(const sqrType frsqr) const
{
  bool nomoves = true;
  SSKing ssking(frsqr);
  sqrType tosqr;

  // Castling

  if (nomoves && IsSomeCastling())
    for (flankType flank = 0; nomoves && (flank < flankLen); flank++)
      if (IsCastlingLegal(cvcolorflanktocast[GetGood()][flank])) nomoves = false;

  // Non castling; scan through the run square sequence for this king

  while (nomoves && IsSqrNotNil(tosqr = ssking.Advance()))
    if ((!IsOccupantGood(tosqr)) && !EvilAttacksSquare(tosqr))
      nomoves = false;
  return nomoves;
}

bool Pos::NoMovesNonEvasionFromSquare(const sqrType frsqr) const
{
  bool nomoves = true;

  switch (GetPiece(frsqr))
  {
    case piecePawn:
      if (!NoMovesNonEvasionFromSquarePawn(frsqr)) nomoves = false;
      break;

    case pieceKnight:
      if (!NoMovesNonEvasionFromSquareKnight(frsqr)) nomoves = false;
      break;

    case pieceBishop:
    case pieceRook:
    case pieceQueen:
      if (!NoMovesNonEvasionFromSquareSweep(frsqr)) nomoves = false;
      break;

    case pieceKing:
      if (!NoMovesNonEvasionFromSquareKing(frsqr)) nomoves = false;
      break;

    default: SwitchFault(); break;
  };
  return nomoves;
}

bool Pos::NoMovesNonEvasion(void) const
{
  TBV scantbv = tbvbc[GetGood()];
  tidType scantid;
  bool nomoves = true;

  while (nomoves && IsTidNotNil(scantid = scantbv.NextTid()))
    if (!NoMovesNonEvasionFromSquare(tidtosqr[scantid])) nomoves = false;
  return nomoves;
}

bool Pos::NoMovesEvasion(void) const
{
  const tidType goodkingtid = cvcolortokingtid[GetGood()];
  const sqrType goodkingsqr = tidtosqr[goodkingtid];
  const manType goodpawnman = GoodMan(piecePawn);
  const bool singlecheck = (checkercount == 1);
  const tidType checkertid = (singlecheck ? checkertbv.FirstTid() : tidNil);
  const sqrType checkersqr = (singlecheck ? tidtosqr[checkertid] : sqrNil);
  const manType checkerman = (singlecheck ? GetMan(checkersqr) : manNil);
  bool nomoves = true;

  // Capture single checker, but not with the king

  if (singlecheck)
  {
    TBV defendertbv;
    tidType defendertid;

    CalcColorAttacksToSquare(GetGood(), checkersqr, defendertbv);
    defendertbv.ResetTid(goodkingtid);
    while (nomoves && IsTidNotNil(defendertid = defendertbv.NextTid()))
      if (!pinnedtbv.TestTid(defendertid)) nomoves = false;
  };

  // Move king; includes captures

  if (nomoves)
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

    for (dirType dir = dirE; nomoves && (dir <= dirSE); dir++)
      if (IsSqrNotNil(tosqrs[dir])) nomoves = false;
  };

  // Interpose against single checking sweeper

  if (nomoves && singlecheck && sweeperflag[checkerman])
  {
    const dirType dir = Board::ExtractDirection(goodkingsqr, checkersqr);
    const sqrdeltaType del = sqrdels[dir];
    sqrType tosqr = goodkingsqr + del;

    while (nomoves && (tosqr != checkersqr))
    {
      // Non-pawn/non-king interpositions

      TBV interposertbv;
      tidType interposertid;

      CalcColorAttacksToSquare(GetGood(), tosqr, interposertbv);
      interposertbv.ResetTid(goodkingtid);
      while (nomoves && IsTidNotNil(interposertid = interposertbv.NextTid()))
      {
        const sqrType interposersqr = tidtosqr[interposertid];
        const manType interposerman = GetMan(interposersqr);

        if ((interposerman != goodpawnman) && !pinnedtbv.TestTid(interposertid)) nomoves = false;
      };

      // Single square advance pawn interpositions

      const sqrType pawn1frsqr = Board::FetchNextSquare(tosqr, pawnadvdir[GetEvil()]);

      if (IsSqrNotNil(pawn1frsqr))
      {
        const manType pawn1man = GetMan(pawn1frsqr);

        if (pawn1man == goodpawnman)
        {
          const tidType pawn1tid = sqrtotid[pawn1frsqr];

          if (!pinnedtbv.TestTid(pawn1tid)) nomoves = false;
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
                if (!pinnedtbv.TestTid(sqrtotid[pawn2frsqr])) nomoves = false;
              };
            };
          };
        };
      };
      tosqr += del;
    };
  };

  // En passant capture

  if (nomoves && IsEpsqNotNil())
  {
    for (flankType flank = 0; nomoves && (flank < flankLen); flank++)
    {
      const sqrType frsqr = Board::FetchNextSquare(GetEpsq(), pawncapdir[GetEvil()][flank]);

      if (IsSqrNotNil(frsqr) && (GetMan(frsqr) == goodpawnman) &&
        IsEnPassantLegal(frsqr, goodkingsqr))
        nomoves = false;
    };
  };

  return nomoves;
}

bool Pos::NoMoves(void) const
{
  return (InCheck() ? NoMovesEvasion() : NoMovesNonEvasion());
}
