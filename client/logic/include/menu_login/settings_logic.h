#ifndef SETTINGS_LOGIC_H
#define SETTINGS_LOGIC_H

typedef enum {
    SETTINGS_OK,
    SETTINGS_PSEUDO_VIDE,
    SETTINGS_PSEUDO_TROP_COURT
} SettingsResult;

SettingsResult validate_pseudo(const char *new_pseudo);

#endif