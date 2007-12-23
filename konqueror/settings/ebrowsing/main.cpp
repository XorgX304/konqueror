/*
 * main.cpp
 *
 * Copyright (c) 2000 Yves Arrouye <yves@realnames.com>
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

// Own
#include "main.h"

// std
#include <unistd.h>

// Qt
#include <QtGui/QLayout>
#include <QtCore/QMap>
#include <QtGui/QTabWidget>
#include <QtGui/QBoxLayout>
#include <klocale.h>
#include <kservice.h>
#include <kservicetypetrader.h>

// KDE
#include <kdialog.h>
#include <kurifilter.h>

// Local
#include "filteropts.h"
#include <KPluginFactory>
#include <KPluginLoader>

K_PLUGIN_FACTORY(KURIFactory,
        registerPlugin<KURIFilterModule>();
        )
K_EXPORT_PLUGIN(KURIFactory("kcmkurifilt"))

class FilterOptions;

KURIFilterModule::KURIFilterModule(QWidget *parent, const QVariantList &)
    : KCModule(KURIFactory::componentData(), parent),
    m_widget(0)
{

    filter = KUriFilter::self();

    setQuickHelp( i18n("<h1>Enhanced Browsing</h1> In this module you can configure some enhanced browsing"
      " features of KDE. <h2>Internet Keywords</h2>Internet Keywords let you"
      " type in the name of a brand, a project, a celebrity, etc... and go to the"
      " relevant location. For example you can just type"
      " \"KDE\" or \"K Desktop Environment\" in Konqueror to go to KDE's homepage."
      "<h2>Web Shortcuts</h2>Web Shortcuts are a quick way of using Web search engines. For example, type \"altavista:frobozz\""
      " or \"av:frobozz\" and Konqueror will do a search on AltaVista for \"frobozz\"."
      " Even easier: just press Alt+F2 (if you have not"
      " changed this shortcut) and enter the shortcut in the KDE Run Command dialog."));

    QVBoxLayout *layout = new QVBoxLayout(this);

#if 0
    opts = new FilterOptions(this);
    tab->addTab(opts, i18n("&Filters"));
    connect(opts, SIGNAL(changed(bool)), SIGNAL(changed(bool)));
#endif

    QMap<QString,KCModule*> helper;
    // Load the plugins. This saves a public method in KUriFilter just for this.
    const KService::List offers = KServiceTypeTrader::self()->query( "KUriFilter/Plugin" );
    KService::List::ConstIterator it = offers.begin();
    const KService::List::ConstIterator end = offers.end();
    for (; it != end; ++it )
    {
        KUriFilterPlugin *plugin = KService::createInstance<KUriFilterPlugin>( *it );
        if (plugin) {
            KCModule *module = plugin->configModule(this, 0);
            if (module) {
                modules.append(module);
                helper.insert(plugin->configName(), module);
                connect(module, SIGNAL(changed(bool)), SIGNAL(changed(bool)));
            }
        }
    }

    if (modules.count() > 1)
    {
        QTabWidget *tab = new QTabWidget(this);

        QMap<QString,KCModule*>::iterator it2;
        for (it2 = helper.begin(); it2 != helper.end(); ++it2)
        {
            tab->addTab(it2.value(), it2.key());
        }

        tab->showPage(modules.first());
        m_widget = tab;
    }
    else if (modules.count() == 1)
    {
        m_widget = modules.first();
        if (m_widget->layout())
        {
            m_widget->layout()->setMargin(0);
        }
    }

    if (m_widget) {
        layout->addWidget(m_widget);
    }
}

void KURIFilterModule::load()
{
    foreach( KCModule* module, modules )
    {
	  module->load();
    }
}

void KURIFilterModule::save()
{
    foreach( KCModule* module, modules )
    {
	  module->save();
    }
}

void KURIFilterModule::defaults()
{
    foreach( KCModule* module, modules )
    {
	  module->defaults();
    }
}

KURIFilterModule::~KURIFilterModule()
{
    qDeleteAll( modules );
}

#include "main.moc"
