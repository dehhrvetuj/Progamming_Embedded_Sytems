#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* Get LED nodes from DT aliases */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

/* Create GPIO specifications for each LED */
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(LED3_NODE, gpios);

/* Thread stack size and priority */
#define STACK_SIZE 512
#define PRIORITY 5

/* Blink periods (milliseconds) */
#define LED0_DELAY_MS 200
#define LED1_DELAY_MS 400
#define LED2_DELAY_MS 600
#define LED3_DELAY_MS 800

/* Thread stacks */
K_THREAD_STACK_DEFINE(led0_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(led1_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(led2_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(led3_stack, STACK_SIZE);

/* Thread control blocks */
static struct k_thread led0_thread_data;
static struct k_thread led1_thread_data;
static struct k_thread led2_thread_data;
static struct k_thread led3_thread_data;

/* Thread function for LED0 */
void blink_led0(void *arg1, void *arg2, void *arg3)
{
	ARG_UNUSED(arg1);
	ARG_UNUSED(arg2);
	ARG_UNUSED(arg3);

	while (1) {
		gpio_pin_toggle_dt(&led0);
		k_msleep(LED0_DELAY_MS);
	}
}

/* Thread function for LED1 */
void blink_led1(void *arg1, void *arg2, void *arg3)
{
	ARG_UNUSED(arg1);
	ARG_UNUSED(arg2);
	ARG_UNUSED(arg3);

	while (1) {
		gpio_pin_toggle_dt(&led1);
		k_msleep(LED1_DELAY_MS);
	}
}

/* Thread function for LED2 */
void blink_led2(void *arg1, void *arg2, void *arg3)
{
	ARG_UNUSED(arg1);
	ARG_UNUSED(arg2);
	ARG_UNUSED(arg3);

	while (1) {
		gpio_pin_toggle_dt(&led2);
		k_msleep(LED2_DELAY_MS);
	}
}

/* Thread function for LED3 */
void blink_led3(void *arg1, void *arg2, void *arg3)
{
	ARG_UNUSED(arg1);
	ARG_UNUSED(arg2);
	ARG_UNUSED(arg3);

	while (1) {
		gpio_pin_toggle_dt(&led3);
		k_msleep(LED3_DELAY_MS);
	}
}

int main(void)
{
   if (usb_enable(NULL)) {
        return 0;
   }
    
   k_sleep(K_MSEC(1000)); 
    
   printk("USB Serial initialized!\n");
   
	/* Check that all GPIO devices are ready */
	if (!gpio_is_ready_dt(&led0) ||
	    !gpio_is_ready_dt(&led1) ||
	    !gpio_is_ready_dt(&led2) ||
	    !gpio_is_ready_dt(&led3)) {
		return 0;
	}

	/* Configure all LEDs as outputs, initially OFF */
	gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&led1, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&led2, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&led3, GPIO_OUTPUT_INACTIVE);

	/* Create 4 threads, one for each LED */
	k_thread_create(&led0_thread_data, led0_stack, STACK_SIZE,
			blink_led0,
			NULL, NULL, NULL,
			PRIORITY, 0, K_NO_WAIT);

	k_thread_create(&led1_thread_data, led1_stack, STACK_SIZE,
			blink_led1,
			NULL, NULL, NULL,
			PRIORITY, 0, K_NO_WAIT);

	k_thread_create(&led2_thread_data, led2_stack, STACK_SIZE,
			blink_led2,
			NULL, NULL, NULL,
			PRIORITY, 0, K_NO_WAIT);

	k_thread_create(&led3_thread_data, led3_stack, STACK_SIZE,
			blink_led3,
			NULL, NULL, NULL,
			PRIORITY, 0, K_NO_WAIT);

	/* main thread can sleep forever */
	while (1) {
		k_msleep(1000);
	}

	return 0;
}