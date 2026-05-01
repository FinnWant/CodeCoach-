#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QProgressBar>

#include "Repository.h"
#include "ImageAnalyzer.h"

class AddEntryDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddEntryDialog(Repository *repo, QWidget *parent = nullptr);

private slots:
    void browseImage();
    void analyzeImage();
    void saveEntry();

private:
    void applyAnalysisResult(const codecoach::AnalysisResult &result,
                             const QString &imagePath);
    Entry buildEntry() const;

    Repository *repo_ = nullptr;

    // Image section
    QLabel      *imagePreview_  = nullptr;
    QLineEdit   *imagePath_     = nullptr;
    QPushButton *browseBtn_     = nullptr;
    QPushButton *analyzeBtn_    = nullptr;
    QProgressBar *progressBar_  = nullptr;

    // Form fields
    QLineEdit *titleEdit_       = nullptr;
    QLineEdit *authorEdit_      = nullptr;
    QComboBox *platformCombo_   = nullptr;
    QLineEdit *tagsEdit_        = nullptr;
    QTextEdit *descriptionEdit_ = nullptr;

    // Raw OCR / payload preview
    QTextEdit *payloadPreview_  = nullptr;

    // Stored analysis
    codecoach::AnalysisResult analysisResult_;
    bool hasAnalysis_ = false;
};
