// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_CIB
#define Included_CIB

// Command input buffer class
//
// The CIB class is used to handle command text input by the user.

#define cibLen 100
#define tokensLen 8

class CIB
{
  public:
    CIB(void) {Reset();}
    ~CIB(void) {}

    void Load(void);

    bool IsEOF(void) const {return iseof;}

    ui8 GetTokenCount(void) const {return tokencount;}
    char *GetToken(const ui index) const {return tokens[index];}

  private:
    void ResetBuffer(void);
    void ResetTokens(void);
    void Reset(void);

    bool iseof;
    ui8 tokencount;
    char cv[cibLen];
    char *tokens[tokensLen];
};

#endif
