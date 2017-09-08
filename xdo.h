#ifndef _XDO_H
#define _XDO_H

#define ALL_DESKS      0xFFFFFFFF
#define DESKEQ(d1,d2)  (d1 == d2 || (cfg.symb_desks && (d1 == ALL_DESKS || d2 == ALL_DESKS)))
#define ISRELA(s)      (s[0] == '+' || s[0] == '-')

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
	char *wm_name;
	char *x;
	char *y;
	char *width;
	char *height;
	uint32_t pid;
	uint8_t evt_code;
	bool symb_desks;
	bool wait_match;
	long int target;
} config_t;

xcb_connection_t *dpy;
int default_screen;
xcb_window_t root;
xcb_ewmh_connection_t *ewmh;
config_t cfg;

void apply(void (*action)(xcb_window_t), xcb_window_t parent, xcb_window_t win, uint32_t desktop, char* class, int* hits);
bool match(xcb_window_t w, xcb_window_t win, uint32_t desktop, char* class);
void init(void);
int usage(void);
int version(void);
void setup(void);
void finish(void);
void get_active_window(xcb_window_t *win);
bool get_class(xcb_window_t win, char *class, size_t len);
bool get_instance(xcb_window_t win, char *instance, size_t len);
bool get_wm_name(xcb_window_t win, char *wm_name, size_t len);
bool get_pid(xcb_window_t win, uint32_t *pid);
bool get_desktop(xcb_window_t win, uint32_t *desktop);
bool get_current_desktop(uint32_t *desktop);
void window_close(xcb_window_t win);
void window_kill(xcb_window_t win);
void window_hide(xcb_window_t win);
void window_show(xcb_window_t win);
void window_raise(xcb_window_t win);
void window_lower(xcb_window_t win);
void window_stack(xcb_window_t win, uint32_t mode);
void window_above(xcb_window_t win);
void window_below(xcb_window_t win);
void window_move(xcb_window_t win);
void window_resize(xcb_window_t win);
void window_activate(xcb_window_t win);
void window_id(xcb_window_t win);
void window_pid(xcb_window_t win);
void fake_input(xcb_window_t win, uint8_t evt, uint8_t code);
void fake_motion(xcb_window_t win, uint8_t rel, uint16_t x, uint16_t y);
void key_press(xcb_window_t win);
void key_release(xcb_window_t win);
void button_press(xcb_window_t win);
void button_release(xcb_window_t win);
void pointer_motion(xcb_window_t win);

#endif
