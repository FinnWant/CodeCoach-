#include "AddEntryDialog.h"
#include "EntryFactory.h"
#include "Validation.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <random>
#include <sstream>

#include <QApplication>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPixmap>
#include <QSplitter>
#include <QVBoxLayout>

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static std::string makeUUID() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis;
    std::ostringstream ss;
    ss << std::hex << std::setfill('0');
    ss << std::setw(8) << dis(gen) << "-";
    ss << std::setw(4) << (dis(gen) & 0xFFFFu) << "-";
    ss << std::setw(4) << ((dis(gen) & 0x0FFFu) | 0x4000u) << "-";
    ss << std::setw(4) << ((dis(gen) & 0x3FFFu) | 0x8000u) << "-";
    ss << std::setw(8) << dis(gen) << std::setw(4) << (dis(gen) & 0xFFFFu);
    return ss.str();
}

static std::string nowISO8601() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", std::gmtime(&t));
    return buf;
}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

AddEntryDialog::AddEntryDialog(Repository *repo, QWidget *parent)
    : QDialog(parent), repo_(repo) {

    setWindowTitle("Add Entry");
    setMinimumSize(900, 640);

    // ---- Left panel: image + OCR controls --------------------------------
    auto *leftWidget  = new QWidget(this);
    auto *leftLayout  = new QVBoxLayout(leftWidget);

    auto *imageGroup  = new QGroupBox("Source Image", leftWidget);
    auto *imageLayout = new QVBoxLayout(imageGroup);

    imagePreview_ = new QLabel(imageGroup);
    imagePreview_->setFixedSize(300, 200);
    imagePreview_->setAlignment(Qt::AlignCenter);
    imagePreview_->setStyleSheet("border: 1px solid #aaa; background: #f0f0f0;");
    imagePreview_->setText("No image selected");
    imageLayout->addWidget(imagePreview_);

    imagePath_ = new QLineEdit(imageGroup);
    imagePath_->setPlaceholderText("Image file path…");
    imagePath_->setReadOnly(true);
    imageLayout->addWidget(imagePath_);

    auto *btnRow  = new QHBoxLayout;
    browseBtn_    = new QPushButton("Browse…", imageGroup);
    analyzeBtn_   = new QPushButton("Analyze Image", imageGroup);
    analyzeBtn_->setEnabled(false);
    btnRow->addWidget(browseBtn_);
    btnRow->addWidget(analyzeBtn_);
    imageLayout->addLayout(btnRow);

    progressBar_ = new QProgressBar(imageGroup);
    progressBar_->setRange(0, 0); // indeterminate
    progressBar_->setVisible(false);
    imageLayout->addWidget(progressBar_);

    leftLayout->addWidget(imageGroup);

    auto *rawGroup  = new QGroupBox("OCR / Payload Preview", leftWidget);
    auto *rawLayout = new QVBoxLayout(rawGroup);
    payloadPreview_ = new QTextEdit(rawGroup);
    payloadPreview_->setReadOnly(true);
    payloadPreview_->setFontFamily("monospace");
    payloadPreview_->setPlaceholderText("Extracted JSON will appear here after analysis…");
    rawLayout->addWidget(payloadPreview_);
    leftLayout->addWidget(rawGroup);

    // ---- Right panel: entry form -----------------------------------------
    auto *rightWidget = new QWidget(this);
    auto *rightLayout = new QVBoxLayout(rightWidget);

    auto *formGroup  = new QGroupBox("Entry Details", rightWidget);
    auto *formLayout = new QFormLayout(formGroup);

    titleEdit_       = new QLineEdit(formGroup);
    authorEdit_      = new QLineEdit(formGroup);
    platformCombo_   = new QComboBox(formGroup);
    platformCombo_->addItems({"scratch", "pixelpad", "unity"});
    tagsEdit_        = new QLineEdit(formGroup);
    tagsEdit_->setPlaceholderText("comma-separated, e.g. game, loops, sprite");
    descriptionEdit_ = new QTextEdit(formGroup);
    descriptionEdit_->setPlaceholderText("Optional description (written by coach)…");
    descriptionEdit_->setFixedHeight(100);

    formLayout->addRow("Title *",       titleEdit_);
    formLayout->addRow("Author",        authorEdit_);
    formLayout->addRow("Platform *",    platformCombo_);
    formLayout->addRow("Tags",          tagsEdit_);
    formLayout->addRow("Description",   descriptionEdit_);

    rightLayout->addWidget(formGroup);
    rightLayout->addStretch();

    // ---- Button box -------------------------------------------------------
    auto *buttons = new QDialogButtonBox(
        QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);

    // ---- Assemble ---------------------------------------------------------
    auto *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(leftWidget);
    splitter->addWidget(rightWidget);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(splitter);
    mainLayout->addWidget(buttons);

    // ---- Connections ------------------------------------------------------
    connect(browseBtn_,  &QPushButton::clicked, this, &AddEntryDialog::browseImage);
    connect(analyzeBtn_, &QPushButton::clicked, this, &AddEntryDialog::analyzeImage);
    connect(buttons->button(QDialogButtonBox::Save), &QPushButton::clicked,
            this, &AddEntryDialog::saveEntry);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

// ---------------------------------------------------------------------------

void AddEntryDialog::browseImage() {
    QString path = QFileDialog::getOpenFileName(
        this, "Select Image", QString(),
        "Images (*.png *.jpg *.jpeg *.bmp *.tiff *.tif)");
    if (path.isEmpty()) return;

    imagePath_->setText(path);
    analyzeBtn_->setEnabled(true);

    QPixmap px(path);
    if (!px.isNull()) {
        imagePreview_->setPixmap(
            px.scaled(imagePreview_->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void AddEntryDialog::analyzeImage() {
    QString path = imagePath_->text();
    if (path.isEmpty()) return;

    progressBar_->setVisible(true);
    analyzeBtn_->setEnabled(false);
    QApplication::processEvents();

    try {
        analysisResult_ = codecoach::analyzeImage(path.toStdString());
        hasAnalysis_ = true;
        applyAnalysisResult(analysisResult_, path);
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Analysis failed", QString::fromStdString(e.what()));
    }

    progressBar_->setVisible(false);
    analyzeBtn_->setEnabled(true);
}

void AddEntryDialog::applyAnalysisResult(const codecoach::AnalysisResult &r,
                                          const QString &imagePath) {
    // Auto-fill platform if confidence is reasonable
    if (r.confidence >= 40) {
        int idx = platformCombo_->findText(QString::fromStdString(r.platform));
        if (idx >= 0) platformCombo_->setCurrentIndex(idx);
    }

    // Show structured payload + raw OCR in preview pane
    nlohmann::json preview;
    preview["platform"]   = r.platform;
    preview["confidence"] = r.confidence;
    preview["payload"]    = r.payload;
    preview["raw_text"]   = r.rawText;
    payloadPreview_->setText(QString::fromStdString(preview.dump(2)));

    // Auto-fill title from filename if blank
    if (titleEdit_->text().trimmed().isEmpty()) {
        QFileInfo fi(imagePath);
        titleEdit_->setText(fi.completeBaseName());
    }
}

void AddEntryDialog::saveEntry() {
    if (titleEdit_->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation", "Title is required.");
        return;
    }

    try {
        Entry entry = buildEntry();
        std::string err;
        if (!codecoach::validateEntry(entry, err)) {
            QMessageBox::warning(this, "Validation failed", QString::fromStdString(err));
            return;
        }
        repo_->saveEntry(entry);
        accept();
    } catch (const std::exception &e) {
        QMessageBox::critical(this, "Save failed", QString::fromStdString(e.what()));
    }
}

Entry AddEntryDialog::buildEntry() const {
    Entry e;
    e.id        = makeUUID();
    e.title     = titleEdit_->text().trimmed().toStdString();
    e.platform  = platformCombo_->currentText().toStdString();
    e.createdAt = nowISO8601();

    if (!authorEdit_->text().trimmed().isEmpty())
        e.author = authorEdit_->text().trimmed().toStdString();

    if (!descriptionEdit_->toPlainText().trimmed().isEmpty())
        e.description = descriptionEdit_->toPlainText().trimmed().toStdString();

    for (const QString &tag : tagsEdit_->text().split(',', Qt::SkipEmptyParts))
        e.tags.push_back(tag.trimmed().toStdString());

    if (!imagePath_->text().isEmpty())
        e.sourceImage = imagePath_->text().toStdString();

    // Use analysed payload if available and platform matches; otherwise
    // build a minimal valid placeholder payload for the selected platform.
    if (hasAnalysis_ && analysisResult_.platform == e.platform) {
        e.payload = analysisResult_.payload;
    } else {
        if (e.platform == "scratch") {
            ScratchPayload p;
            p.visual.stage.push_back(
                ScratchStageSprite{"Sprite1",
                    {ScratchScript{"when green flag clicked", {"(no blocks extracted)"}}}});
            e.payload = nlohmann::json(p);
        } else if (e.platform == "pixelpad") {
            PixelPadPayload p;
            p.classes.push_back(PixelPadClass{"Unknown", "Class",
                {PixelPadMethod{"loop", "(no code extracted)"}}});
            e.payload = nlohmann::json(p);
        } else {
            UnityPayload p;
            p.objects.push_back(
                UnityObject{"Unknown",
                    {UnityComponent{"Script", "(no code extracted)"}}});
            e.payload = nlohmann::json(p);
        }
    }

    return e;
}
