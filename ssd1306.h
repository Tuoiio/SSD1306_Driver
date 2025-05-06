#ifndef _SSD1306_H_
#define _SSD1306_H_

#include <linux/i2c.h>
#include <linux/miscdevice.h>

#define SSD1306_MAX_SEG 128
#define SSD1306_MAX_LINE 7
#define SSD1306_DEF_FONT_SIZE 5

struct ssd1306_i2c_module {
    struct i2c_client *client;
    struct miscdevice misc_dev;
    uint8_t line_num;
    uint8_t cursor_position;
    uint8_t font_size;
};

int ssd1306_i2c_write(struct ssd1306_i2c_module *module, unsigned char *buf, unsigned int len);

int ssd1306_i2c_read(struct ssd1306_i2c_module *module, unsigned char *out_buf, unsigned int len);

void ssd1306_write(struct ssd1306_i2c_module *module, bool is_cmd, unsigned char data);

void ssd1306_set_cursor(struct ssd1306_i2c_module *module, uint8_t line_num, uint8_t cursor_position);

void ssd1306_goto_next_line(struct ssd1306_i2c_module *module);

void ssd1306_print_char(struct ssd1306_i2c_module *module, unsigned char c);

void ssd1306_print_string(struct ssd1306_i2c_module *module, unsigned char *str);

void ssd1306_set_brightness(struct ssd1306_i2c_module *module, uint8_t brightness);

void ssd1306_clear(struct ssd1306_i2c_module *module);

int ssd1306_display_init(struct ssd1306_i2c_module *module);

#endif
