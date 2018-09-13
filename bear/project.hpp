#ifndef BEAR_PROJECT_HPP
#define BEAR_PROJECT_HPP 1

#include <algorithm>
#include <functional>
#include <vector>
#include <filesystem>
#include <fstream>

#include <QObject>
#include <QUrl>
#include <QVector>

class BearProjectSource{
	public:
		BearProjectSource(std::string_view pursonVersion_, const std::filesystem::path &path_)
			: m_pursonVersion{pursonVersion_}, m_path(path_){}

		std::string_view pursonVersion() const noexcept{ return m_pursonVersion; }
		const std::filesystem::path &path() const noexcept{ return m_path; }

	private:
		std::string m_pursonVersion;
		std::filesystem::path m_path;
};

class BearProjectModule{
	public:
		BearProjectModule(
			const std::filesystem::path &dir_,
			std::string_view pursonVersion_,
			std::string_view name_,
			std::vector<BearProjectSource> sources_
		)
			: m_dir{dir_}, m_pursonVersion{pursonVersion_}, m_name{name_}, m_sources{std::move(sources_)}{}

		const std::filesystem::path &dir() const noexcept{ return m_dir; }
		std::string_view pursonVersion() const noexcept{ return m_pursonVersion; }
		std::string_view name() const noexcept{ return m_name; }

		const std::vector<BearProjectSource> &sources() const noexcept{ return m_sources; }

		BearProjectSource &createSource(const std::filesystem::path &p){
			auto res = std::find_if(
				begin(m_sources), end(m_sources),
				[&](auto &&src){ return src.path() == (m_dir / p); }
			);

			if(res != end(m_sources))
				return *res;

			std::ofstream file(m_dir / p);
		}

		void deleteSource(const std::filesystem::path &p){
			auto res = std::find_if(
				begin(m_sources), end(m_sources),
				[&](auto &&src){ return src.path() == (m_dir / p); }
			);

			if(res != end(m_sources)){
				std::filesystem::remove(p);
				m_sources.erase(res);
			}
		}

	private:
		std::filesystem::path m_dir;
		std::string m_pursonVersion, m_name;
		std::vector<BearProjectSource> m_sources;
};

class BearProject{
	public:
		BearProject(const std::filesystem::path &dir);
		~BearProject();

		std::string name() const noexcept{ return m_dir.filename(); }

		const std::vector<BearProjectModule> &modules() const noexcept{ return m_modules; }
		std::vector<BearProjectModule> &modules() noexcept{ return m_modules; }

		BearProjectModule &createModule(std::string_view name);
		bool deleteModule(std::string_view name);

	private:
		std::filesystem::path m_dir;
		std::vector<BearProjectModule> m_modules;
};

class BearProjectHandler: public QObject{
		Q_OBJECT

		Q_PROPERTY(QUrl dirUrl READ dirUrl WRITE setDirUrl NOTIFY dirUrlChanged)
		Q_PROPERTY(QVector<QUrl> modulePaths READ modulePaths WRITE setModulePaths NOTIFY modulePathsChanged)

	public:
		QUrl dirUrl() const;
		const QVector<QUrl> &modulePaths() const;
		BearProject *project(){ return m_project.get(); }

	public slots:
		void setDirUrl(const QUrl &arg);
		void setModulePaths(const QVector<QUrl> &arg);

		Q_INVOKABLE void openProject(const QUrl &dir);

	signals:
		void dirUrlChanged();
		void modulePathsChanged();

	private:
		QUrl m_dirUrl;
		QVector<QUrl> m_modulePaths;

		std::unique_ptr<BearProject> m_project;
};

#endif // !BEAR_PROJECT_HPP
