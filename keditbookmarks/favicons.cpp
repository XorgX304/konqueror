/* This file is part of the KDE project
   Copyright (C) 2002-2003 Alexander Kellett <lypanov@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kdebug.h>
#include <klocale.h>
#include <kapplication.h>

#include <kio/job.h>

#include <kparts/part.h>
#include <kparts/componentfactory.h>
#include <kparts/browserextension.h>

#include "bookmarkiterator.h"
#include "listview.h"
#include "toplevel.h"

#include "favicons.h"

FavIconsItrHolder *FavIconsItrHolder::s_self = 0;

FavIconsItrHolder::FavIconsItrHolder() 
   : BookmarkIteratorHolder() {
   // do stuff
}

void FavIconsItrHolder::doItrListChanged() {
   KEBApp::self()->setCancelFavIconUpdatesEnabled(m_itrs.count() > 0);
}

/* -------------------------- */

FavIconsItr::FavIconsItr(QValueList<KBookmark> bks)
   : BookmarkIterator(bks) {

   m_updater = 0;
   m_done = false;
}

FavIconsItr::~FavIconsItr() {
   delete m_updater;
   if (!m_done && curItem()) {
      curItem()->restoreStatus();
   }
}

void FavIconsItr::slotDone(bool succeeded) {
   kdDebug() << "FavIconsItr::slotDone()" << endl;
   m_done = true;
   curItem()->setTmpStatus(succeeded ? i18n("OK") : i18n("No favicon found"));
   delayedEmitNextOne();
}

bool FavIconsItr::isBlahable(const KBookmark &bk) {
   return (!bk.isGroup() && !bk.isSeparator());
}

void FavIconsItr::doBlah() {
   kdDebug() << "FavIconsItr::doBlah()" << endl;
   m_done = false;
   curItem()->setTmpStatus(i18n("Updating favicon..."));
   if (!m_updater) {
      m_updater = new FavIconUpdater(kapp, "FavIconUpdater");
      connect(m_updater, SIGNAL( done(bool) ),
              this,      SLOT( slotDone(bool) ) );
   }
   m_updater->downloadIcon(m_book);
   // TODO - a single shot timeout?
}

/* -------------------------- */

FavIconUpdater::FavIconUpdater(QObject *parent, const char *name)
   : KonqFavIconMgr(parent, name) {
   ;
}

void FavIconUpdater::slotCompleted() {
   kdDebug() << "FavIconUpdater::slotCompleted" << endl;
   // kdDebug() << "emit done(true)" << endl;
   emit done(true);
}

void FavIconUpdater::downloadIcon(const KBookmark &bk) {
   QString favicon = KonqFavIconMgr::iconForURL(bk.url().url());
   if (!favicon.isNull()) {
      // kdDebug() << "downloadIcon() - favicon" << favicon << endl;
      bk.internalElement().setAttribute("icon", favicon);
      // kdDebug() << "favicon - emitSlotCommandExecuted()" << favicon << endl;
      KEBApp::self()->emitSlotCommandExecuted();
      // kdDebug() << "emit done(true)" << endl;
      emit done(true);

   } else {
      KonqFavIconMgr::downloadHostIcon(bk.url());
      favicon = KonqFavIconMgr::iconForURL(bk.url().url());
      // kdDebug() << "favicon == " << favicon << endl;;
      if (favicon.isNull()) {
         downloadIconActual(bk);
      }
   }
}

void FavIconUpdater::downloadIconActual(const KBookmark &bk) {
   m_bk = bk;

   KParts::ReadOnlyPart *part 
      = KParts::ComponentFactory
              ::createPartInstanceFromQuery<KParts::ReadOnlyPart>("text/html", QString::null);

   part->setProperty("pluginsEnabled", QVariant(false, 1));
   part->setProperty("javaScriptEnabled", QVariant(false, 1));
   part->setProperty("javaEnabled", QVariant(false, 1));
   part->setProperty("autoloadImages", QVariant(false, 1));

   m_part = part;

   connect(part, SIGNAL( canceled(const QString &) ),
           this, SLOT( slotCompleted() ));
   connect(part, SIGNAL( completed() ),
           this, SLOT( slotCompleted() ));

   KParts::BrowserExtension *ext = KParts::BrowserExtension::childObject(m_part);
   if (!ext) {
      return;
   }

   m_browserIface = new FavIconBrowserInterface(this, "browseriface");
   ext->setBrowserInterface(m_browserIface);

   connect(ext, SIGNAL( setIconURL(const KURL &) ),
           this, SLOT( setIconURL(const KURL &) ));

   new FavIconWebGrabber(part, bk.url());
}

// khtml callback
void FavIconUpdater::setIconURL(const KURL &iconURL) {
   setIconForURL(m_bk.url(), iconURL);
}

void FavIconUpdater::notifyChange(bool isHost, QString hostOrURL, QString iconName) {
   kdDebug() << "FavIconUpdater::notifyChange()" << endl;

   Q_UNUSED(isHost);
   // kdDebug() << isHost << endl;;
   Q_UNUSED(hostOrURL);
   // kdDebug() << hostOrURL << "==" << m_bk.url().url() << "-> " << iconName << endl;

   m_bk.internalElement().setAttribute("icon", iconName);
   KEBApp::self()->emitSlotCommandExecuted();
}

/* -------------------------- */

FavIconWebGrabber::FavIconWebGrabber(KParts::ReadOnlyPart *part, const KURL &url)
  : m_part(part), m_url(url) {

   kdDebug() << "FavIconWebGrabber::FavIconWebGrabber starting KIO::get()" << endl;

   // the use of KIO rather than directly using KHTML is to allow silently abort on error

   KIO::Job *job = KIO::get(m_url, false, false);
   connect(job, SIGNAL( result( KIO::Job *)),
           this, SLOT( slotFinished(KIO::Job *) ));
   connect(job, SIGNAL( mimetype( KIO::Job *, const QString &) ),
           this, SLOT( slotMimetype(KIO::Job *, const QString &) ));
}

void FavIconWebGrabber::slotMimetype(KIO::Job *job, const QString & /*type*/) {
   KIO::SimpleJob *sjob = static_cast<KIO::SimpleJob *>(job);
   m_url = sjob->url(); // allow for redirection
   sjob->putOnHold();

   // QString typeLocal = typeUncopied; // local copy
   // kdDebug() << "slotMimetype : " << typeLocal << endl;
   // TODO - what to do if typeLocal is not text/html ??

   m_part->openURL(m_url);
}

void FavIconWebGrabber::slotFinished(KIO::Job *job) {
   if (job->error()) {
      kdDebug() << "FavIconWebGrabber::slotFinished() " << job->errorString() << endl;
   }
}

#include "favicons.moc"
