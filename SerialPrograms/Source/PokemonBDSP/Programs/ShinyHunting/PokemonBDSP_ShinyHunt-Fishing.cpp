/*  Shiny Hunt - Fishing
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "CommonFramework/Notifications/ProgramNotifications.h"
#include "CommonFramework/Inference/VisualInferenceRoutines.h"
#include "NintendoSwitch/Commands/NintendoSwitch_PushButtons.h"
#include "PokemonSwSh/ShinyHuntTracker.h"
#include "PokemonBDSP/PokemonBDSP_Settings.h"
#include "PokemonBDSP/Inference/PokemonBDSP_DialogDetector.h"
#include "PokemonBDSP/Inference/PokemonBDSP_MarkFinder.h"
#include "PokemonBDSP/Inference/PokemonBDSP_StartBattleDetector.h"
#include "PokemonBDSP/Inference/PokemonBDSP_BattleMenuDetector.h"
#include "PokemonBDSP/Inference/PokemonBDSP_ShinyEncounterDetector.h"
#include "PokemonBDSP/Programs/PokemonBDSP_EncounterHandler.h"
#include "PokemonBDSP_ShinyHunt-Fishing.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonBDSP{


ShinyHuntFishing_Descriptor::ShinyHuntFishing_Descriptor()
    : RunnableSwitchProgramDescriptor(
        "PokemonBDSP:ShinyHuntFishing",
        STRING_POKEMON + " BDSP", "Shiny Hunt - Fishing",
        "ComputerControl/blob/master/Wiki/Programs/PokemonBDSP/ShinyHunt-Fishing.md",
        "Shiny hunt fishing " + STRING_POKEMON + ".",
        FeedbackType::REQUIRED,
        PABotBaseLevel::PABOTBASE_12KB
    )
{}


ShinyHuntFishing::ShinyHuntFishing(const ShinyHuntFishing_Descriptor& descriptor)
    : SingleSwitchProgramInstance(descriptor)
    , GO_HOME_WHEN_DONE(false)
    , SHORTCUT("<b>Fishing Shortcut:</b>")
    , ENCOUNTER_BOT_OPTIONS(true, false)
    , NOTIFICATION_PROGRAM_FINISH("Program Finished", true, true)
    , NOTIFICATIONS({
        &ENCOUNTER_BOT_OPTIONS.NOTIFICATION_NONSHINY,
        &ENCOUNTER_BOT_OPTIONS.NOTIFICATION_SHINY,
        &NOTIFICATION_PROGRAM_FINISH,
        &NOTIFICATION_PROGRAM_ERROR,
    })
    , m_advanced_options(
        "<font size=4><b>Advanced Options:</b> You should not need to touch anything below here.</font>"
    )
    , EXIT_BATTLE_TIMEOUT(
        "<b>Exit Battle Timeout:</b><br>After running, wait this long to return to overworld.",
        "10 * TICKS_PER_SECOND"
    )
{
    PA_ADD_OPTION(START_IN_GRIP_MENU);
    PA_ADD_OPTION(GO_HOME_WHEN_DONE);

    PA_ADD_OPTION(LANGUAGE);

    PA_ADD_OPTION(SHORTCUT);

    PA_ADD_OPTION(ENCOUNTER_BOT_OPTIONS);
    PA_ADD_OPTION(NOTIFICATIONS);

    PA_ADD_STATIC(m_advanced_options);
//    PA_ADD_OPTION(WATCHDOG_TIMER);
    PA_ADD_OPTION(EXIT_BATTLE_TIMEOUT);
}



struct ShinyHuntFishing::Stats : public PokemonSwSh::ShinyHuntTracker{
    Stats()
        : ShinyHuntTracker(false)
        , m_nothing(m_stats["Nothing"])
        , m_misses(m_stats["Misses"])
    {
        m_display_order.insert(m_display_order.begin() + 1, Stat("Nothing"));
        m_display_order.insert(m_display_order.begin() + 2, Stat("Misses"));
    }
    std::atomic<uint64_t>& m_nothing;
    std::atomic<uint64_t>& m_misses;
};
std::unique_ptr<StatsTracker> ShinyHuntFishing::make_stats() const{
    return std::unique_ptr<StatsTracker>(new Stats());
}



void ShinyHuntFishing::program(SingleSwitchProgramEnvironment& env){
    Stats& stats = env.stats<Stats>();

    StandardEncounterHandler handler(
        env, env.console,
        LANGUAGE,
        ENCOUNTER_BOT_OPTIONS,
        stats
    );

    //  Connect the controller.
    pbf_press_button(env.console, BUTTON_B, 5, 5);

    //  Encounter Loop
    while (true){
        env.update_stats();
        pbf_mash_button(env.console, BUTTON_B, TICKS_PER_SECOND);
        env.console.botbase().wait_for_all_requests();

        {
            ShortDialogDetectorCallback dialog_detector(env.console);
            MarkDetector mark_detector(env.console, {0.4, 0.2, 0.2, 0.5});
            StartBattleDetector battle(env.console);
            BattleMenuDetector battle_menu(BattleType::WILD);
            int ret = run_until(
                env, env.console,
                [=](const BotBaseContext& context){
                    SHORTCUT.run(context, 30 * TICKS_PER_SECOND);
                },
                {
                    &dialog_detector,
                    &mark_detector,
                    &battle_menu,
                }
            );
            switch (ret){
            case 0:
                env.log("Nothing found...", "orange");
                stats.m_nothing++;
                continue;
            case 1:
                env.log("Hooked something!", Qt::blue);
                pbf_press_button(env.console, BUTTON_ZL, 10, TICKS_PER_SECOND);
                break;
            case 2:
                env.log("Unexpected battle menu.", Qt::red);
                stats.add_error();
                handler.run_away_due_to_error(EXIT_BATTLE_TIMEOUT);
                continue;
            default:
                env.log("Timed out.", Qt::red);
                stats.add_error();
                continue;
            }

            //  Wait for dialog after hooking to appear.
            ret = wait_until(
                env, env.console,
                std::chrono::milliseconds(5000),
                {
                    &dialog_detector,
                    &battle_menu,
                }
            );
            switch (ret){
            case 0:
                pbf_mash_button(env.console, BUTTON_B, TICKS_PER_SECOND);
                break;
            case 1:
                env.log("Unexpected battle menu.", Qt::red);
                stats.add_error();
                handler.run_away_due_to_error(EXIT_BATTLE_TIMEOUT);
                continue;
            default:
                env.log("Timed out.", Qt::red);
                stats.add_error();
                continue;
            }

            //  Wait for battle to start.
            ret = wait_until(
                env, env.console,
                std::chrono::milliseconds(10000),
                {
                    &battle,
                    &battle_menu,
                }
            );
            switch (ret){
            case 0:
                env.console.log("Battle started!");
                break;
            case 1:
                env.log("Unexpected battle menu.", Qt::red);
                stats.add_error();
                handler.run_away_due_to_error(EXIT_BATTLE_TIMEOUT);
                continue;
            default:
                env.log("Missed the hook.", "orange");
                stats.m_misses++;
                continue;
            }
        }

        //  Detect shiny.
        DoublesShinyDetection result = detect_shiny_battle(
            env.console,
            env, env.console, env.console,
            WILD_POKEMON,
            std::chrono::seconds(30)
        );

        bool stop = handler.handle_standard_encounter_end_battle(result, EXIT_BATTLE_TIMEOUT);
        if (stop){
            break;
        }
    }

    send_program_finished_notification(
        env.logger(), NOTIFICATION_PROGRAM_FINISH,
        env.program_info(),
        "",
        stats.to_str()
    );
    GO_HOME_WHEN_DONE.run_end_of_program(env.console);
}





}
}
}