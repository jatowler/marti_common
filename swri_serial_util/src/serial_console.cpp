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

#include <swri_serial_util/serial_console.h>

#include <cerrno>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <termios.h>
#include <linux/serial.h>
#include <ctime>
#include <cstring>

namespace swri_serial_util
{
  SerialConsoleConfig::SerialConsoleConfig() :
      line_end('\r')
  {
  }

  SerialConsoleConfig::SerialConsoleConfig(
      int32_t baud,
      int32_t data_bits,
      int32_t stop_bits,
      Parity parity,
      bool flow_control,
      bool low_latency_mode,
      bool writable,
      char line_end) :
          SerialConfig(baud,
                       data_bits,
                       stop_bits,
                       parity,
                       flow_control,
                       low_latency_mode,
                       writable),
          line_end(line_end)
  {
  }
  
  bool SerialConsole::Open(const std::string &device, SerialConsoleConfig config)
  {
    line_end_ = config.line_end;
    return SerialPort::Open(device, config);
  }
  
  SerialPort::Result SerialConsole::ReadLine(std::vector<char>& line,
                                 int32_t timeout,
                                 bool line_end_termination = false)
  {
    time_t start_time = time(NULL);
    if (fd_ < 0)
    {
      error_msg_ = "Device not open.";
      return ERROR;
    }

    struct pollfd fds[1];
    fds[0].fd = fd_;
    fds[0].events = POLLIN;

    int poll_return = poll(fds, 1, timeout);
    if (poll_return == 0)
    {
      error_msg_ = "Timed out while waiting for data.";
      return TIMEOUT;
    }
    else if (poll_return < 0)
    {
      int error_num = errno;
      switch (error_num)
      {
        case EINTR:
          return INTERRUPTED;
        default:
          error_msg_ = "Error polling serial port: " + std::string(strerror(errno));
          return ERROR;
      }
    }

    char next_char = 0;
    int result = 0;
    while(true)
    {
        result = read(fd_, &next_char, 1);
        if (result > 0 && next_char != line_end_)
        {
          line.push_back(next_char);
        }
        else if (difftime(time(NULL), start_time)/1000 > timeout)
        {
          error_msg_ = "Timed out while waiting for data.";
          return INTERRUPTED;
        }
        else if (result == 0)
        {
          usleep(10000);  // Sleep for 10ms to wait for more data
        }
        else if (next_char == line_end_)
        {
          if (line_end_termination)
          {
            line.push_back(line_end_);
          }
          return SUCCESS;
        }
    }
    int error_num = errno;
    switch (error_num)
    {
      case EINTR:
        return INTERRUPTED;
        break;
      default:
        error_msg_ = "Error reading serial port: " + std::string(strerror(errno));
        return ERROR;
    }
  }

  SerialPort::Result SerialConsole::ReadLine(std::string& line, int32_t timeout)
  {
    std::vector<char> line_vec;
    Result result = ReadLine(line_vec, timeout, false);
    line.assign(line_vec.data(), line_vec.size());
    return result;
  }
}
