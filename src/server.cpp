#include "exporter/ostream_span_exporter.h"
#include "processor/post_sample_processor.h"
#include "protocol/message.h"
#include "sampler/head_variant_sampler.h"
#include "sampler/tail_sampler.h"
#include "span_context.h"
#include "trace_provider.h"
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <ostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>

void initTrace();

void initHeadVariantTrace() {

  auto exporter = std::make_unique<trace::OstreamSpanExporter>();
  auto sampler = std::make_unique<trace::HeadVariantSampler>(1, 3);
  auto processor =
      std::make_unique<trace::PostSampleProcessor>(std::move(exporter));
  trace::TraceProvider::InitProvider(std::move(processor), "client",
                                     std::move(sampler));
}
void initPostTrace() {
  auto exporter = std::make_unique<trace::OstreamSpanExporter>();
  auto sampler = std::make_unique<trace::TailSampler>(3);
  auto processor =
      std::make_unique<trace::PostSampleProcessor>(std::move(exporter));
  trace::TraceProvider::InitProvider(std::move(processor), "server",
                                     std::move(sampler));
}

int main(int argc, char const *argv[]) {
  initHeadVariantTrace();
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8080);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  bind(serverSocket, (sockaddr *)&addr, sizeof(addr));
  listen(serverSocket, 5);
  std::cout << "start" << std::endl;
  while (true) {
    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket =
        accept(serverSocket, (sockaddr *)&clientAddr, &clientAddrLen);
    char buf[1024];

    int len = recv(clientSocket, buf, sizeof(buf), 0);
    std::string str(buf);
    protocol::Message msg = protocol::Message::Deserialize(str);
    trace::Context::Extract(msg);
    auto tracer = trace::TraceProvider::GetTrace();
    auto span = tracer->StartSpan("server");

    try {
      // throw std::runtime_error("error");
    } catch (const std::exception &e) {
      span->SetStatus(trace::StatusCode::kError);
      trace::Context::SetTraceFlag(TraceFlag::kIsSampled);
      span->End();
      trace::RespContext &resp_context = trace::Context::GetReturnContext();
      auto msg = resp_context.ToMessage();
      std::string message = msg.Serialize();
      write(clientSocket, message.c_str(), strlen(message.c_str()));
      return 0;
    }

    span->SetStatus(trace::StatusCode::kOk);
    span->End();
    auto return_context = trace::Context::GetReturnContext();
    auto return_msg = return_context.ToMessage();
    auto return_str = return_msg.Serialize();
    write(clientSocket, return_str.c_str(), len);

    close(clientSocket);
  }

  close(serverSocket);
  return 0;
}

void initTrace() {
  auto exporter = std::make_unique<trace::OstreamSpanExporter>();
  auto processor =
      std::make_unique<trace::SimpleSpanProcessor>(std::move(exporter));
  trace::TraceProvider::InitProvider(std::move(processor), "server");
}