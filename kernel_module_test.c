#include <linux/input.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <asm/io.h>

// #define BUTTON_IRQ  1        /* replace with your real IRQ */
// #define BUTTON_PORT 0x379    /* replace with your real port */

// static struct input_dev *button_dev;

// static irqreturn_t button_interrupt(int irq, void *dummy)
// {
//     int pressed = inb(BUTTON_PORT) & 1;

//     input_report_key(button_dev, BTN_0, pressed);
//     input_sync(button_dev);
//     return IRQ_HANDLED;
// }

// static int __init button_init(void)
// {
//     int error;

//     // if (!request_region(BUTTON_PORT, 1, "button")) {
//     //     pr_err("button: I/O port 0x%x busy\n", BUTTON_PORT);
//     //     return -EBUSY;
//     // }

//     // if (request_irq(BUTTON_IRQ, button_interrupt, IRQF_SHARED, "button", NULL)) {
//     //     pr_err("button: Can't allocate irq %d\n", BUTTON_IRQ);
//     //     release_region(BUTTON_PORT, 1);
//     //     return -EBUSY;
//     // }

//     button_dev = input_allocate_device();
//     if (!button_dev) {
//         pr_err("button: Not enough memory\n");
//         error = -ENOMEM;
//         goto err_free_irq;
//     }

//     __set_bit(EV_KEY, button_dev->evbit);
//     __set_bit(BTN_0, button_dev->keybit);
//     __set_bit(BTN_1, button_dev->keybit);

//     pr_info("hi: %lu", button_dev->keybit);


//     error = input_register_device(button_dev);
//     if (error) {
//         pr_err("button: Failed to register device\n");
//         goto err_free_dev;
//     }
    
//     input_report_key(button_dev, BTN_0, 1);
//     input_sync(button_dev);

//     // while (false) {
//     //     input_report_key(button_dev, BTN_0, 1);
//     //     input_sync(button_dev);
//     // }
    
//     pr_info("button: Module loaded\n");
//     return 0;

// err_free_dev:
//     input_free_device(button_dev);
// err_free_irq:
//     free_irq(BUTTON_IRQ, NULL);
//     release_region(BUTTON_PORT, 1);
//     return error;
// }


struct input_dev *udev;

static int __init button_init(void) {
    
    udev = input_allocate_device();
    udev->name = "I CAN CONNECT TO DOLPHIN!!!!";
    __set_bit(EV_KEY, udev->evbit);
    __set_bit(EV_ABS, udev->evbit);

    // Buttons
    __set_bit(BTN_A, udev->keybit);
    __set_bit(BTN_B, udev->keybit);
    __set_bit(BTN_X, udev->keybit);
    __set_bit(BTN_Y, udev->keybit);

    // Analog stick axes
    input_set_abs_params(udev, ABS_X, -32768, 32767, 0, 0);
    input_set_abs_params(udev, ABS_Y, -32768, 32767, 0, 0);

    input_register_device(udev);


    input_report_key(udev, BTN_A, 1);
    input_sync(udev);

    return 0;

}


static void __exit button_exit(void)
{
    input_unregister_device(udev);
    // free_irq(BUTTON_IRQ, NULL);
    // release_region(BUTTON_PORT, 1);
    pr_info("button: Module unloaded\n");
}



module_init(button_init);
module_exit(button_exit);

MODULE_LICENSE("GPL");
