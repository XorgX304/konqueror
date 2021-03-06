/* "Misc Options" Tab for KFM configuration
 *
 * Copyright (c) Sven Radej 1998
 * Copyright (c) David Faure 1998
 *
 */

#ifndef HTMLOPTS_H
#define HTMLOPTS_H

#include <QCheckBox>

//-----------------------------------------------------------------------------
// The "Misc Options" Tab for the HTML view contains :

// Change cursor over links
// Underline links
// AutoLoad Images
// ... there is room for others :))

#include <kcmodule.h>
#include <ksharedconfig.h>
class QSpinBox;
class QGroupBox;

class KMiscHTMLOptions : public KCModule
{
    Q_OBJECT

public:
    KMiscHTMLOptions(QWidget *parent, const QVariantList &);
    ~KMiscHTMLOptions() override;
    void load() override;
    void save() override;
    void defaults() override;

private:
    KSharedConfig::Ptr m_pConfig;
    QString  m_groupname;

    QCheckBox *m_cbCursor;
    QCheckBox *m_pAutoRedirectCheckBox;
    QCheckBox *m_pOpenMiddleClick;
    QCheckBox *m_pBackRightClick;
    QGroupBox *m_pFormCompletionCheckBox;
    QCheckBox *m_pAdvancedAddBookmarkCheckBox;
    QCheckBox *m_pOnlyMarkedBookmarksCheckBox;
    QCheckBox *m_pAccessKeys;
    QCheckBox *m_pDoNotTrack;
    QCheckBox *m_pOfferToSaveWebsitePassword;
    QSpinBox *m_pMaxFormCompletionItems;
#ifdef WEBENGINE_PDF_VIEWER
    QCheckBox *m_pdfViewer;
#endif
};

#endif // HTMLOPTS_H
