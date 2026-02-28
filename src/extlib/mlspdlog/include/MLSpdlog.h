// Copyright 2025 MLOptic

#ifndef MLSPDLOG_MLSPDLOG_H_
#define MLSPDLOG_MLSPDLOG_H_
#include <string>


#ifdef MLSPDLOG_EXPORTS
#define MLSPDLOG_API __declspec(dllexport)
#else
#define MLSPDLOG_API __declspec(dllimport)
#endif


namespace ML {
	class MLSPDLOG_API MLSpdlog {
	public:
		static MLSpdlog* instance();
		~MLSpdlog();

		void set_log_path(std::string path = "mlhardware_logs");

		void info(std::string msg);

		void debug(std::string msg);

		void warn(std::string msg);

		void error(std::string msg);

	private:
		MLSpdlog();

	private:
		static MLSpdlog* m_instance;
	};
}  // namespace ML

#endif  // MLSPDLOG_MLSPDLOG_H_
