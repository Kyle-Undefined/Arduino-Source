/*  Pokemon Sprite Reader
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_PokemonSwSh_PokemonSpriteReader_H
#define PokemonAutomation_PokemonSwSh_PokemonSpriteReader_H

#include <set>
#include "CommonFramework/Logging/LoggerQt.h"
#include "CommonFramework/ImageMatch/ImageMatchResult.h"
#include "CommonFramework/ImageMatch/ExactImageDictionaryMatcher.h"
#include "CommonFramework/ImageMatch/CroppedImageDictionaryMatcher.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonSwSh{


class PokemonSpriteMatcherExact : public ImageMatch::ExactImageDictionaryMatcher{
public:
    PokemonSpriteMatcherExact(const std::set<std::string>* subset);
};

//  Used by Max Lair when there's an item blocking the right side.
class PokemonLeftSpriteMatcherExact : public ImageMatch::ExactImageDictionaryMatcher{
public:
    PokemonLeftSpriteMatcherExact(const std::set<std::string>* subset);
};



class PokemonSpriteMatcherCropped : public ImageMatch::CroppedImageDictionaryMatcher{
public:
    PokemonSpriteMatcherCropped(const std::set<std::string>* subset, double min_euclidean_distance = 100);

private:
    virtual ImageRGB32 process_image(const ImageViewRGB32& image, QRgb& background) const override;

private:
    double m_min_euclidean_distance_squared;
};





}
}
}
#endif
