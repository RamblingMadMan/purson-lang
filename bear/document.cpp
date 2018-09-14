#include <stdexcept>

#include <QTextStream>
#include <fmt/core.h>

#include "document.hpp"

QQuickTextDocument *BearDocumentHandler::textDocument() const{ return m_textDocument; }

void BearDocumentHandler::setTextDocument(QQuickTextDocument *arg){
	if(m_textDocument != arg){
		m_textDocument = arg;
		m_highlighter.setDocument(m_textDocument->textDocument());
		m_highlighter.rehighlight();
		emit textDocumentChanged();
	}
}

QString BearDocumentHandler::documentTitle() const{ return m_documentTitle; }

void BearDocumentHandler::setDocumentTitle(QString arg){
	if(m_documentTitle != arg){
		m_documentTitle = arg;
		emit documentTitleChanged();
	}
}

QString BearDocumentHandler::text() const{
	return m_text;
}

void BearDocumentHandler::setText(const QString &arg){
	m_text = arg;
	if(m_textDocument){
		m_textDocument->textDocument()->setPlainText(m_text);
		m_highlighter.rehighlight();
	}

	emit textChanged();
}

QUrl BearDocumentHandler::fileUrl() const{
	return m_fileUrl;
}

void BearDocumentHandler::setFileUrl(const QUrl &arg){
	if(m_fileUrl != arg){
		m_fileUrl = arg;

		std::filesystem::path filepath(arg.toLocalFile().toStdString());

		QString fileName = arg.fileName();
		QFile file(arg.toLocalFile());

		if(file.open(QFile::ReadOnly)){
			setText(QString(file.readAll()));
			if(fileName.isEmpty())
				m_documentTitle = QStringLiteral("untitled");
			else
				m_documentTitle = QString::fromStdString(filepath.filename());

			emit documentTitleChanged();
		}
		else{
			throw std::runtime_error{fmt::format("couldn't load file {}", fileName.toStdString())};
		}

		emit fileUrlChanged();
	}
}

void BearDocumentHandler::saveFile(const QUrl &arg) const{
	QFile file(arg.toLocalFile());

	if(file.open(QFile::WriteOnly | QFile::Truncate)){
		if(m_textDocument){
			QTextStream out(&file);
			out << m_textDocument->textDocument()->toPlainText();
		}
	}
}

void BearDocumentHandler::rehighlight() const{
	if(m_textDocument) m_highlighter.rehighlight();
}
