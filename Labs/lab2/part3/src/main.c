#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>

/* LED nodes from devicetree */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

/* Button node from devicetree */
#define BTN0_NODE DT_ALIAS(btn0)

/* GPIO specs */
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(LED3_NODE, gpios);
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BTN0_NODE, gpios);

/* Thread configuration */
#define STACK_SIZE 512
#define PRIORITY   5

/* Blink period for currently selected LED */
#define BLINK_DELAY_MS 300

/* Thread stacks */
K_THREAD_STACK_DEFINE(blink_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(button_stack, STACK_SIZE);

/* Thread control blocks */
static struct k_thread blink_thread_data;
static struct k_thread button_thread_data;

/* GPIO callback structure for button interrupt */
static struct gpio_callback button_cb_data;

/* Shared state: which LED is currently blinking (0..3) */
static int current_led = 0;

/* Record the start time */
static int64_t start_time = 0;

/* Semaphore for deferred interrupt handling */
K_SEM_DEFINE(button_sem, 0, 1);

/* Mutex to protect current_led */
K_MUTEX_DEFINE(led_mutex);

/* Turn all LEDs off */
static void leds_off(void)
{
	gpio_pin_set_dt(&led0, 0);
	gpio_pin_set_dt(&led1, 0);
	gpio_pin_set_dt(&led2, 0);
	gpio_pin_set_dt(&led3, 0);
}

/* Toggle only the selected LED */
static void toggle_selected_led(int led_index)
{
	switch (led_index) {
	case 0:
		gpio_pin_toggle_dt(&led0);
		break;
	case 1:
		gpio_pin_toggle_dt(&led1);
		break;
	case 2:
		gpio_pin_toggle_dt(&led2);
		break;
	case 3:
		gpio_pin_toggle_dt(&led3);
		break;
	default:
		break;
	}
}

/*
 * ISR: 
 * Only signal the button task that a button event happened.
 */
static void button_pressed_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	ARG_UNUSED(dev);
	ARG_UNUSED(cb);
	ARG_UNUSED(pins);

	k_sem_give(&button_sem);
}

/* Task 1: blink the currently selected LED */
void blink_task(void *arg1, void *arg2, void *arg3)
{
	ARG_UNUSED(arg1);
	ARG_UNUSED(arg2);
	ARG_UNUSED(arg3);
   
   int led_index;

	while (1) {
		k_mutex_lock(&led_mutex, K_FOREVER);
		led_index = current_led;
		k_mutex_unlock(&led_mutex);

		toggle_selected_led(led_index);
		k_msleep(BLINK_DELAY_MS);
	}
}

void switch_led()
{
   /* Acquire mutex lock */
   k_mutex_lock(&led_mutex, K_FOREVER);

   /* Stop all LEDs before switching to next one */
   leds_off();

   current_led = (current_led + 1) % 4;
   
   /* Release mutex lock */
   k_mutex_unlock(&led_mutex);   
}

/* Task 2: handle button presses outside ISR */
void button_task(void *arg1, void *arg2, void *arg3)
{
	ARG_UNUSED(arg1);
	ARG_UNUSED(arg2);
	ARG_UNUSED(arg3);

   int64_t end_time = 0;
   int64_t duration = 0;
   
	while (1) {
      if (start_time <= 0) {
         printk("\n*** 3-Second Game ***\n");
         printk("Push the button to START!\n");
         
         /* Wait until ISR signals a button press */
         k_sem_take(&button_sem, K_FOREVER);
         
         start_time = k_uptime_get();
         
         //switch_led();
         
         printk("Started! Please press the button again after 3 seconds\n");
         
         k_msleep(200); 
         
         k_sem_reset(&button_sem);
      }
      else {
         /* Wait until ISR signals a button press */
         k_sem_take(&button_sem, K_FOREVER);
         
         end_time = k_uptime_get();
         duration = end_time - start_time;
         
         start_time = 0;
         
         //switch_led();
         
         printk("You pressed the button after %lld ms\n", duration);
         
         k_msleep(500); 

         k_sem_reset(&button_sem);
      }
	}
}

int main(void)
{
	int ret;
    
   printk("USB Serial initialized!\n");

	/* Check GPIO devices are ready */
	if (!gpio_is_ready_dt(&led0) ||
	    !gpio_is_ready_dt(&led1) ||
	    !gpio_is_ready_dt(&led2) ||
	    !gpio_is_ready_dt(&led3) ||
	    !gpio_is_ready_dt(&button)) {
		return 0;
	}

	/* Configure LEDs as outputs, initially OFF */
	ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&led1, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&led2, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&led3, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		return 0;
	}

	/* Configure button as input */
	ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
	if (ret < 0) {
		return 0;
	}

	/* Configure interrupt on button press */
	ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret < 0) {
		return 0;
	}

	/* Register GPIO callback */
	gpio_init_callback(&button_cb_data, button_pressed_isr, BIT(button.pin));

	ret = gpio_add_callback_dt(&button, &button_cb_data);
	if (ret < 0) {
		return 0;
	}

	/* Start with all LEDs off */
	leds_off();

	/* Create blink task */
	k_thread_create(&blink_thread_data, blink_stack, STACK_SIZE,
			blink_task,
			NULL, NULL, NULL,
			PRIORITY, 0, K_NO_WAIT);

	/* Create button handler task */
	k_thread_create(&button_thread_data, button_stack, STACK_SIZE,
			button_task,
			NULL, NULL, NULL,
			PRIORITY, 0, K_NO_WAIT);

	/* Main thread does nothing hereinafter */
	while (1) {
		k_msleep(1000);
	}

	return 0;
}