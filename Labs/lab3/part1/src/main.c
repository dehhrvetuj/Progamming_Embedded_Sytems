#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <stdio.h>

#include <bme680_reg.h>

#define BME680_ADDR     0x77

#define I2C_LABEL       i2c0

int main(void)
{
    const struct device *i2c_bus = DEVICE_DT_GET(DT_NODELABEL(I2C_LABEL));

    if (i2c_bus == NULL)
    {
        printf("No device found; did initialization fail?\n");
        fflush(stdout);
        return -1;
    } 
    
    /* Try and read chip id */
    uint8_t id;
    if (i2c_reg_read_byte(i2c_bus, BME680_ADDR, BME680_ID, &id) < 0){
        printf("Could not communicate with sensor.\n"); 
        fflush(stdout);
        return -1; 
    }
    
    if (id != 0x61)
    {
        printf("Sensor ID could not be read from I2C device.\n");
        fflush(stdout);
        return -1;
    }

    printf("Everything is working.\n");
    fflush(stdout);
    
    /* Read temperature calibration parameters */
    uint8_t par_t1_lsb;
    uint8_t par_t1_msb;
    uint8_t par_t2_lsb;
    uint8_t par_t2_msb;
    uint8_t par_t3_raw;

    if (i2c_reg_read_byte(i2c_bus, BME680_ADDR, 0xE9, &par_t1_lsb) < 0 ||
        i2c_reg_read_byte(i2c_bus, BME680_ADDR, 0xEA, &par_t1_msb) < 0 ||
        i2c_reg_read_byte(i2c_bus, BME680_ADDR, 0x8A, &par_t2_lsb) < 0 ||
        i2c_reg_read_byte(i2c_bus, BME680_ADDR, 0x8B, &par_t2_msb) < 0 ||
        i2c_reg_read_byte(i2c_bus, BME680_ADDR, 0x8C, &par_t3_raw) < 0)
    {
        printf("Failed to read calibration parameters.\n");
        fflush(stdout);
        return -1;
    }
    
    uint16_t par_t1 = ((uint16_t)par_t1_msb << 8) | par_t1_lsb;
    int16_t par_t2 = (int16_t)(((uint16_t)par_t2_msb << 8) | par_t2_lsb);
    int8_t par_t3 = (int8_t)par_t3_raw;
    
    while (1)
    {
        uint8_t ctrl_meas;
        uint8_t temp_msb, temp_lsb, temp_xlsb;
        uint32_t temp_adc;
        
        int64_t var1;
        int64_t var2;
        int64_t var3;
        int32_t t_fine;
        int32_t temp_comp;

        /* temp oversampling x1, pressure skip, forced mode */
        ctrl_meas = 0x20 | 0x01;

        if (i2c_reg_write_byte(i2c_bus, BME680_ADDR, BME680_CTRL_MEAS, ctrl_meas) < 0)
        {
            printf("Failed to start measurement.\n");
            fflush(stdout);
            k_sleep(K_SECONDS(3));
            continue;
        }

        k_msleep(10);

        if (i2c_reg_read_byte(i2c_bus, BME680_ADDR, BME680_TEMP_MSB, &temp_msb) < 0)
        {
            printf("Failed to read TEMP_MSB.\n");
            fflush(stdout);
            k_sleep(K_SECONDS(3));
            continue;
        }

        if (i2c_reg_read_byte(i2c_bus, BME680_ADDR, BME680_TEMP_LSB, &temp_lsb) < 0)
        {
            printf("Failed to read TEMP_LSB.\n");
            fflush(stdout);
            k_sleep(K_SECONDS(3));
            continue;
        }

        if (i2c_reg_read_byte(i2c_bus, BME680_ADDR, BME680_TEMP_XLSB, &temp_xlsb) < 0)
        {
            printf("Failed to read TEMP_XLSB.\n");
            fflush(stdout);
            k_sleep(K_SECONDS(3));
            continue;
        }

        temp_adc = ((uint32_t)temp_msb << 12) |
                   ((uint32_t)temp_lsb << 4)  |
                   ((uint32_t)temp_xlsb >> 4);

        printf("Raw temperature ADC = %u\n", temp_adc);
        fflush(stdout);
        
        /* Bosch compensation formula */
        var1 = (((int32_t)temp_adc >> 3) - ((int32_t)par_t1 << 1));
        var2 = (var1 * (int32_t)par_t2) >> 11;
        var3 = ((((var1 >> 1) * (var1 >> 1)) >> 12) *
               ((int32_t)par_t3 << 4)) >> 14;

        t_fine = (int32_t)(var2 + var3);
        temp_comp = ((t_fine * 5) + 128) >> 8;   /* 0.01 °C */

        printf("Raw temperature ADC = %u, Temperature = %d.%02d C\n",
               temp_adc,
               temp_comp / 100,
               temp_comp < 0 ? -(temp_comp % 100) : (temp_comp % 100));
        fflush(stdout);

        k_sleep(K_SECONDS(3));

    }

}

