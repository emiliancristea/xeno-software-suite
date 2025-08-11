#include <QThread>
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QFileDialog>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QProgressDialog>
#include <QGroupBox>
#include <QComboBox>
#include <QListWidget>
#include <QTreeWidget>
#include <QTabWidget>
#include <QCompleter>
#include <QStringListModel>
#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QRegularExpression>
#include <memory>

#include "ai_integration.h"
#include "utils.h"

// Simple C++ syntax highlighter
class CppSyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    explicit CppSyntaxHighlighter(QTextDocument *parent = nullptr)
        : QSyntaxHighlighter(parent) {
        setupHighlightingRules();
    }

protected:
    void highlightBlock(const QString &text) override {
        foreach (const HighlightingRule &rule, highlightingRules) {
            QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
            while (matchIterator.hasNext()) {
                QRegularExpressionMatch match = matchIterator.next();
                setFormat(match.capturedStart(), match.capturedLength(), rule.format);
            }
        }
    }

private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    void setupHighlightingRules() {
        HighlightingRule rule;

        // Keywords
        QTextCharFormat keywordFormat;
        keywordFormat.setColor(QColor(86, 156, 214));
        keywordFormat.setFontWeight(QFont::Bold);
        QStringList keywordPatterns;
        keywordPatterns << "\\bclass\\b" << "\\bstruct\\b" << "\\bnamespace\\b"
                       << "\\bif\\b" << "\\belse\\b" << "\\bfor\\b" << "\\bwhile\\b"
                       << "\\breturn\\b" << "\\bint\\b" << "\\bfloat\\b" << "\\bdouble\\b"
                       << "\\bvoid\\b" << "\\bbool\\b" << "\\bchar\\b" << "\\bconst\\b"
                       << "\\bstatic\\b" << "\\bpublic\\b" << "\\bprivate\\b" << "\\bprotected\\b";

        foreach (const QString &pattern, keywordPatterns) {
            rule.pattern = QRegularExpression(pattern);
            rule.format = keywordFormat;
            highlightingRules.append(rule);
        }

        // Strings
        QTextCharFormat stringFormat;
        stringFormat.setColor(QColor(214, 157, 133));
        rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
        rule.format = stringFormat;
        highlightingRules.append(rule);

        // Comments
        QTextCharFormat commentFormat;
        commentFormat.setColor(QColor(106, 153, 85));
        rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));
        rule.format = commentFormat;
        highlightingRules.append(rule);
    }
};

class XenoCodeWindow : public QMainWindow {
    Q_OBJECT

public:
    XenoCodeWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Xeno Code - AI-Assisted IDE");
        setMinimumSize(1400, 900);
        
        // Initialize AI integration
        ai_integration = std::make_unique<xeno::ai::AIIntegration>();
        
        setupUI();
        setupMenus();
        setupToolbars();
        setupConnections();
        
        loadConfiguration();
        
        statusBar()->showMessage("Ready - AI-assisted coding with Xeno Labs integration");
        
        // Add sample code
        addSampleCode();
    }

private slots:
    void newFile() {
        auto editor = new QPlainTextEdit;
        auto highlighter = new CppSyntaxHighlighter(editor->document());
        
        int index = editor_tabs->addTab(editor, "untitled.cpp");
        editor_tabs->setCurrentIndex(index);
        
        // Setup autocompletion
        setupAutoCompletion(editor);
    }
    
    void openFile() {
        QString filename = QFileDialog::getOpenFileName(this,
            "Open File", "", "Source Files (*.cpp *.h *.py *.js *.ts *.java *.go *.rs)");
        
        if (!filename.isEmpty()) {
            QFile file(filename);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                auto editor = new QPlainTextEdit;
                auto highlighter = new CppSyntaxHighlighter(editor->document());
                
                QTextStream in(&file);
                editor->setPlainText(in.readAll());
                
                QFileInfo fileInfo(filename);
                int index = editor_tabs->addTab(editor, fileInfo.fileName());
                editor_tabs->setCurrentIndex(index);
                
                setupAutoCompletion(editor);
                statusBar()->showMessage("File opened: " + filename);
            }
        }
    }
    
    void saveFile() {
        auto current_editor = qobject_cast<QPlainTextEdit*>(editor_tabs->currentWidget());
        if (!current_editor) {
            QMessageBox::warning(this, "Warning", "No file to save!");
            return;
        }
        
        QString filename = QFileDialog::getSaveFileName(this,
            "Save File", "", "Source Files (*.cpp *.h *.py *.js *.ts)");
        
        if (!filename.isEmpty()) {
            QFile file(filename);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                out << current_editor->toPlainText();
                
                QFileInfo fileInfo(filename);
                editor_tabs->setTabText(editor_tabs->currentIndex(), fileInfo.fileName());
                statusBar()->showMessage("File saved: " + filename);
            }
        }
    }
    
    void getCodeSuggestion() {
        auto current_editor = qobject_cast<QPlainTextEdit*>(editor_tabs->currentWidget());
        if (!current_editor) {
            QMessageBox::warning(this, "Warning", "Please open a file first!");
            return;
        }
        
        int required_credits = 1;
        if (!ai_integration->validateCredits(required_credits)) {
            QMessageBox::warning(this, "Insufficient Credits", 
                QString("Code suggestions require %1 credit. Please purchase more in Xeno Labs.")
                .arg(required_credits));
            return;
        }
        
        QString current_code = current_editor->toPlainText();
        QTextCursor cursor = current_editor->textCursor();
        QString context = current_code.left(cursor.position());
        
        QProgressDialog progress("Getting AI code suggestion...", "Cancel", 0, 100, this);
        progress.setWindowModality(Qt::WindowModal);
        progress.show();
        
        for (int i = 0; i <= 100; i += 20) {
            progress.setValue(i);
            QApplication::processEvents();
            if (progress.wasCanceled()) {
                return;
            }
            QThread::msleep(50);
        }
        
        xeno::ai::AIIntegration::AIRequest request;
        request.prompt = "Complete this code: " + context.right(200).toStdString();
        request.operation_type = "code_completion";
        
        auto response = ai_integration->completeCode(request);
        
        if (response.success) {
            // Insert suggestion at cursor
            QString suggestion = QString::fromStdString(response.content);
            cursor.insertText(suggestion);
            
            statusBar()->showMessage(QString("Code suggestion applied - %1 credit used")
                .arg(response.credits_used));
            updateCreditDisplay();
        } else {
            QMessageBox::critical(this, "AI Error", 
                QString("Failed to get code suggestion: %1").arg(QString::fromStdString(response.error_message)));
        }
    }
    
    void refactorCode() {
        auto current_editor = qobject_cast<QPlainTextEdit*>(editor_tabs->currentWidget());
        if (!current_editor) {
            QMessageBox::warning(this, "Warning", "Please open a file first!");
            return;
        }
        
        if (!current_editor->textCursor().hasSelection()) {
            QMessageBox::warning(this, "Warning", "Please select code to refactor!");
            return;
        }
        
        int required_credits = 2;
        if (!ai_integration->validateCredits(required_credits)) {
            QMessageBox::warning(this, "Insufficient Credits", 
                QString("Code refactoring requires %1 credits.").arg(required_credits));
            return;
        }
        
        QString selected_code = current_editor->textCursor().selectedText();
        
        QProgressDialog progress("Refactoring code...", "Cancel", 0, 100, this);
        progress.setWindowModality(Qt::WindowModal);
        progress.show();
        
        for (int i = 0; i <= 100; i += 10) {
            progress.setValue(i);
            QApplication::processEvents();
            if (progress.wasCanceled()) {
                return;
            }
            QThread::msleep(100);
        }
        
        ai_integration->deductCredits(required_credits);
        
        // Simulate refactoring
        QString refactored = "// Refactored code:\n" + selected_code + "\n// End refactored";
        current_editor->textCursor().insertText(refactored);
        
        statusBar()->showMessage(QString("Code refactored - %1 credits used").arg(required_credits));
        updateCreditDisplay();
    }
    
    void explainCode() {
        auto current_editor = qobject_cast<QPlainTextEdit*>(editor_tabs->currentWidget());
        if (!current_editor) {
            QMessageBox::warning(this, "Warning", "Please open a file first!");
            return;
        }
        
        QString selected_code = current_editor->textCursor().selectedText();
        if (selected_code.isEmpty()) {
            selected_code = current_editor->toPlainText().left(500); // First 500 chars
        }
        
        int required_credits = 1;
        if (!ai_integration->validateCredits(required_credits)) {
            QMessageBox::warning(this, "Insufficient Credits", 
                QString("Code explanation requires %1 credit.").arg(required_credits));
            return;
        }
        
        ai_integration->deductCredits(required_credits);
        
        QString explanation = QString("AI Explanation for selected code:\n\n"
                                    "%1\n\n"
                                    "This code appears to define a function or class structure. "
                                    "The AI would provide detailed explanation here about the code's "
                                    "purpose, functionality, and any potential improvements.")
                             .arg(selected_code.left(200));
        
        ai_output->setPlainText(explanation);
        
        statusBar()->showMessage(QString("Code explained - %1 credit used").arg(required_credits));
        updateCreditDisplay();
    }
    
    void switchAIProvider() {
        QString provider = ai_provider_combo->currentText();
        statusBar()->showMessage("Switched to " + provider + " for AI assistance");
        
        // Update UI based on provider
        if (provider == "Ollama (Local)") {
            credit_status->setText("Local Mode - No credits required");
            credit_status->setStyleSheet("font-weight: bold; font-size: 14px; color: #27ae60;");
        } else {
            updateCreditDisplay();
        }
    }

private:
    void setupUI() {
        auto central_widget = new QWidget;
        setCentralWidget(central_widget);
        
        // Main splitter
        auto main_splitter = new QSplitter(Qt::Horizontal);
        
        // Left panel - File explorer and AI tools
        auto left_panel = new QWidget;
        left_panel->setFixedWidth(300);
        left_panel->setStyleSheet("background-color: #f8f9fa;");
        
        auto left_layout = new QVBoxLayout(left_panel);
        
        // File explorer (placeholder)
        auto file_group = new QGroupBox("Project Explorer");
        auto file_layout = new QVBoxLayout(file_group);
        
        auto file_tree = new QTreeWidget;
        file_tree->setHeaderLabel("Files");
        
        auto root = new QTreeWidgetItem(file_tree);
        root->setText(0, "xeno-software-suite");
        auto src_item = new QTreeWidgetItem(root);
        src_item->setText(0, "src");
        auto header_item = new QTreeWidgetItem(root);
        header_item->setText(0, "include");
        
        file_tree->expandAll();
        file_layout->addWidget(file_tree);
        left_layout->addWidget(file_group);
        
        // AI Tools
        auto ai_group = new QGroupBox("AI Assistant");
        auto ai_layout = new QVBoxLayout(ai_group);
        
        // Provider selection
        auto provider_label = new QLabel("AI Provider:");
        ai_provider_combo = new QComboBox;
        ai_provider_combo->addItem("Xeno AI Cloud");
        ai_provider_combo->addItem("Open Router");
        ai_provider_combo->addItem("Ollama (Local)");
        
        ai_layout->addWidget(provider_label);
        ai_layout->addWidget(ai_provider_combo);
        
        // AI action buttons
        auto suggest_btn = new QPushButton("Code Suggestion (1 credit)");
        auto refactor_btn = new QPushButton("Refactor Code (2 credits)");
        auto explain_btn = new QPushButton("Explain Code (1 credit)");
        auto generate_btn = new QPushButton("Generate Code (2 credits)");
        
        suggest_btn->setStyleSheet("background-color: #3498db; color: white; font-weight: bold; padding: 10px;");
        refactor_btn->setStyleSheet("background-color: #9b59b6; color: white; font-weight: bold; padding: 10px;");
        explain_btn->setStyleSheet("background-color: #2ecc71; color: white; font-weight: bold; padding: 10px;");
        generate_btn->setStyleSheet("background-color: #e74c3c; color: white; font-weight: bold; padding: 10px;");
        
        ai_layout->addWidget(suggest_btn);
        ai_layout->addWidget(refactor_btn);
        ai_layout->addWidget(explain_btn);
        ai_layout->addWidget(generate_btn);
        
        left_layout->addWidget(ai_group);
        
        // Credit status
        credit_status = new QLabel("Credits: Loading...");
        credit_status->setStyleSheet("font-weight: bold; font-size: 14px; color: #2c3e50;");
        left_layout->addWidget(credit_status);
        
        left_layout->addStretch();
        
        main_splitter->addWidget(left_panel);
        
        // Center and right area
        auto content_splitter = new QSplitter(Qt::Vertical);
        
        // Code editor tabs
        editor_tabs = new QTabWidget;
        editor_tabs->setTabsClosable(true);
        
        content_splitter->addWidget(editor_tabs);
        
        // AI output panel
        auto output_group = new QGroupBox("AI Output");
        auto output_layout = new QVBoxLayout(output_group);
        
        ai_output = new QPlainTextEdit;
        ai_output->setMaximumHeight(200);
        ai_output->setPlainText("AI responses and explanations will appear here...");
        
        output_layout->addWidget(ai_output);
        content_splitter->addWidget(output_group);
        
        content_splitter->setStretchFactor(0, 3);
        content_splitter->setStretchFactor(1, 1);
        
        main_splitter->addWidget(content_splitter);
        main_splitter->setStretchFactor(0, 0);
        main_splitter->setStretchFactor(1, 1);
        
        auto main_layout = new QVBoxLayout(central_widget);
        main_layout->addWidget(main_splitter);
        
        // Connect signals
        connect(suggest_btn, &QPushButton::clicked, this, &XenoCodeWindow::getCodeSuggestion);
        connect(refactor_btn, &QPushButton::clicked, this, &XenoCodeWindow::refactorCode);
        connect(explain_btn, &QPushButton::clicked, this, &XenoCodeWindow::explainCode);
        connect(ai_provider_combo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
                this, &XenoCodeWindow::switchAIProvider);
        connect(editor_tabs, &QTabWidget::tabCloseRequested, editor_tabs, &QTabWidget::removeTab);
        
        updateCreditDisplay();
    }
    
    void setupMenus() {
        auto file_menu = menuBar()->addMenu("&File");
        file_menu->addAction("&New", this, &XenoCodeWindow::newFile, QKeySequence::New);
        file_menu->addAction("&Open", this, &XenoCodeWindow::openFile, QKeySequence::Open);
        file_menu->addAction("&Save", this, &XenoCodeWindow::saveFile, QKeySequence::Save);
        file_menu->addSeparator();
        file_menu->addAction("&Quit", this, &QWidget::close, QKeySequence::Quit);
        
        auto edit_menu = menuBar()->addMenu("&Edit");
        edit_menu->addAction("&Undo", []() { /* TODO */ }, QKeySequence::Undo);
        edit_menu->addAction("&Redo", []() { /* TODO */ }, QKeySequence::Redo);
        edit_menu->addAction("&Find", []() { /* TODO */ }, QKeySequence::Find);
        
        auto ai_menu = menuBar()->addMenu("&AI Tools");
        ai_menu->addAction("Code Suggestion", this, &XenoCodeWindow::getCodeSuggestion, QKeySequence("Ctrl+Space"));
        ai_menu->addAction("Refactor Code", this, &XenoCodeWindow::refactorCode, QKeySequence("Ctrl+R"));
        ai_menu->addAction("Explain Code", this, &XenoCodeWindow::explainCode, QKeySequence("Ctrl+E"));
    }
    
    void setupToolbars() {
        auto main_toolbar = addToolBar("Main");
        main_toolbar->addAction("New", this, &XenoCodeWindow::newFile);
        main_toolbar->addAction("Open", this, &XenoCodeWindow::openFile);
        main_toolbar->addAction("Save", this, &XenoCodeWindow::saveFile);
        main_toolbar->addSeparator();
        main_toolbar->addAction("AI Suggest", this, &XenoCodeWindow::getCodeSuggestion);
        main_toolbar->addAction("Refactor", this, &XenoCodeWindow::refactorCode);
    }
    
    void setupConnections() {
        // Additional connections
    }
    
    void setupAutoCompletion(QPlainTextEdit* editor) {
        // Basic C++ keywords for autocompletion
        QStringList wordList;
        wordList << "class" << "struct" << "namespace" << "if" << "else" 
                 << "for" << "while" << "return" << "int" << "float" << "double"
                 << "void" << "bool" << "char" << "const" << "static"
                 << "public" << "private" << "protected" << "virtual";
        
        auto completer = new QCompleter(wordList, this);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        
        // Note: QPlainTextEdit doesn't have setCompleter, 
        // but we can implement custom completion logic here
    }
    
    void loadConfiguration() {
        std::string config_path = xeno::utils::Platform::getAppDataPath() + "/config.json";
        ai_integration->loadConfigFromFile(config_path);
    }
    
    void updateCreditDisplay() {
        if (ai_provider_combo->currentText() == "Ollama (Local)") {
            credit_status->setText("Local Mode - No credits required");
            credit_status->setStyleSheet("font-weight: bold; font-size: 14px; color: #27ae60;");
            return;
        }
        
        int balance = ai_integration->getCreditBalance();
        credit_status->setText(QString("Credits: %1").arg(balance));
        
        if (balance < 5) {
            credit_status->setStyleSheet("font-weight: bold; font-size: 14px; color: #e74c3c;");
        } else {
            credit_status->setStyleSheet("font-weight: bold; font-size: 14px; color: #27ae60;");
        }
    }
    
    void addSampleCode() {
        auto editor = new QPlainTextEdit;
        auto highlighter = new CppSyntaxHighlighter(editor->document());
        
        QString sample_code = 
            "#include <iostream>\n"
            "#include <vector>\n"
            "#include <memory>\n"
            "\n"
            "class XenoApplication {\n"
            "public:\n"
            "    XenoApplication() {\n"
            "        // Initialize Xeno AI integration\n"
            "        ai_integration = std::make_unique<AIIntegration>();\n"
            "    }\n"
            "    \n"
            "    void processWithAI(const std::string& input) {\n"
            "        // Use AI to process input\n"
            "        auto result = ai_integration->process(input);\n"
            "        std::cout << \"AI Result: \" << result << std::endl;\n"
            "    }\n"
            "    \n"
            "private:\n"
            "    std::unique_ptr<AIIntegration> ai_integration;\n"
            "};\n"
            "\n"
            "int main() {\n"
            "    XenoApplication app;\n"
            "    app.processWithAI(\"Hello Xeno AI!\");\n"
            "    return 0;\n"
            "}\n";
        
        editor->setPlainText(sample_code);
        
        int index = editor_tabs->addTab(editor, "sample.cpp");
        editor_tabs->setCurrentIndex(index);
        
        setupAutoCompletion(editor);
    }

private:
    std::unique_ptr<xeno::ai::AIIntegration> ai_integration;
    
    // UI elements
    QTabWidget* editor_tabs;
    QComboBox* ai_provider_combo;
    QPlainTextEdit* ai_output;
    QLabel* credit_status;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    QApplication::setApplicationName("Xeno Code");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("Xeno AI");
    
    xeno::utils::Logger::getInstance().info("Starting Xeno Code AI-Assisted IDE");
    
    XenoCodeWindow window;
    window.show();
    
    return app.exec();
}

#include "main.moc"