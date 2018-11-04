/*
 * Copyright 2015 Laszlo Kis-Adam <laszlo.kis-adam@kdemail.net>
 * Copyright 2016 Anton Anikin <anton.anikin@htower.ru>
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

#include "globalconfigpage.h"
#include "ui_globalconfigpage.h"

#include "globalsettings.h"
#include "patternmodel.h"

#include <QKeyEvent>
#include <iostream>
namespace EncapsulateField
{

GlobalConfigPage::GlobalConfigPage(KDevelop::IPlugin* plugin, QWidget* parent)
    : ConfigPage(plugin, GlobalSettings::self(), parent)
    , m_model(new PatternModel(this))
    , m_ui(new Ui::GlobalConfigPage)
{
    m_ui->setupUi(this);

    m_ui->patterns->setSelectionMode(QAbstractItemView::SingleSelection);
    m_ui->patterns->setModel(m_model);
    m_ui->patterns->setRootIsDecorated(false);
    m_ui->patterns->header()->setSectionResizeMode(PatternModel::Pattern, QHeaderView::Stretch);
    m_ui->patterns->header()->setSectionResizeMode(PatternModel::Getter, QHeaderView::Stretch);
    m_ui->patterns->header()->setSectionResizeMode(PatternModel::Setter, QHeaderView::Stretch);
    m_ui->patterns->installEventFilter(this);
    m_ui->patterns->setDragEnabled(true);
    m_ui->patterns->setDragDropMode(QAbstractItemView::InternalMove);
    m_ui->patterns->setAutoScroll(true);

    selectionChanged();

    connect(m_ui->patterns->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &GlobalConfigPage::selectionChanged);
    connect(this, &GlobalConfigPage::changed, this, &GlobalConfigPage::selectionChanged);
    connect(m_model, &PatternModel::dataChanged, this, &GlobalConfigPage::emitChanged);
    connect(m_model, &PatternModel::rowsInserted, this, &GlobalConfigPage::emitChanged);
    connect(m_model, &PatternModel::rowsRemoved, this, &GlobalConfigPage::emitChanged);
    connect(m_model, &PatternModel::modelReset, this, &GlobalConfigPage::emitChanged);
    connect(m_model, &PatternModel::rowsMoved, this, &GlobalConfigPage::emitChanged);

    connect(m_ui->add, &QPushButton::clicked, this, &GlobalConfigPage::add);
    connect(m_ui->remove, &QPushButton::clicked, this, &GlobalConfigPage::remove);
    connect(m_ui->moveUp, &QPushButton::clicked, this, &GlobalConfigPage::moveUp);
    connect(m_ui->moveDown, &QPushButton::clicked, this, &GlobalConfigPage::moveDown);
}

GlobalConfigPage::~GlobalConfigPage()
{
}

KDevelop::ConfigPage::ConfigPageType GlobalConfigPage::configPageType() const
{
    return KDevelop::ConfigPage::LanguageConfigPage;
}

QString GlobalConfigPage::name() const
{
    return i18n("Encapsulate Field");
}

QString GlobalConfigPage::fullName() const
{
    return i18n("Configure Encapsulate Field Settings");
}

QIcon GlobalConfigPage::icon() const
{
    return QIcon::fromTheme(QStringLiteral("EncapsulateField"));
}

bool GlobalConfigPage::eventFilter(QObject* object, QEvent* event)
{
    if (object == m_ui->patterns && event->type() == QEvent::KeyRelease) {
        auto* key = static_cast<QKeyEvent*>(event);
        if (key->key() == Qt::Key_Delete && key->modifiers() == Qt::NoModifier && m_ui->patterns->currentIndex().isValid()) {
            // workaround https://bugs.kde.org/show_bug.cgi?id=324451
            // there is no other way I see to figure out whether an editor is showing...
            auto* editor = m_ui->patterns->viewport()->findChild<QWidget*>();
            if (!editor || !editor->isVisible()) {
                // editor is not showing
                remove();
                return true; // eat event
            }
        }
    }

    return ConfigPage::eventFilter(object, event);
}

void GlobalConfigPage::selectionChanged()
{
    bool hasSelection = m_ui->patterns->currentIndex().isValid();
    int row = -1;
    if (hasSelection) {
        row = m_ui->patterns->currentIndex().row();
    }
    m_ui->remove->setEnabled(hasSelection);

    m_ui->moveDown->setEnabled(hasSelection && row != m_model->rowCount() - 1);
    m_ui->moveUp->setEnabled(hasSelection && row != 0);
}

void GlobalConfigPage::add()
{
    m_model->insertRows(m_model->rowCount(), 1);
    const QModelIndex index = m_model->index(m_model->rowCount() - 1, PatternModel::Pattern, QModelIndex());
    m_ui->patterns->setCurrentIndex(index);
    m_ui->patterns->edit(index);
}

void GlobalConfigPage::remove()
{
    Q_ASSERT(m_ui->patterns->currentIndex().isValid());
    m_model->removeRows(m_ui->patterns->currentIndex().row(), 1);
}

void GlobalConfigPage::moveUp()
{
    Q_ASSERT(m_ui->patterns->currentIndex().isValid());
    m_model->movePatternUp(m_ui->patterns->currentIndex().row());
}

void GlobalConfigPage::moveDown()
{
    Q_ASSERT(m_ui->patterns->currentIndex().isValid());
    m_model->movePatternDown(m_ui->patterns->currentIndex().row());
}

void GlobalConfigPage::emitChanged()
{
//     checkPattern();

    emit changed();
}

void GlobalConfigPage::apply()
{
    writePatterns(m_model->patterns());
    ConfigPage::apply();
}

void GlobalConfigPage::reset()
{
    m_model->setPatterns(readPatterns());
    ConfigPage::reset();
}

void GlobalConfigPage::defaults()
{
    ConfigPage::defaults();
    m_model->setPatterns(defaultPatterns());
}

}
