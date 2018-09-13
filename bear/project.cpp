#include <stdexcept>

#include <QXmlStreamReader>
#include <QFile>

#include "fmt/format.h"

#include "project.hpp"

QUrl BearProjectHandler::dirUrl() const{ return m_dirUrl; }

void BearProjectHandler::setDirUrl(const QUrl &arg){
	if(m_dirUrl != arg){
		m_dirUrl = arg;
		m_project = std::make_unique<BearProject>(std::filesystem::path(arg.toLocalFile().toStdString()));

		emit dirUrlChanged();
	}
}

const QVector<QUrl>& BearProjectHandler::modulePaths() const{ return m_modulePaths; }

void BearProjectHandler::setModulePaths(const QVector<QUrl> &arg){
	m_modulePaths = arg;
	emit modulePathsChanged();
}

void BearProjectHandler::openProject(const QUrl &dir){
	std::filesystem::path path = dir.toLocalFile().toStdString();

	setDirUrl(dir);
}

BearProject::BearProject(const std::filesystem::path &dir_): m_dir{dir_}{
	if(!std::filesystem::exists(dir_) || !std::filesystem::is_directory(dir_))
		throw std::runtime_error{fmt::format("project directory {} doesn't exist", dir_.string())};

	std::filesystem::path projectFilePath = dir_ / "project.bear";

	if(!std::filesystem::exists(projectFilePath))
		throw std::runtime_error{fmt::format("couldn't find '{}'", projectFilePath.string())};

	QFile projectFile(QString::fromStdString(projectFilePath.u8string()));
	projectFile.open(QFile::ReadOnly);

	QXmlStreamReader xml;

	xml.setDevice(&projectFile);

	if(!xml.readNextStartElement())
		throw std::runtime_error{"Invalid project file"};

	if(xml.name() != QLatin1String("bear"))
		throw std::runtime_error{"Expected top-level <bear> element"};

	while(!xml.atEnd() && !xml.hasError()){
		xml.readNext();

		if(xml.isStartElement()){
			if(xml.name() == "module"){
				std::string moduleName, moduleVersion = "dev";
				std::vector<BearProjectSource> moduleSources;

				if(xml.attributes().hasAttribute("purson-version"))
					moduleVersion = xml.attributes().value("purson-version").toString().toStdString();
				else if(xml.attributes().hasAttribute("name"))
					moduleName = xml.attributes().value("name").toString().toStdString();

				while(!xml.atEnd() && !xml.hasError()){
					xml.readNext();

					if(xml.isStartElement()){
						if(xml.name() == "source"){
							xml.readNext();

							std::string name = xml.text().toString().toStdString();
							auto path = m_dir / name;

							xml.readNext();

							if((xml.name() != "source") || xml.isStartElement())
								throw std::runtime_error{fmt::format("Invalid source tag")};

							moduleSources.emplace_back(moduleVersion, path);
						}
					}
					else if(xml.hasError()){

					}
					else if(xml.atEnd()){

					}
					else if(xml.name() == "module"){
						m_modules.emplace_back(m_dir, std::move(moduleVersion), std::move(moduleName), std::move(moduleSources));
						break;
					}
				}
			}
		}
		else if(xml.hasError()){

		}
		else if(xml.atEnd()){

		}
		else{

		}
	}
}

BearProject::~BearProject(){}
