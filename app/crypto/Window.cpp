#include "Window.h"
#include <QPixmap>
#include <QFont>
#include <QSpacerItem>
#include <QSizePolicy>

#include "crypto/classic/Caesar.h"
#include "crypto/classic/Vigenere.h"
#include "crypto/classic/Affine.h"
#include "crypto/classic/Playfair.h"
#include "crypto/classic/Hill.h"
#include "crypto/classic/ICipher.h"

#include "crypto/core/utils.h"
#include "crypto/modern/symmetric/block/AES.h"
#include "crypto/classic/ICipher.h"

MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {
    setupUi();
    setupConnections();
    initializeDefaults();
    applyStyleSheet();
}


void MainWindow::setupUi() {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    setupHeader();
    mainLayout->addWidget(new QLabel("<hr>", this));

    // Main content
    auto *contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(30);
    contentLayout->setAlignment(Qt::AlignTop);
    mainLayout->addLayout(contentLayout);

    auto *controlLayout = setupControls();
    auto *ioLayout = setupIO();

    contentLayout->addLayout(controlLayout);
    contentLayout->addLayout(ioLayout);

    contentLayout->setStretch(0, 0);
    contentLayout->setStretch(1, 1);
    controlLayout->setSizeConstraint(QLayout::SetFixedSize);
}


void MainWindow::setupHeader() {
    // ───────────────────────────── Header (logo + text)
    auto *headerLayout = new QHBoxLayout();
    headerLayout->setAlignment(Qt::AlignHCenter);
    headerLayout->setSpacing(10);

    auto *logoLabel = new QLabel(this);
    QPixmap logo(":/images/ktu_amblem_1b.png");
    logoLabel->setPixmap(logo.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    auto *infoLabel = new QLabel(
        "<b>Karadeniz Technical University</b><br>"
        "Of Faculty of Technology<br>"
        "Software Engineering<br><br>"
        "<b>Mohammed SALIH</b>", this);
    infoLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    infoLabel->setWordWrap(true);
    infoLabel->setFont(QFont("Segoe UI", 10));

    headerLayout->addWidget(logoLabel);
    headerLayout->addWidget(infoLabel);
    mainLayout->addLayout(headerLayout);
}

QVBoxLayout* MainWindow::setupControls() {
    auto *layout = new QVBoxLayout();
    layout->setSpacing(10);
    layout->setAlignment(Qt::AlignTop);

    // ───────────── Cipher Configuration
    auto *cipherBox = new QGroupBox("Cipher Configuration");
    auto *cipherBoxLayout = new QVBoxLayout();
    cipherBoxLayout->setSpacing(8);
    cipherBoxLayout->setAlignment(Qt::AlignTop);

    cipherSelect = new QComboBox(this);
    cipherSelect->addItems({
        "Caesar", "Vigenere", "Affine", "Playfair",  "Hill", "AES", "RSA"
    });
    
    auto *cipherLabel = new QLabel("Select Cipher:");
    cipherBoxLayout->addWidget(cipherLabel);
    cipherBoxLayout->addWidget(cipherSelect);

    stackedPanels      = new QStackedWidget(this);
    caesarPanel        = new CaesarPanel(stackedPanels);
    vigenerePanel      = new VigenerePanel(stackedPanels);
    affinePanel        = new AffinePanel(stackedPanels);
    playfairPanel      = new PlayfairPanel(stackedPanels);
    hillPanel          = new HillPanel(stackedPanels);
    modernAesPanel     = new ModernAESPanel(stackedPanels);
    rsaPanel           = new RSAPanel(stackedPanels);

    stackedPanels->addWidget(caesarPanel);
    stackedPanels->addWidget(vigenerePanel);
    stackedPanels->addWidget(affinePanel);
    stackedPanels->addWidget(playfairPanel);
    stackedPanels->addWidget(hillPanel);

    cipherBoxLayout->addWidget(stackedPanels);
    cipherBox->setLayout(cipherBoxLayout);
    cipherBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    layout->addWidget(cipherBox);

    
    // ───────────── Operation Mode
    auto *modeBox = new QGroupBox("Operation Mode");
    auto *modeLayout = new QVBoxLayout();
    encryptRadio = new QRadioButton("Encrypt");
    decryptRadio = new QRadioButton("Decrypt");
    encryptRadio->setChecked(true);
    modeGroup = new QButtonGroup(this);
    modeGroup->addButton(encryptRadio);
    modeGroup->addButton(decryptRadio);
    modeLayout->addWidget(encryptRadio);
    modeLayout->addWidget(decryptRadio);
    modeBox->setLayout(modeLayout);
    modeBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    layout->addWidget(modeBox);

    // ───────────── File Operations
    auto *fileBox = new QGroupBox("File Operations");
    auto *fileButtonLayout = new QHBoxLayout();
    openButton = new QPushButton("📂 Open");
    saveButton = new QPushButton("💾 Save");
    openButton->setMinimumHeight(32);
    saveButton->setMinimumHeight(32);
    fileButtonLayout->addWidget(openButton);
    fileButtonLayout->addWidget(saveButton);
    fileBox->setLayout(fileButtonLayout);
    fileBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    layout->addWidget(fileBox);

    saveButton->setObjectName("saveButton");
    openButton->setObjectName("openButton");
    
    return layout;
}


QVBoxLayout* MainWindow::setupIO() {
    auto *layout = new QVBoxLayout();
    layout->setSpacing(10);
    layout->setAlignment(Qt::AlignTop);

    inputText = new QTextEdit();
    inputText->setPlaceholderText("Type text to encrypt or decrypt...");
    outputText = new QTextEdit();
    outputText->setReadOnly(true);
    outputText->setStyleSheet("background-color:#fafafa;");

    layout->addWidget(new QLabel("<b>Input:</b>"));
    layout->addWidget(inputText, 1);
    layout->addWidget(new QLabel("<b>Output:</b>"));
    layout->addWidget(outputText, 1);

    inputText->setMinimumHeight(180);
    outputText->setMinimumHeight(180);
    
    return layout;
}


void MainWindow::setupConnections() {

    updateTimer = new QTimer(this);
    updateTimer->setSingleShot(true);
    updateTimer->setInterval(100);

    connect(cipherSelect, QOverload<int>::of(&QComboBox::currentIndexChanged), stackedPanels, &QStackedWidget::setCurrentIndex);
    connect(openButton, &QPushButton::clicked, this, &MainWindow::openFile);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveOutput);
    connect(updateTimer, &QTimer::timeout, this, &MainWindow::process);
    connect(inputText, &QTextEdit::textChanged, this, [this]() { updateTimer->start(); });
    connect(cipherSelect, &QComboBox::currentIndexChanged, this, &MainWindow::process);
    connect(encryptRadio, &QRadioButton::toggled, this, &MainWindow::process);

    for (auto *edit : findChildren<QLineEdit *>()) {
        connect(edit, &QLineEdit::textChanged, this, [this]() { updateTimer->start(); });
    }
}


void MainWindow::initializeDefaults() {
    caesarPanel->setShift(3);
    vigenerePanel->setKey("LEMON");
    affinePanel->setKeys(5, 8);
    playfairPanel->setKey("MONARCHY");
    hillPanel->setKeyAndSize("GYBNQKURP", 3);

    process(); 
}

void MainWindow::applyStyleSheet() {
    QFile file(":/styles/style.qss");
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    }
}

// void MainWindow::process() {
//     try {
//         std::unique_ptr<ICipher> cipher;
//         switch (cipherSelect->currentIndex()) {
//             case 0: cipher = std::make_unique<crypto::classic::Caesar>(caesarPanel->shift()); break;
//             case 1: cipher = std::make_unique<crypto::classic::Vigenere>(vigenerePanel->key().toStdString()); break;
//             case 2: cipher = std::make_unique<crypto::classic::Affine>(affinePanel->a(), affinePanel->b()); break;
//             case 3: cipher = std::make_unique<crypto::classic::Playfair>(playfairPanel->key().toStdString()); break;
//             case 4: cipher = std::make_unique<crypto::classic::Hill>(hillPanel->key().toStdString(), hillPanel->matrixSize()); break; 
//             default: throw std::runtime_error("Invalid cipher selection index."); 
//         }

//         const auto input = inputText->toPlainText().toStdString();
//         const auto output = encryptRadio->isChecked() ? cipher->encrypt(input) : cipher->decrypt(input);

//         outputText->setPlainText(QString::fromStdString(output));
//     }
//     catch (const std::exception &e) {
//         outputText->setPlainText(QString("[Error] ") + e.what());
//     }
// }

void MainWindow::process() {
    try {
        int index = cipherSelect->currentIndex();
        std::string inputStr = inputText->toPlainText().toStdString();
        std::string result;

        if (index <= 4) { 
            // --- Classic Ciphers ---
            std::unique_ptr<ICipher> cipher;
            if(index == 0) cipher = std::make_unique<crypto::classic::Caesar>(caesarPanel->shift());
            else if(index == 1) cipher = std::make_unique<crypto::classic::Vigenere>(vigenerePanel->key().toStdString());
            else if(index == 2) cipher = std::make_unique<crypto::classic::Affine>(affinePanel->a(), affinePanel->b());
            else if(index == 3) cipher = std::make_unique<crypto::classic::Playfair>(playfairPanel->key().toStdString());
            else if(index == 4) cipher = std::make_unique<crypto::classic::Hill>(hillPanel->key().toStdString(), hillPanel->matrixSize());

            result = encryptRadio->isChecked() ? cipher->encrypt(inputStr) : cipher->decrypt(inputStr);
        } 
        else if (index == 5) {
            // --- Modern AES (The logic we just fixed!) ---
            crypto::modern::block::symmetric::AES aes(modernAesPanel->keySize());
            auto keyBytes = crypto::core::utils::fromHex(modernAesPanel->keyHex().toStdString());
            if(keyBytes.size() != modernAesPanel->keySize()) throw std::runtime_error("Key hex length mismatch");
            aes.setKey(keyBytes);

            if (encryptRadio->isChecked()) {
                crypto::core::Bytes data(inputStr.begin(), inputStr.end());
                crypto::core::utils::pad(data, 16);
                
                crypto::core::Bytes encrypted;
                crypto::core::Bytes block(16);
                crypto::core::Bytes outBlock(16);
                
                // Process block by block
                for (size_t i = 0; i < data.size(); i += 16) {
                    std::copy(data.begin() + i, data.begin() + i + 16, block.begin());
                    aes.encryptBlock(block, outBlock);
                    encrypted.insert(encrypted.end(), outBlock.begin(), outBlock.end());
                }
                result = crypto::core::utils::toHex(encrypted);
            } else {
                auto data = crypto::core::utils::fromHex(inputStr);
                crypto::core::Bytes decrypted;
                crypto::core::Bytes block(16);
                crypto::core::Bytes outBlock(16);

                for (size_t i = 0; i < data.size(); i += 16) {
                    std::copy(data.begin() + i, data.begin() + i + 16, block.begin());
                    aes.decryptBlock(block, outBlock);
                    decrypted.insert(decrypted.end(), outBlock.begin(), outBlock.end());
                }
                crypto::core::utils::unpad(decrypted);
                result = std::string(decrypted.begin(), decrypted.end());
            }
        }

        outputText->setPlainText(QString::fromStdString(result));
    }
    catch (const std::exception &e) {
        outputText->setPlainText(QString("[Error] ") + e.what());
    }
}
void MainWindow::openFile() {
    // In openFile()
    QString path = QFileDialog::getOpenFileName(this, "Open Text File", lastDirectory, "Text Files (*.txt)");
    
    if (path.isEmpty()) return;
    lastDirectory = QFileInfo(path).absolutePath();


    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Failed to open file for reading.");
        return;
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    inputText->setPlainText(content);
    process(); // trigger encryption/decryption automatically
}

void MainWindow::saveOutput() {
    QString suggestedName = "output.txt";
    QString path = QFileDialog::getSaveFileName(this, "Save Output File", lastDirectory + "/" + suggestedName, "Text Files (*.txt)");

    if (path.isEmpty()) return;
    lastDirectory = QFileInfo(path).absolutePath();

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Failed to open file for writing.");
        return;
    }

    QTextStream out(&file);
    out << outputText->toPlainText();
    file.close();

    QMessageBox::information(this, "File Saved", "Output saved successfully.");
}
