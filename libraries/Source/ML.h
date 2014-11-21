// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_ML
#define Included_ML

class Pos;

// Move list class
//
// The ML class is used to represent a list of moves.

class ML
{
  public:
    ML(void) {baseptr = nextptr = 0; count = limit = 0;}
    ML(const ML& priorml) {LoadFromPrior(priorml);}
    ML(Move *ptr, const miType max) {baseptr = nextptr = ptr; count = 0; limit = max;}
    ~ML(void) {}

    void Preset(Move moves[], const miType length)
    {
      baseptr = moves; nextptr = baseptr; count = 0; limit = length;
    }

    void JamAssign(const ML& ml)
    {
      baseptr = ml.baseptr; nextptr = ml.nextptr;
      count = ml.count; limit = ml.limit;
    }

    void LoadFromPrior(const ML& priorml)
    {
      baseptr = nextptr = priorml.baseptr + priorml.count;
      count = 0; limit = priorml.limit - priorml.count;
    }

    miType GetCount(void) const {return count;}
    void ResetCount(void) {count = 0; nextptr = baseptr;}

    void Push(const Move& move)
    {
      if (count < limit) {*nextptr++ = move; count++;} else DieFS(fsFeOverflowMS);
    }

    const Move& FetchMove(const miType index) const {return baseptr[index];}
    Move& RefMove(const miType index) const {return baseptr[index];}

    void MarkChecksAndCheckmates(Pos &pos) const;
    void MarkDisambiguation(const Pos& pos) const;
    void MarkNotation(Pos& pos) const;

    void SortBySAN(void) const;
    void SortBySv(void) const;

    miType Locate(const Move& move) const;
    miType LocateNoFail(const Move& move) const;

    void MatchMove(const char *str, Move& move) const;

    void ClearSearchFlags(void) const;

    miType FindBestCertainIndex(void) const;

    bool AllScoresCertain(void) const;

    void AssignMaxGain(void) const;
    void AssignLikelyGain(const Pos& pos) const;
    void AssignCenterTropismGain(void) const;

    void ShiftIndexedMoveToFront(const miType index);

    void FilterByFrSqr(const sqrType sqr);
    void FilterByToSqr(const sqrType sqr);
    void FilterByFrMan(const manType man);
    void FilterByToMan(const manType man);
    void FilterByMsc(const mscType msc);

    void FilterByMove(const Move& move);

    void Print(void) const;

  private:
    void RemoveVoidMoves(void);

    Move *baseptr, *nextptr;
    miType count, limit;
};

inline miType ML::Locate(const Move& move) const
{
  // Return the index of the given move if present; else return -1

  miType match = -1, index = 0;

  while ((match < 0) && (index < count))
    if (Move::SameMove(move, FetchMove(index))) match = index; else index++;
  return match;
}

#endif
