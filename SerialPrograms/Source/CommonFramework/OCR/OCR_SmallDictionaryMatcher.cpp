/*  Small Database Matcher
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include <QtGlobal>
#include "Common/Cpp/Json/JsonValue.h"
#include "Common/Cpp/Json/JsonObject.h"
#include "CommonFramework/Globals.h"
#include "OCR_StringNormalization.h"
#include "OCR_TextMatcher.h"
#include "OCR_SmallDictionaryMatcher.h"

namespace PokemonAutomation{
namespace OCR{



SmallDictionaryMatcher::SmallDictionaryMatcher(const QString& json_offset, bool first_only)
    : SmallDictionaryMatcher(
        load_json_file(RESOURCE_PATH().toStdString() + json_offset.toStdString()).get_object_throw(),
        first_only
    )
{}
SmallDictionaryMatcher::SmallDictionaryMatcher(const JsonObject& json, bool first_only){
    for (const auto& item : json){
        Language language = language_code_to_enum(item.first);
        const LanguageData& data = language_data(language);
        m_database.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(language),
            std::forward_as_tuple(item.second.get_object_throw(), nullptr, data.random_match_chance, first_only)
        );
        m_languages += language;
    }
}


void SmallDictionaryMatcher::save(const QString& json_path) const{
    JsonObject root;
    for (const auto& item : m_database){
        root[language_data(item.first).code] = item.second.to_json();
    }
    root.dump(json_path.toStdString());
}



}
}
