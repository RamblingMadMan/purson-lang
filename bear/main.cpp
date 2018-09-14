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
	QGuiApplication::setOrganizationName("Purson");
	QGuiApplication::setOrganizationDomain("purson.io");
	QGuiApplication::setApplicationName("Bear");

	QSettings settings;

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

	QQuickWindow *root = qobject_cast<QQuickWindow*>(engine.rootObjects().at(0));
	auto project = root->findChild<QObject*>(QStringLiteral("project"));
	auto openDialog = root->findChild<QObject*>(QStringLiteral("openDialog"));
	auto tabBar = root->findChild<QQuickView*>(QStringLiteral("tabBar"));

	QObject *textEdit = root->findChild<QObject*>(QStringLiteral("textEdit"));
	QObject *lineNumbers = root->findChild<QObject*>(QStringLiteral("lineNumbers"));

	auto documentHandler = root->findChild<BearDocumentHandler*>(QStringLiteral("documentHandler"));
	auto projectHandler = root->findChild<BearProjectHandler*>(QStringLiteral("projectHandler"));
	auto sources = root->findChild<QObject*>(QStringLiteral("sourceList"));
	auto moduleHeader = root->findChild<QObject*>(QStringLiteral("moduleHeader"));

	QQuickTextDocument *quickTextDocument = textEdit->property("textDocument").value<QQuickTextDocument *>();
	QTextDocument *document = quickTextDocument->textDocument();

	namespace fs = std::filesystem;

	QWindow::Visibility currVis = root->visibility();

	project->connect(
		project,
		&QObject::destroyed,
		project,
		[&]{
			settings.setValue("openProject", projectHandler->dirUrl());
			settings.setValue("openFile", QString::fromStdString(fs::path(documentHandler->fileUrl().toLocalFile().toStdString()).filename()));
			settings.setValue("maximized", root->visibility() == QWindow::FullScreen);

			switch(root->visibility()){
				case QQuickWindow::Windowed: fmt::print("Windowed\n"); break;
				case QQuickWindow::FullScreen: fmt::print("Fullscreen\n"); break;
				case QQuickWindow::Maximized: fmt::print("Maximized\n"); break;
				case QQuickWindow::Minimized: fmt::print("Minimized\n"); break;
				case QQuickWindow::AutomaticVisibility: fmt::print("AutomaticVisibility\n"); break;
				case QQuickWindow::Hidden: fmt::print("Hidden\n"); break;

				default:
					fmt::print("unknown visibility\n");
			}

			if(root->visibility() != QWindow::FullScreen){
				settings.setValue("width", root->width());
				settings.setValue("height", root->height());
			}
			else{
				fmt::print("saving as maximized\n");
			}
		}
	);

	projectHandler->connect(
		projectHandler,
		&BearProjectHandler::dirUrlChanged,
		sources,
		[&](){
			moduleHeader->setProperty("text", QString::fromStdString(projectHandler->project()->name()));

			QStringList modulePaths;

			/*for(int i = 0; i < tabBar->count(); i++){
				tabBar->removeTab(i);
			}*/

			for(auto &&source : projectHandler->project()->modules()[0].sources()){
				auto dirStr = source.path().filename().string();
				//tabBar->addTab(nullptr, QString::fromStdString(dirStr));
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
					documentHandler->fileUrl().toLocalFile().toStdString(),
					document->toPlainText().toStdString()
				);

				auto exprs = purson::parse("dev", toks);
			}
			catch(...){
			}
		}
	);

	if(settings.contains("openProject")){
		auto projectDir = settings.value("openProject");
		projectHandler->openProject(projectDir.value<QUrl>());

		if(settings.contains("openFile")){
			auto openFile = settings.value("openFile");
			documentHandler->setFileUrl(projectDir.toString() + "/" + openFile.value<QUrl>().toString());
		}
	}
	else{
		openDialog->setProperty("visible", true);
	}

	auto handleGeometry = [&]{
		if(settings.contains("width")){
			auto width = settings.value("width");
			root->setWidth(width.value<int>());
		}

		if(settings.contains("height")){
			auto height = settings.value("height");
			root->setHeight(height.value<int>());
		}
	};

	if(settings.contains("maximized")){
		auto maximized = settings.value("maximized");
		if(maximized.value<bool>())
			root->setProperty("visibility", QWindow::Maximized);
		else
			handleGeometry();
	}
	else handleGeometry();

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
