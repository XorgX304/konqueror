/*
 * main.h
 *
 * Copyright (c) 1999 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#ifndef __MAIN_H__
#define __MAIN_H__


#include <qtabwidget.h>


#include <kcmodule.h>


class KBehaviourOptions;
class KFontOptions;
class KColorOptions;
class KHtmlOptions;
class KHTTPOptions;
class KMiscOptions;
class KRootOptions;


class KonqyModule : public KCModule
{
  Q_OBJECT

public:

  KonqyModule(QWidget *parent, const char *name);

  void load();
  void save();
  void defaults();


protected:

  void resizeEvent(QResizeEvent *e);


protected slots:

  void moduleChanged(bool state);


private:

  QTabWidget   *tab;

  KBehaviourOptions *behaviour;
  KFontOptions      *font;
  KColorOptions     *color;
  KHtmlOptions      *html;
  KHTTPOptions      *http;
  KMiscOptions      *misc;

};


class KDesktopModule : public KCModule
{
  Q_OBJECT

public:

  KDesktopModule(QWidget *parent, const char *name);

  void load();
  void save();
  void defaults();


protected:

  void resizeEvent(QResizeEvent *e);


protected slots:

  void moduleChanged(bool state);


private:

  QTabWidget   *tab;

  KRootOptions      *root;
  KFontOptions      *font;
  KColorOptions     *color;
  KHtmlOptions      *html;

};


#endif
