#ifndef MAIN_WINDOW
#define MAIN_WINDOW

#include <QWidget>
#include <QChildEvent>
#include <QVulkanWindow>

class MainWindow : public QWidget {
public: 
	explicit MainWindow(QVulkanWindow* window, QWidget* parent = nullptr);
};

#endif // MAIN_WINDOW