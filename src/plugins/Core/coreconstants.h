#pragma once
namespace Core {
    namespace Constants {
        const char MAIN_WINDOW[] = "ROOT";

        const char G_DEFAULT_ONE[] = "Group.Default.One";
        const char G_DEFAULT_TWO[] = "Group.Default.Two";
        const char G_DEFAULT_THREE[] = "Group.Default.Three";
        const char G_DEFAULT_FOUR[] = "Group.Default.Four";
        const char G_DEFAULT_FIVE[] = "Group.Default.Five";
        const char C_GLOBAL[] = "Global Context";

        // Menubar
        const char MENU_BAR[] = "MenuBar";

        // Main menu bar groups
        const char G_FILE[] = "Group.File";
        const char G_EDIT[] = "Group.Edit";
        const char G_VIEW[] = "Group.View";
        const char G_TOOLS[] = "Group.Tools";
        const char G_HELP[] = "Group.Help";

        // Menus
        const char M_FILE[] = "Menu.File";
        const char M_TOOLS[] = "Menu.Tools";

        const char M_TOOL_VIEWS[] = "Menu.Tools.Views";

        const char M_HELP[] = "Menu.Help";

        // Help groups (global)
        const char G_HELP_HELP[] = "Group.Help.Help";
        const char G_HELP_SUPPORT[] = "Group.Help.Supprt";
        const char G_HELP_ABOUT[] = "Group.Help.About";
        const char G_HELP_UPDATES[] = "Group.Help.Updates";

        const char TR_SHOW_RIGHT_SIDEBAR[] = QT_TRANSLATE_NOOP("Core", "Show Right Sidebar");
        const char TOGGLE_RIGHT_SIDEBAR[] = "ToggleRightSidebar";
        const char TOGGLE_DUT_CONFIG[] = "ToggleDutConfig";
        const char TOGGLE_INIT_CALIBRATE_CFG[] = "InitialCalibrateConfig";
        const char TOGGLE_DEVICES_MOVE_INIT_POS_DUT[] = "Load DUT Position";
        const char TOGGLE_DEVICES_MOVE_INIT_POS_SLB[] = "Load and Align SLB Position";
        const char TOGGLE_HISTORY_IMAGE_DATA[] = "History Image Data";
        const char TOGGLE_WEB_CAMERA[] = "ToggleWebCamera";
		const char TOGGLE_MQTT_CONFIG[] = "ToggleMqttConfig";

        const int DEFAULT_MAX_CHAR_COUNT = 1000000;

        const int MODEBAR_ICON_SIZE = 34;
        const int MODEBAR_ICONSONLY_BUTTON_SIZE = MODEBAR_ICON_SIZE + 4;
    }
}