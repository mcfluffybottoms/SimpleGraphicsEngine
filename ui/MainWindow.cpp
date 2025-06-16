#include <QVBoxLayout>
#include <qobject.h>
#include "MainWindow.h"
#include <QVBoxLayout>
#include "QVulkanMainWindow.h"
#include <QPushButton>
MainWindow::MainWindow(QVulkanWindow* window, QWidget* parent) : QWidget(parent)
{
    QWidget *wrapper = QWidget::createWindowContainer(window);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(wrapper);
    
    setLayout(layout);
    setWindowTitle("Qt + Vulkan ECS Renderer");
    resize(1280, 720);
}