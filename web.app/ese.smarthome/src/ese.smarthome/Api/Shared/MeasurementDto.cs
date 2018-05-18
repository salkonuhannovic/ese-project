using System;

namespace ESE.SmartHome.Api
{
    public class MeasurementDto
    {
        public long DeviceId { get; set; }
        public DateTime Timestamp { get; set; }
        public double Temperature { get; set; }
        public double Humidity { get; set; }
    }
}