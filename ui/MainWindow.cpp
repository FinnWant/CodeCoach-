#include "MainWindow.h"
#include "AddEntryDialog.h"
#include "BrowseWidget.h"
#include "DetailWidget.h"

#include <QAction>
#include <QDir>
#include <QLabel>
#include <QMessageBox>
#include <QToolBar>
#include <QWidget>
#ifdef HAVE_QT_SVG
#  include <QPainter>
#  include <QPixmap>
#  include <QSvgRenderer>
#endif

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("CodeCoach+");
    resize(1100, 700);

    QDir::current().mkpath("data");
    db_   = new Database("data/codecoach.db");
    repo_ = new Repository(*db_);
    repo_->initSchema();

    stack_        = new QStackedWidget(this);
    browseWidget_ = new BrowseWidget(repo_, this);
    detailWidget_ = new DetailWidget(this);

    stack_->addWidget(browseWidget_);
    stack_->addWidget(detailWidget_);
    setCentralWidget(stack_);

    setupToolBar();

    connect(browseWidget_, &BrowseWidget::entrySelected,
            this,          &MainWindow::onEntrySelected);
    connect(detailWidget_, &DetailWidget::backRequested,
            this,          &MainWindow::showBrowse);

    showBrowse();
}

void MainWindow::setupToolBar() {
    QToolBar *tb = addToolBar("Main");
    tb->setMovable(false);
    tb->setIconSize(QSize(32, 32));
    tb->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

#ifdef HAVE_QT_SVG
    // Logo rendered from the embedded SVG
    QSvgRenderer renderer(QString(":/images/icon.svg"));
    QPixmap logoPx(36, 36);
    logoPx.fill(Qt::transparent);
    QPainter painter(&logoPx);
    renderer.render(&painter);
    painter.end();

    auto *logoLabel = new QLabel(tb);
    logoLabel->setPixmap(logoPx);
    logoLabel->setContentsMargins(6, 2, 2, 2);
    tb->addWidget(logoLabel);
#endif

    auto *appLabel = new QLabel("<b style='font-size:14px; color:#7c2d12;'>CodeCoach++</b>", tb);
    appLabel->setContentsMargins(0, 2, 12, 2);
    tb->addWidget(appLabel);

    tb->addSeparator();

    QAction *browseAct = tb->addAction("Browse");
    QAction *addAct    = tb->addAction("+ Add Entry");

    connect(browseAct, &QAction::triggered, this, &MainWindow::showBrowse);
    connect(addAct,    &QAction::triggered, this, &MainWindow::openAddEntry);
}

void MainWindow::showBrowse() {
    browseWidget_->refresh();
    stack_->setCurrentWidget(browseWidget_);
}

void MainWindow::openAddEntry() {
    AddEntryDialog dlg(repo_, this);
    if (dlg.exec() == QDialog::Accepted) {
        showBrowse();
    }
}

void MainWindow::onEntrySelected(const QString &id) {
    auto entry = repo_->getEntry(id.toStdString());
    if (!entry) {
        QMessageBox::warning(this, "Not found", "Entry not found: " + id);
        return;
    }
    detailWidget_->setEntry(*entry);
    stack_->setCurrentWidget(detailWidget_);
}
