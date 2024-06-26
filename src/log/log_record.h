
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

#ifndef LOG_RECORD_H
#define LOG_RECORD_H
namespace logger {
enum LogLevel { DEBUG, INFO, WARN, ERROR };

class LogRecord {
private:
  std::string service_name;
  std::string trace_id;
  std::string span_id;
  std::string content;
  std::unordered_map<std::string, std::string> tags;
  bool error;
  long timestamp;
  LogLevel level;

public:
  LogRecord(
      std::string service_name_, std::string trace_id_, std::string span_id_,
      std::string content_, bool error_, long timestamp_, LogLevel level_,
      std::initializer_list<std::pair<std::string, std::string>> tags_ = {})
      : service_name(service_name_), trace_id(trace_id_), span_id(span_id_),
        content(content_), error(error_), timestamp(timestamp_), level(level_) {

    for (auto &tag : tags_) {
      tags.insert(std::pair<std::string, std::string>(tag.first, tag.second));
    }
  }
  std::string GetTraceId() { return this->trace_id; }
  std::string GetSpanId() { return this->span_id; }
  std::string GetServiceName() { return this->service_name; }
  std::string GetContent() { return this->content; }
  long GetTimestamp() { return this->timestamp; }
  std::unordered_map<std::string, std::string> GetTags() { return this->tags; }
  LogLevel GetLogLevel() { return this->level; }
  bool GetError() { return this->error; }

  ~LogRecord() = default;
};

inline std::string printLogLevel(LogLevel level) {
  switch (level) {
  case LogLevel::DEBUG:
    return "DEBUG";
  case LogLevel::INFO:
    return "INFO";
  case LogLevel::WARN:
    return "WARN";
  case LogLevel::ERROR:
    return "ERROR";
  };
  throw std::runtime_error("invalid log level");
}

inline std::string printTags(std::unordered_map<std::string, std::string> &tags) {
  std::string tags_str = "{";
  for (auto &tag : tags) {
    tags_str += "\"" + tag.first + "\": \"" + tag.second + "\",";
  }
  tags_str += "}";
  return tags_str;
}

inline std::string printError(bool error) {
  return error ? "true" : "false";
}

} // namespace logger
#endif // !LOG_H
