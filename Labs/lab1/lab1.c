#include "pico/stdlib.h"
#include "pico/util/queue.h"
#include "hardware/pwm.h" 

/* !!! PART 2 & 3 !!! */

/* !!! MIGHT WANT TO CHANGE THIS !!! */
#define BUTTON_DEBOUNCE_DELAY   200

/* Function pointer primitive */ 
typedef void (*state_func_t)( void );

typedef struct _state_t
{
    uint8_t id;
    state_func_t Enter;
    state_func_t Do;
    state_func_t Exit;
    uint32_t delay_ms;
} state_t;

typedef enum _event_t 
{
    b1_evt = 0,
    b2_evt = 1,
    b3_evt = 2,
    no_evt = 3
} event_t;

/* !!! PART 2 & 3 !!! */
/* Define event queue */
static queue_t evt_queue;

/* PWM slice number */
static uint pwm_slice; 

void button_isr(uint gpio, uint32_t events) 
{
    /* !!! PART 2 !!! */
    static uint32_t last_time = 0;
    uint32_t now = to_ms_since_boot(get_absolute_time());

    if (now - last_time < BUTTON_DEBOUNCE_DELAY) 
        return;

    last_time = now;

    event_t evt = no_evt;
    if (gpio == 20) 
        evt = b1_evt;
    else if (gpio == 21) 
        evt = b2_evt;
    else if (gpio == 22)
        evt = b3_evt;

    if (evt != no_evt) 
        queue_try_add(&evt_queue, &evt);
    
    return; 
}

void private_init() 
{
    /* !!! PART 2 !!! */
    /* Event queue setup */ 
    queue_init(&evt_queue, sizeof(event_t), 10);

    /* !!! PART 2 !!! */
    /* Button setup */
    uint buttons[] = {20, 21, 22};
    for (int i = 0; i < 3; i++) {
        gpio_init(buttons[i]);
        gpio_set_dir(buttons[i], GPIO_IN);
        gpio_pull_up(buttons[i]);
        gpio_set_irq_enabled_with_callback(buttons[i], GPIO_IRQ_EDGE_FALL, true, &button_isr);
    }

    /* !!! PART 1 !!! */
    /* LED setup */
    for (int i = 0; i <= 3; i++) {
        gpio_init(i);              // Initialization
        gpio_set_dir(i, GPIO_OUT); // Set as output
    }
}

/* The next three methods are for convenience, you might want to use them. */
event_t get_event(void)
{
    /* !!!! PART 2 !!!! */
    event_t evt = no_evt;
    queue_try_remove(&evt_queue, &evt);
    return evt;
}

void leds_off () 
{
    /* !!! PART 1 !!! */
    for (int i = 0; i <= 3; i++) {
        gpio_set_function(i, GPIO_FUNC_SIO);
        gpio_put(i, 0); // Turn off LED
    }
}

void leds_on () 
{
    /* !!! PART 2 !!! */
    for (int i = 0; i <= 3; i++) {
        gpio_set_function(i, GPIO_FUNC_SIO);
        gpio_put(i, 1); // Turn on LED
    }
}

// State 0 logic
void do_state_0(void)
{
    /* !!! PART 1 !!! */
    static int current_led = 0; // Keep value between calls

    leds_off();                 // Reset LEDs
    gpio_put(current_led, 1);   // Light up current LED

    current_led = (current_led + 1) % 4; // Shift to next (0->1->2->3->0)
}

// S1: Blinking (500ms)
void do_state_1() {
    static bool toggle = false;
    if (toggle) 
        leds_on(); 
    else 
        leds_off();
    toggle = !toggle;
}

// S2: Reverse fast run (200ms)
void do_state_2() {
    static int led = 3;
    leds_off();
    gpio_put(led, 1);
    led = (led - 1 + 4) % 4;
}

// S3: PWM breathing effect
void do_state_3() {
    static int brightness = 0;
    static int direction = 5;

    brightness += direction;
    // Implement breathing instead of simple modulo jump
    if (brightness >= 255) {
        direction = -direction;
        brightness = 255;
    }
    else if (brightness <= 0) {
        direction = -direction;
        brightness = 0;
    }
    
    pwm_set_chan_level(pwm_slice, PWM_CHAN_A, brightness);
}

/* Enter/Exit functions to prevent NULL pointer calls in main */
void enter_state_0(void) {
    leds_off();
}

void exit_state_0(void) {
    leds_off(); 
}

void enter_state_3() {
    leds_off();
    // Config GP0 for PWM mode
    pwm_slice = pwm_gpio_to_slice_num(0);
    gpio_set_function(0, GPIO_FUNC_PWM);
    
    // Set wrap (0-255)
    pwm_set_wrap(pwm_slice, 255);
    pwm_set_chan_level(pwm_slice, PWM_CHAN_A, 0);
    pwm_set_enabled(pwm_slice, true);
}

void exit_state_3() {
    // Restore SIO mode for GPIO
    gpio_set_function(0, GPIO_FUNC_SIO);
    pwm_set_enabled(pwm_slice, false);
    leds_off();
}

/* --- State Definitions --- */
const state_t s0 = {0, leds_off, do_state_0, leds_off, 200};
const state_t s1 = {1, leds_off, do_state_1, leds_off, 500};
const state_t s2 = {2, leds_off, do_state_2, leds_off, 200};
const state_t s3 = {3, enter_state_3, do_state_3, exit_state_3, 30};


/* !!! PART 1 !!! */
const state_t state0 = {
    0, 
    enter_state_0,
    do_state_0,
    exit_state_0, 
    500
};

/* !!! PART 2 !!! */
/* --- State Transition Table --- */
const state_t* state_table[4][3] = {
    /* Current S0 */ {&s2, &s1, &s3},
    /* Current S1 */ {&s0, &s2, &s3},
    /* Current S2 */ {&s1, &s0, &s3},
    /* Current S3 */ {&s0, &s0, &s0},
};

/* !!! ALL PARTS !!! */
int main() 
{
    private_init(); 

    const state_t * p_current_state = &s0;

    for(;;) 
    {
        p_current_state->Enter();

        while(1)
        {
            p_current_state->Do();

            sleep_ms(p_current_state->delay_ms);

            event_t evt = get_event();

            if (evt != no_evt) {
                p_current_state->Exit();
                p_current_state = state_table[p_current_state->id][evt];
                break;
            }
        }
    }
}