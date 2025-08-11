#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QGridLayout>
#include <QProcess>
#include <QMessageBox>
#include <QTimer>
#include <QProgressBar>
#include <QGroupBox>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <memory>

#include "ai_integration.h"
#include "utils.h"

class XenoLauncher : public QMainWindow {
    Q_OBJECT

public:
    XenoLauncher(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Xeno Software Suite - Launcher");
        setMinimumSize(800, 600);
        
        // Initialize AI integration
        ai_integration = std::make_unique<xeno::ai::AIIntegration>();
        loadConfiguration();
        
        setupUI();
        setupConnections();
        updateCreditBalance();
        
        // Update credit balance every 30 seconds
        auto timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &XenoLauncher::updateCreditBalance);
        timer->start(30000);
    }

private slots:
    void launchImageEdit() {
        launchApplication("Image Edit", "xeno-image-edit");
    }
    
    void launchVideoEdit() {
        launchApplication("Video Edit", "xeno-video-edit");
    }
    
    void launchAudioEdit() {
        launchApplication("Audio Edit", "xeno-audio-edit");
    }
    
    void launchXenoCode() {
        launchApplication("Xeno Code", "xeno-code");
    }
    
    void checkForUpdates() {
        // Simulate update check
        QMessageBox::information(this, "Updates", "All applications are up to date!");
    }
    
    void updateCreditBalance() {
        int balance = ai_integration->getCreditBalance();
        credit_label->setText(QString("Credits: %1").arg(balance));
        
        // Update color based on balance
        if (balance < 10) {
            credit_label->setStyleSheet("color: red; font-weight: bold;");
        } else if (balance < 50) {
            credit_label->setStyleSheet("color: orange; font-weight: bold;");
        } else {
            credit_label->setStyleSheet("color: green; font-weight: bold;");
        }
    }

private:
    void setupUI() {
        auto central_widget = new QWidget;
        setCentralWidget(central_widget);
        
        auto main_layout = new QVBoxLayout(central_widget);
        
        // Header with title and credits
        auto header_layout = new QHBoxLayout;
        auto title_label = new QLabel("Xeno Software Suite");
        title_label->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50;");
        
        credit_label = new QLabel("Credits: Loading...");
        credit_label->setStyleSheet("font-size: 16px; font-weight: bold;");
        
        header_layout->addWidget(title_label);
        header_layout->addStretch();
        header_layout->addWidget(credit_label);
        
        main_layout->addLayout(header_layout);
        
        // Applications grid
        auto apps_group = new QGroupBox("AI-Enhanced Applications");
        auto apps_layout = new QGridLayout(apps_group);
        
        // Create application buttons
        image_edit_btn = createAppButton("Image Edit", 
            "AI-powered image editor with generative fill", 
            "📷");
        video_edit_btn = createAppButton("Video Edit", 
            "Video editor with auto-editing and stabilization", 
            "🎬");
        audio_edit_btn = createAppButton("Audio Edit", 
            "Audio editor with voice cloning and noise reduction", 
            "🎵");
        xeno_code_btn = createAppButton("Xeno Code", 
            "AI-assisted IDE with code suggestions", 
            "💻");
        
        apps_layout->addWidget(image_edit_btn, 0, 0);
        apps_layout->addWidget(video_edit_btn, 0, 1);
        apps_layout->addWidget(audio_edit_btn, 1, 0);
        apps_layout->addWidget(xeno_code_btn, 1, 1);
        
        main_layout->addWidget(apps_group);
        
        // Platform status
        auto status_group = new QGroupBox("Platform Status");
        auto status_layout = new QVBoxLayout(status_group);
        
        xeno_cloud_status = new QLabel("Xeno AI Cloud: Checking...");
        open_router_status = new QLabel("Open Router: Checking...");
        ollama_status = new QLabel("Ollama: Checking...");
        
        status_layout->addWidget(xeno_cloud_status);
        status_layout->addWidget(open_router_status);
        status_layout->addWidget(ollama_status);
        
        main_layout->addWidget(status_group);
        
        // Control buttons
        auto controls_layout = new QHBoxLayout;
        auto update_btn = new QPushButton("Check for Updates");
        auto settings_btn = new QPushButton("Settings");
        auto quit_btn = new QPushButton("Quit");
        
        controls_layout->addWidget(update_btn);
        controls_layout->addWidget(settings_btn);
        controls_layout->addStretch();
        controls_layout->addWidget(quit_btn);
        
        main_layout->addLayout(controls_layout);
        
        // Connect control buttons
        connect(update_btn, &QPushButton::clicked, this, &XenoLauncher::checkForUpdates);
        connect(quit_btn, &QPushButton::clicked, this, &QWidget::close);
    }
    
    QPushButton* createAppButton(const QString& name, const QString& description, const QString& icon) {
        auto button = new QPushButton;
        button->setMinimumSize(200, 120);
        button->setStyleSheet(
            "QPushButton {"
            "   background-color: #ecf0f1;"
            "   border: 2px solid #bdc3c7;"
            "   border-radius: 10px;"
            "   font-size: 14px;"
            "   text-align: left;"
            "   padding: 10px;"
            "}"
            "QPushButton:hover {"
            "   background-color: #d5dbdb;"
            "   border-color: #95a5a6;"
            "}"
            "QPushButton:pressed {"
            "   background-color: #bdc3c7;"
            "}"
        );
        
        button->setText(QString("%1 %2\n\n%3").arg(icon, name, description));
        return button;
    }
    
    void setupConnections() {
        connect(image_edit_btn, &QPushButton::clicked, this, &XenoLauncher::launchImageEdit);
        connect(video_edit_btn, &QPushButton::clicked, this, &XenoLauncher::launchVideoEdit);
        connect(audio_edit_btn, &QPushButton::clicked, this, &XenoLauncher::launchAudioEdit);
        connect(xeno_code_btn, &QPushButton::clicked, this, &XenoLauncher::launchXenoCode);
    }
    
    void loadConfiguration() {
        // Try to load configuration from app data directory
        std::string config_path = xeno::utils::Platform::getAppDataPath() + "/config.json";
        if (!ai_integration->loadConfigFromFile(config_path)) {
            xeno::utils::Logger::getInstance().warning("Could not load configuration from " + config_path);
        }
        
        // Check provider availability
        updateProviderStatus();
    }
    
    void updateProviderStatus() {
        // Update status labels based on provider availability
        if (ai_integration->isProviderAvailable(xeno::ai::AIIntegration::AIProvider::XenoCloud)) {
            xeno_cloud_status->setText("Xeno AI Cloud: ✅ Connected");
            xeno_cloud_status->setStyleSheet("color: green;");
        } else {
            xeno_cloud_status->setText("Xeno AI Cloud: ❌ Not configured");
            xeno_cloud_status->setStyleSheet("color: red;");
        }
        
        if (ai_integration->isProviderAvailable(xeno::ai::AIIntegration::AIProvider::OpenRouter)) {
            open_router_status->setText("Open Router: ✅ Connected");
            open_router_status->setStyleSheet("color: green;");
        } else {
            open_router_status->setText("Open Router: ❌ Not configured");
            open_router_status->setStyleSheet("color: orange;");
        }
        
        if (ai_integration->isProviderAvailable(xeno::ai::AIIntegration::AIProvider::Ollama)) {
            ollama_status->setText("Ollama: ✅ Available");
            ollama_status->setStyleSheet("color: green;");
        } else {
            ollama_status->setText("Ollama: ❌ Not available");
            ollama_status->setStyleSheet("color: orange;");
        }
    }
    
    void launchApplication(const QString& app_name, const QString& executable) {
        // For now, show a message since we haven't built the other apps yet
        QMessageBox::information(this, "Launch " + app_name, 
            QString("Launching %1...\n\nNote: This will execute '%2' when the application is built.")
            .arg(app_name, executable));
            
        // In production, this would use QProcess:
        // auto process = new QProcess(this);
        // process->start(executable);
    }

private:
    std::unique_ptr<xeno::ai::AIIntegration> ai_integration;
    
    // UI elements
    QLabel* credit_label;
    QPushButton* image_edit_btn;
    QPushButton* video_edit_btn;
    QPushButton* audio_edit_btn;
    QPushButton* xeno_code_btn;
    QLabel* xeno_cloud_status;
    QLabel* open_router_status;
    QLabel* ollama_status;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Set application properties
    QApplication::setApplicationName("Xeno Launcher");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("Xeno AI");
    
    // Initialize logging
    xeno::utils::Logger::getInstance().info("Starting Xeno Software Suite Launcher");
    
    // Create and show main window
    XenoLauncher launcher;
    launcher.show();
    
    return app.exec();
}

#include "main.moc"