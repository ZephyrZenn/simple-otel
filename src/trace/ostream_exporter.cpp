
#include "ostream_exporter.h"
#include <string>
#include "span_metadata.h"
#include "span.h"
namespace trace
{
    std::string printTags(Span &span);
    std::string printStatus(StatusCode status);

    void OstreamSpanExporter::Export_(Span &span)
    {
        sout << "{"
             << "\n  \"trace_id\": \"" << span.TraceId() << "\","
             << "\n  \"span_id\": \"" << span.Id() << "\","
             << "\n  \"parent_span_id\": \"" << span.ParentId() << "\","
             << "\n  \"name\": \"" << span.Name() << "\","
             << "\n  \"service_name\": \"" << span.ServiceName() << "\","
             << "\n  \"start_time\": " << span.StartTime() << ","
             << "\n  \"end_time\": " << span.EndTime() << ","
             << "\n  \"tags\": " << printTags(span) << ","
             << "\n  \"status\": \"" << printStatus(span.Status()) << "\""
             << "\n }" << std::endl;
    }

    std::string printTags(Span &span)
    {
        std::string tags = "{";
        for (auto &tag : span.Tags())
        {
            tags += "\"" + tag.first + "\": \"" + tag.second + "\",";
        }
        tags += "}";
        return tags;
    }

    std::string printStatus(StatusCode status)
    {
        switch (status)
        {
        case StatusCode::kUnset:
            return "Unset";
        case StatusCode::kOk:
            return "Ok";
        case StatusCode::kError:
            return "Error";
        default:
            return "Unknown";
        }
    }
} // namespace trace
