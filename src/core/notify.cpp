#include "./notify.h"
#include "./logger.h"

#include <libnotify/notify.h>

namespace phrosty {

    struct Notifier::Context {
        NotifyNotification* handle = nullptr;
        bool initialized = false;
    };

    Notifier::Notifier(const std::string& title, const std::string& msg) {
        m_ctx = new Notifier::Context;

        if (!notify_init("phrosty")) {
            PHLOG_ERRORN("notify_init() failed");
            return;
        }

        m_ctx->handle = notify_notification_new(title.c_str(), msg.c_str(), "dialog-information");
        m_ctx->initialized = true;

        // [XXX] When called in fullscreen, this notification is not shown by default.
        // This is a workaround for it.
        notify_notification_set_urgency(m_ctx->handle, NOTIFY_URGENCY_CRITICAL);
    }

    Notifier::~Notifier() {
        if (m_ctx && m_ctx->initialized) {
            g_object_unref(G_OBJECT(m_ctx->handle));
            notify_uninit();
            m_ctx->initialized = false;
            delete m_ctx;
        }
    }

    void Notifier::show() {
        if (m_ctx->initialized) {
            GError* err = nullptr;
            if (!notify_notification_show(m_ctx->handle, &err)) {
                PHLOG_ERROR("Failed to show notification: {}", err->message);
            }
        }
    }

    void Notifier::close() {
        if (m_ctx->initialized) {
            notify_notification_close(m_ctx->handle, nullptr);
        }
    }

}
