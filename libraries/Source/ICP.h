// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_ICP
#define Included_ICP

// Forward class declaration(s)

class Move;
class State;

// Interactive Command Processor class
//
// Thu ICP class is one of several command processor classes.  A command processor
// class instance provides for the layer between the main program and the single
// State class instance that controls the chess processing.

class ICP
{
  public:
    void SetStatePtr(State *ptr) {stateptr = ptr;}

    void Init(void) const;
    void Term(void) const;

    void Loop(void);

    void Interrupt(void);

  private:
    static void RetrieveIcpcString(const icpcType icpc, char cv[]);
    static icpcType DecodeCommand(const char *str);

    void ShowBoard(void) const;
    void ShowFEN(void) const;
    void ShowLimits(void) const;
    void ShowMoves(void) const;
    void ShowOptions(void) const;
    void ShowPosHash(void) const;

    void SingleCycle(void);

    void NoParameters(void) const;

    bool IsGameOver(void) const;

    void ReportGameOver(void) const;

    void Play(const Move& move) const;

    void ProgMove(void) const;
    void UserMove(const Move& move) const;

    void FindMove(Move& move) const;

    void CondShowBoard(void) const;

    void Dispatch(const icpcType icpc) const;

    void DcAT(void) const; // Activate common trace options
    void DcBB(void) const; // Build book
    void DcDB(void) const; // Display board
    void DcDF(void) const; // Display FEN
    void DcDH(void) const; // Display position hash
    void DcDL(void) const; // Display opening library moves
    void DcDM(void) const; // Display moves
    void DcDO(void) const; // Display options
    void DcDS(void) const; // Display status
    void DcEM(void) const; // Enumerate movepaths
    void DcFM(void) const; // Fast enumerate movepaths
    void DcFP(void) const; // Flip position
    void DcGG(void) const; // Go and play to end of game
    void DcGP(void) const; // Go and play
    void DcGS(void) const; // Go search
    void DcHM(void) const; // Help me
    void DcID(void) const; // Show program identification
    void DcNG(void) const; // New game
    void DcPT(void) const; // Program test
    void DcQP(void) const; // Quit program
    void DcRO(void) const; // Reset option(s)
    void DcRP(void) const; // Reset program
    void DcSD(void) const; // Set depth limit
    void DcSF(void) const; // Set FEN
    void DcSO(void) const; // Set option(s)
    void DcST(void) const; // Set time limit
    void DcTB(void) const; // Take back move

    void Test(void) const;

    State *stateptr;
    CIB cib;
};

#endif
