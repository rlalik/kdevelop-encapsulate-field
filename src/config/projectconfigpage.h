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

#ifndef ENCAPSULATEFIELD_PROJECT_CONFIG_PAGE_H
#define ENCAPSULATEFIELD_PROJECT_CONFIG_PAGE_H

#include "parameters.h"

#include <interfaces/configpage.h>

namespace KDevelop
{
class IProject;
}

namespace EncapsulateField
{

namespace Ui
{
class ProjectConfigPage;
}

class Parameters;

class ProjectConfigPage: public KDevelop::ConfigPage
{
    Q_OBJECT

public:
    ProjectConfigPage(KDevelop::IPlugin* plugin, KDevelop::IProject* project, QWidget* parent);
    ~ProjectConfigPage() override;

//     KDevelop::ConfigPage::ConfigPageType configPageType() const override;

    QString name() const override;
    QIcon icon() const override;

public Q_SLOTS:
    void reset() override;
    void defaults() override;

private:
    void update();

    QScopedPointer<Ui::ProjectConfigPage> ui;
    QScopedPointer<Parameters> m_parameters;
};

}

#endif
