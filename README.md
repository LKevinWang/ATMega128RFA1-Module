# ATMega128RFA1-Module
This design allows you to use a Minimum System based on ATMega128RFA1: 3.3V Power Supply 2.0MHz Oscillator, 2 Digital IO Pins and an SPI Slave Interface, to create a transceiver module of IEEE 802.15.4 PHY and MAC, with address and CRC filtering,  accompanied with specific High Data Rate Modes up to 2Mbps. You can use SPI interface to control it with access of every RF register.

## Hardware Connection

## Key Features

## Library Structure
This repository contains:
--src
    --
--avr_proj_win
    --
--example_RTL
    --Example code by VHDL to drive RF module for Init, Transmitting and Receiving. Along with some higher layers(MAC address filtering, Auto ACK, CSMA Retrial and UDP)
--examples
    --example_multiplePlatform
    --example_STM32FSerie_HAL
    --example_dsPIC33FSerie
    --example_nRF52840
--hardware
    --Schematic graph of example module with ordinary SPI and power supply with XH socket, and corresponded Gerber file.

