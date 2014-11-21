// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_BB
#define Included_BB

#if (IsDevHost)

// Forward class declaration(s)

class State;

// Book builder class

class BB
{
  public:
    bool MakeBook(State *stateptr, const char *gamefn, const char *codefn);

  private:
};

#endif

#endif
