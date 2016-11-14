int device;

/* LDD3 p. 169 */
int init_gamepad()
{
  device = fopen("/dev/GPIOGamepad", "rb");
  if (device < 0)
  {
    printf("Could not load driver.\n");
    return -1;
  }

  if (signal(SIGIO, &input_handler) == SIG_ERR)
  {
    printf("Could not register signal handler.\n");
    return -1;
  }

  if (fcntl(device, F_SETOWN, getpid()) < 0)
  {
    printf("Could not specify this process as the owner.\n");
    return -1;
  }

  long oflags = fcntl(device, F_GETFL); // gets file access mode and file status flags

  if (fcntl(device, F_SETFL, oflags | FASYNC) < 0)
  {
    printf("Could not set file status flags.\n");
    return -1;
  }

  return 0; // Success
}

/* LDD3 p. 169 */
void input_handler(int sigio)
{
  char buffer[1];
  int gpio_value = read(device, &buffer, 1);

  if (gpio_value[0] == 0b0)
  {
    // some button pressed
  }
  else if (gpio_value[1] == 0b0)
  {
    // some other button pressed
  }
  // ...

  delete[] buffer;
}
