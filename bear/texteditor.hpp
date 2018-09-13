#ifndef BEAR_TEXTEDITOR_HPP
#define BEAR_TEXTEDITOR_HPP 1

#include <QWidget>
#include <QPlainTextEdit>

#include "editor.hpp"

class CodeEditor: public QPlainTextEdit{
		Q_OBJECT

	public:
		CodeEditor(QWidget *parent = nullptr);
		~CodeEditor();

		void lineNumberAreaPaintEvent(QPaintEvent *event);
		int lineNumberAreaWidth();

	protected:
		void resizeEvent(QResizeEvent *event) override;

	private slots:
		void updateLineNumberAreaWidth(int newBlockCount);
		void highlightCurrentLine();
		void updateLineNumberArea(const QRect &, int);

	private:
		QWidget *m_lineNumberArea;

		BearHighlighter m_highlighter;
};

class LineNumberArea: public QWidget{
	Q_OBJECT

	public:
		LineNumberArea(CodeEditor *editor)
			: QWidget(editor), m_codeEditor(editor){}

		QSize sizeHint() const override {
			return QSize(m_codeEditor->lineNumberAreaWidth(), 0);
		}

	protected:
		void paintEvent(QPaintEvent *event) override {
			m_codeEditor->lineNumberAreaPaintEvent(event);
		}

	private:
		CodeEditor *m_codeEditor;
};

#endif // !BEAR_TEXTEDITOR_HPP
