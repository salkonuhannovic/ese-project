using System;
using Newtonsoft.Json;

namespace ESE.SmartHome.SensorMock.Mocks
{
    public class Measurement
    {
        public long DeviceId { get; set; }

        [JsonProperty("timestamp")]
        public DateTime Timestamp { get; set; }

        [JsonProperty("value")]
        public double Temperature { get; set; }
        public TemperatureScale TemperatureScale { get; set; }

        public double Humidity { get; set; }

        public override string ToString()
        {
            return $"Measurement (Timestamp = {Timestamp}, Value = {Temperature})";
        }
    }

    public enum TemperatureScale
    {
        Celsius,
        Fahrenheit,
        Kelvin
    }
}
