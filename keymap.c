#include QMK_KEYBOARD_H
//#include "tap_dance.h"
#include "features/select_word.h"

enum layers {
    _QUERTY,
    _SYMBOL,
    _NAV_RGB_MEDIA,
    _MOUSE_ADJUST
};

typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_TAP,
    TD_HOLD,
} td_state_t;

typedef struct {
    bool is_press_action;
    td_state_t state;
} td_tap_t;

// My custom keys
enum {
    TD_OMNILAYR,
};
enum fekeys {
  MK_SELWD = SAFE_RANGE,
  MK_SELLN,
  MK_JIGGR,
  MK_LSHFT,
  MK_RSHFT
};

// TAPPING_TERM is too slow for left shift
#define LEFT_SHIFT_TOLERANCE 100

// Jiggler control variables
bool is_jiggler_active = false;
bool jiggler_direction = false; // used to alternate direction
uint16_t jiggler_frequency = 15000; // in miliseconds
uint16_t jiggler_timer = 0;

// Shift curly brackets timer
static uint16_t brace_timer;

// Macro processor
bool process_record_user(uint16_t keycode, keyrecord_t* record) {
  // Select wors processor
  if (!process_select_word(keycode, record, MK_SELWD))
  {
    return false;
  }
  
  // Select line processor
  if(keycode == MK_SELLN)
  {
    SEND_STRING(SS_TAP(X_HOME) SS_LSFT(SS_TAP(X_END)));
    return false;
  }

  // Toggle Jiggler
  if(keycode == MK_JIGGR)
  {
    if (record->event.pressed) {
        is_jiggler_active = !is_jiggler_active;
        return false;
    }
  }

  if(keycode == MK_LSHFT)
  {
    if (record->event.pressed) {
        brace_timer = timer_read();
        register_code(KC_LSFT);
    }
    else
    {
      unregister_code(KC_LSFT);
      if (timer_elapsed(brace_timer) < LEFT_SHIFT_TOLERANCE) {
        SEND_STRING(SS_LSFT(SS_TAP(X_LBRC)));
      }
    }
    return false;
  }

  if(keycode == MK_RSHFT)
  {
    if (record->event.pressed) {
        brace_timer = timer_read();
        register_code(KC_RSFT);
    }
    else
    {
      unregister_code(KC_RSFT);
      if (timer_elapsed(brace_timer) < TAPPING_TERM) {
        SEND_STRING(SS_LSFT(SS_TAP(X_RBRC)));
      }
    }
    return false;
  }

  

  return true;
}

void matrix_scan_user(void) {
    if (is_keyboard_master()) {
        // initialize timer on master half only, remove if statement above for non-split
        if (jiggler_timer == 0) 
            jiggler_timer = timer_read();
    }

    if (is_jiggler_active) {
        if (timer_elapsed(jiggler_timer) > jiggler_frequency) {
            jiggler_timer = timer_read();
            if (jiggler_direction) {
                tap_code(KC_MS_LEFT);
            } else {
                tap_code(KC_MS_RIGHT);
            }
            jiggler_direction = !jiggler_direction;
        }
    }
}

// Function associated with all tap dances
td_state_t cur_dance(tap_dance_state_t *state);

// Omni layer finished/reset methods
void omni_finished(tap_dance_state_t *state, void *user_data);
void omni_reset(tap_dance_state_t *state, void *user_data);


// Shortcut to make keymap more readable
#define FK_RALT RALT_T(KC_ENT)
#define FK_OMNILAYR TD(TD_OMNILAYR)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	[_QUERTY] = LAYOUT(
    //┌────────┬────────┬────────┬────────┬────────┬────────┐                                           ┌────────┬────────┬────────┬────────┬────────┬────────┐
       KC_ESC  , KC_1   , KC_2   , KC_3   , KC_4   , KC_5   ,                                             KC_6   , KC_7   , KC_8   , KC_9   , KC_0   , KC_MINS,
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐                         ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
       KC_TAB  , KC_Q   , KC_W   , KC_E   , KC_R   , KC_T   , TO(1)  ,                           KC_TRNS, KC_Y   , KC_U   , KC_I   , KC_O   , KC_P   , KC_EQL , 
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┤                         ├────────┼────────┼────────┼────────┼────────┼────────┼────────┤   
       KC_ENT  , KC_A   , KC_S   , KC_D   , KC_F   , KC_G   , KC_PSCR,                          MK_SELLN, KC_H   , KC_J   , KC_K   , KC_L   , KC_SCLN, KC_QUOT, 
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┼────────┐       ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┼────────┤      
       MK_LSHFT, KC_Z   , KC_X   , KC_C   , KC_V   , KC_B   ,MK_SELWD, KC_PSCR,         KC_HOME, KC_END , KC_N   , KC_M   , KC_COMM, KC_DOT , KC_SLSH,MK_RSHFT, 
    //├────────┼────────┼────────┼────────┼────┬───┴────┬───┼────────┼────────┤       ├────────┼────────┼───┬────┴───┬────┼────────┼────────┼────────┼────────┤
       KC_LCTL , KC_NUBS, KC_LGUI, KC_LALT,   FK_OMNILAYR   , KC_SPC , KC_DEL ,         FK_RALT, KC_BSPC,   FK_OMNILAYR   , KC_LEFT, KC_DOWN, KC_UP  , KC_RGHT
    //└────────┴────────┴────────┴────────┘    └────────┘   └────────┴────────┘       └────────┴────────┘   └────────┘    └────────┴────────┴────────┴────────┘   
    ),


	[_SYMBOL] = LAYOUT(
    //┌────────┬────────┬────────┬────────┬────────┬────────┐                                           ┌────────┬────────┬────────┬────────┬────────┬────────┐
       KC_GRV  , KC_F1  , KC_F2  , KC_F3  , KC_F4  , KC_F5  ,                                             KC_F6  , KC_F7  , KC_F8  , KC_F9  , KC_F10 , KC_F11 , 
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐                         ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤   
       KC_TILD , KC_EXLM, KC_P7  , KC_P8  , KC_P9  , KC_PPLS, TO(2)  ,                           TO(0)  , KC_PGUP, KC_LCBR, KC_RCBR, KC_DLR , KC_AMPR, KC_F12 , 
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┤                         ├────────┼────────┼────────┼────────┼────────┼────────┼────────┤   
       KC_PCMM , KC_AT  , KC_P4  , KC_P5  , KC_P6  , KC_ASTR, KC_CAPS,                           KC_APP , KC_HOME, KC_LBRC, KC_RBRC, KC_PIPE, KC_END , KC_BSLS, 
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┼────────┐       ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┼────────┤   
       KC_TRNS , KC_NUBS, KC_P1  , KC_P2  , KC_P3  , KC_PEQL, KC_TRNS, KC_TRNS,         KC_TRNS, KC_TRNS, KC_PGDN, KC_LPRN, KC_RPRN, KC_HASH, KC_PERC, KC_TRNS, 
    //├────────┼────────┼────────┼────────┼────┬───┴────┬───┼────────┼────────┤       ├────────┼────────┼───┬────┴───┬────┼────────┼────────┼────────┼────────┤   
       KC_TRNS , KC_P0  , KC_PDOT, KC_P0  ,      KC_NO  ,     KC_TRNS, KC_TRNS,         KC_TRNS, KC_TRNS,     KC_NO  ,      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    //└────────┴────────┴────────┴────────┘    └────────┘   └────────┴────────┘       └────────┴────────┘   └────────┘    └────────┴────────┴────────┴────────┘   
    ),


	[_NAV_RGB_MEDIA] = LAYOUT(
    //┌────────┬────────┬────────┬────────┬────────┬────────┐                                           ┌────────┬────────┬────────┬────────┬────────┬────────┐    
        KC_VOLU, RGB_MOD, RGB_SPI, RGB_VAI, RGB_SAI, RGB_HUI,                                             KC_F13 , KC_F14 , KC_F15 , KC_F16 , KC_F17 , KC_MAIL, 
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐                         ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤       
        KC_VOLD,RGB_RMOD, RGB_SPD, RGB_VAD, RGB_SAD, RGB_HUD, TO(3)  ,                           TO(1)  , KC_WBAK, KC_PGUP, KC_UP  , KC_PGDN, KC_WFWD, KC_EXEC, 
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┤                         ├────────┼────────┼────────┼────────┼────────┼────────┼────────┤     
        KC_MNXT, RGB_M_P, RGB_M_B, RGB_M_R,RGB_M_SW,RGB_M_SN, RGB_TOG,                          KC_WREF , KC_HOME, KC_LEFT, KC_DOWN, KC_RGHT, KC_END , KC_MYCM, 
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┼────────┐       ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┼────────┤     
        KC_MPRV, RGB_M_K, RGB_M_X, RGB_M_G, RGB_M_T, KC_NO  , KC_BRIU, KC_BRID,         KC_TRNS, KC_TRNS, KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , 
    //├────────┼────────┼────────┼────────┼────┬───┴────┬───┼────────┼────────┤       ├────────┼────────┼───┬────┴───┬────┼────────┼────────┼────────┼────────┤    
        KC_MUTE, KC_NO  , KC_NO  , KC_NO  ,      KC_NO  ,     KC_MPLY, KC_MPLY,         KC_TRNS, KC_TRNS,     KC_NO  ,      KC_NO  , KC_NO  , KC_NO  , KC_NO
    //└────────┴────────┴────────┴────────┘    └────────┘   └────────┴────────┘       └────────┴────────┘   └────────┘    └────────┴────────┴────────┴────────┘     
    ),


	[_MOUSE_ADJUST] = LAYOUT(
    //┌────────┬────────┬────────┬────────┬────────┬────────┐                                           ┌────────┬────────┬────────┬────────┬────────┬────────┐     
        KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  ,                                             KC_F18 , KC_F19 , KC_F19 , KC_F20 , KC_F21 , KC_PWR ,
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐                         ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤     
        QK_BOOT, KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  ,                           TO(2)  , KC_WH_L, KC_WH_U, KC_MS_U, KC_WH_D, KC_WH_R, KC_SLEP, 
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┤                         ├────────┼────────┼────────┼────────┼────────┼────────┼────────┤     
        DB_TOGG, KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  ,                           KC_NO  , KC_NO  , KC_MS_L, KC_MS_D, KC_MS_R, KC_NO  , KC_WAKE,
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┼────────┐       ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┼────────┤     
        EE_CLR,  KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  ,         KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  ,MK_JIGGR,
    //├────────┼────────┼────────┼────────┼────┬───┴────┬───┼────────┼────────┤       ├────────┼────────┼───┬────┴───┬────┼────────┼────────┼────────┼────────┤     
        KC_NO  , KC_NO  , KC_NO  , KC_NO  ,      KC_NO  ,     KC_NO  , KC_NO  ,         KC_BTN2, KC_BTN1,     KC_NO  ,      KC_ACL0, KC_ACL1, KC_ACL2, KC_NO
    //└────────┴────────┴────────┴────────┘    └────────┘   └────────┴────────┘       └────────┴────────┘   └────────┘    └────────┴────────┴────────┴────────┘ 
    )
};

#if defined(ENCODER_ENABLE) && defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {

};
#endif // defined(ENCODER_ENABLE) && defined(ENCODER_MAP_ENABLE)


td_state_t cur_dance(tap_dance_state_t *state) {
    if (state->count >= 1) {
        if (!state->pressed) 
            return TD_TAP;
        else 
            return TD_HOLD;
    } 
    else 
        return TD_UNKNOWN;
}

// Static field to store the state
static td_tap_t omni_tap_state = {
    .is_press_action = true,
    .state = TD_NONE
};

// Called when a dance finishes
void omni_finished(tap_dance_state_t *state, void *user_data) {
    omni_tap_state.state = cur_dance(state);
    switch (omni_tap_state.state) {
        case TD_HOLD:
            layer_on(state->count);
            break;
        default:
            break;
    }
}

// CAlled when a dance is reset
void omni_reset(tap_dance_state_t *state, void *user_data) {
    // If the key was held down and now is released then switch off the layer
    if (omni_tap_state.state == TD_HOLD) {
        layer_off(state->count);
    }
    omni_tap_state.state = TD_NONE;
}

// Binds key with callback
tap_dance_action_t tap_dance_actions[] = {
    [TD_OMNILAYR] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, omni_finished, omni_reset)
};

// Set a longer time window
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case QK_TAP_DANCE ... QK_TAP_DANCE_MAX:
            return 275;
        default:
            return TAPPING_TERM;
    }
}
