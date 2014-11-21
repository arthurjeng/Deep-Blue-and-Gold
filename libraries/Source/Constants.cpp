// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#include "Definitions.h"
#include "Constants.h"

// Pieces on their initial files

const pieceType filepiece[fileLen] =
{
  pieceRook, pieceKnight, pieceBishop, pieceQueen, pieceKing, pieceBishop, pieceKnight, pieceRook
};

// Chessboard file directional deltas

const filedeltaType filedels[dirLen] =
{
  delfE,   delfN,   delfW,   delfS,   delfNE,  delfNW,  delfSW,  delfSE,
  delfENE, delfNNE, delfNNW, delfWNW, delfWSW, delfSSW, delfSSE, delfESE
};

// Chessboard rank directional deltas

const rankdeltaType rankdels[dirLen] =
{
  delrE,   delrN,   delrW,   delrS,   delrNE,  delrNW,  delrSW,  delrSE,
  delrENE, delrNNE, delrNNW, delrWNW, delrWSW, delrSSW, delrSSE, delrESE
};

// Chessboard square directional deltas

const sqrdeltaType sqrdels[dirLen] =
{
  delE,   delN,   delW,   delS,   delNE,  delNW,  delSW,  delSE,
  delENE, delNNE, delNNW, delWNW, delWSW, delSSW, delSSE, delESE
};

// Chessman synthesis from color and piece

const manType synthman[colorRLen][pieceLen] =
{
  {
    manWhitePawn, manWhiteKnight, manWhiteBishop, manWhiteRook, manWhiteQueen, manWhiteKing,
    manVacant, manExtra
  },
  {
    manBlackPawn, manBlackKnight, manBlackBishop, manBlackRook, manBlackQueen, manBlackKing,
    manVacant, manExtra
  }
};

// Chessman inversion

const manType otherman[manLen] =
{
  manBlackPawn, manBlackKnight, manBlackBishop, manBlackRook, manBlackQueen, manBlackKing,
  manWhitePawn, manWhiteKnight, manWhiteBishop, manWhiteRook, manWhiteQueen, manWhiteKing,
  manVacant, manExtra
};

// Chessman sweeper flags

const ui8 sweeperflag[manRLen] = {0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0};

// Chessman attribute bit masks

const manfmType manflags[manLen] =
{
  manfmWhite | manfmPawn,
  manfmWhite | manfmKnight,
  manfmWhite | manfmBishop,
  manfmWhite | manfmRook,
  manfmWhite | manfmQueen,
  manfmWhite | manfmKing,
  manfmBlack | manfmPawn,
  manfmBlack | manfmKnight,
  manfmBlack | manfmBishop,
  manfmBlack | manfmRook,
  manfmBlack | manfmQueen,
  manfmBlack | manfmKing,
  0,
  0
};

// Chessman initial attacking directions

const dirType mands0dir[manRLen] =
{
  dirNE, dirENE, dirNE, dirE, dirE, dirE,
  dirSW, dirENE, dirNE, dirE, dirE, dirE
};

// Chessman final attacking directions

const dirType mands1dir[manRLen] =
{
  dirNW, dirESE, dirSE, dirS, dirSE, dirSE,
  dirSE, dirESE, dirSE, dirS, dirSE, dirSE
};

// Converting sweep directions into bidirections

const bdrType cvsweepdirtobdr[dirSLen] =
{
  bdrE, bdrN, bdrE, bdrN, bdrNE, bdrNW, bdrNE, bdrNW
};

// Pawn advance directions and square deltas

const dirType pawnadvdir[colorRLen] = {dirN, dirS};
const sqrdeltaType pawnadvdel[colorRLen] = {delN, delS};

// Pawn capture directions

const dirType pawncapdir[colorRLen][flankLen] = {{dirNW, dirNE}, {dirSW, dirSE}};

// King castling location squares

const sqrType kinghomesqr[colorRLen] = {sqrE1, sqrE8};
const sqrType kingawaysqr[castLen] = {sqrC1, sqrG1, sqrC8, sqrG8};

// Rook castling location squares

const sqrType rookhomesqr[castLen] = {sqrA1, sqrH1, sqrA8, sqrH8};
const sqrType rookawaysqr[castLen] = {sqrD1, sqrF1, sqrD8, sqrF8};

// Castling vacancy square checklist

const sqrType castvacsqr[castLen][cstlvacsqLen] =
{
  {sqrD1, sqrC1, sqrB1}, {sqrF1, sqrG1, sqrNil}, {sqrD8, sqrC8, sqrB8}, {sqrF8, sqrG8, sqrNil}
};

// Castling attacked square checklist

const sqrType castatksqr[castLen][cstlatksqLen] =
{
  {sqrD1, sqrC1}, {sqrF1, sqrG1}, {sqrD8, sqrC8}, {sqrF8, sqrG8}
};

// Castling inversion

const castType othercast[castLen] = {castBQ, castBK, castWQ, castWK};

// Pawn color/direction attack capability

const ui8 pawncolordirattack[colorRLen][dirLen] =
{
  {0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0}
};

// Rank maps by color

const rankType r1rank[colorRLen] = {rank1, rank8};
const rankType r2rank[colorRLen] = {rank2, rank7};
const rankType r3rank[colorRLen] = {rank3, rank6};
const rankType r4rank[colorRLen] = {rank4, rank5};
const rankType r5rank[colorRLen] = {rank5, rank4};
const rankType r6rank[colorRLen] = {rank6, rank3};
const rankType r7rank[colorRLen] = {rank7, rank2};
const rankType r8rank[colorRLen] = {rank8, rank1};

// Converting color and flank to castling

const castType cvcolorflanktocast[colorRLen][flankLen] = {{castWQ, castWK}, {castBQ, castBK}};

// Converting move special case to castling flank

const flankType cvmsctoflank[mscLen] =
{
  flankNil, flankNil, flankQ, flankK, flankNil, flankNil, flankNil, flankNil
};

// Converting castling flank to move special case

const mscType cvflanktomsc[flankLen] = {mscCQS, mscCKS};

// Converting chessman to color

const colorType cvmantocolor[manLen] =
{
  colorWhite, colorWhite, colorWhite, colorWhite, colorWhite, colorWhite,
  colorBlack, colorBlack, colorBlack, colorBlack, colorBlack, colorBlack,
  colorVacant, colorExtra
};

// Converting chessman to piece

const pieceType cvmantopiece[manLen] =
{
  piecePawn, pieceKnight, pieceBishop, pieceRook, pieceQueen, pieceKing,
  piecePawn, pieceKnight, pieceBishop, pieceRook, pieceQueen, pieceKing,
  pieceVacant, pieceExtra
};

// Converting castling to color

const colorType cvcasttocolor[castLen] = {colorWhite, colorWhite, colorBlack, colorBlack};

// Converting pawn promotion to move special case

const mscType cvpromtomsc[promLen] = {mscPPQ, mscPPR, mscPPB, mscPPN};

// Converting move special case to pawn promotion

const promType cvmsctoprom[mscLen] =
{
  promNil, promNil, promNil, promNil, promKnight, promBishop, promRook, promQueen
};

// Converting pawn promotion to piece

const pieceType cvpromtopiece[promLen] = {pieceQueen, pieceRook, pieceBishop, pieceKnight};

// Converting color to king target ID

const tidType cvcolortokingtid[colorRLen] = {tidWhiteKing, tidBlackKing};

// Converting color to target ID limit

const tidType cvcolortotidlimit[colorRLen] =
{
  (tidWhiteKing + troopLen), (tidBlackKing + troopLen)
};

// Converting chessman to score value

const svType cvmantosv[manLen] =
{
  svPawn, svKnight, svBishop, svRook, svQueen, svKing,
  svPawn, svKnight, svBishop, svRook, svQueen, svKing,
  svVacant, svExtra
};

// Converting move special case to score value

const svType cvmsctosv[mscLen] =
{
  svEven,              svPawn,              svEven,            svEven,
  (svKnight - svPawn), (svBishop - svPawn), (svRook - svPawn), (svQueen - svPawn)
};

// Character flash storage: digits

const char charofdigit[16]        PROGMEM =
{
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

// Character flash storage: files and ranks

const char charoffile[fileLen]    PROGMEM = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
const char charofrank[rankLen]    PROGMEM = {'1', '2', '3', '4', '5', '6', '7', '8'};

// Character flash storage: colors and pieces

const char charofcolor[colorLen]  PROGMEM = {'w', 'b', ' ', '?'};
const char charofpiece[pieceLen]  PROGMEM = {'P', 'N', 'B', 'R', 'Q', 'K', ' ', '?'};

// Character flash storage: chessmen (for FEN Man Placement Data field)

const char charofman[manLen]      PROGMEM =
{
  'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k', ' ', '?'
};

// Character flash storage: castlings (for FEN castling availability field)

const char charofcast[castLen]    PROGMEM = {'Q', 'K', 'q', 'k'};

// String flash storage: program banner

const char fsBanner0[]            PROGMEM = "Myopic v2010.08.13: A simple chess program for small systems\n";
const char fsBanner1[]            PROGMEM = "Copyright (C) 2010 by chessnotation@me.com   Some rights reserved.\n";
const char fsBanner2[]            PROGMEM = "License: Creative Commons Attribution-Share Alike 3.0\n";
const char fsBanner3[]            PROGMEM = "See: http://creativecommons.org/licenses/by-sa/3.0/\n";

// String flash storage: General options (must be pairwise ASCII ordered)

const char fsOptnStrs[]           PROGMEM = "aacbctcvifipitnlnrpspvpzrbsrstts";

// String flash storage: ICP commands (must be pairwise ASCII ordered)

const char fsIcpcStrs[]           PROGMEM = "atbbdbdfdhdldmdodsemfmfpgggpgshmidngptqprorpsdsfsosttb";

// String flash storage: ICP user diagnostics

const char fsUdBadMove[]          PROGMEM = "Bad move (try the 'dm' or 'hm' commands)\n";
const char fsUdBadParmCount[]     PROGMEM = "Bad parameter count\n";
const char fsUdBadParmValue[]     PROGMEM = "Bad parameter value\n";
const char fsUdDevHostOnly[]      PROGMEM = "Development host only\n";
const char fsUdIgnoredParms[]     PROGMEM = "Parameters are ignored for this command.\n";
const char fsUdNeedSingleUIParm[] PROGMEM = "Need single unsigned integer parameter\n";
const char fsUdNoTakeBackAvail[]  PROGMEM = "No take back available\n";

// String flash storage: forced terminations

const char fsFtCheckmate[]        PROGMEM = "Checkmate";
const char fsFtDrawFiftyMoves[]   PROGMEM = "Draw (fifty  move rule)";
const char fsFtDrawInsufficient[] PROGMEM = "Draw (insufficient material)";
const char fsFtDrawRepetition[]   PROGMEM = "Draw (position repetition)";
const char fsFtDrawStalemate[]    PROGMEM = "Draw (stalemate)";
const char fsFtUnterminated[]     PROGMEM = "Unterminated";

// String flash storage: search terminations

const char fsStAllCertain[]       PROGMEM = "All certain scores";
const char fsStBook[]             PROGMEM = "Book move selected";
const char fsStForcedLose[]       PROGMEM = "Forced lose detected";
const char fsStForcedMate[]       PROGMEM = "Forced mate detected";
const char fsStInterrupt[]        PROGMEM = "User interrupt";
const char fsStLimitDepth[]       PROGMEM = "Depth limit reached";
const char fsStLimitTime[]        PROGMEM = "Time limit reached";
const char fsStMateIn1[]          PROGMEM = "Mate in one move detected";
const char fsStNoMoves[]          PROGMEM = "No moves at root";
const char fsStSingleton[]        PROGMEM = "Only one move";
const char fsStUnterminated[]     PROGMEM = "Search stil in progress";

// String flash storage: special scores, including mate/lose score prefixes

const char fsScBroken[]           PROGMEM = "Broken";
const char fsScCheckmated[]       PROGMEM = "Checkmated";
const char fsScLoseIn[]           PROGMEM = "LoseIn";
const char fsScMateIn[]           PROGMEM = "MateIn";
const char fsScNegInf[]           PROGMEM = "NegInf";
const char fsScPosInf[]           PROGMEM = "PosInf";

// String flash storage: labels

const char fsLbDepthLimit[]       PROGMEM = "Depth limit";
const char fsLbDepth[]            PROGMEM = "Depth";
const char fsLbDistance[]         PROGMEM = "Distance";
const char fsLbFen[]              PROGMEM = "FEN";
const char fsLbFrequency[]        PROGMEM = "Frequency";
const char fsLbGameOver[]         PROGMEM = "Game over";
const char fsLbHash[]             PROGMEM = "Hash";
const char fsLbIteration[]        PROGMEM = "Iteration";
const char fsLbMove[]             PROGMEM = "Move";
const char fsLbMoves[]            PROGMEM = "Moves";
const char fsLbMyMove[]           PROGMEM = "My move";
const char fsLbNodes[]            PROGMEM = "Nodes";
const char fsLbOptions[]          PROGMEM = "Options";
const char fsLbPV[]               PROGMEM = "PV";
const char fsLbPlaying[]          PROGMEM = "Playing";
const char fsLbScore[]            PROGMEM = "Score";
const char fsLbSeconds[]          PROGMEM = "Seconds";
const char fsLbTimeLimit[]        PROGMEM = "Time limit";
const char fsLbUnplayedMove[]     PROGMEM = "Unplayed move";
const char fsLbYourMove[]         PROGMEM = "Your move";

// String flash storage: fatal errors

const char fsFeLocateFault[]      PROGMEM = "Can't locate move: ";
const char fsFeOverflowMS[]       PROGMEM = "Move stack overflow\n";
const char fsFeOverflowPS[]       PROGMEM = "Ply stack overflow\n";
const char fsFeSwitchFault[]      PROGMEM = "Switch fault\n";
const char fsFeUnderflowPS[]      PROGMEM = "Ply stack underflow\n";

// String flash storage: miscellaneous

const char fsMsAssist[]           PROGMEM = "Enter 'hm' for help, 'qp' to quit.\n";
const char fsMsCaution[]          PROGMEM = "Caution: No warranty; use at your own risk.\n";
const char fsMsEllipsis[]         PROGMEM = "...";
const char fsMsFinished[]         PROGMEM = "Done\n";
const char fsMsInterrupt[]        PROGMEM = "Interrupt\n";
const char fsMsMateAnnounce0[]    PROGMEM = "I have a checkmate in ";
const char fsMsMateAnnounce1[]    PROGMEM = " move.\n";
const char fsMsMateAnnounce2[]    PROGMEM = " moves.\n";
const char fsMsNoResult[]         PROGMEM = "No result was found.\n";
const char fsMsPrompt[]           PROGMEM = "[] ";
const char fsMsResigns[]          PROGMEM = "I resign.\n";
const char fsMsSfxHz[]            PROGMEM = " Hz";
const char fsMsSqBlack[]          PROGMEM = "::";
const char fsMsSqWhite[]          PROGMEM = "  ";

// String flash storage: the ICP help string

const char fsIcpHelp[]            PROGMEM =
  "Commands:\n"
  "  at  Activate common trace options\n"
  "  bb  Build book <input-game-file> <output-code-file>\n"
  "  db  Display board\n"
  "  df  Display FEN\n"
  "  dh  Display position hash\n"
  "  dl  Display opening library moves\n"
  "  dm  Display moves\n"
  "  do  Display options\n"
  "  ds  Display status\n"
  "  em  Enumerate movepaths to depth <n> plies\n"
  "  fm  Fast enumerate movepaths to depth <n> plies\n"
  "  fp  Flip position\n"
  "  gg  Go and play to end of game\n"
  "  gp  Go and play\n"
  "  gs  Go search\n"
  "  hm  Help me\n"
  "  id  Show program identification\n"
  "  ng  New game\n"
  "  pt  Program test\n"
  "  qp  Quit program\n"
  "  ro  Reset option(s) (limit 7)\n"
  "  rp  Reset program\n"
  "  sd  Set depth limit to <n> plies\n"
  "  sf  Set FEN\n"
  "  so  Set option(s) (limit 7)\n"
  "  st  Set time limit to <n> seconds\n"
  "  tb  Take back move\n"
  "\n"
  "Options:\n"
  "  aa  Audible alert on move reply\n"
  "  cb  Auto display chessboard\n"
  "  ct  Trace command processing time\n"
  "  cv  Trace current variation\n"
  "  if  Trace input FEN\n"
  "  ip  Trace input parameters\n"
  "  it  Trace iteration announcements\n"
  "  nl  No opening library book\n"
  "  nr  No auto move reply\n"
  "  ps  Trace preliminary scoring\n"
  "  pv  Trace predicted variation updates\n"
  "  pz  Trace ply zero move activity\n"
  "  rb  Rotate board display (White at top)\n"
  "  sr  Trace search result\n"
  "  st  Trace search termination\n"
  "  ts  Trace timing statistics\n"
  "\n"
  "Moves:\n"
  "  Enter one or move moves (limit 8); use 'dm' command for list.\n";
