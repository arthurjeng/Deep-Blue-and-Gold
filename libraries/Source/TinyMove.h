// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_TinyMove
#define Included_TinyMove

// Tiny move code byte length

#define tmcbLen 2

// Compressed move class

class TinyMove
{
  public:
    TinyMove(void) {Reset();}
    TinyMove(const sqrType frsqr, const sqrType tosqr, const mscType msc)
    {
      Build(frsqr, tosqr, msc);
    }

    ~TinyMove(void) {}

    void Reset(void) {Load(0, 0);}

    ui8 GetCodeByte(const ui8 index) const {return codebyte[index];}

    void Load(const ui8 cb0, const ui8 cb1) {codebyte[0] = cb0; codebyte[1] = cb1;}

    void Flip(void);

    void ConvertFromMove(const Move& move);
    void ConvertToMove(Move& move, const Board& board) const;

    void Print(void) const;

    static si CompareTinyMove(const TinyMove& tinymove0, const TinyMove& tinymove1);

  private:
    void BuildB0(const sqrType frsqr, const sqrType tosqr)
    {
      codebyte[0] = frsqr | ((tosqr & 0x03) << 6);
    }

    void BuildB1(const sqrType tosqr, const mscType msc)
    {
      codebyte[1] = ((tosqr >> 2) & 0x0f) | (msc << 4);
    }

    void Build(const sqrType frsqr, const sqrType tosqr, const mscType msc)
    {
      BuildB0(frsqr, tosqr); BuildB1(tosqr, msc);
    }

    sqrType GetFrSqr(void) const
    {
      return (sqrType) (codebyte[0] & 0x3f);
    }

    sqrType GetToSqr(void) const
    {
      return (sqrType) (((codebyte[0] >> 6) & 0x03) | ((codebyte[1] & 0x0f) << 2));
    }

    mscType GetMsc(void) const
    {
      return (mscType) ((codebyte[1] >> 4) & 0x07);
    }

    ui8 codebyte[tmcbLen];
};

#endif
