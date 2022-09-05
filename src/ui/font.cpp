#include "./font.h"

#include <QFontDatabase>
#include <core/logger.h>

namespace phrosty {

    std::string FontAwesomeFamily;

    bool load_font_FontAwesome() {
        int font_id = -1;
        if ((font_id = QFontDatabase::addApplicationFont(":/FontAwesome-solid.otf")) < 0) {
            PHLOG_ERRORN("Failed to load font: FontAwesome-solid.otf\n");
            return false;
        }
        FontAwesomeFamily = QFontDatabase::applicationFontFamilies(font_id).at(0).toStdString();
        return true;
    }

}
