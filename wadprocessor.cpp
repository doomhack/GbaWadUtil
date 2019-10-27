#include "wadprocessor.h"
#include "doomtypes.h"

WadProcessor::WadProcessor(WadFile& wad, QObject *parent)
    : QObject(parent), wadFile(wad)
{

}

bool WadProcessor::ProcessWad()
{
    //Figure out if our IWAD is Doom or Doom2. (E1M1 or MAP01)

    Lump mapLump;

    int lumpNum = wadFile.GetLumpByName("MAP01", mapLump);

    if(lumpNum != -1)
    {
        return ProcessD2Levels();
    }
    else
    {
        int lumpNum = wadFile.GetLumpByName("E1M1", mapLump);

        //Can't find any maps.
        if(lumpNum == -1)
            return false;
    }

    return ProcessD1Levels();
}

bool WadProcessor::ProcessD2Levels()
{
    for(int m = 1; m <= 32; m++)
    {
        Lump l;

        QString mapName = QString("MAP%1").arg(m, 2, 10, QChar('0'));

        int lumpNum = wadFile.GetLumpByName(mapName, l);

        if(lumpNum != -1)
        {
            ProcessLevel(lumpNum);
        }
    }

    return true;
}

bool WadProcessor::ProcessD1Levels()
{
    for(int e = 1; e <= 4; e++)
    {
        for(int m = 1; m <= 9; m++)
        {
            Lump l;

            QString mapName = QString("E%1M%2").arg(e).arg(m);

            int lumpNum = wadFile.GetLumpByName(mapName, l);

            if(lumpNum != -1)
            {
                ProcessLevel(lumpNum);
            }
        }
    }

    return true;
}

bool WadProcessor::ProcessLevel(quint32 lumpNum)
{
    ProcessVertexes(lumpNum);
    ProcessLines(lumpNum);
    ProcessSegs(lumpNum);

    return true;
}

bool WadProcessor::ProcessVertexes(quint32 lumpNum)
{
    quint32 vtxLumpNum = lumpNum+ML_VERTEXES;

    Lump vxl;

    if(!wadFile.GetLumpByNum(vtxLumpNum, vxl))
        return false;

    if(vxl.length == 0)
        return false;

    quint32 vtxCount = vxl.length / sizeof(mapvertex_t);

    vertex_t* newVtx = new vertex_t[vtxCount];
    const mapvertex_t* oldVtx = reinterpret_cast<const mapvertex_t*>(vxl.data.constData());

    for(int i = 0; i < vtxCount; i++)
    {
        newVtx[i].x = (oldVtx[i].x << 16);
        newVtx[i].y = (oldVtx[i].y << 16);
    }

    Lump newVxl;
    newVxl.name = vxl.name;
    newVxl.length = vtxCount * sizeof(vertex_t);
    newVxl.data = QByteArray(reinterpret_cast<const char*>(newVtx), newVxl.length);

    delete[] newVtx;

    wadFile.ReplaceLump(vtxLumpNum, newVxl);

    return true;
}

bool WadProcessor::ProcessLines(quint32 lumpNum)
{
    quint32 lineLumpNum = lumpNum+ML_LINEDEFS;

    Lump lines;

    if(!wadFile.GetLumpByNum(lineLumpNum, lines))
        return false;

    if(lines.length == 0)
        return false;

    quint32 lineCount = lines.length / sizeof(maplinedef_t);

    line_t* newLines = new line_t[lineCount];

    const maplinedef_t* oldLines = reinterpret_cast<const maplinedef_t*>(lines.data.constData());

    //We need vertexes for this...

    quint32 vtxLumpNum = lumpNum+ML_VERTEXES;

    Lump vxl;

    if(!wadFile.GetLumpByNum(vtxLumpNum, vxl))
        return false;

    if(vxl.length == 0)
        return false;

    const vertex_t* vtx = reinterpret_cast<const vertex_t*>(vxl.data.constData());

    for(unsigned int i = 0; i < lineCount; i++)
    {
        newLines[i].v1.x = vtx[oldLines[i].v1].x;
        newLines[i].v1.y = vtx[oldLines[i].v1].y;

        newLines[i].v2.x = vtx[oldLines[i].v2].x;
        newLines[i].v2.y = vtx[oldLines[i].v2].y;

        newLines[i].special = oldLines[i].special;
        newLines[i].flags = oldLines[i].flags;
        newLines[i].tag = oldLines[i].tag;

        newLines[i].dx = newLines[i].v2.x - newLines[i].v1.x;
        newLines[i].dy = newLines[i].v2.y - newLines[i].v1.y;

        newLines[i].slopetype =
                !newLines[i].dx ? ST_VERTICAL : !newLines[i].dy ? ST_HORIZONTAL :
                FixedDiv(newLines[i].dy, newLines[i].dx) > 0 ? ST_POSITIVE : ST_NEGATIVE;

        newLines[i].sidenum[0] = oldLines[i].sidenum[0];
        newLines[i].sidenum[1] = oldLines[i].sidenum[1];

        newLines[i].lineno = i;

    }

    Lump newLine;
    newLine.name = lines.name;
    newLine.length = lineCount * sizeof(line_t);
    newLine.data = QByteArray(reinterpret_cast<const char*>(newLines), newLine.length);

    delete[] newLines;

    wadFile.ReplaceLump(lineLumpNum, newLine);

    return true;
}

bool WadProcessor::ProcessSegs(quint32 lumpNum)
{
    quint32 segsLumpNum = lumpNum+ML_SEGS;

    Lump segs;

    if(!wadFile.GetLumpByNum(segsLumpNum, segs))
        return false;

    if(segs.length == 0)
        return false;

    quint32 segCount = segs.length / sizeof(mapseg_t);

    seg_t* newSegs = new seg_t[segCount];

    const mapseg_t* oldSegs = reinterpret_cast<const mapseg_t*>(segs.data.constData());

    //We need vertexes for this...

    quint32 vtxLumpNum = lumpNum+ML_VERTEXES;

    Lump vxl;

    if(!wadFile.GetLumpByNum(vtxLumpNum, vxl))
        return false;

    if(vxl.length == 0)
        return false;

    const vertex_t* vtx = reinterpret_cast<const vertex_t*>(vxl.data.constData());

    //And LineDefs. Must process lines first.

    quint32 linesLumpNum = lumpNum+ML_LINEDEFS;

    Lump lxl;

    if(!wadFile.GetLumpByNum(linesLumpNum, lxl))
        return false;

    if(lxl.length == 0)
        return false;

    const line_t* lines = reinterpret_cast<const line_t*>(lxl.data.constData());

    //And sides too...

    quint32 sidesLumpNum = lumpNum+ML_SIDEDEFS;

    Lump sxl;

    if(!wadFile.GetLumpByNum(sidesLumpNum, sxl))
        return false;

    if(sxl.length == 0)
        return false;

    const mapsidedef_t* sides = reinterpret_cast<const mapsidedef_t*>(sxl.data.constData());


    //****************************

    for(unsigned int i = 0; i < segCount; i++)
    {
        newSegs[i].v1.x = vtx[oldSegs[i].v1].x;
        newSegs[i].v1.y = vtx[oldSegs[i].v1].y;

        newSegs[i].v2.x = vtx[oldSegs[i].v2].x;
        newSegs[i].v2.y = vtx[oldSegs[i].v2].y;

        newSegs[i].angle = oldSegs[i].angle << 16;
        newSegs[i].offset = oldSegs[i].offset << 16;

        newSegs[i].linenum = oldSegs[i].linedef;

        const line_t* ldef = &lines[newSegs[i].linenum];

        int side = oldSegs[i].side;

        newSegs[i].sidenum = ldef->sidenum[side];

        if(newSegs[i].sidenum != NO_INDEX)
        {
            newSegs[i].frontsectornum = sides[newSegs[i].sidenum].sector;
        }
        else
        {
            newSegs[i].frontsectornum = NO_INDEX;
        }

        newSegs[i].backsectornum = NO_INDEX;

        if(ldef->flags & ML_TWOSIDED)
        {
            if(ldef->sidenum[side^1] != NO_INDEX)
            {
                newSegs[i].backsectornum = sides[ldef->sidenum[side^1]].sector;
            }
        }
    }

    Lump newSeg;
    newSeg.name = segs.name;
    newSeg.length = segCount * sizeof(seg_t);
    newSeg.data = QByteArray(reinterpret_cast<const char*>(newSegs), newSeg.length);

    delete[] newSegs;

    wadFile.ReplaceLump(segsLumpNum, newSeg);

    return true;
}
