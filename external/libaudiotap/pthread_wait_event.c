#include "wait_event.h"

struct wait_event;

#if __GNUC__ >= 4
 __attribute__ ((visibility ("hidden")))
#endif
int size_of_wait_event(void){
  return 0;
}

#if __GNUC__ >= 4
 __attribute__ ((visibility ("hidden")))
#endif
void create_wait_event(struct wait_event *wait_event) {
}

#if __GNUC__ >= 4
 __attribute__ ((visibility ("hidden")))
#endif
void pause_if_necessary(struct wait_event *wait_event){
}

#if __GNUC__ >= 4
 __attribute__ ((visibility ("hidden")))
#endif
void set_pause(struct wait_event *wait_event) {
}

#if __GNUC__ >= 4
 __attribute__ ((visibility ("hidden")))
#endif
void resume_from_pause(struct wait_event *wait_event) {
}

#if __GNUC__ >= 4
 __attribute__ ((visibility ("hidden")))
#endif
void destroy_wait_event(struct wait_event *wait_event) {
}

