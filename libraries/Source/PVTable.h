// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_PVTable
#define Included_PVTable

// Predicted variation table length

#define PVTableLen (((MaxPVLen * MaxPVLen) + (MaxPVLen * 3)) / 2)

// Predicted variation table class

class PVTable
{
  public:
    PVTable(void);
    ~PVTable(void) {}

    void Reset(void) {ResetPrior(); ResetTrace();}

    void ResetPrior(void);
    void ResetTrace(void);

    const Move& FetchPVMove(const plyType ply) const {return prior[ply];}
    Move& RefPVMove(const plyType ply) {return prior[ply];}

    Move *GetPVBase(void) {return prior;}

    void ClearAtPly(const plyType ply) {if (ply < MaxPVLen) trace[bases[ply]].MakeVoid();}
    void CopyUpToPly(const plyType frply, const Move& move);

    void CopyToPrior(const svType sv);

    void SetSingleMovePV(const Move& move);

    void PrintRawPV(void) const;
    void PrintPV(const FEnv& fenv) const;
    void PrintPVAndScore(const FEnv& fenv) const;

  private:
    Move prior[MaxPVLenP1]; // This is the result of prior analysis
    Move trace[PVTableLen]; // A triangular semi-matrix with ply indexed PV moves
    miType bases[MaxPVLen]; // Bases used to address ply indexed trace move data
};

#endif
