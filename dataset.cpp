#include "dataset.h"
#include <QAbstractTableModel>
#include <QColor>
#include <QJson/parser.h>
#include <QFile>
#include <iostream>

using namespace std;

Dataset::Dataset(QObject *parent) :
    QAbstractTableModel(parent)
{
}

int Dataset::loadData(QString schema_file_name, QString data_file)
{
    // CSV File Import

    FILE* f = fopen(data_file.toUtf8().data(), "r");

    if (! f)
        return DS_ERR_FILE_OPEN_FAILED;

    char line[1024];

    while(!feof(f))
    {
        fgets(line, 1024, f);
        if (strlen(line) < 2)
            continue;

        QString qline = line;
        QStringList row = qline.split(",");

        vdata.append(row);
    }

    fclose(f);

    // JSON Schema File Import

    QJson::Parser json_parser;
    QFile schemaFile(schema_file_name);

    if (! schemaFile.exists())
        return DS_ERR_FILE_OPEN_FAILED;

    bool json_ok;
    QVariant result = json_parser.parse(&schemaFile, &json_ok);

    if (! json_ok)
    {
        cerr << "Failed to parse schema json string." << endl;
        return DS_ERR_JSON_PARSE_FAILED;
    }

    // Copy values to field container
    QVariantList columns = result.toList();
    for (int i = 0; i < columns.size(); ++i) {
        QMap<QString, QVariant> col = columns.at(i).toMap();
//        QVariantMap jcol = ;

//        QMapIterator<QString, QVariant> i(jcol);
//        while (i.hasNext()) {
//            i.next();
//            col[i.key()] = i.value();
//        }

        this->schema.append(col);
    }

    return DS_ERR_SUCCEED;
}

int Dataset::rowCount(const QModelIndex&) const {
    return vdata.size();
}

int Dataset::columnCount(const QModelIndex&) const
{
    return schema.size();
}

QVariant Dataset::data(const QModelIndex &index, int role) const
{
    switch(role)
    {
    case Qt::DisplayRole:
        return vdata.at(index.row()).at(index.column());
        break;
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    case Qt::BackgroundColorRole:
        return index.row() % 2 ? QColor(220, 220, 220) : Qt::white;
    default:
        return QVariant();
    }
}

QVariant Dataset::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Vertical)
        {
            return QVariant(section+1).toString();
        } else {
            return schema.at(section).value("Name").toString();
        }
    }
    else
        return QVariant();
}

