/*
 * plistloader.h
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

#ifndef PLISTLOADER_H
#define PLISTLOADER_H

#include <QObject>
#include <QVariant>
#include <QMap>
#include <QImage>
#include <QtXml/QDomDocument>

// TODO: Voir QDeclarativeImageProvider pour peut être changer le type de fonctionnement

class PListLoader : public QObject
{
    Q_OBJECT
public:
    explicit PListLoader(QObject *parent = 0);
    explicit PListLoader(QString filename, QObject *parent = 0);

    bool loadFile(QString filename);

    Q_INVOKABLE QMultiMap<QString, QVariant> get(QString xpath_query);
    Q_INVOKABLE QDomDocument getDomDocument(QString xpath_query);

    Q_INVOKABLE QImage getImage(QString name);

    Q_INVOKABLE QMultiMap<QString, QVariant> getMetadata();

    Q_INVOKABLE QMultiMap<QString, QVariant> getFrames();

    int getErrorLine() { return errorLine;}
    int gerErrorColumn() {return errorColumn;}
    QString getErrorMessage() { return errorMsg;}

signals:
    
public slots:
    
protected:
    QMultiMap<QString, QVariant> frames;
    QString imageFilePathName;

    QImage loadedImage;
    QDomDocument document;

    QMultiMap<QString, QVariant>  getSubChild(QDomElement el);

    QVariant getValue(QVariant parent, QString valName);

    bool checkPointAndRect(QString name, QDomElement el, QMultiMap<QString, QVariant> *map, QMultiMap<QString, QVariant>  *tmpMap);

    int errorLine;
    int errorColumn;
    QString errorMsg;
};

#endif // PLISTLOADER_H
