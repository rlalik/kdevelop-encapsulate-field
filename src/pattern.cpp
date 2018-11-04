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

#include "pattern.h"

#include <KConfigGroup>

namespace EncapsulateField {

Pattern::Pattern(const SerializedPattern& p)
    : pattern(p.pattern)
    , getter(p.getter)
    , setter(p.setter)
{
}

SerializedPattern::SerializedPattern(const QString& pattern, const QString& getter, const QString& setter)
    : pattern(pattern)
    , getter(getter)
    , setter(setter)
{
}

SerializedPatterns defaultPatterns()
{
    SerializedPatterns ret;
    ret.reserve(20);

    // filter hidden files
    ret << SerializedPattern(QStringLiteral("[a-z]([A-Z].+)"),
                             QStringLiteral("get%1"),
                             QStringLiteral("set%1"));
    ret << SerializedPattern(QStringLiteral("[a-z]([A-Z].+)"),
                             QStringLiteral("Get%1"),
                             QStringLiteral("Set%1"));
    ret << SerializedPattern(QStringLiteral("([a-z]+[A-Z].+)"),
                             QStringLiteral("get%1"),
                             QStringLiteral("set%1"));
    ret << SerializedPattern(QStringLiteral("([a-z]+[A-Z].+)"),
                             QStringLiteral("Get%1"),
                             QStringLiteral("Set%1"));
    ret << SerializedPattern(QStringLiteral("[a-z]_([a-zA-Z0-9].+)"),
                             QStringLiteral("%1"),
                             QStringLiteral("%1"));

    return ret;
}

SerializedPatterns readPatterns()
{
    auto config = KSharedConfig::openConfig();
    if (!config->hasGroup("Patterns")) return defaultPatterns();

    const KConfigGroup& group = config->group("Patterns");
    const int size = group.readEntry("size", -1);

    if (size == -1) return defaultPatterns();   // fallback

    SerializedPatterns patterns;
    patterns.reserve(size);
    for (int i = 0; i < size; ++i) {
        const QByteArray subGroup = QByteArray::number(i);
        if (!group.hasGroup(subGroup)) continue;

        const KConfigGroup& subConfig = group.group(subGroup);
        const QString pattern = subConfig.readEntry("pattern", QString());
        const QString getter = subConfig.readEntry("getter", QString());
        const QString setter = subConfig.readEntry("setter", QString());
        patterns << SerializedPattern(pattern, getter, setter);
    }

    return patterns;
}

void writePatterns(const SerializedPatterns& patterns)
{
    auto config = KSharedConfig::openConfig();

    // clear existing
    config->deleteGroup("Patterns");

    // write new
    KConfigGroup group = config->group("Patterns");
    group.writeEntry("size", patterns.size());
    int i = 0;
    for (const SerializedPattern& pattern : patterns)
    {
        KConfigGroup subGroup = group.group(QByteArray::number(i++));
        subGroup.writeEntry("pattern", pattern.pattern);
        subGroup.writeEntry("getter", pattern.getter);
        subGroup.writeEntry("setter", pattern.setter);
    }
    config->sync();
}

Patterns deserialize(const SerializedPatterns& patterns)
{
    Patterns ret;
    ret.reserve(patterns.size());
    for (const SerializedPattern& pattern : patterns)
        ret << Pattern(pattern);

    return ret;
}

}
