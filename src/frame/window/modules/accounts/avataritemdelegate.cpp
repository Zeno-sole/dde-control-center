/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "avataritemdelegate.h"
#include "avatarlistwidget.h"

#include <DStyle>

#include <QObject>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QPainter>
#include <QPixmap>
#include <QPen>
#include <QSize>
#include <QRect>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE::accounts;

AvatarItemDelegate::AvatarItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void AvatarItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->setRenderHints(painter->renderHints()
                            | QPainter::Antialiasing
                            | QPainter::SmoothPixmapTransform);

    if (!index.isValid())
        return;

    auto pm = static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth);
    int borderWidth = option.widget->style()->pixelMetric(pm, &option, nullptr);
    pm = static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderSpacing);
    int borderSpacing = option.widget->style()->pixelMetric(pm, &option, nullptr);
    const QMargins margins(borderWidth + borderSpacing, borderWidth + borderSpacing,
                           borderWidth + borderSpacing, borderWidth + borderSpacing);
    QPixmap pixmap = index.data(Qt::DecorationRole).value<QPixmap>();
    QPainterPath path;
    path.addEllipse(option.rect.marginsRemoved(margins));
    painter->setClipPath(path);

    if (!pixmap.isNull()) {
        painter->drawPixmap(option.rect.marginsRemoved(margins), pixmap);
        painter->setClipping(false);
    } else {
        QString iconpath = index.data(AvatarListWidget::AddAvatarRole).value<LastItemData>().iconPath;
        //画+号
        QRect rect(0, 0, option.rect.width() / 3.0, option.rect.height() / 3.0);
        rect.moveCenter(QRect(option.rect).center());
        painter->setClipPath(path);
        painter->setClipping(true);
        QPen pen(Qt::transparent);
        pen.setColor(Qt::white);
        painter->setPen(pen);
        painter->setBrush(QColor("#E3E3E3"));
        painter->drawEllipse(option.rect.marginsRemoved(margins));
        painter->setClipping(false);
        painter->setPen(QPen(option.palette.text(), 1.0));
        painter->drawLine(QPointF(rect.x(), rect.center().y()), QPointF(rect.right(), rect.center().y()));
        painter->drawLine(QPointF(rect.center().x(), rect.y()), QPointF(rect.center().x(), rect.bottom()));
    }

    if (index.data(Qt::CheckStateRole) == Qt::Checked) {
        painter->setPen(QPen(option.palette.highlight(), borderWidth));
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(option.rect.adjusted(1, 1, -1, -1));

        //在中间绘制选中小图标
        QStyleOptionViewItem opt(option);
        initStyleOption(&opt, index);
        int radius = 8;
        int cx = option.rect.marginsRemoved(margins).center().x();
        int cy = option.rect.marginsRemoved(margins).center().y();
        QRect crect(QPoint(cx - radius, cy - radius), QPoint(cx + radius, cy + radius));
        opt.rect = crect;
        opt.state |= QStyle::State_On;
        option.widget->style()->drawPrimitive(DStyle::PE_IndicatorItemViewItemCheck, &opt, painter, nullptr);
        return;
    }
    // draw + in the end
}

QSize AvatarItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    return index.data(Qt::SizeHintRole).toSize();
}
