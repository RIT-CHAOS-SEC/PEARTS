# IMPORTANT NOTE:  THIS repo is moving to https://github.com/SPINS-RG/pearts with a port for raspberry pi pico 2 and will be available soon


# PEARTS

This is the open source repository for the paper PEARTS: 
Provable Execution in Real-Time Embedded Systems.



# Note: This Page is Under Construction

Please note that this page is currently under construction. We are in the process of finalizing the code and documentation for PEARTS library. 

The code will be made available in a library format, along with comprehensive documentation, upon the publication of our research paper. 

We appreciate your patience and understanding. Stay tuned for updates!



# License

PEARTS is released under the [MIT License](https://opensource.org/licenses/MIT).



## Reproducing the Code

To reproduce the code in this repository, follow these steps:

1. Download `STM32CubeProgrammer 1.9.0 v2.10.0` from the official website.

2. Connect the `NUCLEO-L552ZE-Q board` to your computer using a USB port.

3. Open STM32CubeProgrammer and establish a connection with the NUCLEO-L552ZE-Q board.

4. Activate TrustZone by following these steps:
    - Go to the user configuration settings and set the `TZEN` and `DBANK` variables.
    - Navigate to the security areas configuration and set them as below:

---
### Secure Area 1
NAME | Value 
-|-
SECWM1_PSTRT | Value = 0x0; Address = 0x08000000 
SECWM1_PEND | Value = 0x7f; Address = 0x0803f800

### Secure Area 2
NAME | Value 
-|-
SECWM1_PSTRT | Value = 0x1; Address = 0x08040800 
SECWM1_PEND | Value = 0x0;  Address = 0x08040000

---

5. Download `STM32CubeIDE 1.9.0` from the official website.

By following these steps, you will be able to reproduce the code in this repository.


--- 

## Running a Task

The shadow task is located at `NonSecure/core/Src/rtpox_tasks`. It serves as a bridge between the secure and non-secure worlds in the PEARTS library. The shadow task provides an interface for communication and coordination between the secure and non-secure tasks.

To use the shadow task, follow these steps:

1. Open the `rtpox_tasks.c` file located at `NonSecure/core/Src/rtpox_tasks`.

2. Implement the necessary functionality within the wrapper task using the function `run_application()`. 

Additional functionalities and usage details for PEARTS will be added soon as we update the documentation and rearrange the code.
