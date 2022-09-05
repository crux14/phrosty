#pragma once

#include <QObject>

namespace phrosty {

    class Store final : public QObject {
        Q_OBJECT
      public:
        explicit Store(QObject* parent = 0);
        virtual ~Store() = default;

      Q_SIGNALS:
        void fps_changed(unsigned int new_value);
        void opacity_changed(double new_value);

      public Q_SLOTS:
        void change_fps(unsigned int new_value);
        void change_opacity(double new_value);
    };

}
