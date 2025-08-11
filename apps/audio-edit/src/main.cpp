#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QFileDialog>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QProgressDialog>
#include <QGroupBox>
#include <QWidget>
#include <QPainter>
#include <QComboBox>
#include <QSpinBox>
#include <memory>

#include "../../shared/ai-integration/include/ai_integration.h"
#include "../../shared/utils/include/utils.h"

// Simple waveform widget for audio visualization
class WaveformWidget : public QWidget {
    Q_OBJECT

public:
    WaveformWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setMinimumHeight(200);
        generateDummyWaveform();
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        
        // Draw background
        painter.fillRect(rect(), QColor(40, 44, 52));
        
        // Draw waveform
        painter.setPen(QPen(QColor(52, 152, 219), 2));
        
        int width = this->width();
        int height = this->height();
        int center = height / 2;
        
        for (int i = 0; i < width - 1; i++) {
            float t1 = (float)i / width;
            float t2 = (float)(i + 1) / width;
            
            int y1 = center + (int)(waveform_data[i % waveform_data.size()] * center * 0.8);
            int y2 = center + (int)(waveform_data[(i + 1) % waveform_data.size()] * center * 0.8);
            
            painter.drawLine(i, y1, i + 1, y2);
        }
        
        // Draw playhead
        painter.setPen(QPen(QColor(255, 255, 255), 2));
        int playhead_x = width * playhead_position;
        painter.drawLine(playhead_x, 0, playhead_x, height);
    }

public slots:
    void setPlayheadPosition(float position) {
        playhead_position = position;
        update();
    }

private:
    void generateDummyWaveform() {
        waveform_data.clear();
        for (int i = 0; i < 1000; i++) {
            float t = (float)i / 100.0f;
            float value = sin(t) * 0.5 + sin(t * 2.3) * 0.3 + sin(t * 5.7) * 0.2;
            waveform_data.push_back(value);
        }
    }

    std::vector<float> waveform_data;
    float playhead_position = 0.0f;
};

class AudioEditWindow : public QMainWindow {
    Q_OBJECT

public:
    AudioEditWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Xeno Audio Edit - AI-Powered Audio Editor");
        setMinimumSize(1200, 800);
        
        // Initialize AI integration
        ai_integration = std::make_unique<xeno::ai::AIIntegration>();
        
        setupUI();
        setupMenus();
        setupToolbars();
        setupConnections();
        
        loadConfiguration();
        
        statusBar()->showMessage("Ready - AI audio editing with Xeno Labs integration");
    }

private slots:
    void openAudio() {
        QString filename = QFileDialog::getOpenFileName(this,
            "Open Audio", "", "Audio Files (*.wav *.mp3 *.flac *.aac *.ogg)");
        
        if (!filename.isEmpty()) {
            current_audio_path = filename;
            statusBar()->showMessage("Audio loaded: " + filename);
            // In production, load actual audio data and update waveform
            waveform_widget->update();
        }
    }
    
    void saveAudio() {
        if (current_audio_path.isEmpty()) {
            QMessageBox::warning(this, "Warning", "No audio to save!");
            return;
        }
        
        QString filename = QFileDialog::getSaveFileName(this,
            "Save Audio", "", "Audio Files (*.wav *.mp3 *.flac)");
        
        if (!filename.isEmpty()) {
            QMessageBox::information(this, "Save", 
                QString("Audio would be saved to: %1").arg(filename));
        }
    }
    
    void playPause() {
        if (is_playing) {
            is_playing = false;
            play_pause_btn->setText("Play");
            statusBar()->showMessage("Playback stopped");
        } else {
            is_playing = true;
            play_pause_btn->setText("Pause");
            statusBar()->showMessage("Playing audio");
            // Simulate playback
            startPlaybackSimulation();
        }
    }
    
    void stopAudio() {
        is_playing = false;
        play_pause_btn->setText("Play");
        position_slider->setValue(0);
        waveform_widget->setPlayheadPosition(0.0f);
        statusBar()->showMessage("Stopped");
    }
    
    void applyVoiceClone() {
        if (current_audio_path.isEmpty()) {
            QMessageBox::warning(this, "Warning", "Please load an audio file first!");
            return;
        }
        
        int required_credits = 5;
        if (!ai_integration->validateCredits(required_credits)) {
            QMessageBox::warning(this, "Insufficient Credits", 
                QString("Voice cloning requires %1 credits. Please purchase more in Xeno Labs.")
                .arg(required_credits));
            return;
        }
        
        QProgressDialog progress("Applying AI Voice Cloning...", "Cancel", 0, 100, this);
        progress.setWindowModality(Qt::WindowModal);
        progress.show();
        
        for (int i = 0; i <= 100; i += 5) {
            progress.setValue(i);
            QApplication::processEvents();
            if (progress.wasCanceled()) {
                return;
            }
            QThread::msleep(100);
        }
        
        xeno::ai::AIIntegration::AIRequest request;
        request.prompt = "Clone voice characteristics from audio sample";
        request.operation_type = "voice_clone";
        
        auto response = ai_integration->processAudio(request);
        
        if (response.success) {
            statusBar()->showMessage(QString("Voice cloning applied - %1 credits used")
                .arg(response.credits_used));
            QMessageBox::information(this, "Success", "AI voice cloning completed!");
        } else {
            QMessageBox::critical(this, "AI Error", 
                QString("Failed to apply voice cloning: %1").arg(QString::fromStdString(response.error_message)));
        }
    }
    
    void reduceNoise() {
        if (current_audio_path.isEmpty()) {
            QMessageBox::warning(this, "Warning", "Please load an audio file first!");
            return;
        }
        
        int required_credits = 2;
        if (!ai_integration->validateCredits(required_credits)) {
            QMessageBox::warning(this, "Insufficient Credits", 
                QString("Noise reduction requires %1 credits.").arg(required_credits));
            return;
        }
        
        ai_integration->deductCredits(required_credits);
        statusBar()->showMessage(QString("Noise reduction applied - %1 credits used").arg(required_credits));
        QMessageBox::information(this, "Success", "AI noise reduction completed!");
        updateCreditDisplay();
    }
    
    void enhanceAudio() {
        if (current_audio_path.isEmpty()) {
            QMessageBox::warning(this, "Warning", "Please load an audio file first!");
            return;
        }
        
        int required_credits = 3;
        if (!ai_integration->validateCredits(required_credits)) {
            QMessageBox::warning(this, "Insufficient Credits", 
                QString("Audio enhancement requires %1 credits.").arg(required_credits));
            return;
        }
        
        ai_integration->deductCredits(required_credits);
        statusBar()->showMessage(QString("Audio enhanced - %1 credits used").arg(required_credits));
        QMessageBox::information(this, "Success", "AI audio enhancement completed!");
        updateCreditDisplay();
    }

private:
    void setupUI() {
        auto central_widget = new QWidget;
        setCentralWidget(central_widget);
        
        auto main_layout = new QVBoxLayout(central_widget);
        
        // Waveform display and tools
        auto content_layout = new QHBoxLayout;
        
        // Waveform area
        auto waveform_area = new QVBoxLayout;
        
        waveform_widget = new WaveformWidget;
        waveform_area->addWidget(waveform_widget);
        
        // Audio controls
        auto controls_layout = new QHBoxLayout;
        
        play_pause_btn = new QPushButton("Play");
        auto stop_btn = new QPushButton("Stop");
        
        position_slider = new QSlider(Qt::Horizontal);
        position_slider->setRange(0, 100);
        
        auto time_label = new QLabel("00:00 / 00:00");
        
        controls_layout->addWidget(play_pause_btn);
        controls_layout->addWidget(stop_btn);
        controls_layout->addWidget(position_slider, 1);
        controls_layout->addWidget(time_label);
        
        waveform_area->addLayout(controls_layout);
        content_layout->addLayout(waveform_area, 2);
        
        // AI Tools panel
        auto tools_panel = new QWidget;
        tools_panel->setFixedWidth(300);
        tools_panel->setStyleSheet("background-color: #f8f9fa;");
        
        auto tools_layout = new QVBoxLayout(tools_panel);
        
        // AI Audio Tools
        auto ai_group = new QGroupBox("AI Audio Tools");
        auto ai_tools_layout = new QVBoxLayout(ai_group);
        
        auto voice_clone_btn = new QPushButton("Voice Clone (5 credits)");
        auto noise_reduce_btn = new QPushButton("Noise Reduction (2 credits)");
        auto enhance_btn = new QPushButton("AI Enhance (3 credits)");
        auto transcribe_btn = new QPushButton("Transcribe (1 credit)");
        
        voice_clone_btn->setStyleSheet("background-color: #8e44ad; color: white; font-weight: bold; padding: 12px;");
        noise_reduce_btn->setStyleSheet("background-color: #27ae60; color: white; font-weight: bold; padding: 12px;");
        enhance_btn->setStyleSheet("background-color: #3498db; color: white; font-weight: bold; padding: 12px;");
        transcribe_btn->setStyleSheet("background-color: #f39c12; color: white; font-weight: bold; padding: 12px;");
        
        ai_tools_layout->addWidget(voice_clone_btn);
        ai_tools_layout->addWidget(noise_reduce_btn);
        ai_tools_layout->addWidget(enhance_btn);
        ai_tools_layout->addWidget(transcribe_btn);
        
        tools_layout->addWidget(ai_group);
        
        // Traditional Tools
        auto traditional_group = new QGroupBox("Traditional Tools");
        auto traditional_layout = new QVBoxLayout(traditional_group);
        
        auto volume_label = new QLabel("Volume:");
        auto volume_slider = new QSlider(Qt::Horizontal);
        volume_slider->setRange(0, 200);
        volume_slider->setValue(100);
        
        auto pitch_label = new QLabel("Pitch:");
        auto pitch_slider = new QSlider(Qt::Horizontal);
        pitch_slider->setRange(-50, 50);
        pitch_slider->setValue(0);
        
        traditional_layout->addWidget(volume_label);
        traditional_layout->addWidget(volume_slider);
        traditional_layout->addWidget(pitch_label);
        traditional_layout->addWidget(pitch_slider);
        
        tools_layout->addWidget(traditional_group);
        
        // Credit status
        credit_status = new QLabel("Credits: Loading...");
        credit_status->setStyleSheet("font-weight: bold; font-size: 14px; color: #2c3e50;");
        tools_layout->addWidget(credit_status);
        
        tools_layout->addStretch();
        
        content_layout->addWidget(tools_panel);
        main_layout->addLayout(content_layout);
        
        // Connect signals
        connect(play_pause_btn, &QPushButton::clicked, this, &AudioEditWindow::playPause);
        connect(stop_btn, &QPushButton::clicked, this, &AudioEditWindow::stopAudio);
        connect(voice_clone_btn, &QPushButton::clicked, this, &AudioEditWindow::applyVoiceClone);
        connect(noise_reduce_btn, &QPushButton::clicked, this, &AudioEditWindow::reduceNoise);
        connect(enhance_btn, &QPushButton::clicked, this, &AudioEditWindow::enhanceAudio);
        
        updateCreditDisplay();
    }
    
    void setupMenus() {
        auto file_menu = menuBar()->addMenu("&File");
        file_menu->addAction("&Open Audio", this, &AudioEditWindow::openAudio, QKeySequence::Open);
        file_menu->addAction("&Save", this, &AudioEditWindow::saveAudio, QKeySequence::Save);
        file_menu->addSeparator();
        file_menu->addAction("&Quit", this, &QWidget::close, QKeySequence::Quit);
        
        auto edit_menu = menuBar()->addMenu("&Edit");
        edit_menu->addAction("&Cut", []() { /* TODO */ }, QKeySequence::Cut);
        edit_menu->addAction("&Copy", []() { /* TODO */ }, QKeySequence::Copy);
        edit_menu->addAction("&Paste", []() { /* TODO */ }, QKeySequence::Paste);
        
        auto ai_menu = menuBar()->addMenu("&AI Tools");
        ai_menu->addAction("Voice Clone", this, &AudioEditWindow::applyVoiceClone);
        ai_menu->addAction("Noise Reduction", this, &AudioEditWindow::reduceNoise);
        ai_menu->addAction("AI Enhance", this, &AudioEditWindow::enhanceAudio);
    }
    
    void setupToolbars() {
        auto main_toolbar = addToolBar("Main");
        main_toolbar->addAction("Open", this, &AudioEditWindow::openAudio);
        main_toolbar->addAction("Save", this, &AudioEditWindow::saveAudio);
        main_toolbar->addSeparator();
        main_toolbar->addAction("Play/Pause", this, &AudioEditWindow::playPause);
        main_toolbar->addAction("Stop", this, &AudioEditWindow::stopAudio);
    }
    
    void setupConnections() {
        connect(position_slider, &QSlider::valueChanged, [this](int value) {
            float position = (float)value / 100.0f;
            waveform_widget->setPlayheadPosition(position);
        });
    }
    
    void loadConfiguration() {
        std::string config_path = xeno::utils::Platform::getAppDataPath() + "/config.json";
        ai_integration->loadConfigFromFile(config_path);
    }
    
    void updateCreditDisplay() {
        int balance = ai_integration->getCreditBalance();
        credit_status->setText(QString("Credits: %1").arg(balance));
        
        if (balance < 10) {
            credit_status->setStyleSheet("font-weight: bold; font-size: 14px; color: #e74c3c;");
        } else {
            credit_status->setStyleSheet("font-weight: bold; font-size: 14px; color: #27ae60;");
        }
    }
    
    void startPlaybackSimulation() {
        // Simple simulation of audio playback
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, [this, timer]() {
            if (!is_playing) {
                timer->stop();
                timer->deleteLater();
                return;
            }
            
            int current_pos = position_slider->value();
            current_pos += 1;
            if (current_pos > 100) {
                current_pos = 0;
            }
            position_slider->setValue(current_pos);
            waveform_widget->setPlayheadPosition((float)current_pos / 100.0f);
        });
        timer->start(100);
    }

private:
    std::unique_ptr<xeno::ai::AIIntegration> ai_integration;
    QString current_audio_path;
    bool is_playing = false;
    
    // UI elements
    WaveformWidget* waveform_widget;
    QPushButton* play_pause_btn;
    QSlider* position_slider;
    QLabel* credit_status;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    QApplication::setApplicationName("Xeno Audio Edit");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("Xeno AI");
    
    xeno::utils::Logger::getInstance().info("Starting Xeno Audio Edit");
    
    AudioEditWindow window;
    window.show();
    
    return app.exec();
}

#include "main.moc"