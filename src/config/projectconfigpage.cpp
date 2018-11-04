// Copyright (c) 2018 Rafał Lalik <rafallalik@gmail.com>
// 
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#include "projectconfigpage.h"
#include "ui_projectconfigpage.h"

#include "projectsettings.h"

#include <interfaces/iproject.h>

namespace EncapsulateField
{

ProjectConfigPage::ProjectConfigPage(KDevelop::IPlugin* plugin, KDevelop::IProject* project, QWidget* parent)
    : ConfigPage(plugin, new ProjectSettings, parent)
    , ui(new Ui::ProjectConfigPage)
    , m_parameters(new Parameters(project))
{
    configSkeleton()->setSharedConfig(project->projectConfiguration());
    configSkeleton()->load();

    ui->setupUi(this);

    connect(this, &ProjectConfigPage::changed, this, &ProjectConfigPage::update);

    qDebug() << "CONFIG PAGE";
}

ProjectConfigPage::~ProjectConfigPage()
{
}

// KDevelop::ConfigPage::ConfigPageType ProjectConfigPage::configPageType() const
// {
//     return KDevelop::ConfigPage::LanguageConfigPage;
// }

QString ProjectConfigPage::name() const
{
    return i18n("Encapsulate Field");
}

QIcon ProjectConfigPage::icon() const
{
    return QIcon::fromTheme(QStringLiteral("EncapsulateField"));
}

void ProjectConfigPage::reset()
{
    ConfigPage::reset();
    update();

    qDebug() << "RESET" << m_parameters->useInline << m_parameters->useConst;
}

void ProjectConfigPage::defaults()
{
    ConfigPage::defaults();
    update();

    qDebug() << "DEFAULTS" << m_parameters->useInline << m_parameters->useConst;
}

void ProjectConfigPage::update()
{
    m_parameters->useInline = ui->kcfg_useInline->isChecked();
    m_parameters->useConst = ui->kcfg_useConst->isChecked();

    qDebug() << "UPDATE" << m_parameters->useInline << m_parameters->useConst;
}

}
