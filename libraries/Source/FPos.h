// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_FPos
#define Included_FPos

// Forward class declaration(s)

class Hash;

// FEN Position class
//
// The FPos class inherits the Board class and the FEnv class.  It is used to
// represent a chess positon as can be specified by a FEN string.  This class
// serves as a base class for the Pos (general position) class.  It is also used
// to represent a lightweight chess positon in those cases where the more general
// and larger Pos class is unneeded.

class FPos: public Board, public FEnv
{
  public:
    bool LoadFromStr(const char *fen);

    void SetInitialArray(void);

    void CalcPosHashWTM(Hash& hash) const;

    void Flip(void);

    bool IsValid(void) const;

    void PrintFEN(void) const;
    void PrintGraphic(const bool normal) const;

  protected:
    void Reset(void);

    bool GoodAttacksSquare(const sqrType sqr) const
    {
      return ColorAttacksSquare(GetGood(), sqr);
    }

    bool EvilAttacksSquare(const sqrType sqr) const
    {
      return ColorAttacksSquare(GetEvil(), sqr);
    }

    sqrType LocateGoodKing(void) const {return LocateKing(GetGood());}
    sqrType LocateEvilKing(void) const {return LocateKing(GetEvil());}

    bool IsOccupantGood(const sqrType sqr) const {return IsColorGood(OccupantColor(sqr));}
    bool IsOccupantEvil(const sqrType sqr) const {return IsColorEvil(OccupantColor(sqr));}

    bool IsCastlingLegal(const castType cast) const;
    bool IsEnPassantLegal(const sqrType frsqr, const sqrType kingsqr) const;

  private:
    void CalcPosHash(Hash& hash) const;
    void CalcPosHashRev(Hash& hash) const;
};

#endif
