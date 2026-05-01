#pragma once

#include <QWidget>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTabWidget>

#include "Model.h"

class DetailWidget : public QWidget {
    Q_OBJECT

public:
    explicit DetailWidget(QWidget *parent = nullptr);

    void setEntry(const Entry &entry);

signals:
    void backRequested();

private:
    // Header row
    QPushButton *backBtn_       = nullptr;
    QLabel      *titleLabel_    = nullptr;
    QLabel      *metaLabel_     = nullptr;
    QLabel      *descLabel_     = nullptr;

    // Left panel
    QLabel      *thumbnailLabel_= nullptr;

    // Right panel tabs
    QTabWidget      *tabs_       = nullptr;
    QPlainTextEdit  *visualView_ = nullptr;  // primary
    QPlainTextEdit  *jsonView_   = nullptr;  // secondary
};
