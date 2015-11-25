// *****************************************************************************
//
// Copyright (c) 2015, Southwest Research Institute® (SwRI®)
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Southwest Research Institute® (SwRI®) nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// *****************************************************************************

#ifndef SERIAL_UTIL_SERIAL_CONSOLE_H_
#define SERIAL_UTIL_SERIAL_CONSOLE_H_

#include <swri_serial_util/serial_port.h>

#include <string>
#include <vector>

namespace swri_serial_util
{

  /**
   * Structure defining serial port configuration parameters for a serial
   * console
   */
  struct SerialConsoleConfig : public SerialConfig
  {
    /**
     * Default constructor.
     *
     * baud = 115200
     * parity = NO_PARITY
     * flow control = false
     * data bits = 8
     * stop bits = 1
     * low latency mode = false
     * line_end = \r (carriage return)
     */
    SerialConsoleConfig();

    SerialConsoleConfig(
        int32_t baud,
        int32_t data_bits,
        int32_t stop_bits,
        Parity parity,
        bool flow_control,
        bool low_latency_mode,
        bool writable,
        char line_end);
    
    char line_end;
  };
  
  /**
   * A serial port class for serial ports that operate like consoles.
   * 
   **/
  class SerialConsole : public SerialPort
  {
   public:

    /**
     * Constructor.
     */
    SerialConsole();

    /**
     * Destructor.
     *
     * Closes serial port if open.
     */
    ~SerialConsole();

    /**
     * Open and configure the serial port.
     *
     * The default configuration is:
     *   baud = 115200
     *   parity = NO_PARITY
     *   flow control = false
     *   data bits = 8
     *   stop bits = 1
     *   low latency mode = false
     *   line_end = \r (carriage return)
     *
     * @param[in]  device  The OS path of the device.
     * @param[in]  config  The port configuration settings.
     */
    bool Open(const std::string &device, SerialConsoleConfig config = SerialConsoleConfig());
    
    /**
     * Read a single line (including line end character) from the serial port
     * and appends it to a vector.
     *
     * Blocks until the timeout is reached or the line_end character is read.
     * If the timeout is reached before a line_end character is read, a partial
     * line may be returned.
     * 
     * Note that the vector will *not* be null-terminated
     * @param[out] line    The vector to append the line to
     * @param[in]  timeout    The maximum time to block in milliseconds
     * @param[in]  line_end_termination    If true, terminate a complete line
     *             with the line_end character. (Optional; Default: false)
     *
     * @returns Read result (SUCCESS, TIMEOUT, INTERRUPTED, or ERROR).
     * 
     **/
    SerialPort::Result ReadLine(std::vector<char>& line, int32_t timeout, bool line_end_termination);
    
    /**
     * Read a single line (excluding line end character) from the serial port
     * and store it in the string. Any existing contents of the string are
     * deleted.
     * 
     * Blocks until the timeout is reached or the line_end character is read.
     * If the timeout is reached before a line_end character is read, a partial
     * line may be returned.
     * @param[out] line The string to store the line in
     * @param[in]  timeout    The maximum time to block in milliseconds
     *
     * @returns Read result (SUCCESS, TIMEOUT, INTERRUPTED, or ERROR).
     * 
     **/
    SerialPort::Result ReadLine(std::string& line, int32_t timeout);
    
    /// The character that signals the end of a line
    char line_end_;
  };
}

#endif  // SERIAL_UTIL_SERIAL_CONSOLE_H_
