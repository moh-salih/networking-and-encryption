#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QValidator>
#include <QRegularExpression>

// --- Caesar Cipher Panel ---
class CaesarPanel : public QWidget {
    Q_OBJECT
public:
    explicit CaesarPanel(QWidget *parent = nullptr);
    int shift() const { return shiftEdit->text().toInt(); }
    void setShift(int value) { shiftEdit->setText(QString::number(value)); }
private:
    QLineEdit *shiftEdit;
};


// --- Vigenere Cipher Panel ---
class VigenerePanel : public QWidget {
    Q_OBJECT
public:
    explicit VigenerePanel(QWidget *parent = nullptr);
    QString key() const { return keyEdit->text(); }
    void setKey(const QString &value) { keyEdit->setText(value); }
private:
    QLineEdit *keyEdit;
};


// --- Affine Cipher Panel ---
class AffinePanel : public QWidget {
    Q_OBJECT
public:
    explicit AffinePanel(QWidget *parent = nullptr);
    int a() const { return aEdit->text().toInt(); }
    int b() const { return bEdit->text().toInt(); }
    void setKeys(int a, int b) { 
        aEdit->setText(QString::number(a)); 
        bEdit->setText(QString::number(b)); 
    }
private:
    QLineEdit *aEdit;
    QLineEdit *bEdit;
};


// --- Playfair Cipher Panel ---
class PlayfairPanel : public QWidget {
    Q_OBJECT
public:
    explicit PlayfairPanel(QWidget *parent = nullptr);
    QString key() const { return keyEdit->text(); }
    void setKey(const QString &value) { keyEdit->setText(value); }
private:
    QLineEdit *keyEdit;
};


// --- Hill Cipher Panel ---
class HillPanel : public QWidget {
    Q_OBJECT
public:
    explicit HillPanel(QWidget *parent = nullptr);

    QString key() const { return m_keyEdit->text().trimmed().toUpper(); }
    int matrixSize() const { return m_sizeEdit->text().toInt(); }
    
    void setKeyAndSize(const QString &key, int size) { 
        m_keyEdit->setText(key);
        m_sizeEdit->setText(QString::number(size));
    }

private:
    QLineEdit *m_keyEdit;
    QLineEdit *m_sizeEdit;
};


// --- NEW: Modern AES Panel ---
class ModernAESPanel : public QWidget {
    Q_OBJECT
public:
    explicit ModernAESPanel(QWidget *parent = nullptr);
    QString keyHex() const { return keyEdit->text(); }
    int keySize() const { return sizeCombo->currentData().toInt(); }
private:
    QLineEdit *keyEdit;
    QComboBox *sizeCombo;
};

// --- NEW: Standard RSA Panel ---
class RSAPanel : public QWidget {
    Q_OBJECT
public:
    explicit RSAPanel(QWidget *parent = nullptr);
    QString publicKey() const { return pubEdit->text(); }
    QString privateKey() const { return privEdit->text(); }
private:
    QLineEdit *pubEdit, *privEdit;
};