#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define DEVICE_FILE "/dev/ioctl_device"
#define IOCTL_SET_VALUE _IOW('a', 1, int)
#define IOCTL_GET_VALUE _IOR('a', 2, int)

int main() {
    int fd, value = 100, received_value;

    fd = open(DEVICE_FILE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return -1;
    }

    // Send a value to the driver
    printf("Setting value to %d\n", value);
    if (ioctl(fd, IOCTL_SET_VALUE, &value) < 0) {
        perror("Failed to set value");
        close(fd);
        return -1;
    }

    // Get the stored value from the driver
    if (ioctl(fd, IOCTL_GET_VALUE, &received_value) < 0) {
        perror("Failed to get value");
        close(fd);
        return -1;
    }

    printf("Received value from driver: %d\n", received_value);

    close(fd);
    return 0;
}
