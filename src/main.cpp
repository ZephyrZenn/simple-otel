#include "ostream_span_exporter.h"
#include "post_sample_processor.h"
#include "sampler/tail_sampler.h"
#include "span_context.h"
#include "trace/trace.h"
#include "trace_provider.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <ostream>
void initTrace() {
  auto exporter = std::make_unique<trace::OstreamSpanExporter>();
  auto processor =
      std::make_unique<trace::SimpleSpanProcessor>(std::move(exporter));
  trace::TraceProvider::InitProvider(std::move(processor));
}

void initPostTrace() {
  auto exporter = std::make_unique<trace::OstreamSpanExporter>();
  auto sampler = std::make_unique<trace::TailSampler>(3);
  auto processor = std::make_unique<trace::PostSampleProcessor>(
      std::move(exporter), std::move(sampler));
  trace::TraceProvider::InitProvider(std::move(processor));
}

int main(int argc, char const *argv[]) {
  initPostTrace();
  auto trace = trace::TraceProvider::GetTrace();
  auto span1 = trace->StartSpan("outer", __func__);
  std::cout << span1->GetId() << std::endl;
  auto span2 = trace->StartSpan("inner", __func__);
  std::cout << span2->GetId() << std::endl;
  std::cout << trace::Context::GetCurrentContext().GetSpanId() << std::endl;
  std::cout << trace::Context::GetParentContext().GetSpanId() << std::endl;
  span2->End();
  std::cout << trace::Context::GetCurrentContext().GetSpanId() << std::endl;
  span1->End();

  return 0;
}