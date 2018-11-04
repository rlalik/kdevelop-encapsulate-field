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

#include "parameters.h"

#include <globalsettings.h>
#include <projectsettings.h>

#include <interfaces/iplugin.h>
#include <interfaces/iproject.h>
#include <project/interfaces/ibuildsystemmanager.h>
#include <project/projectmodel.h>

#include <KShell>
#include <KLocalizedString>

#include <QFile>
#include <QRegularExpression>

namespace EncapsulateField
{


Parameters::Parameters(KDevelop::IProject* project)
    : m_project(project)
{
    if (!project)
    {
        useInline   = defaults::useInline;
        useConst    = defaults::useConst;

        patterns    = defaults::patterns;
        return;
    }

    ProjectSettings projectSettings;
    projectSettings.setSharedConfig(project->projectConfiguration());
    projectSettings.load();

    useInline   = projectSettings.useInline();
    useConst    = projectSettings.useConst();
    qDebug() << "CONFIG" << useInline << useConst;
}

}
