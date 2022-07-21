/*  Generate Pokemon Name OCR Data (Pokedex)
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_PokemonSwSh_GenerateNameOCRData_H
#define PokemonAutomation_PokemonSwSh_GenerateNameOCRData_H

#include "CommonFramework/Options/EnumDropdownOption.h"
#include "CommonFramework/OCR/OCR_LanguageOptionOCR.h"
#include "NintendoSwitch/Framework/NintendoSwitch_SingleSwitchProgram.h"


namespace PokemonAutomation{
    class JsonArray;
    class ImageViewRGB32;
namespace NintendoSwitch{
namespace PokemonSwSh{


class GenerateNameOCRDataPokedex_Descriptor : public RunnableSwitchProgramDescriptor{
public:
    GenerateNameOCRDataPokedex_Descriptor();
};



class GenerateNameOCRDataPokedex : public SingleSwitchProgramInstance{
public:
    enum Mode{
        READ_AND_SAVE,
        GENERATE_TRAINING_DATA,
    };

public:
    GenerateNameOCRDataPokedex(const GenerateNameOCRDataPokedex_Descriptor& descriptor);

    virtual void program(SingleSwitchProgramEnvironment& env, BotBaseContext& context) override;

private:
    void read(
        JsonArray& output,
        LoggerQt& logger,
        const ImageViewRGB32& image
    ) const;
    void dump_images(
        const std::vector<std::string>& expected,
        size_t index,
        const ImageViewRGB32& image
    ) const;

private:
    OCR::LanguageOCR LANGUAGE;
    EnumDropdownOption POKEDEX;
    EnumDropdownOption MODE;
};



}
}
}
#endif
