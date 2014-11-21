// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_Window
#define Included_Window

// Window class

class Window
{
  public:
    Window(void) {SetFullWidth();}
    Window(const svType alfasv, const svType betasv) {alfa = alfasv; beta = betasv;}

    svType GetAlfa(void) const {return alfa;}
    void PutAlfa(const svType sv) {alfa = sv;}

    svType GetBeta(void) const {return beta;}
    void PutBeta(const svType sv) {beta = sv;}

    void SetFullWidth(void) {alfa = svNegInf; beta = svPosInf;}

    void GoDown(void)
    {
      const svType tmpsv = alfa;

      alfa = ShiftDown(beta); beta = ShiftDown(tmpsv);
    }

    bool IsCutOff(void) const {return alfa >= beta;}

    void Encode(char **bptrptr) const;
    void Print(void) const;

  private:
    svType alfa, beta;
};

#endif
