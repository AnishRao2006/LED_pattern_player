#if defined (CY_USING_HAL)
#include "cyhal.h"
#endif
#include "cybsp.h"
#include "cy_retarget_io.h"
#include <stdio.h>

//flags for button and timer
volatile bool button_flag = false;

// button
#define BTN CYBSP_USER_BTN
uint8_t button_press = 0;
static void button_isr(void *button_arg, cyhal_gpio_event_t event);

// time for LEDs
#define DEBOUNCE_TIME 3
#define SLOW 50

// timers init
#define timer_ticks 100
#define timer_freq 10000
volatile uint32_t tick_counter = 0;
volatile uint32_t tick_counter_led = 0;
volatile bool current_reading = false;
volatile bool stable_reading = false;
volatile uint32_t current_tick = 0;
cyhal_timer_t timer_obj;
static void timer_isr(void *timer_arg, cyhal_timer_event_t event);

// enum for led states
typedef enum 
{
    OFF,
    SLOW_BLINK,
    FAST_BLINK,
    DOUBLE_BLINK,
    HEARTBEAT
}led_states;

int main(void)
{
    cy_rslt_t result;
    cy_rslt_t uart_init;

#if defined (CY_DEVICE_SECURE) && defined (CY_USING_HAL)
    cyhal_wdt_t wdt_obj;

    /* Clear watchdog timer so that it doesn't trigger a reset */
    result = cyhal_wdt_init(&wdt_obj, cyhal_wdt_get_max_timeout_ms());
    CY_ASSERT(CY_RSLT_SUCCESS == result);
    cyhal_wdt_free(&wdt_obj);
#endif

    /* Initialize the device and board peripherals */
    result = cybsp_init();

    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Enable global interrupts */
    __enable_irq();

    // uart init
    uart_init = cy_retarget_io_init_fc(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, 
                                       CYBSP_DEBUG_UART_CTS, CYBSP_DEBUG_UART_RTS, 
                                       CY_RETARGET_IO_BAUDRATE);
    if(uart_init != CY_RSLT_SUCCESS)
    {
        printf("uart failed to initialize\r\n");
    }

    // LED
    uint32_t led_init;
    led_init = cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, 1);

    if(led_init != CY_RSLT_SUCCESS)
    {
        printf("led failed to initialize\r\n");
    }

    // button init
    uint32_t btn_init;
    btn_init = cyhal_gpio_init(BTN, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_PULLUP, 1);

    if(btn_init != CY_RSLT_SUCCESS)
    {
        printf("button failed to initialize\r\n");
    }

    // assigning callback function
    cyhal_gpio_callback_data_t button;
    button.callback = button_isr;

    // callback for button
    cyhal_gpio_register_callback(BTN, &button);

    // enabling the event
    cyhal_gpio_enable_event(BTN, CYHAL_GPIO_IRQ_FALL, 6, 1);

    // configure the timer
    const cyhal_timer_cfg_t timer1 = (cyhal_timer_cfg_t){true, CYHAL_TIMER_DIR_UP, false, timer_ticks, 0, 0};

    // init the timer
    cy_rslt_t timer_init;
    timer_init = cyhal_timer_init(&timer_obj, NC, NULL);

    if(timer_init != CY_RSLT_SUCCESS)
    {
        printf("timer init failed to initialize\r\n");
    }

    // configure the timer
    cy_rslt_t timer_config;
    timer_config = cyhal_timer_configure(&timer_obj, &timer1);

    if(timer_config != CY_RSLT_SUCCESS)
    {
        printf("timer config failed to initialize\r\n");
    }

    // setting the freq
    cy_rslt_t timer_period;
    timer_period = cyhal_timer_set_frequency(&timer_obj, timer_freq);

    if(timer_period != CY_RSLT_SUCCESS)
    {
        printf("timer freq failed to initialize\r\n");
    }

    // callback
    cyhal_timer_register_callback(&timer_obj, timer_isr, NULL);

    // enable event
    cyhal_timer_enable_event(&timer_obj, CYHAL_TIMER_IRQ_TERMINAL_COUNT, 7, 1);

    // start timer
    cy_rslt_t start;
    start = cyhal_timer_start(&timer_obj);

    if(start != CY_RSLT_SUCCESS)
    {
        printf("start timer failed to initialize\r\n");
    }

    // welcome banner
    printf("Welcome to LED Pattern Player!!!!!\r\n");

    // infinite loop
    while(1)
    {   
        // check for change in state and capture the current tick
        if(button_flag == true)
        {
            button_flag = false;
            current_reading = true;
            current_tick = tick_counter;
        }

        // wait for debounce time 
        if(current_reading && (tick_counter - current_tick) >= DEBOUNCE_TIME)
        {
            if(current_reading == true)
            {   
                current_reading = false;
                stable_reading = true;
            }
        }

        // using stable reading for final decision making
        if(stable_reading == true)
        {
            stable_reading = false;

            // wrap around
            if(button_press == 4) 
            {
                button_press = 0;
            }
            else 
            {
                button_press++;
            }

            // printing on terminal
            switch(button_press)
            {
                case OFF :
                {
                    printf("led is off\r\n");
                    tick_counter_led = tick_counter;
                    break;
                }
                case SLOW_BLINK :
                {
                    printf("led is blinking slow\r\n");
                    tick_counter_led = tick_counter;
                    break;
                }
                case FAST_BLINK :
                {
                    printf("led is blinking fast\r\n");
                    tick_counter_led = tick_counter;
                    break;
                }
                case DOUBLE_BLINK : 
                {
                    printf("led is blinking twice\r\n");
                    tick_counter_led = tick_counter;
                    break;
                }
                case HEARTBEAT : 
                {
                    printf("led is heartbeating\r\n");
                    tick_counter_led = tick_counter;
                    break;
                }
                default : 
                {
                    printf("led is invalid\r\n");
                    tick_counter_led = tick_counter;
                    break;
                }
            }
        }

        // using the button press to actually blink LED
        switch(button_press)
        {
            case OFF :
            {
                // led is off
                cyhal_gpio_write(CYBSP_USER_LED,0);
                break;
            }
            case SLOW_BLINK :
            {
                // 500ms on 500ms off
                if((((tick_counter - tick_counter_led) % 100) / SLOW) == 1)
                {
                    cyhal_gpio_write(CYBSP_USER_LED,0);
                }
                else 
                {
                    cyhal_gpio_write(CYBSP_USER_LED,1);
                }
                break;
            }
            case FAST_BLINK :
            {
                // 100ms on 100ms off
                if((((tick_counter - tick_counter_led) % 20) / 10) == 1)
                {
                    cyhal_gpio_write(CYBSP_USER_LED,0);
                }
                else 
                {
                    cyhal_gpio_write(CYBSP_USER_LED,1);
                }
                break;
            }
            case DOUBLE_BLINK : 
            {
                // pause is 100ms after 2 blinks of 50ms each
                if(((tick_counter - tick_counter_led) % 30) >= 20 && ((tick_counter - tick_counter_led) % 30) < 30)
                {   
                    cyhal_gpio_write(CYBSP_USER_LED,0);
                }
                else if((((tick_counter - tick_counter_led) % 10) / 5) == 1)
                {
                    cyhal_gpio_write(CYBSP_USER_LED,0);
                }
                else 
                {
                    cyhal_gpio_write(CYBSP_USER_LED,1);
                }
                break;
            }
            case HEARTBEAT : 
            {
                // pause is 150ms after 2 blinks of 50ms each
                if(((tick_counter - tick_counter_led) % 35) >= 20 && ((tick_counter - tick_counter_led) % 35) < 35)
                {   
                    cyhal_gpio_write(CYBSP_USER_LED,0);
                }
                else if((((tick_counter - tick_counter_led) % 10) / 5) == 1)
                {
                    cyhal_gpio_write(CYBSP_USER_LED,0);
                }
                else 
                {
                    cyhal_gpio_write(CYBSP_USER_LED,1);
                }
                break;
            }
            default : 
            {
                cyhal_gpio_write(CYBSP_USER_LED,0);
                break;
            }
        }
    }

    return 0;
}

// timer isr
static void timer_isr(void *timer_arg, cyhal_timer_event_t event)
{
    // 10ms
    tick_counter++;
}

// button isr
static void button_isr(void *button_arg, cyhal_gpio_event_t event)
{
    button_flag = true;
}

/* [] END OF FILE */
