#ifndef BEAR_DOCUMENT_HPP
#define BEAR_DOCUMENT_HPP 1

#include <QObject>
#include <QtQuickControls2>

#include "purson/expressions.hpp"

#include "editor.hpp"

class SourceFunctionParam: public QObject{
		Q_OBJECT

		Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
		Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)

	public:
		SourceFunctionParam(const SourceFunctionParam&) = default;

		QString name() const { return m_name; }
		QString type() const { return m_type; }

	public slots:
		void setName(const QString &name){ m_name = name; emit nameChanged(); }
		void setType(const QString &type){ m_type = type; emit typeChanged(); }

	signals:
		void nameChanged();
		void typeChanged();

	private:
		QString m_name, m_type;
};

class SourceFunction: public QObject{
		Q_OBJECT

		Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
		Q_PROPERTY(QVector<SourceFunctionParam*> params READ params WRITE setParams NOTIFY paramsChanged)

	public:
		QString name() const noexcept{ return m_name; }
		const QVector<SourceFunctionParam*> &params() const noexcept{ return m_params; }

	public slots:
		void setName(const QString &name_){
			if(m_name != name_){
				m_name = name_;
				emit nameChanged();
			}
		};

		void setParams(QVector<SourceFunctionParam*> params_){
			m_params = std::move(params_);
			emit paramsChanged();
		}

	signals:
		void nameChanged();
		void paramsChanged();

	private:
		QString m_name;
		QVector<SourceFunctionParam*> m_params;
};

class BearDocumentHandler: public QObject{
		Q_OBJECT

		Q_PROPERTY(QQuickTextDocument* textDocument READ textDocument WRITE setTextDocument NOTIFY textDocumentChanged)
		Q_PROPERTY(QUrl fileUrl READ fileUrl WRITE setFileUrl NOTIFY fileUrlChanged)
		Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
		Q_PROPERTY(QString documentTitle READ documentTitle WRITE setDocumentTitle NOTIFY documentTitleChanged)
		Q_PROPERTY(QVector<SourceFunction*> functions READ functions WRITE setFunctions NOTIFY functionsChanged)

	public:
		QQuickTextDocument *textDocument() const;
		QUrl fileUrl() const;
		QString text() const;
		QString documentTitle() const;
		const QVector<SourceFunction*> &functions() const noexcept{ return m_fns; }

	public slots:
		void setFileUrl(const QUrl &arg);
		void setText(const QString &arg);
		void setTextDocument(QQuickTextDocument *arg);
		void setDocumentTitle(QString arg);

		void setFunctions(QVector<SourceFunction*> fns){
			m_fns = std::move(fns);
			emit functionsChanged();
		}

		Q_INVOKABLE void saveFile(const QUrl &arg) const;
		Q_INVOKABLE void rehighlight() const;

	signals:
		void fileUrlChanged();
		void textChanged();
		void textDocumentChanged();
		void documentTitleChanged();
		void functionsChanged();

	private:
		mutable QQuickTextDocument *m_textDocument;
		QUrl m_fileUrl;
		QString m_text;
		QString m_documentTitle;
		QVector<SourceFunction*> m_fns;

		mutable BearHighlighter m_highlighter;
};

#endif // !BEAR_DOCUMENT_HPP
