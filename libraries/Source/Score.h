// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_Score
#define Included_Score

#define svrMargin 1024

#define svLoseIn0 (svNegInf + svrMargin)
#define svMateIn1 (svPosInf - svrMargin)

#define svSlowMate (svMateIn1 - svrMargin)
#define svSlowLose (svLoseIn0 + svrMargin)

inline bool IsSvBroken(const svType sv) {return sv == svBroken;}
inline bool IsSvPosInf(const svType sv) {return sv == svPosInf;}
inline bool IsSvNegInf(const svType sv) {return sv == svNegInf;}
inline bool IsSvEven(const svType sv)   {return sv == svEven;}

inline bool IsSvMating(const svType sv) {return sv > svSlowMate;}
inline bool IsSvLosing(const svType sv) {return sv < svSlowLose;}

inline bool IsSvInRange(const svType sv) {return (sv >= svSlowLose) && (sv <= svSlowMate);}

inline svType SynthMateInN(const fmvnType n) {return svMateIn1 - n + 1;}
inline svType SynthLoseInN(const fmvnType n) {return svLoseIn0 + n;}

inline fmvnType CalcMateDistance(const svType sv) {return svMateIn1 - sv + 1;}
inline fmvnType CalcLoseDistance(const svType sv) {return sv - svLoseIn0;}

inline svType ShiftUp(const svType sv)
{
  svType result;

  if (IsSvInRange(sv)) result = -sv;
  else
  {
    if (IsSvBroken(sv)) result = sv;
    else
    {
      if (IsSvPosInf(sv)) result = svNegInf;
      else
      {
        if (IsSvNegInf(sv)) result = svPosInf;
        else
        {
          if (IsSvMating(sv)) result = -sv + 1; else result = -sv;
        };
      };
    };
  };
  return result;
}

inline svType ShiftDown(const svType sv)
{
  svType result;

  if (IsSvInRange(sv)) result = -sv;
  else
  {
    if (IsSvBroken(sv)) result = sv;
    else
    {
      if (IsSvPosInf(sv)) result = svNegInf;
      else
      {
        if (IsSvNegInf(sv)) result = svPosInf;
        else
        {
          if (IsSvMating(sv)) result = -sv; else result = -sv + 1;
        };
      };
    };
  };
  return result;
}

// Score class

class Score
{
  public:
    Score(void) {}
    Score(const svType value) {PutSv(value);}
    ~Score(void) {}

    void Reset(void) {PutSv(svEven);}

    svType GetSv(void) const {return sv;}
    void PutSv(const svType value) {sv = value;}

    static void PrintMateAnnouncement(const svType sv);

    static void EncodeSv(char **bptrptr, const svType sv);
    static void PrintSv(const svType sv);

  private:
    svType sv;
};

#endif
