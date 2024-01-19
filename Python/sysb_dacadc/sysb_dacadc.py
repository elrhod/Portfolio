"""
sysb_dacadc.py
====================================
"""
from serial import Serial
import sysb_dacadc_pb2 as pb
import crcmod.predefined
import numpy as np

class Sysbdacadc:
    """Creates an Sysbdacadc instance and opens the comport to communicate sysboard.

    Args:
        comport (string): Serial port to use, ie "COM4"
        timeout (float):  Serial timeout in seconds

    Returns:
        Sysbdacadc: instance for controlling the ADC and DAC of Sysboard.

    """
    def __init__(self, comport, timeout=1, baudrate=115200):
        self.ser = Serial(comport, timeout=timeout, baudrate=baudrate)

    def _handle_error(self, error, command):
        if error.code == pb.ErrorCode.DECODING:
            print('The MCU could not decode the message. ' + error.message)
            self._send_command(command)
#             raise IOError('The MCU could not decode the message. ' + error.message)
        if error.code == pb.ErrorCode.CRC:
            raise IOError('The CRC check failed on transmission from PC to the MCU. ' + error.message)
        elif error.code == pb.ErrorCode.OUT_OF_RANGE:
            raise ValueError('A scanner parameter was set outside of the valid range. ' + error.message)
        elif error.code == pb.ErrorCode.MISSING_PARAMETER:
            raise ValueError('A command parameter was missing. ' + error.message)
        elif error.code != pb.ErrorCode.OK:
            print(error.code)
#             raise ValueError('An unhandled error code was passed back from the MCU. ' + error.message)
            print('An unhandled error code was passed back from the MCU. ' + error.message)
            self._send_command(command)

    def _send_command(self, command):
        """
        This is a private function that sends and receives the command from the MCU.
        This function wraps the encoded message with message length and a CRC for transmission error checking.
        CRC errors and errors sent back in the reply are handled by this method.

        Args:
            command (Command): The protobuf command to be sent to the MCU.
        Returns:
            Command: The decoded protobuf command returned from the MCU.
        """
        self.ser.flushOutput()

        string = command.SerializeToString()
        length_bytes = bytearray([(len(string) >> 8) &  0xFF, len(string) & 0xFF])
        #time.sleep(0.01)
        self.ser.write(length_bytes)
#         print(string)
        self.ser.write(string)
        crc16 = crcmod.predefined.Crc('crc-16')
        crc16.update(string)
        crc_bytes = bytearray([(crc16.crcValue >> 8) & 0xFF, crc16.crcValue & 0xFF])
        self.ser.write(crc_bytes)
#         print(f"crc_bytes: {crc_bytes}")

        retry_n = 10
        for i in range(retry_n):
            # time.sleep(.01)
            r = self.ser.read(1)
            if len(r) > 0:
                break
            else:
                print("Error receiving data, retrying...")
                self.ser.write(length_bytes)
                self.ser.write(string)
                self.ser.write(crc_bytes)
            if i == retry_n - 1:
                raise IOError('The MCU did not respond. Please check the connection')

        reply_binary_length1 = ord(r)
        reply_binary_length0 = ord(self.ser.read(1))
#         print(f"reply_binary_length0: {reply_binary_length0}")
        reply_binary_length = reply_binary_length1 << 8 | reply_binary_length0
        reply_binary = self.ser.read(reply_binary_length)

        reply_crc01 = ord(self.ser.read(1))
        reply_crc0 = ord(self.ser.read(1))
        reply_crc = reply_crc01 << 8 | reply_crc0

        self.ser.flushInput()

        crc16 = crcmod.predefined.Crc('crc-16')
        crc16.update(reply_binary)
#         print(f'crc16.crcValue:{crc16.crcValue} reply_crc: {reply_crc}, reply_binary_length: {reply_binary_length}')

        if crc16.crcValue != reply_crc:
            print(f'crc16.crcValue:{crc16.crcValue} reply_crc: {reply_crc}')
            raise IOError('The CRC check failed on transmission from the MCU to the PC.')

        reply = pb.Command()
        reply.ParseFromString(reply_binary)

        if reply.error.code!=pb.ErrorCode.OK:
            self._handle_error(reply.error ,command)
            print(f'Error Code:{reply.error.code}')
#         else:
#             print("Error message:"+reply.error.message)

        return reply

    # def write_tx_dac(self, channel, value):
    #     """
    #     Writes to any of the 4 channels of the TX DAC (LTC2664) that are available at the IO board
    #
    #     Args:
    #         channel (string): '0', '1', '2', '3'
    #         value (float) : Voltage from 0 to 5 volts
    #     """
    #     command = pb.Command()
    #     command.name = pb.CommandName.WRITE_TX_DAC
    #     assert channel in ['0', '1', '2', '3'], "Error: channel must be '0', '1', '2', '3'"
    #     if value < 0 or value> 5:
    #         raise TypeError("Error: value must be between 0 and 5")
    #
    #     pb_channels = [pb.ChannelNum.CHANNEL_NUM_0, pb.ChannelNum.CHANNEL_NUM_1, pb.ChannelNum.CHANNEL_NUM_2, pb.ChannelNum.CHANNEL_NUM_3]
    #     channels = ['0', '1', '2', '3']
    #     i = channels.index(channel)
    #     command.dac_state.channel = pb_channels[i]
    #     command.dac_state.value = value
    #     self._send_command(command)

    def write_cfg_dac(self, channel, value):
        """
        Writes to any of the 4 channels of the CFG DAC (LTC2668) that are available at the IO board

        Args:
            channel (string): '0', '1', '2', '3''
            value (float) : Voltage from 0 to 5 volts
        """
        command = pb.Command()
        command.name = pb.CommandName.WRITE_CFG_DAC
        assert channel in ['0', '1', '2', '3'], "Error: channel must be '0', '1', '2', '3'"
        # assert value in range (0, 5), "Error: value must be between 0 and 5"
        pb_channels = [pb.ChannelNum.CHANNEL_NUM_0, pb.ChannelNum.CHANNEL_NUM_1, pb.ChannelNum.CHANNEL_NUM_2, pb.ChannelNum.CHANNEL_NUM_3]
        channels = ['0', '1', '2', '3']
        i = channels.index(channel)
        command.dac_state.channel = pb_channels[i]
        command.dac_state.value = value
        self._send_command(command)

    def read_ss_adc(self, channel):
        """
        Reads one of the 4 channels of the Simultaneous ADC (LTC2325)

        Args:
            channel (string): '0', '1', '2', '3'

        Returns:
            uint32_t: Voltage value from requested channel (16 bits resolution)
        """

        channels = ('0', '1', '2', '3')

        assert channel in ['0', '1', '2', '3'], "Error: channel must be '0', '1', '2' and '3'"

        pb_channel = [pb.ChannelNum.CHANNEL_NUM_0, pb.ChannelNum.CHANNEL_NUM_1, pb.ChannelNum.CHANNEL_NUM_2, pb.ChannelNum.CHANNEL_NUM_3]
        pb_user_channel = pb_channel[channels.index(channel)]

        command = pb.Command()
        command.name = pb.CommandName.READ_SS_ADC
        command.adc_state.channel = pb_user_channel

        reply = self._send_command(command)

        return reply.adc_state.value

    def write_sel(self, sel, val):
        """
        Writes 0 or 1 to sel 1 or sel 2

        Args:
            sel (string): '1', '2' chosen sel
            val (string): '0', '1' valut low or high to set or reset sel
        """
        command = pb.Command()
        command.name = pb.CommandName.WRITE_SEL
        assert sel in ['1', '2'], "Error: sel must be '1' or '2'"
        assert val in ['0', '1'], "Error: val must be '0' or '1'"
        # assert value in range (0, 5), "Error: value must be between 0 and 5"
        pb_sels = [pb.SelNum.SEL_NUM_1, pb.SelNum.SEL_NUM_2]
        pb_vals = [pb.LogicVal.LOGIC_0, pb.LogicVal.LOGIC_1]
        sels = ['1', '2']
        vals = ['0', '1']
        i = sels.index(sel)
        j = vals.index(val)
        command.sel_state.num = pb_sels[i]
        command.sel_state.val = pb_sels[j]
        self._send_command(command)
