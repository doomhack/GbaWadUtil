#include <QCoreApplication>

#include "wadfile.h"
#include "wadprocessor.h"

void SaveBytesAsCFile(QByteArray& bytes, QString file);


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QStringList args = QCoreApplication::arguments();

    QString inFile;
    QString outFile;
    QString cFile;

    for(int i = 0; i < args.count()-1; i++)
    {
        if(args.at(i) == "-in")
            inFile = args.at(++i);
        else if(args.at(i) == "-out")
            outFile = args.at(++i);
        else if(args.at(i) == "-cfile")
            cFile = args.at(++i);

    }

    WadFile wf(inFile);
    wf.LoadWadFile();

    WadProcessor wp(wf);

    wp.ProcessWad();

    if(outFile.length())
        wf.SaveWadFile(outFile);

    if(cFile.length())
    {
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        buffer.open(QIODevice::WriteOnly);

        wf.SaveWadFile(&buffer);

        SaveBytesAsCFile(byteArray, cFile);
    }
}

void SaveBytesAsCFile(QByteArray& bytes, QString file)
{
    QFile f(file);

    if(!f.open(QIODevice::Truncate | QIODevice::ReadWrite))
        return;

    QString decl = QString("const unsigned char doom_iwad[%1UL] = {\n").arg(bytes.size());

    f.write(decl.toLatin1());

    for(int i = 0; i < bytes.size(); i++)
    {
        QString element = QString("0x%1,").arg((quint8)bytes.at(i),2, 16, QChar('0'));

        if(( (i+1) % 40) == 0)
            element += "\n";

        f.write(element.toLatin1());
    }

    QString close = QString("\n};");
    f.write(close.toLatin1());

    f.close();
}
