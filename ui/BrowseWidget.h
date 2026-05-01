#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>

#include "Repository.h"

class BrowseWidget : public QWidget {
    Q_OBJECT

public:
    explicit BrowseWidget(Repository *repo, QWidget *parent = nullptr);

    void refresh();

signals:
    void entrySelected(const QString &id);

private slots:
    void onSearch(const QString &text);
    void onRowDoubleClicked(int row, int col);

private:
    void populateTable(const std::vector<Entry> &entries);

    Repository   *repo_  = nullptr;
    QLineEdit    *search_ = nullptr;
    QTableWidget *table_  = nullptr;

    std::vector<Entry> allEntries_;
};
