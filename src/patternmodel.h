/*
 * This file is part of KDevelop
 * Copyright 2013 Milian Wolff <mail@milianw.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ENCAPSULATEFIELD_PATTERNMODEL_H
#define ENCAPSULATEFIELD_PATTERNMODEL_H

#include <QAbstractTableModel>

#include "pattern.h"

namespace EncapsulateField {

class PatternModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit PatternModel(QObject* parent = nullptr);
    ~PatternModel() override;

    SerializedPatterns patterns() const;
    void setPatterns(const SerializedPatterns& patterns);

    void movePatternUp(int row);
    void movePatternDown(int row);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    Qt::DropActions supportedDropActions() const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    QMap< int, QVariant > itemData(const QModelIndex& index) const override;
    bool setItemData(const QModelIndex& index, const QMap< int, QVariant >& roles) override;

    enum Columns {
        Pattern,
        Getter,
        Setter,
        NUM_COLUMNS
    };

private:
    SerializedPatterns m_patterns;
    // workaround a strange behavior in Qt when we try to drop after the last item in the list
    bool m_ignoredLastInsert;
};

}

#endif // ENCAPSULATEFIELD_PATTERNMODEL_H
