#ifndef WADPROCESSOR_H
#define WADPROCESSOR_H

#include <QtCore>
#include "wadfile.h"

class WadProcessor : public QObject
{
    Q_OBJECT
public:
    explicit WadProcessor(WadFile& wad, QObject *parent = nullptr);

    bool ProcessWad();

private:

    bool ProcessD2Levels();
    bool ProcessD1Levels();

    bool ProcessLevel(quint32 lumpNum);

    bool ProcessVertexes(quint32 lumpNum);
    bool ProcessLines(quint32 lumpNum);


    WadFile& wadFile;

};

#endif // WADPROCESSOR_H
