add_library(log.logger "Logger.hpp" "Logger.cpp")
target_link_libraries(log.logger
	log.loggerbackend
	util.string)

add_library(log.loggerbackend INTERFACE "LoggerBackend.hpp")

add_library(log.stdoutloggerbackend "StdoutLoggerBackend.hpp" "StdoutLoggerBackend.cpp")
target_link_libraries(log.stdoutloggerbackend
	log.loggerbackend)
