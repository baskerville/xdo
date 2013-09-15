#ifndef _XDO_H
#define _XDO_H

#define ALL_DESKS      0xFFFFFFFF
#define DESKEQ(d1,d2)  (d1 == d2 || (cfg.symb_desks && (d1 == ALL_DESKS || d2 == ALL_DESKS)))

typedef enum {
    VALUE_IGNORE,
    VALUE_SAME,
    VALUE_DIFFERENT
} value_cmp_t;

typedef struct {
    value_cmp_t wid;
    value_cmp_t class;
    value_cmp_t desktop;
    char *class_name;
    char *instance_name;
    uint32_t pid;
    uint8_t evt_code;
    bool symb_desks;
} config_t;

xcb_connection_t *dpy;
int default_screen;
xcb_window_t root;
xcb_ewmh_connection_t *ewmh;
config_t cfg;

void init(void);
int usage(void);
int version(void);
void setup(void);
void finish(void);
bool match(xcb_window_t, xcb_window_t, uint32_t, char*);
void get_atom(char *, xcb_atom_t *);
void get_active_window(xcb_window_t *);
bool get_class(xcb_window_t, char *, size_t);
bool get_instance(xcb_window_t, char *, size_t);
bool get_pid(xcb_window_t win, uint32_t *);
bool get_desktop(xcb_window_t, uint32_t *);
bool get_current_desktop(uint32_t *);
void window_close(xcb_window_t);
void window_kill(xcb_window_t);
void window_hide(xcb_window_t);
void window_show(xcb_window_t);
void window_raise(xcb_window_t);
void window_lower(xcb_window_t);
void window_activate(xcb_window_t);
void window_id(xcb_window_t);
void window_pid(xcb_window_t);
void fake_input(xcb_window_t, uint8_t, uint8_t);
void key_press_release(xcb_window_t);
void button_press_release(xcb_window_t);

#endif
