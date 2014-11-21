// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_UnDoStack
#define Included_UnDoStack

// Move undo limit and stack length

#define MaxMoveUndo 8
#define UndoStackLen (MaxPlyLen + MaxMoveUndo)

// Move undo stack class

class UnDoStack
{
  public:
    UnDoStack(void) {Reset();}
    ~UnDoStack(void) {}

    void Reset(void);

    miType GetCount(void) const {return count;}

    void Push(const FEnv& fenv, const Move& move, const tidType capttid);
    void PopDel(void) {count--;}

    const Move& FetchTopMove(void) const {return undostack[count - 1].FetchMove();}
    const FEnv& FetchTopFEnv(void) const {return undostack[count - 1].FetchFEnv();}
    tidType GetTopCaptTid(void) const {return undostack[count - 1].GetCaptTid();}

    const Move& FetchNextMove(void) const {return undostack[count].FetchMove();}

    const Move& FetchPastMove(const ui index) const
    {
      return undostack[count - 1 - index].FetchMove();
    }

  private:
    UnDo undostack[UndoStackLen];
    miType count;
};

#endif
