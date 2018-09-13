#include "window.hpp"

BearMainWindow::BearMainWindow(QWidget *parent)
	: QMainWindow(parent), m_editor(new BearEditor("test", this)){
	resize(1280, 720);
	setWindowTitle(tr("Purson's Bear"));
	//m_hbox.addWidget(&m_editor);
	//m_widget.setLayout(&m_hbox);

	setCentralWidget(m_editor.get());
}
