/* This file is part of the KDE libraries
   Copyright (C) 2000 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kbookmark.h"
#include <kdebug.h>
#include <kglobal.h>
#include <kmimetype.h>
#include <krun.h>
#include <kstddirs.h>
#include <kstringhandler.h>
#include <kurl.h>
#include <klineeditdlg.h>
#include <qtextstream.h>
#include <klocale.h>
#include <assert.h>

KBookmarkGroup::KBookmarkGroup()
 : KBookmark( QDomElement() )
{
}

KBookmarkGroup::KBookmarkGroup( QDomElement elem )
 : KBookmark(elem)
{
}

QString KBookmarkGroup::groupAddress() const
{
    if (m_address.isEmpty())
        m_address = address();
    assert(!m_address.isEmpty());
    return m_address;
}

KBookmark KBookmarkGroup::first() const
{
    QDomElement firstChild = element.firstChild().toElement();
    if ( firstChild.tagName() == "TEXT" )
        firstChild = firstChild.nextSibling().toElement();
    return KBookmark(firstChild);
}

KBookmark KBookmarkGroup::next( const KBookmark & current ) const
{
    return KBookmark(current.element.nextSibling().toElement());
}

KBookmarkGroup KBookmarkGroup::createNewFolder( const QString & text )
{
    QString txt( text );
    if ( text.isEmpty() )
    {
        KLineEditDlg l( i18n("New Folder:"), "", 0L );
        l.setCaption( i18n("Create new bookmark folder in %1").arg( parentGroup().text() ) );
        if ( l.exec() )
            txt = l.text();
        else
            return KBookmarkGroup();
    }

    ASSERT(!element.isNull());
    QDomDocument doc = element.ownerDocument();
    QDomElement groupElem = doc.createElement( "GROUP" );
    element.appendChild( groupElem );
    QDomElement textElem = doc.createElement( "TEXT" );
    groupElem.appendChild( textElem );
    textElem.appendChild( doc.createTextNode( txt ) );
    return KBookmarkGroup(groupElem);
}

KBookmark KBookmarkGroup::createNewSeparator()
{
    ASSERT(!element.isNull());
    QDomDocument doc = element.ownerDocument();
    ASSERT(!doc.isNull());
    QDomElement groupElem = doc.createElement( "SEPARATOR" );
    return KBookmark(groupElem);
}

bool KBookmarkGroup::moveItem( const KBookmark & item, const KBookmark & after )
{
    QDomNode n;
    if ( !after.isNull() )
        n = element.insertAfter( item.element, after.element );
    else // the dom is quite strange. I need insertBefore to set a first child.
    {
        // in addition to that, we have this hidden TEXT child
        QDomElement firstChild = element.firstChild().toElement();
        if ( firstChild.tagName() == "TEXT" )
            n = element.insertAfter( item.element, firstChild );
        else // well, this is not supposed to happen
            n = element.insertBefore( item.element, QDomElement() );
    }
    return (!n.isNull());
}

KBookmark KBookmarkGroup::addBookmark( const QString & text, const QString & url )
{
    kdDebug() << "KBookmarkGroup::addBookmark " << text << " into " << m_address << endl;
    QDomDocument doc = element.ownerDocument();
    QDomElement elem = doc.createElement( "BOOKMARK" );
    element.appendChild( elem );
    elem.setAttribute( "URL", url );
    QString icon = KMimeType::iconForURL( KURL(url) );
    elem.setAttribute( "ICON", icon );
    elem.appendChild( doc.createTextNode( text ) );
    return KBookmark(elem);
}

void KBookmarkGroup::deleteBookmark( KBookmark bk )
{
    element.removeChild( bk.element );
}

bool KBookmarkGroup::isToolbarGroup() const
{
    return ( element.attribute("TOOLBAR") == "1" );
}

QDomElement KBookmarkGroup::findToolbar() const
{
    // Search among the "GROUP" children only
    QDomNodeList list = element.elementsByTagName("GROUP");
    for ( uint i = 0 ; i < list.count() ; ++i )
    {
        QDomElement e = list.item(i).toElement();
        if ( e.attribute("TOOLBAR") == "1" )
            return e;
        else
        {
            QDomElement result = KBookmarkGroup(e).findToolbar();
            if (!result.isNull())
                return result;
        }
    }
    return QDomElement();
}

//////

bool KBookmark::isGroup() const
{
    return (element.tagName() == "GROUP"
            || element.tagName() == "BOOKMARKS" ); // don't forget the toplevel group
}

bool KBookmark::isSeparator() const
{
    return (element.tagName() == "SEPARATOR");
}

QString KBookmark::text() const
{
    return KStringHandler::csqueeze( fullText() );
}

QString KBookmark::fullText() const
{
    QString txt;
    // This small hack allows to avoid virtual tables in
    // KBookmark and KBookmarkGroup :)
    if (isGroup())
        txt = element.namedItem("TEXT").toElement().text();
    else
        if (isSeparator())
            txt = i18n("--- separator ---");
        else
            txt = element.text();

    return txt;
}

QString KBookmark::url() const
{
    return element.attribute("URL");
}

QString KBookmark::icon() const
{
    QString icon = element.attribute("ICON");
    if ( icon.isEmpty() )
        // Default icon depends on URL for bookmarks, and is default directory
        // icon for groups.
        if ( isGroup() )
            icon = KMimeType::mimeType( "inode/directory" )->KMimeType::icon( QString::null, true );
        else
            if ( isSeparator() )
                icon = "eraser"; // whatever
            else
                icon = KMimeType::iconForURL( url() );
    return icon;
}

KBookmarkGroup KBookmark::parentGroup() const
{
    return KBookmarkGroup( element.parentNode().toElement() );
}

KBookmarkGroup KBookmark::toGroup() const
{
    ASSERT( isGroup() );
    return KBookmarkGroup(element);
}

QString KBookmark::address() const
{
    if ( element.tagName() == "BOOKMARKS" )
        return ""; // not QString::null !
    else
    {
        // Use keditbookmarks's DEBUG_ADDRESSES flag to debug this code :)
        QDomElement parent = element.parentNode().toElement();
        ASSERT(!parent.isNull());
        KBookmarkGroup group( parent );
        QString parentAddress = group.address();
        uint counter = 0;
        // Implementation note: we don't use QDomNode's childNode list because we
        // would have to skip "TEXT", which KBookmarkGroup already does for us.
        for ( KBookmark bk = group.first() ; !bk.isNull() ; bk = group.next(bk), ++counter )
        {
            if ( bk.element == element )
                return parentAddress + "/" + QString::number(counter);
        }
        kdWarning() << "KBookmark::address : this can't happen!  " << parentAddress << endl;
        return "ERROR";
    }
}
