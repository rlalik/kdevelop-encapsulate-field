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

#include "encapsulatefield.h"

#include "config/globalconfigpage.h"
#include "config/projectconfigpage.h"
#include "pattern.h"

#include <interfaces/configpage.h>
#include <interfaces/context.h>
#include <interfaces/contextmenuextension.h>
#include <interfaces/icore.h>
#include <interfaces/idocumentcontroller.h>
#include <interfaces/iprojectcontroller.h>
#include <interfaces/iuicontroller.h>
#include <language/duchain/declaration.h>
#include <language/duchain/duchainlock.h>
#include <language/duchain/functiondeclaration.h>
#include <language/duchain/types/structuretype.h>
#include <language/editor/documentrange.h>
#include <language/interfaces/codecontext.h>
#include <language/interfaces/editorcontext.h>
#include <project/projectconfigpage.h>
#include <projectsettings.h>
#include <debug.h>

#include <KActionCollection>
#include <KLocalizedString>
#include <KPluginFactory>

#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QMimeDatabase>
#include <QObject>
#include <QRegExp>

#include <cstdio>

K_PLUGIN_FACTORY_WITH_JSON(EncapsulateFieldFactory, "kdevencapsulatefield.json",
                           registerPlugin<EncapsulateField::Plugin>(); )

namespace EncapsulateField {

Plugin::Plugin (QObject *parent, const QVariantList& args)
    : KDevelop::IPlugin(QStringLiteral("kdevencapsulatefield"), parent)
{
    Q_UNUSED(args);
}

static
bool isSupportedMimeType(const QMimeType& mimeType)
{
    const QString mimeName = mimeType.name();
    return (mimeName == QLatin1String("text/x-c++src") ||
            mimeName == QLatin1String("text/x-c++hdr") ||
            mimeName == QLatin1String("text/x-chdr")   ||
            mimeName == QLatin1String("text/x-csrc"));
}

static bool isVariableDeclaration(KDevelop::Declaration * decl)
{
    if (nullptr == decl)
        return false;

    if (decl->isFunctionDeclaration())  return false;
    if (decl->isForwardDeclaration())   return false;
    if (decl->isDefinition())           return false;
    if (decl->isTypeAlias())            return false;

    return true;
}
KDevelop::ContextMenuExtension Plugin::contextMenuExtension(KDevelop::Context* context, QWidget* parent)
{
    KDevelop::ContextMenuExtension extension = KDevelop::IPlugin::contextMenuExtension(context, parent);

    auto* codeContext = dynamic_cast<KDevelop::DeclarationContext*>(context);

    if (!codeContext) return extension;

    if (context->hasType(KDevelop::Context::EditorContext))
    {
        auto eContext = static_cast<KDevelop::EditorContext*>(context);
        QMimeDatabase db;
        const auto mime = db.mimeTypeForUrl(eContext->url());
        if (!isSupportedMimeType(mime)) return extension;

        KDevelop::DUChainReadLocker readLock;
        KDevelop::Declaration* decl(codeContext->declaration().data());

        if (nullptr == decl || !isVariableDeclaration(decl)) return extension;

        KDevelop::DocumentRange dr = codeContext->use();
        if (!dr.isValid()) return extension;
        if (!dr.onSingleLine()) return extension;

        int lineno = dr.start().line();
        QString line = eContext->currentLine();
        int pos = line.indexOf(QRegExp("[a-zA-Z/#]"));
        QString indent = line.left(pos);

        KDevelop::Identifier ident = decl->identifier();
        QString stripped_var;

        KDevelop::IProject * proj = KDevelop::ICore::self()->projectController()->findProjectForUrl(decl->url().toUrl());
        Parameters pars(proj);

        bool use_inline = pars.useInline;
        bool use_const = pars.useConst;

        foreach (QAction *a, items)
        {
            a->disconnect();
            delete a;
        }
        items.clear();

        QMenu * menu_encaps = new QMenu(i18n("Encapsulate Field"), parent);
        menu_encaps->setToolTipsVisible(true);

        bool r = false;
        Patterns newPatterns = deserialize(readPatterns());
        for (const Pattern& p : newPatterns)
        {
            QRegExp regexp(p.pattern);
            bool ret = regexp.exactMatch(ident.toString());
            if (ret)
            {
                stripped_var = regexp.cap(1);
                QString r_g, r_s, s_g, s_s;
                r = formatFunctions(p.getter, p.setter,
                                         decl,
                                         stripped_var,
                                         r_g, r_s,
                                         s_g, s_s,
                                         use_inline, use_const);
                QAction * item = new QAction;
                item->setText(s_g + " / " + s_s);
                QString tt_string = r_g + "\n";
                if (r_s.length())
                    tt_string += r_s;
                else
                    tt_string += "-- constant variables have no setter function --";
                item->setToolTip(tt_string);
                connect(item, &QAction::triggered, [=]
                { insertFunctions(lineno+1, indent, r_g, r_s); });
                items.push_back(item);
                menu_encaps->addAction(item);
            }
        }

        if (!r)     return extension;

        extension.addAction(KDevelop::ContextMenuExtension::EditGroup,
                            menu_encaps->menuAction());
    }

    return extension;
}

QString stripHigherLevelContext(const KDevelop::DUContext * ctx, const QString & name)
{
    QString ret;
    auto owner = ctx->owner();

    if (nullptr != owner)
        ret = stripHigherLevelContext(ctx->parentContext(), name);
    else
        return name;

    if (ret.startsWith(ctx->owner()->identifier().toString()))
    {
        auto len_r = ret.length();
        auto len_i = ctx->owner()->identifier().toString().length();
        if (len_i < (len_r-2))
        {
            return ret.right(len_r-len_i-2);
        }
    }

    return ret;
}

bool Plugin::formatFunctions(const QString& g_pattern,
                             const QString& s_pattern,
                             const KDevelop::Declaration* decl,
                             const QString& stripped,
                             QString & ret_g, QString & ret_s,
                             QString & short_g, QString & short_s,
                             bool useInline, bool useConst)
{
    QString mod_g, mod_s;
    if (useInline)
    {
        mod_g = "inline ";
        mod_s = "inline void ";
    }

    KDevelop::AbstractType::Ptr at = decl->abstractType();

    auto ctx = decl->context();

    QString type;
    auto wt = at->whichType();
    switch (wt)
    {
        case KDevelop::AbstractType::TypeIntegral:
        case KDevelop::AbstractType::TypePointer:
            type = at->toString();
            break;
        case KDevelop::AbstractType::TypeReference:
            type = "const " + at->toString();
            break;
        case KDevelop::AbstractType::TypeStructure:
        {
            KDevelop::StructureType::Ptr st = KDevelop::StructureType::Ptr::dynamicCast(at);

            auto qi = st->qualifiedIdentifier();
            QString sqi = stripHigherLevelContext(ctx, qi.toString());

            type = "const " + sqi + "&";
            break;
        }
        default:
            return false;
            break;
    }
    ret_g = mod_g + type + " " + g_pattern.arg(stripped)
          + "()" + (useConst ? " const" : "")
          + " { return " + decl->identifier().toString() + "; }";
    short_g = type + " " + g_pattern.arg(stripped)
            + "()" + (useConst ? " const" : "");

    quint32 modifiers = at->modifiers();
    if (!(modifiers & KDevelop::AbstractType::ConstModifier))
    {
        ret_s = mod_s + s_pattern.arg(stripped) + "(" + type + " val) { "
        + decl->identifier().toString() + " = val; }";
        short_s = s_pattern.arg(stripped) + "(" + type + " val)";
    }

    return true;
}

void Plugin::insertFunctions(int line, const QString& indent,
                             const QString& func_g,
                             const QString& func_s)
{
    KDevelop::IDocument* doc = core()->documentController()->activeDocument();
    KTextEditor::Document* kteDoc = doc->textDocument();
    if (kteDoc)
    {
        kteDoc->insertLine(line, indent+func_g);
        if (func_s.length())
            kteDoc->insertLine(line+1, indent+func_s);
    }
}

KDevelop::ConfigPage * Plugin::configPage(int number, QWidget* parent)
{
    return number ? nullptr : new EncapsulateField::GlobalConfigPage(this, parent);
}

KDevelop::ConfigPage* Plugin::perProjectConfigPage(int number, const KDevelop::ProjectConfigOptions& options, QWidget* parent)
{
    return number ? nullptr : new EncapsulateField::ProjectConfigPage(this, options.project, parent);
}

}

// needed for QObject class created from K_PLUGIN_FACTORY_WITH_JSON
#include "encapsulatefield.moc"
