#include "../utils/id_generator.h"
#include "span_context.h"
#include "span.h"
#include <memory>
#include <string>
#include <vector>
namespace trace
{

    class Trace
    {
    private:
        // trace的id
        std::string trace_id;
        // trace在当前服务产生的span
        std::vector<Span> spans;

    public:
        Trace()
        {
            trace_id = utils::IdGenerator::GenerateId();
        }
        Trace(std::string trace_id_) : trace_id(trace_id_){};
        ~Trace(){};
        std::shared_ptr<Span> StartSpan(std::string name, std::string service_name, SpanContext& span_context);
    };

} // namespace trace
