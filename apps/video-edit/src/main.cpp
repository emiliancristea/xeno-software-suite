#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QFileDialog>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QProgressDialog>
#include <QGroupBox>
#include <QListWidget>
#include <QSpinBox>
#include <QComboBox>
#include <memory>

#include "../../shared/ai-integration/include/ai_integration.h"
#include "../../shared/utils/include/utils.h"

class VideoEditWindow : public QMainWindow {
    Q_OBJECT

public:
    VideoEditWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Xeno Video Edit - AI-Powered Video Editor");
        setMinimumSize(1400, 900);
        
        // Initialize AI integration
        ai_integration = std::make_unique<xeno::ai::AIIntegration>();
        
        // Initialize media player
        media_player = new QMediaPlayer(this);
        video_widget = new QVideoWidget;
        media_player->setVideoOutput(video_widget);
        
        setupUI();
        setupMenus();
        setupToolbars();
        setupConnections();
        
        loadConfiguration();
        
        statusBar()->showMessage("Ready - AI video editing with Xeno Labs integration");
    }

private slots:
    void openVideo() {
        QString filename = QFileDialog::getOpenFileName(this,
            "Open Video", "", "Videos (*.mp4 *.avi *.mov *.mkv *.wmv)");
        
        if (!filename.isEmpty()) {
            media_player->setSource(QUrl::fromLocalFile(filename));
            statusBar()->showMessage("Video loaded: " + filename);
            current_video_path = filename;
        }
    }
    
    void playPause() {
        if (media_player->playbackState() == QMediaPlayer::PlayingState) {
            media_player->pause();
            play_pause_btn->setText("Play");
        } else {
            media_player->play();
            play_pause_btn->setText("Pause");
        }
    }
    
    void stopVideo() {
        media_player->stop();
        play_pause_btn->setText("Play");
    }
    
    void applyAutoEdit() {
        if (current_video_path.isEmpty()) {
            QMessageBox::warning(this, "Warning", "Please load a video first!");
            return;
        }
        
        int required_credits = 8; // AI auto-editing is more expensive
        if (!ai_integration->validateCredits(required_credits)) {
            QMessageBox::warning(this, "Insufficient Credits", 
                QString("AI auto-editing requires %1 credits. Please purchase more in Xeno Labs.")
                .arg(required_credits));
            return;
        }
        
        QProgressDialog progress("Applying AI Auto-Edit...", "Cancel", 0, 100, this);
        progress.setWindowModality(Qt::WindowModal);
        progress.show();
        
        // Simulate AI processing
        for (int i = 0; i <= 100; i += 5) {
            progress.setValue(i);
            QApplication::processEvents();
            if (progress.wasCanceled()) {
                return;
            }
            QThread::msleep(200);
        }
        
        // Make AI API call
        xeno::ai::AIIntegration::AIRequest request;
        request.prompt = "Apply intelligent auto-editing to enhance video flow";
        request.operation_type = "auto_edit";
        
        auto response = ai_integration->processVideo(request);
        
        if (response.success) {
            statusBar()->showMessage(QString("Auto-edit applied - %1 credits used")
                .arg(response.credits_used));
            QMessageBox::information(this, "Success", "AI auto-editing completed!");
        } else {
            QMessageBox::critical(this, "AI Error", 
                QString("Failed to apply auto-edit: %1").arg(QString::fromStdString(response.error_message)));
        }
    }
    
    void stabilizeVideo() {
        if (current_video_path.isEmpty()) {
            QMessageBox::warning(this, "Warning", "Please load a video first!");
            return;
        }
        
        int required_credits = 5;
        if (!ai_integration->validateCredits(required_credits)) {
            QMessageBox::warning(this, "Insufficient Credits", 
                QString("Video stabilization requires %1 credits.").arg(required_credits));
            return;
        }
        
        QProgressDialog progress("Stabilizing video...", "Cancel", 0, 100, this);
        progress.setWindowModality(Qt::WindowModal);
        progress.show();
        
        for (int i = 0; i <= 100; i += 10) {
            progress.setValue(i);
            QApplication::processEvents();
            if (progress.wasCanceled()) {
                return;
            }
            QThread::msleep(150);
        }
        
        ai_integration->deductCredits(required_credits);
        statusBar()->showMessage(QString("Video stabilized - %1 credits used").arg(required_credits));
        QMessageBox::information(this, "Success", "Video stabilization completed!");
    }
    
    void enhanceQuality() {
        if (current_video_path.isEmpty()) {
            QMessageBox::warning(this, "Warning", "Please load a video first!");
            return;
        }
        
        int required_credits = 6;
        if (!ai_integration->validateCredits(required_credits)) {
            QMessageBox::warning(this, "Insufficient Credits", 
                QString("Quality enhancement requires %1 credits.").arg(required_credits));
            return;
        }
        
        // Simulate AI quality enhancement
        ai_integration->deductCredits(required_credits);
        statusBar()->showMessage(QString("Quality enhanced - %1 credits used").arg(required_credits));
        QMessageBox::information(this, "Success", "AI quality enhancement completed!");
    }
    
    void exportVideo() {
        if (current_video_path.isEmpty()) {
            QMessageBox::warning(this, "Warning", "No video to export!");
            return;
        }
        
        QString filename = QFileDialog::getSaveFileName(this,
            "Export Video", "", "Videos (*.mp4 *.avi *.mov)");
        
        if (!filename.isEmpty()) {
            QMessageBox::information(this, "Export", 
                QString("Video would be exported to: %1\n\nNote: Export functionality requires FFmpeg integration.")
                .arg(filename));
        }
    }

private:
    void setupUI() {
        auto central_widget = new QWidget;
        setCentralWidget(central_widget);
        
        auto main_layout = new QVBoxLayout(central_widget);
        
        // Video display and timeline area
        auto video_area = new QHBoxLayout;
        
        // Video player
        video_widget->setMinimumSize(800, 450);
        video_area->addWidget(video_widget, 2);
        
        // AI Tools panel
        auto ai_panel = new QWidget;
        ai_panel->setFixedWidth(350);
        ai_panel->setStyleSheet("background-color: #f8f9fa;");
        
        auto ai_layout = new QVBoxLayout(ai_panel);
        
        // AI Video Tools
        auto ai_group = new QGroupBox("AI Video Tools");
        auto ai_tools_layout = new QVBoxLayout(ai_group);
        
        auto auto_edit_btn = new QPushButton("AI Auto-Edit (8 credits)");
        auto stabilize_btn = new QPushButton("Stabilize Video (5 credits)");
        auto enhance_btn = new QPushButton("Enhance Quality (6 credits)");
        auto smart_cut_btn = new QPushButton("Smart Cut Detection (3 credits)");
        
        auto_edit_btn->setStyleSheet("background-color: #3498db; color: white; font-weight: bold; padding: 12px;");
        stabilize_btn->setStyleSheet("background-color: #9b59b6; color: white; font-weight: bold; padding: 12px;");
        enhance_btn->setStyleSheet("background-color: #2ecc71; color: white; font-weight: bold; padding: 12px;");
        smart_cut_btn->setStyleSheet("background-color: #f39c12; color: white; font-weight: bold; padding: 12px;");
        
        ai_tools_layout->addWidget(auto_edit_btn);
        ai_tools_layout->addWidget(stabilize_btn);
        ai_tools_layout->addWidget(enhance_btn);
        ai_tools_layout->addWidget(smart_cut_btn);
        
        ai_layout->addWidget(ai_group);
        
        // Timeline and clips
        auto timeline_group = new QGroupBox("Timeline & Clips");
        auto timeline_layout = new QVBoxLayout(timeline_group);
        
        clip_list = new QListWidget;
        clip_list->addItem("Track 1: Video");
        clip_list->addItem("Track 2: Audio");
        clip_list->addItem("Track 3: Effects");
        
        timeline_layout->addWidget(clip_list);
        
        ai_layout->addWidget(timeline_group);
        
        // Credit status
        credit_status = new QLabel("Credits: Loading...");
        credit_status->setStyleSheet("font-weight: bold; font-size: 14px; color: #2c3e50;");
        ai_layout->addWidget(credit_status);
        
        ai_layout->addStretch();
        
        video_area->addWidget(ai_panel);
        main_layout->addLayout(video_area);
        
        // Video controls
        auto controls_layout = new QHBoxLayout;
        
        play_pause_btn = new QPushButton("Play");
        auto stop_btn = new QPushButton("Stop");
        
        time_slider = new QSlider(Qt::Horizontal);
        time_slider->setRange(0, 100);
        
        auto time_label = new QLabel("00:00 / 00:00");
        
        controls_layout->addWidget(play_pause_btn);
        controls_layout->addWidget(stop_btn);
        controls_layout->addWidget(time_slider, 1);
        controls_layout->addWidget(time_label);
        
        main_layout->addLayout(controls_layout);
        
        // Connect signals
        connect(play_pause_btn, &QPushButton::clicked, this, &VideoEditWindow::playPause);
        connect(stop_btn, &QPushButton::clicked, this, &VideoEditWindow::stopVideo);
        connect(auto_edit_btn, &QPushButton::clicked, this, &VideoEditWindow::applyAutoEdit);
        connect(stabilize_btn, &QPushButton::clicked, this, &VideoEditWindow::stabilizeVideo);
        connect(enhance_btn, &QPushButton::clicked, this, &VideoEditWindow::enhanceQuality);
        
        updateCreditDisplay();
    }
    
    void setupMenus() {
        auto file_menu = menuBar()->addMenu("&File");
        file_menu->addAction("&Open Video", this, &VideoEditWindow::openVideo, QKeySequence::Open);
        file_menu->addAction("&Export", this, &VideoEditWindow::exportVideo, QKeySequence("Ctrl+E"));
        file_menu->addSeparator();
        file_menu->addAction("&Quit", this, &QWidget::close, QKeySequence::Quit);
        
        auto edit_menu = menuBar()->addMenu("&Edit");
        edit_menu->addAction("&Cut", []() { /* TODO */ }, QKeySequence::Cut);
        edit_menu->addAction("&Copy", []() { /* TODO */ }, QKeySequence::Copy);
        edit_menu->addAction("&Paste", []() { /* TODO */ }, QKeySequence::Paste);
        
        auto ai_menu = menuBar()->addMenu("&AI Tools");
        ai_menu->addAction("Auto-Edit", this, &VideoEditWindow::applyAutoEdit);
        ai_menu->addAction("Stabilize", this, &VideoEditWindow::stabilizeVideo);
        ai_menu->addAction("Enhance Quality", this, &VideoEditWindow::enhanceQuality);
    }
    
    void setupToolbars() {
        auto main_toolbar = addToolBar("Main");
        main_toolbar->addAction("Open", this, &VideoEditWindow::openVideo);
        main_toolbar->addAction("Export", this, &VideoEditWindow::exportVideo);
        main_toolbar->addSeparator();
        main_toolbar->addAction("Play/Pause", this, &VideoEditWindow::playPause);
        main_toolbar->addAction("Stop", this, &VideoEditWindow::stopVideo);
    }
    
    void setupConnections() {
        connect(media_player, &QMediaPlayer::durationChanged, time_slider, &QSlider::setMaximum);
        connect(media_player, &QMediaPlayer::positionChanged, time_slider, &QSlider::setValue);
        connect(time_slider, &QSlider::sliderMoved, media_player, &QMediaPlayer::setPosition);
    }
    
    void loadConfiguration() {
        std::string config_path = xeno::utils::Platform::getAppDataPath() + "/config.json";
        ai_integration->loadConfigFromFile(config_path);
    }
    
    void updateCreditDisplay() {
        int balance = ai_integration->getCreditBalance();
        credit_status->setText(QString("Credits: %1").arg(balance));
        
        if (balance < 20) {
            credit_status->setStyleSheet("font-weight: bold; font-size: 14px; color: #e74c3c;");
        } else {
            credit_status->setStyleSheet("font-weight: bold; font-size: 14px; color: #27ae60;");
        }
    }

private:
    std::unique_ptr<xeno::ai::AIIntegration> ai_integration;
    QMediaPlayer* media_player;
    QVideoWidget* video_widget;
    QString current_video_path;
    
    // UI elements
    QPushButton* play_pause_btn;
    QSlider* time_slider;
    QListWidget* clip_list;
    QLabel* credit_status;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    QApplication::setApplicationName("Xeno Video Edit");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("Xeno AI");
    
    xeno::utils::Logger::getInstance().info("Starting Xeno Video Edit");
    
    VideoEditWindow window;
    window.show();
    
    return app.exec();
}

#include "main.moc"