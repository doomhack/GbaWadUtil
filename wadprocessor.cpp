#include "wadprocessor.h"
#include "doomtypes.h"

WadProcessor::WadProcessor(WadFile& wad, QObject *parent)
    : QObject(parent), wadFile(wad)
{

}

bool WadProcessor::ProcessWad()
{

}

bool WadProcessor::ProcessLevel(quint32 lumpNum)
{

}
