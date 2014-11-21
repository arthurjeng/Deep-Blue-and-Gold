// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_Move
#define Included_Move

// Formard class references

class FEnv;
class ML;
class Pos;

// Move class
//
// The Move class is used to represent a chess move.

class Move
{
  public:
    void Reset(void) {ResetAux(); flags = 0;}

    void MakeNull(void) {ResetAux(); flags = mfmNull;}
    void MakeVoid(void) {Reset();}

    sqrType GetFrSqr(void) const {return frsqr;}
    void PutFrSqr(const sqrType value) {frsqr = value;}

    sqrType GetToSqr(void) const {return tosqr;}
    void PutToSqr(const sqrType value) {tosqr = value;}

    manType GetFrMan(void) const {return frman;}
    void PutFrMan(const manType value) {frman = value;}

    manType GetToMan(void) const {return toman;}
    void PutToMan(const manType value) {toman = value;}

    mscType GetMsc(void) const {return msc;}
    void PutMsc(const mscType value) {msc = value;}

    mfmType GetFlags(void) const {return flags;}
    void PutFlags(const mfmType value) {flags = value;}

    svType GetSv(void) const {return sv;}
    void PutSv(const svType value) {sv = value;}

    void ResetFlagM(const mfmType mfm) {flags &= ~mfm;}
    void SetFlagM(const mfmType mfm) {flags |= mfm;}
    bool TestFlagM(const mfmType mfm) const {return flags & mfm;}

    bool IsBook(void)          const {return TestFlagM(mfmBook);}
    bool IsCapture(void)       const {return IsSimpleCapture() || IsEnPassant();}
    bool IsCastling(void)      const {return (msc == mscCKS) || (msc == mscCQS);}
    bool IsCertain(void)       const {return TestFlagM(mfmCert);}
    bool IsCheck(void)         const {return TestFlagM(mfmChck);}
    bool IsDisamFile(void)     const {return TestFlagM(mfmAdaf);}
    bool IsDisamRank(void)     const {return TestFlagM(mfmAdar);}
    bool IsEnPassant(void)     const {return msc == mscEPC;}
    bool IsGainer(void)        const {return IsCapture() || IsPromotion();}
    bool IsHmvcReset(void)     const {return IsPawnMove() || IsCapture();}
    bool IsHolder(void)        const {return !IsGainer();}
    bool IsMate(void)          const {return TestFlagM(mfmMate);}
    bool IsNotCertain(void)    const {return !TestFlagM(mfmCert);}
    bool IsNotNullOrVoid(void) const {return (frsqr != tosqr);}
    bool IsNotVoid(void)       const {return (frsqr != tosqr) || (flags != 0);}
    bool IsNull(void)          const {return TestFlagM(mfmNull);}
    bool IsNullOrVoid(void)    const {return (frsqr == tosqr);}
    bool IsPawnMove(void)      const {return IsManPawn(frman);}
    bool IsPawnTwoStep(void)   const {return IsPawnMove() && IsTwoRankDelta();}
    bool IsPromotion(void)     const {return msc >= mscPPN;}
    bool IsRegular(void)       const {return msc == mscReg;}
    bool IsSearched(void)      const {return TestFlagM(mfmSrch);}
    bool IsSimpleCapture(void) const {return IsManNotVacant(toman);}
    bool IsSpecial(void)       const {return msc != mscReg;}
    bool IsTwoRankDelta(void)  const {return (frsqr ^ tosqr) == (fileLen * 2);}
    bool IsVoid(void)          const {return (frsqr == tosqr) && (flags == 0);}

    void SetCertain(const svType value) {PutSv(value); SetFlagM(mfmCert);}

    svType MaxGain(void) const;
    svType LikelyGain(const Pos& pos) const;
    svType CenterTropismGain(void) const;

    sqrType CalcEPVictimSqr(void) const;

    void Flip(void);

    void Mark(Pos& pos, const ML& priorml);

    void EncodeSAN(char **bptrptr) const;
    void Print(void) const;
    void Print(const FEnv& fenv) const;

    static bool SameMove(const Move& move0, const Move& move1);

    static void PrintVTMS(const Move *mptr);
    static void PrintVTMS(const Move *mptr, const FEnv& startfenv);

  private:
    void ResetAux(void)
    {
      frsqr = tosqr = 0; frman = toman = manVacant; msc = mscReg; sv = svBroken;
    };

    sqrType frsqr, tosqr;
    manType frman, toman;
    mscType msc;
    mfmType flags;
    svType sv;
};

#endif
