// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_Definitions
#define Included_Definitions

// This program was developed on an Apple Macintosh using the Xcode integrated
// development environment that in turn uses the open source GNU tool chain.  Further
// verification was done under Linux also using the GNU tool chain.  Small system
// testing was done using an ATmega1280 (128 KB Flash, 8 KB SRAM) on an Arduino Mega
// board.  The Java-based Arduino IDE was used for Arduino testing.
//
// Target processors included CPUs from AMD, Atmel, Intel, and Motorola/FreeScale.
// Target system word lengths were 16, 32, and 64 bits, and both big and small endian
// CPUs were employed.
//
// Standard ANSI C++ was used for all source coding.  Compilation of the sources can
// be performed by:
//
//     g++ -o myopic *.cpp
//
// or (better) by:
//
//     g++ -O3 -pipe -Wall -o myopic *.cpp
//
// The program can be localized to any eight bit glyph language by changing the
// appropriate string and character constants in the Constants.cpp source file.
//
// The established chess data interchange standards of SAN (Standard Algebraic Notation,
// for chess moves) and FEN (Forsyth-Edwards Notation, for chess positions) have been
// used for all external chess data representation.
//
// Trivia: All keyboarding was done using a Dvorak layout keyboard.

// ******** Environment

// Target vs development host indicator

#define IsFullOS (defined(__APPLE__) || defined(__linux__) || defined(__CYGWIN__))

#define IsDevHost   IsFullOS
#define IsTarget  (!IsFullOS)

// Integer types, best host size

typedef signed   int si;
typedef unsigned int ui;

// Platform dependent fixed length integral types

#if (IsDevHost)
typedef signed   short int si16;
typedef signed         int si32;
typedef unsigned short int ui16;
typedef unsigned       int ui32;
#endif

#if (IsTarget)
typedef signed         int si16;
typedef signed   long  int si32;
typedef unsigned       int ui16;
typedef unsigned long  int ui32;
#endif

// Character/byte types

typedef signed   char si8;
typedef unsigned char ui8;

// Flash memory storage attributes

#if (IsDevHost)
#define PGM_P char *
#define PROGMEM
#define prog_char char
#endif

// Flash memory reading

inline ui8 ReadFlashUi8(const ui8 *ptr)
{
#if (IsDevHost)
  return *ptr;
#endif

#if (IsTarget)
  return pgm_read_byte_near(ptr);
#endif
}

inline si8 ReadFlashSi8(const si8 *ptr)
{
#if (IsDevHost)
  return *ptr;
#endif

#if (IsTarget)
  return (si8) pgm_read_byte_near((ui8 *) ptr);
#endif
}

// ******** All platform independent code follows

// Useful mask

#define bytemask 0xff

// Binary exponentiation and mask generation

#define BX(n)  (1   << (n))
#define BXL(n) (((ui32) 1) << (n))

#define MX(n)  (BX(n)  - 1)
#define MXL(n) (BXL(n) - 1)

// Useful constants

#define bytebitLen 8
#define byteLen 1
#define wordLen (byteLen * 2)
#define pageLen BX(bytebitLen)

// Time type

typedef ui32 msType; // Milliseconds

// ******** Chess related items used for storage requirement calculations

// Maximum ply depth; higher numbers consume more storage

#define MaxPlyLen   20
#define MaxPlyLenP1 (MaxPlyLen + 1)

// Move stack size (move sized units; a move uses eight bytes)

#define MoveStackLen (MaxPlyLen * 16)

// Predicted variation maximum length; MaxPVLen <= MaxPlyLen

#define MaxPVLen    7
#define MaxPVLenP1 (MaxPVLen + 1)

// ******** Chess related items

// For the fifty move rule

#define Fifty 50

// Default search depth and search time limits

#define DefaultSD MaxPlyLen
#define DefaultST 5

// Move index and node count types

typedef si16 miType;
typedef ui32 ncType;

// Ply and depth types

typedef ui16 plyType;
typedef si16 depthType;

// Half move clock and full move number types

typedef ui16 hmvcType;
typedef ui16 fmvnType;

// Chessboard files (columns; left to right from White's point of view)

typedef si8 fileType;

#define makefile(ordinal) ((fileType) (ordinal))

#define fileA makefile(0) // Queen rook
#define fileB makefile(1) // Queen knight
#define fileC makefile(2) // Queen bishop
#define fileD makefile(3) // Queen
#define fileE makefile(4) // King
#define fileF makefile(5) // King bishop
#define fileG makefile(6) // King knight
#define fileH makefile(7) // King rook

#define fileNil makefile(-1)
#define fileLen (fileH + 1)

inline bool IsFileNil(const fileType file)    {return file <  0;}
inline bool IsFileNotNil(const fileType file) {return file >= 0;}

inline fileType OtherFile(const fileType file) {return file ^ 0x07;}

// Chessboard ranks (rows; nearest to farthest from White's point of view)

typedef si8 rankType;

#define makerank(ordinal) ((rankType) (ordinal))

#define rank1 makerank(0)
#define rank2 makerank(1)
#define rank3 makerank(2)
#define rank4 makerank(3)
#define rank5 makerank(4)
#define rank6 makerank(5)
#define rank7 makerank(6)
#define rank8 makerank(7)

#define rankNil makerank(-1)
#define rankLen (rank8 + 1)

inline bool IsRankNil(const rankType rank)    {return rank <  0;}
inline bool IsRankNotNil(const rankType rank) {return rank >= 0;}

inline rankType OtherRank(const rankType rank) {return rank ^ 0x07;}

// Chessboard squares

typedef si8 sqrType;

#define makesqr(file, rank) ((sqrType) (file + (rank * fileLen)))

#define sqrA1 makesqr(fileA, rank1)
#define sqrB1 makesqr(fileB, rank1)
#define sqrC1 makesqr(fileC, rank1)
#define sqrD1 makesqr(fileD, rank1)
#define sqrE1 makesqr(fileE, rank1)
#define sqrF1 makesqr(fileF, rank1)
#define sqrG1 makesqr(fileG, rank1)
#define sqrH1 makesqr(fileH, rank1)

#define sqrA2 makesqr(fileA, rank2)
#define sqrB2 makesqr(fileB, rank2)
#define sqrC2 makesqr(fileC, rank2)
#define sqrD2 makesqr(fileD, rank2)
#define sqrE2 makesqr(fileE, rank2)
#define sqrF2 makesqr(fileF, rank2)
#define sqrG2 makesqr(fileG, rank2)
#define sqrH2 makesqr(fileH, rank2)

#define sqrA3 makesqr(fileA, rank3)
#define sqrB3 makesqr(fileB, rank3)
#define sqrC3 makesqr(fileC, rank3)
#define sqrD3 makesqr(fileD, rank3)
#define sqrE3 makesqr(fileE, rank3)
#define sqrF3 makesqr(fileF, rank3)
#define sqrG3 makesqr(fileG, rank3)
#define sqrH3 makesqr(fileH, rank3)

#define sqrA4 makesqr(fileA, rank4)
#define sqrB4 makesqr(fileB, rank4)
#define sqrC4 makesqr(fileC, rank4)
#define sqrD4 makesqr(fileD, rank4)
#define sqrE4 makesqr(fileE, rank4)
#define sqrF4 makesqr(fileF, rank4)
#define sqrG4 makesqr(fileG, rank4)
#define sqrH4 makesqr(fileH, rank4)

#define sqrA5 makesqr(fileA, rank5)
#define sqrB5 makesqr(fileB, rank5)
#define sqrC5 makesqr(fileC, rank5)
#define sqrD5 makesqr(fileD, rank5)
#define sqrE5 makesqr(fileE, rank5)
#define sqrF5 makesqr(fileF, rank5)
#define sqrG5 makesqr(fileG, rank5)
#define sqrH5 makesqr(fileH, rank5)

#define sqrA6 makesqr(fileA, rank6)
#define sqrB6 makesqr(fileB, rank6)
#define sqrC6 makesqr(fileC, rank6)
#define sqrD6 makesqr(fileD, rank6)
#define sqrE6 makesqr(fileE, rank6)
#define sqrF6 makesqr(fileF, rank6)
#define sqrG6 makesqr(fileG, rank6)
#define sqrH6 makesqr(fileH, rank6)

#define sqrA7 makesqr(fileA, rank7)
#define sqrB7 makesqr(fileB, rank7)
#define sqrC7 makesqr(fileC, rank7)
#define sqrD7 makesqr(fileD, rank7)
#define sqrE7 makesqr(fileE, rank7)
#define sqrF7 makesqr(fileF, rank7)
#define sqrG7 makesqr(fileG, rank7)
#define sqrH7 makesqr(fileH, rank7)

#define sqrA8 makesqr(fileA, rank8)
#define sqrB8 makesqr(fileB, rank8)
#define sqrC8 makesqr(fileC, rank8)
#define sqrD8 makesqr(fileD, rank8)
#define sqrE8 makesqr(fileE, rank8)
#define sqrF8 makesqr(fileF, rank8)
#define sqrG8 makesqr(fileG, rank8)
#define sqrH8 makesqr(fileH, rank8)

#define sqrNil ((sqrType) (-1))
#define sqrLen (rankLen * fileLen)

inline bool IsSqrNil(const sqrType sqr)    {return sqr <  0;}
inline bool IsSqrNotNil(const sqrType sqr) {return sqr >= 0;}

inline fileType MapSqrToFile(const sqrType sqr) {return sqr & 0x07;}
inline rankType MapSqrToRank(const sqrType sqr) {return sqr >> 3;}

inline sqrType MapFileRankToSqr(const fileType file, const rankType rank)
{
  return (rank << 3) | file;
}

inline sqrType OtherSqr(const sqrType sqr) {return sqr ^ 0x38;}

// Square pairs; used for indexing square/square tables

#define sqrsqrLen (sqrLen * sqrLen)

// Chessboard flanks (left and right from White's point of view)

typedef si8 flankType;

#define makeflank(ordinal) ((flankType) (ordinal))

#define flankQ makeflank(0)
#define flankK makeflank(1)

#define flankNil makeflank(-1)
#define flankLen (flankK + 1)

// Direction indices (orthogonal, diagonal, and knight motion)

typedef si8 dirType;

#define makedir(ordinal) ((dirType) (ordinal))

#define dirE   makedir( 0) // East
#define dirN   makedir( 1) // North
#define dirW   makedir( 2) // West
#define dirS   makedir( 3) // South
#define dirNE  makedir( 4) // Northeast
#define dirNW  makedir( 5) // Northwest
#define dirSW  makedir( 6) // Southwest
#define dirSE  makedir( 7) // Southeast
#define dirENE makedir( 8) // East by northeast
#define dirNNE makedir( 9) // North by northeast
#define dirNNW makedir(10) // North by northwest
#define dirWNW makedir(11) // West by northwest
#define dirWSW makedir(12) // West by southwest
#define dirSSW makedir(13) // South by southwest
#define dirSSE makedir(14) // South by southeast
#define dirESE makedir(15) // East by southeast

#define dirNil  makedir(-1)
#define dirSLen ((dirSE  - dirE)   + 1)
#define dirCLen ((dirESE - dirENE) + 1)
#define dirLen  (dirESE + 1)

inline bool IsDirNil(const dirType dir)    {return dir <  0;}
inline bool IsDirNotNil(const dirType dir) {return dir >= 0;}

inline bool IsDirOrtho(const dirType dir) {return (dir >= dirE)   && (dir <= dirS);  }
inline bool IsDirDiago(const dirType dir) {return (dir >= dirNE)  && (dir <= dirSE); }
inline bool IsDirSweep(const dirType dir) {return (dir >= dirE)   && (dir <= dirSE); }
inline bool IsDirCrook(const dirType dir) {return (dir >= dirENE) && (dir <= dirESE);}

// Indexing of square/direction tables

#define sqrdirLen (sqrLen * dirLen)

// Direction descriptor byte

typedef ui8 ddbType;

inline bool IsDdbNil(const ddbType ddb)    {return ddb == 0;}
inline bool IsDdbNotNil(const ddbType ddb) {return ddb != 0;}

inline bool IsDdbOrtho(const ddbType ddb)    {return ddb & 0x10;}
inline bool IsDdbDiago(const ddbType ddb)    {return ddb & 0x20;}
inline bool IsDdbSweep(const ddbType ddb)    {return ddb & 0x30;}
inline bool IsDdbCrook(const ddbType ddb)    {return ddb & 0x40;}
inline bool IsDdbAdjacent(const ddbType ddb) {return ddb & 0x80;}

inline dirType ExtractDir(const ddbType ddb) {return (dirType) (ddb & 0x0f);}

// Separation descriptor byte

typedef ui8 sdbType;

inline ui ExtractMinFR(const sdbType sdb) {return sdb & 0x0f;}
inline ui ExtractSumFR(const sdbType sdb) {return sdb >> 4;}

// Bidirections (sweep diretions only)

typedef si8 bdrType;

#define makebdr(ordinal) ((bdrType) (ordinal))

#define bdrE  makebdr(0) // East/west
#define bdrN  makebdr(1) // North/south
#define bdrNE makebdr(2) // Northeast/southwest
#define bdrNW makebdr(3) // Northwest/southeast

#define bdrNil makebdr(-1)
#define bdrLen (bdrNW + 1)

// Direction deltas for files

typedef si8 filedeltaType;

#define delfE     1
#define delfN     0
#define delfW   (-1)
#define delfS     0
#define delfNE    1
#define delfNW  (-1)
#define delfSW  (-1)
#define delfSE    1
#define delfENE   2
#define delfNNE   1
#define delfNNW (-1)
#define delfWNW (-2)
#define delfWSW (-2)
#define delfSSW (-1)
#define delfSSE   1
#define delfESE   2

// Direction deltas for ranks

typedef si8 rankdeltaType;

#define delrE     0
#define delrN     1
#define delrW     0
#define delrS   (-1)
#define delrNE    1
#define delrNW    1
#define delrSW  (-1)
#define delrSE  (-1)
#define delrENE   1
#define delrNNE   2
#define delrNNW   2
#define delrWNW   1
#define delrWSW (-1)
#define delrSSW (-2)
#define delrSSE (-2)
#define delrESE (-1)

// Direction deltas for squares

typedef si8 sqrdeltaType;

#define makedel(delf, delr) ((sqrdeltaType) (delf + (delr * fileLen)))

#define delE   makedel(delfE,   delrE)
#define delN   makedel(delfN,   delrN)
#define delW   makedel(delfW,   delrW)
#define delS   makedel(delfS,   delrS)
#define delNE  makedel(delfNE,  delrNE)
#define delNW  makedel(delfNW,  delrNW)
#define delSW  makedel(delfSW,  delrSW)
#define delSE  makedel(delfSE,  delrSE)
#define delENE makedel(delfENE, delrENE)
#define delNNE makedel(delfNNE, delrNNE)
#define delNNW makedel(delfNNW, delrNNW)
#define delWNW makedel(delfWNW, delrWNW)
#define delWSW makedel(delfWSW, delrWSW)
#define delSSW makedel(delfSSW, delrSSW)
#define delSSE makedel(delfSSE, delrSSE)
#define delESE makedel(delfESE, delrESE)

// Chess colors

typedef si8 colorType;

#define makecolor(ordinal) ((colorType) (ordinal))

#define colorWhite  makecolor(0) // First mover
#define colorBlack  makecolor(1) // Second mover
#define colorVacant makecolor(2) // Empty, or neither color
#define colorExtra  makecolor(3) // Power of two length filler

#define colorNil  makecolor(-1)
#define colorRLen (colorBlack  + 1)
#define colorLen  (colorExtra  + 1)

inline bool IsColorNil(const colorType color)    {return color <  0;}
inline bool IsColorNotNil(const colorType color) {return color >= 0;}

inline bool IsColorWhite(const colorType color) {return color == colorWhite;}
inline bool IsColorBlack(const colorType color) {return color == colorBlack;}

inline bool IsColorVacant(const colorType color)    {return color == colorVacant;}
inline bool IsColorNotVacant(const colorType color) {return color != colorVacant;}

inline colorType OtherColor(const colorType color) {return color ^ 1;}

inline colorType CalcSqrColor(const sqrType sqr)
{
  return ((MapSqrToFile(sqr) ^ MapSqrToRank(sqr)) & 1) ^ 1;
}

inline bool IsSqrWhite(const sqrType sqr) {return IsColorWhite(CalcSqrColor(sqr));}
inline bool IsSqrBlack(const sqrType sqr) {return IsColorBlack(CalcSqrColor(sqr));}

#define colorsqrLen (colorRLen * sqrLen)

// Chess pieces without color

typedef si8 pieceType;

#define makePiece(ordinal) ((pieceType) (ordinal))

#define piecePawn   makePiece(0) // The little guy who is hoping for a sex change
#define pieceKnight makePiece(1) // The horsey who can't gallop in a straight line
#define pieceBishop makePiece(2) // The guy with the funny hat
#define pieceRook   makePiece(3) // A castle that moves, what a concept!
#define pieceQueen  makePiece(4) // The power behind the throne
#define pieceKing   makePiece(5) // Spends most of his time on the throne
#define pieceVacant makePiece(6) // A ghost, I suppose
#define pieceExtra  makePiece(7) // Power of two length filler

#define pieceNil  makePiece((-1)
#define pieceRLen (pieceKing  + 1)
#define pieceLen  (pieceExtra + 1)

// Chessmen

typedef si8 manType;

#define makeman(color, piece) ((manType) (piece + (color * pieceRLen)))

#define manWhitePawn   makeman(colorWhite, piecePawn)
#define manWhiteKnight makeman(colorWhite, pieceKnight)
#define manWhiteBishop makeman(colorWhite, pieceBishop)
#define manWhiteRook   makeman(colorWhite, pieceRook)
#define manWhiteQueen  makeman(colorWhite, pieceQueen)
#define manWhiteKing   makeman(colorWhite, pieceKing)
#define manBlackPawn   makeman(colorBlack, piecePawn)
#define manBlackKnight makeman(colorBlack, pieceKnight)
#define manBlackBishop makeman(colorBlack, pieceBishop)
#define manBlackRook   makeman(colorBlack, pieceRook)
#define manBlackQueen  makeman(colorBlack, pieceQueen)
#define manBlackKing   makeman(colorBlack, pieceKing)
#define manVacant      ((manType) (manBlackKing + 1))
#define manExtra       ((manType) (manBlackKing + 2))

#define manNil  ((manType) (-1))
#define manRLen (manBlackKing + 1)
#define manLen  (manExtra     + 1)

inline bool IsManNil(const manType man)    {return man <  0;}
inline bool IsManNotNil(const manType man) {return man >= 0;}

inline bool IsManPawn(const manType man)
{
  return (man == manWhitePawn) || (man == manBlackPawn);
}

inline bool IsManKing(const manType man)
{
  return (man == manWhiteKing) || (man == manBlackKing);
}

inline bool IsManVacant(const manType man)    {return (man == manVacant);}
inline bool IsManNotVacant(const manType man) {return (man != manVacant);}

// Chessmen attribute flags

typedef enum
{
  manfNil = -1,
  manfPawn,
  manfKnight,
  manfBishop,
  manfRook,
  manfQueen,
  manfKing,
  manfWhite,
  manfBlack
} manfType;

// Chessmen attribute masks

typedef ui8 manfmType;

#define makemanfm(bitordinal) ((manfmType) BX(bitordinal))

#define manfmPawn   BX(manfPawn)
#define manfmKnight BX(manfKnight)
#define manfmBishop BX(manfBishop)
#define manfmRook   BX(manfRook)
#define manfmQueen  BX(manfQueen)
#define manfmKing   BX(manfKing)
#define manfmWhite  BX(manfWhite)
#define manfmBlack  BX(manfBlack)

#define manfmOrthoSweeper ((manfmType) (manfmRook | manfmQueen))
#define manfmDiagoSweeper ((manfmType) (manfmBishop | manfmQueen))
#define manfmOrtho        ((manfmType) (manfmOrthoSweeper | manfmKing))
#define manfmDiago        ((manfmType) (manfmDiagoSweeper | manfmKing))
#define manfmKingOrPawn   ((manfmType) (manfmPawn | manfmKing))

// Army indexing (16 men per color; King has troop number zero)

typedef si8 troopType;

#define troopKing 0

#define troopNil (-1)
#define troopLen (fileLen * 2)

// Target ID (two troop sets; 0-15:White, 15-31:Black)

typedef si8 tidType;

#define tidWhiteKing ((tidType) (colorWhite * troopLen))
#define tidBlackKing ((tidType) (colorBlack * troopLen))

#define tidNil ((tidType) (-1))
#define tidLen (troopLen * colorRLen)

inline bool IsTidNil(const tidType tid)    {return tid <  0;}
inline bool IsTidNotNil(const tidType tid) {return tid >= 0;}

// Chess castlings

typedef si8 castType;

#define makecast(color, flank) ((castType) ((color * flankLen) + flank))

#define castWQ makecast(colorWhite, flankQ)
#define castWK makecast(colorWhite, flankK)
#define castBQ makecast(colorBlack, flankQ)
#define castBK makecast(colorBlack, flankK)

#define castNil ((castType) (-1))
#define castLen (colorRLen * flankLen)

inline bool IsCastNil(const castType cast)    {return cast <  0;}
inline bool IsCastNotNil(const castType cast) {return cast >= 0;}

// Chess castling status availability bits

typedef ui8 csabType;

#define csabWQ BX(castWQ)
#define csabWK BX(castWK)
#define csabBQ BX(castBQ)
#define csabBK BX(castBK)

#define csabWhite (csabWQ | csabWK)
#define csabBlack (csabBQ | csabBK)

#define csabNone 0
#define csabFull (csabWhite | csabBlack)

// Chess castling vacancy and attack square scan limits

#define cstlvacsqLen 3
#define cstlatksqLen 2

// Chess move flags bit indices

typedef enum
{
  mfNil = -1,
  mfAdaf, // Needs file letter disambiguation
  mfAdar, // Needs rank digit disambiguation
  mfBook, // Seen in the opening library
  mfCert, // Associated score is known as being certain
  mfChck, // Checking move
  mfMate, // Checkmating move
  mfNull, // Null move
  mfSrch  // Move has been searched
} mfType;

#define mfLen (mfSrch + 1)

// Chess move flag masks

typedef ui8 mfmType;

#define mfmAdaf BX(mfAdaf)
#define mfmAdar BX(mfAdar)
#define mfmBook BX(mfBook)
#define mfmCert BX(mfCert)
#define mfmChck BX(mfChck)
#define mfmMate BX(mfMate)
#define mfmNull BX(mfNull)
#define mfmSrch BX(mfSrch)

#define mfmNotation (mfmAdaf | mfmAdar | mfmChck | mfmMate)

// Chess move special cases

typedef si8 mscType;

#define mscReg 0 // Regular move including ordinary capture moves
#define mscEPC 1 // En passant capture move
#define mscCQS 2 // Castling queenside
#define mscCKS 3 // Castling kingside
#define mscPPN 4 // Pawn promotes to a knight
#define mscPPB 5 // Pawn promotes to a bishop
#define mscPPR 6 // Pawn promotes to a rook
#define mscPPQ 7 // Pawn promotes to a queen

#define mscNil (-1)
#define mscLen (mscPPQ + 1)

inline bool IsMscProm(const mscType msc) {return (msc >= mscPPN) && (msc <= mscPPQ);}

// Pawn promotions (move generation order)

typedef enum
{
  promNil = -1,
  promQueen,  // Pawn promotes to a queen
  promRook,   // Pawn promotes to a rook
  promBishop, // Pawn promotes to a bishop
  promKnight  // Pawn promotes to a knight
} promType;

#define promLen (promKnight + 1)

// Standard Algebraic Notation maximum character length plus one

#define sanLen 8

// Forsyth-Edwards Notation maximum character length construction

#define fenBoardLen ((fileLen + 1) * rankLen) // 72

#define fenGoodLen (1 + 1) // Active color
#define fenCsabLen (4 + 1) // Castling availability
#define fenEpsqLen (2 + 1) // En passant target square
#define fenHmvcLen (4 + 1) // Half move counter
#define fenFmvcLen (4 + 1) // Full move number

#define fenFEnvLen (fenGoodLen + fenCsabLen + fenEpsqLen + fenHmvcLen + fenFmvcLen) // 20

#define fenLen (fenBoardLen + fenFEnvLen) // 92

// Forced terminations; value determined by position and history only

typedef enum
{
  ftNil = -1,
  ftCheckmate,        // The King is dead
  ftDrawFiftyMoves,   // Not enough progress
  ftDrawInsufficient, // Not enough wood
  ftDrawRepetition,   // Not enough imagination
  ftDrawStalemate,    // The penalty for mauling without killing
  ftUnterminated      // Still going!
} ftType;

#define ftLen (ftUnterminated + 1)

// Search terminations; value determined by search exit reason

typedef enum
{
  stNil = -1,
  stAllCertain,  // All certain scores
  stBook,        // Book move selected
  stForcedLose,  // Forced lose detected
  stForcedMate,  // Forced mate detected
  stInterrupt,   // User interrupt
  stLimitDepth,  // Depth limit reached
  stLimitTime,   // Time limit reached
  stMateIn1,     // Mate in one move detected
  stNoMoves,     // No moves
  stSingleton,   // Only one move
  stUnterminated // Search stil in progress
} stType;

#define stLen (stUnterminated + 1)

// Chess position evaluation score value type (uses 1/100 pawn units)

typedef si16 svType;

#define svBroken (   -32768)
#define svPosInf      32767
#define svNegInf (-svPosInf)
#define svEven            0

#define svCP 1

#define svPawn   (100 * svCP)
#define svKnight (325 * svCP)
#define svBishop (333 * svCP)
#define svRook   (500 * svCP)
#define svQueen  (925 * svCP)
#define svKing      0
#define svVacant    0
#define svExtra     0

#define svMaxPos (svPawn * 2)

#define svInitial ((svPawn * 8) + (svKnight * 2) + (svBishop * 2) + (svRook * 2) + (svQueen * 1))

// Move generation type (gaining moves and holding moves)

typedef enum
{
  genNil = -1,
  genGain, // Captures or promotions
  genHold  // Non-captures and non-promotions
} genType;

#define genLen (genHold + 1)

// Move generation set mask type

typedef ui8 genmType;

#define genmGain BX(genGain) // Captures or promotions
#define genmHold BX(genHold) // Non-captures and non-promotions

#define genmAll (genmGain | genmHold) // All moves

// Alpha beta search node type

typedef enum
{
  abnNil = -1,
  abnBase, // Ply zero (root) operations
  abnEvad, // All check evasion (other than at ply zero)
  abnFull, // Full width operations (other than at ply zero)
  abnGain  // Capture/promotion quiescence search
} abnType;

// Options

typedef enum
{
  optnNil = -1,
  optnAA, // Audible alert on move reply
  optnCB, // Trace chessboard
  optnCT, // Trace command processing time
  optnCV, // Trace current variation
  optnIF, // Trace input FEN
  optnIP, // Trace input parameters
  optnIT, // Trace iteration announcements
  optnNL, // No opening library book
  optnNR, // No auto move reply
  optnPS, // Trace preliminary scoring
  optnPV, // Trace predicted variation updates
  optnPZ, // Trace ply zero move activity
  optnRB, // Rotate board display (White at top)
  optnSR, // Trace search result
  optnST, // Trace search termination
  optnTS  // Trace timing statistics
} optnType;

#define optnLen (optnTS + 1)

inline bool IsOptnNil(const optnType optn)    {return optn <  0;}
inline bool IsOptnNotNil(const optnType optn) {return optn >= 0;}

#define optnstrLen 2

// Option masks

typedef ui16 optnmType;

#define optnmAA BX(optnAA)
#define optnmCB BX(optnCB)
#define optnmCT BX(optnCT)
#define optnmCV BX(optnCV)
#define optnmIF BX(optnIF)
#define optnmIP BX(optnIP)
#define optnmIT BX(optnIT)
#define optnmNL BX(optnNL)
#define optnmNR BX(optnNR)
#define optnmPS BX(optnPS)
#define optnmPV BX(optnPV)
#define optnmPZ BX(optnPZ)
#define optnmRB BX(optnRB)
#define optnmSR BX(optnSR)
#define optnmST BX(optnST)
#define optnmTS BX(optnTS)

// User commands (ICP interface)

typedef enum
{
  icpcNil = -1,
  icpcAT, // Activate common trace options
  icpcBB, // Build book <input-game-file> <output-code-file>
  icpcDB, // Display board
  icpcDF, // Display FEN
  icpcDH, // Display position hash
  icpcDL, // Display opening library moves
  icpcDM, // Display moves
  icpcDO, // Display options
  icpcDS, // Display status
  icpcEM, // Enumerate movepaths
  icpcFM, // Fast enumerate movepaths
  icpcFP, // Flip position
  icpcGG, // Go and play to end of game
  icpcGP, // Go and play
  icpcGS, // Go search
  icpcHM, // Help me
  icpcID, // Show program identification
  icpcNG, // New game
  icpcPT, // Program test
  icpcQP, // Quit program
  icpcRO, // Reset option(s)
  icpcRP, // Reset program
  icpcSD, // Set depth limit
  icpcSF, // Set FEN
  icpcSO, // Set option(s)
  icpcST, // Set time limit
  icpcTB  // Take back move
} icpcType;

#define icpcLen (icpcTB + 1)

inline bool IsIcpcNil(const icpcType icpc)    {return icpc <  0;}
inline bool IsIcpcNotNil(const icpcType icpc) {return icpc >= 0;}

#define icpcstrLen 2

// Application control stage

typedef enum
{
  acsNil = -1,
  acsInit, acsMain, acsTerm, acsExit
} acsType;

#endif
