#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QToolBar>

#include "Database.h"
#include "Repository.h"

class BrowseWidget;
class AddEntryDialog;
class DetailWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void showBrowse();
    void openAddEntry();
    void onEntrySelected(const QString &id);

private:
    void setupToolBar();

    Database       *db_     = nullptr;
    Repository     *repo_   = nullptr;

    QStackedWidget *stack_        = nullptr;
    BrowseWidget   *browseWidget_ = nullptr;
    DetailWidget   *detailWidget_ = nullptr;
};
