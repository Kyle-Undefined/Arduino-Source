/*  Switch System
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_NintendoSwitch_SwitchSystemWidget_H
#define PokemonAutomation_NintendoSwitch_SwitchSystemWidget_H

#include <QWidget>
#include "NintendoSwitch/Framework/NintendoSwitch_SwitchSystemSession.h"
#include "NintendoSwitch/Framework/NintendoSwitch_SwitchSystemOption.h"

namespace PokemonAutomation{
    class CollapsibleGroupBox;
    class AudioFeed;
    class CameraSelectorWidget;
    class VideoDisplayWidget;
    class AudioDisplayWidget;
    class VideoOverlay;

namespace NintendoSwitch{

class CommandRow;

class SwitchSystemWidget final : public QWidget{
public:
    virtual ~SwitchSystemWidget();
    SwitchSystemWidget(
        QWidget& parent,
        SwitchSystemSession& session,
        uint64_t program_id
    );

public:
    Logger& logger();
    BotBase* botbase();
    VideoFeed& camera();
    VideoOverlay& overlay();
    AudioFeed& audio();
    void update_ui(ProgramState state);

private:
    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void keyReleaseEvent(QKeyEvent* event) override;
    virtual void focusInEvent(QFocusEvent* event) override;
    virtual void focusOutEvent(QFocusEvent* event) override;

private:
    SwitchSystemSession& m_session;

    CollapsibleGroupBox* m_group_box;

    SerialPortWidget* m_serial_widget = nullptr;

    VideoDisplayWidget* m_video_display;
    AudioDisplayWidget* m_audio_display;

    CommandRow* m_command;

    CameraSelectorWidget* m_camera_widget;
    AudioSelectorWidget* m_audio_widget;
};




}
}
#endif