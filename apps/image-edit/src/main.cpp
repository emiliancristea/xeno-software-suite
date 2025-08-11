#include <QThread>
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QFileDialog>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QProgressDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QGroupBox>
#include <QSlider>
#include <opencv2/opencv.hpp>
#include <memory>

#include "ai_integration.h"
#include "utils.h"

class ImageEditWindow : public QMainWindow {
    Q_OBJECT

public:
    ImageEditWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Xeno Image Edit - AI-Powered Image Editor");
        setMinimumSize(1200, 800);
        
        // Initialize AI integration
        ai_integration = std::make_unique<xeno::ai::AIIntegration>();
        
        setupUI();
        setupMenus();
        setupToolbars();
        setupConnections();
        
        // Load configuration
        loadConfiguration();
        
        statusBar()->showMessage("Ready - Integrate with Xeno Labs credit wallet");
    }

private slots:
    void openImage() {
        QString filename = QFileDialog::getOpenFileName(this,
            "Open Image", "", "Images (*.png *.jpg *.jpeg *.bmp *.tiff)");
        
        if (!filename.isEmpty()) {
            loadImage(filename);
        }
    }
    
    void saveImage() {
        if (current_image.empty()) {
            QMessageBox::warning(this, "Warning", "No image to save!");
            return;
        }
        
        QString filename = QFileDialog::getSaveFileName(this,
            "Save Image", "", "Images (*.png *.jpg *.jpeg *.bmp *.tiff)");
        
        if (!filename.isEmpty()) {
            cv::imwrite(filename.toStdString(), current_image);
            statusBar()->showMessage("Image saved successfully");
        }
    }
    
    void applyGenerativeFill() {
        if (current_image.empty()) {
            QMessageBox::warning(this, "Warning", "Please load an image first!");
            return;
        }
        
        // Check credits before proceeding
        int required_credits = 3;
        if (!ai_integration->validateCredits(required_credits)) {
            QMessageBox::warning(this, "Insufficient Credits", 
                QString("This operation requires %1 credits. Please purchase more credits in Xeno Labs.")
                .arg(required_credits));
            return;
        }
        
        QProgressDialog progress("Applying AI Generative Fill...", "Cancel", 0, 100, this);
        progress.setWindowModality(Qt::WindowModal);
        progress.show();
        
        // Simulate AI processing
        for (int i = 0; i <= 100; i += 10) {
            progress.setValue(i);
            QApplication::processEvents();
            if (progress.wasCanceled()) {
                return;
            }
            QThread::msleep(100);
        }
        
        // Make AI API call
        xeno::ai::AIIntegration::AIRequest request;
        request.prompt = "Apply generative fill to enhance image";
        request.operation_type = "generative_fill";
        
        auto response = ai_integration->generateImage(request);
        
        if (response.success) {
            // Apply a simple filter as simulation (in production, this would be the AI result)
            cv::Mat filtered;
            cv::GaussianBlur(current_image, filtered, cv::Size(15, 15), 0);
            cv::addWeighted(current_image, 0.7, filtered, 0.3, 0, current_image);
            
            updateImageDisplay();
            statusBar()->showMessage(QString("Generative fill applied - %1 credits used")
                .arg(response.credits_used));
        } else {
            QMessageBox::critical(this, "AI Error", 
                QString("Failed to apply generative fill: %1").arg(QString::fromStdString(response.error_message)));
        }
    }
    
    void removeObject() {
        if (current_image.empty()) {
            QMessageBox::warning(this, "Warning", "Please load an image first!");
            return;
        }
        
        int required_credits = 2;
        if (!ai_integration->validateCredits(required_credits)) {
            QMessageBox::warning(this, "Insufficient Credits", 
                QString("This operation requires %1 credits.").arg(required_credits));
            return;
        }
        
        // Simulate object removal with inpainting
        cv::Mat mask = cv::Mat::zeros(current_image.size(), CV_8UC1);
        cv::circle(mask, cv::Point(current_image.cols/2, current_image.rows/2), 50, cv::Scalar(255), -1);
        
        cv::Mat result;
        cv::inpaint(current_image, mask, result, 3, cv::INPAINT_TELEA);
        current_image = result;
        
        updateImageDisplay();
        
        // Deduct credits
        ai_integration->deductCredits(required_credits);
        statusBar()->showMessage(QString("Object removed - %1 credits used").arg(required_credits));
    }
    
    void enhanceImage() {
        if (current_image.empty()) {
            QMessageBox::warning(this, "Warning", "Please load an image first!");
            return;
        }
        
        // Apply basic enhancement (contrast, brightness)
        cv::Mat enhanced;
        current_image.convertTo(enhanced, -1, 1.2, 30); // alpha=1.2 (contrast), beta=30 (brightness)
        current_image = enhanced;
        
        updateImageDisplay();
        statusBar()->showMessage("Image enhanced");
    }

private:
    void setupUI() {
        auto central_widget = new QWidget;
        setCentralWidget(central_widget);
        
        auto main_layout = new QHBoxLayout(central_widget);
        
        // Image display area
        graphics_view = new QGraphicsView;
        graphics_scene = new QGraphicsScene;
        graphics_view->setScene(graphics_scene);
        graphics_view->setDragMode(QGraphicsView::ScrollHandDrag);
        
        main_layout->addWidget(graphics_view, 3);
        
        // Tools panel
        auto tools_panel = new QWidget;
        tools_panel->setFixedWidth(300);
        tools_panel->setStyleSheet("background-color: #f8f9fa;");
        
        auto tools_layout = new QVBoxLayout(tools_panel);
        
        // AI Tools group
        auto ai_group = new QGroupBox("AI Tools");
        auto ai_layout = new QVBoxLayout(ai_group);
        
        auto generative_fill_btn = new QPushButton("Generative Fill (3 credits)");
        auto object_removal_btn = new QPushButton("Remove Object (2 credits)");
        auto enhance_btn = new QPushButton("AI Enhance (1 credit)");
        
        generative_fill_btn->setStyleSheet("background-color: #3498db; color: white; font-weight: bold; padding: 10px;");
        object_removal_btn->setStyleSheet("background-color: #e74c3c; color: white; font-weight: bold; padding: 10px;");
        enhance_btn->setStyleSheet("background-color: #2ecc71; color: white; font-weight: bold; padding: 10px;");
        
        ai_layout->addWidget(generative_fill_btn);
        ai_layout->addWidget(object_removal_btn);
        ai_layout->addWidget(enhance_btn);
        
        tools_layout->addWidget(ai_group);
        
        // Traditional Tools group
        auto traditional_group = new QGroupBox("Traditional Tools");
        auto traditional_layout = new QVBoxLayout(traditional_group);
        
        auto brightness_label = new QLabel("Brightness:");
        auto brightness_slider = new QSlider(Qt::Horizontal);
        brightness_slider->setRange(-100, 100);
        brightness_slider->setValue(0);
        
        auto contrast_label = new QLabel("Contrast:");
        auto contrast_slider = new QSlider(Qt::Horizontal);
        contrast_slider->setRange(-100, 100);
        contrast_slider->setValue(0);
        
        traditional_layout->addWidget(brightness_label);
        traditional_layout->addWidget(brightness_slider);
        traditional_layout->addWidget(contrast_label);
        traditional_layout->addWidget(contrast_slider);
        
        tools_layout->addWidget(traditional_group);
        
        // Credit status
        credit_status = new QLabel("Credits: Loading...");
        credit_status->setStyleSheet("font-weight: bold; font-size: 14px; color: #2c3e50;");
        tools_layout->addWidget(credit_status);
        
        tools_layout->addStretch();
        
        main_layout->addWidget(tools_panel);
        
        // Connect AI tool buttons
        connect(generative_fill_btn, &QPushButton::clicked, this, &ImageEditWindow::applyGenerativeFill);
        connect(object_removal_btn, &QPushButton::clicked, this, &ImageEditWindow::removeObject);
        connect(enhance_btn, &QPushButton::clicked, this, &ImageEditWindow::enhanceImage);
        
        // Update credit display
        updateCreditDisplay();
    }
    
    void setupMenus() {
        auto file_menu = menuBar()->addMenu("&File");
        file_menu->addAction("&Open", this, &ImageEditWindow::openImage, QKeySequence::Open);
        file_menu->addAction("&Save", this, &ImageEditWindow::saveImage, QKeySequence::Save);
        file_menu->addSeparator();
        file_menu->addAction("&Quit", this, &QWidget::close, QKeySequence::Quit);
        
        auto edit_menu = menuBar()->addMenu("&Edit");
        edit_menu->addAction("&Undo", []() { /* TODO */ }, QKeySequence::Undo);
        edit_menu->addAction("&Redo", []() { /* TODO */ }, QKeySequence::Redo);
        
        auto ai_menu = menuBar()->addMenu("&AI Tools");
        ai_menu->addAction("Generative Fill", this, &ImageEditWindow::applyGenerativeFill);
        ai_menu->addAction("Remove Object", this, &ImageEditWindow::removeObject);
        ai_menu->addAction("AI Enhance", this, &ImageEditWindow::enhanceImage);
    }
    
    void setupToolbars() {
        auto main_toolbar = addToolBar("Main");
        main_toolbar->addAction("Open", this, &ImageEditWindow::openImage);
        main_toolbar->addAction("Save", this, &ImageEditWindow::saveImage);
        main_toolbar->addSeparator();
        main_toolbar->addAction("Generative Fill", this, &ImageEditWindow::applyGenerativeFill);
        main_toolbar->addAction("Remove Object", this, &ImageEditWindow::removeObject);
    }
    
    void setupConnections() {
        // Additional connections can be added here
    }
    
    void loadConfiguration() {
        std::string config_path = xeno::utils::Platform::getAppDataPath() + "/config.json";
        ai_integration->loadConfigFromFile(config_path);
    }
    
    void loadImage(const QString& filename) {
        current_image = cv::imread(filename.toStdString());
        if (current_image.empty()) {
            QMessageBox::critical(this, "Error", "Failed to load image!");
            return;
        }
        
        updateImageDisplay();
        statusBar()->showMessage("Image loaded: " + filename);
    }
    
    void updateImageDisplay() {
        if (current_image.empty()) return;
        
        // Convert OpenCV Mat to QPixmap
        cv::Mat rgb_image;
        cv::cvtColor(current_image, rgb_image, cv::COLOR_BGR2RGB);
        
        QImage qimg(rgb_image.data, rgb_image.cols, rgb_image.rows, rgb_image.step, QImage::Format_RGB888);
        QPixmap pixmap = QPixmap::fromImage(qimg);
        
        graphics_scene->clear();
        graphics_scene->addPixmap(pixmap);
        graphics_scene->setSceneRect(pixmap.rect());
        graphics_view->fitInView(graphics_scene->itemsBoundingRect(), Qt::KeepAspectRatio);
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

private:
    std::unique_ptr<xeno::ai::AIIntegration> ai_integration;
    cv::Mat current_image;
    
    // UI elements
    QGraphicsView* graphics_view;
    QGraphicsScene* graphics_scene;
    QLabel* credit_status;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    QApplication::setApplicationName("Xeno Image Edit");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("Xeno AI");
    
    xeno::utils::Logger::getInstance().info("Starting Xeno Image Edit");
    
    ImageEditWindow window;
    window.show();
    
    return app.exec();
}

#include "main.moc"