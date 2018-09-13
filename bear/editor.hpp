#ifndef BEAR_EDITOR_HPP
#define BEAR_EDITOR_HPP 1

#include <filesystem>
#include <set>

#include <QToolBar>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QTreeWidget>
#include <QSyntaxHighlighter>
#include <QPlainTextEdit>

#include "purson/lexer.hpp"
#include "purson/parser.hpp"

#include "project.hpp"

class BearHighlighter: public QSyntaxHighlighter{
		Q_OBJECT

	public:
		BearHighlighter(QTextDocument *parent = nullptr);

	protected:
		void highlightBlock(const QString &text) override;

	private:
		QTextCharFormat regularFormat, kwFormat, commentFormat, stringFormat, numFormat;
};

class BearEditor: public QWidget{
		Q_OBJECT

	public:
		BearEditor(const std::filesystem::path &projectDir, QWidget *parent = nullptr);

		~BearEditor();

	protected slots:
		void saveFile();
		void openFile(const std::filesystem::path &p);
		void updateTab();

	private:
		void addModule(BearProjectModule &mod);

		BearProject m_project;

		QHBoxLayout m_hbox;

		QToolBar m_toolbar;

		QTreeWidget m_moduleTree;
		QTabWidget m_tabs;
		BearHighlighter m_highlighter;

		mutable std::set<std::unique_ptr<QTreeWidgetItem>> m_moduleTreeItems;
		mutable std::set<std::pair<std::unique_ptr<QFile>, std::unique_ptr<QPlainTextEdit>>> m_files;
};

#endif // !BEAR_EDITOR_HPP
