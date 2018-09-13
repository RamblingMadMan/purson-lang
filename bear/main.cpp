//
// Created by testing on 9/2/18.
//

#include <iostream>

#include <QtWidgets>

#include <QGuiApplication>
#include <QQuickItem>
#include <QtQuickControls2>
#include <QQuickTextDocument>
#include <QtQml/QQmlApplicationEngine>

#include "fmt/format.h"

#include "window.hpp"
#include "project.hpp"
#include "document.hpp"
#include "texteditor.hpp"

int main(int argc, char *argv[]){
	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QGuiApplication app(argc, argv);

	int boldId = QFontDatabase::addApplicationFont(":/fonts/Monoid-Bold.ttf");
	int italicId = QFontDatabase::addApplicationFont(":/fonts/Monoid-Italic.ttf");
	int regularId = QFontDatabase::addApplicationFont(":/fonts/Monoid-Regular.ttf");
	int retinaId = QFontDatabase::addApplicationFont(":/fonts/Monoid-Retina.ttf");

	QQuickStyle::setStyle("Material");

	QQmlApplicationEngine engine;

	qmlRegisterType<CodeEditor>("Bear.Editor", 1, 0, "CodeEditor");
	qmlRegisterType<BearProjectHandler>("Bear.Editor", 1, 0, "ProjectHandler");
	qmlRegisterType<BearDocumentHandler>("Bear.Editor", 1, 0, "DocumentHandler");

	engine.load(QUrl("qrc:/main.qml"));

	QQmlComponent component(&engine, "ListElement");

	QObject *root = engine.rootObjects().at(0);

	QObject *textEdit = root->findChild<QObject*>(QStringLiteral("textEdit"));
	QObject *lineNumbers = root->findChild<QObject*>(QStringLiteral("lineNumbers"));

	auto documentHandler = root->findChild<BearDocumentHandler*>(QStringLiteral("document"));
	auto project = root->findChild<BearProjectHandler*>(QStringLiteral("project"));
	auto sources = root->findChild<QObject*>(QStringLiteral("sourceList"));
	auto moduleHeader = root->findChild<QObject*>(QStringLiteral("moduleHeader"));

	QQuickTextDocument *quickTextDocument = textEdit->property("textDocument").value<QQuickTextDocument *>();
	QTextDocument *document = quickTextDocument->textDocument();

	project->connect(
		project,
		&BearProjectHandler::dirUrlChanged,
		sources,
		[&](){
			fmt::print(stderr, "Project\n\tname: {}\n", project->project()->name());

			moduleHeader->setProperty("text", QString::fromStdString(project->project()->name()));

			QStringList modulePaths;

			for(auto &&source : project->project()->modules()[0].sources()){
				auto dirStr = source.path().filename().string();
				fmt::print(stderr, "\tmodule: {}\n", source.path().string());
				modulePaths.push_back(QString::fromStdString(dirStr));
			}

			sources->setProperty("model", QVariant::fromValue(modulePaths));
		}
	);

	document->connect(
		document,
		&QTextDocument::contentsChanged,
		lineNumbers,
		[documentHandler, document, lineNumbers](){
			if(document->lineCount() != lineNumbers->property("number").value<int>()){
				QString text = "";

				for(std::size_t i = 0; i < document->lineCount(); i++){
					text += QString::fromStdString(fmt::format("{}\n", i + 1));
					lineNumbers->setProperty("text", text);
				}

				lineNumbers->setProperty("number", document->lineCount());
			}

			try{
				auto toks = purson::lex(
					"dev",
					documentHandler->fileUrl().toString().toStdString(),
					document->toPlainText().toStdString()
				);

				auto exprs = purson::parse("dev", toks);
			}
			catch(...){
			}
		}
	);

	return app.exec();

	/*
	QApplication app(argc, argv);

	QFile f(":qdarkstyle/style.qss");
	if(f.exists()){
		f.open(QFile::ReadOnly | QFile::Text);
		app.setStyleSheet(f.readAll());
	}

	int boldId = QFontDatabase::addApplicationFont(":/fonts/Monoid-Bold.ttf");
	int italicId = QFontDatabase::addApplicationFont(":/fonts/Monoid-Italic.ttf");
	int regularId = QFontDatabase::addApplicationFont(":/fonts/Monoid-Regular.ttf");
	int retinaId = QFontDatabase::addApplicationFont(":/fonts/Monoid-Retina.ttf");

	BearMainWindow window;
	window.show();
	return app.exec();
	 */
}
