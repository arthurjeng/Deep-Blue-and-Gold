// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_State
#define Included_State

class State
{
  public:
    void OneTimeSetup(void);

    bool IsDone(void) const {return isdone;}
    void SetDone(void) {isdone = true;}

    void Interrupt(void);

    ui16 GetLoopCount(void) const {return loopcount;}
    void IncLoopCount(void) {loopcount++;}

    bool TestInterrupt(void) {return interrupt;}
    void ResetInterrupt(void) {interrupt = false;}

    miType GetUnDoCount(void) const {return undostack.GetCount();}

    void SetOption(const optnType optn) {options |= BX(optn); SendOptions();}
    void ResetOption(const optnType optn) {options &= ~BX(optn); SendOptions();}
    bool TestOption(const optnType optn) const {return options & BX(optn);}

    void ResetOptions(void) {options = 0; SendOptions();}

    msType GetLimitMsec(void) const {return limitmsec;}
    void PutLimitMsec(const msType value) {limitmsec = value;}

    plyType GetLimitPly(void) const {return limitply;}
    void PutLimitPly(const plyType value) {limitply = value;}

    const FEnv& FetchSearchFEnv(void) const {return search.FetchSearchFEnv();}
    const FPos& FetchSearchFPos(void) const {return search.FetchSearchFPos();}
    const Hash& FetchPosHash(void) const {return search.FetchPosHash();}

    bool IsOver(void) const {return search.IsOver();}

    bool IsLoseIn1AfterMove(const Move& move) {return search.IsLoseIn1AfterMove(move);}

    ftType CalcFT(void) const {return search.CalcFT();}

    void SetDefaultLimits(void);

    void NewGame(void);

    void Play(const Move& move);
    void Unplay(void);

    const Move& FetchLastMove(void) const {return undostack.FetchTopMove();}

    void Flip(void);

    void PrintFEN(void) const {search.PrintFEN();}
    void PrintGraphic(void) const {search.PrintGraphic();}
    void PrintMoves(void) {search.PrintMoves();}
    void PrintBookMoveList(void) {search.PrintBookMoveList();}

    void MatchMove(const char *str, Move& move) {search.MatchMove(str, move);}

    void LoadStateFromFPos(const FPos& fpos);

    const Move& PVMove(const plyType ply) const {return search.PVMove(ply);}

    void RunMP(const plyType emplimitply, const bool fast) {search.RunMP(emplimitply, fast);}
    void RunABSearch(void) {search.RunABSearch(limitmsec, limitply);}

  private:
    void SendOptions(void) {search.RefOptions() = options;}
    void ResetStacks(void) {history.Reset(); undostack.Reset();}

    bool isdone;
    volatile bool interrupt;
    ui16 loopcount;
    optnmType options;
    msType limitmsec;
    plyType limitply;
    Move movestack[MoveStackLen];
    ML movelist;
    History history;
    Search search;
    UnDoStack undostack;
};

#endif
