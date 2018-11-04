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

#ifndef ENCAPSULATEFIELD_H
#define ENCAPSULATEFIELD_H

#include <interfaces/iplugin.h>
#include <language/duchain/declaration.h>

#include <QObject>

class QAction;
class QActionGroup;
class QMenu;

namespace KDevelop {
class EditorContext;
}

namespace EncapsulateField {

class Plugin : public KDevelop::IPlugin
{
    Q_OBJECT

public:
    // KPluginFactory-based plugin wants constructor with this signature
    Plugin (QObject* parent, const QVariantList& args);

    KDevelop::ContextMenuExtension contextMenuExtension(KDevelop::Context* context, QWidget* parent) override;

    int configPages() const override { return 1; }
    KDevelop::ConfigPage* configPage(int number, QWidget* parent) override;

    int perProjectConfigPages() const override { return 1; }
    KDevelop::ConfigPage* perProjectConfigPage(int number, const KDevelop::ProjectConfigOptions& options, QWidget* parent) override;

private:
    bool formatFunctions(const QString& g_pattern,
                         const QString& s_pattern,
                         const KDevelop::Declaration* decl,
                         const QString& stripped,
                         QString & ret_g,
                         QString & ret_s,
                         QString & short_g,
                         QString & short_s,
                         bool useInline,
                         bool useConst);

private Q_SLOTS:
    void insertFunctions(int line, const QString& indent,
                         const QString& func_g,
                         const QString& func_s);

private:
    QVector<QAction *> items;
    QAction * item_get_set;
    QAction * item_Get_Set;
    QAction * item_direct;
};

}

#endif // ENCAPSULATEFIELD_H
