// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_SAN
#define Included_SAN

// Standard Algebraic Notation move class

class SAN
{
  public:
    SAN(void) {}
    SAN(const Move& move) {LoadFromMove(move);}
    ~SAN(void) {}

    void LoadFromMove(const Move& move);
    const char *FetchStr(void) const {return cv;}
    void Encode(char **bptrptr) const;
    void Print(void) const;

  private:
    char cv[sanLen];
};

#endif
