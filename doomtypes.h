#ifndef DOOMTYPES_H
#define DOOMTYPES_H

#include <QtCore>


typedef qint32 fixed_t;

//*************************************************************************************
//WAD file structs from Doom sources.
//*************************************************************************************

#pragma pack(push,1)

typedef struct
{
    char identification[4];                  // Should be "IWAD" or "PWAD".
    qint32  numlumps;
    qint32  infotableofs;
} wadinfo_t;

typedef struct
{
    qint32  filepos;
    qint32  size;
    char name[8];
} filelump_t;



//
// Map level types.
// The following data structures define the persistent format
// used in the lumps of the WAD files.
//

// Lump order in a map WAD: each map needs a couple of lumps
// to provide a complete scene geometry description.
enum {
  ML_LABEL,             // A separator, name, ExMx or MAPxx
  ML_THINGS,            // Monsters, items..
  ML_LINEDEFS,          // LineDefs, from editing
  ML_SIDEDEFS,          // SideDefs, from editing
  ML_VERTEXES,          // Vertices, edited and BSP splits generated
  ML_SEGS,              // LineSegs, from LineDefs split by BSP
  ML_SSECTORS,          // SubSectors, list of LineSegs
  ML_NODES,             // BSP nodes
  ML_SECTORS,           // Sectors, from editing
  ML_REJECT,            // LUT, sector-sector visibility
  ML_BLOCKMAP           // LUT, motion clipping, walls/grid element
};



//*************************************************************************************
//r_defs structs from Doom sources.
//*************************************************************************************

typedef struct
{
    fixed_t x, y;
} vertex_t;

// A single Vertex.
typedef struct
{
    short x,y;
} mapvertex_t;

#pragma pack(pop)

#endif // DOOMTYPES_H
