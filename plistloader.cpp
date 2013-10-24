/*
 * plistloader.cpp
 *
 * Copyright (c) 2013, Florent Ruard-Dumaine. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301  USA
 */
#include "plistloader.h"

#include <QFile>
#include <QDir>

#include <QtXmlPatterns/QXmlQuery>
#include <QtXmlPatterns/QXmlResultItems>
#include <QRegExp>
#include <QBuffer>

#include <QDebug>


PListLoader::PListLoader(QObject *parent) :
    QObject(parent)
{
}


PListLoader::PListLoader(QString filename, QObject *parent) :
    QObject(parent)
{
    loadFile(filename);
}


bool PListLoader::loadFile(QString filename)
{

    qDebug() << filename;


    if ( !document.setContent(new QFile(filename), false, &errorMsg, &errorLine, &errorColumn))
    {
        qDebug() << QString("ERROR %1 at line %2 %3").arg(errorMsg).arg(errorLine).arg(errorColumn) ;

        return false;
//        (new QErrorMessage())->showMessage(QString("%1 parse error line %2 column %3 :\n%4")
//                      .arg(filename)
//                      .arg(errorLine)
//                      .arg(errorColumn)
//                      .arg(errorMsg));
//        return false;
    }

    frames = get("//key[text()='frames']/following-sibling::dict");

    if (frames == QMultiMap<QString, QVariant>())
        return true;

//    qDebug() << frames;

    QMultiMap<QString, QVariant> metadata = getMetadata();

    qDebug() << "GE TMETADATA" << metadata;

    QString textureFileName = metadata.value("textureFileName").toString();

    qDebug() << QString("Parsed") << QFileInfo(filename).dir().path() << filename << textureFileName;
    imageFilePathName = QFileInfo(filename).dir().path() + "/" + textureFileName;
    loadedImage.load(imageFilePathName);

    qDebug() << QString("loaded");
    qDebug() << imageFilePathName;

    return true;
}

QMultiMap<QString, QVariant> PListLoader::getFrames()
{
    return frames;
}

QVariant PListLoader::getValue(QVariant parent, QString valName)
{
    if (parent.type() == QVariant::Map && parent.toMap().contains(valName))
    {
        if (parent.toMap().value(valName).type() == QVariant::Map && parent.toMap().value(valName).toMap().contains("value"))
            return parent.toMap().value(valName).toMap().value("value");
        else if (parent.toMap().value(valName).type() == QVariant::Map )
            return parent.toMap().value(valName).toMap();
        else
            return parent.toMap().value(valName);
    }
    return QVariant();
}

QImage PListLoader::getImage(QString name)
{
    qDebug() << "Test GetImage";

    if (loadedImage == QImage())
        return loadedImage;

    qDebug() << "FRAMES" << frames;
    if (frames.contains(name))
    {
        qDebug() << "GET IMAGE " << frames.value(name) << getValue(frames.value(name), "textureRect");
        if (getValue(frames.value(name), "textureRect").isValid())
        {
            return loadedImage.copy(getValue(frames.value(name), "textureRect").toRect());
        }
        else if (getValue(frames.value(name), "width").isValid())
        {
            return loadedImage.copy(QRect(
                    getValue(frames.value(name), "x").toInt(),
                    getValue(frames.value(name), "y").toInt(),
                    getValue(frames.value(name), "width").toInt(),
                    getValue(frames.value(name), "height").toInt()
            ));
        }
    }
    return QImage();


}

QDomDocument PListLoader::getDomDocument(QString xpath_query)
{
    QXmlQuery query;
//    qDebug() << document.toString(0);


    QByteArray outArray;
    QBuffer outBuff(&outArray);
    outBuff.open(QIODevice::ReadWrite);

    QByteArray inArray = document.toByteArray();
    QBuffer inBuf(&inArray);
    inBuf.open(QIODevice::ReadOnly);


    query.setFocus(&inBuf);//document.toString(0));
    qDebug() << QString("Focused");

    query.setQuery(xpath_query);
    qDebug() << QString("queryed");

    if (query.isValid())
        query.evaluateTo(&outBuff);
    qDebug() << QString("Evaluate");

    QString strReturn = QString::fromUtf8(outArray.constData());
    qDebug() << QString("DOMDOC BEFORE ") << strReturn;

    QDomDocument frag;
    frag.setContent(strReturn);
    qDebug() << QString("DOMDOC ") <<frag.toString();

    return frag;
}

QMultiMap<QString, QVariant> PListLoader::getSubChild(QDomElement el)
{

    QMultiMap<QString, QVariant>  map;
    QMultiMap<QString, QVariant>  tmpMap;


    QDomNode node = el.firstChild();

    while(!node.isNull())
    {

        if(node.isElement())
        {
            qDebug() << node.nodeName() << node.nodeValue();
            QString tagName = node.toElement().tagName();
            if (node.hasChildNodes() && tagName == "key")
            {
                QString name = node.toElement().firstChild().toText().data();
                node = node.nextSibling();
                if (node.isElement())
                {
                    tagName = node.toElement().tagName();
                    qDebug() << name << tagName << (tagName == "false") << (tagName == "true") << (tagName == "false" || tagName == "true");
                    if (tagName == "false" || tagName == "true")
                    {
                        qDebug() << QVariant((tagName == "false" ? false : true));
                        map.insert(name, QVariant((tagName == "false" ? false : true)));
                    }
                    else if(tagName == "integer")
                    {
                        qDebug() << "IsInteger" << node.toElement().firstChild().toText().data().trimmed().toInt();
                        tmpMap.insert(name, node.toElement().firstChild().toText().data().trimmed().toInt() );
                    }
                    else if(tagName == "float")
                    {
                        qDebug() << "IsFloat" << node.toElement().firstChild().toText().data().trimmed().toFloat();
                        tmpMap.insert(name, node.toElement().firstChild().toText().data().trimmed().toFloat() );

                    }
                    else if (tagName == "array")
                    {

                        // parseArray(getSubChild(node.toElement()).value("value"));
//                        QMultiMap<QString, QVariant> child = getSubChild(node.toElement());

                        bool chp = checkPointAndRect(name, node.toElement(), &map, &tmpMap);

                        qDebug() << QString("ARRAY") << chp;
                    }
                    else
                    {
                        bool chp = checkPointAndRect(name, node.toElement(), &map, &tmpMap);

//                        QMultiMap<QString, QVariant> child = getSubChild(node.toElement());
//                        // parseArray() , parsePoint(), ...
//                        qDebug() << QString("CHILD TEST RECT") << child << child.value("value");

//                        bool inserted = false;
//                        if (child.value("value").type() == QVariant::String)
//                        {
//                            QString value = child.value("value").toString();
//                            QString base = "\\{\\s*(-?\\d*)\\s*,\\s*(-?\\d*)\\s*\\}";

//                            QRegExp re(base);//"{\\s*"+base+"\\s*,\\s*"+base+"\\s*}");
//                            int pos = 0;
//                            qDebug() << "IsString" << value << base << re.pattern();
//                            pos = re.indexIn(value, pos);
//                            if (pos != -1)
//                            {
//                                //QPoint, QRect, QPointList
//                                QList<QVariant> pointList;
//                                QStringList sl = re.capturedTexts();

//                                qDebug() << pos << sl;
//                                pointList.append(QPoint(sl.at(1).toInt(), sl.at(2).toInt()));
//                                pos += re.matchedLength();

//                                while (( pos = re.indexIn(value, pos)) != -1)
//                                {
//                                    sl = re.capturedTexts();
//                                    pointList.append(QPoint(sl.at(1).toInt(), sl.at(2).toInt()));
//                                    qDebug() << pos << sl;
//                                    pos += re.matchedLength();
//                                }
//                                qDebug() << pointList;
//                                if (pointList.size() == 1)
//                                {
//                                    map.insert(name, pointList.at(0).toPoint());
//                                }
//                                else if(pointList.size() == 2)
//                                {
//                                    qDebug() << QString("Rect");
//                                    map.insert(name, QRect(pointList.at(0).toPoint(), QSize(pointList.at(1).toPoint().x(), pointList.at(1).toPoint().y()) ));
//                                }
//                                else
//                                {
//                                    qDebug() << QString("Point Size %1").arg(pointList.size());
//                                    map.insert(name, pointList);
//                                }
//                                inserted = true;
//                            }
//                            else
//                            {
//                                map.insert(name, value);
//                                inserted = true;
//                            }

//                        }
//                        if (!inserted)
//                        {
//                            qDebug() << QString("Non inserted");

//                            qDebug() << name << child;
//                            tmpMap.insert(name, child);
//                        }
                    }
                }
                else if (node.isText())
                {
                    qDebug() << QString("isText");

                    map.insert(name, node.toElement().firstChild().toText().data().trimmed());
                }
            }
            else if (tagName == "string")
            {
                qDebug() << "IsString" << node.toElement().firstChild().toText().data().trimmed();
                tmpMap.insert("value", node.toElement().firstChild().toText().data().trimmed() );
//                return tmpMap;
            }
            else if(tagName == "integer")
            {
                qDebug() << "IsInteger" << node.toElement().firstChild().toText().data().trimmed().toInt();
                tmpMap.insert("value", node.toElement().firstChild().toText().data().trimmed().toInt() );
            }
            else if(tagName == "float")
            {
                qDebug() << "IsFloat" << node.toElement().firstChild().toText().data().trimmed().toFloat();
                tmpMap.insert("value", node.toElement().firstChild().toText().data().trimmed().toFloat() );

            }
            else
            {
                tmpMap.insert("value", node.nodeValue());
//                return tmpMap;
            }
        }
        else if (node.isText())
        {
            qDebug() << QString("NodeIsText") << node.nodeValue();
            tmpMap.insert("value", node.nodeValue());
//            return tmpMap;
        }

        node = node.nextSibling();
    }
    if (!tmpMap.isEmpty())
    {
        if (map.isEmpty())
            return tmpMap;
        else
            map += tmpMap;
    }

    return map;
}

bool PListLoader::checkPointAndRect(QString name, QDomElement el, QMultiMap<QString, QVariant>  *map, QMultiMap<QString, QVariant>  *tmpMap)
{
    QMultiMap<QString, QVariant> child = getSubChild(el);
    // parseArray() , parsePoint(), ...
    qDebug() << QString("CHILD TEST RECT") << child << child.value("value");


    bool inserted = false;
    QString value = "";
    if (child.count("value") > 1)
    {
        qDebug() << "IS MAAAAAAAP";
        QList<QVariant> lvc = child.values("value");
        for(int i = 0; i < lvc.count(); i++)
        {
            if (!lvc.at(i).type() == QVariant::String)
            {
                value = "";
                break;
            }
            value += QString(" %1 ").arg(lvc.at(i).toString());
        }
    }

    if (value != "" || child.value("value").type() == QVariant::String)
    {
        if (value == "")
            value = child.value("value").toString();
        QString base = "\\{\\s*(-?\\d*)\\s*,\\s*(-?\\d*)\\s*\\}";

        QRegExp re(base);//"{\\s*"+base+"\\s*,\\s*"+base+"\\s*}");
        int pos = 0;
        qDebug() << "IsString" << value << base << re.pattern();
        pos = re.indexIn(value, pos);
        if (pos != -1)
        {
            //QPoint, QRect, QPointList
            QList<QVariant> pointList;
            QStringList sl = re.capturedTexts();

            qDebug() << pos << sl;
            pointList.append(QPoint(sl.at(1).toInt(), sl.at(2).toInt()));
            pos += re.matchedLength();

            while (( pos = re.indexIn(value, pos)) != -1)
            {
                sl = re.capturedTexts();
                pointList.append(QPoint(sl.at(1).toInt(), sl.at(2).toInt()));
                qDebug() << pos << sl;
                pos += re.matchedLength();
            }
            qDebug() << pointList;
            if (pointList.size() == 1)
            {
                map->insert(name, pointList.at(0).toPoint());
            }
            else if(pointList.size() == 2)
            {
                qDebug() << QString("Rect");
                map->insert(name, QRect(pointList.at(0).toPoint(), QSize(pointList.at(1).toPoint().x(), pointList.at(1).toPoint().y()) ));
            }
            else
            {
                qDebug() << QString("Point Size %1").arg(pointList.size());
                map->insert(name, pointList);
            }
            inserted = true;
        }
        else
        {
            map->insert(name, value);
            inserted = true;
        }

    }
    if (!inserted)
    {
        qDebug() << QString("Non inserted");

        qDebug() << name << child;
        tmpMap->insert(name, child);
    }
    return inserted;
}

QMultiMap<QString, QVariant> PListLoader::get(QString xpath_query)
{
    QDomDocument frag = getDomDocument(xpath_query);

    QDomElement root = frag.documentElement();

    qDebug() << QString("get") << xpath_query;

    if (!root.hasChildNodes() || !root.isElement())
        return QMultiMap<QString, QVariant>();

    qDebug() << QString("GET HAS CHILD");

    QMultiMap<QString, QVariant> map = getSubChild(root.toElement());
    foreach(QString name, map.keys())
    {
        qDebug() << name << map.value(name).toMap().value("value");
    }
    return map;
}

QMultiMap<QString, QVariant> PListLoader::getMetadata()
{
    return get("//key[text()='metadata']/following-sibling::dict");
}
