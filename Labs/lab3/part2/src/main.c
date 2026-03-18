#include <zephyr/kernel.h>
#include <stdio.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>

//west build -p always -b rpi_pico2/rp2350a/m33 -S cdc-acm-console

int main(void)
{
   const struct device *bme = DEVICE_DT_GET_ANY(bosch_bme680);

   if (bme == NULL)
   {
        printf("No BME680 device found.\n");
        fflush(stdout);
        return -1;
   }

   if (!device_is_ready(bme))
   {
      printf("BME680 device is not ready.\n");
      fflush(stdout);
      return -1;
   }

   printf("BME680 found and ready.\n");
   fflush(stdout);

   while (1)
   {
      struct sensor_value temp, humidity;
      int ret;

      ret = sensor_sample_fetch(bme);
      if (ret < 0)
      {
         printf("Failed to fetch sample: %d\n", ret);
         fflush(stdout);
         k_sleep(K_SECONDS(3));
         continue;
      }

        ret = sensor_channel_get(bme, SENSOR_CHAN_AMBIENT_TEMP, &temp);
        if (ret < 0)
        {
            printf("Failed to get temperature channel: %d\n", ret);
            fflush(stdout);
            k_sleep(K_SECONDS(3));
            continue;
        }
        
        ret = sensor_channel_get(bme, SENSOR_CHAN_HUMIDITY, &humidity);
        if (ret < 0)
        {
            printf("Failed to get humidity channel: %d\n", ret);
            fflush(stdout);
            k_sleep(K_SECONDS(3));
            continue;
        }

        printf("Temperature: %d.%06dC; Humidity: %d.%06d%% \n", (int)temp.val1, (int)temp.val2, (int)humidity.val1, (int)humidity.val2);
        fflush(stdout);

        k_sleep(K_SECONDS(3));
    }

    return 0;
}
