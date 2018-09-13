#ifndef BEAR_WINDOW_HPP
#define BEAR_WINDOW_HPP 1

#include <QMainWindow>
#include <QTabWidget>
#include <QHBoxLayout>

#include "editor.hpp"

class BearMainWindow: public QMainWindow{
		Q_OBJECT

	public:
		BearMainWindow(QWidget *parent = nullptr);

	private:
		std::unique_ptr<BearEditor> m_editor;
};

#endif // !BEAR_WINDOW_HPP
