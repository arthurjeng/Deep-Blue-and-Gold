// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_XCP
#define Included_XCP

#if (IsDevHost)

// Forward class declaration(s)

class State;

// Xboard Command Processor class
//
// Thu XCP class is one of several command processor classes.  A command processor
// class instance provides for the layer between the main program and the single
// State class instance that controls the chess processing.

class XCP
{
  public:
    void SetStatePtr(State *ptr) {stateptr = ptr;}

  private:
    State *stateptr;
    CIB cib;
};

#endif

#endif
