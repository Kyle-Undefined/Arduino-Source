/*  Summary Shiny Symbol
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 *
 *      Determine if a raid is full and ready to start early.
 *
 */

#ifndef PokemonAutomation_PokemonSwSh_SummaryShinySymbolDetector_H
#define PokemonAutomation_PokemonSwSh_SummaryShinySymbolDetector_H

#include <chrono>
#include "CommonFramework/Logging/LoggerQt.h"
#include "CommonFramework/VideoPipeline/VideoFeed.h"

namespace PokemonAutomation{
    class CancellableScope;
namespace NintendoSwitch{
namespace PokemonSwSh{


class SummaryShinySymbolDetector{
public:
    enum Detection{
        NO_DETECTION,
        NOT_SHINY,
        SHINY,
    };

public:
    SummaryShinySymbolDetector(LoggerQt& logger, VideoOverlay& overlay);

    Detection detect(const QImage& screen);
    Detection wait_for_detection(
        CancellableScope& scope, VideoFeed& feed,
        std::chrono::seconds timeout = std::chrono::seconds(10)
    );

private:
    LoggerQt& m_logger;
    InferenceBoxScope m_state0_box;
    InferenceBoxScope m_state1_box;
    InferenceBoxScope m_symbol_box;
};



}
}
}

#endif

