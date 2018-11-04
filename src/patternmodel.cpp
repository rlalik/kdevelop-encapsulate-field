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

#include "patternmodel.h"

#include <QIcon>

#include <KLocalizedString>

namespace EncapsulateField {

PatternModel::PatternModel(QObject* parent)
    : QAbstractTableModel(parent)
    , m_ignoredLastInsert(false)
{
}

PatternModel::~PatternModel()
{

}

SerializedPatterns PatternModel::patterns() const
{
    return m_patterns;
}

void PatternModel::setPatterns(const SerializedPatterns& patterns)
{
    beginResetModel();
    m_patterns = patterns;
    endResetModel();
}

void PatternModel::movePatternUp(int row)
{
    beginMoveRows(QModelIndex(), row, row, QModelIndex(), row - 1);
    qSwap(m_patterns[row], m_patterns[row - 1]);
    endMoveRows();
}

void PatternModel::movePatternDown(int row)
{
    beginMoveRows(QModelIndex(), row, row, QModelIndex(), row + 2);
    qSwap(m_patterns[row], m_patterns[row + 1]);
    endMoveRows();
}

int PatternModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_patterns.size();
}

int PatternModel::columnCount(const QModelIndex& /*parent*/) const
{
    return NUM_COLUMNS;
}

QVariant PatternModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QVariant();
    }

    Q_ASSERT(section >= 0 && section < NUM_COLUMNS);

    if (section == Pattern) {
        return i18n("Pattern");
    } else if (section == Getter) {
        return i18n("Getter");
    } else if (section == Setter) {
        return i18n("Setter");
    }

    return QVariant();
}

QVariant PatternModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    Q_ASSERT(!index.parent().isValid());
    Q_ASSERT(index.row() >= 0 && index.row() < m_patterns.size());
    Q_ASSERT(index.column() >= 0 && index.column() < NUM_COLUMNS);

    if (role != Qt::DisplayRole && role != Qt::DecorationRole
        && role != Qt::EditRole && role != Qt::ToolTipRole)
    {
        return QVariant();
    }

    const SerializedPattern& pattern = m_patterns.at(index.row());
    const int column = index.column();

    if (column == Pattern) {
        if (role == Qt::DecorationRole) {
            return QVariant();
        } else if (role == Qt::ToolTipRole) {
            return i18n(
                "Define the pattern for the variable name search. Enclose the core variable name within ()."
            );
        }
        return pattern.pattern;
    } else if (column == Getter) {
        if (role == Qt::DecorationRole) {
            return QVariant();
        } else if (role == Qt::ToolTipRole) {
            return i18n(
                "Set prefix for the getter function, use %1 inplace where the core variable name should be used."
            );
        }
        return pattern.getter;
    } else if (column == Setter) {
        if (role == Qt::DecorationRole) {
            return QVariant();
        } else if (role == Qt::ToolTipRole) {
            return i18n(
                "Set prefix for the setter function, use %1 inplace where the core variable name should be used."
            );
        }
        return pattern.setter;
    }

    return QVariant();
}

bool PatternModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid()) {
        return false;
    }
    Q_ASSERT(!index.parent().isValid());
    Q_ASSERT(index.row() >= 0 && index.row() < m_patterns.size());
    Q_ASSERT(index.column() >= 0 && index.column() < NUM_COLUMNS);
    if (role != Qt::EditRole && role != Qt::DisplayRole) {
        return false;
    }
    SerializedPattern& pattern = m_patterns[index.row()];
    const int column = index.column();
    if (column == Pattern) {
        pattern.pattern = value.toString();
    } else if (column == Getter) {
        pattern.getter = value.toString();
    } else if (column == Setter) {
        pattern.setter = value.toString();
    }
    emit dataChanged(index, index);
    return true;
}

Qt::DropActions PatternModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

Qt::ItemFlags PatternModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags baseFlags = QAbstractTableModel::flags(index);
    if (index.isValid() && !index.parent().isValid()) {
        return baseFlags | Qt::ItemIsEditable | Qt::ItemIsDragEnabled;
    }
    return baseFlags | Qt::ItemIsDropEnabled;
}

bool PatternModel::insertRows(int row, int count, const QModelIndex& parent)
{
    Q_ASSERT(!parent.isValid());
    Q_ASSERT(count == 1);
    if (row == -1) {
        // after end of list and we cannot just append either as then the
        // later setData events will fails...
        m_ignoredLastInsert = true;
        return false;
    }
    m_ignoredLastInsert = false;
    Q_ASSERT(row >= 0 && row <= m_patterns.size());
    Q_ASSERT(row + count - 1 <= m_patterns.size());

    beginInsertRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        m_patterns.insert(row + i, SerializedPattern());
    }
    endInsertRows();
    return true;
}

bool PatternModel::removeRows(int row, int count, const QModelIndex& parent)
{
    Q_ASSERT(!parent.isValid());
    Q_ASSERT(count == 1);
    Q_ASSERT(row >= 0 && row < m_patterns.size());
    Q_ASSERT(row + count <= m_patterns.size());

    if (m_ignoredLastInsert) {
        return false;
    }

    beginRemoveRows(parent, row, row + count - 1);
    m_patterns.remove(row, count);
    endRemoveRows();

    return true;
}

QMap<int, QVariant> PatternModel::itemData(const QModelIndex& index) const
{
    QMap<int, QVariant> ret;
    if (!index.isValid()) {
        return ret;
    }
    Q_ASSERT(!index.parent().isValid());
    Q_ASSERT(index.row() >= 0 && index.row() < m_patterns.size());
    const SerializedPattern& pattern = m_patterns.at(index.row());
    ret.insert(Qt::UserRole + Pattern, pattern.pattern);
    ret.insert(Qt::UserRole + Getter, pattern.getter);
    ret.insert(Qt::UserRole + Setter, pattern.setter);
    return ret;
}

bool PatternModel::setItemData(const QModelIndex& index, const QMap< int, QVariant >& roles)
{
    Q_ASSERT(index.isValid());
    Q_ASSERT(!index.parent().isValid());
    Q_ASSERT(index.row() >= 0 && index.row() < m_patterns.size());
    Q_ASSERT(roles.size() == 3);
    Q_ASSERT(roles.contains(Qt::UserRole + Pattern));
    Q_ASSERT(roles.contains(Qt::UserRole + Getter));
    Q_ASSERT(roles.contains(Qt::UserRole + Setter));

    if (m_ignoredLastInsert) {
        return false;
    }

    SerializedPattern& pattern = m_patterns[index.row()];
    pattern.pattern = roles[Qt::UserRole + Pattern].toString();
    pattern.getter = roles[Qt::UserRole + Getter].toString();
    pattern.setter = roles[Qt::UserRole + Setter].toString();
    return true;
}

}
