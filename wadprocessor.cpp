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
    return ProcessVertexes(lumpNum);
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
