#include <QColor>
#include <QLayout>

#include "fmt/format.h"

#include "editor.hpp"

BearHighlighter::BearHighlighter(QTextDocument *parent)
	: QSyntaxHighlighter(parent){
	regularFormat.setForeground(QColor(222, 222, 222, 255));
	kwFormat.setForeground(QColor(226, 112, 255, 255));
	opFormat.setForeground(QColor(137, 182, 255, 255));
	commentFormat.setForeground(Qt::green);
	stringFormat.setForeground(Qt::yellow);
	numFormat.setForeground(QColor(255, 246, 81, 255));
}

void BearHighlighter::highlightBlock(const QString &text){
	try{
		auto tokens = purson::lex("dev", "editorSrc", text.toUtf8().data());

		for(auto &&tok : tokens){
			QTextCharFormat *format = &regularFormat;
			switch(tok.type()){
				case purson::token_type::keyword:{
					format = &kwFormat;
					break;
				}

				case purson::token_type::op:{
					format = &opFormat;
					break;
				}

				case purson::token_type::integer:
				case purson::token_type::real:{
					format = &numFormat;
					break;
				}

				case purson::token_type::string:{
					format = &stringFormat;
					break;
				}

				default:
					break;
			}

			setFormat(tok.loc().col() + tok.loc().line() - 2, tok.loc().len(), *format);
		}
	}
	catch(const purson::lexer_error &err){
		fmt::print("lexer error: {}\n", err.what());
	}
	catch(...){
		throw;
	}
}

BearEditor::BearEditor(const std::filesystem::path &projectDir, QWidget *parent)
	: QWidget(parent), m_project{projectDir}, m_hbox{this}{
	m_moduleTree.setMaximumWidth(200);
	m_hbox.addWidget(&m_toolbar);
	m_hbox.addWidget(&m_moduleTree);
	m_hbox.addWidget(&m_tabs);
	//this->layout()->setContentsMargins(0, 0, 0, 0);

	for(auto &&mod : m_project.modules()){
		addModule(mod);
	}

	openFile("untitled.pur");
	openFile("gl.pur");
	connect(&m_tabs, &QTabWidget::currentChanged, this, &BearEditor::updateTab);
}

BearEditor::~BearEditor(){}

void BearEditor::addModule(BearProjectModule &mod){
	auto ptr = std::make_unique<QTreeWidgetItem>(&m_moduleTree);
	ptr->setText(0, QString::fromStdString(std::string(mod.name())));

	for(auto &&src : mod.sources()){
		auto newPtr = new QTreeWidgetItem(ptr.get());
		newPtr->setText(0, QString::fromStdString(std::string(src.path())));
		ptr->addChild(newPtr);
	}

	auto &&item = m_moduleTreeItems.emplace(std::move(ptr));
}

void BearEditor::saveFile(){

}

void BearEditor::openFile(const std::filesystem::path &p){
	for(auto &&file : m_files){
		if(file.first->fileName() == QString{p.c_str()})
			return;
	}

	auto file = m_files.insert(
		end(m_files),
		std::make_pair(
			std::make_unique<QFile>(QString{p.c_str()}),
			std::make_unique<QPlainTextEdit>(&m_tabs)
		)
	);

	QFont font;
	font.setFamily("Monoid");
	font.setFixedPitch(true);
	font.setPointSize(12);

	if((*file->first).exists()){
		(*file->first).open(QFile::ReadWrite | QFile::Text);
		file->second->setPlainText(file->first->readAll());

		(*file->second).setFont(font);
		QFontMetrics metrics(font);
		(*file->second).setTabStopWidth(4 * metrics.width(' '));

		m_tabs.addTab(file->second.get(), file->first->fileName());
		m_tabs.setCurrentIndex(m_tabs.count() - 1);

		m_highlighter.setDocument((*file->second).document());
	}
	else
		m_files.erase(file);
}

void BearEditor::updateTab(){
	m_highlighter.setDocument(dynamic_cast<QPlainTextEdit*>(m_tabs.currentWidget())->document());
}
