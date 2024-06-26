// C++ program to illustrate the client application in the
// socket programming
#include "exporter/ostream_span_exporter.h"
#include "processor/post_sample_processor.h"
#include "protocol/message.h"
#include "sampler/head_variant_sampler.h"
#include "sampler/tail_sampler.h"
#include "span_context.h"
#include "span_metadata.h"
#include "trace_provider.h"
#include <cstring>
#include <memory>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>

void initTrace();

void initPostTrace() {
  auto exporter = std::make_unique<trace::OstreamSpanExporter>();
  auto sampler = std::make_unique<trace::TailSampler>(3);
  auto processor =
      std::make_unique<trace::PostSampleProcessor>(std::move(exporter));
  trace::TraceProvider::InitProvider(std::move(processor), "client",
                                     std::move(sampler));
}

void initHeadVariantTrace() {

  auto exporter = std::make_unique<trace::OstreamSpanExporter>();
  auto sampler = std::make_unique<trace::HeadVariantSampler>(1, 3);
  auto processor =
      std::make_unique<trace::PostSampleProcessor>(std::move(exporter));
  trace::TraceProvider::InitProvider(std::move(processor), "client",
                                     std::move(sampler));
}

int main() {
  initHeadVariantTrace();
  // creating socket
  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

  // specifying address
  sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(8080);
  serverAddress.sin_addr.s_addr = INADDR_ANY;

  // sending connection request
  connect(clientSocket, (struct sockaddr *)&serverAddress,
          sizeof(serverAddress));

  // sending data
  protocol::Message msg;
  auto trace = trace::TraceProvider::GetTrace();
  trace::SpanContext context;
  auto span = trace->StartSpan("client");
  trace::Context::WriteToMessage(msg);
  std::string message = msg.Serialize().c_str();

  send(clientSocket, message.c_str(), strlen(message.c_str()), 0);
  char buffer[1024];
  recv(clientSocket, buffer, sizeof(buffer), 0);

  std::string recv_msg(buffer);
  protocol::Message msg_recv = protocol::Message::Deserialize(recv_msg);
  auto resp_context = trace::RespContext::FromMessage(msg_recv);
  trace::Context::AddRespContext(resp_context);
  span->SetStatus(trace::StatusCode::kOk);
  span->End();
  // closing socket
  close(clientSocket);

  return 0;
}

void initTrace() {
  auto exporter = std::make_unique<trace::OstreamSpanExporter>();
  auto processor =
      std::make_unique<trace::SimpleSpanProcessor>(std::move(exporter));
  trace::TraceProvider::InitProvider(std::move(processor), "client");
}