// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_Constants
#define Included_Constants

extern const pieceType filepiece[fileLen];

extern const filedeltaType filedels[dirLen];
extern const rankdeltaType rankdels[dirLen];
extern const sqrdeltaType sqrdels[dirLen];

extern const manType synthman[colorRLen][pieceLen];

extern const manType otherman[manLen];

extern const ui8 sweeperflag[manRLen];

extern const manfmType manflags[manLen];

extern const dirType mands0dir[manRLen];
extern const dirType mands1dir[manRLen];

extern const bdrType cvsweepdirtobdr[dirSLen];

extern const dirType pawnadvdir[colorRLen];
extern const sqrdeltaType pawnadvdel[colorRLen];

extern const dirType pawncapdir[colorRLen][flankLen];

extern const sqrType kinghomesqr[colorRLen];
extern const sqrType kingawaysqr[castLen];

extern const sqrType rookhomesqr[castLen];
extern const sqrType rookawaysqr[castLen];

extern const sqrType castvacsqr[castLen][cstlvacsqLen];
extern const sqrType castatksqr[castLen][cstlatksqLen];

extern const castType othercast[castLen];

extern const ui8 pawncolordirattack[colorRLen][dirLen];

extern const rankType r1rank[colorRLen];
extern const rankType r2rank[colorRLen];
extern const rankType r3rank[colorRLen];
extern const rankType r4rank[colorRLen];
extern const rankType r5rank[colorRLen];
extern const rankType r6rank[colorRLen];
extern const rankType r7rank[colorRLen];
extern const rankType r8rank[colorRLen];

extern const colorType cvmantocolor[manLen];
extern const pieceType cvmantopiece[manLen];

extern const castType cvcolorflanktocast[colorRLen][flankLen];

extern const flankType cvmsctoflank[mscLen];
extern const mscType cvflanktomsc[flankLen];

extern const colorType cvcasttocolor[castLen];

extern const mscType cvpromtomsc[promLen];
extern const promType cvmsctoprom[mscLen];
extern const pieceType cvpromtopiece[promLen];

extern const tidType cvcolortokingtid[colorRLen];
extern const tidType cvcolortotidlimit[colorRLen];

extern const svType cvmantosv[manLen];
extern const svType cvmsctosv[mscLen];

extern const char charofdigit[16]        PROGMEM;

extern const char charoffile[fileLen]    PROGMEM;
extern const char charofrank[rankLen]    PROGMEM;

extern const char charofcolor[colorLen]  PROGMEM;
extern const char charofpiece[pieceLen]  PROGMEM;

extern const char charofman[manLen]      PROGMEM;

extern const char charofcast[castLen]    PROGMEM;

extern const char fsBanner0[]            PROGMEM;
extern const char fsBanner1[]            PROGMEM;
extern const char fsBanner2[]            PROGMEM;
extern const char fsBanner3[]            PROGMEM;

extern const char fsOptnStrs[]           PROGMEM;

extern const char fsIcpcStrs[]           PROGMEM;

extern const char fsUdBadMove[]          PROGMEM;
extern const char fsUdBadParmCount[]     PROGMEM;
extern const char fsUdBadParmValue[]     PROGMEM;
extern const char fsUdDevHostOnly[]      PROGMEM;
extern const char fsUdIgnoredParms[]     PROGMEM;
extern const char fsUdNeedSingleUIParm[] PROGMEM;
extern const char fsUdNoTakeBackAvail[]  PROGMEM;

extern const char fsFtCheckmate[]        PROGMEM;
extern const char fsFtDrawFiftyMoves[]   PROGMEM;
extern const char fsFtDrawInsufficient[] PROGMEM;
extern const char fsFtDrawRepetition[]   PROGMEM;
extern const char fsFtDrawStalemate[]    PROGMEM;
extern const char fsFtUnterminated[]     PROGMEM;

extern const char fsStAllCertain[]       PROGMEM;
extern const char fsStBook[]             PROGMEM;
extern const char fsStForcedLose[]       PROGMEM;
extern const char fsStForcedMate[]       PROGMEM;
extern const char fsStInterrupt[]        PROGMEM;
extern const char fsStLimitDepth[]       PROGMEM;
extern const char fsStLimitTime[]        PROGMEM;
extern const char fsStMateIn1[]          PROGMEM;
extern const char fsStNoMoves[]          PROGMEM;
extern const char fsStSingleton[]        PROGMEM;
extern const char fsStUnterminated[]     PROGMEM;

extern const char fsScBroken[]           PROGMEM;
extern const char fsScCheckmated[]       PROGMEM;
extern const char fsScLoseIn[]           PROGMEM;
extern const char fsScMateIn[]           PROGMEM;
extern const char fsScNegInf[]           PROGMEM;
extern const char fsScPosInf[]           PROGMEM;

extern const char fsLbDepthLimit[]       PROGMEM;
extern const char fsLbDepth[]            PROGMEM;
extern const char fsLbDistance[]         PROGMEM;
extern const char fsLbFen[]              PROGMEM;
extern const char fsLbFrequency[]        PROGMEM;
extern const char fsLbGameOver[]         PROGMEM;
extern const char fsLbHash[]             PROGMEM;
extern const char fsLbIteration[]        PROGMEM;
extern const char fsLbMove[]             PROGMEM;
extern const char fsLbMoves[]            PROGMEM;
extern const char fsLbMyMove[]           PROGMEM;
extern const char fsLbNodes[]            PROGMEM;
extern const char fsLbOptions[]          PROGMEM;
extern const char fsLbPV[]               PROGMEM;
extern const char fsLbPlaying[]          PROGMEM;
extern const char fsLbScore[]            PROGMEM;
extern const char fsLbSeconds[]          PROGMEM;
extern const char fsLbTimeLimit[]        PROGMEM;
extern const char fsLbUnplayedMove[]     PROGMEM;
extern const char fsLbYourMove[]         PROGMEM;

extern const char fsFeLocateFault[]      PROGMEM;
extern const char fsFeOverflowMS[]       PROGMEM;
extern const char fsFeOverflowPS[]       PROGMEM;
extern const char fsFeSwitchFault[]      PROGMEM;
extern const char fsFeUnderflowPS[]      PROGMEM;

extern const char fsMsAssist[]           PROGMEM;
extern const char fsMsCaution[]          PROGMEM;
extern const char fsMsEllipsis[]         PROGMEM;
extern const char fsMsFinished[]         PROGMEM;
extern const char fsMsInterrupt[]        PROGMEM;
extern const char fsMsMateAnnounce0[]    PROGMEM;
extern const char fsMsMateAnnounce1[]    PROGMEM;
extern const char fsMsMateAnnounce2[]    PROGMEM;
extern const char fsMsNoResult[]         PROGMEM;
extern const char fsMsPrompt[]           PROGMEM;
extern const char fsMsResigns[]          PROGMEM;
extern const char fsMsSfxHz[]            PROGMEM;
extern const char fsMsSqBlack[]          PROGMEM;
extern const char fsMsSqWhite[]          PROGMEM;

extern const char fsIcpHelp[]            PROGMEM;

#endif
