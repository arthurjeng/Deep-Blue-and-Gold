// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_MGS
#define Included_MGS

// Pick state

typedef enum
{
  psNil = -1,
  psBaseInit,
  psBaseNext,
  psEvadInit,
  psEvadPV,
  psEvadNext,
  psFullInit,
  psFullPV,
  psFullGainSeqInit,
  psFullGainSeqMove,
  psFullKiller,
  psFullKillerPM2,
  psFullHoldSeqInit,
  psFullHoldSeqPieceTBV,
  psFullHoldSeqPieceTid,
  psFullHoldSeqPieceMove,
  psGainInit,
  psGainPV,
  psGainNext,
  psTerm
} psType;

#define psLen (psTerm + 1)

// Avoidance flags and masks

typedef enum
{
  mgafNil = -1,
  mgafKiller,
  mgafKillerPM2,
  mgafPV
} mgafType;

typedef ui8 mgafmType;

#define mgafmKiller    BX(mgafKiller)
#define mgafmKillerPM2 BX(mgafKillerPM2)
#define mgafmPV        BX(mgafPV)

// Move generation state class

class MGS
{
  public:
    MGS(void) {Reset();}
    ~MGS(void) {}

    void Reset(void);
    void InitTrack(const abnType abn);

    psType GetPs(void) const {return ps;}
    void PutPs(const psType psvalue) {ps = psvalue;}

    ui8 GetUsedCount(void) const {return usedcount;}
    ui8 NextBaseMoveIndex(void) {return usedcount++;}

    void ResetAvoidPV(void) {mgafm &= ~mgafmPV;}
    void SetAvoidPV(void) {mgafm |= mgafmPV;}
    bool AvoidPV(void) const {return mgafm & mgafmPV;}

    void ResetAvoidKiller(void) {mgafm &= ~mgafmKiller;}
    void SetAvoidKiller(void) {mgafm |= mgafmKiller;}
    bool AvoidKiller(void) const {return mgafm &   mgafmKiller;}

    void ResetAvoidKillerPM2(void) {mgafm &= ~mgafmKillerPM2;}
    void SetAvoidKillerPM2(void) {mgafm |= mgafmKillerPM2;}
    bool AvoidKillerPM2(void) const {return mgafm & mgafmKillerPM2;}

    pieceType GetPiece(void) const {return piece;}
    void PutPiece(const pieceType value) {piece = value;}
    void IncPiece(void) {piece++;}

    TBV& RefGoodTBV(void) {return goodtbv;}

  private:
    psType ps;
    ui8 usedcount;
    pieceType piece;
    mgafmType mgafm;
    TBV goodtbv;
};

inline void MGS::Reset(void)
{
  ps = psNil; usedcount = 0; mgafm = 0; piece = piecePawn; goodtbv.Reset();
}

inline void MGS::InitTrack(const abnType abn)
{
  Reset();
  switch (abn)
  {
    case abnBase: ps = psBaseInit; break;
    case abnEvad: ps = psEvadInit; break;
    case abnFull: ps = psFullInit; break;
    case abnGain: ps = psGainInit; break;
    default: SwitchFault(); break;
  };
}

#endif
