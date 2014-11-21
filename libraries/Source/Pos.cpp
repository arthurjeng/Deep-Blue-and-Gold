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
#include "TinyMove.h"
#include "ML.h"
#include "Hash.h"
#include "BookMove.h"
#include "Book.h"
#include "History.h"
#include "TBV.h"
#include "PEnv.h"
#include "Pos.h"

void Pos::Reset(void)
{
  // Reset the base classes

  FPos::Reset(); PEnv::Reset();

  // Reset the private member variables

  for (manType man = 0; man < manRLen; man++) {mcbm[man] = 0; tbvbm[man].Reset();};
  for (colorType color = 0; color < colorRLen; color++)
  {
    mcbc[color] = 0; msbc[color] = svEven; tbvbc[color].Reset();
  };
  for (sqrType sqr = 0; sqr < sqrLen; sqr++) sqrtotid[sqr] = tidNil;
  for (tidType tid = 0; tid < tidLen; tid++) tidtosqr[tid] = sqrNil;
  targettbv.Reset(); sweepertbv.Reset(); pdhash.Reset(); capttid = tidNil;
}

void Pos::LoadPosFromFPos(const FPos& fpos)
{
  // Load the position from a FEN position

  tidType tids[colorRLen];

  // Reset the position then copy in the FEN environment data

  Reset(); *(FEnv *) this = fpos;

  // Initialize the target ID assignment counter pair

  for (colorType color = 0; color < colorRLen; color++) tids[color] = (color * troopLen) + 1;

  // Scan the FEN position board and load the men

  for (sqrType sqr = 0; sqr < sqrLen; sqr++)
  {
    // For a given square, get the man and its color

    const manType man = fpos.GetMan(sqr);
    const colorType color = cvmantocolor[man];

    // Was a man found?

    if (IsColorNotVacant(color))
    {
      // Add the man to the position; kings get special target ID treatment

      if (IsManKing(man))
        AddMan(sqr, man, cvcolortokingtid[color]);
      else
        AddMan(sqr, man, tids[color]++);
    };
  };

  // Handle the castling hash codes

  for (castType cast = 0; cast < castLen; cast++) if (TestCast(cast)) pdhash.FoldCast(cast);

  // Handle the en passant hash code

  if (IsEpsqNotNil()) pdhash.FoldEpSqr(GetEpsq());

  // Regenerate the checking data and the pinned/frozen target bit vectors

  Regen();
}

void Pos::SetInitialArray(void)
{
  // Set up the initial chess position

  FPos fpos;

  fpos.SetInitialArray(); LoadPosFromFPos(fpos);
}

void Pos::Flip(void)
{
  // Flip the position by exchanging colors and status

  FPos fpos = *this;

  fpos.Flip(); LoadPosFromFPos(fpos);
}

bool Pos::ColorAttacksSquare(const colorType color, const sqrType tosqr) const
{
  // Determine if a color attacks a square

  bool atkflag = false;
  TBV scantbv = tbvbc[color];
  tidType scantid;

  // Scan through each possible attacker; stop early if an attack was found

  while (!atkflag && IsTidNotNil(scantid = scantbv.NextTid()))
  {
    // Get the square from the target ID, then get the direction descriptor byte

    const sqrType frsqr = tidtosqr[scantid];
    const ddbType ddb = FetchDDB(frsqr, tosqr);

    // Is there a direction from the man to the square?

    if (IsDdbNotNil(ddb))
    {
      // Branch according to potential attacker piece kind

      switch (GetPiece(frsqr))
      {
        case piecePawn:
          if (IsDdbAdjacent(ddb) && pawncolordirattack[color][ExtractDir(ddb)]) atkflag = true;
          break;
        case pieceKnight:
          if (IsDdbCrook(ddb)) atkflag = true;
          break;
        case pieceBishop:
          if (IsDdbDiago(ddb) && IsClearPath(frsqr, tosqr, ExtractDir(ddb))) atkflag = true;
          break;
        case pieceRook:
          if (IsDdbOrtho(ddb) && IsClearPath(frsqr, tosqr, ExtractDir(ddb))) atkflag = true;
          break;
        case pieceQueen:
          if (IsDdbSweep(ddb) && IsClearPath(frsqr, tosqr, ExtractDir(ddb))) atkflag = true;
          break;
        case pieceKing:
          if (IsDdbAdjacent(ddb)) atkflag = true;
          break;
        default: SwitchFault(); break;
      };
    };
  };
  return atkflag;
}

void Pos::CalcColorAttacksToSquare(
  const colorType color, const sqrType tosqr, TBV& attackertbv) const
{
  // Produce a target bit vector of all attacks by a color to a square

  TBV scantbv = tbvbc[color];
  tidType scantid;

  attackertbv.Reset();

  // Scan through each possible attacker

  while (IsTidNotNil(scantid = scantbv.NextTid()))
  {
    // Get the square from the target ID, then get the direction descriptor byte

    const sqrType frsqr = tidtosqr[scantid];
    const ddbType ddb = FetchDDB(frsqr, tosqr);

    // Is there a direction from the man to the square?

    if (IsDdbNotNil(ddb))
    {
      // Branch according to potential attacker piece kind

      switch (GetPiece(frsqr))
      {
        case piecePawn:
          if (IsDdbAdjacent(ddb) && pawncolordirattack[color][ExtractDir(ddb)])
            attackertbv.SetTid(scantid);
          break;
        case pieceKnight:
          if (IsDdbCrook(ddb)) attackertbv.SetTid(scantid);
          break;
        case pieceBishop:
          if (IsDdbDiago(ddb) && IsClearPath(frsqr, tosqr, ExtractDir(ddb)))
            attackertbv.SetTid(scantid);
          break;
        case pieceRook:
          if (IsDdbOrtho(ddb) && IsClearPath(frsqr, tosqr, ExtractDir(ddb)))
            attackertbv.SetTid(scantid);
          break;
        case pieceQueen:
          if (IsDdbSweep(ddb) && IsClearPath(frsqr, tosqr, ExtractDir(ddb)))
            attackertbv.SetTid(scantid);
          break;
        case pieceKing:
          if (IsDdbAdjacent(ddb)) attackertbv.SetTid(scantid);
          break;
        default: SwitchFault(); break;
      };
    };
  };
}

void Pos::PrintMoves(const ML& priorml)
{
  // Print the available moves fully marked and in ascending SAN order

  ML ml(priorml);

  GenCanonical(ml); ml.Print();
}

void Pos::PrintBookMoveList(const ML& priorml)
{
  // Print the book moves fully marked and in ascending SAN order with popularity values

  ML ml(priorml);

  GenCanonical(ml);
  if (Book::BookMark(ml, *this))
  {
    const ui popsum = Book::CalcPopularitySum(ml);

    for (miType index = 0; index < ml.GetCount(); index++)
    {
      if (ml.FetchMove(index).IsBook())
      {
        ml.FetchMove(index).Print(*this); PrintSpace();
        PrintChar('('); PrintUi16(ml.FetchMove(index).GetSv());
        PrintChar('/'); PrintUi16(popsum);
        PrintChar(')'); PrintNL();
      };
    };
  };
}

bool Pos::HasSufficientWhite(void) const
{
  // Does White have sufficient checkmating material?

  return
    mcbm[manWhitePawn] || mcbm[manWhiteRook] || mcbm[manWhiteQueen] ||
    ((mcbm[manWhiteKnight] + mcbm[manWhiteBishop]) > 1);
}

bool Pos::HasSufficientBlack(void) const
{
  // Does Black have sufficient checkmating material?

  return
    mcbm[manBlackPawn] || mcbm[manBlackRook] || mcbm[manBlackQueen] ||
    ((mcbm[manBlackKnight] + mcbm[manBlackBishop]) > 1);
}

bool Pos::IsLikelyDraw(const History& history) const
{
  return IsDrawFiftyMoves() || IsDrawInsufficient() || history.ALOneRep(pdhash, GetHmvc());
}

bool Pos::IsRepeated(const History& history) const
{
  return history.ALOneRep(pdhash, GetHmvc());
}

bool Pos::IsDrawRepetition(const History& history) const
{
  return history.ALTwoRep(pdhash, GetHmvc());
}

ftType Pos::CalcFT(const History& history) const
{
  // Determine forced game termination status; one of six possibilities

  ftType ft;

  // Mates have first priority status

  if (NoMoves()) ft = (InCheck() ? ftCheckmate : ftDrawStalemate);
  else
  {
    // Has there been too much dancing without any smootching?

    if (IsDrawFiftyMoves()) ft = ftDrawFiftyMoves;
    else
    {
      // When nearly all of the wood is gone, it's time for the rest to go back in the box

      if (IsDrawInsufficient()) ft = ftDrawInsufficient;
      else
      {
        // Could it be like déjà vu all over again?

        if (IsDrawRepetition(history)) ft = ftDrawRepetition;
        else

          // Keep on fighting

          ft = ftUnterminated;
      };
    };
  };
  return ft;
}

bool Pos::IsLoseIn1AfterMove(const Move& move, const ML& priorml)
{
  // Return true if the side to move can be checkmated after playing the given move

  bool islosing = false;
  FEnv fenv0;
  tidType capttid0;
  PEnv penv0;
  ML ml(priorml);
  miType index = 0;

  Execute(move, fenv0, capttid0, penv0);
  Gen(ml);
  while (!islosing && (index < ml.GetCount()))
  {
    FEnv fenv1;
    tidType capttid1;
    PEnv penv1;

    Execute(ml.FetchMove(index), fenv1, capttid1, penv1);
    if (IsCheckmate()) islosing = true;
    Retract(ml.FetchMove(index), fenv1, capttid1, penv1);
    index++;
  };
  Retract(move, fenv0, capttid0, penv0);
  return islosing;
}

void Pos::Regen(void)
{
  // Collect and record checking data

  CalcColorAttacksToSquare(GetEvil(), LocateGoodKing(), checkertbv);
  checkercount = checkertbv.CardByColor(GetEvil()); ischecked = (checkercount > 0);

  // Regenerate pinned and frozen target bit vectors

  pinnedtbv.Reset(); frozentbv.Reset();
  for (colorType color = 0; color < colorRLen; color++)
  {
    // Calculate the pinned and frozen target bit vector for a color

    const colorType othercolor = OtherColor(color);
    const sqrType kingsqr = LocateKing(color);
    TBV atksweepertbv = sweepertbv;
    tidType atksweepertid;

    // Loop through sweeper men of the other color

    atksweepertbv.OnlyColor(othercolor);
    while (IsTidNotNil(atksweepertid = atksweepertbv.NextTid()))
    {
      // Process possible pins by a given sweeper

      const sqrType atksweepersqr = tidtosqr[atksweepertid];
      const ddbType ddb = Board::FetchDDB(kingsqr, atksweepersqr);

      // Examine the directional descriptor byte for the king/sweeper square pair

      if (IsDdbSweep(ddb))
      {
        // At this point there is a straight line from the king to the other color sweeper

        const dirType dir = ExtractDir(ddb);

        if (Board::FetchManSweepFlag(GetMan(atksweepersqr), dir))
        {
          // And the sweeper attacks along the given line

          const sqrdeltaType del = sqrdels[dir];
          sqrType runsqr = kingsqr + del;
          manType runman;

          // Run a square scan through vacant squares from the king towards the sweeper

          while (IsManVacant(runman = GetMan(runsqr))) runsqr += del;
          if (cvmantocolor[runman] == color)
          {
            // A potentially pinned man has been found; now scan beyond along the same direction

            sqrType extsqr = runsqr + del;

            while (IsOccupantVacant(extsqr)) extsqr += del;
            if (extsqr == atksweepersqr)
            {
              // The scan beyond has encountered the other color sweeper man, so there's a pin

              const tidType runtid = sqrtotid[runsqr];

              pinnedtbv.SetTid(runtid);

              // Check for frozen status as well

              switch (cvmantopiece[runman])
              {
                case piecePawn:
                  if (IsDirOrtho(dir))
                  {
                    if (cvsweepdirtobdr[dir] == bdrE) frozentbv.SetTid(runtid);
                  }
                  else
                  {
                    if (IsColorWhite(color))
                    {
                      if (dir >= dirSW) frozentbv.SetTid(runtid);
                    }
                    else
                    {
                      if (dir <= dirNW) frozentbv.SetTid(runtid);
                    };
                  };
                  break;
                case pieceKnight:
                  frozentbv.SetTid(runtid);
                  break;
                case pieceBishop:
                  if (IsDirOrtho(dir)) frozentbv.SetTid(runtid);
                  break;
                case pieceRook:
                  if (IsDirDiago(dir)) frozentbv.SetTid(runtid);
                  break;
                default:
                  break;
              };
            };
          };
        };
      };
    };
  };
}

void Pos::MatchMove(const char *str, Move& move, const ML& priorml)
{
  // Match a move string; note that only correct SAN move strings can match

  ML ml(priorml);

  GenMarked(ml); ml.MatchMove(str, move);
}

bool Pos::IsSane(void) const
{
  // Simple sanity checking for a position

  bool okay = Board::IsSane();

  // Test mcbc

  if (okay)
  {
    ui8 loc_mcbc[colorRLen];

    for (colorType color = 0; color < colorRLen; color++) loc_mcbc[color] = 0;
    for (sqrType sqr = 0; sqr < sqrLen; sqr++)
    {
      const manType man = GetMan(sqr);
      const colorType color = cvmantocolor[man];

      if (IsColorNotVacant(color)) loc_mcbc[color]++;
    };

    for (colorType color = 0; okay && (color < colorRLen); color++)
      if (loc_mcbc[color] != mcbc[color]) okay = false;
  };

  // Test mcbm

  if (okay)
  {
    ui8 loc_mcbm[manRLen];

    for (manType man = 0; man < manRLen; man++) loc_mcbm[man] = 0;
    for (sqrType sqr = 0; sqr < sqrLen; sqr++)
    {
      const manType man = GetMan(sqr);
      const colorType color = cvmantocolor[man];

      if (IsColorNotVacant(color)) loc_mcbm[man]++;
    };

    for (manType man = 0; okay && (man < manRLen); man++)
      if (loc_mcbm[man] != mcbm[man]) okay = false;
  };

  // Test sqrtotid

  if (okay)
  {
    for (sqrType sqr = 0; okay && (sqr < sqrLen); sqr++)
    {
      const tidType tid = sqrtotid[sqr];
      const manType man = GetMan(sqr);
      const colorType color = cvmantocolor[man];

      if (IsColorVacant(color) && IsTidNotNil(tid)) okay = false;
      else
      {
        if (IsColorNotVacant(color) && IsTidNil(tid)) okay = false;
        else
        {
          if (IsColorNotVacant(color) && (sqr != tidtosqr[tid])) okay = false;
        };
      };
    };
  };

  // Test tidtosqr

  if (okay)
  {
    for (tidType tid = 0; okay && (tid < tidLen); tid++)
    {
      const sqrType sqr = tidtosqr[tid];

      if (IsSqrNotNil(sqr))
      {
        if (tid != sqrtotid[sqr]) okay = false;
      };
    };
  };

  // Test targettbv

  if (okay)
  {
    for (tidType tid = 0; okay && (tid < tidLen); tid++)
    {
      if (targettbv.TestTid(tid))
      {
        const sqrType sqr = tidtosqr[tid];

        if (IsSqrNil(sqr)) okay = false;
      };
    };
  };

  // Test sweepertbv

  if (okay)
  {
    for (tidType tid = 0; okay && (tid < tidLen); tid++)
    {
      if (sweepertbv.TestTid(tid))
      {
        const sqrType sqr = tidtosqr[tid];

        if (IsSqrNil(sqr)) okay = false;
        else
        {
          const manType man = GetMan(sqr);

          if (IsManNil(man)) okay = false;
          else
          {
            if (!sweeperflag[man]) okay = false;
          };
        };
      };
    };
  };

  // Test tbvbc

  if (okay)
  {
    for (colorType color = 0; okay && (color < colorRLen); color++)
    {
      for (tidType tid = 0; okay && (tid < tidLen); tid++)
      {
        if (tbvbc[color].TestTid(tid))
        {
          const sqrType sqr = tidtosqr[tid];

          if (IsSqrNil(sqr)) okay = false;
          else
          {
            const manType man = GetMan(sqr);

            if (IsManNil(man)) okay = false;
            else
            {
              if (cvmantocolor[man] != color) okay = false;
            };
          };
        };
      };
    };
  };

  // Test tbvbm

  if (okay)
  {
    for (manType man = 0; okay && (man < manRLen); man++)
    {
      for (tidType tid = 0; okay && (tid < tidLen); tid++)
      {
        if (tbvbm[man].TestTid(tid))
        {
          const sqrType sqr = tidtosqr[tid];

          if (IsSqrNil(sqr)) okay = false;
          else
          {
            if (GetMan(sqr) != man) okay = false;
          };
        };
      };
    };
  };

  return okay;
}

// Score values for pawn placement; white then black

const si8 Pos::pawnplacement[colorsqrLen] PROGMEM =
{
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0, -20, -20,   0,   0,   0,
   10,  10,  10,  20,  20,  10,  10,  10,
   15,  15,  15,  40,  40,  15,  15,  15,
   30,  30,  40,  60,  60,  40,  30,  30,
   50,  60,  70,  80,  80,  70,  60,  50,
   70,  80,  90, 100, 100,  90,  80,  70,
    0,   0,   0,   0,   0,   0,   0,   0,

    0,   0,   0,   0,   0,   0,   0,   0,
   70,  80,  90, 100, 100,  90,  80,  70,
   50,  60,  70,  80,  80,  70,  60,  50,
   30,  30,  40,  60,  60,  40,  30,  30,
   15,  15,  15,  40,  40,  15,  15,  15,
   10,  10,  10,  20,  20,  10,  10,  10,
    0,   0,   0, -20, -20,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0
};

// Score values for knight center tropism

const si8 Pos::knightcentertropism[sqrLen] PROGMEM =
{
  -24, -16,  -8,   0,   0,  -8, -16, -24,
  -16,  -8,   0,   8,   8,   0,  -8, -16,
   -8,   0,   8,  16,  16,   8,   0,  -8,
    0,   8,  16,  24,  24,  16,   8,   0,
    0,   8,  16,  24,  24,  16,   8,   0,
   -8,   0,   8,  16,  16,   8,   0,  -8,
  -16,  -8,   0,   8,   8,   0,  -8, -16,
  -24, -16,  -8,   0,   0,  -8, -16, -24,
};
