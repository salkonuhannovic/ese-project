using System;
using Newtonsoft.Json;

namespace ESE.SmartHome.SensorMock.Mocks
{
    public class Measurement
    {
        [JsonProperty("deviceid")]
        public long DeviceId { get; set; }

        [JsonProperty("timestamp")]
        public DateTime Timestamp { get; set; }

        [JsonProperty("temperature")]
        public double Temperature { get; set; }
        public TemperatureScale TemperatureScale { get; set; }

        [JsonProperty("humidity")]
        public double Humidity { get; set; }

        public override string ToString()
        {
            return $"Measurement (DeviceId = {DeviceId}, Timestamp = {Timestamp}, Temperature = {Temperature}, Humidity = {Humidity})";
        }
    }

    public enum TemperatureScale
    {
        Celsius,
        Fahrenheit,
        Kelvin
    }
}
