// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_Myopic
#define Included_Myopic

// Main application class
//
// The Myopic class is a single instanced class that is used to represent the chess
// program application.

class Myopic
{
  public:
    void Init(void);
    void Main(void);
    void Term(void);

    void Interrupt(void);

  private:
    State state;
    ICP icp;
};

#endif
