#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>


struct cursor_pos {
    int x;
    int y;
};

#define IOCTL_MAGIC 'S'
#define IOCTL_SSD1306_CLEAR _IO(IOCTL_MAGIC, 0)
#define IOCTL_SSD1306_SET_CURSOR _IOW(IOCTL_MAGIC, 1, struct cursor_pos)

int main() {
    int fd = open("/dev/ssd1306", O_RDWR);
    if (fd == -1) {
        perror("Failed to open device");
        return 1;
    }

    // Gọi IOCTL để làm rõ màn hình
    if (ioctl(fd, IOCTL_SSD1306_CLEAR) < 0) {
        perror("ioctl failed");
        close(fd);
        return 1;
    }
    struct cursor_pos pos;
    pos.x = 1;
    pos.y = 1;

    if (ioctl(fd, IOCTL_SSD1306_SET_CURSOR, &pos) < 0) {
        perror("ioctl failed");
        close(fd);
        return -1;
    }

    char *text = "Duong Xuan Tuoi";
    write(fd, text, strlen(text));

    sleep(3);

    if (ioctl(fd, IOCTL_SSD1306_CLEAR) < 0) {
        perror("ioctl failed");
        close(fd);
        return 1;
    }

    // Thực hiện một số thao tác khác ở đây nếu cần

    close(fd);
    return 0;
}
