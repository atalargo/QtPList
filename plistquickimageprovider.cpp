/*
 * plistquickimageprovider.cpp
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
#include "plistquickimageprovider.h"

#include <QDebug>

QImage PListQuickImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    qDebug() << "Query image " << id;
    QImage wantedImage = pli->getImage(id);
    QSize originalSize = QSize(wantedImage.width(), wantedImage.height()) - QSize(wantedImage.offset().x() * -2, wantedImage.offset().y()*-2);
    if (size)
        *size = originalSize;
    qDebug() << QString("Image found ? %1").arg(wantedImage != QImage());

    qDebug() << QString("original size %1 - %2 requested size %3 - %4").arg(size->width()).arg(size->height()).arg(requestedSize.width()).arg(requestedSize.height());
    if (requestedSize.width() != -1 && originalSize != requestedSize)
        return wantedImage.scaled(requestedSize, Qt::KeepAspectRatio);

    return wantedImage;
}
