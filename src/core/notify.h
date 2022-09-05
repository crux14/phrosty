#pragma once

#include <string>

namespace phrosty {

    class Notifier final {
        struct Context;

      public:
        explicit Notifier(const std::string& title, const std::string& msg);
        virtual ~Notifier();
        void show();
        void close();

      private:
        Notifier::Context* m_ctx;
    };

}
