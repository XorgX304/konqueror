/*
    Copyright (c) 2009 David Faure <faure@kde.org>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2 of the License or ( at
    your option ) version 3 or, at the discretion of KDE e.V. ( which shall
    act as a proxy as in section 14 of the GPLv3 ), any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef HISTORY_MODULE_H
#define HISTORY_MODULE_H

#include <konqsidebarplugin.h>
#include "konqhistorysettings.h"

class QModelIndex;
class KonqHistoryView;

class KonqSidebarHistoryModule : public KonqSidebarModule
{
    Q_OBJECT
public:
    KonqSidebarHistoryModule(QWidget *parent, const KConfigGroup &configGroup);
    virtual ~KonqSidebarHistoryModule();
    virtual QWidget *getWidget() override;

public Q_SLOTS:
    void slotCurViewUrlChanged(const QUrl& url) override;

private Q_SLOTS:
    void slotActivated(const QModelIndex &index);
    void slotPressed(const QModelIndex &index);
    void slotClicked(const QModelIndex &index);
    void slotOpenWindow(const QUrl &url);
    void slotOpenTab(const QUrl &url);
    void reparseConfiguration();

private:
    KonqHistoryView *m_historyView;
    Qt::MouseButtons m_lastPressedButtons;
    KonqHistorySettings *m_settings;
    KonqHistorySettings::Action m_defaultAction;
    QUrl m_currentUrl;
};

#endif // HISTORY_MODULE_H
