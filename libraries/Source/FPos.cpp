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
#include "Hash.h"

void FPos::Reset(void) {Board::Reset(); FEnv::Reset();}

bool FPos::LoadFromStr(const char *fen)
{
  // Load the FEN position by parsing a FEN string; return validity status

  bool valid = true;
  char ch = *fen++;

  // Unconditional clearing before a parse-and-load

  Reset();

  // The first field is the chessboard MPD (Man Placement Data)

  if (valid)
  {
    while (ch == ' ') ch = *fen++;
    if (!ch) valid = false;
    else
    {
      rankType rank = rank8;
      fileType file = fileA;

      while (valid && ((ch == '/') || IsCharMan(ch) || IsCharDigit(ch)))
      {
        if (ch == '/')
        {
          if (rank == rank1) valid = false;
          else
          {
            while (file < fileLen) ResetMan(file++, rank);
            file = fileA; rank--; ch = *fen++;
          };
        }
        else
        {
          if (IsCharMan(ch))
          {
            if (file > fileH) valid = false;
            else
            {
              PutMan(file++, rank, MapCharToMan(ch)); ch = *fen++;
            };
          }
          else
          {
            const ui digit = MapCharToDigit(ch);

            if ((digit == 0) || (digit == 9)) valid = false;
            else
            {
              if ((file + digit) > fileLen) valid = false;
              else
              {
                for (ui index = 0; index < digit; index++) ResetMan(file++, rank);
                ch = *fen++;
              };
            };
          };
        };
      };
      if (valid && (rank == rank1))
      {
        while (file < fileLen) ResetMan(file++, rank);
        rank--;
      };
      if (valid && (rank != -1)) valid = false;
      if (valid && (ch != ' ')) valid = false;
    };
  };

  // The second field is the active color

  if (valid)
  {
    while (ch == ' ') ch = *fen++;
    if (!ch) valid = false;
    else
    {
      if (!IsCharColor(ch)) valid = false;
      else
      {
        // For every good there is a hidden evil

        SetGood(MapCharToColor(ch)); ch = *fen++;
      };
      if (valid && (ch != ' ')) valid = false;
    };
  };

  // The third field is the castling availability

  if (valid)
  {
    while (ch == ' ') ch = *fen++;
    if (!ch) valid = false;
    else
    {
      PutCsab(csabNone);
      if (ch == '-') ch = *fen++;
      else
      {
        if (!IsCharCast(ch)) valid = false;
        else
        {
          ui castcount = 0;

          while (valid && IsCharCast(ch))
          {
            if (castcount == castLen) valid = false;
            else
            {
              const castType cast = MapCharToCast(ch);

              if (TestCast(cast)) valid = false;
              else
              {
                PutCsab(GetCsab() | BX(cast)); castcount++; ch = *fen++;
              };
            };
          };
        };
      };
      if (valid && (ch != ' ')) valid = false;
    };
  };

  // The fourth field is the en passant target square

  if (valid)
  {
    while (ch == ' ') ch = *fen++;
    if (!ch) valid = false;
    else
    {
      if (ch == '-') {PutEpsq(sqrNil); ch = *fen++;}
      else
      {
        if (!IsCharFile(ch)) valid = false;
        else
        {
          const fileType file = MapCharToFile(ch);

          ch = *fen++;
          if (!IsCharRank(ch)) valid = false;
          else
          {
            const rankType rank = MapCharToRank(ch);

            PutEpsq(MapFileRankToSqr(file, rank)); ch = *fen++;
          };
        };
      };
      if (valid && (ch != ' ')) valid = false;
    };
  };

  // The fifth field is the half move counter (used for the fifty full move draw rule)

  if (valid)
  {
    while (ch == ' ') ch = *fen++;
    if (!ch) valid = false;
    else
    {
      if (!IsCharDigit(ch)) valid = false;
      else
      {
        PutHmvc(0);
        while (valid && IsCharDigit(ch))
        {
          PutHmvc(GetHmvc() * 10); PutHmvc(GetHmvc() + MapCharToDigit(ch));
          ch = *fen++;
        };
        if (valid && (ch != ' ')) valid = false;
      };
    };
  };

  // The sixth and last field is the full move number

  if (valid)
  {
    while (ch == ' ') ch = *fen++;
    if (!ch) valid = false;
    else
    {
      if (!IsCharDigit(ch)) valid = false;
      else
      {
        PutFmvn(0);
        while (valid && IsCharDigit(ch))
        {
          PutFmvn(GetFmvn() * 10); PutFmvn(GetFmvn() + MapCharToDigit(ch));
          ch = *fen++;
        };
        if (valid && (ch != ' ') && ch) valid = false;
      };
    };
  };

  // Do whatever other checking is possible at this point

  if (valid && !IsValid()) valid = false;

  // Ensure FEN position reset if there was a problem with the string

  if (!valid) Reset();
  return valid;
}

void FPos::SetInitialArray(void)
{
  // Set up the initial chess position

  Board::SetInitialArray(); FEnv::SetInitialArray();
}

void FPos::CalcPosHash(Hash& hash) const
{
  // Calculate the position hash

  Hash boardhash, fenvhash;

  Board::CalcPosHash(boardhash); FEnv::CalcPosHash(fenvhash);
  hash.Reset(); hash.Fold(boardhash); hash.Fold(fenvhash);
}

void FPos::CalcPosHashRev(Hash& hash) const
{
  // Calculate the position hash with color flip

  Hash boardhash, fenvhash;

  Board::CalcPosHashRev(boardhash); FEnv::CalcPosHashRev(fenvhash);
  hash.Reset(); hash.Fold(boardhash); hash.Fold(fenvhash);
}

void FPos::CalcPosHashWTM(Hash& hash) const
{
  // Calculate the position hash based on White To Move

  if (IsWTM()) CalcPosHash(hash); else CalcPosHashRev(hash);
}

void FPos::Flip(void) {Board::Flip(); FEnv::Flip();}

bool FPos::IsValid(void) const
{
  // Even more validity checking starting with the base classes separately

  bool valid = Board::IsValid() && FEnv::IsValid();

  if (valid)
  {
    // Check for castling availability problems

    if (IsSomeCastling())
    {
      castType cast = 0;

      while (valid && (cast < castLen))
      {
        // Check a single castling

        if (TestCast(cast))
        {
          const colorType color = cvcasttocolor[cast];

          // The king and the rook must be on their home squares

          if ((GetMan(kinghomesqr[color]) != synthman[color][pieceKing]) ||
            (GetMan(rookhomesqr[cast]) != synthman[color][pieceRook])) valid = false;
        };
        cast++;
      };
    };

    // Check for en passant target inconsistencies

    if (valid && IsEpsqNotNil())
    {
      // Need an empty target square

      if (IsManNotVacant(GetMan(GetEpsq()))) valid = false;
      else
      {
        // Need an empty enemy pawn predecessor square

        if (IsManNotVacant(GetMan(GetEpsq() - pawnadvdel[GetEvil()]))) valid = false;
        else
        {
          // Need an enemy pawn on the right square

          if (GetMan(GetEpsq() + pawnadvdel[GetEvil()]) != EvilMan(piecePawn)) valid = false;
        };
      };
    };

    // Check for the badness of the passive color king being checked

    if (valid && GoodAttacksSquare(LocateEvilKing())) valid = false;

    // And the active color king can't be checked by more than two attackers

    if (valid && (CountColorAttacksToSquare(GetEvil(), LocateGoodKing()) > 2)) valid = false;
  };
  return valid;
}

bool FPos::IsCastlingLegal(const castType cast) const
{
  // Castling legality checking

  bool cc = TestCast(cast);

  // Need to have vacant squares between the king and rook

  if (cc)
  {
    ui vindex = 0;

    while (cc && (vindex < cstlvacsqLen))
    {
      const sqrType vsqr = castvacsqr[cast][vindex];

      if (IsSqrNil(vsqr) || IsOccupantVacant(vsqr)) vindex++; else cc = false;
    };
  };

  // Can't be in check

  if (cc && EvilAttacksSquare(kinghomesqr[GetGood()])) cc = false;

  // Can't move the king through check or into check

  if (cc)
  {
    ui aindex = 0;

    while (cc && (aindex < cstlatksqLen))
      if (!EvilAttacksSquare(castatksqr[cast][aindex])) aindex++; else cc = false;
  };

  // Return true if castling is legal

  return cc;
}

bool FPos::IsEnPassantLegal(const sqrType frsqr, const sqrType kingsqr) const
{
  // En passant legality checking

  const sqrType vicsqr = GetEpsq() + pawnadvdel[GetEvil()];
  FPos fpos = *this;

  fpos.ResetMan(vicsqr); fpos.MoveMan(frsqr, GetEpsq());
  return !fpos.EvilAttacksSquare(kingsqr);
}

void FPos::PrintFEN(void) const {Board::PrintMPD(); PrintSpace(); FEnv::Print();}

void FPos::PrintGraphic(const bool normal) const
{
  // Simple ASCII position output

  Board::PrintGraphic(normal); PrintChar('['); FEnv::Print(); PrintChar(']'); PrintNL();
}
