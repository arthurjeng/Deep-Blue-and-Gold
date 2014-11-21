// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_Pos
#define Included_Pos

// Formard class declaration(s)

class History;

// General position class

class Pos: public FPos, public PEnv
{
  public:
    void LoadPosFromFPos(const FPos& fpos);

    void SetInitialArray(void);

    ui8 GetManCountByColor(const colorType color) const {return mcbc[color];}
    ui8 GetManCountByMan(const manType man) const {return mcbm[man];}
    ui8 GetManCountByColorPiece(const colorType color, const pieceType piece) const
    {
      return mcbcp[color][piece];
    }

    sqrType GetTidToSqr(const tidType tid) const {return tidtosqr[tid];}

    tidType GetCaptTid(void) const {return capttid;}
    void PutCaptTid(const tidType tid) {capttid = tid;}
    tidType& RefCaptTid(void) {return capttid;}

    const Hash& FetchPosHash(void) const {return pdhash;}

    void Flip(void);

    const TBV& FetchTBVBM(const manType man) const {return tbvbm[man];}

    const TBV& FetchTBVBCP(const colorType color, const pieceType piece) const
    {
      return tbvbcp[color][piece];
    }

    bool GoodAttacksSquare(const sqrType sqr) const
    {
      return ColorAttacksSquare(GetGood(), sqr);
    }

    bool EvilAttacksSquare(const sqrType sqr) const
    {
      return ColorAttacksSquare(GetEvil(), sqr);
    }

    void CalcColorAttacksToSquare(
      const colorType color, const sqrType tosqr, TBV& attackertbv) const;

    svType Evaluate(void);

    void Execute(const Move& move);
    void Execute(const Move& move, FEnv& fenv, tidType& tid, PEnv& penv);

    void Retract(const Move& move, const FEnv& fenv, const tidType savedtid);
    void Retract(const Move& move, const FEnv& fenv, const tidType savedtid, const PEnv& penv);

    void GenAddNonEvasionFromSquare(const genmType genm, const sqrType frsqr, ML& ml) const;
    void GenNonEvasion(const genmType genm, ML& ml) const;
    void GenNonEvasion(ML& ml) const;
    void GenEvasion(ML& ml) const;

    void Gen(ML& ml) const;
    void GenMarked(ML& ml);
    void GenCanonical(ML& ml);

    miType CountMoves(void) const;

    void PrintMoves(const ML& priorml);
    void PrintBookMoveList(const ML& priorml);

    bool NoMoves(void) const;

    bool IsCheckmate(void) const {return  InCheck() && NoMoves();}
    bool IsStalemate(void) const {return !InCheck() && NoMoves();}

    bool IsLikelyDraw(const History& history) const;

    bool IsDraw(const History& history) const {return IsDrawFIR(history) || IsDrawStalemate();}
    bool IsOver(const History& history) const {return IsDrawFIR(history) || NoMoves();}

    ftType CalcFT(const History& history) const;

    bool IsLoseIn1AfterMove(const Move& move, const ML& priorml);

    svType MaterialBalance(void) const {return msbc[GetGood()] - msbc[GetEvil()];}
    svType MaterialTotal(void)   const {return msbc[GetGood()] + msbc[GetEvil()];}

    bool IsEndgame(void) const {return MaterialTotal() < (svInitial / 3);}

    void MatchMove(const char *str, Move& move, const ML& priorml);

    void Regen(void);

    bool IsSane(void) const;

  private:
    void Reset(void);

    svType EvaluatePawn(void) const;
    svType EvaluateKnight(void) const;
    svType EvaluateBishop(void) const;
    svType EvaluateRook(void) const;
    svType EvaluateQueen(void) const;
    svType EvaluateKing(void) const;

    sqrType LocateKing(const colorType color) const {return colortrooptosqr[color][troopKing];}
    sqrType LocateGoodKing(void) const {return LocateKing(GetGood());}
    sqrType LocateEvilKing(void) const {return LocateKing(GetEvil());}

    bool ColorAttacksSquare(const colorType color, const sqrType tosqr) const;

    void AddMan(const sqrType sqr, const manType man, const tidType tid);
    void DelMan(const sqrType sqr);
    void MoveMan(const sqrType frsqr, const sqrType tosqr);

    void GenAddNonEvasionFromSquarePawn(
      const genmType genm, const sqrType frsqr, const manType frman, ML& ml) const;

    void GenAddNonEvasionFromSquareKnight(
      const genmType genm, const sqrType frsqr, const manType frman, ML& ml) const;

    void GenAddNonEvasionFromSquareSweep(
      const genmType genm, const sqrType frsqr, const manType frman, ML& ml) const;

    void GenAddNonEvasionFromSquareKing(
      const genmType genm, const sqrType frsqr, const manType frman, ML& ml) const;

    miType CountMovesNonEvasionFromSquarePawn(const sqrType frsqr) const;
    miType CountMovesNonEvasionFromSquareKnight(const sqrType frsqr) const;
    miType CountMovesNonEvasionFromSquareSweep(const sqrType frsqr) const;
    miType CountMovesNonEvasionFromSquareKing(const sqrType frsqr) const;
    miType CountMovesNonEvasionFromSquare(const sqrType frsqr) const;
    miType CountMovesNonEvasion(void) const;
    miType CountMovesEvasion(void) const;

    bool NoMovesNonEvasionFromSquarePawn(const sqrType frsqr) const;
    bool NoMovesNonEvasionFromSquareKnight(const sqrType frsqr) const;
    bool NoMovesNonEvasionFromSquareSweep(const sqrType frsqr) const;
    bool NoMovesNonEvasionFromSquareKing(const sqrType frsqr) const;
    bool NoMovesNonEvasionFromSquare(const sqrType frsqr) const;
    bool NoMovesNonEvasion(void) const;
    bool NoMovesEvasion(void) const;

    bool HasSufficientWhite(void) const;
    bool HasSufficientBlack(void) const;
    bool HasSufficient(void) const {return HasSufficientWhite() || HasSufficientBlack();}

    bool IsRepeated(const History& history) const;

    bool IsDrawFiftyMoves(void) const {return GetHmvc() >= (Fifty * colorRLen);};
    bool IsDrawInsufficient(void) const {return !HasSufficient();}
    bool IsDrawRepetition(const History& history) const;
    bool IsDrawStalemate(void) const {return IsStalemate();};

    bool IsDrawFIR(const History& history) const
    {
      return IsDrawFiftyMoves() || IsDrawInsufficient() || IsDrawRepetition(history);
    }

    static const si8 pawnplacement[colorsqrLen] PROGMEM;
    static const si8 knightcentertropism[sqrLen] PROGMEM;

    ui8 mcbc[colorRLen]; // Man counts indexed by color

    union
    {
      ui8 mcbm[manRLen]; // Man counts indexed by man
      ui8 mcbcp[colorRLen][pieceRLen]; // And by color/piece
    };

    tidType sqrtotid[sqrLen]; // Map of target IDs indexed by square

    union
    {
      sqrType tidtosqr[tidLen]; // Map of squares indexed by target ID
      sqrType colortrooptosqr[colorRLen][troopLen]; // And by color/troop
    };

    TBV targettbv;        // All men target bit vector
    TBV sweepertbv;       // Sweeper men only target bit vector
    TBV tbvbc[colorRLen]; // Target bit vectors indexed by color

    union
    {
      TBV tbvbm[manRLen]; // Target bit vectors indexed by man
      TBV tbvbcp[colorRLen][pieceRLen]; // And by color/piece
    };

    svType msbc[colorRLen]; // Material scores by color

    Hash pdhash; // Position data hash

    tidType capttid; // Target ID of the last captured man

    colorType thiscolor, thatcolor;    // Evaluation colors
    sqrType thissqr;                   // Evaluation square
    tidType thistid;                   // Evaluation target ID
    ui8 filepawns[colorRLen][fileLen]; // Evaluation pawn counts per file/color
};

#endif
