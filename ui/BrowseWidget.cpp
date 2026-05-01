#include "BrowseWidget.h"

#include <QHeaderView>
#include <QVBoxLayout>
#include <QLabel>

BrowseWidget::BrowseWidget(Repository *repo, QWidget *parent)
    : QWidget(parent), repo_(repo) {

    auto *layout = new QVBoxLayout(this);

    search_ = new QLineEdit(this);
    search_->setPlaceholderText("Search by title, author, platform, or tag…");
    layout->addWidget(search_);

    table_ = new QTableWidget(this);
    table_->setColumnCount(5);
    table_->setHorizontalHeaderLabels({"Title", "Platform", "Author", "Tags", "Date"});
    table_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    table_->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setAlternatingRowColors(true);
    layout->addWidget(table_);

    connect(search_, &QLineEdit::textChanged, this, &BrowseWidget::onSearch);
    connect(table_,  &QTableWidget::cellDoubleClicked,
            this,    &BrowseWidget::onRowDoubleClicked);
}

void BrowseWidget::refresh() {
    allEntries_ = repo_->listEntries();
    populateTable(allEntries_);
}

void BrowseWidget::onSearch(const QString &text) {
    if (text.trimmed().isEmpty()) {
        populateTable(allEntries_);
        return;
    }

    QString lower = text.toLower();
    std::vector<Entry> filtered;
    for (const auto &e : allEntries_) {
        bool match =
            QString::fromStdString(e.title).toLower().contains(lower) ||
            QString::fromStdString(e.platform).toLower().contains(lower) ||
            (e.author && QString::fromStdString(*e.author).toLower().contains(lower));

        if (!match) {
            for (const auto &tag : e.tags) {
                if (QString::fromStdString(tag).toLower().contains(lower)) {
                    match = true;
                    break;
                }
            }
        }

        if (match) filtered.push_back(e);
    }
    populateTable(filtered);
}

void BrowseWidget::onRowDoubleClicked(int row, int /*col*/) {
    auto *idItem = table_->item(row, 0);
    if (!idItem) return;
    emit entrySelected(idItem->data(Qt::UserRole).toString());
}

void BrowseWidget::populateTable(const std::vector<Entry> &entries) {
    table_->setRowCount(static_cast<int>(entries.size()));

    for (int i = 0; i < static_cast<int>(entries.size()); ++i) {
        const Entry &e = entries[i];

        auto *titleItem = new QTableWidgetItem(QString::fromStdString(e.title));
        titleItem->setData(Qt::UserRole, QString::fromStdString(e.id));
        table_->setItem(i, 0, titleItem);

        table_->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(e.platform)));
        table_->setItem(i, 2, new QTableWidgetItem(
            e.author ? QString::fromStdString(*e.author) : ""));

        QStringList tagList;
        for (const auto &t : e.tags) tagList << QString::fromStdString(t);
        table_->setItem(i, 3, new QTableWidgetItem(tagList.join(", ")));

        table_->setItem(i, 4, new QTableWidgetItem(
            e.createdAt ? QString::fromStdString(*e.createdAt).left(10) : ""));
    }
}
