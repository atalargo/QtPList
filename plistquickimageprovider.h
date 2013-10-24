/*
 * plistquickimageprovider.h
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

#ifndef PLISTQMLIMAGEPROVIDER_H
#define PLISTQMLIMAGEPROVIDER_H

#include <QQuickImageProvider>

#include "plistloader.h"

class PListQuickImageProvider : public QQuickImageProvider
{
public:
    PListQuickImageProvider() : QQuickImageProvider(QQuickImageProvider::Image)
    {
    }

    PListQuickImageProvider(PListLoader *plistloader) : QQuickImageProvider(QQuickImageProvider::Image)
    {
        setPListLoader(plistloader);
    }

    void setPListLoader(PListLoader *plistloader) {
        pli = plistloader;
    }

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

protected:
    PListLoader *pli;
};

#endif // PLISTQMLIMAGEPROVIDER_H
