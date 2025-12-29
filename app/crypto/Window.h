#pragma once
#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QMessageBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QTimer>
#include <QFileDialog>
#include <QTextStream>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLineEdit>


#include "panels.h"


class MainWindow : public QWidget {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

    
private slots:
    void process();
    void openFile();
    void saveOutput();

private:
    QVBoxLayout *mainLayout = nullptr;
    QComboBox *cipherSelect;
    QStackedWidget *stackedPanels;
    QTextEdit *inputText;
    QTextEdit *outputText;
    QRadioButton *encryptRadio;
    QRadioButton *decryptRadio;
    QButtonGroup *modeGroup;
    QTimer *updateTimer;

    QRadioButton *immediateModeRadio = nullptr; 


    QPushButton *openButton = nullptr;
    QPushButton *saveButton = nullptr;

    CaesarPanel *caesarPanel;
    VigenerePanel *vigenerePanel;
    AffinePanel *affinePanel;
    PlayfairPanel *playfairPanel;
    HillPanel *hillPanel;
    ModernAESPanel *modernAesPanel;
    RSAPanel *rsaPanel;

    QString lastDirectory;
    
    void setupUi();
    void setupConnections();
    void initializeDefaults();
    void setupHeader();
    QVBoxLayout* setupControls();
    QVBoxLayout* setupIO();
    void applyStyleSheet();
};
