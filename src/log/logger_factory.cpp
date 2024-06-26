#include "logger_factory.h"
#include <memory>

namespace logger {

LoggerFactory LoggerFactory::factory;
LoggerFactory &LoggerFactory::GetInstance() { return factory; }

void LoggerFactory::InitFactory(std::shared_ptr<LogProcessor> processor_,
                                std::string service_name_) {
  factory = LoggerFactory(service_name_, processor_);
}

std::unique_ptr<Logger> LoggerFactory::Create() {
  auto factory = GetInstance();
  return std::make_unique<Logger>(factory.processor, factory.service_name);
}

} // namespace logger
