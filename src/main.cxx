#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <asio.hpp>

#include<whereami++.hpp>

#include "TelemetryData.pb.h"

using asio::ip::tcp;

std::string make_daytime_string()
{
  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);
  return ctime(&now);
}

namespace CSV
{
    std::ofstream file_csv;
    std::string packet;
    void writeFile(TelemetryData* data)
    {
        packet.append(std::to_string(data->id())).append(",");
        packet.append(std::to_string(data->packet_number())).append(",");
        packet.append(data->mission_time()).append(",");
        packet.append(std::to_string(data->pressure())).append(",");
        packet.append(std::to_string(data->altitude())).append(",");
        packet.append(std::to_string(data->descent_rate())).append(",");
        packet.append(std::to_string(data->temp())).append(",");
        packet.append(std::to_string(data->voltage_level())).append(",");
        packet.append(std::to_string(data->gps_latitude())).append(",");
        packet.append(std::to_string(data->gps_longtitude())).append(",");
        packet.append(std::to_string(data->gps_altitude())).append(",");
        packet.append(data->status()).append(",");
        packet.append(std::to_string(data->pitch())).append(",");
        packet.append(std::to_string(data->roll())).append(",");
        packet.append(std::to_string(data->yaw())).append(",");
        packet.append(std::to_string(data->spin_rate())).append("\n");
        file_csv.write(packet.c_str() , packet.length());
        file_csv.flush();
        packet.clear();
    }
}

int main(int argc ,  const char** argv)
{
  try
  {
    asio::io_context io_context;

    tcp::resolver resolver(io_context);
    tcp::resolver::results_type endpoints =
      resolver.resolve("", "2005");

    tcp::socket socket(io_context);
    asio::connect(socket, endpoints);

    std::ostringstream oss;

    std::srand(std::time(0));

    TelemetryData telemetry_data;
    size_t output_buffer_size = telemetry_data.ByteSizeLong() + 100;
    void* output_buffer = new char[output_buffer_size];
    CSV::file_csv.open(whereami::executable_dir().append("/output.csv") , std::ios_base::out);

    for (;;)
    {
      std::array<char, 128> buf;
      asio::error_code error;


      std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
      oss << std::put_time(std::localtime(&now), "%d/%m/%Y %H/%M/%S") ;
      std::string mission_time = oss.str();
      oss.seekp(0);
      
      telemetry_data.set_id(atoi(argv[0]));
      telemetry_data.set_packet_number(telemetry_data.packet_number() + 1);
      telemetry_data.set_mission_time(mission_time);
      telemetry_data.set_pressure(static_cast<double>(std::rand()));
      telemetry_data.set_altitude(static_cast<double>(std::rand()));
      telemetry_data.set_descent_rate(static_cast<double>(std::rand()));
      telemetry_data.set_temp(std::rand());
      telemetry_data.set_voltage_level(std::rand());
      telemetry_data.set_gps_latitude(static_cast<double>(std::rand()));
      telemetry_data.set_gps_longtitude(static_cast<double>(std::rand()));
      telemetry_data.set_gps_altitude(static_cast<double>(std::rand()));
      telemetry_data.set_status("ROCKING HARDDDD!");
      telemetry_data.set_pitch(static_cast<double>(std::rand()));
      telemetry_data.set_roll(static_cast<double>(std::rand()));
      telemetry_data.set_yaw(static_cast<double>(std::rand()));
      telemetry_data.set_spin_rate(std::rand());
      
      CSV::writeFile(&telemetry_data);
      std::cout << telemetry_data.ByteSizeLong() << "\n";
      telemetry_data.SerializeToArray(output_buffer , telemetry_data.ByteSizeLong());
      auto asio_buffer = asio::buffer(output_buffer , telemetry_data.ByteSizeLong());
      socket.write_some(asio_buffer);


      using namespace std::chrono_literals;
      std::this_thread::sleep_for(1s);
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}