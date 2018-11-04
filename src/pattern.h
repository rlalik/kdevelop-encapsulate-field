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

#ifndef ENCAPSULATEFIELD_PATTERN_H
#define ENCAPSULATEFIELD_PATTERN_H

#include <QRegExp>
#include <QString>
#include <QVector>
#include <KSharedConfig>

namespace EncapsulateField {

struct SerializedPattern;

/**
 * The Pattern is a the class which is used for actual matching against items.
 *
 * It "compiles" serialized filters for performance and extracts useful information.
 */
struct Pattern
{
public:
    Pattern() {}
    Pattern(const SerializedPattern& pattern);

    bool operator==(const Pattern& pattern) const
    {
        return pattern.pattern == this->pattern
            && pattern.setter == this->setter
            && pattern.getter == this->getter;
    }

    QRegExp pattern;
    QString getter;
    QString setter;
};

using Patterns = QVector<Pattern>;

/**
 * SerializedPattern is what gets stored on disk in the configuration and represents
 * the interface which the user can interact with.
 */
struct SerializedPattern
{
    SerializedPattern() {}
    SerializedPattern(const QString& pattern, const QString& getter, const QString& setter);
    QString pattern;
    QString getter;
    QString setter;
};

using SerializedPatterns = QVector<SerializedPattern>;

SerializedPatterns defaultPatterns();
SerializedPatterns readPatterns(/*const KSharedConfigPtr& config*/);
void writePatterns(const SerializedPatterns& patterns/*, KSharedConfigPtr config*/);
Patterns deserialize(const SerializedPatterns& patterns);

}

Q_DECLARE_TYPEINFO(EncapsulateField::Pattern, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(EncapsulateField::SerializedPattern, Q_MOVABLE_TYPE);

#endif // ENCAPSULATEFIELD_PATTERN_H
