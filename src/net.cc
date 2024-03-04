module;

#include "asio.hpp"
#include <memory>
#include <thread>

import ring_buffer;
import event;

export module net;

using asio::ip::udp;

export class NetClient {
  public:
  NetClient(std::shared_ptr<RingBuffer<Event, 128>> eq) : socket(udp::socket(ctx)), resolver(ctx) {
    events = eq;
  };
  ~NetClient() {
    finish = true;
  }

  void start() {
    endpoint = *resolver.resolve(udp::v4(), "0.0.0.0", "p2");
    receiver = thread_recv();
  }

  Event decode(std::string) {
    return {};
  }

  std::thread thread_recv() {
    return std::thread([this] {
      while(finish == false) {
        std::string recv_buf;
        size_t len = socket.receive_from(
            asio::buffer(recv_buf), 
            endpoint);
        if(len) {
          auto e = decode(recv_buf);
          events->push(e);
        }
      }
    });
  }

  asio::io_context ctx{};
  udp::socket socket;
  udp::resolver resolver;
  udp::endpoint endpoint;
  std::thread receiver;
  std::shared_ptr<RingBuffer<Event, 128>> events;
  bool finish = false;
};


