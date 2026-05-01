#include "DetailWidget.h"
#include "PayloadRenderer.h"

#include <QFileInfo>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QPixmap>
#include <QSplitter>
#include <QVBoxLayout>

DetailWidget::DetailWidget(QWidget *parent) : QWidget(parent) {
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(6);

    // ---- Top bar: back button + title -----------------------------------
    auto *topRow = new QHBoxLayout;

    backBtn_ = new QPushButton("← Browse", this);
    backBtn_->setFixedWidth(90);
    topRow->addWidget(backBtn_);

    titleLabel_ = new QLabel(this);
    titleLabel_->setStyleSheet("font-size: 15px; font-weight: bold;");
    titleLabel_->setWordWrap(true);
    topRow->addWidget(titleLabel_, 1);

    mainLayout->addLayout(topRow);

    // ---- Meta row -------------------------------------------------------
    metaLabel_ = new QLabel(this);
    metaLabel_->setWordWrap(true);
    metaLabel_->setStyleSheet("color: #444; font-size: 12px;");
    mainLayout->addWidget(metaLabel_);

    descLabel_ = new QLabel(this);
    descLabel_->setWordWrap(true);
    descLabel_->setStyleSheet("color: #666; font-style: italic;");
    mainLayout->addWidget(descLabel_);

    // ---- Splitter: thumbnail left, content tabs right -------------------
    auto *splitter = new QSplitter(Qt::Horizontal, this);

    // Left: source image thumbnail
    auto *leftWidget = new QWidget(splitter);
    auto *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 4, 0);

    auto *thumbGroup  = new QGroupBox("Source Image", leftWidget);
    auto *thumbLayout = new QVBoxLayout(thumbGroup);
    thumbnailLabel_ = new QLabel(thumbGroup);
    thumbnailLabel_->setFixedSize(280, 210);
    thumbnailLabel_->setAlignment(Qt::AlignCenter);
    thumbnailLabel_->setStyleSheet("border: 1px solid #bbb; background: #f5f5f5;");
    thumbnailLabel_->setText("No image");
    thumbLayout->addWidget(thumbnailLabel_);
    leftLayout->addWidget(thumbGroup);
    leftLayout->addStretch();

    // Right: tab widget — Visual (default) + JSON
    auto *rightWidget = new QWidget(splitter);
    auto *rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setContentsMargins(4, 0, 0, 0);

    tabs_ = new QTabWidget(rightWidget);

    visualView_ = new QPlainTextEdit(tabs_);
    visualView_->setReadOnly(true);
    visualView_->setFont(QFont("Monospace", 10));
    visualView_->setLineWrapMode(QPlainTextEdit::NoWrap);

    jsonView_ = new QPlainTextEdit(tabs_);
    jsonView_->setReadOnly(true);
    jsonView_->setFont(QFont("Monospace", 9));
    jsonView_->setLineWrapMode(QPlainTextEdit::NoWrap);

    tabs_->addTab(visualView_, "Visual");
    tabs_->addTab(jsonView_,   "JSON");

    rightLayout->addWidget(tabs_);

    splitter->addWidget(leftWidget);
    splitter->addWidget(rightWidget);
    splitter->setStretchFactor(0, 0);   // thumbnail: fixed feel
    splitter->setStretchFactor(1, 1);   // content: takes all spare space
    splitter->setSizes({300, 780});

    mainLayout->addWidget(splitter, 1);

    connect(backBtn_, &QPushButton::clicked, this, &DetailWidget::backRequested);
}

void DetailWidget::setEntry(const Entry &entry) {
    // Title
    titleLabel_->setText(QString::fromStdString(entry.title));

    // Meta line
    QStringList meta;
    meta << "Platform: <b>" + QString::fromStdString(entry.platform) + "</b>";
    if (entry.author)    meta << "Author: " + QString::fromStdString(*entry.author);
    if (entry.createdAt) meta << "Created: " + QString::fromStdString(*entry.createdAt).left(10);
    QStringList tagList;
    for (const auto &t : entry.tags)
        tagList << QString::fromStdString(t);
    if (!tagList.isEmpty()) meta << "Tags: " + tagList.join(", ");
    metaLabel_->setText(meta.join("  ·  "));
    metaLabel_->setTextFormat(Qt::RichText);

    // Description
    descLabel_->setText(entry.description
        ? QString::fromStdString(*entry.description)
        : QString());

    // Thumbnail
    QString imgPath = entry.sourceImage
        ? QString::fromStdString(*entry.sourceImage)
        : QString();
    if (!imgPath.isEmpty() && QFileInfo::exists(imgPath)) {
        QPixmap px(imgPath);
        thumbnailLabel_->setPixmap(
            px.scaled(thumbnailLabel_->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        thumbnailLabel_->setPixmap(QPixmap());
        thumbnailLabel_->setText("No image");
    }

    // Visual tab — ASCII representation
    visualView_->setPlainText(
        PayloadRenderer::render(entry.platform, entry.payload));

    // JSON tab — raw payload
    jsonView_->setPlainText(
        QString::fromStdString(entry.payload.dump(2)));

    // Always open on the Visual tab
    tabs_->setCurrentIndex(0);
}
