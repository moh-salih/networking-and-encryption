#include "panels.h"

CaesarPanel::CaesarPanel(QWidget *parent): QWidget(parent) {
    auto layout = new QVBoxLayout(this);
    auto grid = new QGridLayout();

    grid->addWidget(new QLabel("Shift (0–25):"), 0, 0);
    shiftEdit = new QLineEdit(this);
    shiftEdit->setPlaceholderText("Enter numeric shift");
    
    // 🌟 Preventinput outside of 0-25 integers
    shiftEdit->setValidator(new QIntValidator(0, 25, this)); 
    
    grid->addWidget(shiftEdit, 0, 1);

    layout->addLayout(grid);

    auto note = new QLabel("Note: Shift must be an integer between 0 and 25.");
    note->setStyleSheet("color: gray; font-size: 11px;");
    layout->addWidget(note);

    setLayout(layout);
}

VigenerePanel::VigenerePanel(QWidget *parent): QWidget(parent) {
    auto layout = new QVBoxLayout(this);
    auto grid = new QGridLayout();

    grid->addWidget(new QLabel("Key:"), 0, 0);
    keyEdit = new QLineEdit(this);
    keyEdit->setPlaceholderText("Alphabetic characters only");
    
    // 🌟 R-3: Add QRegularExpressionValidator to restrict key to letters
    keyEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[A-Za-z]+"), this));
    
    grid->addWidget(keyEdit, 0, 1);

    layout->addLayout(grid);

    auto note = new QLabel("Note: Key must contain only letters (A–Z).");
    note->setStyleSheet("color: gray; font-size: 11px;");
    layout->addWidget(note);

    setLayout(layout);
}

AffinePanel::AffinePanel(QWidget *parent): QWidget(parent) {
    auto layout = new QVBoxLayout(this);
    auto grid = new QGridLayout();

    grid->addWidget(new QLabel("a:"), 0, 0);
    aEdit = new QLineEdit(this);
    aEdit->setPlaceholderText("Multiplicative key");
    
    // 🌟 R-5: Add QIntValidator (Affine keys are usually small integers)
    aEdit->setValidator(new QIntValidator(1, 25, this)); 
    
    grid->addWidget(aEdit, 0, 1);

    grid->addWidget(new QLabel("b:"), 1, 0);
    bEdit = new QLineEdit(this);
    bEdit->setPlaceholderText("Additive key");
    
    // 🌟 R-6: Add QIntValidator
    bEdit->setValidator(new QIntValidator(0, 25, this)); 
    
    grid->addWidget(bEdit, 1, 1);

    layout->addLayout(grid);

    auto note = new QLabel("Note: 'a' must be coprime with 26 (valid values: 1, 3, 5, 7, 9, 11, 15, 17, 19, 21, 23, 25).");
    note->setStyleSheet("color: gray; font-size: 11px;");
    layout->addWidget(note);

    setLayout(layout);
}

PlayfairPanel::PlayfairPanel(QWidget *parent): QWidget(parent){
    auto layout = new QVBoxLayout(this);
    auto grid = new QGridLayout();

    grid->addWidget(new QLabel("Key:"), 0, 0);
    keyEdit = new QLineEdit(this);
    keyEdit->setPlaceholderText("Enter key (letters only)");
    
    // 🌟 R-8: Add QRegularExpressionValidator
    keyEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[A-Za-z]+"), this)); 
    
    grid->addWidget(keyEdit, 0, 1);

    layout->addLayout(grid);

    auto note = new QLabel("Note: Only letters A–Z are allowed. 'J' is treated as 'I'.");
    note->setStyleSheet("color: gray; font-size: 11px;");
    layout->addWidget(note);

    setLayout(layout);
}

HillPanel::HillPanel(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);
    layout->setSpacing(5);

    auto *keyLabel = new QLabel("Key Matrix (A–Z letters):");
    m_keyEdit = new QLineEdit("GYBNQKURP");
    m_keyEdit->setPlaceholderText("Enter key letters (e.g., GYBNQKURP for 3x3)");
    
    m_keyEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[A-Za-z]+"), this));
    
    layout->addWidget(keyLabel);
    layout->addWidget(m_keyEdit);

    auto *sizeLabel = new QLabel("Matrix Size:");
    m_sizeEdit = new QLineEdit("3");
    m_sizeEdit->setValidator(new QIntValidator(2, 4, this));
    layout->addWidget(sizeLabel);
    layout->addWidget(m_sizeEdit);
}



ModernAESPanel::ModernAESPanel(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    sizeCombo = new QComboBox(this);
    sizeCombo->addItem("AES-128", 16);
    sizeCombo->addItem("AES-192", 24);
    sizeCombo->addItem("AES-256", 32);

    keyEdit = new QLineEdit(this);
    keyEdit->setPlaceholderText("Enter Key in HEX (e.g., 000102...)");
    
    layout->addWidget(new QLabel("Key Size:"));
    layout->addWidget(sizeCombo);
    layout->addWidget(new QLabel("Key (Hex):"));
    layout->addWidget(keyEdit);
}

RSAPanel::RSAPanel(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    pubEdit = new QLineEdit(this);
    privEdit = new QLineEdit(this);
    pubEdit->setPlaceholderText("Public Key PEM...");
    privEdit->setPlaceholderText("Private Key PEM...");
    
    layout->addWidget(new QLabel("Public Key (Encryption):"));
    layout->addWidget(pubEdit);
    layout->addWidget(new QLabel("Private Key (Decryption):"));
    layout->addWidget(privEdit);
}