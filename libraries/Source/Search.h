// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_Search
#define Included_Search

// Forward class declaration(s)

class Window;

// Search class

class Search
{
  public:
    Search(void) {options = 0;}
    ~Search(void) {}

    void OneTimeSetup(const ML& priorml, History& history, UnDoStack& undostack);

    void Interrupt(void) {interrupt = true;}

    void LoadSearchFromFPos(const FPos& fpos);

    void SetInitialArray(void);

    optnmType& RefOptions(void) {return options;}

    const FEnv& FetchSearchFEnv(void) const {return spos;}
    const FPos& FetchSearchFPos(void) const {return spos;}

    const Hash& FetchPosHash(void) const {return spos.FetchPosHash();}

    void Flip(void) {ResetAux(); spos.Flip();}

    void DoExecute(const Move& move);
    void DoRetract(void);

    void Play(const Move& move);
    void Unplay(void);

    const Move& PVMove(const plyType plyindex) const {return pvtable.FetchPVMove(plyindex);}

    void PrintPVAndScore(void) const;

    bool IsOver(void) const {return spos.IsOver(*historyptr);}

    bool IsLoseIn1AfterMove(const Move& move) {return spos.IsLoseIn1AfterMove(move, rootml);}

    ftType CalcFT(void) const {return spos.CalcFT(*historyptr);}

    void PrintMoves(void) {spos.PrintMoves(rootml);}
    void PrintBookMoveList(void) {spos.PrintBookMoveList(rootml);}

    void MatchMove(const char *str, Move& move);

    void RunABSearch(const ui32 limitmsec, const plyType limitply);
    void RunMP(const plyType limitply, const bool fast);

    void RedrawBoardDisplay(void) const {spos.RedrawBoardDisplay();}

    void PrintFEN(void) const {spos.PrintFEN();}
    void PrintGraphic(void) const {spos.PrintGraphic(!TestOptionM(optnmRB));}

  private:
    void ResetAux(void);

    bool TestOptionM(const optnmType optnm) const {return options & optnm;}

    void UpdateActivityLED(void) const;

    void Indent(void) const;

    void DoExecuteAux(const Move& move);
    void DoRetractAux(void);

    const Move& RootMove(const miType index) const {return rootml.FetchMove(index);}
    Move& RootMove(const miType index) {return rootml.RefMove(index);}

    Move& RefPVMove(const plyType plyindex) {return pvtable.RefPVMove(plyindex);}

    void ClearLocalPV(void) {pvtable.ClearAtPly(ply);}
    void ClearLowerPV(void) {pvtable.ClearAtPly(ply + 1);}
    void CopyUpLocalPV(const Move& move) {pvtable.CopyUpToPly(ply, move);}

    void MarkCV(const ML& priorml);
    void MarkPV(const ML& priorml);

    void PrintCV(void) const;

    void Stop(const stType stvalue) {st = stvalue;}
    bool NotStopped(void) const {return st == stUnterminated;}

    bool InCheck(void)      const {return spos.InCheck();}
    bool IsCheckmate(void)  const {return spos.IsCheckmate();}
    bool IsLikelyDraw(void) const {return spos.IsLikelyDraw(*historyptr);}
    bool IsDraw(void)       const {return spos.IsDraw(*historyptr);}

    miType CountMoves(void) const {return spos.CountMoves();}

    bool CheckInterrupt(void)
    {
      if (interrupt) {Stop(stInterrupt); interrupt = false; abort = true;};
      return abort;
    }

    svType Evaluate(void) {return spos.Evaluate();}

    void GenNonEvasion(const genmType genm, ML& ml) const
    {
      spos.GenNonEvasion(genm, ml);
    }

    void GenEvasion(ML& ml)   const {spos.GenEvasion(ml);}
    void Gen(ML& ml)          const {spos.Gen(ml);}
    void GenMarked(ML& ml)          {spos.GenMarked(ml);}
    void GenCanonical(ML& ml)       {spos.GenCanonical(ml);}

    void ScoreAndSortRootML(void);

    miType Pick(ML& ml);
    miType PickBestUnsearched(const ML& ml);

    void UpdateCheckTime(void) {checkusec = ElapsedMsec();}

    bool IsPastTime(void) const {return ElapsedMsec() >= timeoutmsec;}

    bool IsTriggerAcitivityLEDUpdate(void);
    bool IsTriggerBoardDisplayUpdate(void);
    bool IsTriggerTimeCheck(void) const;

    svType ABNode(const Window& window, const ML& priorml);
    svType ABRootNode(const Window& window, const ML& priorml);

    void ABIteration(const plyType iteration);
    void ABIterationSequence(const plyType limitply);

    void ABSearch(const plyType limitply);

    void MPAuxML(const ML& ml);
    void MPAux(const ML& priorml);

    void Trace0(void);
    void Trace1(void);

    optnmType options;
    plyType ply;
    depthType depth;
    bool abort, isftdown, isprelim, fastmp, actledstate;
    volatile bool interrupt;
    stType st;
    msType startmsec, timeoutmsec, usedmsec, checkusec, actleddisplayusec, boarddisplayusec;
    Counter nodecounter;
    MGS mgs;
    ML rootml;
    Pos spos;
    PIR pirstack[MaxPlyLenP1];
    Move killers[MaxPlyLen];
    PVTable pvtable;
    History *historyptr;
    UnDoStack *undostackptr;
};

#endif
