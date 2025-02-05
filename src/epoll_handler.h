//
// Created by binhtt4 on 04/02/25.
//

#ifndef HTTP_SERVER_SRC_EPOLL_HANDLER_H
#define HTTP_SERVER_SRC_EPOLL_HANDLER_H


#include <algorithm>
#include <iostream>
#include <memory>

#include "request_handler.h"


namespace net {


// TODO: template-ize the RequestHandler and then this EpollHandler
struct EpollHandler {
  template <typename THandler>
  explicit EpollHandler(std::shared_ptr<RequestHandler<THandler>> handler)
      : epoll_handler_(std::make_unique<EpollHandlerModel<THandler>>(std::move(handler))) {
  }

  inline void Proceed(uint32_t events) const {
    epoll_handler_->Proceed(events);
  }

  struct EpollHandlerConcept {
    virtual ~EpollHandlerConcept() = default;

    virtual void Proceed(uint32_t events) const = 0;
  };

  template <typename THandler>
  struct EpollHandlerModel : public EpollHandlerConcept {
    explicit EpollHandlerModel(std::shared_ptr<RequestHandler<THandler>> handler)
        : handler_(std::move(handler)) {
    }

    inline void Proceed(uint32_t events) const override {
      handler_->Handle(events);
    }

    std::shared_ptr<RequestHandler<THandler>> handler_;
  };

  std::unique_ptr<EpollHandlerConcept> epoll_handler_;
};


} // namespace net


#endif // HTTP_SERVER_SRC_EPOLL_HANDLER_H
