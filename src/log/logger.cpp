#include "logger.h"
#include "span.h"
#include "span_context.h"
#include "timestamp_generator.h"
#include <fmt/format.h>
#include <iostream>
#include <string>

namespace logger {

void Logger::EmitLog(
    std::string trace_id, std::string span_id, std::string content,
    std::string service_name, LogLevel level,
    std::initializer_list<std::pair<std::string, std::string>> tags) {

  LogRecord log = LogRecord(service_name, trace_id, span_id, content,
                            level == LogLevel::ERROR,
                            utils::TimestampGenerator::Now(), level, tags);
  std::string msg =
      fmt::format("[{}] service: {} trace_id: {} span_id: {} content: {}",
                  printLogLevel(log.GetLogLevel()), log.GetServiceName(),
                  log.GetTraceId(), log.GetSpanId(), log.GetContent());
  std::cout << msg << std::endl;
  processor->Process(log);
}

void Logger::EmitLog(std::string content, LogLevel level) {

  trace::SpanContext &current_context = trace::Context::GetCurrentContext();
  std::string trace_id, span_id, service_name = this->service_name;
  if (current_context.IsValid()) {
    trace_id = current_context.GetTraceId();
    span_id = current_context.GetSpanId();
  }
  EmitLog(trace_id, span_id, content, service_name, level, {});
}

void Logger::Debug(std::string content) { EmitLog(content, LogLevel::DEBUG); }

void Logger::Info(std::string content) { EmitLog(content, LogLevel::INFO); }

void Logger::Warn(std::string content) { EmitLog(content, LogLevel::WARN); }

void Logger::Error(std::string content) { EmitLog(content, LogLevel::ERROR); }
} // namespace logger
