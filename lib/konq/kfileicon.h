/*  This file is part of the KDE project
    Copyright (C) 1997 David Faure <faure@kde.org>
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/ 

#ifndef __konq_fileicon_h__
#define __konq_fileicon_h__

/*
 * A "file icon container" is a specialised icon container, 
 * based on KIconContainer, and adding functionalities for files
 * (mimetype, icon, listing (TODO), refreshing (TODO), ...)
 */

#include <qstrlist.h>

#include <kio_interface.h>
#include <kurl.h>

class KMimeType;

class KFileIcon
{
public:
  KFileIcon( UDSEntry& _entry, KURL& _url );
  virtual ~KFileIcon() { }

  virtual QString url() { return m_url.url(); }

  virtual bool isMarked() { return m_bMarked; }
  virtual void mark() { m_bMarked = true; }
  virtual void unmark() { m_bMarked = false; }
  
  virtual UDSEntry udsEntry() { return m_entry; }

  /*
   * @return the string to be displayed in the statusbar when the mouse 
   *         is over this item
   */
  virtual QString getStatusBarInfo();

  virtual bool acceptsDrops( QStrList& /* _formats */ );

  virtual KMimeType* mimeType() { return m_pMimeType; }

protected:
  void init();
  
  UDSEntry m_entry;
  KURL m_url;

  KMimeType* m_pMimeType;

  bool m_bMarked;
  bool m_bIsLocalURL;
};

#endif
