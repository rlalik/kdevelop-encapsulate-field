/*
 * Copyright 2018 Rafał Lalik <rafallalik@kgmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ENCAPSULATEFIELD_GLOBAL_CONFIG_PAGE_H
#define ENCAPSULATEFIELD_GLOBAL_CONFIG_PAGE_H

#include <interfaces/configpage.h>

namespace EncapsulateField
{

namespace Ui {
class GlobalConfigPage;
}

class PatternModel;

class GlobalConfigPage: public KDevelop::ConfigPage
{
    Q_OBJECT

public:
    GlobalConfigPage(KDevelop::IPlugin* plugin, QWidget* parent);
    ~GlobalConfigPage() override;

    KDevelop::ConfigPage::ConfigPageType configPageType() const override;

    QString name() const override;
    QString fullName() const override;
    QIcon icon() const override;

protected:
    bool eventFilter(QObject* object, QEvent* event) override;

private Q_SLOTS:
    void add();
    void remove();
    void moveUp();
    void moveDown();
    void selectionChanged();
    void emitChanged();

public Q_SLOTS:
    void apply() override;
    void reset() override;
    void defaults() override;

private:
    PatternModel * m_model;
    QScopedPointer<Ui::GlobalConfigPage> m_ui;
};

}

#endif
