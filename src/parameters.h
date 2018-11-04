/* This file is part of KDevelop

   Copyright 2016 Anton Anikin <anton.anikin@htower.ru>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef ENCAPSULATEFIELD_PARAMETERS_H
#define ENCAPSULATEFIELD_PARAMETERS_H

#include <util/path.h>

#include "pattern.h"

namespace KDevelop
{
class IProject;
}

namespace EncapsulateField
{

namespace defaults
{

// global settings
static QVector<Pattern> patterns = deserialize(defaultPatterns());

// project settings
static const bool useInline = true;
static const bool useConst = true;

}

class Parameters
{
public:
    explicit Parameters(KDevelop::IProject* project = nullptr);

    // global settings
    QVector<Pattern> patterns;

    // project settings
    bool useInline;
    bool useConst;

    // runtime settings

private:
    KDevelop::IProject* m_project;
};

}
#endif
