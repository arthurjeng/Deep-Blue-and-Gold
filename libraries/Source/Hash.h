// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_Hash
#define Included_Hash

// Hash code byte length

#define hashLen 8

// Byte lengths of the three hash code tables

#define casthashbyteLen   (castLen * hashLen)
#define epfilehashbyteLen (fileLen * hashLen)
#define mansqrhashbyteLen (manRLen * sqrLen * hashLen)

// Hash code class
//
// The Hash class is used to represent a single hash code.  The class is used mostly
// for position repetition detection.

class Hash
{
  public:
    void Reset(void) {for (ui index = 0; index < hashLen; index++) codebyte[index] = 0;}

    ui8 GetCodeByte(const ui index) const {return codebyte[index];}

    void Fold(const Hash& hash)
    {
      for (ui index = 0; index < hashLen; index++)
        codebyte[index] ^= hash.GetCodeByte(index);
    }

    void FoldCast(const castType cast)
    {
      ui offset = cast * hashLen;

      for (ui index = 0; index < hashLen; index++)
        codebyte[index] ^= ReadFlashUi8(casthashbyte + offset++);
    }

    void FoldEpFile(const fileType file)
    {
      ui offset = file * hashLen;

      for (ui index = 0; index < hashLen; index++)
        codebyte[index] ^= ReadFlashUi8(epfilehashbyte + offset++);
    }

    void FoldEpSqr(const sqrType sqr) {FoldEpFile(MapSqrToFile(sqr));}

    void FoldManSqr(const manType man, const sqrType sqr)
    {
      ui offset = (man * (sqrLen * hashLen)) + (sqr * hashLen);

      for (ui index = 0; index < hashLen; index++)
        codebyte[index] ^= ReadFlashUi8(mansqrhashbyte + offset++);
    }

    void FoldManSqrSqr(const manType man, const sqrType frsqr, const sqrType tosqr)
    {
      ui offset0 = (man * (sqrLen * hashLen)) + (frsqr * hashLen);
      ui offset1 = (man * (sqrLen * hashLen)) + (tosqr * hashLen);

      for (ui index = 0; index < hashLen; index++)
      {
        codebyte[index] ^=
          ReadFlashUi8(mansqrhashbyte + offset0++) ^ ReadFlashUi8(mansqrhashbyte + offset1++);
      };
    }

    void Print(void) const;

    static si CompareHash(const Hash& hash0, const Hash& hash1);
    static bool SameHash(const Hash& hash0, const Hash& hash1);

  private:
    static const ui8 casthashbyte[casthashbyteLen] PROGMEM;
    static const ui8 epfilehashbyte[epfilehashbyteLen] PROGMEM;
    static const ui8 mansqrhashbyte[mansqrhashbyteLen] PROGMEM;

    ui8 codebyte[hashLen];
};

#endif
