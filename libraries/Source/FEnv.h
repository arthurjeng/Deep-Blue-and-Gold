// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_FEnv
#define Included_FEnv

// Forward class declaration(s)

class Hash;

// FEN Environment class
//
// The FEnv class is used to provide position environmental scalars for
// a chess position.  It is a base class for the FPos (FEN Position)
// class.

class FEnv
{
  public:
    void Reset(void);

    colorType GetGood(void) const {return good;}
    colorType GetEvil(void) const {return evil;}
    csabType  GetCsab(void) const {return csab;}
    sqrType   GetEpsq(void) const {return epsq;}
    hmvcType  GetHmvc(void) const {return hmvc;}
    fmvnType  GetFmvn(void) const {return fmvn;}

    bool IsWTM(void) const {return IsColorWhite(good);}
    bool IsBTM(void) const {return IsColorWhite(evil);}

    void Print(void) const;

  protected:
    void SetInitialArray(void);

    void SetGood(const colorType color) {good = color; evil = OtherColor(good);}

    void PutCsab(const csabType value) {csab = value;}

    void PutEpsq(const sqrType sqr) {epsq = sqr;}

    void PutHmvc(const hmvcType value) {hmvc = value;}
    void IncHmvc(void) {hmvc++;}

    void PutFmvn(const fmvnType value) {fmvn = value;}
    void IncFmvn(void) {fmvn++;}

    bool TestCast(const castType cast) const {return csab & BX(cast);}

    void ToggleColors(void) {good ^= 1; evil ^= 1;}

    bool IsValid(void) const;

    void Flip(void);

    void CalcPosHash(Hash& hash) const;
    void CalcPosHashRev(Hash& hash) const;

    bool IsSomeCastling(void) const {return GetCsab() != csabNone;}

    bool IsEpsqNotNil(void) const {return GetEpsq() != sqrNil;}

    bool IsColorGood(const colorType color) const {return color == GetGood();}
    bool IsColorEvil(const colorType color) const {return color == GetEvil();}

    bool IsManGood(const manType man) const {return cvmantocolor[man] == GetGood();}
    bool IsManEvil(const manType man) const {return cvmantocolor[man] == GetEvil();}

    manType GoodMan(const pieceType piece) const {return synthman[GetGood()][piece];}
    manType EvilMan(const pieceType piece) const {return synthman[GetEvil()][piece];}

  private:
    void ResetAux(void);

    colorType good; // The color on the move
    colorType evil; // The color not on the move
    csabType  csab; // The castling availaibilty bits
    sqrType   epsq; // The en passant target square
    hmvcType  hmvc; // The half move counter
    fmvnType  fmvn; // The full move number
};

#endif
