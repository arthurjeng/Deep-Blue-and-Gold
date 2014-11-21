// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_Board
#define Included_Board

// Byte length of the man/sweep property table

#define mansweepflagLen (manRLen * dirSLen)

// Forward class declaration(s)

class Hash;

// Board class
//
// A Board object represents the contents of a single chessboard.  It is a simple
// vector of 64 chessmen including vacant square chessmen.  This class is a base class
// for the FPos (FEN Position) class.

class Board
{
  public:
    manType GetMan(const sqrType sqr) const {return sqrtoman[sqr];}

    manType GetMan(const fileType file, const rankType rank) const
    {
      return GetMan(MapFileRankToSqr(file, rank));
    }

    void PrintMPD(void) const;

    void PrintGraphic(const bool normal) const;

    void RedrawBoardDisplay(void) const;

    static ui8 FetchCenterTropismDistance(const sqrType frsqr);
    static sqrType FetchNextSquare(const sqrType frsqr, const dirType dir);

  protected:
    void Reset(void);

    void SetInitialArray(void);

    bool IsValid(void) const;
    bool IsSane(void) const;

    void ResetMan(const sqrType sqr) {PutMan(sqr, manVacant);}
    void ResetMan(const fileType file, const rankType rank) {PutMan(file, rank, manVacant);}

    void PutMan(const sqrType sqr, const manType man) {sqrtoman[sqr] = man;}

    void PutMan(const fileType file, const rankType rank, const manType man)
    {
      PutMan(MapFileRankToSqr(file, rank), man);
    }

    void MoveMan(const sqrType frsqr, const sqrType tosqr)
    {
      sqrtoman[tosqr] = sqrtoman[frsqr]; sqrtoman[frsqr] = manVacant;
    }

    pieceType GetPiece(const sqrType sqr) const {return cvmantopiece[GetMan(sqr)];}

    pieceType GetPiece(const fileType file, const rankType rank) const
    {
      return cvmantopiece[GetMan(file, rank)];
    }

    sqrType LocateKing(const colorType color) const;

    colorType OccupantColor(const sqrType sqr) const {return cvmantocolor[GetMan(sqr)];}

    bool IsOccupantVacant(const sqrType sqr) const {return OccupantColor(sqr) == colorVacant;}

    bool IsClearPath(const sqrType frsqr, const sqrType tosqr, const dirType dir) const
    {
      bool isclear = true;
      const sqrdeltaType del = sqrdels[dir];
      sqrType sqr = frsqr + del;

      while (isclear && (sqr != tosqr))
        if (IsOccupantVacant(sqr)) sqr += del; else isclear = false;
      return isclear;
    }

    bool ColorAttacksSquare(const colorType color, const sqrType tosqr) const;

    ui CountColorAttacksToSquare(const colorType color, const sqrType tosqr) const;

    ui CountAttacksFromSquare(const sqrType sqr) const;

    void CalcPosHash(Hash& hash) const;
    void CalcPosHashRev(Hash& hash) const;

    void Flip(void);

    static si CalcFileDelta(const sqrType frsqr, const sqrType tosqr);
    static si CalcRankDelta(const sqrType frsqr, const sqrType tosqr);

    static ui CalcAbsFileDelta(const sqrType frsqr, const sqrType tosqr);
    static ui CalcAbsRankDelta(const sqrType frsqr, const sqrType tosqr);

    static ui8 FetchManSweepFlag(const manType man, const dirType dir);

    static ui8 FetchPawnAttackCount(const sqrType frsqr);
    static ui8 FetchKnightAttackCount(const sqrType frsqr);
    static ui8 FetchKingAttackCount(const sqrType frsqr);

    static bool IsRimFile(const fileType file);
    static bool IsRimRank(const rankType rank);
    static bool IsRimSqr(const sqrType sqr);

    static sqrType CalcNextSquare(const sqrType frsqr, const dirType dir);

    static dirType CalcDirection(const sqrType frsqr, const sqrType tosqr);

    static bool CalcAdjacent(sqrType frsqr, sqrType tosqr);

    static ddbType CalcDDB(sqrType frsqr, sqrType tosqr);
    static ddbType FetchDDB(const sqrType frsqr, const sqrType tosqr);
    static dirType ExtractDirection(const sqrType frsqr, const sqrType tosqr);

    static ui CalcMinFRSeparation(const sqrType frsqr, const sqrType tosqr);
    static ui CalcSumFRSeparation(const sqrType frsqr, const sqrType tosqr);
    static sdbType CalcSDB(const sqrType frsqr, const sqrType tosqr);
    static ddbType FetchSDB(const sqrType frsqr, const sqrType tosqr);

    static ui MapSdbToMinFR(const sdbType sdb) {return sdb & 0x0f;}
    static ui MapSdbToSumFR(const sdbType sdb) {return (sdb >> 4) & 0x0f;}

  private:
    static const ui8 mansweepflag[mansweepflagLen] PROGMEM;
    static const ui8 centertropismdistance[sqrLen] PROGMEM;
    static const ui8 pawnattackcount[sqrLen] PROGMEM;
    static const ui8 knightattackcount[sqrLen] PROGMEM;
    static const ui8 kingattackcount[sqrLen] PROGMEM;

    static const sqrType sqrdirtosqr[sqrdirLen] PROGMEM;
    static const ddbType sqrsqrtoddb[sqrsqrLen] PROGMEM;
    static const sdbType sqrsqrtosdb[sqrsqrLen] PROGMEM;

    bool IsValidDistribution(void) const;
    bool IsValidPawnLocation(void) const;

    void PrintGraphicSqrContent(const sqrType sqr) const;

    manType sqrtoman[sqrLen]; // The chessmen on the chessboard
};

inline si Board::CalcFileDelta(const sqrType frsqr, const sqrType tosqr)
{
  return MapSqrToFile(tosqr) - MapSqrToFile(frsqr);
}

inline si Board::CalcRankDelta(const sqrType frsqr, const sqrType tosqr)
{
  return MapSqrToRank(tosqr) - MapSqrToRank(frsqr);
}

inline ui Board::CalcAbsFileDelta(const sqrType frsqr, const sqrType tosqr)
{
  const si filedelta = CalcFileDelta(frsqr, tosqr);

  return (filedelta < 0) ? -filedelta : filedelta;
}

inline ui Board::CalcAbsRankDelta(const sqrType frsqr, const sqrType tosqr)
{
  const si rankdelta = CalcRankDelta(frsqr, tosqr);

  return (rankdelta < 0) ? -rankdelta : rankdelta;
}

inline bool Board::IsRimFile(const fileType file)
{
  return (file == fileA) || (file == fileH);
}

inline bool Board::IsRimRank(const rankType rank)
{
  return (rank == rank1) || (rank == rank8);
}

inline bool Board::IsRimSqr(const sqrType sqr)
{
  return IsRimFile(MapSqrToFile(sqr)) || IsRimRank(MapSqrToRank(sqr));
}

inline ui8 Board::FetchManSweepFlag(const manType man, const dirType dir)
{
  return ReadFlashUi8(mansweepflag + (man << 3) + dir);
}

inline ui8 Board::FetchCenterTropismDistance(const sqrType frsqr)
{
  return ReadFlashUi8(centertropismdistance + frsqr);
}

inline ui8 Board::FetchPawnAttackCount(const sqrType frsqr)
{
  return ReadFlashUi8(pawnattackcount + frsqr);
}

inline ui8 Board::FetchKnightAttackCount(const sqrType frsqr)
{
  return ReadFlashUi8(knightattackcount + frsqr);
}

inline ui8 Board::FetchKingAttackCount(const sqrType frsqr)
{
  return ReadFlashUi8(kingattackcount + frsqr);
}

inline sqrType Board::FetchNextSquare(const sqrType frsqr, const dirType dir)
{
  return (sqrType) ReadFlashSi8(sqrdirtosqr + (frsqr << 4) + dir);
}

inline ddbType Board::FetchDDB(const sqrType frsqr, const sqrType tosqr)
{
  return (ddbType) ReadFlashUi8(sqrsqrtoddb + (frsqr << 6) + tosqr);
}

inline dirType Board::ExtractDirection(const sqrType frsqr, const sqrType tosqr)
{
  return ExtractDir(FetchDDB(frsqr, tosqr));
}

inline sdbType Board::FetchSDB(const sqrType frsqr, const sqrType tosqr)
{
  return (sdbType) ReadFlashUi8(sqrsqrtosdb + (frsqr << 6) + tosqr);
}

#endif
